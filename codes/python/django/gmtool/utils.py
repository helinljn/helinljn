"""公共辅助函数"""
import json
import logging

from django.utils.translation import gettext_lazy as _

logger = logging.getLogger(__name__)

# 脱敏字段（日志详情接口返回前处理）
SENSITIVE_FIELD_NAMES = {
    'password', 'passwd', 'pwd', 'token', 'access_token', 'refresh_token',
    'secret', 'sign', 'signature', 'sessionid', 'cookie', 'authorization',
}


def get_client_ip(request):
    """获取客户端IP地址

    仅在配置了可信反向代理（TRUSTED_PROXY=True）时才使用 X-Forwarded-For，
    否则直接使用 REMOTE_ADDR，防止客户端伪造 IP。
    """
    from django.conf import settings
    if getattr(settings, 'TRUSTED_PROXY', False):
        x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
        if x_forwarded_for:
            return x_forwarded_for.split(',')[0].strip()
    return request.META.get('REMOTE_ADDR', '0.0.0.0')


def _group_commands_by_tab(commands):
    """将命令查询集按 tab 字段分组"""
    tab_groups = {}
    for cmd in commands:
        tab = cmd.tab
        if tab not in tab_groups:
            tab_groups[tab] = []
        tab_groups[tab].append(cmd)
    return tab_groups


def _validate_command_params_basic(command, params):
    """
    基于 command.request_params 做基础参数校验：
    - 必填字段校验（isnull=false）
    - 基础类型校验（int/uint/string/bool）
    """
    schema = command.request_params or []
    if not isinstance(schema, list):
        return False, _('命令参数定义错误，请联系管理员')

    for field in schema:
        if not isinstance(field, dict):
            continue

        field_id = field.get('id')
        if not field_id:
            continue

        raw_type = str(field.get('type', '')).strip().lower()
        is_required = str(field.get('isnull', 'false')).strip().lower() == 'false'
        value = params.get(field_id)

        if is_required and (value is None or value == ''):
            return False, _('缺少必填参数: %(field)s') % {'field': field_id}

        if value is None or value == '':
            continue

        # uint/int 系列
        if raw_type.startswith('uint') or raw_type.startswith('int'):
            try:
                int_value = int(value)
            except (TypeError, ValueError):
                return False, _('参数 %(field)s 必须为整数') % {'field': field_id}
            if raw_type.startswith('uint') and int_value < 0:
                return False, _('参数 %(field)s 不能为负数') % {'field': field_id}
            params[field_id] = int_value
            continue

        # string 系列
        if raw_type.startswith('string'):
            if not isinstance(value, str):
                params[field_id] = str(value)
            continue

        # bool 系列
        if raw_type.startswith('bool'):
            if isinstance(value, bool):
                continue
            if isinstance(value, str):
                lowered = value.strip().lower()
                if lowered in ('true', '1', 'yes', 'on'):
                    params[field_id] = True
                    continue
                if lowered in ('false', '0', 'no', 'off'):
                    params[field_id] = False
                    continue
            return False, _('参数 %(field)s 必须为布尔值') % {'field': field_id}

    return True, ''


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
