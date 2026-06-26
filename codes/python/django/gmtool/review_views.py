"""审核管理相关视图。"""
import json

from django.conf import settings
from django.contrib import messages
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
from .decorators import announcement_permission_required
from .models import AnnouncementReview
from .query_utils import apply_time_range_filters_to_queryset, paginate_request_queryset
from .security_utils import get_client_ip


def _review_next_url(request):
    next_url = request.POST.get('next', '')
    if next_url and url_has_allowed_host_and_scheme(
        next_url,
        allowed_hosts={request.get_host()},
        require_https=request.is_secure(),
    ):
        return next_url
    return reverse('gmtool:review_announcement_list')


def _redirect_back(request):
    return redirect(_review_next_url(request))


@announcement_permission_required
@require_GET
def review_index(request):
    """审核管理入口，默认进入公告审核。"""
    return redirect('gmtool:review_announcement_list')


@announcement_permission_required
@require_GET
def review_mail(request):
    """邮件审核第一阶段空态。"""
    return render(
        request,
        'gmtool/review_mail.html',
        {
            'active_review_tab': 'mail',
        },
    )


@announcement_permission_required
@require_GET
def review_marquee(request):
    """走马灯审核第一阶段空态。"""
    return render(
        request,
        'gmtool/review_marquee.html',
        {
            'active_review_tab': 'marquee',
        },
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
