"""安全相关辅助函数。"""
import hashlib
import hmac
import re
import time

from django.conf import settings

# 脱敏字段（日志详情接口返回前处理）— 从 settings 获取，便于扩展
SENSITIVE_FIELD_NAMES = getattr(settings, 'SENSITIVE_FIELDS', {
    'password', 'passwd', 'pwd', 'token', 'access_token', 'refresh_token',
    'secret', 'sign', 'signature', 'sessionid', 'cookie', 'authorization',
})

# 删除确认令牌有效期（秒），5 分钟窗口
CONFIRM_TOKEN_TTL = 300


def make_confirm_token(obj_id, obj_name, ttl=CONFIRM_TOKEN_TTL):
    """生成删除确认令牌，使用 HMAC-SHA256 + 时间窗口防重放。"""
    timestamp = str(int(time.time() // ttl))
    raw = f'delete:{obj_id}:{obj_name}:{timestamp}'
    return hmac.new(
        settings.SECRET_KEY.encode(),
        raw.encode(),
        hashlib.sha256,
    ).hexdigest()[:32]



def verify_confirm_token(obj_id, obj_name, token, ttl=CONFIRM_TOKEN_TTL):
    """验证删除确认令牌。"""
    if not token:
        return False
    expected = make_confirm_token(obj_id, obj_name, ttl=ttl)
    return hmac.compare_digest(str(token), expected)



def get_client_ip(request):
    """获取客户端 IP 地址。"""
    if getattr(settings, 'TRUSTED_PROXY', False):
        x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
        if x_forwarded_for:
            ips = [ip.strip() for ip in x_forwarded_for.split(',') if ip.strip()]
            if ips:
                try:
                    proxy_count = max(int(getattr(settings, 'TRUSTED_PROXY_COUNT', 1)), 0)
                except (TypeError, ValueError):
                    proxy_count = 1

                client_index = len(ips) - proxy_count - 1
                if client_index >= 0:
                    return ips[client_index]
                return ips[0]
    return request.META.get('REMOTE_ADDR', '0.0.0.0')



def mask_sensitive_data(data):
    """递归脱敏日志中的敏感字段。"""
    if isinstance(data, dict):
        masked = {}
        for key, value in data.items():
            if str(key).lower() in SENSITIVE_FIELD_NAMES:
                masked[key] = '***'
            else:
                masked[key] = mask_sensitive_data(value)
        return masked
    if isinstance(data, list):
        return [mask_sensitive_data(item) for item in data]
    return data


def mask_sensitive_text(text):
    """对非结构化文本中的常见 key=value/key: value 敏感字段做脱敏。"""
    if not isinstance(text, str) or not text:
        return text

    field_names = sorted((str(name) for name in SENSITIVE_FIELD_NAMES if name), key=len, reverse=True)
    if not field_names:
        return text

    field_pattern = '|'.join(re.escape(name) for name in field_names)
    quoted_pattern = re.compile(
        rf'(["\']?(?:{field_pattern})["\']?\s*[:=]\s*["\'])(.*?)(["\'])',
        re.IGNORECASE,
    )
    unquoted_pattern = re.compile(
        rf'(\b(?:{field_pattern})\b\s*[:=]\s*)(?!["\'])([^&\s,;]+)',
        re.IGNORECASE,
    )

    masked = quoted_pattern.sub(lambda match: f'{match.group(1)}***{match.group(3)}', text)
    return unquoted_pattern.sub(lambda match: f'{match.group(1)}***', masked)
