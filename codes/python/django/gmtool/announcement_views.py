"""公告管理相关视图。"""
import logging
from urllib.parse import urlencode

from django.conf import settings
from django.contrib import messages
from django.db import DatabaseError
from django.http import QueryDict
from django.shortcuts import redirect, render
from django.urls import reverse
from django.utils.translation import gettext_lazy as _
from django.views.decorators.http import require_GET, require_http_methods

from .announcement_client import (
    create_announcement,
    delete_announcement,
    get_announcement_base_url_error,
    query_announcements,
)
from .audit_log import log_operation
from .decorators import announcement_permission_required
from .forms import (
    AnnouncementCreateForm,
    AnnouncementDeleteForm,
    AnnouncementQueryForm,
    get_announcement_config_errors,
)
from .models import AnnouncementLog
from .query_utils import apply_time_range_filters_to_queryset, paginate_request_queryset
from .security_utils import get_client_ip

logger = logging.getLogger(__name__)

_ANNOUNCEMENT_LOG_PERSISTENCE_WARNING = _('公告远端操作已完成，但本地公告日志记录失败，请联系管理员核查应用日志')


def _has_query_input(data):
    return bool(data.get('Platform') or data.get('AnnouncementType') or data.get('Channel'))


def _query_initial_from_data(data):
    if not data:
        return {}
    return {
        'Platform': data.get('Platform', ''),
        'Channel': data.get('Channel', ''),
        'AnnouncementType': data.get('AnnouncementType', ''),
    }


def _query_string(platform, channel, announcement_type):
    query = {
        'Platform': platform,
        'Channel': channel,
        'AnnouncementType': announcement_type,
    }
    return urlencode(query)


def _redirect_to_announcement_query(platform, channel, announcement_type):
    query_string = _query_string(platform, channel, announcement_type)
    url = reverse('gmtool:announcement_query')
    if query_string:
        url = f'{url}?{query_string}'
    return redirect(url)


def _delete_initial_from_data(data):
    if not data:
        return {}
    return {
        'Platform': data.get('Platform', ''),
        'Channel': data.get('Channel', ''),
        'AnnouncementType': data.get('AnnouncementType', ''),
        'AnnouncementId': data.get('AnnouncementId', ''),
    }


def _announcement_config_errors():
    errors = []
    base_url_error = get_announcement_base_url_error()
    if base_url_error:
        errors.append(base_url_error)
    errors.extend(get_announcement_config_errors())
    return errors


def _status_from_error(error_message, error_type):
    if not error_message:
        return 'success'
    if error_type == 'timeout':
        return 'timeout'
    return 'failed'


def _write_announcement_log(
    request,
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
):
    try:
        AnnouncementLog.objects.create(
            user=request.user,
            operator_username=request.user.username,
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
            ip_address=get_client_ip(request),
        )
    except DatabaseError as exc:
        logger.exception(
            'Announcement log persistence failed: action=%s user=%s status=%s error=%s',
            action,
            request.user.username,
            status,
            exc,
        )
        return False
    return True


def _query_for_form(query_form):
    results = []
    failures = []
    if not query_form.is_valid():
        return results, failures

    platform = query_form.cleaned_data['Platform']
    channel = query_form.cleaned_data['Channel']
    announcement_type = query_form.cleaned_data['AnnouncementType']
    response_data, error_message, raw_response, error_type = query_announcements(
        platform,
        channel,
        announcement_type,
    )
    if error_message:
        failures.append({
            'channel': channel,
            'error': error_message,
            'error_type': error_type,
            'raw_response': raw_response,
        })
        return results, failures

    announcements = []
    for announcement in response_data or []:
        announcement_data = dict(announcement)
        announcement_data['delete_query_string'] = urlencode({
            'Platform': announcement_data.get('Platform') or platform,
            'Channel': announcement_data.get('Channel') or channel,
            'AnnouncementType': announcement_data.get('AnnouncementType') or announcement_type,
            'AnnouncementId': announcement_data.get('AnnouncementId', ''),
        })
        announcements.append(announcement_data)

    results.append({
        'channel': channel,
        'announcements': announcements,
        'raw_response': raw_response,
    })
    return results, failures


