"""gmtool 模板上下文处理器。"""

from .command_review_services import has_command_review_type_permission
from .decorators import has_announcement_permission, is_super_admin
from .models import CommandReview


def gmtool_permissions(request):
    """提供导航和模板常用权限变量。"""
    user = getattr(request, 'user', None)
    if user is None or not user.is_authenticated:
        return {
            'is_admin': False,
            'has_announcement_permission': False,
            'has_mail_review_permission': False,
            'has_marquee_review_permission': False,
            'has_review_permission': False,
        }

    has_announcements = has_announcement_permission(user, request)
    has_mail_reviews = has_command_review_type_permission(user, CommandReview.TYPE_MAIL, request)
    has_marquee_reviews = has_command_review_type_permission(user, CommandReview.TYPE_MARQUEE, request)
    return {
        'is_admin': is_super_admin(user, request),
        'has_announcement_permission': has_announcements,
        'has_mail_review_permission': has_mail_reviews,
        'has_marquee_review_permission': has_marquee_reviews,
        'has_review_permission': has_announcements or has_mail_reviews or has_marquee_reviews,
    }
