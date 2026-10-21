"""批量命令执行服务。

设计要点：
- 明细级幂等：只执行 pending/failed，success 永不重跑；
- 超时/连接中断/响应不可解析一律记为 unknown，默认不自动重试，避免重复发放；
- 通过条件更新抢占任务，避免同一任务并发执行。
"""
import logging
import time
from dataclasses import dataclass

from django.conf import settings
from django.db import DatabaseError, transaction
from django.db.models import Count
from django.utils import timezone
from django.utils.translation import gettext_lazy as _

from .idip_client import send_idip_command
from .models import BatchExecuteItem, BatchExecuteJob, CommandLog

logger = logging.getLogger(__name__)

# 自动重试允许的任务状态
AUTO_RETRY_JOB_STATUSES = (
    BatchExecuteJob.STATUS_PARTIAL_FAILED,
    BatchExecuteJob.STATUS_FAILED,
)

# 首次执行（免审核命令确认导入，或整批审核通过）允许的任务状态
FIRST_RUN_JOB_STATUSES = (
    BatchExecuteJob.STATUS_DRAFT,
    BatchExecuteJob.STATUS_PENDING_REVIEW,
)

RUNNABLE_ITEM_STATUSES = (
    BatchExecuteItem.STATUS_PENDING,
    BatchExecuteItem.STATUS_FAILED,
)


@dataclass
class BatchExecuteResult:
    job: BatchExecuteJob
    started: bool
    executed_count: int = 0


def classify_result(response_data, error_message, error_type):
    """根据远端返回判定明细状态，返回 (item_status, error_text, command_log_status)。

    只有在拿到明确的业务结构时才判定成功/失败；其余无法确定结果的情况记为 unknown。
    """
    has_definitive_response = (
        isinstance(response_data, dict)
        and 'status' in response_data
        and 'json' in response_data
    )

    if has_definitive_response:
        if bool(response_data.get('status')):
            return BatchExecuteItem.STATUS_SUCCESS, '', 'success'

        wrapped_json = response_data.get('json')
        message = ''
        if isinstance(wrapped_json, dict):
            message = wrapped_json.get('RetMsg') or wrapped_json.get('retmsg') or ''
        return (
            BatchExecuteItem.STATUS_FAILED,
            message or str(_('命令执行失败')),
            'failed',
        )

    # 没有明确的业务响应：远端可能已执行成功，按未知处理，禁止自动重试
    message = error_message or str(_('未收到明确响应，执行结果未知'))
    log_status = 'timeout' if error_type == 'timeout' else 'failed'
    return BatchExecuteItem.STATUS_UNKNOWN, message, log_status


def _acquire_job(job_id, allowed_statuses):
    """条件更新抢占任务，返回是否抢占成功。"""
    updated = BatchExecuteJob.objects.filter(
        pk=job_id,
        status__in=allowed_statuses,
    ).update(status=BatchExecuteJob.STATUS_RUNNING)

    if not updated:
        return False

    BatchExecuteJob.objects.filter(
        pk=job_id,
        started_at__isnull=True,
    ).update(started_at=timezone.now())
    return True


def _write_command_log(job, command, item, user, ip_address, response_data, request_content, log_status):
    try:
        with transaction.atomic():
            return CommandLog.objects.create(
                user=user,
                operator_username=getattr(user, 'username', '') or '',
                command=command,
                partition=int((item.params or {}).get('Partition', 0) or 0),
                request_data=item.params or {},
                request_content=request_content or '',
                response_data=response_data,
                status=log_status,
                ip_address=ip_address,
            )
    except (DatabaseError, TypeError, ValueError):
        logger.exception(
            'Batch command log persistence failed: job=%s row=%s command=%s',
            job.id,
            item.row_index,
            getattr(command, 'command_id', ''),
        )
        return None


def _execute_item(job, command, item, user, ip_address):
    item.status = BatchExecuteItem.STATUS_RUNNING
    item.attempt_count += 1
    item.save(update_fields=['status', 'attempt_count', 'updated_at'])

    response_data, error_message, request_content, error_type = send_idip_command(
        command,
        dict(item.params or {}),
    )
    item_status, error_text, log_status = classify_result(
        response_data,
        error_message,
        error_type,
    )

    command_log = _write_command_log(
        job,
        command,
        item,
        user,
        ip_address,
        response_data,
        request_content,
        log_status,
    )

    item.status = item_status
    item.response_data = response_data
    item.error_message = error_text
    item.command_log = command_log
    item.executed_at = timezone.now()
    item.save(update_fields=[
        'status',
        'response_data',
        'error_message',
        'command_log',
        'executed_at',
        'updated_at',
    ])


