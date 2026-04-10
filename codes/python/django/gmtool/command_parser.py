"""命令解析器 - 解析idip_commands.json为模型可用数据"""
import json
import logging
import os

from django.conf import settings

logger = logging.getLogger(__name__)


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

        commands.append({
            'command_id': cmd_id,
            'command_name': cmd_data.get('tab', ''),
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
    from .models import GMCommand, UserCommandPermission

    commands = parse_commands(json_path)
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
                logger.info(f'新增权限已自动授予 {superadmin_users.count()} 个超级管理员用户')
        except Exception as e:
            logger.warning(f'超级管理员用户权限自动授权跳过: {e}')

    return created_count, updated_count, deactivated_count
