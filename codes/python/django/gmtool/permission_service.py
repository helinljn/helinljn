"""权限与超级管理员相关服务。"""
from django.contrib.auth import get_user_model



def is_super_admin_user(user):
    """统一判断用户是否为超级管理员（仅认 Django is_superuser）。"""
    return bool(user and user.is_authenticated and user.is_superuser)



def get_super_admin_users_queryset():
    """统一获取全部超级管理员用户（仅 Django is_superuser=True）。"""
    user_model = get_user_model()
    return user_model.objects.filter(is_superuser=True)



def assign_super_admin_permissions(user, new_commands=None):
    """
    兼容保留：超级管理员的命令权限直接由 Django `is_superuser` 决定，
    不再冗余写入 `UserCommandPermission`。
    """
    if not is_super_admin_user(user):
        return 0
    return 0



def assign_permissions_to_all_super_admins(new_commands):
    """兼容保留的批量接口：超级管理员不再需要额外权限写入。"""
    if not new_commands:
        return 0, 0
    return get_super_admin_users_queryset().count(), 0
