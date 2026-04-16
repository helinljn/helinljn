"""命令查询与参数处理服务。"""
from datetime import datetime

from django.utils import timezone
from django.utils.translation import gettext_lazy as _

from .models import GMCommand



def get_visible_commands_queryset(user, *, is_admin=False, only_fields=None):
    """获取当前用户可见的活跃命令查询集。"""
    commands = GMCommand.objects.filter(is_active=True)
    if not is_admin:
        commands = commands.filter(usercommandpermission__user=user).distinct()
    if only_fields:
        commands = commands.only(*only_fields)
    return commands



def group_commands_by_tab(commands):
    """将命令查询集按 tab 字段分组。"""
    tab_groups = {}
    for cmd in commands:
        tab_groups.setdefault(cmd.tab, []).append(cmd)
    return tab_groups



def _parse_time_param_value(field_id, value):
    """将 time 类型参数归一化为 Unix 时间戳（秒）。"""
    if isinstance(value, bool):
        return False, _('参数 %(field)s 必须为时间戳或 ISO 时间') % {'field': field_id}

    if isinstance(value, (int, float)):
        timestamp = int(value)
        if timestamp < 0:
            return False, _('参数 %(field)s 不能为负数') % {'field': field_id}
        return True, timestamp

    if isinstance(value, str):
        stripped = value.strip()
        if not stripped:
            return False, _('参数 %(field)s 必须为时间戳或 ISO 时间') % {'field': field_id}

        if stripped.isdigit() or (stripped.startswith('-') and stripped[1:].isdigit()):
            timestamp = int(stripped)
            if timestamp < 0:
                return False, _('参数 %(field)s 不能为负数') % {'field': field_id}
            return True, timestamp

        normalized = stripped.replace('Z', '+00:00')
        try:
            if 'T' in normalized or '+' in normalized:
                parsed_dt = datetime.fromisoformat(normalized)
            else:
                parsed_dt = datetime.strptime(normalized, '%Y-%m-%d %H:%M:%S')
        except ValueError:
            return False, _('参数 %(field)s 必须为时间戳或 ISO 时间') % {'field': field_id}

        if timezone.is_naive(parsed_dt):
            parsed_dt = timezone.make_aware(parsed_dt, timezone.get_current_timezone())
        return True, int(parsed_dt.timestamp())

    return False, _('参数 %(field)s 必须为时间戳或 ISO 时间') % {'field': field_id}



def validate_command_params_basic(command, params):
    """
    基于 command.request_params 做基础参数校验。

    注意：此函数会原地修改 params dict。
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

        if raw_type.startswith('uint') or raw_type.startswith('int'):
            try:
                int_value = int(value)
            except (TypeError, ValueError):
                return False, _('参数 %(field)s 必须为整数') % {'field': field_id}
            if raw_type.startswith('uint') and int_value < 0:
                return False, _('参数 %(field)s 不能为负数') % {'field': field_id}
            params[field_id] = int_value
            continue

        if raw_type == 'time':
            success, parsed_value = _parse_time_param_value(field_id, value)
            if not success:
                return False, parsed_value
            params[field_id] = parsed_value
            continue

        if raw_type.startswith('string'):
            if not isinstance(value, str):
                params[field_id] = str(value)
            continue

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
