"""API相关视图"""
import json
import logging
import os
import tempfile

from django.conf import settings
from django.contrib.auth.decorators import login_required
from django.http import JsonResponse
from django.shortcuts import get_object_or_404
from django.utils.translation import gettext_lazy as _
from django.views.decorators.http import require_POST

from .audit_log import log_operation
from .command_parser import load_commands_json_content, sync_commands_to_db, validate_json_command_ids
from .decorators import is_super_admin, super_admin_required
from .models import CommandLog
from .utils import get_client_ip, _mask_sensitive_data

logger = logging.getLogger(__name__)


@login_required
def log_detail_api(request, log_id):
    """日志详情 API，返回单条日志的 JSON 数据（含权限校验与脱敏）"""
    logs = CommandLog.objects.all()
    if not is_super_admin(request.user, request):
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


@login_required
@super_admin_required
@require_POST
def upload_commands_api(request):
    """上传idip_commands.json并自动同步命令定义"""

    # 校验文件
    uploaded_file = request.FILES.get('file')
    if not uploaded_file:
        return JsonResponse({'error': _('请选择文件')}, status=400)

    # 校验文件名
    if not uploaded_file.name.endswith('.json'):
        return JsonResponse({'error': _('仅支持 .json 文件')}, status=400)

    # 校验文件大小
    if uploaded_file.size > settings.UPLOAD_MAX_SIZE:
        max_size_mb = settings.UPLOAD_MAX_SIZE / (1024 * 1024)
        if max_size_mb.is_integer():
            max_size_display = str(int(max_size_mb))
        else:
            max_size_display = f'{max_size_mb:.2f}'.rstrip('0').rstrip('.')
        return JsonResponse(
            {'error': _('文件大小不能超过 %(size)sMB') % {'size': max_size_display}},
            status=400,
        )

    # 解析并校验 JSON 内容
    try:
        raw = uploaded_file.read()
        data = json.loads(raw)
    except json.JSONDecodeError as e:
        return JsonResponse({'error': _('JSON 解析失败: %(error)s') % {'error': str(e)}}, status=400)

    # 校验顶层结构：必须是 dict，且每个值包含必要字段
    if not isinstance(data, dict):
        return JsonResponse({'error': _('JSON 顶层必须是对象(dict)')}, status=400)

    required_keys = {'tab', 'request', 'request_id', 'responseid', 'respone'}
    for cmd_id, cmd_data in data.items():
        if not isinstance(cmd_data, dict):
            return JsonResponse({'error': _('命令 %(id)s 的值必须是对象') % {'id': cmd_id}}, status=400)
        missing = required_keys - set(cmd_data.keys())
        if missing:
            return JsonResponse({'error': _('命令 %(id)s 缺少必要字段: %(fields)s') % {'id': cmd_id, 'fields': ', '.join(missing)}}, status=400)
        # 校验 request_name 对应的参数列表存在
        request_name = cmd_data.get('request', '')
        if request_name and not isinstance(cmd_data.get(request_name), list):
            return JsonResponse({'error': _('命令 %(id)s 的请求参数 %(name)s 必须是数组') % {'name': request_name, 'id': cmd_id}}, status=400)

    # 校验 ID 重复（CommandId、request_id、response_id）
    is_valid_ids, id_error_msg = validate_json_command_ids(data)
    if not is_valid_ids:
        return JsonResponse({'error': _('检测到 ID 冲突: %(errors)s') % {'errors': id_error_msg}}, status=400)

    # 原子写入 idip_commands.json（先写临时文件再替换，防止中途崩溃损坏文件）
    try:
        json_path = getattr(settings, 'IDIP_JSON_PATH', settings.BASE_DIR / 'idip_commands.json')
        fd, tmp_path = tempfile.mkstemp(suffix='.json.tmp')
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
        return JsonResponse({'error': _('文件写入失败: %(error)s') % {'error': str(e)}}, status=500)

    # 自动执行同步
    try:
        created, updated, deactivated = sync_commands_to_db()
    except Exception as e:
        logger.exception('Command sync error: %s', e)
        return JsonResponse({'error': _('文件已上传但同步失败')}, status=500)

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