def _render_announcement_query_page(
    request,
    *,
    query_data=None,
    run_query=False,
    query_form=None,
    status=200,
):
    query_data = query_data if query_data is not None else request.GET
    query_results = []
    query_failures = []
    announcement_config_errors = _announcement_config_errors()

    if query_form is None:
        if _has_query_input(query_data):
            query_form = AnnouncementQueryForm(query_data)
        else:
            query_form = AnnouncementQueryForm()

    if (
        run_query
        and query_form.is_bound
        and query_form.is_valid()
        and not announcement_config_errors
    ):
        query_results, query_failures = _query_for_form(query_form)
        if query_failures and not query_results:
            messages.error(request, _('公告查询失败，请查看失败明细'))
        elif query_failures:
            messages.warning(request, _('公告查询部分失败，请查看失败明细'))

    return render(
        request,
        'gmtool/announcement_query.html',
        {
            'active_announcement_tab': 'query',
            'query_form': query_form,
            'query_results': query_results,
            'query_failures': query_failures,
            'current_query_string': query_data.urlencode(),
            'selected_query': _query_initial_from_data(query_data),
            'announcement_config_errors': announcement_config_errors,
        },
        status=status,
    )


def _render_announcement_create_page(
    request,
    *,
    query_data=None,
    create_form=None,
    operation_results=None,
    status=200,
):
    query_data = query_data if query_data is not None else request.GET
    if create_form is None:
        create_form = AnnouncementCreateForm(initial=_query_initial_from_data(query_data))

    return render(
        request,
        'gmtool/announcement_create.html',
        {
            'active_announcement_tab': 'create',
            'create_form': create_form,
            'operation_results': operation_results or {},
            'announcement_config_errors': _announcement_config_errors(),
        },
        status=status,
    )


def _render_announcement_delete_page(
    request,
    *,
    delete_form=None,
    operation_results=None,
    status=200,
):
    if delete_form is None:
        delete_form = AnnouncementDeleteForm(initial=_delete_initial_from_data(request.GET))

    return render(
        request,
        'gmtool/announcement_delete.html',
        {
            'active_announcement_tab': 'delete',
            'delete_form': delete_form,
            'operation_results': operation_results or {},
            'announcement_config_errors': _announcement_config_errors(),
        },
        status=status,
    )


def _append_failure(failures, channel, error_message):
    failures.append({
        'channel': channel,
        'error': str(error_message),
    })


@announcement_permission_required
@require_GET
def announcement_list(request):
    """公告管理兼容入口，默认进入查询公告页。"""
    url = reverse('gmtool:announcement_query')
    if request.META.get('QUERY_STRING'):
        url = f'{url}?{request.META["QUERY_STRING"]}'
    return redirect(url)


@announcement_permission_required
@require_GET
def announcement_query(request):
    """公告查询页面。"""
    return _render_announcement_query_page(
        request,
        run_query=_has_query_input(request.GET),
    )


