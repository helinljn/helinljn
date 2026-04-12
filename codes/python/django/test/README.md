# IDIP Mock Server（测试用）

该目录提供一个简单 HTTP Server，用于模拟 IDIP 接口返回数据，供本项目联调使用。

## 1. 功能说明

- 从项目根目录读取 `idip_commands.json`
- 接收与 `gmtool/idip_client.py` 一致的请求格式（`application/x-www-form-urlencoded`）
- 业务接口路径固定为：`POST /cy_idip`
- 支持每个 GM 命令都返回 mock 数据：
  - 默认按响应 schema 自动生成
  - 可在 `CUSTOM_RESPONSES` 中按 commandId 覆盖为固定数据
- 提供健康检查接口：`GET /health`

---

## 2. 启动方式

在项目根目录执行：

```bash
python test/mock_idip_server.py --host 127.0.0.1 --port 18080
```

默认参数也是 `127.0.0.1:18080`，所以可以简写：

```bash
python test/mock_idip_server.py
```

---

## 3. Django 配置

将 Django 的 `IDIP_API_URL` 指向（注意路径必须带 `/cy_idip`）：

```text
http://127.0.0.1:18080/cy_idip
```

这样现有 `gmtool/idip_client.py` 发出的请求会直接打到 mock server。

---

## 4. 接口约定

### 4.1 健康检查

- 方法：`GET`
- 路径：`/health`
- 示例响应：

```json
{
  "status": "ok",
  "service": "mock-idip-server"
}
```

### 4.2 业务请求

- 方法：`POST`
- 路径：`/cy_idip`
- Content-Type：`application/x-www-form-urlencoded`
- 字段：
  - `id`: GM 命令 ID（如 `10226000`）
  - `GSA`: 透传字段
  - `content`: URL 编码后的 JSON（包含 `head` / `body`）
  - `__mock_fail`（可选）: 传 `1` 时强制模拟失败

也支持在 `content.body` 中传 `_mock_fail: true` 触发失败。

---

## 5. 返回数据协议（最新）

### 5.1 失败

```json
{
  "status": false,
  "id": 4105,
  "json": {
    "Result": 5001,
    "RetMsg": "mock forced failure"
  }
}
```

说明：
- `id` 为该命令在 `idip_commands.json` 里定义的请求协议 ID（字段 `id`，数字）
- `json` 是对象，含错误码与错误消息

### 5.2 成功

```json
{
  "status": true,
  "id": 4105,
  "json": {
    "Result": 0,
    "RetMsg": "OK(mock)",
    "Data": {}
  }
}
```

说明：
- `id` 为该命令请求协议 ID（数字）
- `json` 为该命令在 `idip_commands.json` 对应响应结构的 JSON 对象（`json_obj`）

---

## 6. 快速测试（curl）

> Windows cmd 下建议用双引号并转义；下面示例为通用思路。

### 6.1 正常成功

```bash
curl -X POST "http://127.0.0.1:18080/cy_idip" ^
  -H "Content-Type: application/x-www-form-urlencoded" ^
  --data-urlencode "id=10226001" ^
  --data-urlencode "GSA=" ^
  --data-urlencode "content={\"head\":{\"Requestid\":4105},\"body\":{\"RollingContent\":\"hello\"}}"
```

### 6.2 强制失败

```bash
curl -X POST "http://127.0.0.1:18080/cy_idip" ^
  -H "Content-Type: application/x-www-form-urlencoded" ^
  --data-urlencode "id=10226001" ^
  --data-urlencode "__mock_fail=1" ^
  --data-urlencode "GSA=" ^
  --data-urlencode "content={\"head\":{\"Requestid\":4105},\"body\":{\"RollingContent\":\"hello\"}}"
```

---

## 7. 自定义某个命令的固定返回

编辑 `test/mock_idip_server.py` 中的 `CUSTOM_RESPONSES`：

```python
CUSTOM_RESPONSES = {
    "10226001": {
        "Result": 0,
        "RetMsg": "公告发送成功",
        "Ticket": "mock-ticket-001"
    }
}
```

说明：
- `CUSTOM_RESPONSES[commandId]` 是成功响应中业务 payload 的主体内容（示例里为 Data 相关结构）
- 服务会统一封装为新协议：`status/id/json`
