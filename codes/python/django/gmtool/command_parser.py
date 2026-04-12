"""命令解析器 - 解析idip_commands.json为模型可用数据"""
import json
import logging
import os
import tempfile

from django.conf import settings

logger = logging.getLogger(__name__)


def validate_command_ids(command_id, request_id, response_id, exclude_command_id=None):
    """
    校验命令ID和协议ID是否重复。

    校验规则：
    1. command_id 在数据库中唯一
    2. request_id 不能与任何现有命令的 request_id 或 response_id 重复
    3. response_id 不能与任何现有命令的 request_id 或 response_id 重复
    4. request_id 和 response_id 不能相同

    返回: (is_valid, error_message)
    """
    from .models import GMCommand

    errors = []

    # request_id 和 response_id 不能相同
    if request_id == response_id:
        errors.append('Request ID and Response ID cannot be the same')

    # 查询数据库中已有的所有 request_id 和 response_id
    existing_commands = GMCommand.objects.all()
    if exclude_command_id:
        existing_commands = existing_commands.exclude(command_id=exclude_command_id)

    # 检查 command_id 是否已存在
    if GMCommand.objects.filter(command_id=command_id).exists():
        if not exclude_command_id or command_id != exclude_command_id:
            errors.append(f'Command ID {command_id} already exists')

    # 收集所有已有的 request_id 和 response_id，以及它们所属的 command_id
    for cmd in existing_commands:
        if cmd.request_id == request_id:
            errors.append(f'Request ID {request_id} is already used by command {cmd.command_id} (as request ID)')
        if cmd.request_id == response_id:
            errors.append(f'Response ID {response_id} is already used by command {cmd.command_id} (as request ID)')
        if cmd.response_id == request_id:
            errors.append(f'Request ID {request_id} is already used by command {cmd.command_id} (as response ID)')
        if cmd.response_id == response_id:
            errors.append(f'Response ID {response_id} is already used by command {cmd.command_id} (as response ID)')

    # 去重
    errors = list(dict.fromkeys(errors))

    if errors:
        return False, '; '.join(str(e) for e in errors)
    return True, ''


def validate_json_command_ids(data):
    """
    校验 JSON 数据中所有命令的 ID 是否在数据库中存在冲突。

    data: dict，即 idip_commands.json 的完整内容
    返回: (is_valid, error_message)
    """
    from .models import GMCommand

    errors = []

    # 先检查 JSON 内部是否有重复
    json_command_ids = []
    json_request_ids = []
    json_response_ids = []

    for cmd_id, cmd_data in data.items():
        json_command_ids.append(cmd_id)
        req_id = cmd_data.get('id', 0)
        rsp_id = cmd_data.get('responseid', 0)
        json_request_ids.append((cmd_id, req_id))
        json_response_ids.append((cmd_id, rsp_id))

        if req_id == rsp_id:
            errors.append(f'Command {cmd_id}: request ID and response ID cannot be the same')

    # 检查 JSON 内 command_id 重复
    seen_cmd_ids = {}
    for cid in json_command_ids:
        if cid in seen_cmd_ids:
            errors.append(f'Command ID {cid} is duplicated in the JSON file')
        seen_cmd_ids[cid] = True

    # 检查 JSON 内 request_id / response_id 交叉重复
    all_protocol_ids = []
    for cmd_id, rid in json_request_ids:
        all_protocol_ids.append((rid, cmd_id, 'request'))
    for cmd_id, rid in json_response_ids:
        all_protocol_ids.append((rid, cmd_id, 'response'))

    seen_protocol = {}
    for pid, cmd_id, id_type in all_protocol_ids:
        key = pid
        if key in seen_protocol:
            prev_cmd_id, prev_type = seen_protocol[key]
            errors.append(
                f'Protocol ID {pid} is used by both command {prev_cmd_id} ({prev_type}) and command {cmd_id} ({id_type})'
            )
        seen_protocol[key] = (cmd_id, id_type)

    # 检查与数据库中已有命令的冲突
    db_commands = GMCommand.objects.all()
    for cmd in db_commands:
        # command_id 冲突（如果 JSON 中包含该 command_id 则是更新，不算冲突）
        if cmd.command_id not in seen_cmd_ids:
            # 数据库中有但 JSON 中没有的命令，不检查
            pass

        # 检查协议ID冲突（仅对 JSON 中新增的命令检查）
        for json_cmd_id, req_id in json_request_ids:
            if json_cmd_id == cmd.command_id:
                continue  # 同一命令更新，跳过
            if req_id == cmd.request_id:
                errors.append(
                    f'Command {json_cmd_id} request ID {req_id} conflicts with existing command {cmd.command_id} (request ID)'
                )
            if req_id == cmd.response_id:
                errors.append(
                    f'Command {json_cmd_id} request ID {req_id} conflicts with existing command {cmd.command_id} (response ID)'
                )

        for json_cmd_id, rsp_id in json_response_ids:
            if json_cmd_id == cmd.command_id:
                continue
            if rsp_id == cmd.request_id:
                errors.append(
                    f'Command {json_cmd_id} response ID {rsp_id} conflicts with existing command {cmd.command_id} (request ID)'
                )
            if rsp_id == cmd.response_id:
                errors.append(
                    f'Command {json_cmd_id} response ID {rsp_id} conflicts with existing command {cmd.command_id} (response ID)'
                )

    # 去重
    errors = list(dict.fromkeys(errors))

    if errors:
        return False, '; '.join(errors)
    return True, ''