@announcement_permission_required
@require_http_methods(['GET', 'POST'])
def announcement_create(request):
    """发布公告。"""
    if request.method == 'GET':
        return _render_announcement_create_page(request)

    form = AnnouncementCreateForm(request.POST)
    if not form.is_valid():
        messages.error(request, _('公告发布表单校验失败，请检查输入'))
        return _render_announcement_create_page(
            request,
            query_data=request.POST,
            create_form=form,
            status=400,
        )

    platform = form.cleaned_data['Platform']
    channel = form.cleaned_data['Channel']
    channels = [channel]
    announcement_type = form.cleaned_data['AnnouncementType']
    client_ip = get_client_ip(request)
    success_channels = []
    failed_channels = []
    remote_call_count = 0
    log_persisted = True

    for channel in channels:
        if announcement_type == '1':
            old_announcements, query_error, _raw_response, _error_type = query_announcements(
                platform,
                channel,
                announcement_type,
            )
            if query_error:
                _append_failure(failed_channels, channel, query_error)
                continue

            delete_failed = False
            for old_announcement in old_announcements or []:
                old_announcement_id = old_announcement.get('AnnouncementId')
                if not old_announcement_id or str(old_announcement_id) == '-1':
                    _append_failure(failed_channels, channel, _('旧周更新公告缺少有效公告 ID'))
                    delete_failed = True
                    break

                old_channel = old_announcement.get('Channel') or channel
                old_type = old_announcement.get('AnnouncementType') or announcement_type
                delete_response, delete_error, delete_raw, delete_error_type = delete_announcement(
                    platform,
                    old_channel,
                    old_type,
                    old_announcement_id,
                )
                remote_call_count += 1
                delete_status = _status_from_error(delete_error, delete_error_type)
                delete_log_persisted = _write_announcement_log(
                    request,
                    action='delete',
                    platform=platform,
                    channel=old_channel,
                    announcement_type=old_type,
                    announcement_id=old_announcement_id,
                    request_data={
                        'Platform': platform,
                        'Channel': old_channel,
                        'AnnouncementType': old_type,
                        'AnnouncementId': str(old_announcement_id),
                    },
                    response_data=delete_response,
                    raw_response=delete_raw,
                    status=delete_status,
                    error_message=delete_error,
                )
                log_persisted = log_persisted and delete_log_persisted
                if delete_error:
                    _append_failure(failed_channels, channel, delete_error)
                    delete_failed = True
                    break

            if delete_failed:
                continue

        payload = form.to_payload(channel)
        response_data, error_message, raw_response, error_type = create_announcement(payload)
        remote_call_count += 1
        status = _status_from_error(error_message, error_type)
        create_log_persisted = _write_announcement_log(
            request,
            action='create',
            platform=platform,
            channel=channel,
            announcement_type=announcement_type,
            announcement_id=payload['AnnouncementId'],
            request_data=payload,
            response_data=response_data,
            raw_response=raw_response,
            status=status,
            error_message=error_message,
        )
        log_persisted = log_persisted and create_log_persisted

        if error_message:
            _append_failure(failed_channels, channel, error_message)
        else:
            success_channels.append(channel)

    overall_status = 'success' if success_channels and not failed_channels else 'failed'
    log_operation(
        'announcement',
        'create',
        user=request.user,
        ip_address=client_ip,
        detail={
            'platform': platform,
            'channels': channels,
            'announcement_type': announcement_type,
            'announcement_id': '-1',
            'remote_call_count': remote_call_count,
            'success_channels': success_channels,
            'failed_channels': failed_channels,
            'status': overall_status,
            'log_persisted': log_persisted,
        },
    )

    if not log_persisted:
        messages.warning(request, _ANNOUNCEMENT_LOG_PERSISTENCE_WARNING)

    if success_channels and not failed_channels:
        messages.success(request, _('公告发布成功'))
        return _redirect_to_announcement_query(platform, channel, announcement_type)

    if success_channels:
        messages.warning(request, _('公告发布部分失败，请查看失败明细'))
    else:
        messages.error(request, _('公告发布失败，请查看失败明细'))

    query_data = QueryDict('', mutable=True)
    query_data['Platform'] = platform
    query_data['AnnouncementType'] = announcement_type
    query_data['Channel'] = channel
    return _render_announcement_create_page(
        request,
        query_data=query_data,
        create_form=form,
        operation_results={
            'success_channels': success_channels,
            'failed_channels': failed_channels,
        },
        status=400,
    )


