"""命令解析器 - 解析idip_commands.json为模型可用数据"""
import json
import logging
import os
import tempfile

from django.conf import settings
from django.db import transaction
from django.db.models import Q
from django.utils.translation import gettext as _

from .models import GMCommand

logger = logging.getLogger(__name__)


def _get_commands_json_path(json_path=None):
    """统一解析命令 JSON 文件路径。"""
    return json_path or getattr(settings, 'IDIP_JSON_PATH', os.path.join(settings.BASE_DIR, 'idip_commands.json'))


def _write_text_atomically(target_path, content):
    """以临时文件替换方式原子写入文本文件。"""
    fd, tmp_path = tempfile.mkstemp(suffix='.json.tmp')
    try:
        with os.fdopen(fd, 'w', encoding='utf-8') as f:
            f.write(content)
        os.replace(tmp_path, str(target_path))
    except OSError:
        if os.path.exists(tmp_path):
            os.unlink(tmp_path)
        raise


def read_commands_json_snapshot(json_path=None):
    """读取当前命令 JSON 的原始文本，用于失败回滚。"""
    resolved_path = _get_commands_json_path(json_path)
    try:
        with open(resolved_path, 'r', encoding='utf-8') as f:
            return f.read()
    except FileNotFoundError:
        return None


def write_commands_json_content(raw_content, json_path=None):
    """原子写入命令 JSON 原文。"""
    resolved_path = _get_commands_json_path(json_path)
    content = raw_content if isinstance(raw_content, str) else str(raw_content)
    _write_text_atomically(resolved_path, content)


def restore_commands_json_snapshot(snapshot, json_path=None):
    """将命令 JSON 恢复到指定快照；快照为 None 时删除当前文件。"""
    resolved_path = _get_commands_json_path(json_path)
    if snapshot is None:
        try:
            os.remove(resolved_path)
        except FileNotFoundError:
            return
        return
    write_commands_json_content(snapshot, resolved_path)


def _json_object_pairs_no_duplicates(pairs):
    """在 JSON 解析阶段拒绝重复键，避免静默覆盖。"""
    data = {}
    duplicates = []
    for key, value in pairs:
        if key in data:
            duplicates.append(str(key))
        data[key] = value
    if duplicates:
        duplicate_keys = ', '.join(dict.fromkeys(duplicates))
        raise ValueError(_('JSON 中存在重复键: %(keys)s') % {'keys': duplicate_keys})
    return data


def load_commands_json_content(raw_content):
    """解析命令 JSON 原文，并在解析阶段检测重复键。"""
    if isinstance(raw_content, bytes):
        raw_text = raw_content.decode('utf-8')
    else:
        raw_text = str(raw_content)

    data = json.loads(raw_text, object_pairs_hook=_json_object_pairs_no_duplicates)
    if not isinstance(data, dict):
        raise ValueError(_('JSON 顶层必须是对象(dict)'))
    return raw_text, data


def load_commands_json_file(json_path=None):
    """从文件读取命令 JSON，并在解析阶段检测重复键。"""
    resolved_path = _get_commands_json_path(json_path)
    with open(resolved_path, 'r', encoding='utf-8') as f:
        return load_commands_json_content(f.read())


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
        rsp_id = cmd_data.get('response_id', 0)
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
    # 对于本次 JSON 中已出现的命令，统一以 JSON 的目标值为准，不再与数据库中的旧值比较，
    # 避免“同批更新 / 交换协议 ID”时误报；仅与 JSON 之外仍存在于数据库的命令比较。
    json_command_id_set = set(json_command_ids)
    db_commands = list(
        GMCommand.objects.exclude(command_id__in=json_command_id_set).values('command_id', 'request_id', 'response_id')
    )
    db_req_map = {}
    db_rsp_map = {}
    for db_cmd in db_commands:
        cid = db_cmd['command_id']
        rid = db_cmd['request_id']
        rpid = db_cmd['response_id']
        if rid not in db_req_map:
            db_req_map[rid] = []
        db_req_map[rid].append(cid)
        if rpid not in db_rsp_map:
            db_rsp_map[rpid] = []
        db_rsp_map[rpid].append(cid)

    for json_cmd_id, req_id in json_request_ids:
        for db_cid in db_req_map.get(req_id, []):
            errors.append(
                _(
                    '命令 %(json_cmd)s 的请求 ID %(req)s 与现有命令 %(db_cmd)s 冲突（请求 ID）'
                )
                % {'json_cmd': json_cmd_id, 'req': req_id, 'db_cmd': db_cid}
            )
        for db_cid in db_rsp_map.get(req_id, []):
            errors.append(
                _(
                    '命令 %(json_cmd)s 的请求 ID %(req)s 与现有命令 %(db_cmd)s 冲突（响应 ID）'
                )
                % {'json_cmd': json_cmd_id, 'req': req_id, 'db_cmd': db_cid}
            )

    for json_cmd_id, rsp_id in json_response_ids:
        for db_cid in db_req_map.get(rsp_id, []):
            errors.append(
                _(
                    '命令 %(json_cmd)s 的响应 ID %(rsp)s 与现有命令 %(db_cmd)s 冲突（请求 ID）'
                )
                % {'json_cmd': json_cmd_id, 'rsp': rsp_id, 'db_cmd': db_cid}
            )
        for db_cid in db_rsp_map.get(rsp_id, []):
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
    返回: (success, error_message, previous_snapshot)
    """
    json_path = _get_commands_json_path()
    previous_snapshot = read_commands_json_snapshot(json_path)

    try:
        _raw_content, data = load_commands_json_file(json_path)
    except (json.JSONDecodeError, FileNotFoundError, ValueError) as e:
        return False, str(e), None

    command_id = command_data.get('command_id')
    if not command_id:
        return False, _('command_data 中缺少 command_id'), None

    data[command_id] = command_data['data']

    try:
        content_str = json.dumps(data, ensure_ascii=False, indent=4)
        write_commands_json_content(content_str, json_path)
    except (OSError, TypeError, ValueError) as e:
        logger.exception('Failed to write JSON file: %s', e)
        return False, str(e), None

    return True, '', previous_snapshot


def _extract_field_labels(cmd_data):
    """
    从命令 JSON 数据中提取字段 id -> name 映射。

    当前实现只扫描命令对象第一层中值为 list 的字段，收集列表项里的
    id / name；不会递归深入更深层的嵌套结构。
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


