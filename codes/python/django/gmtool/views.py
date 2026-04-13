"""
GM命令后台管理系统 - 视图函数（拆分后的统一导出模块）

注意：此文件已拆分为多个模块，此处仅提供统一导出以保持向后兼容。
新代码应直接导入对应的子模块。
"""

from .auth_views import (
    login_view,
    logout_view,
    custom_404,
    custom_500,
    custom_403,
    csrf_failure,
)

from .command_views import (
    dashboard,
    command_list,
    command_execute,
    add_gm_command,
    sync_commands_api,
    command_log_list,
)

from .user_views import (
    user_list,
    user_create,
    user_edit,
    user_delete,
    role_list,
    role_create,
    role_edit,
    user_permissions,
    role_delete,
    login_log_list,
)

from .api_views import (
    log_detail_api,
    upload_commands_api,
)

# 向后兼容：导出所有视图函数
__all__ = [
    # 认证相关
    'login_view',
    'logout_view',
    'custom_404',
    'custom_500',
    'custom_403',
    'csrf_failure',

    # 命令管理
    'dashboard',
    'command_list',
    'command_execute',
    'add_gm_command',
    'sync_commands_api',
    'command_log_list',

    # 用户和角色管理
    'user_list',
    'user_create',
    'user_edit',
    'user_delete',
    'role_list',
    'role_create',
    'role_edit',
    'user_permissions',
    'role_delete',
    'login_log_list',

    # API
    'log_detail_api',
    'upload_commands_api',
]
