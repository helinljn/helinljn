"""审核管理相关视图。"""
import json

from django.conf import settings
from django.contrib import messages
from django.contrib.auth.decorators import login_required
from django.http import HttpResponseForbidden
from django.shortcuts import redirect, render
from django.urls import reverse
from django.utils.http import url_has_allowed_host_and_scheme
from django.utils.translation import gettext_lazy as _
from django.views.decorators.http import require_GET, require_POST

from .announcement_log_services import ANNOUNCEMENT_LOG_PERSISTENCE_WARNING
from .announcement_review_services import (
    ReviewActionError,
    ReviewSelfActionError,
    approve_announcement_review,
    reject_announcement_review,
    retry_announcement_review,
)
from .command_review_services import (
    COMMAND_LOG_PERSISTENCE_WARNING,
    CommandReviewActionError,
    CommandReviewSelfActionError,
    approve_command_review,
    has_command_review_type_permission,
    reject_command_review,
    retry_command_review,
)
from .decorators import announcement_permission_required, has_announcement_permission
from .models import AnnouncementReview, CommandReview
from .query_utils import apply_time_range_filters_to_queryset, paginate_request_queryset
from .security_utils import get_client_ip


def _review_next_url(request, fallback_name='gmtool:review_announcement_list'):
    next_url = request.POST.get('next', '')
    if next_url and url_has_allowed_host_and_scheme(
        next_url,
        allowed_hosts={request.get_host()},
        require_https=request.is_secure(),
    ):
        return next_url
    return reverse(fallback_name)


def _redirect_back(request, fallback_name='gmtool:review_announcement_list'):
    return redirect(_review_next_url(request, fallback_name=fallback_name))


@login_required
@require_GET
def review_index(request):
    """审核管理入口，进入当前用户可访问的第一个审核页。"""
    if has_announcement_permission(request.user, request):
        return redirect('gmtool:review_announcement_list')
    if has_command_review_type_permission(request.user, CommandReview.TYPE_MAIL, request):
        return redirect('gmtool:review_mail')
    if has_command_review_type_permission(request.user, CommandReview.TYPE_MARQUEE, request):
        return redirect('gmtool:review_marquee')
    messages.warning(request, _('您没有审核管理权限'))
    return redirect('gmtool:dashboard')


def _command_review_permission_denied(request, review_type):
    label = dict(CommandReview.REVIEW_TYPE_CHOICES).get(review_type, _('命令审核'))
    messages.warning(request, _('您没有%(type)s权限') % {'type': label})
    return redirect('gmtool:dashboard')


def _command_review_fallback_name(review_type):
    if review_type == CommandReview.TYPE_MAIL:
        return 'gmtool:review_mail'
    return 'gmtool:review_marquee'


def _validate_command_review_type(review_type):
    if review_type not in (CommandReview.TYPE_MAIL, CommandReview.TYPE_MARQUEE):
        raise CommandReviewActionError(_('审核类型不匹配'))
    return review_type


def _render_command_review_list(request, *, review_type, active_review_tab, page_title):
    if not has_command_review_type_permission(request.user, review_type, request):
        return _command_review_permission_denied(request, review_type)

    reviews = CommandReview.objects.select_related('submitter', 'reviewer', 'command').filter(
        review_type=review_type,
    )

    status_filter = request.GET.get('status', '') if 'status' in request.GET else 'pending'
    if status_filter:
        reviews = reviews.filter(status=status_filter)

    command_filter = request.GET.get('command', '')
    if command_filter:
        reviews = reviews.filter(command_name__icontains=command_filter)

    submitter_filter = request.GET.get('submitter', '')
    if submitter_filter:
        reviews = reviews.filter(submitter_username__icontains=submitter_filter)

    reviews, start_time_filter, end_time_filter = apply_time_range_filters_to_queryset(
        reviews,
        request,
        default_days=None,
    )
    reviews = reviews.order_by('-created_at')

    page_obj, elided_page_range = paginate_request_queryset(reviews, request, settings.PAGE_SIZE)
    for review in page_obj:
        review.params_json = json.dumps(review.params or {}, ensure_ascii=False, indent=2, default=str)
        review.response_json = json.dumps(review.response_data or {}, ensure_ascii=False, indent=2, default=str)

    review_filter_query = request.GET.copy()
    review_filter_query.pop('page', None)

    return render(
        request,
        'gmtool/review_command_list.html',
        {
            'page_obj': page_obj,
            'elided_page_range': elided_page_range,
            'active_review_tab': active_review_tab,
            'page_title': page_title,
            'review_type': review_type,
            'status_filter': status_filter,
            'command_filter': command_filter,
            'submitter_filter': submitter_filter,
            'start_time_filter': start_time_filter,
            'end_time_filter': end_time_filter,
            'status_choices': CommandReview.STATUS_CHOICES,
            'review_filter_query_string': review_filter_query.urlencode(),
            'current_review_path': request.get_full_path(),
        },
    )


