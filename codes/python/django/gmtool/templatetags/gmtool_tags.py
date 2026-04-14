"""GM工具自定义模板标签"""
import hashlib

from django import template
from django.conf import settings

register = template.Library()


@register.simple_tag
def confirm_token(obj_id, obj_name):
    """生成删除确认令牌，防止绕过前端直接POST删除"""
    raw = f'{settings.SECRET_KEY}:delete:{obj_id}:{obj_name}'
    return hashlib.sha256(raw.encode()).hexdigest()[:16]
