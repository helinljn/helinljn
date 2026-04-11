"""GM命令后台管理系统 - 视图函数"""
import json
import logging
import os
import tempfile

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

from .audit_log import log_operation
from .command_parser import sync_commands_to_db
from .decorators import command_permission_required, get_user_permissions, is_super_admin, super_admin_required
from .forms import RoleForm, UserCreateForm, UserEditForm
from .idip_client import send_idip_command
from .models import CommandLog, GMCommand, LoginLog, Role, UserCommandPermission, UserProfile

logger = logging.getLogger(__name__)


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
        # 已登录用户访问登录页，显示提示而非静默跳转
        # 避免多标签页场景下 CSRF 失败重定向回登录页后自动跳转，看起来像绕过了密码验证
        return render(request, 'gmtool/login.html', {
            'already_logged_in': True,
            'current_user': request.user,
        })
    if request.method == 'POST':
        username = request.POST.get('username', '')
        password = request.POST.get('password', '')
        user = authenticate(request, username=username, password=password)
        ip = get_client_ip(request)
        ua = request.META.get('HTTP_USER_AGENT', '')[:500]
        if user is not None:
            if user.is_active:
                login(request, user)
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
                LoginLog.objects.create(
                    user=None, username=username, action='login_failed',
                    ip_address=ip, user_agent=ua, reason='账号已禁用',
                )
                log_operation('auth', 'login_failed', ip_address=ip,
                              detail={'username': username, 'reason': '账号已禁用'})
                return render(request, 'gmtool/login.html', {'error': '账号已被禁用'})
        else:
            LoginLog.objects.create(
                user=None, username=username, action='login_failed',
                ip_address=ip, user_agent=ua, reason='用户名或密码错误',
            )
            log_operation('auth', 'login_failed', ip_address=ip,
                          detail={'username': username, 'reason': '用户名或密码错误'})
            return render(request, 'gmtool/login.html', {'error': '用户名或密码错误'})
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
    command = GMCommand.objects.filter(command_id=cmd_id).first()

    # 命令不存在或已停用
    if not command or not command.is_active:
        if request.method == 'POST':
            return JsonResponse(
                {'success': False, 'error': '该命令已被停用或删除，请刷新页面获取最新命令列表', 'command_deactivated': True},
                status=410,
            )
        from django.contrib import messages
        messages.error(request, '该命令已被停用或删除')
        return redirect('gmtool:command_list')

    if request.method == 'POST':
        try:
            body = json.loads(request.body)
            if not isinstance(body, dict):
                return JsonResponse({'success': False, 'error': '请求体必须为JSON对象'}, status=400)

            params = body.get('params', {})
            if not isinstance(params, dict):
                return JsonResponse({'success': False, 'error': 'params必须为对象'}, status=400)

            raw_partition = params.get('Partition', 0)
            try:
                partition = int(raw_partition)
            except (TypeError, ValueError):
                return JsonResponse({'success': False, 'error': 'Partition必须为整数'}, status=400)

            if partition < 0:
                return JsonResponse({'success': False, 'error': 'Partition不能为负数'}, status=400)

            # 归一化写回，确保后续下游与日志类型一致
            params['Partition'] = partition

            # 调用IDIP API
            response_data, error, request_content = send_idip_command(command, params)

            # 记录日志
            status = 'success'
            if error:
                status = 'timeout' if '超时' in error else 'failed'

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

            return JsonResponse({'success': True, 'data': response_data})

        except json.JSONDecodeError:
            return JsonResponse({'success': False, 'error': '请求数据格式错误'}, status=400)
        except Exception as e:
            logger.exception('命令执行异常: %s', e)
            return JsonResponse({'success': False, 'error': '服务器内部错误'}, status=500)

    # GET: 显示执行表单
    return render(request, 'gmtool/command_execute.html', {
        'command': command,
        'request_params': command.request_params,
        'is_admin': is_super_admin(request.user),
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
        'title': '创建用户',
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

    if request.method == 'POST':
        form = UserEditForm(request.POST, instance=user_obj, is_self=is_self)
        if form.is_valid():
            # 禁止禁用自己的账号
            if is_self and not form.cleaned_data.get('is_active', True):
                form.add_error('is_active', '不能禁用自己的账号')
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
        form = UserEditForm(instance=user_obj, is_self=is_self)

    return render(request, 'gmtool/user_form.html', {
        'form': form,
        'title': '编辑用户',
        'edit_user': user_obj,
        'is_self': is_self,
        'is_admin': True,
    })


@login_required
@super_admin_required
@require_POST
def user_delete(request, user_id):
    """删除用户"""
    user_obj = get_object_or_404(User, pk=user_id)
    if user_obj != request.user:
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
    roles = Role.objects.all()
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
        'title': '创建角色',
        'is_admin': True,
    })


