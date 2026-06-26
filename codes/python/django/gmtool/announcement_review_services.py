"""公告审核业务服务。"""
from dataclasses import dataclass

from django.core.exceptions import ObjectDoesNotExist
from django.db import DatabaseError, IntegrityError, transaction
from django.db.models import Q
from django.utils import timezone
from django.utils.translation import gettext_lazy as _

from .announcement_client import create_announcement, delete_announcement, query_announcements
from .announcement_log_services import announcement_status_from_error, write_announcement_log
from .audit_log import log_operation
from .models import AnnouncementReview


class ReviewActionError(Exception):
    """审核操作业务错误。"""


class ReviewSelfActionError(ReviewActionError):
    """当前用户操作自己提交内容。"""


@dataclass
class ReviewActionResult:
    review: AnnouncementReview
    succeeded: bool
    error_message: str = ''
    remote_call_count: int = 0
    log_persisted: bool = True


def _append_failure(failures, channel, error_message):
    failures.append({
        'channel': channel,
        'error': str(error_message),
    })


def _review_defaults_from_payload(form, payload, channel):
    return {
        'platform': payload['Platform'],
        'channel': channel,
        'announcement_type': payload['AnnouncementType'],
        'announcement_id': str(payload.get('AnnouncementId') or '-1'),
        'title': payload.get('Title', '') or '',
        'content': payload.get('Content', '') or '',
        'priority': form.cleaned_data.get('Priority') or 0,
        'image_1': payload.get('Image_1', '') or '',
        'image_link_1': payload.get('ImageLink_1', '') or '',
        'image_2': payload.get('Image_2', '') or '',
        'image_link_2': payload.get('ImageLink_2', '') or '',
        'image_3': payload.get('Image_3', '') or '',
        'image_link_3': payload.get('ImageLink_3', '') or '',
        'reserve_1': payload.get('Reserve_1', '') or '',
        'reserve_2': payload.get('Reserve_2', '') or '',
        'payload': payload,
    }


def submit_announcement_reviews(form, user, ip_address):
    """按渠道创建公告待审核记录，不调用目录服。"""
    platform = form.cleaned_data['Platform']
    channels = form.cleaned_data['Channel']
    announcement_type = form.cleaned_data['AnnouncementType']
    success_channels = []
    failed_channels = []

    for channel in channels:
        payload = form.to_payload(channel)
        try:
            with transaction.atomic():
                if (
                    announcement_type == '1'
                    and AnnouncementReview.objects.filter(
                        platform=platform,
                        channel=channel,
                        announcement_type='1',
                        status='pending',
                    ).exists()
                ):
                    _append_failure(failed_channels, channel, _('该平台和渠道已有待审核周更新公告'))
                    continue

                AnnouncementReview.objects.create(
                    submitter=user,
                    submitter_username=getattr(user, 'username', '') or '',
                    **_review_defaults_from_payload(form, payload, channel),
                )
        except IntegrityError:
            _append_failure(failed_channels, channel, _('该平台和渠道已有待审核周更新公告'))
            continue
        except DatabaseError as exc:
            _append_failure(failed_channels, channel, _('待审核记录保存失败: %(error)s') % {'error': str(exc)})
            continue

        success_channels.append(channel)

    overall_status = 'success' if success_channels and not failed_channels else 'failed'
    log_operation(
        'announcement_review',
        'submit',
        user=user,
        ip_address=ip_address,
        detail={
            'platform': platform,
            'channels': channels,
            'announcement_type': announcement_type,
            'success_channels': success_channels,
            'failed_channels': failed_channels,
            'status': overall_status,
        },
    )
    return {
        'success_channels': success_channels,
        'failed_channels': failed_channels,
    }


def _ensure_can_operate(review, user):
    if review.submitter_id and user and review.submitter_id == user.id:
        raise ReviewSelfActionError(_('不能审核自己提交的内容'))


def _has_newer_weekly_review(review):
    if review.announcement_type != '1':
        return False
    return AnnouncementReview.objects.filter(
        platform=review.platform,
        channel=review.channel,
        announcement_type='1',
    ).exclude(
        pk=review.pk,
    ).exclude(
        status='rejected',
    ).filter(
        Q(created_at__gt=review.created_at) | Q(created_at=review.created_at, pk__gt=review.pk)
    ).exists()


