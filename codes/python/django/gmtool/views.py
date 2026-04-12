"""GM命令后台管理系统 - 视图函数"""
import json
import logging
import os
import tempfile
import time

from django.conf import settings
from django.contrib.auth import authenticate, login
from django.contrib.auth import logout as auth_logout
from django.contrib.auth.decorators import login_required
from django.contrib.auth.models import User
from django.core.paginator import Paginator
from django.db import transaction
from django.http import JsonResponse
from django.shortcuts import get_object_or_404, redirect, render
from django.views.decorators.http import require_POST
from django.utils.http import url_has_allowed_host_and_scheme
from django.utils.translation import gettext_lazy as _
from django.utils.translation import gettext as __

from .audit_log import log_operation
from .command_parser import add_command_to_json, sync_commands_to_db, validate_json_command_ids
from .decorators import command_permission_required, get_user_permissions, is_super_admin, super_admin_required
from .forms import AddGMCommandForm, RoleForm, UserCreateForm, UserEditForm
from .idip_client import send_idip_command
from .models import CommandLog, GMCommand, LoginLog, Role, UserCommandPermission, UserProfile

logger = logging.getLogger(__name__)

# 脱敏字段（日志详情接口返回前处理）
SENSITIVE_FIELD_NAMES = {
    'password', 'passwd', 'pwd', 'token', 'access_token', 'refresh_token',
    'secret', 'sign', 'signature', 'sessionid', 'cookie', 'authorization',
}

# 登录失败限速配置
LOGIN_MAX_ATTEMPTS = 5          # 最大尝试次数
LOGIN_LOCKOUT_SECONDS = 300     # 锁定时长（秒），5分钟
LOGIN_THROTTLE_CACHE_PREFIX = 'gmtool:login_throttle:'


def _get_safe_next_url(request):
    """获取安全的登录后跳转地址，仅允许本站内相对路径"""
    next_url = request.GET.get('next', '')
    if not next_url:
        return ''

    # 禁止 scheme-relative URL（如 //evil.com）
    if next_url.startswith('//'):
        return ''

    # 仅允许当前主机，且 HTTPS 请求下要求 https
    if not url_has_allowed_host_and_scheme(
        next_url,
        allowed_hosts={request.get_host()},
        require_https=request.is_secure(),
    ):
        return ''

    # 仅允许相对路径跳转，避免外部跳转风险
    if not next_url.startswith('/'):
        return ''

    return next_url


def _group_commands_by_tab(commands):
    """将命令查询集按 tab 字段分组"""
    tab_groups = {}
    for cmd in commands:
        tab = cmd.tab
        if tab not in tab_groups:
            tab_groups[tab] = []
        tab_groups[tab].append(cmd)
    return tab_groups


# ========== 认证相关 ==========

