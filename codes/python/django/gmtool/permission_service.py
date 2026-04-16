"""权限与超级管理员相关服务。"""
from django.contrib.auth import get_user_model

from .models import GMCommand, UserCommandPermission



def is_super_admin_user(user):
    """统一判断用户是否为超级管理员（仅认 Django is_superuser）。"""
    return bool(user and user.is_authenticated and user.is_superuser)



def get_super_admin_users_queryset():
    """统一获取全部超级管理员用户（仅 Django is_superuser=True）。"""
    user_model = get_user_model()
    return user_model.objects.filter(is_superuser=True)



def assign_super_admin_permissions(user, new_commands=None):
    """为超级管理员授予命令权限。"""
    if not is_super_admin_user(user):
        return 0

    if new_commands is None:
        new_commands = GMCommand.objects.filter(is_active=True)

    existing_perm_ids = set(
        UserCommandPermission.objects.filter(user=user).values_list('command_id', flat=True)
    )

    new_user_perms = [
        UserCommandPermission(user=user, command=cmd)
        for cmd in new_commands
        if cmd.id not in existing_perm_ids
    ]

    if new_user_perms:
        UserCommandPermission.objects.bulk_create(new_user_perms)

    return len(new_user_perms)



def assign_permissions_to_all_super_admins(new_commands):
    """将一批新命令授权给所有 Django 超级管理员。"""
    if not new_commands:
        return 0, 0

    assigned_user_count = 0
    created_permission_count = 0
    for user in get_super_admin_users_queryset():
        created_permission_count += assign_super_admin_permissions(user, new_commands=new_commands)
        assigned_user_count += 1
    return assigned_user_count, created_permission_count
