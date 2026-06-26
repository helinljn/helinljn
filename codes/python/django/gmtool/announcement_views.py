"""公告管理相关视图。"""
from urllib.parse import urlencode

from django.conf import settings
from django.contrib import messages
from django.http import QueryDict
from django.shortcuts import redirect, render
from django.urls import reverse
from django.utils.translation import gettext_lazy as _
from django.views.decorators.http import require_GET, require_POST, require_http_methods

from .announcement_client import (
    delete_announcement,
    get_announcement_base_url_error,
    query_announcements,
)
from .announcement_log_services import (
    ANNOUNCEMENT_LOG_PERSISTENCE_WARNING,
    announcement_status_from_error,
    write_announcement_log,
)
from .announcement_review_services import submit_announcement_reviews
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


def _has_query_input(data):
    return bool(data.get('Platform') or data.get('Channel'))


def _channels_from_data(data):
    """从请求数据中提取渠道列表，兼容无 getlist 的普通 dict。"""
    if not data:
        return []
    if hasattr(data, 'getlist'):
        return data.getlist('Channel')
    channel = data.get('Channel', '')
    return [channel] if channel else []


def _query_initial_from_data(data):
    if not data:
        return {}
    return {
        'Platform': data.get('Platform', ''),
        'Channel': _channels_from_data(data),
    }


def _typed_initial_from_data(data):
    initial = _query_initial_from_data(data)
    if data and data.get('AnnouncementType'):
        initial['AnnouncementType'] = data.get('AnnouncementType', '')
    return initial


def _normalize_channels(channels):
    """将渠道参数归一为列表，兼容单值字符串。"""
    if not channels:
        return []
    if isinstance(channels, str):
        return [channels]
    return [channel for channel in channels if channel]


def _query_string(platform, channels):
    query = {}
    if platform:
        query['Platform'] = platform
    channel_list = _normalize_channels(channels)
    if channel_list:
        query['Channel'] = channel_list
    return urlencode(query, doseq=True)


def _redirect_to_announcement_query(platform, channels):
    query_string = _query_string(platform, channels)
    url = reverse('gmtool:announcement_query')
    if query_string:
        url = f'{url}?{query_string}'
    return redirect(url)


def _announcement_config_errors():
    errors = []
    base_url_error = get_announcement_base_url_error()
    if base_url_error:
        errors.append(base_url_error)
    errors.extend(get_announcement_config_errors())
    return errors


def _status_from_error(error_message, error_type):
    return announcement_status_from_error(error_message, error_type)


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
    return write_announcement_log(
        request.user,
        get_client_ip(request),
        action=action,
        platform=platform,
        channel=channel,
        announcement_type=announcement_type,
        announcement_id=announcement_id,
        request_data=request_data,
        response_data=response_data,
        raw_response=raw_response,
        status=status,
        error_message=error_message,
    )


