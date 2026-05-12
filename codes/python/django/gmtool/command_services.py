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


def _is_empty_value(value):
    return value is None or value == ''


def _normalize_scalar_param(field_id, raw_type, value):
    """Validate and normalize a primitive parameter value."""
    if raw_type.startswith('uint') or raw_type.startswith('int'):
        try:
            int_value = int(value)
        except (TypeError, ValueError):
            return False, _('参数 %(field)s 必须为整数') % {'field': field_id}
        if raw_type.startswith('uint') and int_value < 0:
            return False, _('参数 %(field)s 不能为负数') % {'field': field_id}
        return True, int_value

    if raw_type == 'time':
        return _parse_time_param_value(field_id, value)

    if raw_type.startswith('string'):
        if not isinstance(value, str):
            return True, str(value)
        return True, value

    if raw_type.startswith('bool'):
        if isinstance(value, bool):
            return True, value
        if isinstance(value, str):
            lowered = value.strip().lower()
            if lowered in ('true', '1', 'yes', 'on'):
                return True, True
            if lowered in ('false', '0', 'no', 'off'):
                return True, False
        return False, _('参数 %(field)s 必须为布尔值') % {'field': field_id}

    return True, value


def _normalize_schema_object(schema, values, field_prefix=''):
    """Validate and normalize one object against a schema list."""
    if not isinstance(values, dict):
        return False, _('参数 %(field)s 必须为对象') % {'field': field_prefix.rstrip('.') or 'object'}

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
        value = values.get(field_id)
        display_field = f'{field_prefix}{field_id}'

        if is_required and _is_empty_value(value):
            return False, _('缺少必填参数: %(field)s') % {'field': display_field}

        if _is_empty_value(value):
            continue

        children = field.get('children')
        if isinstance(children, list):
            if not isinstance(value, dict):
                return False, _('参数 %(field)s 必须为对象') % {'field': display_field}
            success, normalized = _normalize_schema_object(children, value, f'{display_field}.')
            if not success:
                return False, normalized
            values[field_id] = normalized
            continue

        success, normalized = _normalize_scalar_param(display_field, raw_type, value)
        if not success:
            return False, normalized
        values[field_id] = normalized

    return True, values


def _normalize_struct_list_param(field, params):
    """Validate a structured list parameter and keep its *_count in sync."""
    field_id = field.get('id')
    children = field.get('children')
    value = params.get(field_id)
    is_required = str(field.get('isnull', 'false')).strip().lower() == 'false'
    raw_max_size = field.get('max_size')

    try:
        max_size = int(raw_max_size)
    except (TypeError, ValueError):
        return False, _('请求结构体列表字段 %(field)s 缺少有效的 max_size') % {'field': field_id}

    if max_size <= 0:
        return False, _('请求结构体列表字段 %(field)s 缺少有效的 max_size') % {'field': field_id}

    if is_required and (_is_empty_value(value) or value == []):
        return False, _('缺少必填参数: %(field)s') % {'field': field_id}

    if _is_empty_value(value):
        return True, ''

    if not isinstance(value, list):
        return False, _('参数 %(field)s 必须为数组') % {'field': field_id}

    if len(value) > max_size:
        return False, _('参数 %(field)s 数量不能超过 %(max_size)s') % {
            'field': field_id,
            'max_size': max_size,
        }

    normalized_items = []
    for index, item in enumerate(value, start=1):
        success, normalized = _normalize_schema_object(children, item, f'{field_id}[{index}].')
        if not success:
            return False, normalized
        normalized_items.append(normalized)

    params[field_id] = normalized_items

    count_id = field.get('count_id')
    if count_id:
        raw_count = params.get(count_id, len(normalized_items))
        try:
            count_value = int(raw_count)
        except (TypeError, ValueError):
            return False, _('参数 %(field)s 必须为整数') % {'field': count_id}
        if count_value < 0:
            return False, _('参数 %(field)s 不能为负数') % {'field': count_id}
        if count_value != len(normalized_items):
            return False, _('参数 %(field)s 必须等于 %(target)s 的数量') % {
                'field': count_id,
                'target': field_id,
            }
        params[count_id] = count_value

    return True, ''



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

        if field.get('auto_count_for') and field_id not in params:
            continue

        children = field.get('children')
        if isinstance(children, list):
            success, error = _normalize_struct_list_param(field, params)
            if not success:
                return False, error
            continue

        if is_required and _is_empty_value(value):
            return False, _('缺少必填参数: %(field)s') % {'field': field_id}

        if _is_empty_value(value):
            continue

        success, normalized = _normalize_scalar_param(field_id, raw_type, value)
        if not success:
            return False, normalized
        params[field_id] = normalized

    return True, ''
