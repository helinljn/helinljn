"""权限装饰器"""
import functools

from django.contrib import messages
from django.http import JsonResponse
from django.shortcuts import redirect
from django.utils.translation import gettext_lazy as _

from .models import GMCommand, UserAnnouncementPermission, UserCommandPermission
from .permission_service import is_super_admin_user


def _wants_json_response(request):
    """判断当前请求更适合返回 JSON 还是页面跳转。"""
    accept = (request.headers.get('Accept') or '').lower()
    return (
        request.method != 'GET'
        or request.path.startswith('/gmtool/api/')
        or request.headers.get('X-Requested-With') == 'XMLHttpRequest'
        or 'application/json' in accept
    )


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
    未登录跳转登录页，非超级管理员时：
    - JSON / POST / AJAX 请求返回 JSON 错误
    - 普通页面请求跳转并提示
    """
    @functools.wraps(view_func)
    def wrapper(request, *args, **kwargs):
        if not request.user.is_authenticated:
            if _wants_json_response(request):
                return JsonResponse({'error': _('请先登录')}, status=401)
            return redirect('gmtool:login')
        if is_super_admin(request.user, request):
            return view_func(request, *args, **kwargs)
        if _wants_json_response(request):
            return JsonResponse({'error': _('您没有管理员权限')}, status=403)
        messages.warning(request, _('您没有管理员权限'))
        return redirect('gmtool:dashboard')
    return wrapper


def command_permission_required(view_func):
    """
    命令权限装饰器。
    检查当前用户是否拥有指定命令的执行权限，超级管理员自动通过。
    权限校验通过后，将查询到的 command 对象附加到 request._gmcommand，避免视图重复查询。
    """
    @functools.wraps(view_func)
    def wrapper(request, *args, **kwargs):
        if not request.user.is_authenticated:
            if _wants_json_response(request):
                return JsonResponse({'error': _('请先登录')}, status=401)
            return redirect('gmtool:login')

        cmd_id = kwargs.get('cmd_id')
        if cmd_id:
            # 所有用户（含超管）都需检查命令是否存在且活跃
            cmd = GMCommand.objects.filter(command_id=cmd_id).first()
            if not cmd or not cmd.is_active:
                if _wants_json_response(request):
                    return JsonResponse(
                        {'error': _('该命令已被停用或删除'), 'command_deactivated': True},
                        status=410,
                    )
                messages.error(request, _('该命令已被停用或删除'))
                return redirect('gmtool:command_list')

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
                request._gmcommand = perm.command
                return view_func(request, *args, **kwargs)

        if _wants_json_response(request):
            return JsonResponse({'error': _('您没有执行该命令的权限')}, status=403)
        messages.warning(request, _('您没有执行该命令的权限'))
        return redirect('gmtool:command_list')
    return wrapper


def has_announcement_permission(user, request=None):
    """判断用户是否拥有公告管理权限，超级管理员自动拥有。"""
    if not user.is_authenticated:
        return False

    if is_super_admin(user, request):
        return True

    if request:
        cache = getattr(request, '_announcement_permission_cache', None)
        if cache is not None and user.id in cache:
            return cache[user.id]

    result = UserAnnouncementPermission.objects.filter(user=user).exists()

    if request:
        if not hasattr(request, '_announcement_permission_cache'):
            request._announcement_permission_cache = {}
        request._announcement_permission_cache[user.id] = result
    return result


def announcement_permission_required(view_func):
    """
    公告管理权限装饰器。
    未登录跳转登录页或返回 JSON 401；超级管理员和拥有公告权限的普通用户可访问。
    """
    @functools.wraps(view_func)
    def wrapper(request, *args, **kwargs):
        if not request.user.is_authenticated:
            if _wants_json_response(request):
                return JsonResponse({'error': _('请先登录')}, status=401)
            return redirect('gmtool:login')

        if has_announcement_permission(request.user, request):
            return view_func(request, *args, **kwargs)

        if _wants_json_response(request):
            return JsonResponse({'error': _('您没有公告管理权限')}, status=403)
        messages.warning(request, _('您没有公告管理权限'))
        return redirect('gmtool:dashboard')
    return wrapper
