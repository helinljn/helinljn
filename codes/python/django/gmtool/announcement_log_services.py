"""公告远端操作日志辅助服务。"""
import logging

from django.db import DatabaseError, transaction
from django.utils.translation import gettext_lazy as _

from .models import AnnouncementLog

logger = logging.getLogger(__name__)

ANNOUNCEMENT_LOG_PERSISTENCE_WARNING = _('公告远端操作已完成，但本地公告日志记录失败，请联系管理员核查应用日志')


def announcement_status_from_error(error_message, error_type):
    """根据公告目录服调用错误返回公告日志状态。"""
    if not error_message:
        return 'success'
    if error_type == 'timeout':
        return 'timeout'
    return 'failed'


def write_announcement_log(
    user,
    ip_address,
    *,
    action,
    platform,
    channel,
    announcement_type,
    announcement_id,
    request_data,
    response_data,
    raw_response,
    status,
    error_message,
    operator_username=None,
):
    """写入公告远端发布/删除日志；写入失败时返回 False。"""
    username = operator_username if operator_username is not None else (getattr(user, 'username', '') or '')
    try:
        with transaction.atomic():
            AnnouncementLog.objects.create(
                user=user,
                operator_username=username,
                action=action,
                platform=platform,
                channel=channel,
                announcement_type=announcement_type,
                announcement_id=str(announcement_id or ''),
                request_data=request_data,
                response_data=response_data,
                raw_response=raw_response or '',
                status=status,
                error_message=error_message or '',
                ip_address=ip_address,
            )
    except DatabaseError as exc:
        logger.exception(
            'Announcement log persistence failed: action=%s user=%s status=%s error=%s',
            action,
            username,
            status,
            exc,
        )
        return False
    return True
