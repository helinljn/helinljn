"""邮件和走马灯 GM 命令审核业务服务。"""
import logging
from dataclasses import dataclass
from types import SimpleNamespace

from django.db import DatabaseError, transaction
from django.utils import timezone
from django.utils.translation import gettext_lazy as _

from .audit_log import log_operation
from .idip_client import send_idip_command
from .models import CommandLog, CommandReview, GMCommand
from .permission_service import is_super_admin_user


logger = logging.getLogger(__name__)

COMMAND_LOG_PERSISTENCE_WARNING = _('命令请求已完成，但执行结果记录失败，请联系管理员核查应用日志')


class CommandReviewActionError(Exception):
    """命令审核操作业务错误。"""


class CommandReviewSelfActionError(CommandReviewActionError):
    """当前用户操作自己提交内容。"""


@dataclass
class CommandReviewActionResult:
    review: CommandReview
    succeeded: bool
    error_message: str = ''
    log_persisted: bool = True


def _command_search_text(command):
    return ' '.join(
        str(value or '')
        for value in (
            getattr(command, 'tab', ''),
            getattr(command, 'command_name', ''),
            getattr(command, 'request_name', ''),
            getattr(command, 'response_name', ''),
        )
    )


def is_mail_review_command(command):
    """邮件类 GM 命令需要邮件审核。"""
    text = _command_search_text(command).lower()
    return '邮件' in text or 'mail' in text


def is_marquee_review_command(command):
    """公告发布类 GM 命令需要走马灯审核；查询/删除不审核。"""
    text = _command_search_text(command)
    lowered = text.lower()
    if any(keyword in text for keyword in ('查询公告', '删除公告')):
        return False
    if any(keyword in lowered for keyword in ('query', 'delete', 'remove')):
        return False
    return (
        '发公告' in text
        or '发布公告' in text
        or 'rollingmsg' in lowered
        or 'sendnotice' in lowered
        or 'publishnotice' in lowered
    )


def get_command_review_type(command):
    if is_mail_review_command(command):
        return CommandReview.TYPE_MAIL
    if is_marquee_review_command(command):
        return CommandReview.TYPE_MARQUEE
    return ''


def _iter_user_review_commands(user, review_type):
    commands = GMCommand.objects.filter(is_active=True)
    if not is_super_admin_user(user):
        commands = commands.filter(usercommandpermission__user=user).distinct()

    for command in commands.only('id', 'tab', 'command_name', 'request_name', 'response_name'):
        if get_command_review_type(command) == review_type:
            yield command


def has_command_review_type_permission(user, review_type, request=None):
    """判断用户是否有指定 GM 命令审核类型权限。"""
    if not user or not user.is_authenticated:
        return False
    if review_type not in (CommandReview.TYPE_MAIL, CommandReview.TYPE_MARQUEE):
        return False
    if is_super_admin_user(user):
        return True

    if request:
        cache = getattr(request, '_command_review_permission_cache', None)
        if cache is not None and review_type in cache:
            return cache[review_type]

    result = any(_iter_user_review_commands(user, review_type))

    if request:
        if not hasattr(request, '_command_review_permission_cache'):
            request._command_review_permission_cache = {}
        request._command_review_permission_cache[review_type] = result
    return result


def has_any_command_review_permission(user, request=None):
    return (
        has_command_review_type_permission(user, CommandReview.TYPE_MAIL, request)
        or has_command_review_type_permission(user, CommandReview.TYPE_MARQUEE, request)
    )


def submit_command_review(command, params, user, ip_address):
    """创建 GM 命令审核记录，不调用 IDIP。"""
    review_type = get_command_review_type(command)
    if not review_type:
        raise CommandReviewActionError(_('该命令不需要审核'))

    partition = int(params.get('Partition', 0))
    review = CommandReview.objects.create(
        review_type=review_type,
        command=command,
        command_code=command.command_id,
        command_name=command.command_name,
        command_tab=command.tab or '',
        request_name=command.request_name,
        request_id=command.request_id,
        response_name=command.response_name,
        response_id=command.response_id,
        submitter=user,
        submitter_username=getattr(user, 'username', '') or '',
        partition=partition,
        params=params,
        ip_address=ip_address,
    )
    log_operation(
        'command_review',
        'submit',
        user=user,
        ip_address=ip_address,
        detail={
            'review_id': review.id,
            'review_type': review.review_type,
            'command_id': review.command_code,
            'command_name': review.command_name,
            'partition': review.partition,
            'status': review.status,
        },
    )
    return review


def _ensure_can_operate(review, user):
    if review.submitter_id and user and review.submitter_id == user.id:
        raise CommandReviewSelfActionError(_('不能审核自己提交的内容'))


def _get_review_for_update(review_id, expected_type=None):
    try:
        review = CommandReview.objects.select_for_update().get(pk=review_id)
    except CommandReview.DoesNotExist as exc:
        raise CommandReviewActionError(_('待审核命令不存在')) from exc

    if expected_type and review.review_type != expected_type:
        raise CommandReviewActionError(_('审核类型不匹配'))
    return review