def login_view(request):
    """登录页面"""
    if request.user.is_authenticated:
        return render(request, 'gmtool/login.html', {
            'already_logged_in': True,
            'current_user': request.user,
        })
    if request.method == 'POST':
        ip = get_client_ip(request)

        # 登录失败限速检查
        from django.core.cache import cache
        cache_key = f'{LOGIN_THROTTLE_CACHE_PREFIX}{ip}'
        attempts = cache.get(cache_key, 0)
        if attempts >= LOGIN_MAX_ATTEMPTS:
            LoginLog.objects.create(
                user=None, username=request.POST.get('username', ''),
                action='login_failed', ip_address=ip,
                user_agent=request.META.get('HTTP_USER_AGENT', '')[:500],
                reason=_('尝试次数过多'),
            )
            return render(request, 'gmtool/login.html', {
                'error': _('登录尝试次数过多，请 %(minutes)d 分钟后再试') % {'minutes': LOGIN_LOCKOUT_SECONDS // 60},
            })

        username = request.POST.get('username', '')
        password = request.POST.get('password', '')
        user = authenticate(request, username=username, password=password)
        ua = request.META.get('HTTP_USER_AGENT', '')[:500]
        if user is not None:
            if user.is_active:
                login(request, user)
                # 登录成功，清除失败计数
                cache.delete(cache_key)
                LoginLog.objects.create(
                    user=user, username=username, action='login',
                    ip_address=ip, user_agent=ua,
                )
                log_operation('auth', 'login', user=user, ip_address=ip,
                              detail={'username': username})
                next_url = _get_safe_next_url(request)
                if next_url:
                    return redirect(next_url)
                return redirect('gmtool:dashboard')
            else:
                # 登录失败，累加计数
                cache.set(cache_key, attempts + 1, timeout=LOGIN_LOCKOUT_SECONDS)
                LoginLog.objects.create(
                    user=None, username=username, action='login_failed',
                    ip_address=ip, user_agent=ua, reason=_('账号已禁用'),
                )
                log_operation('auth', 'login_failed', ip_address=ip,
                              detail={'username': username, 'reason': 'Account disabled'})
                return render(request, 'gmtool/login.html', {'error': _('账号已被禁用')})
        else:
            # 登录失败，累加计数
            cache.set(cache_key, attempts + 1, timeout=LOGIN_LOCKOUT_SECONDS)
            remaining = LOGIN_MAX_ATTEMPTS - attempts - 1
            LoginLog.objects.create(
                user=None, username=username, action='login_failed',
                ip_address=ip, user_agent=ua, reason=_('用户名或密码错误'),
            )
            log_operation('auth', 'login_failed', ip_address=ip,
                          detail={'username': username, 'reason': 'Incorrect username or password'})
            if remaining > 0:
                error_msg = _('用户名或密码错误，还可尝试 %(remaining)d 次') % {'remaining': remaining}
            else:
                error_msg = _('登录尝试次数过多，请 %(minutes)d 分钟后再试') % {'minutes': LOGIN_LOCKOUT_SECONDS // 60}
            return render(request, 'gmtool/login.html', {'error': error_msg})
    return render(request, 'gmtool/login.html')


@login_required
@require_POST
def logout_view(request):
    """登出（仅允许POST，防止CSRF强制登出）"""
    log_operation('auth', 'logout', user=request.user, ip_address=get_client_ip(request),
                  detail={'username': request.user.username})
    LoginLog.objects.create(
        user=request.user, username=request.user.username, action='logout',
        ip_address=get_client_ip(request),
        user_agent=request.META.get('HTTP_USER_AGENT', '')[:500],
    )
    auth_logout(request)
    return redirect('gmtool:login')


# ========== 仪表盘 ==========

@login_required
def dashboard(request):
    """仪表盘首页"""
    user_perms = get_user_permissions(request.user)
    commands = GMCommand.objects.filter(is_active=True, command_id__in=user_perms)
    tab_groups = _group_commands_by_tab(commands)

    recent_logs = CommandLog.objects.filter(user=request.user).select_related('user', 'command')[:5]
    is_admin = is_super_admin(request.user)

    return render(request, 'gmtool/dashboard.html', {
        'tab_groups': tab_groups,
        'recent_logs': recent_logs,
        'is_admin': is_admin,
        'command_count': commands.count(),
    })


# ========== 命令管理 ==========

@login_required
def command_list(request):
    """可用命令列表"""
    user_perms = get_user_permissions(request.user)
    commands = GMCommand.objects.filter(is_active=True, command_id__in=user_perms)

    # 搜索
    search = request.GET.get('search', '')
    if search:
        commands = commands.filter(command_name__icontains=search)

    tab_groups = _group_commands_by_tab(commands)

    return render(request, 'gmtool/command_list.html', {
        'tab_groups': tab_groups,
        'search': search,
        'is_admin': is_super_admin(request.user),
    })


@login_required
@command_permission_required
def command_execute(request, cmd_id):
    """命令执行页面：GET显示动态表单，POST转发请求"""
    # 优先使用装饰器已查询的 command 对象，避免重复查询；超管直接通过时需自行查询
    command = getattr(request, '_gmcommand', None) or GMCommand.objects.filter(command_id=cmd_id).first()

    # 命令不存在或已停用
    if not command or not command.is_active:
        if request.method == 'POST':
            return JsonResponse(
                {'success': False, 'error': __('该命令已被停用或删除，请刷新页面获取最新命令列表'), 'command_deactivated': True},
                status=410,
            )
        from django.contrib import messages
        messages.error(request, _('该命令已被停用或删除'))
        return redirect('gmtool:command_list')

    if request.method == 'POST':
        try:
            body = json.loads(request.body)
            if not isinstance(body, dict):
                return JsonResponse({'success': False, 'error': __('请求体必须是 JSON 对象')}, status=400)

            params = body.get('params', {})
            if not isinstance(params, dict):
                return JsonResponse({'success': False, 'error': __('参数 Params 必须是对象')}, status=400)

            raw_partition = params.get('Partition', 0)
            try:
                partition = int(raw_partition)
            except (TypeError, ValueError):
                return JsonResponse({'success': False, 'error': __('Partition 必须为整数')}, status=400)

            if partition < 0:
                return JsonResponse({'success': False, 'error': __('Partition 不能为负数')}, status=400)

            # 归一化写回，确保后续下游与日志类型一致
            params['Partition'] = partition

            # 调用IDIP API
            response_data, error, request_content, error_type = send_idip_command(command, params)

            # 包装协议语义失败（status=false）视为失败
            wrapped_failed = False
            if isinstance(response_data, dict) and 'status' in response_data and 'json' in response_data:
                wrapped_failed = not bool(response_data.get('status'))

            # 记录日志
            status = 'success'
            if error or wrapped_failed:
                status = 'timeout' if error_type == 'timeout' else 'failed'

            client_ip = get_client_ip(request)

            CommandLog.objects.create(
                user=request.user,
                command=command,
                partition=partition,
                request_data=params,
                request_content=request_content,
                response_data=response_data,
                status=status,
                ip_address=client_ip,
            )

            # 审计日志
            log_operation('command', 'execute', user=request.user,
                          ip_address=client_ip,
                          detail={
                              'command_id': cmd_id,
                              'command_name': command.command_name,
                              'partition': partition,
                              'status': status,
                          })

            if error:
                return JsonResponse({'success': False, 'error': error})

            # 新包装协议：直接透传给前端
            if isinstance(response_data, dict) and 'status' in response_data and 'json' in response_data:
                return JsonResponse(response_data)

            # 旧协议：保持兼容
            return JsonResponse({'success': True, 'data': response_data})

        except json.JSONDecodeError:
            return JsonResponse({'success': False, 'error': __('请求数据格式错误')}, status=400)
        except Exception as e:
            logger.exception('Command execution error: %s', e)
            return JsonResponse({'success': False, 'error': __('服务器内部错误')}, status=500)

    # GET: 显示执行表单
    from django.conf import settings

    # 读取 idip_commands.json，构建字段 id -> name 的展示映射（用于结果表格列名）
    response_field_labels = {}
    try:
        json_path = settings.BASE_DIR / 'idip_commands.json'
        with open(json_path, 'r', encoding='utf-8') as f:
            command_json_map = json.load(f)
        cmd_json = command_json_map.get(command.command_id, {})
        if isinstance(cmd_json, dict):
            for _, val in cmd_json.items():
                if isinstance(val, list):
                    for item in val:
                        if isinstance(item, dict):
                            field_id = item.get('id')
                            field_name = item.get('name')
                            if field_id and field_name:
                                response_field_labels[field_id] = field_name
    except Exception:
        logger.warning('Load idip_commands.json field labels failed for command=%s', command.command_id)

    return render(request, 'gmtool/command_execute.html', {
        'command': command,
        'request_params': command.request_params,
        'response_params': command.response_params,
        'response_params_json': json.dumps(command.response_params, ensure_ascii=False),
        'response_field_labels_json': json.dumps(response_field_labels, ensure_ascii=False),
        'response_name': command.response_name,
        'is_admin': is_super_admin(request.user),
        'batch_max_targets': getattr(settings, 'BATCH_EXECUTE_MAX_TARGETS', 200),
        'batch_interval_ms': getattr(settings, 'BATCH_EXECUTE_INTERVAL_MS', 200),
    })


@login_required
@super_admin_required
def add_gm_command(request):
    """添加GM命令（仅超级管理员）"""
    if request.method == 'POST':
        form = AddGMCommandForm(request.POST)
        if form.is_valid():
            command_id = form.cleaned_data['command_id']
            tab = form.cleaned_data['tab']
            command_name = form.cleaned_data['command_name']
            request_name = form.cleaned_data['request_name']
            request_id = form.cleaned_data['request_id']
            response_name = form.cleaned_data['response_name']
            response_id = form.cleaned_data['response_id']
            request_desc = form.cleaned_data.get('request_desc', '')
            response_desc = form.cleaned_data.get('response_desc', '')

            # 解析参数
            default_request_params = [
                {"isnull": "false", "id": "AreaId", "type": "uint32", "name": "*渠道信息(Channel information)"},
                {"isnull": "false", "id": "Partition", "type": "uint32", "name": "服务器组号(Server group number)"},
                {"isnull": "false", "id": "PlatId", "type": "uint8", "name": "*平台信息(Platform information)"},
            ]
            default_response_params = [
                {"isnull": "false", "id": "Result", "type": "int32", "name": "结果(Result Id)"},
                {"isnull": "false", "id": "RetMsg", "type": "string", "name": "返回消息(Result message)"},
            ]

            request_params_raw = form.cleaned_data.get('request_params', '').strip()
            response_params_raw = form.cleaned_data.get('response_params', '').strip()

            request_params = json.loads(request_params_raw) if request_params_raw else default_request_params
            response_params = json.loads(response_params_raw) if response_params_raw else default_response_params

            # 构建 JSON 格式的命令数据
            cmd_json_data = {
                "tab": tab,
                "request_desc": request_desc or f"{command_name}请求({command_name} request)",
                "id": request_id,
                "request": request_name,
                "respone_desc": response_desc or f"{command_name}应答({command_name} response)",
                response_name: response_params,
                "responseid": response_id,
                "respone": response_name,
                request_name: request_params,
            }

            # 写入 idip_commands.json
            success, error_msg = add_command_to_json({
                'command_id': command_id,
                'data': cmd_json_data,
            })
            if not success:
                from django.contrib import messages
                messages.error(request, _('写入命令到 JSON 文件失败: %(error)s') % {'error': error_msg})
            else:
                # 同步到数据库
                try:
                    sync_commands_to_db()
                except Exception as e:
                    logger.exception('Command sync error after add: %s', e)
                    from django.contrib import messages
                    messages.warning(request, _('命令已添加到 JSON 文件，但数据库同步失败'))

                log_operation('command', 'add', user=request.user,
                              ip_address=get_client_ip(request),
                              detail={
                                  'command_id': command_id,
                                  'command_name': command_name,
                                  'request_id': request_id,
                                  'response_id': response_id,
                              })

                from django.contrib import messages
                messages.success(request, _('命令 %(id)s 添加成功') % {'id': command_id})
                return redirect('gmtool:dashboard')
    else:
        form = AddGMCommandForm()

    return render(request, 'gmtool/command_add.html', {
        'form': form,
        'title': _('添加GM命令'),
        'is_admin': True,
    })


# ========== 用户管理（super_admin） ==========

@login_required
@super_admin_required
def user_list(request):
    """用户列表管理"""
    from django.db.models import Count
    users = User.objects.select_related('userprofile', 'userprofile__role').annotate(
        perm_count=Count('usercommandpermission', distinct=True)
    ).all()

    # 搜索
    search = request.GET.get('search', '')
    if search:
        users = users.filter(username__icontains=search)

    paginator = Paginator(users, 20)
    page_number = request.GET.get('page', 1)
    page_obj = paginator.get_page(page_number)

    return render(request, 'gmtool/user_list.html', {
        'page_obj': page_obj,
        'search': search,
        'is_admin': True,
    })


@login_required
@super_admin_required
def user_create(request):
    """创建用户"""
    if request.method == 'POST':
        form = UserCreateForm(request.POST)
        if form.is_valid():
            new_user = form.save()
            log_operation('user', 'create', user=request.user,
                          ip_address=get_client_ip(request),
                          detail={
                              'target_user': new_user.username,
                              'is_active': new_user.is_active,
                              'role': str(form.cleaned_data.get('role')),
                          })
            return redirect('gmtool:user_list')
    else:
        form = UserCreateForm()

    return render(request, 'gmtool/user_form.html', {
        'form': form,
        'title': _('创建用户'),
        'is_admin': True,
    })


@login_required
@super_admin_required
def user_edit(request, user_id):
    """编辑用户"""
    user_obj = get_object_or_404(User, pk=user_id)
    # 确保UserProfile存在
    UserProfile.objects.get_or_create(user=user_obj, defaults={})

    is_self = user_obj == request.user
    is_target_super_admin = is_super_admin(user_obj)

    if request.method == 'POST':
        form = UserEditForm(request.POST, instance=user_obj, is_self=is_self,
                            is_target_super_admin=is_target_super_admin)
        if form.is_valid():
            # 禁止禁用自己的账号
            if is_self and not form.cleaned_data.get('is_active', True):
                form.add_error('is_active', _('不能禁用自己的账号'))
            else:
                form.save()
                log_operation('user', 'update', user=request.user,
                              ip_address=get_client_ip(request),
                              detail={
                                  'target_user': user_obj.username,
                                  'target_id': user_id,
                                  'is_active': form.cleaned_data.get('is_active'),
                                  'role': str(form.cleaned_data.get('role')),
                                  'password_changed': bool(form.cleaned_data.get('new_password')),
                              })
                return redirect('gmtool:user_list')
    else:
        form = UserEditForm(instance=user_obj, is_self=is_self,
                            is_target_super_admin=is_target_super_admin)

    return render(request, 'gmtool/user_form.html', {
        'form': form,
        'title': _('编辑用户'),
        'edit_user': user_obj,
        'is_self': is_self,
        'is_target_super_admin': is_target_super_admin,
        'is_admin': True,
    })


@login_required
@super_admin_required
@require_POST
def user_delete(request, user_id):
    """删除用户"""
    user_obj = get_object_or_404(User, pk=user_id)
    if user_obj == request.user:
        from django.contrib import messages
        messages.warning(request, __('不能删除自己的账号'))
    elif is_super_admin(user_obj):
        from django.contrib import messages
        messages.warning(request, __('不能删除超级管理员账号'))
    else:
        target_name = user_obj.username
        user_obj.delete()
        log_operation('user', 'delete', user=request.user,
                      ip_address=get_client_ip(request),
                      detail={'target_user': target_name, 'target_id': user_id})
    return redirect('gmtool:user_list')


# ========== 角色管理（super_admin） ==========

@login_required
@super_admin_required
def role_list(request):
    """角色列表管理"""
    from django.db.models import Count
    roles = Role.objects.annotate(
        user_count=Count('userprofile', distinct=True)
    ).all()
    return render(request, 'gmtool/role_list.html', {
        'roles': roles,
        'is_admin': True,
    })


@login_required
@super_admin_required
def role_create(request):
    """创建角色"""
    if request.method == 'POST':
        form = RoleForm(request.POST)
        if form.is_valid():
            role = form.save()
            log_operation('role', 'create', user=request.user,
                          ip_address=get_client_ip(request),
                          detail={
                              'role_name': role.name,
                              'display_name': role.display_name,
                              'is_super_admin': role.is_super_admin,
                          })
            return redirect('gmtool:role_list')
    else:
        form = RoleForm()

    return render(request, 'gmtool/role_form.html', {
        'form': form,
        'title': _('创建角色'),
        'is_admin': True,
    })


@login_required
@super_admin_required
def role_edit(request, role_id):
    """编辑角色"""
    role = get_object_or_404(Role, pk=role_id)
    # 禁止编辑超级管理员角色
    if role.is_super_admin:
        from django.contrib import messages
        messages.warning(request, __('超级管理员角色不可编辑'))
        return redirect('gmtool:role_list')
    if request.method == 'POST':
        form = RoleForm(request.POST, instance=role)
        if form.is_valid():
            form.save()
            log_operation('role', 'update', user=request.user,
                          ip_address=get_client_ip(request),
                          detail={
                              'role_id': role_id,
                              'role_name': form.cleaned_data.get('name'),
                              'display_name': form.cleaned_data.get('display_name'),
                          })
            return redirect('gmtool:role_list')
    else:
        form = RoleForm(instance=role)

    return render(request, 'gmtool/role_form.html', {
        'form': form,
        'title': _('编辑角色'),
        'edit_role': role,
        'is_admin': True,
    })


@login_required
@super_admin_required
def user_permissions(request, user_id):
    """用户权限分配（针对每个用户单独设置）"""
    user_obj = get_object_or_404(User, pk=user_id)
    # 超级管理员不需要手动分配权限
    if is_super_admin(user_obj):
        return redirect('gmtool:user_list')

    commands = GMCommand.objects.filter(is_active=True)

    if request.method == 'POST':
        selected_ids = request.POST.getlist('commands')
        # 记录变更前的权限
        old_perm_ids = set(UserCommandPermission.objects.filter(
            user=user_obj
        ).values_list('command_id', flat=True))

        # 校验：仅允许分配活跃命令的权限，忽略非法ID避免500
        active_command_ids = set(
            GMCommand.objects.filter(is_active=True).values_list('id', flat=True)
        )
        valid_ids = []
        invalid_ids = []
        for cid in selected_ids:
            try:
                cmd_pk = int(cid)
            except (TypeError, ValueError):
                invalid_ids.append(cid)
                continue
            if cmd_pk in active_command_ids:
                valid_ids.append(cmd_pk)

        if invalid_ids:
            logger.warning(
                'Invalid command IDs received in permission assignment: operator=%s, target_user_id=%s, invalid_ids=%s',
                request.user.username,
                user_id,
                invalid_ids,
            )

        # 差集模式更新权限（事务保护）：仅删除需移除的，仅创建需新增的
        with transaction.atomic():
            valid_ids_set = set(valid_ids)
            old_perm_ids_set = set(old_perm_ids)
            to_remove = old_perm_ids_set - valid_ids_set
            to_add = valid_ids_set - old_perm_ids_set

            if to_remove:
                UserCommandPermission.objects.filter(
                    user=user_obj, command_id__in=to_remove
                ).delete()
            if to_add:
                new_user_perms = [
                    UserCommandPermission(user=user_obj, command_id=cmd_id)
                    for cmd_id in to_add
                ]
                UserCommandPermission.objects.bulk_create(new_user_perms)

        new_perm_ids = set(valid_ids)
        log_operation('permission', 'assign', user=request.user,
                      ip_address=get_client_ip(request),
                      detail={
                          'target_user': user_obj.username,
                          'target_user_id': user_id,
                          'added': list(new_perm_ids - old_perm_ids),
                          'removed': list(old_perm_ids - new_perm_ids),
                          'total': len(new_perm_ids),
                      })
        return redirect('gmtool:user_list')

    # 获取当前用户的命令权限
    permitted_ids = set(UserCommandPermission.objects.filter(
        user=user_obj
    ).values_list('command_id', flat=True))

    # 按 tab 分组
    tab_groups = {}
    for cmd in commands:
        tab = cmd.tab
        if tab not in tab_groups:
            tab_groups[tab] = []
        tab_groups[tab].append({
            'id': cmd.id,
            'command_id': cmd.command_id,
            'command_name': cmd.command_name,
            'permitted': cmd.id in permitted_ids,
        })

    return render(request, 'gmtool/user_permissions.html', {
        'target_user': user_obj,
        'tab_groups': tab_groups,
        'is_admin': True,
    })


@login_required
@super_admin_required
@require_POST
def role_delete(request, role_id):
    """删除角色"""
    role = get_object_or_404(Role, pk=role_id)
    if not role.is_super_admin:
        role_name = role.name
        role.delete()
        log_operation('role', 'delete', user=request.user,
                      ip_address=get_client_ip(request),
                      detail={'role_id': role_id, 'role_name': role_name})
    return redirect('gmtool:role_list')


# ========== 操作日志 ==========

@login_required
@super_admin_required
def login_log_list(request):
    """登录日志列表"""
    logs = LoginLog.objects.select_related('user').all()

    # 筛选
    action_filter = request.GET.get('action', '')
    if action_filter:
        logs = logs.filter(action=action_filter)

    user_filter = request.GET.get('user', '')
    if user_filter:
        logs = logs.filter(username__icontains=user_filter)

    paginator = Paginator(logs, 20)
    page_number = request.GET.get('page', 1)
    page_obj = paginator.get_page(page_number)

    return render(request, 'gmtool/login_log.html', {
        'page_obj': page_obj,
        'action_filter': action_filter,
        'user_filter': user_filter,
        'is_admin': True,
    })

@login_required
def command_log_list(request):
    """操作日志列表"""
    is_admin = is_super_admin(request.user)
    logs = CommandLog.objects.select_related('user', 'command').all()

    # 非管理员仅可查看自己的日志
    if not is_admin:
        logs = logs.filter(user=request.user)

    # 筛选
    status_filter = request.GET.get('status', '')
    if status_filter:
        logs = logs.filter(status=status_filter)

    cmd_filter = request.GET.get('command', '')
    if cmd_filter:
        logs = logs.filter(command__command_id=cmd_filter)

    user_filter = request.GET.get('user', '')
    if user_filter and is_admin:
        logs = logs.filter(user__username__icontains=user_filter)

    paginator = Paginator(logs, 20)
    page_number = request.GET.get('page', 1)
    page_obj = paginator.get_page(page_number)

    # 普通用户只展示其日志中出现过的命令，避免暴露全量命令信息
    if is_admin:
        commands_qs = GMCommand.objects.filter(is_active=True)
    else:
        command_ids = logs.exclude(command__isnull=True).values_list('command_id', flat=True).distinct()
        commands_qs = GMCommand.objects.filter(id__in=command_ids, is_active=True)

    return render(request, 'gmtool/command_log.html', {
        'page_obj': page_obj,
        'status_filter': status_filter,
        'cmd_filter': cmd_filter,
        'user_filter': user_filter if is_admin else '',
        'commands': commands_qs,
        'is_admin': is_admin,
    })


def _mask_sensitive_data(data):
    """递归脱敏日志中的敏感字段"""
    if isinstance(data, dict):
        masked = {}
        for key, value in data.items():
            if str(key).lower() in SENSITIVE_FIELD_NAMES:
                masked[key] = '***'
            else:
                masked[key] = _mask_sensitive_data(value)
        return masked
    if isinstance(data, list):
        return [_mask_sensitive_data(item) for item in data]
    return data


@login_required
def log_detail_api(request, log_id):
    """日志详情 API，返回单条日志的 JSON 数据（含权限校验与脱敏）"""
    logs = CommandLog.objects.all()
    if not is_super_admin(request.user):
        logs = logs.filter(user=request.user)

    log = get_object_or_404(logs, pk=log_id)

    masked_request_data = _mask_sensitive_data(log.request_data) if log.request_data else ''
    masked_response_data = _mask_sensitive_data(log.response_data) if log.response_data else ''

    # request_content 为 JSON 字符串时进行结构化脱敏，失败则返回空串避免透传敏感原文
    masked_request_content = ''
    if log.request_content:
        try:
            parsed_content = json.loads(log.request_content)
            masked_request_content = json.dumps(_mask_sensitive_data(parsed_content), ensure_ascii=False)
        except (TypeError, ValueError, json.JSONDecodeError):
            masked_request_content = ''

    return JsonResponse({
        'request_content': masked_request_content,
        'request_data': json.dumps(masked_request_data, ensure_ascii=False) if masked_request_data else '',
        'response_data': json.dumps(masked_response_data, ensure_ascii=False) if masked_response_data else '',
    })


# ========== API ==========

@login_required
@super_admin_required
@require_POST
def upload_commands_api(request):
    """上传idip_commands.json并自动同步命令定义"""

    # 校验文件
    uploaded_file = request.FILES.get('file')
    if not uploaded_file:
        return JsonResponse({'error': __('请选择文件')}, status=400)

    # 校验文件名
    if not uploaded_file.name.endswith('.json'):
        return JsonResponse({'error': __('仅支持 .json 文件')}, status=400)

    # 校验文件大小（限制 5MB）
    if uploaded_file.size > 5 * 1024 * 1024:
        return JsonResponse({'error': __('文件大小不能超过 5MB')}, status=400)

    # 解析并校验 JSON 内容
    try:
        raw = uploaded_file.read()
        data = json.loads(raw)
    except json.JSONDecodeError as e:
        return JsonResponse({'error': __('JSON 解析失败: %(error)s') % {'error': str(e)}}, status=400)

    # 校验顶层结构：必须是 dict，且每个值包含必要字段
    if not isinstance(data, dict):
        return JsonResponse({'error': __('JSON 顶层必须是对象(dict)')}, status=400)

    required_keys = {'tab', 'request', 'id', 'responseid', 'respone'}
    for cmd_id, cmd_data in data.items():
        if not isinstance(cmd_data, dict):
            return JsonResponse({'error': __('命令 %(id)s 的值必须是对象') % {'id': cmd_id}}, status=400)
        missing = required_keys - set(cmd_data.keys())
        if missing:
            return JsonResponse({'error': __('命令 %(id)s 缺少必要字段: %(fields)s') % {'id': cmd_id, 'fields': ', '.join(missing)}}, status=400)
        # 校验 request_name 对应的参数列表存在
        request_name = cmd_data.get('request', '')
        if request_name and not isinstance(cmd_data.get(request_name), list):
            return JsonResponse({'error': __('命令 %(id)s 的请求参数 %(name)s 必须是数组') % {'name': request_name, 'id': cmd_id}}, status=400)

    # 校验 ID 重复（CommandId、request_id、response_id）
    is_valid_ids, id_error_msg = validate_json_command_ids(data)
    if not is_valid_ids:
        return JsonResponse({'error': _('检测到 ID 冲突: %(errors)s') % {'errors': id_error_msg}}, status=400)

    # 原子写入 idip_commands.json（先写临时文件再替换，防止中途崩溃损坏文件）
    try:
        json_path = settings.BASE_DIR / 'idip_commands.json'
        content_str = raw.decode('utf-8') if isinstance(raw, bytes) else raw
        fd, tmp_path = tempfile.mkstemp(dir=str(settings.BASE_DIR), suffix='.json.tmp')
        try:
            with os.fdopen(fd, 'w', encoding='utf-8') as f:
                f.write(content_str)
            os.replace(tmp_path, str(json_path))
        except Exception:
            # 清理临时文件
            if os.path.exists(tmp_path):
                os.unlink(tmp_path)
            raise
    except Exception as e:
        logger.exception('Failed to write JSON file: %s', e)
        return JsonResponse({'error': __('文件写入失败: %(error)s') % {'error': str(e)}}, status=500)

    # 自动执行同步
    try:
        created, updated, deactivated = sync_commands_to_db()
    except Exception as e:
        logger.exception('Command sync error: %s', e)
        return JsonResponse({'error': __('文件已上传但同步失败')}, status=500)

    log_operation('command', 'upload_and_sync', user=request.user,
                  ip_address=get_client_ip(request),
                  detail={
                      'filename': uploaded_file.name,
                      'created': created,
                      'updated': updated,
                      'deactivated': deactivated,
                  })

    return JsonResponse({
        'success': True,
        'created': created,
        'updated': updated,
        'deactivated': deactivated,
    })


@login_required
@super_admin_required
@require_POST
def sync_commands_api(request):
    """从JSON同步命令定义"""

    # 先校验 JSON 文件中的 ID 是否有冲突
    json_path = os.path.join(settings.BASE_DIR, 'idip_commands.json')
    try:
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except (json.JSONDecodeError, FileNotFoundError) as e:
        return JsonResponse({'error': _('读取 JSON 文件失败: %(error)s') % {'error': str(e)}}, status=400)

    is_valid_ids, id_error_msg = validate_json_command_ids(data)
    if not is_valid_ids:
        return JsonResponse({'error': _('检测到 ID 冲突: %(errors)s') % {'errors': id_error_msg}}, status=400)

    try:
        created, updated, deactivated = sync_commands_to_db()
        log_operation('command', 'sync', user=request.user,
                      ip_address=get_client_ip(request),
                      detail={
                          'created': created,
                          'updated': updated,
                          'deactivated': deactivated,
                      })
        return JsonResponse({
            'success': True,
            'created': created,
            'updated': updated,
            'deactivated': deactivated,
        })
    except Exception as e:
        logger.exception('Command sync error: %s', e)
        return JsonResponse({'error': str(e)}, status=500)


# ========== 辅助函数 ==========

def get_client_ip(request):
    """获取客户端IP地址

    仅在配置了可信反向代理（TRUSTED_PROXY=True）时才使用 X-Forwarded-For，
    否则直接使用 REMOTE_ADDR，防止客户端伪造 IP。
    """
    if getattr(settings, 'TRUSTED_PROXY', False):
        x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
        if x_forwarded_for:
            return x_forwarded_for.split(',')[0].strip()
    return request.META.get('REMOTE_ADDR', '0.0.0.0')


# ========== 自定义错误页面 ==========

def custom_404(request, exception=None):
    """404 页面未找到"""
    return render(request, 'gmtool/404.html', status=404)


def custom_500(request):
    """500 服务器错误"""
    return render(request, 'gmtool/500.html', status=500)


def custom_403(request, exception=None):
    """403 访问被拒绝"""
    return render(request, 'gmtool/403.html', status=403)


def csrf_failure(request, reason=""):
    """CSRF 验证失败处理"""
    from django.contrib import messages
    from django.urls import reverse
    if request.path == reverse('gmtool:login'):
        messages.error(request, __('页面已过期，请重新登录。'))
        return redirect('gmtool:login')
    return render(request, 'gmtool/403.html', status=403)
