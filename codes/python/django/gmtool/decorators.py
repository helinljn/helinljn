"""权限装饰器"""
import functools

from django.http import JsonResponse
from django.shortcuts import redirect

from .models import GMCommand, UserProfile, UserCommandPermission


def is_super_admin(user):
    """
    判断用户是否为超级管理员。
    优先检查 Django 内置 is_superuser，同时检查 GM 系统角色。
    两者任一为 True 即视为超级管理员，确保唯一绑定。
    """
    if not user.is_authenticated:
        return False
    # Django 内置超级管理员直接视为 GM 超级管理员
    if user.is_superuser:
        return True
    try:
        profile = user.userprofile
        return profile.role is not None and profile.role.is_super_admin
    except UserProfile.DoesNotExist:
        return False


def super_admin_required(view_func):
    """
    超级管理员权限装饰器。
    未登录跳转登录页，非超级管理员返回仪表盘。
    """
    @functools.wraps(view_func)
    def wrapper(request, *args, **kwargs):
        if not request.user.is_authenticated:
            return redirect('gmtool:login')
        if is_super_admin(request.user):
            return view_func(request, *args, **kwargs)
        return redirect('gmtool:dashboard')
    return wrapper


def command_permission_required(view_func):
    """
    命令权限装饰器。
    检查当前用户是否拥有指定命令的执行权限，超级管理员自动通过。
    用于AJAX请求时返回JSON错误。
    """
    @functools.wraps(view_func)
    def wrapper(request, *args, **kwargs):
        if not request.user.is_authenticated:
            return JsonResponse({'error': '请先登录'}, status=401)
        # 超级管理员自动通过
        if is_super_admin(request.user):
            return view_func(request, *args, **kwargs)
        # 检查用户是否有该命令的直接权限
        cmd_id = kwargs.get('cmd_id')
        if cmd_id and UserCommandPermission.objects.filter(
            user=request.user,
            command__command_id=cmd_id,
            command__is_active=True,
        ).exists():
            return view_func(request, *args, **kwargs)
        return JsonResponse({'error': '您没有执行该命令的权限'}, status=403)
    return wrapper


def get_user_permissions(user):
    """
    获取用户可执行的命令ID列表。
    超级管理员(Django is_superuser 或 GM角色)返回所有活跃命令。
    普通用户从 UserCommandPermission 读取直接分配的权限。
    """
    # Django 超级管理员 或 GM 超级管理员角色 → 所有活跃命令
    if is_super_admin(user):
        return list(GMCommand.objects.filter(is_active=True).values_list('command_id', flat=True))
    # 普通用户：直接权限
    return list(UserCommandPermission.objects.filter(
        user=user,
        command__is_active=True,
    ).values_list('command__command_id', flat=True))