@login_required
@require_GET
def review_mail(request):
    """邮件审核列表。"""
    return _render_command_review_list(
        request,
        review_type=CommandReview.TYPE_MAIL,
        active_review_tab='mail',
        page_title=_('邮件审核'),
    )


@login_required
@require_GET
def review_marquee(request):
    """走马灯审核列表。"""
    return _render_command_review_list(
        request,
        review_type=CommandReview.TYPE_MARQUEE,
        active_review_tab='marquee',
        page_title=_('走马灯审核'),
    )


@announcement_permission_required
@require_GET
def review_announcement_list(request):
    """公告审核列表。"""
    reviews = AnnouncementReview.objects.select_related('submitter', 'reviewer').all()

    status_filter = request.GET.get('status', '') if 'status' in request.GET else 'pending'
    if status_filter:
        reviews = reviews.filter(status=status_filter)

    platform_filter = request.GET.get('Platform', '')
    if platform_filter:
        reviews = reviews.filter(platform=platform_filter)

    channel_filter = request.GET.get('Channel', '')
    if channel_filter:
        reviews = reviews.filter(channel=channel_filter)

    type_filter = request.GET.get('AnnouncementType', '')
    if type_filter:
        reviews = reviews.filter(announcement_type=type_filter)

    submitter_filter = request.GET.get('submitter', '')
    if submitter_filter:
        reviews = reviews.filter(submitter_username__icontains=submitter_filter)

    reviews, start_time_filter, end_time_filter = apply_time_range_filters_to_queryset(
        reviews,
        request,
        default_days=None,
    )
    reviews = reviews.order_by('-created_at')

    page_obj, elided_page_range = paginate_request_queryset(reviews, request, settings.PAGE_SIZE)
    for review in page_obj:
        review.payload_json = json.dumps(review.payload or {}, ensure_ascii=False, indent=2, default=str)

    review_filter_query = request.GET.copy()
    review_filter_query.pop('page', None)

    return render(
        request,
        'gmtool/review_announcement_list.html',
        {
            'page_obj': page_obj,
            'elided_page_range': elided_page_range,
            'active_review_tab': 'announcements',
            'status_filter': status_filter,
            'platform_filter': platform_filter,
            'channel_filter': channel_filter,
            'type_filter': type_filter,
            'submitter_filter': submitter_filter,
            'start_time_filter': start_time_filter,
            'end_time_filter': end_time_filter,
            'platforms': getattr(settings, 'ANNOUNCEMENT_PLATFORMS', []),
            'channels': getattr(settings, 'ANNOUNCEMENT_CHANNELS', []),
            'status_choices': AnnouncementReview.STATUS_CHOICES,
            'type_choices': AnnouncementReview.ANNOUNCEMENT_TYPE_CHOICES,
            'review_filter_query_string': review_filter_query.urlencode(),
            'current_review_path': request.get_full_path(),
        },
    )


def _handle_command_review_result(request, result, *, success_message, failure_message, fallback_name):
    if not result.log_persisted:
        messages.warning(request, COMMAND_LOG_PERSISTENCE_WARNING)

    if result.succeeded:
        messages.success(request, success_message)
    else:
        messages.error(request, failure_message % {'error': result.error_message})
    return _redirect_back(request, fallback_name=fallback_name)


