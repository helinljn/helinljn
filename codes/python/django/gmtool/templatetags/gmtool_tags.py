"""GM工具自定义模板标签"""
import json

from django import template
from django.utils.html import escape

from gmtool.security_utils import make_confirm_token

register = template.Library()


@register.simple_tag
def confirm_token(obj_id, obj_name):
    """生成删除确认令牌。"""
    return make_confirm_token(obj_id, obj_name)


@register.filter
def json_compact(value):
    """将字典/列表紧凑渲染为单行 JSON 字符串。"""
    try:
        return json.dumps(value, ensure_ascii=False, separators=(',', ':'))
    except (TypeError, ValueError):
        return str(value)


@register.filter
def json_compact_escaped(value):
    """紧凑 JSON 并做 HTML 转义，便于直接放进 pre/code。"""
    return escape(json_compact(value))
