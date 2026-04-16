"""兼容层：历史上的通用工具函数已按领域拆分到独立模块。"""
from .command_services import (
    group_commands_by_tab as _group_commands_by_tab,
    validate_command_params_basic as _validate_command_params_basic,
)
from .permission_service import (
    assign_super_admin_permissions,
    get_super_admin_users_queryset,
    is_super_admin_user,
)
from .query_utils import parse_time_range_filters
from .security_utils import (
    get_client_ip,
    make_confirm_token,
    mask_sensitive_data as _mask_sensitive_data,
    verify_confirm_token,
)

__all__ = [
    'assign_super_admin_permissions',
    'get_client_ip',
    'get_super_admin_users_queryset',
    'is_super_admin_user',
    'make_confirm_token',
    'parse_time_range_filters',
    'verify_confirm_token',
    '_group_commands_by_tab',
    '_mask_sensitive_data',
    '_validate_command_params_basic',
]
