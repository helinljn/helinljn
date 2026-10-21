"""GM 命令批量导入与执行视图。"""
import json
import logging

from django.conf import settings
from django.contrib import messages
from django.contrib.auth.decorators import login_required
from django.db import DatabaseError, transaction
from django.http import Http404, HttpResponse
from django.shortcuts import get_object_or_404, redirect, render
from django.utils.translation import gettext_lazy as _
from django.views.decorators.http import require_GET, require_POST

from .audit_log import log_operation
from .batch_execute_service import (
    AUTO_RETRY_JOB_STATUSES,
    FIRST_RUN_JOB_STATUSES,
    confirm_unknown_and_retry,
    execute_job,
    retry_failed_items,
)
from .batch_import_service import (
    BatchImportError,
    build_template_csv,
    parse_uploaded_file,
)
from .command_review_services import (
    CommandReviewActionError,
    get_command_review_type,
    has_command_review_type_permission,
    submit_batch_command_review,
)
from .decorators import command_permission_required, is_super_admin
from .forms import BatchImportForm
from .models import BatchExecuteItem, BatchExecuteJob, GMCommand
from .query_utils import paginate_request_queryset
from .security_utils import get_client_ip

logger = logging.getLogger(__name__)


def _make_job_display_rows(rows):
    """把解析结果转换为模板可展示的行数据。"""
    display_rows = []
    for row in rows:
        params_json = ''
        if row.params is not None:
            params_json = json.dumps(row.params, ensure_ascii=False, separators=(',', ':'))
        display_rows.append({
            'row_index': row.row_index,
            'is_valid': row.is_valid,
            'error_message': row.error_message,
            'params_json': params_json,
        })
    return display_rows


def _get_job_for_user(job_id, user, request, *, allow_reviewer=False):
    job = get_object_or_404(
        BatchExecuteJob.objects.select_related('command'),
        pk=job_id,
    )
    if is_super_admin(user, request) or job.submitter_id == getattr(user, 'id', None):
        return job

    # 审核人可查看批量任务详情用于审核，但不能在其页面执行确认/重试
    if (
        allow_reviewer
        and job.review_type
        and has_command_review_type_permission(user, job.review_type, request)
    ):
        return job

    raise Http404


def _create_draft_job(command, import_result, request, filename):
    client_ip = get_client_ip(request)
    with transaction.atomic():
        job = BatchExecuteJob.objects.create(
            command=command,
            command_code=command.command_id,
            command_name=command.command_name,
            review_type=get_command_review_type(command),
            status=BatchExecuteJob.STATUS_DRAFT,
            submitter=request.user,
            submitter_username=request.user.username,
            source_filename=(filename or '')[:255],
            total_items=len(import_result.valid_rows),
        )
        BatchExecuteItem.objects.bulk_create([
            BatchExecuteItem(
                job=job,
                row_index=row.row_index,
                params=row.params,
            )
            for row in import_result.valid_rows
        ])

    log_operation(
        'command',
        'batch_import',
        user=request.user,
        ip_address=client_ip,
        detail={
            'batch_job_id': job.id,
            'command_id': command.command_id,
            'total_items': job.total_items,
            'filename': job.source_filename,
        },
    )
    return job


@login_required
@command_permission_required
def command_batch(request, cmd_id):
    """批量导入页面：GET 显示上传表单，POST 解析并生成预览/草稿任务。"""
    command = getattr(request, '_gmcommand', None) or GMCommand.objects.filter(command_id=cmd_id).first()
    if not command or not command.is_active:
        messages.error(request, _('该命令已被停用或删除'))
        return redirect('gmtool:command_list')

    form = BatchImportForm(request.POST or None, request.FILES or None)

    if request.method == 'POST' and form.is_valid():
        uploaded = form.cleaned_data['file']
        try:
            import_result = parse_uploaded_file(command, uploaded)
        except BatchImportError as exc:
            form.add_error('file', str(exc))
        else:
            if import_result.is_valid:
                try:
                    job = _create_draft_job(command, import_result, request, uploaded.name)
                except DatabaseError:
                    logger.exception(
                        'Batch job creation failed: command=%s user=%s',
                        cmd_id,
                        request.user.username,
                    )
                    messages.error(request, _('批量任务保存失败，请稍后重试'))
                    return redirect('gmtool:command_batch', cmd_id=command.command_id)
                return redirect('gmtool:batch_job_detail', job_id=job.id)

            messages.error(request, _('文件中存在校验未通过的行，请修正后重新上传'))
            return render(request, 'gmtool/command_batch.html', {
                'command': command,
                'form': form,
                'import_result': import_result,
                'display_rows': _make_job_display_rows(import_result.rows),
                'max_rows': import_result.total_rows,
                'is_admin': is_super_admin(request.user, request),
            }, status=400)

    return render(request, 'gmtool/command_batch.html', {
        'command': command,
        'form': form,
        'import_result': None,
        'display_rows': [],
        'is_admin': is_super_admin(request.user, request),
    })


@login_required
@command_permission_required
@require_GET
def command_batch_template(request, cmd_id):
    """下载按命令字段生成的 CSV 模板。"""
    command = getattr(request, '_gmcommand', None) or GMCommand.objects.filter(command_id=cmd_id).first()
    if not command or not command.is_active:
        messages.error(request, _('该命令已被停用或删除'))
        return redirect('gmtool:command_list')

    content = build_template_csv(command)
    response = HttpResponse(content, content_type='text/csv; charset=utf-8')
    response['Content-Disposition'] = (
        f'attachment; filename="{command.command_id}_batch_template.csv"'
    )
    return response