@announcement_permission_required
@require_http_methods(['GET', 'POST'])
def announcement_delete(request):
    """删除单条公告。"""
    if request.method == 'GET':
        return _render_announcement_delete_page(request)

    form = AnnouncementDeleteForm(request.POST)
    if not form.is_valid():
        messages.error(request, _('公告删除表单校验失败，请刷新后重试'))
        return _render_announcement_delete_page(
            request,
            delete_form=form,
            status=400,
        )

    payload = form.to_payload()
    response_data, error_message, raw_response, error_type = delete_announcement(
        payload['Platform'],
        payload['Channel'],
        payload['AnnouncementType'],
        payload['AnnouncementId'],
    )
    status = _status_from_error(error_message, error_type)
    log_persisted = _write_announcement_log(
        request,
        action='delete',
        platform=payload['Platform'],
        channel=payload['Channel'],
        announcement_type=payload['AnnouncementType'],
        announcement_id=payload['AnnouncementId'],
        request_data=payload,
        response_data=response_data,
        raw_response=raw_response,
        status=status,
        error_message=error_message,
    )

    log_operation(
        'announcement',
        'delete',
        user=request.user,
        ip_address=get_client_ip(request),
        detail={
            'platform': payload['Platform'],
            'channels': [payload['Channel']],
            'announcement_type': payload['AnnouncementType'],
            'announcement_id': payload['AnnouncementId'],
            'remote_call_count': 1,
            'success_channels': [] if error_message else [payload['Channel']],
            'failed_channels': [{'channel': payload['Channel'], 'error': error_message}] if error_message else [],
            'status': status,
            'log_persisted': log_persisted,
        },
    )

    if not log_persisted:
        messages.warning(request, _ANNOUNCEMENT_LOG_PERSISTENCE_WARNING)

    if not error_message:
        messages.success(request, _('公告删除成功'))
        return _redirect_to_announcement_query(
            payload['Platform'],
            payload['Channel'],
            payload['AnnouncementType'],
        )

    messages.error(request, _('公告删除失败: %(error)s') % {'error': error_message})
    return _render_announcement_delete_page(
        request,
        delete_form=form,
        operation_results={
            'failed_channels': [{'channel': payload['Channel'], 'error': error_message}],
        },
        status=400,
    )


@announcement_permission_required
@require_GET
def announcement_log_list(request):
    """公告操作日志列表。"""
    logs = AnnouncementLog.objects.select_related('user').all()

    action_filter = request.GET.get('action', '')
    if action_filter:
        logs = logs.filter(action=action_filter)

    status_filter = request.GET.get('status', '')
    if status_filter:
        logs = logs.filter(status=status_filter)

    platform_filter = request.GET.get('Platform', '')
    if platform_filter:
        logs = logs.filter(platform=platform_filter)

    channel_filter = request.GET.get('Channel', '')
    if channel_filter:
        logs = logs.filter(channel=channel_filter)

    type_filter = request.GET.get('AnnouncementType', '')
    if type_filter:
        logs = logs.filter(announcement_type=type_filter)

    user_filter = request.GET.get('user', '')
    if user_filter:
        logs = logs.filter(operator_username__icontains=user_filter)

    logs, start_time_filter, end_time_filter = apply_time_range_filters_to_queryset(logs, request)
    logs = logs.order_by('-created_at')

    page_obj, elided_page_range = paginate_request_queryset(logs, request, settings.PAGE_SIZE)
    log_filter_query = request.GET.copy()
    log_filter_query.pop('page', None)

    return render(
        request,
        'gmtool/announcement_log.html',
        {
            'page_obj': page_obj,
            'elided_page_range': elided_page_range,
            'active_announcement_tab': 'logs',
            'action_filter': action_filter,
            'status_filter': status_filter,
            'platform_filter': platform_filter,
            'channel_filter': channel_filter,
            'type_filter': type_filter,
            'user_filter': user_filter,
            'start_time_filter': start_time_filter,
            'end_time_filter': end_time_filter,
            'platforms': getattr(settings, 'ANNOUNCEMENT_PLATFORMS', []),
            'channels': getattr(settings, 'ANNOUNCEMENT_CHANNELS', []),
            'action_choices': AnnouncementLog.ACTION_CHOICES,
            'status_choices': AnnouncementLog.STATUS_CHOICES,
            'log_filter_query_string': log_filter_query.urlencode(),
        },
    )
