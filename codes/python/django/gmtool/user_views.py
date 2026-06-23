"""用户管理相关视图"""
import logging

from django.conf import settings as django_settings
from django.contrib import messages
from django.contrib.auth import get_user_model
from django.contrib.auth.decorators import login_required
from django.db import transaction
from django.db.models import Count
from django.shortcuts import get_object_or_404, redirect, render
from django.utils.translation import gettext_lazy as _
from django.views.decorators.http import require_POST

from .audit_log import log_operation
from .decorators import is_super_admin, super_admin_required
from .forms import UserCreateForm, UserEditForm
from .models import GMCommand, LoginLog, UserAnnouncementPermission, UserCommandPermission, UserProfile
from .query_utils import apply_time_range_filters_to_queryset, paginate_request_queryset
from .security_utils import get_client_ip, verify_confirm_token

User = get_user_model()
logger = logging.getLogger(__name__)


@login_required
@super_admin_required
def user_list(request):
    """用户列表管理"""
    users = User.objects.select_related('userprofile').annotate(
        perm_count=Count('usercommandpermission', distinct=True)
    ).all()

    # 搜索
    search = request.GET.get('search', '')
    if search:
        users = users.filter(username__icontains=search)

    # 添加排序以避免分页警告，按用户ID升序排列
    users = users.order_by('id')

    page_obj, elided_page_range = paginate_request_queryset(users, request, django_settings.PAGE_SIZE)

    return render(request, 'gmtool/user_list.html', {
        'page_obj': page_obj,
        'elided_page_range': elided_page_range,
        'search': search,
        'is_admin': True,
    })


@login_required
@super_admin_required
def user_create(request):
    """创建用户"""
    if request.method == 'POST':
        form = UserCreateForm(request.POST)
        if form.is_valid():
            new_user = form.save()
            log_operation(
                'user',
                'create',
                user=request.user,
                ip_address=get_client_ip(request),
                detail={
                    'target_user': new_user.username,
                    'is_active': new_user.is_active,
                    'phone': form.cleaned_data.get('phone', ''),
                },
            )
            return redirect('gmtool:user_list')
    else:
        form = UserCreateForm()

    return render(request, 'gmtool/user_form.html', {
        'form': form,
        'title': _('创建用户'),
        'is_admin': True,
    })


@login_required
@super_admin_required
def user_edit(request, user_id):
    """编辑用户"""
    user_obj = get_object_or_404(User, pk=user_id)
    UserProfile.objects.get_or_create(user=user_obj)

    is_self = user_obj == request.user
    is_target_super_admin = is_super_admin(user_obj, request)

    if request.method == 'POST':
        form = UserEditForm(
            request.POST,
            instance=user_obj,
            is_self=is_self,
            is_target_super_admin=is_target_super_admin,
        )
        if form.is_valid():
            # 禁止禁用自己的账号
            if is_self and not form.cleaned_data.get('is_active', True):
                form.add_error('is_active', _('不能禁用自己的账号'))
            else:
                form.save()
                log_operation(
                    'user',
                    'update',
                    user=request.user,
                    ip_address=get_client_ip(request),
                    detail={
                        'target_user': user_obj.username,
                        'target_id': user_id,
                        'is_active': form.cleaned_data.get('is_active'),
                        'phone': form.cleaned_data.get('phone', ''),
                        'password_changed': bool(form.cleaned_data.get('new_password')),
                    },
                )
                return redirect('gmtool:user_list')
    else:
        form = UserEditForm(
            instance=user_obj,
            is_self=is_self,
            is_target_super_admin=is_target_super_admin,
        )

    return render(request, 'gmtool/user_form.html', {
        'form': form,
        'title': _('编辑用户'),
        'edit_user': user_obj,
        'is_self': is_self,
        'is_target_super_admin': is_target_super_admin,
        'is_admin': True,
    })


@login_required
@super_admin_required
@require_POST
def user_delete(request, user_id):
    """删除用户"""
    user_obj = get_object_or_404(User, pk=user_id)
    if user_obj == request.user:
        messages.warning(request, _('不能删除自己的账号'))
    elif is_super_admin(user_obj, request):
        messages.warning(request, _('不能删除超级管理员账号'))
    else:
        # 验证确认令牌，防止绕过前端直接 POST 删除
        confirm_token = request.POST.get('confirm_token', '')
        if not verify_confirm_token(user_id, user_obj.username, confirm_token):
            messages.error(request, _('删除确认失败，请重试'))
            return redirect('gmtool:user_list')
        target_name = user_obj.username
        user_obj.delete()
        log_operation(
            'user',
            'delete',
            user=request.user,
            ip_address=get_client_ip(request),
            detail={'target_user': target_name, 'target_id': user_id},
        )
    return redirect('gmtool:user_list')