@login_required
@require_POST
def batch_job_confirm(request, job_id):
    """确认草稿任务：需要审核则提交审核，否则立即执行。"""
    job = _get_job_for_user(job_id, request.user, request)
    client_ip = get_client_ip(request)

    if job.status != BatchExecuteJob.STATUS_DRAFT:
        messages.warning(request, _('该任务已确认，不能重复提交'))
        return redirect('gmtool:batch_job_detail', job_id=job.id)

    if job.command is None:
        messages.error(request, _('命令已被删除，无法执行批量任务'))
        return redirect('gmtool:batch_job_detail', job_id=job.id)

    if job.review_type:
        try:
            submit_batch_command_review(job, request.user, client_ip)
        except (CommandReviewActionError, DatabaseError) as exc:
            logger.exception('Batch review submission failed: job=%s', job.id)
            messages.error(request, str(exc) or _('提交审核失败，请稍后重试'))
        else:
            messages.success(request, _('批量任务已提交审核，请等待审核通过后执行'))
    else:
        try:
            execute_job(
                job,
                request.user,
                client_ip,
                allowed_statuses=FIRST_RUN_JOB_STATUSES,
            )
        except DatabaseError:
            logger.exception('Batch execution failed: job=%s', job.id)
            messages.error(request, _('批量任务执行失败，请稍后重试'))
        else:
            messages.success(request, _('批量任务已执行'))

    return redirect('gmtool:batch_job_detail', job_id=job.id)


@login_required
@require_POST
def batch_job_retry(request, job_id):
    """重试任务中的失败明细。"""
    job = _get_job_for_user(job_id, request.user, request)
    client_ip = get_client_ip(request)

    if job.status not in AUTO_RETRY_JOB_STATUSES:
        messages.warning(request, _('当前任务状态不支持重试'))
        return redirect('gmtool:batch_job_detail', job_id=job.id)

    result = retry_failed_items(job, request.user, client_ip)
    if not result.started:
        messages.warning(request, _('任务正在执行或状态已变化，请刷新后查看'))
    else:
        messages.success(request, _('已重试失败项，请查看最新结果'))
    return redirect('gmtool:batch_job_detail', job_id=job.id)


@login_required
@require_POST
def batch_job_resolve_unknown(request, job_id):
    """人工确认后重发结果未知的明细。"""
    job = _get_job_for_user(job_id, request.user, request)
    client_ip = get_client_ip(request)

    if job.status not in AUTO_RETRY_JOB_STATUSES:
        messages.warning(request, _('当前任务状态不支持重发'))
        return redirect('gmtool:batch_job_detail', job_id=job.id)

    result = confirm_unknown_and_retry(job, request.user, client_ip)
    if not result.started:
        messages.warning(request, _('任务正在执行或状态已变化，请刷新后查看'))
    else:
        messages.success(request, _('已重发结果未知的明细，请查看最新结果'))
    return redirect('gmtool:batch_job_detail', job_id=job.id)


@login_required
@require_POST
def batch_job_discard(request, job_id):
    """放弃草稿任务。"""
    job = _get_job_for_user(job_id, request.user, request)
    if job.status != BatchExecuteJob.STATUS_DRAFT:
        messages.warning(request, _('只有待确认的任务可以放弃'))
        return redirect('gmtool:batch_job_detail', job_id=job.id)

    job_id_value = job.id
    job.delete()
    messages.success(request, _('已放弃批量任务'))
    return redirect('gmtool:batch_job_list')


@login_required
def batch_job_detail(request, job_id):
    """批量任务详情与执行结果。"""
    job = _get_job_for_user(job_id, request.user, request, allow_reviewer=True)
    items = job.items.select_related('command_log').order_by('row_index')
    page_obj, elided_page_range = paginate_request_queryset(
        items,
        request,
        settings.PAGE_SIZE,
    )

    is_admin = is_super_admin(request.user, request)
    is_owner = is_admin or job.submitter_id == request.user.id

    can_confirm = is_owner and job.status == BatchExecuteJob.STATUS_DRAFT
    can_retry = (
        is_owner
        and job.status in AUTO_RETRY_JOB_STATUSES
        and job.failed_count > 0
    )
    can_resolve_unknown = (
        is_owner
        and job.status in AUTO_RETRY_JOB_STATUSES
        and job.unknown_count > 0
    )

    return render(request, 'gmtool/batch_job_detail.html', {
        'job': job,
        'page_obj': page_obj,
        'elided_page_range': elided_page_range,
        'can_confirm': can_confirm,
        'can_retry': can_retry,
        'can_resolve_unknown': can_resolve_unknown,
        'is_owner': is_owner,
        'is_admin': is_admin,
    })


@login_required
def batch_job_list(request):
    """批量任务列表。"""
    is_admin = is_super_admin(request.user, request)
    jobs = BatchExecuteJob.objects.select_related('command')
    if not is_admin:
        jobs = jobs.filter(submitter=request.user)

    status_filter = request.GET.get('status', '')
    if status_filter:
        jobs = jobs.filter(status=status_filter)

    jobs = jobs.order_by('-created_at')
    page_obj, elided_page_range = paginate_request_queryset(
        jobs,
        request,
        settings.PAGE_SIZE,
    )

    return render(request, 'gmtool/batch_job_list.html', {
        'page_obj': page_obj,
        'elided_page_range': elided_page_range,
        'status_filter': status_filter,
        'status_choices': BatchExecuteJob.STATUS_CHOICES,
        'is_admin': is_admin,
    })
