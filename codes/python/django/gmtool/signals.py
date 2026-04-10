"""信号处理器 — Django 超级管理员与 GM 超级管理员自动绑定"""
import logging

from django.db.models.signals import post_save, post_migrate
from django.dispatch import receiver

from django.contrib.auth.models import User

logger = logging.getLogger(__name__)


@receiver(post_save, sender=User)
def auto_assign_super_admin_role(sender, instance, created, **kwargs):
    """
    当 Django User 保存时，如果 is_superuser=True，
    自动确保其拥有 super_admin 角色的 UserProfile，
    并自动授予所有活跃命令的用户级权限。
    """
    if not instance.is_superuser:
        return

    try:
        from .models import Role, UserProfile, UserCommandPermission, GMCommand

        # 确保超级管理员角色存在
        role, role_created = Role.objects.get_or_create(
            name='super_admin',
            defaults={
                'display_name': '超级管理员',
                'description': '系统最高权限角色，拥有所有命令执行权限及管理功能',
                'is_super_admin': True,
            }
        )

        # 确保 UserProfile 存在并绑定超级管理员角色
        profile, profile_created = UserProfile.objects.get_or_create(
            user=instance,
            defaults={'role': role}
        )
        # 如果 profile 存在但没有角色，或角色不是超级管理员，则更新
        if profile.role != role:
            profile.role = role
            profile.save(update_fields=['role'])

        # 自动赋予所有活跃命令的用户级权限
        existing_perm_ids = set(UserCommandPermission.objects.filter(
            user=instance
        ).values_list('command_id', flat=True))
        new_user_perms = []
        for cmd in GMCommand.objects.filter(is_active=True).exclude(id__in=existing_perm_ids):
            new_user_perms.append(UserCommandPermission(user=instance, command=cmd))
        if new_user_perms:
            UserCommandPermission.objects.bulk_create(new_user_perms)

        if created or role_created or profile_created or new_user_perms:
            logger.info(
                '超级管理员自动绑定: user=%s, 新增用户权限=%d',
                instance.username, len(new_user_perms),
            )

    except Exception as e:
        # 数据库未就绪时（如迁移阶段）忽略错误
        logger.debug('超级管理员自动绑定跳过: %s', e)


@receiver(post_migrate)
def auto_bind_existing_superusers(sender, **kwargs):
    """
    迁移完成后，自动为所有已有的 Django 超级管理员绑定 GM 超级管理员角色，
    并授予所有活跃命令的用户级权限。
    """
    if sender.name != 'gmtool':
        return

    try:
        from .models import Role, UserProfile, UserCommandPermission, GMCommand

        # 确保角色存在
        Role.objects.get_or_create(
            name='super_admin',
            defaults={
                'display_name': '超级管理员',
                'description': '系统最高权限角色，拥有所有命令执行权限及管理功能',
                'is_super_admin': True,
            }
        )

        # 为所有 Django 超级管理员绑定角色和权限
        role = Role.objects.get(name='super_admin')
        for user in User.objects.filter(is_superuser=True):
            profile, _ = UserProfile.objects.get_or_create(
                user=user,
                defaults={'role': role}
            )
            if profile.role != role:
                profile.role = role
                profile.save(update_fields=['role'])

            # 授予用户级权限
            existing_perm_ids = set(UserCommandPermission.objects.filter(
                user=user
            ).values_list('command_id', flat=True))
            new_perms = []
            for cmd in GMCommand.objects.filter(is_active=True).exclude(id__in=existing_perm_ids):
                new_perms.append(UserCommandPermission(user=user, command=cmd))
            if new_perms:
                UserCommandPermission.objects.bulk_create(new_perms)

    except Exception:
        # 数据库表可能还未创建，安全忽略
        pass