@login_required
@super_admin_required
def user_permissions(request, user_id):
    """用户权限分配（针对每个用户单独设置）"""
    user_obj = get_object_or_404(User, pk=user_id)
    # 超级管理员不需要手动分配权限
    if is_super_admin(user_obj, request):
        return redirect('gmtool:user_list')

    commands = GMCommand.objects.filter(is_active=True).only(
        'id', 'command_id', 'command_name', 'tab'
    )

    if request.method == 'POST':
        selected_ids = request.POST.getlist('commands')
        wants_announcement_permission = request.POST.get('announcement_permission') == 'on'
        # 记录变更前的权限
        old_perm_ids = set(UserCommandPermission.objects.filter(
            user=user_obj
        ).values_list('command_id', flat=True))
        old_has_announcement_permission = UserAnnouncementPermission.objects.filter(user=user_obj).exists()

        # 校验：仅允许分配活跃命令的权限，忽略非法ID避免500
        valid_ids = []
        invalid_ids = []
        for cid in selected_ids:
            try:
                valid_ids.append(int(cid))
            except (TypeError, ValueError):
                invalid_ids.append(cid)
        # 用一条 IN 查询替代全量获取
        active_command_ids = set(
            GMCommand.objects.filter(id__in=valid_ids, is_active=True).values_list('id', flat=True)
        )
        valid_ids = [cid for cid in valid_ids if cid in active_command_ids]

        if invalid_ids:
            logger.warning(
                'Invalid command IDs received in permission assignment: operator=%s, target_user_id=%s, invalid_ids=%s',
                request.user.username,
                user_id,
                invalid_ids,
            )

        # 差集模式更新权限（事务保护）：仅删除需移除的，仅创建需新增的
        with transaction.atomic():
            valid_ids_set = set(valid_ids)
            old_perm_ids_set = set(old_perm_ids)
            to_remove = old_perm_ids_set - valid_ids_set
            to_add = valid_ids_set - old_perm_ids_set

            if to_remove:
                UserCommandPermission.objects.filter(
                    user=user_obj, command_id__in=to_remove
                ).delete()
            if to_add:
                new_user_perms = [
                    UserCommandPermission(user=user_obj, command_id=cmd_id)
                    for cmd_id in to_add
                ]
                UserCommandPermission.objects.bulk_create(new_user_perms)

            if wants_announcement_permission:
                UserAnnouncementPermission.objects.get_or_create(user=user_obj)
            else:
                UserAnnouncementPermission.objects.filter(user=user_obj).delete()

        new_perm_ids = set(valid_ids)
        new_has_announcement_permission = UserAnnouncementPermission.objects.filter(user=user_obj).exists()
        log_operation(
            'permission',
            'assign',
            user=request.user,
            ip_address=get_client_ip(request),
            detail={
                'target_user': user_obj.username,
                'target_user_id': user_id,
                'added': list(new_perm_ids - old_perm_ids),
                'removed': list(old_perm_ids - new_perm_ids),
                'total': len(new_perm_ids),
                'announcement_permission_before': old_has_announcement_permission,
                'announcement_permission_after': new_has_announcement_permission,
                'announcement_permission_changed': old_has_announcement_permission != new_has_announcement_permission,
            },
        )
        return redirect('gmtool:user_list')

    # 获取当前用户的命令权限
    permitted_ids = set(UserCommandPermission.objects.filter(
        user=user_obj
    ).values_list('command_id', flat=True))
    target_has_announcement_permission = UserAnnouncementPermission.objects.filter(user=user_obj).exists()

    # 按 tab 分组
    tab_groups = {}
    for cmd in commands:
        tab = cmd.tab
        if tab not in tab_groups:
            tab_groups[tab] = []
        tab_groups[tab].append({
            'id': cmd.id,
            'command_id': cmd.command_id,
            'command_name': cmd.command_name,
            'permitted': cmd.id in permitted_ids,
        })

    return render(request, 'gmtool/user_permissions.html', {
        'target_user': user_obj,
        'tab_groups': tab_groups,
        'target_has_announcement_permission': target_has_announcement_permission,
        'is_admin': True,
    })


@login_required
@super_admin_required
def login_log_list(request):
    """登录日志列表"""
    logs = LoginLog.objects.select_related('user').all()

    # 筛选
    action_filter = request.GET.get('action', '')
    if action_filter:
        logs = logs.filter(action=action_filter)

    user_filter = request.GET.get('user', '')
    if user_filter:
        logs = logs.filter(username__icontains=user_filter)

    logs, start_time_filter, end_time_filter = apply_time_range_filters_to_queryset(logs, request)

    # 添加排序以避免分页警告
    logs = logs.order_by('-created_at')

    page_obj, elided_page_range = paginate_request_queryset(logs, request, django_settings.PAGE_SIZE)

    return render(request, 'gmtool/login_log.html', {
        'page_obj': page_obj,
        'elided_page_range': elided_page_range,
        'action_filter': action_filter,
        'user_filter': user_filter,
        'start_time_filter': start_time_filter,
        'end_time_filter': end_time_filter,
        'is_admin': True,
    })