def _query_for_form(query_form):
    results = []
    failures = []
    if not query_form.is_valid():
        return results, failures

    platform = query_form.cleaned_data['Platform']
    channels = query_form.cleaned_data['Channel']
    for channel in channels:
        response_data, error_message, raw_response, error_type = query_announcements(
            platform,
            channel,
        )
        if error_message:
            failures.append({
                'channel': channel,
                'error': error_message,
                'error_type': error_type,
                'raw_response': raw_response,
            })
            continue

        announcements = []
        for announcement in response_data or []:
            announcement_data = dict(announcement)
            announcement_data['delete_query_string'] = urlencode({
                'Platform': announcement_data.get('Platform') or platform,
                'Channel': announcement_data.get('Channel') or channel,
                'AnnouncementType': announcement_data.get('AnnouncementType', ''),
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
            'current_query_string': _query_string(query_data.get('Platform'), _channels_from_data(query_data)),
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
        create_form = AnnouncementCreateForm(initial=_typed_initial_from_data(query_data))

    return render(
        request,
        'gmtool/announcement_create.html',
        {
            'active_announcement_tab': 'create',
            'create_form': create_form,
            'operation_results': operation_results or {},
            'announcement_config_errors': get_announcement_config_errors(),
        },
        status=status,
    )


@announcement_permission_required
@require_GET
def announcement_list(request):
    """公告管理兼容入口，默认进入查询公告页。"""
    url = reverse('gmtool:announcement_query')
    query_string = _query_string(request.GET.get('Platform'), _channels_from_data(request.GET))
    if query_string:
        url = f'{url}?{query_string}'
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
    """提交公告审核。"""
    if request.method == 'GET':
        return _render_announcement_create_page(request)

    form = AnnouncementCreateForm(request.POST)
    if not form.is_valid():
        messages.error(request, _('公告提交审核表单校验失败，请检查输入'))
        return _render_announcement_create_page(
            request,
            query_data=request.POST,
            create_form=form,
            status=400,
        )

    operation_results = submit_announcement_reviews(form, request.user, get_client_ip(request))
    success_channels = operation_results['success_channels']
    failed_channels = operation_results['failed_channels']

    if success_channels and not failed_channels:
        messages.success(request, _('公告已提交审核'))
        return redirect('gmtool:review_announcement_list')

    if success_channels:
        messages.warning(request, _('公告审核部分提交失败，请查看失败明细'))
    else:
        messages.error(request, _('公告提交审核失败，请查看失败明细'))

    # 绑定表单已保留用户的多选渠道等输入，直接重渲染即可。
    return _render_announcement_create_page(
        request,
        create_form=form,
        operation_results=operation_results,
        status=400,
    )


@announcement_permission_required
@require_POST
def announcement_batch_delete(request):
    """批量删除公告：服务端循环调用远端单条删除接口，逐条记录日志并汇总结果。"""
    items = request.POST.getlist('items')
    redirect_platform = request.POST.get('Platform', '')
    redirect_channel = request.POST.get('Channel', '')

    if not items:
        messages.error(request, _('请至少选择一条公告'))
        return _redirect_to_announcement_query(redirect_platform, redirect_channel)

    max_targets = getattr(settings, 'BATCH_EXECUTE_MAX_TARGETS', 100)
    if len(items) > max_targets:
        messages.error(request, _('单次批量删除不能超过 %(n)s 条') % {'n': max_targets})
        return _redirect_to_announcement_query(redirect_platform, redirect_channel)

    success_items = []
    failed_items = []
    remote_call_count = 0
    log_persisted = True

    for item in items:
        form = AnnouncementDeleteForm(QueryDict(item))
        if not form.is_valid():
            failed_items.append({'item': item, 'error': _('选中项数据非法')})
            continue

        payload = form.to_payload()
        response_data, error_message, raw_response, error_type = delete_announcement(
            payload['Platform'],
            payload['Channel'],
            payload['AnnouncementType'],
            payload['AnnouncementId'],
        )
        remote_call_count += 1
        status = _status_from_error(error_message, error_type)
        item_log_persisted = _write_announcement_log(
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
        log_persisted = log_persisted and item_log_persisted

        if error_message:
            failed_items.append({
                'channel': payload['Channel'],
                'announcement_id': payload['AnnouncementId'],
                'error': error_message,
            })
        else:
            success_items.append(payload['AnnouncementId'])

    overall_status = 'success' if success_items and not failed_items else 'failed'
    log_operation(
        'announcement',
        'batch_delete',
        user=request.user,
        ip_address=get_client_ip(request),
        detail={
            'requested': len(items),
            'remote_call_count': remote_call_count,
            'success_count': len(success_items),
            'failed_count': len(failed_items),
            'failed_items': failed_items,
            'status': overall_status,
            'log_persisted': log_persisted,
        },
    )

    if not log_persisted:
        messages.warning(request, ANNOUNCEMENT_LOG_PERSISTENCE_WARNING)

    if not failed_items:
        messages.success(request, _('批量删除成功：共 %(n)s 条') % {'n': len(success_items)})
    elif success_items:
        messages.warning(
            request,
            _('批量删除部分失败：成功 %(ok)s 条，失败 %(fail)s 条')
            % {'ok': len(success_items), 'fail': len(failed_items)},
        )
    else:
        messages.error(request, _('批量删除失败：%(n)s 条均未成功') % {'n': len(failed_items)})

    return _redirect_to_announcement_query(redirect_platform, redirect_channel)


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
