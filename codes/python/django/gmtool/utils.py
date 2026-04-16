"""公共辅助函数"""
import json
import logging

from django.conf import settings
from django.utils.translation import gettext_lazy as _

logger = logging.getLogger(__name__)

# 脱敏字段（日志详情接口返回前处理）— 从 settings 获取，便于扩展
SENSITIVE_FIELD_NAMES = getattr(settings, 'SENSITIVE_FIELDS', {
    'password', 'passwd', 'pwd', 'token', 'access_token', 'refresh_token',
    'secret', 'sign', 'signature', 'sessionid', 'cookie', 'authorization',
})


def is_super_admin_user(user):
    """统一判断用户是否为超级管理员（仅认 Django is_superuser）。"""
    return bool(user and user.is_authenticated and user.is_superuser)


def get_super_admin_users_queryset():
    """统一获取全部超级管理员用户（仅 Django is_superuser=True）。"""
    from django.contrib.auth import get_user_model

    User = get_user_model()
    return User.objects.filter(is_superuser=True)


def get_client_ip(request):
    """获取客户端IP地址

    仅在配置了可信反向代理（TRUSTED_PROXY=True）时才使用 X-Forwarded-For，
    否则直接使用 REMOTE_ADDR，防止客户端伪造 IP。

    当 TRUSTED_PROXY=True 时，支持 TRUSTED_PROXY_COUNT 配置：
    - 默认从右侧取第一个（最靠近应用服务器的代理添加的客户端IP）
    - 如果有多层代理，可设置 TRUSTED_PROXY_COUNT 跳过指定数量的代理IP
    """
    if getattr(settings, 'TRUSTED_PROXY', False):
        x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
        if x_forwarded_for:
            ips = [ip.strip() for ip in x_forwarded_for.split(',')]
            proxy_count = getattr(settings, 'TRUSTED_PROXY_COUNT', 1)
            # 从右侧取：跳过 proxy_count 个代理 IP
            idx = len(ips) - proxy_count
            if idx >= 0:
                return ips[idx]
            return ips[0]  # 代理数量多于 IP 数量时取最左侧
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

    注意：此函数会原地修改 params dict（如将字符串"123"转为整数123），
    调用者应知晓此副作用。如需保留原始 params，请传入副本。
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


def parse_time_range_filters(request, default_days=7):
    """
    从请求参数中解析时间范围筛选条件。

    通用逻辑：如果未指定任何时间范围，默认筛选最近 default_days 天。
    返回: (start_time_filter, end_time_filter, qs) 其中 qs 是已应用时间过滤的 QuerySet
    用法: start_filter, end_filter, qs = parse_time_range_filters(request, qs, 'created_at')
    """
    from django.utils import timezone
    from datetime import timedelta

    start_time_filter = request.GET.get('start_time', '')
    end_time_filter = request.GET.get('end_time', '')

    if not start_time_filter and not end_time_filter:
        default_start = timezone.now() - timedelta(days=default_days)
        start_time_filter = default_start.strftime('%Y-%m-%dT%H:%M')
        return start_time_filter, end_time_filter, default_start, None

    start_dt = None
    end_dt = None

    if start_time_filter:
        try:
            start_dt = timezone.make_aware(
                timezone.datetime.strptime(start_time_filter, '%Y-%m-%dT%H:%M')
            )
        except (ValueError, TypeError):
            start_time_filter = ''
            start_dt = None

    if end_time_filter:
        try:
            end_dt = timezone.make_aware(
                timezone.datetime.strptime(end_time_filter, '%Y-%m-%dT%H:%M')
            )
        except (ValueError, TypeError):
            end_time_filter = ''
            end_dt = None

    return start_time_filter, end_time_filter, start_dt, end_dt


def assign_super_admin_permissions(user, new_commands=None):
    """
    为超级管理员用户授予命令权限（共享逻辑）。

    如果指定 new_commands，只为该用户授予这些新命令的权限；
    否则授予所有活跃命令的权限（用于首次绑定或迁移后补全）。

    Args:
        user: Django User 对象
        new_commands: 可选，GMCommand QuerySet 或列表。为 None 时授予全部活跃命令。

    Returns:
        int: 新增权限数量
    """
    from .models import GMCommand, UserCommandPermission

    if not is_super_admin_user(user):
        return 0

    if new_commands is None:
        new_commands = GMCommand.objects.filter(is_active=True)

    existing_perm_ids = set(UserCommandPermission.objects.filter(
        user=user
    ).values_list('command_id', flat=True))

    new_user_perms = []
    for cmd in new_commands:
        if cmd.id not in existing_perm_ids:
            new_user_perms.append(UserCommandPermission(user=user, command=cmd))

    if new_user_perms:
        UserCommandPermission.objects.bulk_create(new_user_perms)

    return len(new_user_perms)


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
