"""认证相关视图"""
import logging

from django.conf import settings
from django.contrib.auth import authenticate, login
from django.contrib.auth import logout as auth_logout
from django.contrib.auth.decorators import login_required
from django.core.cache import cache
from django.shortcuts import redirect, render
from django.views.decorators.http import require_POST
from django.utils.http import url_has_allowed_host_and_scheme
from django.utils.translation import gettext_lazy as _

from .audit_log import log_operation
from .models import LoginLog

logger = logging.getLogger(__name__)

# 登录失败限速配置
LOGIN_MAX_ATTEMPTS = getattr(settings, 'LOGIN_MAX_ATTEMPTS', 5)          # 最大尝试次数
LOGIN_LOCKOUT_SECONDS = getattr(settings, 'LOGIN_LOCKOUT_SECONDS', 300)  # 锁定时长（秒），5分钟
LOGIN_THROTTLE_CACHE_PREFIX = 'gmtool:login_throttle:'


def _normalize_login_username(username):
    """规范化登录用户名，用于构建限速键。"""
    return (username or '').strip().lower()


def _build_login_throttle_cache_keys(ip, username):
    """构建登录限速键：IP 维度 + IP+用户名维度。"""
    normalized = _normalize_login_username(username)
    key_ip = f'{LOGIN_THROTTLE_CACHE_PREFIX}ip:{ip}'
    key_ip_user = f'{LOGIN_THROTTLE_CACHE_PREFIX}ip_user:{ip}:{normalized}'
    return key_ip, key_ip_user


def _get_safe_next_url(request):
    """获取安全的登录后跳转地址，仅允许本站内相对路径"""
    next_url = request.GET.get('next', '')
    if not next_url:
        return ''

    # 禁止 scheme-relative URL（如 //evil.com）
    if next_url.startswith('//'):
        return ''

    # 仅允许当前主机，且 HTTPS 请求下要求 https
    if not url_has_allowed_host_and_scheme(
        next_url,
        allowed_hosts={request.get_host()},
        require_https=request.is_secure(),
    ):
        return ''

    # 仅允许相对路径跳转，避免外部跳转风险
    if not next_url.startswith('/'):
        return ''

    return next_url


def get_client_ip(request):
    """获取客户端IP地址

    仅在配置了可信反向代理（TRUSTED_PROXY=True）时才使用 X-Forwarded-For，
    否则直接使用 REMOTE_ADDR，防止客户端伪造 IP。
    """
    from django.conf import settings
    if getattr(settings, 'TRUSTED_PROXY', False):
        x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
        if x_forwarded_for:
            return x_forwarded_for.split(',')[0].strip()
    return request.META.get('REMOTE_ADDR', '0.0.0.0')


def login_view(request):
    """登录页面"""
    if request.user.is_authenticated:
        return render(request, 'gmtool/login.html', {
            'already_logged_in': True,
            'current_user': request.user,
        })
    if request.method == 'POST':
        ip = get_client_ip(request)
        username = request.POST.get('username', '')
        password = request.POST.get('password', '')

        # 登录失败限速检查（IP + username 双维度）
        cache_key_ip, cache_key_ip_user = _build_login_throttle_cache_keys(ip, username)
        attempts_ip = cache.get(cache_key_ip, 0)
        attempts_ip_user = cache.get(cache_key_ip_user, 0)
        attempts = max(attempts_ip, attempts_ip_user)
        if attempts >= LOGIN_MAX_ATTEMPTS:
            LoginLog.objects.create(
                user=None, username=username,
                action='login_failed', ip_address=ip,
                user_agent=request.META.get('HTTP_USER_AGENT', '')[:500],
                reason=_('尝试次数过多'),
            )
            return render(request, 'gmtool/login.html', {
                'error': _('登录尝试次数过多，请 %(minutes)d 分钟后再试') % {'minutes': LOGIN_LOCKOUT_SECONDS // 60},
            })

        user = authenticate(request, username=username, password=password)
        ua = request.META.get('HTTP_USER_AGENT', '')[:500]
        if user is not None:
            if user.is_active:
                login(request, user)
                # 登录成功，清除失败计数（双维度）
                cache.delete(cache_key_ip)
                cache.delete(cache_key_ip_user)
                LoginLog.objects.create(
                    user=user, username=username, action='login',
                    ip_address=ip, user_agent=ua,
                )
                log_operation('auth', 'login', user=user, ip_address=ip,
                              detail={'username': username})
                next_url = _get_safe_next_url(request)
                if next_url:
                    return redirect(next_url)
                return redirect('gmtool:dashboard')
            else:
                # 登录失败，累加计数（双维度）
                next_attempts = attempts + 1
                cache.set(cache_key_ip, next_attempts, timeout=LOGIN_LOCKOUT_SECONDS)
                cache.set(cache_key_ip_user, next_attempts, timeout=LOGIN_LOCKOUT_SECONDS)
                LoginLog.objects.create(
                    user=None, username=username, action='login_failed',
                    ip_address=ip, user_agent=ua, reason=_('账号已禁用'),
                )
                log_operation('auth', 'login_failed', ip_address=ip,
                              detail={'username': username, 'reason': 'Account disabled'})
                return render(request, 'gmtool/login.html', {'error': _('账号已被禁用')})
        else:
            # 登录失败，累加计数（双维度）
            next_attempts = attempts + 1
            cache.set(cache_key_ip, next_attempts, timeout=LOGIN_LOCKOUT_SECONDS)
            cache.set(cache_key_ip_user, next_attempts, timeout=LOGIN_LOCKOUT_SECONDS)
            remaining = LOGIN_MAX_ATTEMPTS - next_attempts
            LoginLog.objects.create(
                user=None, username=username, action='login_failed',
                ip_address=ip, user_agent=ua, reason=_('用户名或密码错误'),
            )
            log_operation('auth', 'login_failed', ip_address=ip,
                          detail={'username': username, 'reason': 'Incorrect username or password'})
            if remaining > 0:
                error_msg = _('用户名或密码错误，还可尝试 %(remaining)d 次') % {'remaining': remaining}
            else:
                error_msg = _('登录尝试次数过多，请 %(minutes)d 分钟后再试') % {'minutes': LOGIN_LOCKOUT_SECONDS // 60}
            return render(request, 'gmtool/login.html', {'error': error_msg})
    return render(request, 'gmtool/login.html')


@login_required
@require_POST
def logout_view(request):
    """登出（仅允许POST，防止CSRF强制登出）"""
    ip = get_client_ip(request)
    log_operation('auth', 'logout', user=request.user, ip_address=ip,
                  detail={'username': request.user.username})
    LoginLog.objects.create(
        user=request.user, username=request.user.username, action='logout',
        ip_address=ip,
        user_agent=request.META.get('HTTP_USER_AGENT', '')[:500],
    )
    auth_logout(request)
    return redirect('gmtool:login')


# 自定义错误页面
def custom_404(request, exception=None):
    """404 页面未找到"""
    return render(request, 'gmtool/404.html', status=404)


def custom_500(request):
    """500 服务器错误"""
    return render(request, 'gmtool/500.html', status=500)


def custom_403(request, exception=None):
    """403 访问被拒绝"""
    return render(request, 'gmtool/403.html', status=403)


def csrf_failure(request, reason=""):
    """CSRF 验证失败处理"""
    from django.contrib import messages
    from django.urls import reverse
    if request.path == reverse('gmtool:login'):
        messages.error(request, _('页面已过期，请重新登录。'))
        return redirect('gmtool:login')
    return render(request, 'gmtool/403.html', status=403)