def _command_from_review(review):
    return SimpleNamespace(
        command_id=review.command_code,
        command_name=review.command_name,
        tab=review.command_tab,
        request_name=review.request_name,
        request_id=review.request_id,
        response_name=review.response_name,
        response_id=review.response_id,
    )


def _wrapped_failed(response_data):
    return isinstance(response_data, dict) and 'status' in response_data and not bool(response_data.get('status'))


def _wrapped_error_message(response_data):
    if not isinstance(response_data, dict):
        return _('命令执行失败')
    wrapped_json = response_data.get('json')
    if isinstance(wrapped_json, dict):
        return wrapped_json.get('RetMsg') or wrapped_json.get('retmsg') or _('命令执行失败')
    return _('命令执行失败')


def _command_log_status(response_data, error_message, error_type):
    if error_message or _wrapped_failed(response_data):
        return 'timeout' if error_type == 'timeout' else 'failed'
    return 'success'


def _write_command_log(review, ip_address, *, response_data, request_content, status):
    try:
        with transaction.atomic():
            CommandLog.objects.create(
                user=review.submitter,
                operator_username=review.submitter_username,
                command=review.command,
                partition=review.partition,
                request_data=review.params,
                request_content=request_content,
                response_data=response_data,
                status=status,
                ip_address=ip_address,
            )
    except DatabaseError as exc:
        logger.exception(
            'Command review log persistence failed: review_id=%s command=%s submitter=%s status=%s error=%s',
            review.id,
            review.command_code,
            review.submitter_username,
            status,
            exc,
        )
        return False
    return True


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
    command = _command_from_review(review)
    response_data, error_message, request_content, error_type = send_idip_command(command, dict(review.params or {}))
    status = _command_log_status(response_data, error_message, error_type)
    log_persisted = _write_command_log(
        review,
        ip_address,
        response_data=response_data,
        request_content=request_content,
        status=status,
    )

    review.reviewer = user
    review.reviewer_username = getattr(user, 'username', '') or ''
    review.reviewed_at = timezone.now()
    review.request_content = request_content or ''
    review.response_data = response_data
    if status == 'success':
        review.status = 'approved'
        review.error_message = ''
        succeeded = True
        result_error = ''
    else:
        review.status = 'failed'
        review.error_message = error_message or _wrapped_error_message(response_data)
        succeeded = False
        result_error = review.error_message
    review.save(update_fields=[
        'reviewer',
        'reviewer_username',
        'reviewed_at',
        'request_content',
        'response_data',
        'status',
        'error_message',
        'updated_at',
    ])

    return CommandReviewActionResult(
        review=review,
        succeeded=succeeded,
        error_message=result_error,
        log_persisted=log_persisted,
    )


def approve_command_review(review_id, user, ip_address, *, expected_type=None):
    """审核通过 pending 命令，并执行 IDIP。"""
    with transaction.atomic():
        review = _get_review_for_update(review_id, expected_type=expected_type)
        if review.status != 'pending':
            raise CommandReviewActionError(_('只有待审核命令可以通过'))
        _ensure_can_operate(review, user)
        _mark_review_processing(review, user)

    result = _publish_review(review, user, ip_address)

    log_operation(
        'command_review',
        'approve',
        user=user,
        ip_address=ip_address,
        detail={
            'review_id': review_id,
            'review_type': result.review.review_type,
            'command_id': result.review.command_code,
            'status': result.review.status,
            'succeeded': result.succeeded,
            'error_message': result.error_message,
            'log_persisted': result.log_persisted,
        },
    )
    return result


def retry_command_review(review_id, user, ip_address, *, expected_type=None):
    """重试执行 failed 命令审核记录。"""
    with transaction.atomic():
        review = _get_review_for_update(review_id, expected_type=expected_type)
        if review.status != 'failed':
            raise CommandReviewActionError(_('只有执行失败的命令可以重试'))
        _ensure_can_operate(review, user)
        _mark_review_processing(review, user)

    result = _publish_review(review, user, ip_address)

    log_operation(
        'command_review',
        'retry',
        user=user,
        ip_address=ip_address,
        detail={
            'review_id': review_id,
            'review_type': result.review.review_type,
            'command_id': result.review.command_code,
            'status': result.review.status,
            'succeeded': result.succeeded,
            'error_message': result.error_message,
            'log_persisted': result.log_persisted,
        },
    )
    return result


def reject_command_review(review_id, user, ip_address, *, expected_type=None, comment=''):
    """作废 pending/failed 命令审核记录。"""
    with transaction.atomic():
        review = _get_review_for_update(review_id, expected_type=expected_type)
        if review.status not in ('pending', 'failed'):
            raise CommandReviewActionError(_('只有待审核或执行失败的命令可以作废'))
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
        'command_review',
        'reject',
        user=user,
        ip_address=ip_address,
        detail={
            'review_id': review_id,
            'review_type': review.review_type,
            'command_id': review.command_code,
            'status': review.status,
            'comment': comment or '',
        },
    )
    return CommandReviewActionResult(review=review, succeeded=True)