@login_required
@super_admin_required
def role_edit(request, role_id):
    """编辑角色"""
    role = get_object_or_404(Role, pk=role_id)
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
        'title': '编辑角色',
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
                '用户权限分配收到非法命令ID: operator=%s, target_user_id=%s, invalid_ids=%s',
                request.user.username,
                user_id,
                invalid_ids,
            )

        # 清除旧权限，写入新权限（事务保护）
        with transaction.atomic():
            UserCommandPermission.objects.filter(user=user_obj).delete()
            new_user_perms = [
                UserCommandPermission(user=user_obj, command_id=cmd_id)
                for cmd_id in valid_ids
            ]
            if new_user_perms:
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
    logs = LoginLog.objects.all()

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
    logs = CommandLog.objects.select_related('user', 'command').all()

    # 筛选
    status_filter = request.GET.get('status', '')
    if status_filter:
        logs = logs.filter(status=status_filter)

    cmd_filter = request.GET.get('command', '')
    if cmd_filter:
        logs = logs.filter(command__command_id=cmd_filter)

    user_filter = request.GET.get('user', '')
    if user_filter:
        logs = logs.filter(user__username__icontains=user_filter)

    paginator = Paginator(logs, 20)
    page_number = request.GET.get('page', 1)
    page_obj = paginator.get_page(page_number)

    return render(request, 'gmtool/command_log.html', {
        'page_obj': page_obj,
        'status_filter': status_filter,
        'cmd_filter': cmd_filter,
        'user_filter': user_filter,
        'commands': GMCommand.objects.filter(is_active=True),
        'is_admin': is_super_admin(request.user),
    })


@login_required
def log_detail_api(request, log_id):
    """日志详情 API，返回单条日志的 JSON 数据"""
    log = get_object_or_404(CommandLog, pk=log_id)
    return JsonResponse({
        'request_content': log.request_content or '',
        'request_data': json.dumps(log.request_data, ensure_ascii=False) if log.request_data else '',
        'response_data': json.dumps(log.response_data, ensure_ascii=False) if log.response_data else '',
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
        return JsonResponse({'error': '请选择文件'}, status=400)

    # 校验文件名
    if not uploaded_file.name.endswith('.json'):
        return JsonResponse({'error': '仅支持 .json 文件'}, status=400)

    # 校验文件大小（限制 5MB）
    if uploaded_file.size > 5 * 1024 * 1024:
        return JsonResponse({'error': '文件大小不能超过 5MB'}, status=400)

    # 解析并校验 JSON 内容
    try:
        raw = uploaded_file.read()
        data = json.loads(raw)
    except json.JSONDecodeError as e:
        return JsonResponse({'error': f'JSON 解析失败: {str(e)}'}, status=400)

    # 校验顶层结构：必须是 dict，且每个值包含必要字段
    if not isinstance(data, dict):
        return JsonResponse({'error': 'JSON 顶层必须是对象(dict)'}, status=400)

    required_keys = {'tab', 'request', 'id', 'responseid', 'respone'}
    for cmd_id, cmd_data in data.items():
        if not isinstance(cmd_data, dict):
            return JsonResponse({'error': f'命令 {cmd_id} 的值必须是对象'}, status=400)
        missing = required_keys - set(cmd_data.keys())
        if missing:
            return JsonResponse({'error': f'命令 {cmd_id} 缺少必要字段: {", ".join(missing)}'}, status=400)
        # 校验 request_name 对应的参数列表存在
        request_name = cmd_data.get('request', '')
        if request_name and not isinstance(cmd_data.get(request_name), list):
            return JsonResponse({'error': f'命令 {cmd_id} 的请求参数 {request_name} 必须是数组'}, status=400)

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
        logger.exception('写入JSON文件失败: %s', e)
        return JsonResponse({'error': f'文件写入失败: {str(e)}'}, status=500)

    # 自动执行同步
    try:
        created, updated, deactivated = sync_commands_to_db()
    except Exception as e:
        logger.exception('同步命令异常: %s', e)
        return JsonResponse({'error': '文件已上传但同步失败'}, status=500)

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
        logger.exception('同步命令异常: %s', e)
        return JsonResponse({'error': str(e)}, status=500)


# ========== 辅助函数 ==========

def get_client_ip(request):
    """获取客户端IP地址"""
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
    """CSRF 验证失败处理

    典型场景：两个标签页共享同一个 session，其中一个登录后 Django 轮转了
    CSRF token，另一个标签页提交时 token 失效。

    处理方式：
    - 登录页 CSRF 失败 → 统一重定向回登录页并提示"页面已过期"，
      不调用 auth_logout（避免销毁共享 session 影响其他标签页），
      也不因 request.user.is_authenticated 而放行（避免绕过密码验证）。
      重定向后 login_view 的 GET 分支会根据 session 状态决定跳转。
    - 其他请求 → 返回 403 页面
    """
    if request.path == '/gmtool/login/':
        from django.contrib import messages
        messages.error(request, '页面已过期，请重新登录。')
        return redirect('gmtool:login')
    return render(request, 'gmtool/403.html', status=403)
