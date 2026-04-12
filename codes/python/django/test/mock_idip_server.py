#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""简单的 IDIP Mock HTTP Server。

功能：
1. 读取项目根目录 idip_commands.json
2. 接收 form-urlencoded POST: id / GSA / content（仅 /cy_idip）
3. 为每个 GM 命令返回可用的 mock 数据（固定或随机）
4. 返回统一包装协议：
   - 成功: {"status": true, "id": request_id, "json": json_obj}
   - 失败: {"status": false, "id": request_id, "json": {"Result": error_code, "RetMsg": "error_msg"}}
"""

import argparse
import json
import random
import time
import urllib.parse
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any, Dict, List, Optional


PROJECT_ROOT = Path(__file__).resolve().parent.parent
COMMANDS_FILE = PROJECT_ROOT / "idip_commands.json"

COMMAND_DEFINITIONS: Dict[str, Dict[str, Any]] = {}

# 可按 commandId 覆盖自动生成结果（优先级最高）
CUSTOM_RESPONSES: Dict[str, Dict[str, Any]] = {
    "10226000": {
        "UsrList_count": 1,
        "UsrList": [
            {
                "Account": "mock_account_001",
                "RoleId": "mock_role_guid_001",
                "RoleName": "测试角色",
                "Gender": 1,
                "Job": "Warrior",
                "Exp": 123456,
                "Level": 88,
                "Scene": 10001,
                "Location": "100,200,0",
                "Fight": 99999,
                "Gold": 5000,
                "Money": 888888,
                "BindGold": 1000,
                "JiaoZi": 6666,
                "SpecialYuanBao": 200,
                "Serverld": 1,
                "CmbiChannelId": "cmbi_test",
                "ChannelId": 10,
                "RegisterTime": int(time.time()) - 86400 * 30,
                "LastLogoutTime": int(time.time()) - 3600,
                "IsOnline": 1,
                "Time": int(time.time()) - 86400,
                "Position": 2,
                "GuildName": "MockGuild",
                "Graduation": 1,
                "IsFriend": 1,
            }
        ],
    }
}


def load_commands() -> Dict[str, Dict[str, Any]]:
    with COMMANDS_FILE.open("r", encoding="utf-8") as f:
        return json.load(f)


def _random_number(type_name: str) -> int:
    t = type_name.lower()
    if t == "uint8":
        return random.randint(0, 255)
    if t == "uint16":
        return random.randint(0, 65535)
    if t == "uint32":
        return random.randint(0, 100000)
    if t == "uint64":
        return random.randint(0, 10**10)
    if t == "int8":
        return random.randint(-128, 127)
    if t == "int16":
        return random.randint(-32768, 32767)
    if t == "int32":
        return random.randint(-100000, 100000)
    if t == "int64":
        return random.randint(-10**10, 10**10)
    return random.randint(0, 10000)


def _primitive_value(type_name: str, field_name: str) -> Any:
    t = type_name.lower()
    if t in {"string"}:
        return f"mock_{field_name.lower()}_{random.randint(100, 999)}"
    if t == "time":
        return int(time.time())
    if t in {"uint8", "uint16", "uint32", "uint64", "int8", "int16", "int32", "int64"}:
        return _random_number(t)
    if t in {"float", "double"}:
        return round(random.uniform(1, 9999), 2)
    if t in {"bool", "boolean"}:
        return random.choice([True, False])
    return f"mock_{field_name.lower()}"


def _generate_from_schema(
    schema_fields: List[Dict[str, Any]],
    command_def: Dict[str, Any],
    depth: int = 0,
) -> Dict[str, Any]:
    if depth > 4:
        return {}

    result: Dict[str, Any] = {}
    count_map: Dict[str, int] = {}

    # 先生成 *_count，便于后续对应列表长度
    for field in schema_fields:
        field_id = field.get("id", "")
        if field_id.endswith("_count"):
            base_name = field_id[:-6]
            count_val = random.randint(1, 3)
            count_map[base_name] = count_val
            result[field_id] = count_val

    for field in schema_fields:
        field_id = field.get("id", "")
        field_type = field.get("type", "")
        if field_id in result:
            continue

        # 结构体类型
        if field_type in command_def:
            child_schema = command_def[field_type]
            list_count = count_map.get(field_id)

            # 常见列表命名兜底
            if list_count is None and field_id.lower().endswith("list"):
                list_count = random.randint(1, 3)

            if list_count is not None:
                result[field_id] = [
                    _generate_from_schema(child_schema, command_def, depth + 1)
                    for _ in range(list_count)
                ]
            else:
                result[field_id] = _generate_from_schema(child_schema, command_def, depth + 1)
        else:
            result[field_id] = _primitive_value(field_type, field_id)

    return result


def _apply_success_defaults(payload: Dict[str, Any], schema_fields: List[Dict[str, Any]]) -> Dict[str, Any]:
    """对常见结果字段应用“成功响应”兜底，避免随机值导致业务判失败。"""
    field_ids = {f.get("id", "") for f in schema_fields if isinstance(f, dict)}

    if "Result" in field_ids:
        payload["Result"] = 0
    if "RetMsg" in field_ids:
        payload["RetMsg"] = "OK"
    if "ret" in field_ids and isinstance(payload.get("ret"), (int, float)):
        payload["ret"] = 0

    return payload


def generate_command_response(command_id: str, command_def: Dict[str, Any]) -> Dict[str, Any]:
    """按当前命令定义自动读取响应协议并组装 json。"""
    # 兼容不同字段命名（当前文件主要是 respone）
    response_name = (
        command_def.get("respone")
        or command_def.get("response")
        or command_def.get("response_name")
    )
    if not response_name:
        return {}

    schema = command_def.get(response_name, [])
    if not isinstance(schema, list):
        return {}

    # 先按 schema 自动生成，保证所有命令都有可用响应
    data = _generate_from_schema(schema, command_def)

    # 再合并手工覆盖（只覆盖指定字段，不影响其他字段自动生成）
    custom = CUSTOM_RESPONSES.get(command_id)
    if isinstance(custom, dict):
        data.update(custom)

    # 对通用状态字段兜底为成功，避免随机值把请求判定为失败
    data = _apply_success_defaults(data, schema)

    return data


def get_request_id(command_def: Optional[Dict[str, Any]]) -> int:
    if not command_def:
        return 0
    rid = command_def.get("id", 0)
    try:
        return int(rid)
    except (TypeError, ValueError):
        return 0


class MockIdipHandler(BaseHTTPRequestHandler):
    server_version = "MockIDIP/2.0"

    def _write_json(self, status: int, payload: Dict[str, Any]) -> None:
        body = json.dumps(payload, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self) -> None:
        if self.path == "/health":
            self._write_json(200, {"status": "ok", "service": "mock-idip-server"})
            return
        self._write_json(404, {"status": False, "id": 0, "json": {"Result": 404, "RetMsg": "Not Found"}})

    def _write_failed(self, request_id: int, error_code: int, error_msg: str, status_code: int = 200) -> None:
        self._write_json(
            status_code,
            {
                "status": False,
                "id": request_id,
                "json": {
                    "Result": error_code,
                    "RetMsg": error_msg,
                },
            },
        )

    def do_POST(self) -> None:
        if self.path != "/cy_idip":
            self._write_failed(0, 404, f"unsupported path: {self.path}", status_code=404)
            return

        content_length = int(self.headers.get("Content-Length", "0"))
        raw_body = self.rfile.read(content_length).decode("utf-8", errors="ignore")
        form = urllib.parse.parse_qs(raw_body)

        command_id = (form.get("id", [""])[0] or "").strip()
        content_raw = form.get("content", [""])[0]
        force_fail_flag = (form.get("__mock_fail", [""])[0] or "").strip()

        parsed_content: Dict[str, Any] = {}
        if content_raw:
            # 兼容 content 可能已被 parse_qs 解码或仍是 quote 字符串
            try:
                parsed_content = json.loads(content_raw)
            except Exception:
                try:
                    parsed_content = json.loads(urllib.parse.unquote(content_raw))
                except Exception:
                    parsed_content = {}

        head = parsed_content.get("head", {}) if isinstance(parsed_content, dict) else {}
        body = parsed_content.get("body", {}) if isinstance(parsed_content, dict) else {}
        body_force_fail = False
        if isinstance(body, dict):
            body_force_fail = bool(body.get("_mock_fail"))

        print("=" * 80)
        print(f"[MockIDIP] path={self.path}, command_id={command_id}")
        print(f"[MockIDIP] head={json.dumps(head, ensure_ascii=False)}")
        print(f"[MockIDIP] body={json.dumps(body, ensure_ascii=False)}")

        command_def = COMMAND_DEFINITIONS.get(command_id)
        request_id = get_request_id(command_def)

        if not command_id:
            self._write_failed(request_id, 4001, "missing command id")
            return

        if not command_def:
            self._write_failed(request_id, 4004, f"command id not found: {command_id}")
            return

        # 手动触发失败，用于联调
        if force_fail_flag == "1" or body_force_fail:
            self._write_failed(request_id, 5001, "mock forced failure")
            return

        response_payload = generate_command_response(command_id, command_def)

        self._write_json(
            200,
            {
                "status": True,
                "id": request_id,
                "json": response_payload,
            },
        )

    def log_message(self, format: str, *args: Any) -> None:
        # 关闭默认噪音日志，保留上面自定义打印
        return


def main() -> None:
    global COMMAND_DEFINITIONS
    COMMAND_DEFINITIONS = load_commands()

    parser = argparse.ArgumentParser(description="Simple IDIP mock HTTP server")
    parser.add_argument("--host", default="127.0.0.1", help="Host to bind (default: 127.0.0.1)")
    parser.add_argument("--port", type=int, default=18080, help="Port to bind (default: 18080)")
    args = parser.parse_args()

    server = ThreadingHTTPServer((args.host, args.port), MockIdipHandler)
    print(f"Mock IDIP server started at http://{args.host}:{args.port}")
    print("POST endpoint: /cy_idip")
    print(f"Loaded command count: {len(COMMAND_DEFINITIONS)}")
    print("Health check: GET /health")
    server.serve_forever()


if __name__ == "__main__":
    main()
