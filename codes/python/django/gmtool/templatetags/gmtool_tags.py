"""GM工具自定义模板标签"""
import hashlib
import hmac
import time

from django import template
from django.conf import settings

register = template.Library()

# 确认令牌有效期（秒），5 分钟窗口 — 需与 user_views.py 保持一致
_CONFIRM_TOKEN_TTL = 300


@register.simple_tag
def confirm_token(obj_id, obj_name):
    """生成删除确认令牌，使用 HMAC-SHA256 + 时间窗口防重放"""
    timestamp = str(int(time.time() // _CONFIRM_TOKEN_TTL))
    raw = f'delete:{obj_id}:{obj_name}:{timestamp}'
    return hmac.new(
        settings.SECRET_KEY.encode(),
        raw.encode(),
        hashlib.sha256,
    ).hexdigest()[:32]