def _get_review_for_update(review_id):
    try:
        return AnnouncementReview.objects.select_for_update().get(pk=review_id)
    except ObjectDoesNotExist as exc:
        raise ReviewActionError(_('待审核公告不存在')) from exc


def _extract_announcement_id(response_data, fallback):
    if isinstance(response_data, dict):
        for key in ('AnnouncementId', 'announcement_id', 'id'):
            value = response_data.get(key)
            if value not in (None, ''):
                return str(value)
    return fallback


def _update_review_after_publish(
    review,
    user,
    *,
    status,
    response_data,
    raw_response,
    error_message,
):
    review.status = status
    review.reviewer = user
    review.reviewer_username = getattr(user, 'username', '') or ''
    review.reviewed_at = timezone.now()
    review.remote_response = response_data
    review.raw_response = raw_response or ''
    review.error_message = error_message or ''
    if status == 'approved':
        review.announcement_id = _extract_announcement_id(response_data, review.announcement_id)
    review.save(update_fields=[
        'status',
        'reviewer',
        'reviewer_username',
        'reviewed_at',
        'remote_response',
        'raw_response',
        'error_message',
        'announcement_id',
        'updated_at',
    ])


def _mark_review_processing(review, user):
    review.status = 'processing'
    review.reviewer = user
    review.reviewer_username = getattr(user, 'username', '') or ''
    review.reviewed_at = timezone.now()
    review.error_message = ''
    review.save(update_fields=[
        'status',
        'reviewer',
        'reviewer_username',
        'reviewed_at',
        'error_message',
        'updated_at',
    ])


def _publish_review(review, user, ip_address):
    remote_call_count = 0
    log_persisted = True
    announcement_log_user = review.submitter
    announcement_log_username = review.submitter_username or getattr(review.submitter, 'username', '') or ''

    if review.announcement_type == '1':
        old_announcements, query_error, query_raw, _query_error_type = query_announcements(
            review.platform,
            review.channel,
        )
        remote_call_count += 1
        if query_error:
            _update_review_after_publish(
                review,
                user,
                status='failed',
                response_data=None,
                raw_response=query_raw,
                error_message=query_error,
            )
            return ReviewActionResult(
                review=review,
                succeeded=False,
                error_message=query_error,
                remote_call_count=remote_call_count,
                log_persisted=log_persisted,
            )

        for old_announcement in old_announcements or []:
            old_type = str(old_announcement.get('AnnouncementType') or '')
            if not old_type:
                error_message = _('旧公告缺少公告类型，已停止发布以避免误删')
                _update_review_after_publish(
                    review,
                    user,
                    status='failed',
                    response_data=None,
                    raw_response=query_raw,
                    error_message=error_message,
                )
                return ReviewActionResult(
                    review=review,
                    succeeded=False,
                    error_message=str(error_message),
                    remote_call_count=remote_call_count,
                    log_persisted=log_persisted,
                )
            if old_type != review.announcement_type:
                continue

            old_announcement_id = old_announcement.get('AnnouncementId')
            if not old_announcement_id or str(old_announcement_id) == '-1':
                error_message = _('旧周更新公告缺少有效公告 ID')
                _update_review_after_publish(
                    review,
                    user,
                    status='failed',
                    response_data=None,
                    raw_response=query_raw,
                    error_message=error_message,
                )
                return ReviewActionResult(
                    review=review,
                    succeeded=False,
                    error_message=str(error_message),
                    remote_call_count=remote_call_count,
                    log_persisted=log_persisted,
                )

            old_channel = old_announcement.get('Channel') or review.channel
            delete_response, delete_error, delete_raw, delete_error_type = delete_announcement(
                review.platform,
                old_channel,
                old_type,
                old_announcement_id,
            )
            remote_call_count += 1
            delete_status = announcement_status_from_error(delete_error, delete_error_type)
            item_log_persisted = write_announcement_log(
                announcement_log_user,
                ip_address,
                action='delete',
                platform=review.platform,
                channel=old_channel,
                announcement_type=old_type,
                announcement_id=old_announcement_id,
                request_data={
                    'Platform': review.platform,
                    'Channel': old_channel,
                    'AnnouncementType': old_type,
                    'AnnouncementId': str(old_announcement_id),
                },
                response_data=delete_response,
                raw_response=delete_raw,
                status=delete_status,
                error_message=delete_error,
                operator_username=announcement_log_username,
            )
            log_persisted = log_persisted and item_log_persisted
            if delete_error:
                _update_review_after_publish(
                    review,
                    user,
                    status='failed',
                    response_data=delete_response,
                    raw_response=delete_raw,
                    error_message=delete_error,
                )
                return ReviewActionResult(
                    review=review,
                    succeeded=False,
                    error_message=delete_error,
                    remote_call_count=remote_call_count,
                    log_persisted=log_persisted,
                )

    payload = dict(review.payload or {})
    response_data, error_message, raw_response, error_type = create_announcement(payload)
    remote_call_count += 1
    create_status = announcement_status_from_error(error_message, error_type)
    item_log_persisted = write_announcement_log(
        announcement_log_user,
        ip_address,
        action='create',
        platform=review.platform,
        channel=review.channel,
        announcement_type=review.announcement_type,
        announcement_id=payload.get('AnnouncementId') or review.announcement_id,
        request_data=payload,
        response_data=response_data,
        raw_response=raw_response,
        status=create_status,
        error_message=error_message,
        operator_username=announcement_log_username,
    )
    log_persisted = log_persisted and item_log_persisted

    if error_message:
        _update_review_after_publish(
            review,
            user,
            status='failed',
            response_data=response_data,
            raw_response=raw_response,
            error_message=error_message,
        )
        return ReviewActionResult(
            review=review,
            succeeded=False,
            error_message=error_message,
            remote_call_count=remote_call_count,
            log_persisted=log_persisted,
        )

    _update_review_after_publish(
        review,
        user,
        status='approved',
        response_data=response_data,
        raw_response=raw_response,
        error_message='',
    )
    return ReviewActionResult(
        review=review,
        succeeded=True,
        remote_call_count=remote_call_count,
        log_persisted=log_persisted,
    )