def _finalize_job(job):
    counts = {
        row['status']: row['count']
        for row in job.items.values('status').annotate(count=Count('id'))
    }
    success = counts.get(BatchExecuteItem.STATUS_SUCCESS, 0)
    failed = counts.get(BatchExecuteItem.STATUS_FAILED, 0)
    unknown = counts.get(BatchExecuteItem.STATUS_UNKNOWN, 0)
    remaining = (
        counts.get(BatchExecuteItem.STATUS_PENDING, 0)
        + counts.get(BatchExecuteItem.STATUS_RUNNING, 0)
    )

    job.success_count = success
    job.failed_count = failed
    job.unknown_count = unknown

    if remaining:
        job.status = BatchExecuteJob.STATUS_RUNNING
        job.finished_at = None
    elif success == job.total_items:
        job.status = BatchExecuteJob.STATUS_COMPLETED
        job.finished_at = timezone.now()
    elif success > 0:
        job.status = BatchExecuteJob.STATUS_PARTIAL_FAILED
        job.finished_at = timezone.now()
    else:
        job.status = BatchExecuteJob.STATUS_FAILED
        job.finished_at = timezone.now()

    job.save(update_fields=[
        'status',
        'success_count',
        'failed_count',
        'unknown_count',
        'finished_at',
        'updated_at',
    ])


def execute_job(job, user, ip_address, *, allowed_statuses):
    """执行任务中待执行/失败的明细，并汇总任务状态。"""
    if not _acquire_job(job.id, allowed_statuses):
        job.refresh_from_db()
        return BatchExecuteResult(job=job, started=False)

    job.refresh_from_db()

    items = list(
        job.items.filter(status__in=RUNNABLE_ITEM_STATUSES).order_by('row_index')
    )
    executed_count = 0

    if not items:
        _finalize_job(job)
        job.refresh_from_db()
        return BatchExecuteResult(job=job, started=True, executed_count=0)

    command = job.command
    if command is None:
        # 命令已被删除，无法继续执行
        job.items.filter(status__in=RUNNABLE_ITEM_STATUSES).update(
            status=BatchExecuteItem.STATUS_FAILED,
            error_message=str(_('命令已被删除，无法执行')),
            executed_at=timezone.now(),
        )
        _finalize_job(job)
        job.refresh_from_db()
        return BatchExecuteResult(job=job, started=True, executed_count=0)

    interval_seconds = max(
        getattr(settings, 'BATCH_EXECUTE_INTERVAL_MS', 100),
        0,
    ) / 1000.0

    for index, item in enumerate(items):
        _execute_item(job, command, item, user, ip_address)
        executed_count += 1
        if interval_seconds and index < len(items) - 1:
            time.sleep(interval_seconds)

    _finalize_job(job)
    job.refresh_from_db()

    logger.info(
        'Batch execute finished: job=%s command=%s total=%s success=%s failed=%s unknown=%s',
        job.id,
        job.command_code,
        job.total_items,
        job.success_count,
        job.failed_count,
        job.unknown_count,
    )

    return BatchExecuteResult(job=job, started=True, executed_count=executed_count)


def retry_failed_items(job, user, ip_address):
    """重试任务中的失败明细；success/unknown 不会被自动重跑。"""
    if job.status not in AUTO_RETRY_JOB_STATUSES:
        job.refresh_from_db()
        return BatchExecuteResult(job=job, started=False)
    return execute_job(job, user, ip_address, allowed_statuses=AUTO_RETRY_JOB_STATUSES)


def confirm_unknown_and_retry(job, user, ip_address):
    """人工确认后，将 unknown 明细重置为 pending 再执行。"""
    if job.status not in AUTO_RETRY_JOB_STATUSES:
        job.refresh_from_db()
        return BatchExecuteResult(job=job, started=False)

    job.items.filter(status=BatchExecuteItem.STATUS_UNKNOWN).update(
        status=BatchExecuteItem.STATUS_PENDING,
        error_message='',
        updated_at=timezone.now(),
    )
    job.refresh_from_db()
    return execute_job(job, user, ip_address, allowed_statuses=AUTO_RETRY_JOB_STATUSES)
