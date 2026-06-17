"""命令管理相关视图"""
import json
import logging

from django.conf import settings
from django.contrib import messages
from django.contrib.auth.decorators import login_required
from django.db import DatabaseError
from django.http import JsonResponse
from django.shortcuts import redirect, render
from django.views.decorators.http import require_POST
from django.utils.translation import gettext_lazy as _

from .audit_log import log_operation
from .command_parser import (
    add_command_to_json,
    load_commands_json_file,
    restore_commands_json_snapshot,
    sync_commands_to_db,
    validate_json_command_ids,
)
from .command_services import get_visible_commands_queryset, group_commands_by_tab, validate_command_params_basic
from .decorators import command_permission_required, is_super_admin, super_admin_required
from .forms import AddGMCommandForm
from .idip_client import send_idip_command
from .models import CommandLog, GMCommand
from .query_utils import apply_time_range_filters_to_queryset, paginate_request_queryset
from .security_utils import get_client_ip

logger = logging.getLogger(__name__)


_COMMAND_LOG_PERSISTENCE_WARNING = _('命令请求已完成，但执行结果记录失败，请联系管理员核查应用日志')


def _build_command_execute_response(response_data, error, *, log_persisted=True, warning=None):
    """根据远端执行结果和本地持久化结果构造响应。"""
    extra = {}
    if not log_persisted:
        extra['log_persisted'] = False
        if warning:
            extra['warning'] = warning

    if error:
        return JsonResponse({'success': False, 'error': error, **extra})

    if isinstance(response_data, dict) and 'status' in response_data and 'json' in response_data:
        payload = dict(response_data)
        payload.update(extra)
        return JsonResponse(payload)

    payload = {'success': True, 'data': response_data}
    payload.update(extra)
    return JsonResponse(payload)


@login_required
def dashboard(request):
    """仪表盘首页"""
    is_admin = is_super_admin(request.user, request)
    commands = get_visible_commands_queryset(
        request.user,
        is_admin=is_admin,
        only_fields=('id', 'command_id', 'command_name', 'tab'),
    )

    tab_groups = group_commands_by_tab(commands)
    # 利用已加载的数据计算命令数量，避免额外 COUNT 查询
    command_count = sum(len(cmds) for cmds in tab_groups.values())

    recent_logs = CommandLog.objects.filter(user=request.user).select_related('command')[:5]

    return render(request, 'gmtool/dashboard.html', {
        'tab_groups': tab_groups,
        'recent_logs': recent_logs,
        'is_admin': is_admin,
        'command_count': command_count,
    })


