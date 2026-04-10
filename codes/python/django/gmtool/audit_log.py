"""操作审计日志模块 — 将用户操作统一记录到本地日志文件"""
import logging
import json

audit_logger = logging.getLogger('gmtool.audit')


def log_operation(category, action, user=None, detail=None, ip_address=None):
    """
    记录用户操作审计日志。

    Args:
        category: 操作分类，如 auth/user/role/permission/command
        action: 操作动作，如 login/logout/create/update/delete/assign
        user: 操作用户对象（Django User），未登录时为 None
        detail: 操作详情（dict 或 str）
        ip_address: 客户端 IP 地址
    """
    username = getattr(user, 'username', 'anonymous') if user else 'anonymous'
    if isinstance(detail, dict):
        detail = json.dumps(detail, ensure_ascii=False, default=str)
    detail = detail or ''

    audit_logger.info(
        f'[{category}] {action} | user={username} | ip={ip_address or "-"} | {detail}'
    )