def _normalize_struct_list_max_size(field_id, field):
    raw_value = field.get('max_size')
    if raw_value is None or raw_value == '':
        raise ValueError(_('请求结构体列表字段 %(field)s 缺少 max_size') % {'field': field_id})

    try:
        max_size = int(raw_value)
    except (TypeError, ValueError):
        raise ValueError(_('请求结构体列表字段 %(field)s 的 max_size 必须为正整数') % {'field': field_id})

    if max_size <= 0:
        raise ValueError(_('请求结构体列表字段 %(field)s 的 max_size 必须为正整数') % {'field': field_id})

    return max_size


def _enrich_schema_fields(schema_fields, struct_definitions, *, require_struct_list_max_size=False):
    """Attach inline structure metadata used by the execution form and validation."""
    if not isinstance(schema_fields, list):
        return schema_fields

    count_fields = {
        field.get('id')[:-6]: field.get('id')
        for field in schema_fields
        if isinstance(field, dict) and str(field.get('id', '')).endswith('_count')
    }
    struct_field_ids = {
        field.get('id')
        for field in schema_fields
        if isinstance(field, dict) and field.get('type') in struct_definitions
    }

    enriched = []
    for field in schema_fields:
        if not isinstance(field, dict):
            enriched.append(field)
            continue

        field_copy = dict(field)
        field_id = field_copy.get('id')
        field_type = field_copy.get('type')

        if field_type in struct_definitions:
            field_copy['children'] = _enrich_schema_fields(
                struct_definitions[field_type],
                struct_definitions,
                require_struct_list_max_size=require_struct_list_max_size,
            )
            if field_id in count_fields:
                field_copy['count_id'] = count_fields[field_id]
                if require_struct_list_max_size:
                    field_copy['max_size'] = _normalize_struct_list_max_size(field_id, field_copy)

        if field_id and field_id.endswith('_count') and field_id[:-6] in struct_field_ids:
            field_copy['auto_count_for'] = field_id[:-6]

        enriched.append(field_copy)

    return enriched


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
    json_path = _get_commands_json_path(json_path)

    _raw_content, data = load_commands_json_file(json_path)

    commands = []
    for cmd_id, cmd_data in data.items():
        request_name = cmd_data.get('request', '')
        response_name = cmd_data.get('response', '')

        struct_definitions = {
            key: value
            for key, value in cmd_data.items()
            if isinstance(value, list) and key not in (request_name, response_name)
        }

        # 提取请求参数定义
        request_params = _enrich_schema_fields(
            cmd_data.get(request_name, []),
            struct_definitions,
            require_struct_list_max_size=True,
        )
        # 提取响应参数定义
        response_params = _enrich_schema_fields(cmd_data.get(response_name, []), struct_definitions)

        command_name = (
            cmd_data.get('name')
            or cmd_data.get('command_name')
            or cmd_data.get('tab', '')
        )

        # 提取字段标签映射（按 `_extract_field_labels()` 的当前扫描范围）
        field_labels = _extract_field_labels(cmd_data)

        commands.append({
            'command_id': cmd_id,
            'command_name': command_name,
            'tab': cmd_data.get('tab', ''),
            'request_name': request_name,
            'request_id': cmd_data.get('request_id', 0),
            'response_name': response_name,
            'response_id': cmd_data.get('response_id', 0),
            'request_params': request_params,
            'response_params': response_params,
            'field_labels': field_labels,
        })

    return commands


def sync_commands_to_db(json_path=None):
    """
    将idip_commands.json中的命令定义同步到数据库。
    - 已存在的命令：更新内容
    - 新增的命令：创建记录
    - JSON中不存在的命令：标记为不活跃(is_active=False)
    返回 (created_count, updated_count, deactivated_count)
    """
    _raw_content, data = load_commands_json_file(json_path)
    is_valid_ids, id_error_msg = validate_json_command_ids(data)
    if not is_valid_ids:
        raise ValueError(_('检测到 ID 冲突: %(errors)s') % {'errors': id_error_msg})

    commands = parse_commands(json_path)

    with transaction.atomic():
        existing_ids = set(GMCommand.objects.values_list('command_id', flat=True))
        json_ids = set()

        created_count = 0
        updated_count = 0

        for cmd in commands:
            json_ids.add(cmd['command_id'])
            _command, created = GMCommand.objects.update_or_create(
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
            else:
                updated_count += 1

        # 标记JSON中不存在的命令为不活跃
        deactivated_ids = existing_ids - json_ids
        deactivated_count = GMCommand.objects.filter(
            command_id__in=deactivated_ids
        ).update(is_active=False)

    return created_count, updated_count, deactivated_count