def add_command_to_json(command_data):
    """
    将新命令追加写入 idip_commands.json 文件。

    command_data: dict，包含完整的命令定义（符合 idip_commands.json 中单个命令的格式）
    返回: (success, error_message)
    """
    json_path = os.path.join(settings.BASE_DIR, 'idip_commands.json')

    try:
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except (json.JSONDecodeError, FileNotFoundError) as e:
        return False, str(e)

    # 获取 command_id
    command_id = command_data.get('command_id')
    if not command_id:
        return False, 'Missing command_id in command_data'

    # 添加到 JSON 数据中
    data[command_id] = command_data['data']

    # 原子写入
    try:
        content_str = json.dumps(data, ensure_ascii=False, indent=4)
        fd, tmp_path = tempfile.mkstemp(dir=str(settings.BASE_DIR), suffix='.json.tmp')
        try:
            with os.fdopen(fd, 'w', encoding='utf-8') as f:
                f.write(content_str)
            os.replace(tmp_path, str(json_path))
        except Exception:
            if os.path.exists(tmp_path):
                os.unlink(tmp_path)
            raise
    except Exception as e:
        logger.exception('Failed to write JSON file: %s', e)
        return False, str(e)

    return True, ''


def parse_commands(json_path=None):
    """
    解析idip_commands.json文件，提取命令定义数据。

    返回格式: [
        {
            'command_id': str,
            'command_name': str,
            'tab': str,
            'request_name': str,
            'request_id': int,
            'response_name': str,
            'response_id': int,
            'request_params': list,
            'response_params': list,
        },
        ...
    ]
    """
    if json_path is None:
        json_path = os.path.join(settings.BASE_DIR, 'idip_commands.json')

    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    commands = []
    for cmd_id, cmd_data in data.items():
        request_name = cmd_data.get('request', '')
        response_name = cmd_data.get('respone', '')

        # 提取请求参数定义
        request_params = cmd_data.get(request_name, [])
        # 提取响应参数定义
        response_params = cmd_data.get(response_name, [])

        command_name = (
            cmd_data.get('name')
            or cmd_data.get('command_name')
            or cmd_data.get('tab', '')
        )

        commands.append({
            'command_id': cmd_id,
            'command_name': command_name,
            'tab': cmd_data.get('tab', ''),
            'request_name': request_name,
            'request_id': cmd_data.get('id', 0),
            'response_name': response_name,
            'response_id': cmd_data.get('responseid', 0),
            'request_params': request_params,
            'response_params': response_params,
        })

    return commands


def sync_commands_to_db(json_path=None):
    """
    将idip_commands.json中的命令定义同步到数据库。
    - 已存在的命令：更新内容
    - 新增的命令：创建记录，并自动授予超级管理员权限
    - JSON中不存在的命令：标记为不活跃(is_active=False)
    返回 (created_count, updated_count, deactivated_count)
    """
    from django.db import transaction
    from .models import GMCommand, UserCommandPermission

    commands = parse_commands(json_path)

    with transaction.atomic():
        existing_ids = set(GMCommand.objects.values_list('command_id', flat=True))
        json_ids = set()

        created_count = 0
        updated_count = 0
        new_commands = []

        for cmd in commands:
            json_ids.add(cmd['command_id'])
            obj, created = GMCommand.objects.update_or_create(
                command_id=cmd['command_id'],
                defaults={
                    'command_name': cmd['command_name'],
                    'tab': cmd['tab'],
                    'request_name': cmd['request_name'],
                    'request_id': cmd['request_id'],
                    'response_name': cmd['response_name'],
                    'response_id': cmd['response_id'],
                    'request_params': cmd['request_params'],
                    'response_params': cmd['response_params'],
                    'is_active': True,
                }
            )
            if created:
                created_count += 1
                new_commands.append(obj)
            else:
                updated_count += 1

        # 标记JSON中不存在的命令为不活跃
        deactivated_ids = existing_ids - json_ids
        deactivated_count = GMCommand.objects.filter(
            command_id__in=deactivated_ids
        ).update(is_active=False)

        # 将新增命令自动授予所有超级管理员用户
        if new_commands:
            try:
                from django.contrib.auth.models import User
                superadmin_users = User.objects.filter(is_superuser=True)
                for user in superadmin_users:
                    existing_user_perm_ids = set(UserCommandPermission.objects.filter(
                        user=user
                    ).values_list('command_id', flat=True))
                    new_user_perms = []
                    for cmd in new_commands:
                        if cmd.id not in existing_user_perm_ids:
                            new_user_perms.append(UserCommandPermission(user=user, command=cmd))
                    if new_user_perms:
                        UserCommandPermission.objects.bulk_create(new_user_perms)
                if superadmin_users.exists():
                    logger.info('新增权限已自动授予 %d 个超级管理员用户', superadmin_users.count())
            except Exception as e:
                logger.warning('超级管理员用户权限自动授权跳过: %s', e)

    return created_count, updated_count, deactivated_count
