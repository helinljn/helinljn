"""管理命令：格式化 idip_commands.json 中每个命令对象的字段顺序"""
import json
import os
import tempfile
from collections import OrderedDict

from django.conf import settings
from django.core.management.base import BaseCommand, CommandError


PREFERRED_FIELD_ORDER = [
    'tab',
    'request_desc',
    'request_id',
    'request',
    'response_desc',
    'response_id',
    'response',
]


def reorder_command_fields(command_data):
    """按统一规则重排单个命令对象字段顺序。"""
    if not isinstance(command_data, dict):
        return command_data

    ordered = OrderedDict()
    request_name = command_data.get('request')
    response_name = command_data.get('response')

    # 先放固定的头部字段
    for key in PREFERRED_FIELD_ORDER:
        if key in command_data:
            ordered[key] = command_data[key]

    # 再放请求/响应参数定义
    if request_name in command_data and request_name not in ordered:
        ordered[request_name] = command_data[request_name]

    if response_name in command_data and response_name not in ordered:
        ordered[response_name] = command_data[response_name]

    # 最后保留其他结构体或扩展字段
    for key, value in command_data.items():
        if key not in ordered:
            ordered[key] = value

    return ordered


def format_idip_commands_data(data):
    """格式化整个 idip_commands.json 数据结构。"""
    if not isinstance(data, dict):
        raise ValueError('idip_commands.json 顶层必须是对象(dict)')

    formatted = OrderedDict()
    for command_id, command_data in data.items():
        formatted[command_id] = reorder_command_fields(command_data)
    return formatted


class Command(BaseCommand):
    help = '格式化 idip_commands.json 中每个命令对象的字段顺序'

    def add_arguments(self, parser):
        parser.add_argument(
            '--check',
            action='store_true',
            help='仅检查是否需要格式化，不写回文件',
        )

    def handle(self, *args, **options):
        json_path = getattr(
            settings,
            'IDIP_JSON_PATH',
            os.path.join(settings.BASE_DIR, 'idip_commands.json'),
        )

        try:
            with open(json_path, 'r', encoding='utf-8') as f:
                data = json.load(f)
        except FileNotFoundError as e:
            raise CommandError(f'找不到文件: {e}') from e
        except json.JSONDecodeError as e:
            raise CommandError(f'JSON 解析失败: {e}') from e

        formatted_data = format_idip_commands_data(data)
        original_content = json.dumps(data, ensure_ascii=False, indent=4)
        formatted_content = json.dumps(formatted_data, ensure_ascii=False, indent=4)

        if options['check']:
            if original_content == formatted_content:
                self.stdout.write(self.style.SUCCESS('idip_commands.json 已符合格式要求'))
                return
            raise CommandError('idip_commands.json 需要格式化')

        if original_content == formatted_content:
            self.stdout.write(self.style.SUCCESS('idip_commands.json 已是规范格式，无需修改'))
            return

        fd, tmp_path = tempfile.mkstemp(suffix='.json.tmp')
        try:
            with os.fdopen(fd, 'w', encoding='utf-8') as f:
                f.write(formatted_content)
            os.replace(tmp_path, str(json_path))
        except Exception:
            if os.path.exists(tmp_path):
                os.unlink(tmp_path)
            raise

        self.stdout.write(self.style.SUCCESS(f'格式化完成: {json_path}'))
