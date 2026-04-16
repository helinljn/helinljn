"""命令解析器 - 解析idip_commands.json为模型可用数据"""
import json
import logging
import os
import tempfile

from django.conf import settings
from django.db import transaction
from django.db.models import Q
from django.utils.translation import gettext as _

from .models import GMCommand, UserCommandPermission

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
    errors = []

    # request_id 和 response_id 不能相同
    if request_id == response_id:
        errors.append(_('请求 ID 和响应 ID 不能相同'))

    # 检查 command_id 是否已存在
    cmd_qs = GMCommand.objects.all()
    if exclude_command_id:
        cmd_qs = cmd_qs.exclude(command_id=exclude_command_id)
    if cmd_qs.filter(command_id=command_id).exists():
        errors.append(_('命令 ID %(id)s 已存在') % {'id': command_id})

    # 一次查询检查 request_id / response_id 冲突（替代4次独立查询）
    conflict_qs = GMCommand.objects.all()
    if exclude_command_id:
        conflict_qs = conflict_qs.exclude(command_id=exclude_command_id)

    conflicts = list(conflict_qs.filter(
        Q(request_id=request_id) | Q(request_id=response_id) |
        Q(response_id=request_id) | Q(response_id=response_id)
    ).values('command_id', 'request_id', 'response_id'))

    for conflict in conflicts:
        if conflict['request_id'] == request_id:
            errors.append(
                _('请求 ID %(req)s 已被命令 %(cmd)s 使用（作为请求 ID）')
                % {'req': request_id, 'cmd': conflict['command_id']}
            )
        if conflict['request_id'] == response_id:
            errors.append(
                _('响应 ID %(rsp)s 已被命令 %(cmd)s 使用（作为请求 ID）')
                % {'rsp': response_id, 'cmd': conflict['command_id']}
            )
        if conflict['response_id'] == request_id:
            errors.append(
                _('请求 ID %(req)s 已被命令 %(cmd)s 使用（作为响应 ID）')
                % {'req': request_id, 'cmd': conflict['command_id']}
            )
        if conflict['response_id'] == response_id:
            errors.append(
                _('响应 ID %(rsp)s 已被命令 %(cmd)s 使用（作为响应 ID）')
                % {'rsp': response_id, 'cmd': conflict['command_id']}
            )

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
    errors = []

    # 先检查 JSON 内部是否有重复
    json_command_ids = []
    json_request_ids = []
    json_response_ids = []

    for cmd_id, cmd_data in data.items():
        json_command_ids.append(cmd_id)
        req_id = cmd_data.get('request_id', 0)
        rsp_id = cmd_data.get('responseid', 0)
        json_request_ids.append((cmd_id, req_id))
        json_response_ids.append((cmd_id, rsp_id))

        if req_id == rsp_id:
            errors.append(
                _('命令 %(cmd)s：请求 ID 和响应 ID 不能相同')
                % {'cmd': cmd_id}
            )

    # 检查 JSON 内 command_id 重复
    seen_cmd_ids = {}
    for cid in json_command_ids:
        if cid in seen_cmd_ids:
            errors.append(_('命令 ID %(id)s 在 JSON 文件中重复') % {'id': cid})
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
                _(
                    '协议 ID %(pid)s 同时被命令 %(prev_cmd)s（%(prev_type)s）和命令 %(cmd)s（%(id_type)s）使用'
                )
                % {
                    'pid': pid,
                    'prev_cmd': prev_cmd_id,
                    'prev_type': prev_type,
                    'cmd': cmd_id,
                    'id_type': id_type,
                }
            )
        seen_protocol[key] = (cmd_id, id_type)

    # 检查与数据库中已有命令的冲突（一次查询 + 内存比对）
    db_commands = list(GMCommand.objects.values('command_id', 'request_id', 'response_id'))
    db_cmd_map = {}
    db_req_map = {}
    db_rsp_map = {}
    for db_cmd in db_commands:
        cid = db_cmd['command_id']
        rid = db_cmd['request_id']
        rpid = db_cmd['response_id']
        db_cmd_map[cid] = db_cmd
        if rid not in db_req_map:
            db_req_map[rid] = []
        db_req_map[rid].append(cid)
        if rpid not in db_rsp_map:
            db_rsp_map[rpid] = []
        db_rsp_map[rpid].append(cid)

    for json_cmd_id, req_id in json_request_ids:
        # 同一命令更新，跳过
        if json_cmd_id in db_cmd_map:
            continue
        for db_cid in db_req_map.get(req_id, []):
            if db_cid != json_cmd_id:
                errors.append(
                    _(
                        '命令 %(json_cmd)s 的请求 ID %(req)s 与现有命令 %(db_cmd)s 冲突（请求 ID）'
                    )
                    % {'json_cmd': json_cmd_id, 'req': req_id, 'db_cmd': db_cid}
                )
        for db_cid in db_rsp_map.get(req_id, []):
            if db_cid != json_cmd_id:
                errors.append(
                    _(
                        '命令 %(json_cmd)s 的请求 ID %(req)s 与现有命令 %(db_cmd)s 冲突（响应 ID）'
                    )
                    % {'json_cmd': json_cmd_id, 'req': req_id, 'db_cmd': db_cid}
                )

    for json_cmd_id, rsp_id in json_response_ids:
        if json_cmd_id in db_cmd_map:
            continue
        for db_cid in db_req_map.get(rsp_id, []):
            if db_cid != json_cmd_id:
                errors.append(
                    _(
                        '命令 %(json_cmd)s 的响应 ID %(rsp)s 与现有命令 %(db_cmd)s 冲突（请求 ID）'
                    )
                    % {'json_cmd': json_cmd_id, 'rsp': rsp_id, 'db_cmd': db_cid}
                )
        for db_cid in db_rsp_map.get(rsp_id, []):
            if db_cid != json_cmd_id:
                errors.append(
                    _(
                        '命令 %(json_cmd)s 的响应 ID %(rsp)s 与现有命令 %(db_cmd)s 冲突（响应 ID）'
                    )
                    % {'json_cmd': json_cmd_id, 'rsp': rsp_id, 'db_cmd': db_cid}
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
    json_path = getattr(settings, 'IDIP_JSON_PATH', os.path.join(settings.BASE_DIR, 'idip_commands.json'))

    try:
        with open(json_path, 'r', encoding='utf-8') as f:
            data = json.load(f)
    except (json.JSONDecodeError, FileNotFoundError) as e:
        return False, str(e)

    # 获取 command_id
    command_id = command_data.get('command_id')
    if not command_id:
        return False, _('command_data 中缺少 command_id')

    # 添加到 JSON 数据中
    data[command_id] = command_data['data']

    # 原子写入
    try:
        content_str = json.dumps(data, ensure_ascii=False, indent=4)
        fd, tmp_path = tempfile.mkstemp(suffix='.json.tmp')
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


def _extract_field_labels(cmd_data):
    """
    从命令 JSON 数据中提取所有字段 id -> name 映射（含嵌套结构体）。

    遍历命令对象中所有列表类型的值，收集其中的 id 和 name 字段。
    这样可以获取包括嵌套结构体（如 SUsrInfo）在内的所有字段标签。
    """
    labels = {}
    for val in cmd_data.values():
        if isinstance(val, list):
            for item in val:
                if isinstance(item, dict):
                    field_id = item.get('id')
                    field_name = item.get('name')
                    if field_id and field_name:
                        labels[field_id] = field_name
    return labels


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
            'field_labels': dict,  # 新增：所有字段的 id -> name 映射
        },
        ...
    ]
    """
    if json_path is None:
        json_path = getattr(settings, 'IDIP_JSON_PATH', os.path.join(settings.BASE_DIR, 'idip_commands.json'))

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

        # 提取所有字段标签（含嵌套结构体）
        field_labels = _extract_field_labels(cmd_data)

        commands.append({
            'command_id': cmd_id,
            'command_name': command_name,
            'tab': cmd_data.get('tab', ''),
            'request_name': request_name,
            'request_id': cmd_data.get('request_id', 0),
            'response_name': response_name,
            'response_id': cmd_data.get('responseid', 0),
            'request_params': request_params,
            'response_params': response_params,
            'field_labels': field_labels,
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
                    'field_labels': cmd['field_labels'],
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

        # 将新增命令自动授予所有 Django 超级管理员用户
        if new_commands:
            try:
                from .utils import assign_super_admin_permissions, get_super_admin_users_queryset

                super_admin_users = get_super_admin_users_queryset()

                superadmin_count = 0
                for user in super_admin_users:
                    assign_super_admin_permissions(user, new_commands=new_commands)
                    superadmin_count += 1

                if superadmin_count > 0:
                    logger.info('新增权限已自动授予 %d 个超级管理员用户', superadmin_count)
            except Exception as e:
                logger.warning('超级管理员用户权限自动授权跳过: %s', e)

    return created_count, updated_count, deactivated_count