@login_required
def command_list(request):
    """可用命令列表"""
    is_admin = is_super_admin(request.user, request)
    commands = get_visible_commands_queryset(
        request.user,
        is_admin=is_admin,
        only_fields=('id', 'command_id', 'command_name', 'tab', 'request_name', 'request_id'),
    )

    # 搜索
    search = request.GET.get('search', '')
    if search:
        commands = commands.filter(command_name__icontains=search)

    tab_groups = group_commands_by_tab(commands)

    return render(request, 'gmtool/command_list.html', {
        'tab_groups': tab_groups,
        'search': search,
        'is_admin': is_admin,
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
                {'success': False, 'error': _('该命令已被停用或删除，请刷新页面获取最新命令列表'), 'command_deactivated': True},
                status=410,
            )
        messages.error(request, _('该命令已被停用或删除'))
        return redirect('gmtool:command_list')

    if request.method == 'POST':
        try:
            body = json.loads(request.body)
            if not isinstance(body, dict):
                return JsonResponse({'success': False, 'error': _('请求体必须是 JSON 对象')}, status=400)

            params = body.get('params', {})
            if not isinstance(params, dict):
                return JsonResponse({'success': False, 'error': _('参数 Params 必须是对象')}, status=400)

            # 基础 schema 校验（必填 + 基础类型）
            valid, schema_error = validate_command_params_basic(command, params)
            if not valid:
                return JsonResponse({'success': False, 'error': schema_error}, status=400)

            raw_partition = params.get('Partition', 0)
            try:
                partition = int(raw_partition)
            except (TypeError, ValueError):
                return JsonResponse({'success': False, 'error': _('Partition 必须为整数')}, status=400)

            if partition < 0:
                return JsonResponse({'success': False, 'error': _('Partition 不能为负数')}, status=400)

            # 归一化写回，确保后续下游与日志类型一致
            params['Partition'] = partition

            # 调用 IDIP API
            response_data, error, request_content, error_type = send_idip_command(command, params)

            # 包装协议语义失败（status=false）视为失败
            wrapped_failed = False
            if isinstance(response_data, dict) and 'status' in response_data and 'json' in response_data:
                wrapped_failed = not bool(response_data.get('status'))

            status = 'success'
            if error or wrapped_failed:
                status = 'timeout' if error_type == 'timeout' else 'failed'

            client_ip = get_client_ip(request)
            log_persisted = True
            persistence_warning = None

            try:
                CommandLog.objects.create(
                    user=request.user,
                    operator_username=request.user.username,
                    command=command,
                    partition=partition,
                    request_data=params,
                    request_content=request_content,
                    response_data=response_data,
                    status=status,
                    ip_address=client_ip,
                )
            except DatabaseError as e:
                log_persisted = False
                persistence_warning = _COMMAND_LOG_PERSISTENCE_WARNING
                logger.exception(
                    'Command execution log persistence failed after remote response: cmd=%s, user=%s, status=%s, error=%s',
                    cmd_id,
                    request.user.username,
                    status,
                    e,
                )

            # 审计日志
            log_operation('command', 'execute', user=request.user,
                          ip_address=client_ip,
                          detail={
                              'command_id': cmd_id,
                              'command_name': command.command_name,
                              'partition': partition,
                              'status': status,
                              'log_persisted': log_persisted,
                          })

            return _build_command_execute_response(
                response_data,
                error,
                log_persisted=log_persisted,
                warning=persistence_warning,
            )

        except json.JSONDecodeError:
            return JsonResponse({'success': False, 'error': _('请求数据格式错误')}, status=400)

    # GET: 显示执行表单

    # 直接使用数据库中同步存储的 field_labels
    # 无需每次请求读取 idip_commands.json，标签在命令同步时已一并入库
    response_field_labels = command.field_labels or {}

    return render(request, 'gmtool/command_execute.html', {
        'command': command,
        'request_params': command.request_params,
        'response_params': command.response_params,
        'request_params_json': json.dumps(command.request_params, ensure_ascii=False),
        'response_params_json': json.dumps(command.response_params, ensure_ascii=False),
        'response_field_labels_json': json.dumps(response_field_labels, ensure_ascii=False),
        'response_name': command.response_name,
        'is_admin': is_super_admin(request.user, request),
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

            try:
                request_params = json.loads(request_params_raw) if request_params_raw else default_request_params
                response_params = json.loads(response_params_raw) if response_params_raw else default_response_params
            except json.JSONDecodeError as e:
                form.add_error(None, _('参数 JSON 格式错误: %(error)s') % {'error': str(e)})
                return render(request, 'gmtool/command_add.html', {
                    'form': form,
                    'title': _('添加GM命令'),
                    'is_admin': True,
                })

            # 构建 JSON 格式的命令数据
            cmd_json_data = {
                "tab": tab,
                "command_name": command_name,
                "request_desc": request_desc or f"{command_name}请求({command_name} request)",
                "request_id": request_id,
                "request": request_name,
                "response_desc": response_desc or f"{command_name}应答({command_name} response)",
                response_name: response_params,
                "response_id": response_id,
                "response": response_name,
                request_name: request_params,
            }

            # 写入 idip_commands.json
            success, error_msg, previous_snapshot = add_command_to_json({
                'command_id': command_id,
                'data': cmd_json_data,
            })
            if not success:
                messages.error(request, _('写入命令到 JSON 文件失败: %(error)s') % {'error': error_msg})
            else:
                # 同步到数据库；若失败则回滚 JSON 文件，避免磁盘状态与数据库状态分叉
                try:
                    sync_commands_to_db()
                except (DatabaseError, OSError, ValueError, json.JSONDecodeError, UnicodeDecodeError) as e:
                    logger.exception('Command sync error after add, rolling back JSON file: %s', e)
                    try:
                        restore_commands_json_snapshot(previous_snapshot)
                    except OSError as rollback_error:
                        logger.exception('Failed to rollback JSON file after add command sync error: %s', rollback_error)
                        messages.error(request, _('命令同步失败，且 JSON 文件回滚失败，请立即人工核查'))
                    else:
                        messages.error(request, _('命令同步失败，JSON 文件已回滚，命令未生效'))
                    return render(request, 'gmtool/command_add.html', {
                        'form': form,
                        'title': _('添加GM命令'),
                        'is_admin': True,
                    })

                log_operation('command', 'add', user=request.user,
                              ip_address=get_client_ip(request),
                              detail={
                                  'command_id': command_id,
                                  'command_name': command_name,
                                  'request_id': request_id,
                                  'response_id': response_id,
                              })

                messages.success(request, _('命令 %(id)s 添加成功') % {'id': command_id})
                return redirect('gmtool:dashboard')
    else:
        form = AddGMCommandForm()

    return render(request, 'gmtool/command_add.html', {
        'form': form,
        'title': _('添加GM命令'),
        'is_admin': True,
    })


@login_required
@super_admin_required
@require_POST
def sync_commands_api(request):
    """从JSON同步命令定义"""

    # 先校验 JSON 文件中的 ID 是否有冲突
    json_path = getattr(settings, 'IDIP_JSON_PATH', settings.BASE_DIR / 'idip_commands.json')
    try:
        _, data = load_commands_json_file(json_path)
    except (json.JSONDecodeError, UnicodeDecodeError, FileNotFoundError, ValueError) as e:
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
    except (DatabaseError, OSError, ValueError, json.JSONDecodeError, UnicodeDecodeError) as e:
        logger.exception('Command sync error: %s', e)
        return JsonResponse({'error': str(_('服务器内部错误'))}, status=500)


@login_required
def command_log_list(request):
    """操作日志列表"""
    is_admin = is_super_admin(request.user, request)
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

    logs, start_time_filter, end_time_filter = apply_time_range_filters_to_queryset(logs, request)

    # 添加排序以避免分页警告（按创建时间降序，最新的在前）
    logs = logs.order_by('-created_at')

    page_obj, elided_page_range = paginate_request_queryset(logs, request, settings.PAGE_SIZE)

    # 普通用户只展示其日志中出现过的命令，避免暴露全量命令信息
    if is_admin:
        commands_qs = get_visible_commands_queryset(request.user, is_admin=True)
    else:
        command_ids = logs.exclude(command__isnull=True).values_list('command_id', flat=True).distinct()
        commands_qs = GMCommand.objects.filter(id__in=command_ids, is_active=True)

    return render(request, 'gmtool/command_log.html', {
        'page_obj': page_obj,
        'elided_page_range': elided_page_range,
        'status_filter': status_filter,
        'cmd_filter': cmd_filter,
        'user_filter': user_filter if is_admin else '',
        'start_time_filter': start_time_filter,
        'end_time_filter': end_time_filter,
        'commands': commands_qs,
        'is_admin': is_admin,
    })
