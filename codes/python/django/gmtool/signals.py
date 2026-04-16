"""信号处理器 — 自动补齐 UserProfile。"""
import logging

from django.contrib.auth import get_user_model
from django.db import DatabaseError
from django.db.models.signals import post_migrate, post_save
from django.dispatch import receiver

logger = logging.getLogger(__name__)

User = get_user_model()


@receiver(post_save, sender=User)
def ensure_user_profile_exists(sender, instance, created, **kwargs):
    """当 Django User 保存时，确保其拥有 `UserProfile`。"""
    try:
        from .models import UserProfile

        _, profile_created = UserProfile.objects.get_or_create(user=instance)

        if created or profile_created:
            logger.info(
                '用户资料自动补齐: user=%s, profile_created=%s',
                instance.username,
                profile_created,
            )

    except DatabaseError as e:
        # 数据库未就绪时（如迁移阶段）忽略错误
        logger.debug('用户资料自动补齐跳过: %s', e)


@receiver(post_migrate)
def ensure_existing_user_profiles(sender, **kwargs):
    """迁移完成后，为已有用户补齐缺失的 `UserProfile`。"""
    if sender.name != 'gmtool':
        return

    try:
        from .models import UserProfile

        total_new_profiles = 0
        for user in User.objects.all().only('id', 'username'):
            _, profile_created = UserProfile.objects.get_or_create(user=user)
            total_new_profiles += int(profile_created)

        if total_new_profiles > 0:
            logger.info('迁移后用户资料补全: 新增资料=%d', total_new_profiles)

    except DatabaseError as e:
        # 数据库表可能还未创建，安全忽略
        logger.debug('迁移后用户资料自动补齐跳过: %s', e)
