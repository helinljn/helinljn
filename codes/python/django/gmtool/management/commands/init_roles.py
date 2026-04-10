"""管理命令：初始化超级管理员角色并为所有Django超级管理员绑定"""
from django.core.management.base import BaseCommand
from django.contrib.auth.models import User


class Command(BaseCommand):
    help = '初始化超级管理员角色、同步命令定义，并为所有Django超级管理员自动绑定角色和权限'

    def handle(self, *args, **options):
        from gmtool.models import Role, UserProfile, RoleCommandPermission, UserCommandPermission, GMCommand
        from gmtool.command_parser import sync_commands_to_db

        # 1. 同步命令定义
        created, updated, deactivated = sync_commands_to_db()
        self.stdout.write(f'命令同步: 新增={created}, 更新={updated}, 停用={deactivated}')

        # 2. 确保超级管理员角色存在
        role, role_created = Role.objects.get_or_create(
            name='super_admin',
            defaults={
                'display_name': '超级管理员',
                'description': '系统最高权限角色，拥有所有命令执行权限及管理功能',
                'is_super_admin': True,
            }
        )
        if role_created:
            self.stdout.write(self.style.SUCCESS('超级管理员角色创建成功'))
        else:
            self.stdout.write('超级管理员角色已存在')

        # 3. 为超级管理员角色分配所有活跃命令权限（兼容性保留）
        commands = GMCommand.objects.filter(is_active=True)
        existing_perm_ids = set(RoleCommandPermission.objects.filter(
            role=role
        ).values_list('command_id', flat=True))
        added = 0
        for cmd in commands:
            if cmd.id not in existing_perm_ids:
                RoleCommandPermission.objects.get_or_create(role=role, command=cmd)
                added += 1
        self.stdout.write(f'角色权限分配: 新增={added}, 共={commands.count()}个活跃命令')

        # 4. 为所有 Django 超级管理员自动绑定 GM 超级管理员角色
        bound_count = 0
        for user in User.objects.filter(is_superuser=True):
            profile, _ = UserProfile.objects.get_or_create(
                user=user,
                defaults={'role': role}
            )
            if profile.role != role:
                profile.role = role
                profile.save(update_fields=['role'])
            bound_count += 1
        self.stdout.write(self.style.SUCCESS(
            f'已为 {bound_count} 个Django超级管理员绑定GM超级管理员角色'
        ))

        # 5. 为所有超级管理员用户授予用户级权限
        user_perm_added = 0
        for user in User.objects.filter(is_superuser=True):
            existing_user_perm_ids = set(UserCommandPermission.objects.filter(
                user=user
            ).values_list('command_id', flat=True))
            for cmd in commands:
                if cmd.id not in existing_user_perm_ids:
                    UserCommandPermission.objects.get_or_create(user=user, command=cmd)
                    user_perm_added += 1
        self.stdout.write(f'用户权限分配: 新增={user_perm_added}')
