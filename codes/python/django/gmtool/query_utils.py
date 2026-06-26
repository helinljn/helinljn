"""查询筛选与分页辅助函数。"""
from datetime import timedelta

from django.core.paginator import Paginator
from django.utils import timezone



def parse_time_range_filters(request, default_days=7):
    """从请求参数中解析时间范围筛选条件。"""
    start_time_filter = request.GET.get('start_time', '')
    end_time_filter = request.GET.get('end_time', '')

    if not start_time_filter and not end_time_filter:
        if default_days is None:
            return '', '', None, None
        default_start = timezone.now() - timedelta(days=default_days)
        start_time_filter = default_start.strftime('%Y-%m-%dT%H:%M')
        return start_time_filter, end_time_filter, default_start, None

    start_dt = None
    end_dt = None

    if start_time_filter:
        try:
            start_dt = timezone.make_aware(
                timezone.datetime.strptime(start_time_filter, '%Y-%m-%dT%H:%M')
            )
        except (ValueError, TypeError):
            start_time_filter = ''
            start_dt = None

    if end_time_filter:
        try:
            end_dt = timezone.make_aware(
                timezone.datetime.strptime(end_time_filter, '%Y-%m-%dT%H:%M')
            )
        except (ValueError, TypeError):
            end_time_filter = ''
            end_dt = None

    return start_time_filter, end_time_filter, start_dt, end_dt



def apply_time_range_filters_to_queryset(queryset, request, *, field_name='created_at', default_days=7):
    """将请求中的时间范围筛选应用到查询集。"""
    start_time_filter, end_time_filter, start_dt, end_dt = parse_time_range_filters(
        request,
        default_days=default_days,
    )
    if start_dt:
        queryset = queryset.filter(**{f'{field_name}__gte': start_dt})
    if end_dt:
        queryset = queryset.filter(**{f'{field_name}__lte': end_dt})
    return queryset, start_time_filter, end_time_filter



def paginate_request_queryset(queryset, request, page_size, *, on_each_side=2, on_ends=1):
    """按请求参数对查询集进行分页。"""
    paginator = Paginator(queryset, page_size)
    page_number = request.GET.get('page', 1)
    page_obj = paginator.get_page(page_number)
    elided_page_range = list(
        paginator.get_elided_page_range(page_obj.number, on_each_side=on_each_side, on_ends=on_ends)
    )
    return page_obj, elided_page_range
