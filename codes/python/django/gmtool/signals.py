"""信号处理器 — Django 超级管理员与 GM 超级管理员自动绑定"""
import logging

from django.contrib.auth import get_user_model
from django.db.models.signals import post_save, post_migrate
from django.dispatch import receiver
from django.utils.translation import gettext_lazy as _

logger = logging.getLogger(__name__)

User = get_user_model()


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
        from .models import UserProfile
        from .utils import assign_super_admin_permissions, ensure_super_admin_role

        # 确保超级管理员角色存在
        role = ensure_super_admin_role()
        role_created = False

        # 确保 UserProfile 存在并绑定超级管理员角色
        profile, profile_created = UserProfile.objects.get_or_create(
            user=instance,
            defaults={'role': role}
        )
        # 如果 profile 存在但没有角色，或角色不是超级管理员，则更新
        if profile.role != role:
            profile.role = role
            profile.save(update_fields=['role'])

        # 仅在首次创建时自动授予全部活跃命令权限，避免每次保存都触发查询
        new_perm_count = 0
        if created or profile_created:
            new_perm_count = assign_super_admin_permissions(instance)

        if created or role_created or profile_created or new_perm_count:
            logger.info(
                '超级管理员自动绑定: user=%s, 新增用户权限=%d',
                instance.username, new_perm_count,
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
        from .models import UserProfile
        from .utils import (
            assign_super_admin_permissions,
            ensure_super_admin_role,
            get_super_admin_users_queryset,
        )

        # 确保角色存在
        role = ensure_super_admin_role()

        # 为所有超级管理员绑定角色和权限
        total_new_perms = 0
        for user in get_super_admin_users_queryset():
            profile, _ = UserProfile.objects.get_or_create(
                user=user,
                defaults={'role': role}
            )
            if profile.role != role:
                profile.role = role
                profile.save(update_fields=['role'])

            total_new_perms += assign_super_admin_permissions(user)

        if total_new_perms > 0:
            logger.info('迁移后超级管理员权限补全: 新增权限=%d', total_new_perms)

    except Exception as e:
        # 数据库表可能还未创建，安全忽略
        logger.debug('迁移后超级管理员自动绑定跳过: %s', e)
