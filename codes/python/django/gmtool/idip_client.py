"""IDIP API 转发客户端"""
import json
import logging
import urllib.parse

from django.conf import settings
from django.utils.translation import gettext as _

logger = logging.getLogger(__name__)


def _get_requests():
    """延迟获取 requests 模块，避免模块级导入失败阻塞 app 加载"""
    try:
        import requests
        return requests
    except ImportError:
        raise ImportError(_("requests 库未安装，请执行: pip install requests"))


def send_idip_command(command, params):
    """
    构造IDIP协议格式的请求体，以HTTP POST (form-urlencoded)转发至游戏API。

    请求格式:
        POST {IDIP_API_URL}
        Content-Type: application/x-www-form-urlencoded
        id={command_id}&GSA=&content={url_encoded_json}

    兼容两种响应格式：
    1) 新格式包装：
       {
         "status": bool,
         "id": request_id,
         "json": {...} | "...(兼容字符串)"
       }
    2) 旧格式：直接返回业务 JSON

    参数:
        command: GMCommand 模型实例
        params: dict, 用户提交的表单参数

    返回:
        (dict|None, str|None, str, str): (响应数据, 错误信息, 完整请求JSON字符串, 错误类型)
    """
    req = _get_requests()

    # 构造content的JSON结构
    content_json = {
        "head": {
            "Uin": 0,
            "AreaId": params.get('AreaId', 10),
            "Partition": params.get('Partition', 0),
            "PlatId": params.get('PlatId', 1),
            "Requestid": command.request_id,
            "Request": command.request_name,
            "Responseid": command.response_id,
            "Response": command.response_name,
        },
        "body": {
            k: v for k, v in params.items()
            if k not in ('AreaId', 'Partition', 'PlatId')
        }
    }

    # JSON序列化并URL编码
    content_str = json.dumps(content_json, ensure_ascii=False)
    content_encoded = urllib.parse.quote(content_str)

    # 构造form-urlencoded数据
    form_data = {
        'id': command.command_id,
        'GSA': '',
        'content': content_encoded,
    }

    # 构造完整的请求JSON（包含form_data原始格式，用于存储和展示）
    full_request = {
        'url': settings.IDIP_API_URL,
        'method': 'POST',
        'form_data': form_data,
        'content_json': content_json,
    }
    request_content_str = json.dumps(full_request, ensure_ascii=False)

    # 记录请求日志
    logger.info('IDIP请求发送: command=%s, request_id=%s, content=%s',
                command.command_id, command.request_id, content_str)

    try:
        response = req.post(
            settings.IDIP_API_URL,
            data=form_data,
            timeout=settings.IDIP_TIMEOUT,
        )
        response.raise_for_status()
        result = response.json()

        # 新格式包装解析并规范化
        if isinstance(result, dict) and 'status' in result and 'json' in result:
            status = bool(result.get('status'))
            wrapped_id = result.get('id', command.request_id)
            try:
                wrapped_id = int(wrapped_id)
            except (TypeError, ValueError):
                wrapped_id = command.request_id

            wrapped_json = result.get('json')

            # 成功：按需求应为对象，同时兼容字符串
            if status and isinstance(wrapped_json, str):
                try:
                    wrapped_json = json.loads(wrapped_json)
                except (TypeError, ValueError, json.JSONDecodeError):
                    logger.error('IDIP包装响应解析失败: command=%s, json=%s',
                                 command.command_id, str(wrapped_json)[:500])
                    return None, _('请求失败: %(error)s') % {'error': 'invalid wrapped json string'}, request_content_str, 'error'

            if not isinstance(wrapped_json, dict):
                if status:
                    logger.error('IDIP包装响应格式错误: command=%s, type=%s',
                                 command.command_id, type(wrapped_json).__name__)
                    return None, _('请求失败: %(error)s') % {'error': 'invalid wrapped json type'}, request_content_str, 'error'
                wrapped_json = {
                    'Result': -1,
                    'RetMsg': _('请求失败: %(error)s') % {'error': 'invalid wrapped error object'},
                }

            normalized = {
                'status': status,
                'id': wrapped_id,
                'json': wrapped_json,
            }
            logger.info('IDIP请求完成(包装): command=%s, response=%s',
                        command.command_id, json.dumps(normalized, ensure_ascii=False)[:500])
            return normalized, None, request_content_str, ''

        # 旧格式：包装成新结构返回，确保上层一致处理
        normalized_old = {
            'status': True,
            'id': command.request_id,
            'json': result if isinstance(result, dict) else {'Result': 0, 'RetMsg': 'OK', 'raw': result},
        }
        logger.info('IDIP请求成功(旧格式兼容): command=%s, response=%s',
                    command.command_id, json.dumps(normalized_old, ensure_ascii=False)[:500])
        return normalized_old, None, request_content_str, ''

    except req.Timeout:
        logger.error('IDIP API请求超时: command=%s', command.command_id)
        return None, _('请求超时，请稍后重试'), request_content_str, 'timeout'
    except req.ConnectionError:
        logger.error('IDIP API连接失败: url=%s', settings.IDIP_API_URL)
        return None, _('无法连接到游戏服务器，请检查配置'), request_content_str, 'connection'
    except Exception as e:
        # 兼容不同版本 requests 的 JSONDecodeError
        if hasattr(req, 'JSONDecodeError') and isinstance(e, req.JSONDecodeError):
            logger.error('IDIP API响应解析失败: response=%.500s', response.text)
            return {'raw_response': response.text}, None, request_content_str, ''
        logger.error('IDIP API请求异常: %s', e)
        return None, _('请求失败: %(error)s') % {'error': str(e)}, request_content_str, 'error'