@login_required
@require_POST
def review_command_approve(request, review_type, review_id):
    """审核通过邮件/走马灯命令并执行。"""
    try:
        review_type = _validate_command_review_type(review_type)
        if not has_command_review_type_permission(request.user, review_type, request):
            return _command_review_permission_denied(request, review_type)
        result = approve_command_review(
            review_id,
            request.user,
            get_client_ip(request),
            expected_type=review_type,
        )
    except CommandReviewSelfActionError as exc:
        return HttpResponseForbidden(str(exc))
    except CommandReviewActionError as exc:
        messages.error(request, exc)
        return _redirect_back(request, fallback_name=_command_review_fallback_name(review_type))

    return _handle_command_review_result(
        request,
        result,
        success_message=_('命令审核已通过并执行'),
        failure_message=_('命令执行失败，已标记为执行失败：%(error)s'),
        fallback_name=_command_review_fallback_name(review_type),
    )


@login_required
@require_POST
def review_command_retry(request, review_type, review_id):
    """重试执行失败的邮件/走马灯命令审核记录。"""
    try:
        review_type = _validate_command_review_type(review_type)
        if not has_command_review_type_permission(request.user, review_type, request):
            return _command_review_permission_denied(request, review_type)
        result = retry_command_review(
            review_id,
            request.user,
            get_client_ip(request),
            expected_type=review_type,
        )
    except CommandReviewSelfActionError as exc:
        return HttpResponseForbidden(str(exc))
    except CommandReviewActionError as exc:
        messages.error(request, exc)
        return _redirect_back(request, fallback_name=_command_review_fallback_name(review_type))

    return _handle_command_review_result(
        request,
        result,
        success_message=_('命令已重试执行成功'),
        failure_message=_('命令重试执行失败：%(error)s'),
        fallback_name=_command_review_fallback_name(review_type),
    )


@login_required
@require_POST
def review_command_reject(request, review_type, review_id):
    """作废邮件/走马灯命令审核记录。"""
    try:
        review_type = _validate_command_review_type(review_type)
        if not has_command_review_type_permission(request.user, review_type, request):
            return _command_review_permission_denied(request, review_type)
        reject_command_review(
            review_id,
            request.user,
            get_client_ip(request),
            expected_type=review_type,
            comment=request.POST.get('review_comment', ''),
        )
    except CommandReviewSelfActionError as exc:
        return HttpResponseForbidden(str(exc))
    except CommandReviewActionError as exc:
        messages.error(request, exc)
        return _redirect_back(request, fallback_name=_command_review_fallback_name(review_type))

    messages.success(request, _('命令审核已作废'))
    return _redirect_back(request, fallback_name=_command_review_fallback_name(review_type))


def _handle_publish_result(request, result, *, success_message, failure_message):
    if not result.log_persisted:
        messages.warning(request, ANNOUNCEMENT_LOG_PERSISTENCE_WARNING)

    if result.succeeded:
        messages.success(request, success_message)
    else:
        messages.error(request, failure_message % {'error': result.error_message})
    return _redirect_back(request)


@announcement_permission_required
@require_POST
def review_announcement_approve(request, review_id):
    """审核通过公告并发布。"""
    try:
        result = approve_announcement_review(review_id, request.user, get_client_ip(request))
    except ReviewSelfActionError as exc:
        return HttpResponseForbidden(str(exc))
    except ReviewActionError as exc:
        messages.error(request, exc)
        return _redirect_back(request)

    return _handle_publish_result(
        request,
        result,
        success_message=_('公告审核已通过并发布'),
        failure_message=_('公告发布失败，已标记为发布失败：%(error)s'),
    )


@announcement_permission_required
@require_POST
def review_announcement_retry(request, review_id):
    """重试发布失败的公告审核记录。"""
    try:
        result = retry_announcement_review(review_id, request.user, get_client_ip(request))
    except ReviewSelfActionError as exc:
        return HttpResponseForbidden(str(exc))
    except ReviewActionError as exc:
        messages.error(request, exc)
        return _redirect_back(request)

    return _handle_publish_result(
        request,
        result,
        success_message=_('公告已重试发布成功'),
        failure_message=_('公告重试发布失败：%(error)s'),
    )


@announcement_permission_required
@require_POST
def review_announcement_reject(request, review_id):
    """作废公告审核记录。"""
    try:
        reject_announcement_review(
            review_id,
            request.user,
            get_client_ip(request),
            comment=request.POST.get('review_comment', ''),
        )
    except ReviewSelfActionError as exc:
        return HttpResponseForbidden(str(exc))
    except ReviewActionError as exc:
        messages.error(request, exc)
        return _redirect_back(request)

    messages.success(request, _('公告审核已作废'))
    return _redirect_back(request)
