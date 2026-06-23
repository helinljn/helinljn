"""公告目录服 HTTP 客户端。"""
import json
import logging
from urllib.parse import urlparse

from django.conf import settings
from django.utils.translation import gettext as _

logger = logging.getLogger(__name__)

CREATE_PATH = '/server/announcementSave.php'
DELETE_PATH = '/server/announcementDelete.php'
QUERY_PATH = '/client/announcement.php'


class AnnouncementConfigError(ValueError):
    """公告接口配置错误。"""


def _get_requests():
    """延迟获取 requests 模块，避免模块级导入失败阻塞 app 加载。"""
    try:
        import requests
        return requests
    except ImportError as exc:
        raise ImportError(_('requests 库未安装，请执行: pip install requests')) from exc


def _build_url(path):
    """构造公告接口 URL，并校验基础地址。"""
    base_url = str(getattr(settings, 'ANNOUNCEMENT_BASE_URL', '') or '').strip().rstrip('/')
    if not base_url:
        raise AnnouncementConfigError(_('公告目录服地址未配置'))

    parsed = urlparse(base_url)
    if parsed.scheme not in ('http', 'https') or not parsed.netloc:
        raise AnnouncementConfigError(_('公告目录服地址必须是有效的 HTTP/HTTPS 地址'))
    if parsed.query or parsed.fragment:
        raise AnnouncementConfigError(_('公告目录服基础地址不能包含 query string 或 fragment'))

    return f'{base_url}/{path.lstrip("/")}'


def get_announcement_base_url_error():
    """返回公告目录服基础地址配置错误；配置可用时返回空字符串。"""
    try:
        _build_url(QUERY_PATH)
    except AnnouncementConfigError as exc:
        return str(exc)
    return ''


def _http_error_message(response):
    status_code = getattr(response, 'status_code', '')
    return _('公告接口 HTTP 错误: %(status)s') % {'status': status_code}


def _raw_response_text(response):
    return getattr(response, 'text', '') or ''


def _request_failed(error_message, raw_response='', error_type='failed'):
    return None, error_message, raw_response, error_type


def _is_timeout_error(exc):
    try:
        req = _get_requests()
    except ImportError:
        return False
    timeout_cls = getattr(req, 'Timeout', None)
    return bool(timeout_cls and isinstance(exc, timeout_cls))


def _post_text(path, payload):
    req = _get_requests()
    url = _build_url(path)
    response = req.post(
        url,
        data=payload,
        timeout=settings.ANNOUNCEMENT_TIMEOUT,
    )
    raw_response = _raw_response_text(response)
    if not 200 <= getattr(response, 'status_code', 200) < 300:
        return _request_failed(_http_error_message(response), raw_response)

    result_text = raw_response.strip()
    if result_text == 'OK':
        return {'result': 'OK'}, '', raw_response, ''
    if result_text.startswith('Invalid:') or result_text.startswith('FAIL:'):
        return _request_failed(result_text, raw_response)
    return _request_failed(
        _('公告接口返回未知结果: %(result)s') % {'result': result_text or '<empty>'},
        raw_response,
    )


def query_announcements(platform, channel):
    """查询公告列表。"""
    raw_response = ''
    try:
        req = _get_requests()
        url = _build_url(QUERY_PATH)
        response = req.get(
            url,
            params={
                'Platform': platform,
                'Channel': channel,
            },
            timeout=settings.ANNOUNCEMENT_TIMEOUT,
        )
        raw_response = _raw_response_text(response)
        if not 200 <= getattr(response, 'status_code', 200) < 300:
            return _request_failed(_http_error_message(response), raw_response)

        try:
            data = response.json()
        except (TypeError, ValueError, json.JSONDecodeError):
            data = json.loads(raw_response)

        if not isinstance(data, list):
            return _request_failed(_('公告查询响应必须是 JSON 数组'), raw_response)
        return data, '', raw_response, ''
    except AnnouncementConfigError as exc:
        return _request_failed(str(exc))
    except ImportError as exc:
        return _request_failed(str(exc), raw_response)
    except (json.JSONDecodeError, ValueError) as exc:
        logger.warning('Announcement query failed: %s', exc)
        return _request_failed(_('公告查询响应解析失败'), raw_response)
    except Exception as exc:
        if _is_timeout_error(exc):
            logger.warning(
                'Announcement query timeout: platform=%s channel=%s',
                platform,
                channel,
            )
            return _request_failed(_('公告查询请求超时，请稍后重试'), raw_response, 'timeout')
        logger.warning('Announcement query request failed: %s', exc)
        return _request_failed(_('公告查询请求失败: %(error)s') % {'error': str(exc)}, raw_response)


def create_announcement(payload):
    """发布公告。"""
    raw_response = ''
    try:
        response_data, error_message, raw_response, error_type = _post_text(CREATE_PATH, payload)
        if error_message:
            logger.warning('Announcement create failed: %s', error_message)
        return response_data, error_message, raw_response, error_type
    except AnnouncementConfigError as exc:
        return _request_failed(str(exc))
    except ImportError as exc:
        return _request_failed(str(exc))
    except Exception as exc:
        if _is_timeout_error(exc):
            logger.warning('Announcement create timeout: payload=%s', payload)
            return _request_failed(_('公告发布请求超时，请稍后重试'), raw_response, 'timeout')
        logger.warning('Announcement create request failed: %s', exc)
        return _request_failed(_('公告发布请求失败: %(error)s') % {'error': str(exc)}, raw_response)


def delete_announcement(platform, channel, announcement_type, announcement_id):
    """删除公告。"""
    payload = {
        'Platform': platform,
        'Channel': channel,
        'AnnouncementType': announcement_type,
        'AnnouncementId': str(announcement_id),
    }
    raw_response = ''
    try:
        response_data, error_message, raw_response, error_type = _post_text(DELETE_PATH, payload)
        if error_message:
            logger.warning('Announcement delete failed: %s', error_message)
        return response_data, error_message, raw_response, error_type
    except AnnouncementConfigError as exc:
        return _request_failed(str(exc))
    except ImportError as exc:
        return _request_failed(str(exc))
    except Exception as exc:
        if _is_timeout_error(exc):
            logger.warning('Announcement delete timeout: payload=%s', payload)
            return _request_failed(_('公告删除请求超时，请稍后重试'), raw_response, 'timeout')
        logger.warning('Announcement delete request failed: %s', exc)
        return _request_failed(_('公告删除请求失败: %(error)s') % {'error': str(exc)}, raw_response)
