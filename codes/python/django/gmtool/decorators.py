"""权限装饰器"""
import functools

from django.contrib import messages
from django.http import JsonResponse
from django.shortcuts import redirect
from django.utils.translation import gettext_lazy as _

from .models import GMCommand, UserCommandPermission
from .utils import is_super_admin_user


def is_super_admin(user, request=None):
    """
    判断用户是否为超级管理员。
    仅检查 Django 内置 is_superuser，并支持请求级缓存。
    """
    if not user.is_authenticated:
        return False

    # 请求级缓存：基于 user.id 的字典，避免不同用户的缓存互相覆盖
    if request:
        cache = getattr(request, '_super_admin_cache', None)
        if cache is not None and user.id in cache:
            return cache[user.id]

    result = is_super_admin_user(user)

    if request:
        if not hasattr(request, '_super_admin_cache'):
            request._super_admin_cache = {}
        request._super_admin_cache[user.id] = result
    return result


def super_admin_required(view_func):
    """
    超级管理员权限装饰器。
    未登录跳转登录页，非超级管理员：
    - AJAX 请求返回 JSON 错误
    - 普通请求返回仪表盘并提示
    """
    @functools.wraps(view_func)
    def wrapper(request, *args, **kwargs):
        if not request.user.is_authenticated:
            if request.headers.get('X-Requested-With') == 'XMLHttpRequest':
                return JsonResponse({'error': _('请先登录')}, status=401)
            return redirect('gmtool:login')
        if is_super_admin(request.user, request):
            return view_func(request, *args, **kwargs)
        if request.headers.get('X-Requested-With') == 'XMLHttpRequest':
            return JsonResponse({'error': _('您没有管理员权限')}, status=403)
        messages.warning(request, _('您没有管理员权限'))
        return redirect('gmtool:dashboard')
    return wrapper


def command_permission_required(view_func):
    """
    命令权限装饰器。
    检查当前用户是否拥有指定命令的执行权限，超级管理员自动通过。
    用于AJAX请求时返回JSON错误。
    区分"权限不足"和"命令已停用"两种情况。
    权限校验通过后，将查询到的 command 对象附加到 request._gmcommand，避免视图重复查询。
    """
    @functools.wraps(view_func)
    def wrapper(request, *args, **kwargs):
        if not request.user.is_authenticated:
            return JsonResponse({'error': _('请先登录')}, status=401)

        cmd_id = kwargs.get('cmd_id')
        if cmd_id:
            # 所有用户（含超管）都需检查命令是否存在且活跃
            cmd = GMCommand.objects.filter(command_id=cmd_id).first()
            if not cmd or not cmd.is_active:
                return JsonResponse(
                    {'error': _('该命令已被停用或删除'), 'command_deactivated': True},
                    status=410,
                )

            # 超级管理员自动通过
            if is_super_admin(request.user, request):
                request._gmcommand = cmd
                return view_func(request, *args, **kwargs)

            # 检查用户是否有该命令的直接权限（同时确认命令对象关联）
            perm = UserCommandPermission.objects.filter(
                user=request.user,
                command=cmd,
            ).select_related('command').first()
            if perm:
                # 将已查询的 command 对象缓存到 request，避免视图重复查询
                request._gmcommand = perm.command
                return view_func(request, *args, **kwargs)
        return JsonResponse({'error': _('您没有执行该命令的权限')}, status=403)
    return wrapper


def get_user_permissions(user):
    """
    获取用户可执行的命令ID列表。
    超级管理员（Django is_superuser）返回所有活跃命令。
    普通用户从 UserCommandPermission 读取直接分配的权限。
    """
    if is_super_admin(user):  # get_user_permissions 通常不在请求上下文中调用，不传 request
        return list(GMCommand.objects.filter(is_active=True).values_list('command_id', flat=True))
    return list(UserCommandPermission.objects.filter(
        user=user,
        command__is_active=True,
    ).values_list('command__command_id', flat=True))
