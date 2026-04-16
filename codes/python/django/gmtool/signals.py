"""信号处理器 — 自动补齐 UserProfile 与超级管理员权限"""
import logging

from django.contrib.auth import get_user_model
from django.db.models.signals import post_migrate, post_save
from django.dispatch import receiver

logger = logging.getLogger(__name__)

User = get_user_model()


@receiver(post_save, sender=User)
def ensure_user_profile_and_superuser_permissions(sender, instance, created, **kwargs):
    """
    当 Django User 保存时，确保其拥有 UserProfile；
    如果 is_superuser=True，则自动授予全部活跃命令权限。
    """
    try:
        from .models import UserProfile
        from .utils import assign_super_admin_permissions

        _, profile_created = UserProfile.objects.get_or_create(user=instance)
        new_perm_count = assign_super_admin_permissions(instance) if instance.is_superuser else 0

        if created or profile_created or new_perm_count:
            logger.info(
                '用户资料/超管权限自动补齐: user=%s, profile_created=%s, 新增用户权限=%d',
                instance.username,
                profile_created,
                new_perm_count,
            )

    except Exception as e:
        # 数据库未就绪时（如迁移阶段）忽略错误
        logger.debug('用户资料/超管权限自动补齐跳过: %s', e)


@receiver(post_migrate)
def auto_bind_existing_superusers(sender, **kwargs):
    """
    迁移完成后，自动为已有 Django 超级管理员补齐 UserProfile，
    并授予所有活跃命令的用户级权限。
    """
    if sender.name != 'gmtool':
        return

    try:
        from .models import UserProfile
        from .utils import assign_super_admin_permissions, get_super_admin_users_queryset

        total_new_profiles = 0
        total_new_perms = 0
        for user in get_super_admin_users_queryset():
            _, profile_created = UserProfile.objects.get_or_create(user=user)
            total_new_profiles += int(profile_created)
            total_new_perms += assign_super_admin_permissions(user)

        if total_new_profiles > 0 or total_new_perms > 0:
            logger.info(
                '迁移后超级管理员补全: 新增资料=%d, 新增权限=%d',
                total_new_profiles,
                total_new_perms,
            )

    except Exception as e:
        # 数据库表可能还未创建，安全忽略
        logger.debug('迁移后超级管理员自动补齐跳过: %s', e)
