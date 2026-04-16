"""认证相关视图"""
import logging

from django.conf import settings
from django.contrib import messages
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
from .utils import get_client_ip

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


def _get_login_failure_attempts(ip, username):
    """读取当前失败次数（IP 维度 + IP+用户名维度）。"""
    cache_key_ip, cache_key_ip_user = _build_login_throttle_cache_keys(ip, username)
    attempts_ip = int(cache.get(cache_key_ip, 0) or 0)
    attempts_ip_user = int(cache.get(cache_key_ip_user, 0) or 0)
    return attempts_ip, attempts_ip_user


def _increment_login_failure_attempts(ip, username):
    """登录失败后递增失败计数，返回递增后的双维度次数。"""
    cache_key_ip, cache_key_ip_user = _build_login_throttle_cache_keys(ip, username)

    try:
        attempts_ip = cache.incr(cache_key_ip)
    except ValueError:
        attempts_ip = 1
        cache.set(cache_key_ip, attempts_ip, timeout=LOGIN_LOCKOUT_SECONDS)

    try:
        attempts_ip_user = cache.incr(cache_key_ip_user)
    except ValueError:
        attempts_ip_user = 1
        cache.set(cache_key_ip_user, attempts_ip_user, timeout=LOGIN_LOCKOUT_SECONDS)

    return attempts_ip, attempts_ip_user


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

        cache_key_ip, cache_key_ip_user = _build_login_throttle_cache_keys(ip, username)

        # 登录前先检查是否已被锁定；只统计失败次数，不统计总尝试次数
        attempts_ip, attempts_ip_user = _get_login_failure_attempts(ip, username)
        if attempts_ip >= LOGIN_MAX_ATTEMPTS or attempts_ip_user >= LOGIN_MAX_ATTEMPTS:
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
                attempts_ip, attempts_ip_user = _increment_login_failure_attempts(ip, username)
                LoginLog.objects.create(
                    user=None, username=username, action='login_failed',
                    ip_address=ip, user_agent=ua, reason=_('账号已禁用'),
                )
                log_operation('auth', 'login_failed', ip_address=ip,
                              detail={'username': username, 'reason': str(_('账号已禁用'))})
                if attempts_ip >= LOGIN_MAX_ATTEMPTS or attempts_ip_user >= LOGIN_MAX_ATTEMPTS:
                    return render(request, 'gmtool/login.html', {
                        'error': _('登录尝试次数过多，请 %(minutes)d 分钟后再试') % {'minutes': LOGIN_LOCKOUT_SECONDS // 60},
                    })
                remaining = LOGIN_MAX_ATTEMPTS - max(attempts_ip, attempts_ip_user)
                return render(request, 'gmtool/login.html', {
                    'error': _('账号已被禁用，还可尝试 %(remaining)d 次') % {'remaining': remaining},
                })
        else:
            attempts_ip, attempts_ip_user = _increment_login_failure_attempts(ip, username)
            remaining = LOGIN_MAX_ATTEMPTS - max(attempts_ip, attempts_ip_user)
            LoginLog.objects.create(
                user=None, username=username, action='login_failed',
                ip_address=ip, user_agent=ua, reason=_('用户名或密码错误'),
            )
            log_operation('auth', 'login_failed', ip_address=ip,
                          detail={'username': username, 'reason': str(_('用户名或密码错误'))})
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
    from django.urls import reverse
    if request.path == reverse('gmtool:login'):
        messages.error(request, _('页面已过期，请重新登录。'))
        return redirect('gmtool:login')
    return render(request, 'gmtool/403.html', status=403)
