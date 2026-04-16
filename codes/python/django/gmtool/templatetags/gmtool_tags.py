"""GM工具自定义模板标签"""
from django import template

from gmtool.security_utils import make_confirm_token

register = template.Library()


@register.simple_tag
def confirm_token(obj_id, obj_name):
    """生成删除确认令牌。"""
    return make_confirm_token(obj_id, obj_name)
