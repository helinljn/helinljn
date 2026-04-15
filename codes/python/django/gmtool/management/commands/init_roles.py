"""管理命令：初始化超级管理员角色并为所有超级管理员绑定角色和权限"""
from django.core.management.base import BaseCommand

from gmtool.command_parser import sync_commands_to_db
from gmtool.models import UserProfile
from gmtool.utils import (
    assign_super_admin_permissions,
    ensure_super_admin_role,
    get_super_admin_users_queryset,
)


class Command(BaseCommand):
    help = '初始化超级管理员角色、同步命令定义，并为所有超级管理员自动绑定角色和权限'

    def handle(self, *args, **options):
        # 1. 同步命令定义
        created, updated, deactivated = sync_commands_to_db()
        self.stdout.write(f'命令同步: 新增={created}, 更新={updated}, 停用={deactivated}')

        # 2. 确保超级管理员角色存在
        role = ensure_super_admin_role()
        self.stdout.write('超级管理员角色已确认')

        # 3. 为所有超级管理员自动绑定 GM 超级管理员角色
        # 统一超管范围：
        # - Django 内置超级管理员 is_superuser=True
        # - GM 业务超级管理员 userprofile.role.is_super_admin=True
        bound_count = 0
        for user in get_super_admin_users_queryset():
            profile, _ = UserProfile.objects.get_or_create(
                user=user,
                defaults={'role': role}
            )
            if profile.role != role:
                profile.role = role
                profile.save(update_fields=['role'])
            bound_count += 1

        self.stdout.write(self.style.SUCCESS(
            f'已为 {bound_count} 个超级管理员确认 GM 超级管理员角色'
        ))

        # 4. 为所有超级管理员用户授予用户级权限
        total_new_perms = 0
        for user in get_super_admin_users_queryset():
            total_new_perms += assign_super_admin_permissions(user)

        self.stdout.write(f'用户权限分配: 新增={total_new_perms}')