def approve_announcement_review(review_id, user, ip_address):
    """审核通过 pending 公告，并发布到目录服。"""
    with transaction.atomic():
        review = _get_review_for_update(review_id)
        if review.status != 'pending':
            raise ReviewActionError(_('只有待审核公告可以通过'))
        _ensure_can_operate(review, user)
        _mark_review_processing(review, user)

    result = _publish_review(review, user, ip_address)

    log_operation(
        'announcement_review',
        'approve',
        user=user,
        ip_address=ip_address,
        detail={
            'review_id': review_id,
            'status': result.review.status,
            'succeeded': result.succeeded,
            'error_message': result.error_message,
            'remote_call_count': result.remote_call_count,
            'log_persisted': result.log_persisted,
        },
    )
    return result


def retry_announcement_review(review_id, user, ip_address):
    """重试 failed 公告发布。"""
    with transaction.atomic():
        review = _get_review_for_update(review_id)
        if review.status != 'failed':
            raise ReviewActionError(_('只有发布失败的公告可以重试'))
        _ensure_can_operate(review, user)
        if _has_newer_weekly_review(review):
            raise ReviewActionError(_('该周更新公告已有更新的审核记录，请先处理最新记录'))
        _mark_review_processing(review, user)

    result = _publish_review(review, user, ip_address)

    log_operation(
        'announcement_review',
        'retry',
        user=user,
        ip_address=ip_address,
        detail={
            'review_id': review_id,
            'status': result.review.status,
            'succeeded': result.succeeded,
            'error_message': result.error_message,
            'remote_call_count': result.remote_call_count,
            'log_persisted': result.log_persisted,
        },
    )
    return result


def reject_announcement_review(review_id, user, ip_address, comment=''):
    """作废 pending/failed 公告审核记录。"""
    with transaction.atomic():
        review = _get_review_for_update(review_id)
        if review.status not in ('pending', 'failed'):
            raise ReviewActionError(_('只有待审核或发布失败的公告可以作废'))
        _ensure_can_operate(review, user)
        review.status = 'rejected'
        review.reviewer = user
        review.reviewer_username = getattr(user, 'username', '') or ''
        review.reviewed_at = timezone.now()
        review.review_comment = comment or ''
        review.save(update_fields=[
            'status',
            'reviewer',
            'reviewer_username',
            'reviewed_at',
            'review_comment',
            'updated_at',
        ])

    log_operation(
        'announcement_review',
        'reject',
        user=user,
        ip_address=ip_address,
        detail={
            'review_id': review_id,
            'status': review.status,
            'comment': comment or '',
        },
    )
    return ReviewActionResult(review=review, succeeded=True)
