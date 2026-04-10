"""IDIP API 转发客户端"""
import json
import logging
import urllib.parse

from django.conf import settings

logger = logging.getLogger(__name__)

# 延迟导入 requests，避免模块级导入失败阻塞 app 加载
requests = None


def _get_requests():
    """延迟获取 requests 模块"""
    global requests
    if requests is None:
        try:
            import requests as _requests
            requests = _requests
        except ImportError:
            raise ImportError(
                "requests 库未安装，请执行: pip install requests"
            )
    return requests


def send_idip_command(command, params):
    """
    构造IDIP协议格式的请求体，以HTTP POST (form-urlencoded)转发至游戏API。

    请求格式:
        POST {IDIP_API_URL}
        Content-Type: application/x-www-form-urlencoded
        id={command_id}&GSA=&content={url_encoded_json}

    参数:
        command: GMCommand 模型实例
        params: dict, 用户提交的表单参数

    返回:
        (dict|None, str|None): (响应数据, 错误信息)
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

    try:
        response = req.post(
            settings.IDIP_API_URL,
            data=form_data,
            timeout=settings.IDIP_TIMEOUT,
        )
        response.raise_for_status()
        return response.json(), None
    except req.Timeout:
        logger.error(f'IDIP API请求超时: command={command.command_id}')
        return None, '请求超时，请稍后重试'
    except req.ConnectionError:
        logger.error(f'IDIP API连接失败: url={settings.IDIP_API_URL}')
        return None, '无法连接到游戏服务器，请检查配置'
    except Exception as e:
        # 兼容不同版本 requests 的 JSONDecodeError
        if hasattr(req, 'JSONDecodeError') and isinstance(e, req.JSONDecodeError):
            logger.error(f'IDIP API响应解析失败: response={response.text[:500]}')
            return {'raw_response': response.text}, None
        logger.error(f'IDIP API请求异常: {e}')
        return None, f'请求失败: {str(e)}'
