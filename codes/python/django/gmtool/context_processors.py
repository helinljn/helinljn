"""gmtool 模板上下文处理器。"""

from .decorators import has_announcement_permission, is_super_admin


def gmtool_permissions(request):
    """提供导航和模板常用权限变量。"""
    user = getattr(request, 'user', None)
    if user is None or not user.is_authenticated:
        return {
            'is_admin': False,
            'has_announcement_permission': False,
        }

    return {
        'is_admin': is_super_admin(user, request),
        'has_announcement_permission': has_announcement_permission(user, request),
    }
