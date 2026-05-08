# 游戏 GM 命令后台管理系统（Django）

基于 Django 的 GM 命令后台管理平台，提供命令执行、权限控制、命令定义同步、日志审计、国际化与基础安全防护能力。

---

## 1. 项目简介

本项目用于承载游戏 GM 命令后台管理场景，核心目标是：

- 为不同用户提供受控的 GM 命令执行入口
- 从 `idip_commands.json` 解析命令定义并同步到数据库
- 根据命令定义动态渲染请求表单并展示响应结果
- 支持用户管理、命令权限分配、登录日志与命令执行日志
- 提供审计日志、应用日志与敏感字段脱敏能力
- 支持中英文国际化
- 支持反向代理部署与 HTTPS 场景

项目不依赖 Django Admin 承载业务操作，主要通过自定义页面、API 与管理命令完成日常管理。

---

## 2. 当前功能概览

### 2.1 认证与权限
- 登录 / 登出
- 登录失败限速与锁定
- 超级管理员保护
- 普通用户按命令粒度授权
- 超级管理员自动拥有全部活跃命令访问权限
- 用户扩展资料 `UserProfile` 自动补齐

### 2.2 命令管理
- 仪表盘首页
- 命令列表按分组展示
- 动态参数表单执行 GM 命令
- 支持批量执行参数配置
- 在线新增命令并写入 `idip_commands.json`
- 上传命令定义 JSON 并自动同步数据库
- JSON 中不存在的命令自动标记为停用
- 停用命令不可继续执行

### 2.3 日志与审计
- 登录日志 `LoginLog`
- 命令执行日志 `CommandLog`
- 审计日志 `gmtool.audit`
- 应用日志 `gmtool`
- 日志详情接口自动脱敏敏感字段
- 若远端命令执行成功但本地日志写入失败，接口会返回 `warning` 与 `log_persisted=false`

### 2.4 配置与运行
- `.env` 驱动核心配置
- 默认使用 SQLite，可通过环境变量切换到 MySQL
- 支持国际化（简体中文 / 英文）
- 支持反向代理 HTTPS 感知
- 支持命令定义文件变更监控（开发便利功能）
- 提供多条 `manage.py` 自定义命令辅助维护

---

## 3. 技术栈

| 类别 | 说明 |
|---|---|
| 后端框架 | Django 5.2.12 |
| Python | 3.11 |
| 数据库 | SQLite（默认）/ MySQL（可配置） |
| HTTP 客户端 | requests |
| 配置管理 | python-decouple |
| 国际化 | Django i18n |
| 前端 | Django Template + Tabler/Bootstrap 风格页面 |
| 日志轮转 | 自定义 `SafeTimedRotatingFileHandler` |

---

## 4. 项目结构

```text
d:\helinljn\codes\python\django\
├── .env
├── manage.py
├── db.sqlite3
├── environment.yml
├── idip_commands.json
├── README.md
├── mysite/
│   ├── __init__.py
│   ├── asgi.py
│   ├── settings.py
│   ├── urls.py
│   └── wsgi.py
├── gmtool/
│   ├── api_views.py
│   ├── audit_log.py
│   ├── auth_views.py
│   ├── command_parser.py
│   ├── command_services.py
│   ├── command_views.py
│   ├── decorators.py
│   ├── forms.py
│   ├── idip_client.py
│   ├── logging_handlers.py
│   ├── middleware.py
│   ├── models.py
│   ├── permission_service.py
│   ├── query_utils.py
│   ├── security_utils.py
│   ├── signals.py
│   ├── urls.py
│   ├── user_views.py
│   ├── views.py
│   ├── management/commands/
│   ├── migrations/
│   ├── templates/gmtool/
│   └── templatetags/
├── locale/
├── logs/
└── test/
    ├── README.md
    └── mock_idip_server.py
```

---

## 5. 核心模块说明

### 5.1 `mysite/`
项目级配置与入口。

- `settings.py`：运行配置、环境变量、安全、缓存、日志
- `urls.py`：项目级路由入口
- `asgi.py` / `wsgi.py`：部署入口

### 5.2 `gmtool/`
主业务应用。

- `auth_views.py`：登录、登出与认证相关逻辑
- `command_views.py`：仪表盘、命令列表、命令执行、命令新增、命令同步、命令日志
- `user_views.py`：用户管理、用户编辑、删除、权限分配、登录日志查看
- `api_views.py`：命令定义上传接口、日志详情接口
- `command_parser.py`：命令定义文件解析、ID 校验、JSON 快照读写、数据库同步
- `command_services.py`：命令执行前的参数校验与命令查询辅助逻辑
- `idip_client.py`：与远端 IDIP 服务通信
- `middleware.py`：命令定义文件变更监控
- `security_utils.py`：脱敏与安全辅助函数
- `audit_log.py`：审计日志封装
- `logging_handlers.py`：Windows 友好的日志轮转处理器
- `models.py`：命令、权限、用户扩展、日志等模型
- `signals.py`：自动补齐 `UserProfile`

### 5.3 `test/`
开发联调辅助内容。

- `mock_idip_server.py`：本地模拟 IDIP 服务
- `test/README.md`：测试或联调说明

---

## 6. 主要数据模型

### 6.1 `GMCommand`
GM 命令定义模型，由 `idip_commands.json` 同步生成。

关键字段：
- `command_id`
- `command_name`
- `tab`
- `request_name`
- `request_id`
- `response_name`
- `response_id`
- `request_params`
- `response_params`
- `field_labels`
- `is_active`

关键约束：
- `command_id` 唯一
- `request_id` 唯一
- `response_id` 唯一
- 同一命令中 `request_id != response_id`

补充说明：
- `field_labels` 为已同步字段的 `id -> name` 映射
- 当前只提取命令对象第一层列表中的字段定义，不递归更深层嵌套结构

### 6.2 `UserCommandPermission`
普通用户与命令之间的直接授权关系。

关键约束：
- `(user, command)` 唯一

### 6.3 `UserProfile`
用户扩展信息模型。

当前字段：
- `user`
- `phone`

### 6.4 `CommandLog`
命令执行日志。

关键字段：
- `user`
- `operator_username`
- `command`
- `partition`
- `request_data`
- `request_content`
- `response_data`
- `status`
- `ip_address`
- `created_at`

状态枚举：
- `success`
- `failed`
- `timeout`

### 6.5 `LoginLog`
登录行为日志。

关键字段：
- `user`
- `username`
- `action`
- `ip_address`
- `user_agent`
- `reason`
- `created_at`

动作枚举：
- `login`
- `logout`
- `login_failed`

---

## 7. 权限模型

### 7.1 设计原则
1. 命令权限直接按用户分配
2. 超级管理员身份只以 Django `is_superuser` 为准
3. `UserProfile` 只承载扩展资料，不承担权限职责

### 7.2 行为规则
- 超级管理员自动拥有全部活跃命令权限
- 普通用户只能访问显式授权的命令
- 普通用户不能将目标用户提升为超级管理员
- 超级管理员用户不能被删除
- 用户不能禁用自己
- 编辑超级管理员时，其启用状态受保护

---

## 8. 命令定义文件与同步机制

### 8.1 文件位置
项目默认使用根目录下的：

```text
idip_commands.json
```

### 8.2 命令定义结构
单个命令对象当前约定示例如下：

```json
{
  "10226000": {
    "tab": "查询个人信息(Query personal information)",
    "request_desc": "查询个人信息请求(Personal information query request)",
    "request_id": 4097,
    "request": "QueryUsrInfoReq",
    "respone_desc": "查询个人信息应答(Personal information query response)",
    "response_id": 4098,
    "respone": "QueryUsrInfoRsp",
    "QueryUsrInfoReq": [],
    "QueryUsrInfoRsp": []
  }
}
```

说明：
- 顶层请求协议 ID 使用 `request_id`
- 顶层响应协议 ID 使用 `response_id`
- 请求/响应参数列表中的 `id` 表示字段名，不要与协议 ID 字段混淆

### 8.3 同步逻辑
`gmtool.command_parser` 会将 JSON 定义同步到数据库：

- JSON 有、数据库无 → 创建命令
- JSON 有、数据库有 → 更新命令
- JSON 无、数据库有 → 标记命令为停用

在线新增命令、上传命令定义文件后，也会自动触发同步。

### 8.4 一致性与回滚
在以下场景中，系统会尽量保证文件与数据库状态一致：

- 上传新的命令定义 JSON
- 在线新增命令

若更新 JSON 后数据库同步失败，系统会尝试回滚到原始 JSON 快照，避免磁盘状态与数据库状态分叉。

### 8.5 文件监控
项目包含 `IDIPFileMonitorMiddleware`，用于检测命令定义文件变化并自动同步。

定位：
- 这是开发便利功能
- 更适合单进程、本地联调环境
- 生产环境建议关闭，并改用显式同步方式

相关配置：
- `ENABLE_IDIP_FILE_MONITOR`
- `IDIP_FILE_CHECK_INTERVAL`
- `IDIP_USE_HASH_CHECK`

### 8.6 JSON 格式化命令
为了统一 `idip_commands.json` 中每个命令对象的字段顺序，提供管理命令：

```bash
python manage.py format_idip_commands
```

仅检查是否需要格式化：

```bash
python manage.py format_idip_commands --check
```

当前字段顺序规则为：

1. `tab`
2. `request_desc`
3. `request_id`
4. `request`
5. `respone_desc`
6. `response_id`
7. `respone`
8. 请求参数定义
9. 响应参数定义
10. 其他扩展字段

---

## 9. IDIP 调用说明

### 9.1 请求方式
系统向配置项 `IDIP_API_URL` 发起请求，当前使用：

- `POST`
- `Content-Type: application/x-www-form-urlencoded`

主要参数包括：
- `id`
- `GSA`
- `content`

### 9.2 Python 调用返回值
`send_idip_command(command, params)` 当前返回：

```python
(response_data, error_message, request_content_str, error_type)
```

### 9.3 响应包装支持
系统支持如下包装结构：

```json
{
  "status": true,
  "id": 123,
  "json": {}
}
```

或：

```json
{
  "status": false,
  "id": 123,
  "json": {
    "Result": 5001,
    "RetMsg": "error"
  }
}
```

---

## 10. 路由说明

### 10.1 项目级路由
项目入口在 `mysite/urls.py`，主要挂载：

- `/gmtool/`
- `/i18n/`
- `/jsi18n/`
- `/` 重定向到 `/gmtool/`

### 10.2 应用路由
`gmtool/urls.py` 当前主要包括：

#### 认证
- `/gmtool/`
- `/gmtool/login/`
- `/gmtool/logout/`

#### 命令管理
- `/gmtool/commands/`
- `/gmtool/commands/add/`
- `/gmtool/commands/<cmd_id>/execute/`

#### 用户管理
- `/gmtool/users/`
- `/gmtool/users/create/`
- `/gmtool/users/<user_id>/edit/`
- `/gmtool/users/<user_id>/delete/`
- `/gmtool/users/<user_id>/permissions/`

#### 日志
- `/gmtool/logs/`
- `/gmtool/logs/login/`

#### API
- `/gmtool/api/v1/commands/sync/`
- `/gmtool/api/v1/commands/upload/`
- `/gmtool/api/v1/logs/<log_id>/`

---

## 11. 配置说明

主要配置位于：

- `mysite/settings.py`
- `.env`

### 11.1 运行相关
- `DJANGO_DEBUG`
- `DJANGO_SECRET_KEY`
- `DJANGO_ALLOWED_HOSTS`

### 11.2 数据库
- `DB_ENGINE`
- `DB_NAME`
- `DB_SQLITE_TIMEOUT`
- `DB_USER`
- `DB_PASSWORD`
- `DB_HOST`
- `DB_PORT`
- `DB_CONN_MAX_AGE`

默认 `DB_ENGINE=sqlite`。生产环境如需使用 MySQL，设置 `DB_ENGINE=mysql` 并提供 MySQL 连接参数。

### 11.3 IDIP 与命令定义
- `IDIP_API_URL`
- `IDIP_TIMEOUT`
- `IDIP_JSON_PATH`
- `UPLOAD_MAX_SIZE`

### 11.4 命令执行与展示
- `BATCH_EXECUTE_MAX_TARGETS`
- `BATCH_EXECUTE_INTERVAL_MS`
- `PAGE_SIZE`

### 11.5 文件监控
- `ENABLE_IDIP_FILE_MONITOR`
- `IDIP_FILE_CHECK_INTERVAL`
- `IDIP_USE_HASH_CHECK`

### 11.6 登录安全
- `LOGIN_MAX_ATTEMPTS`
- `LOGIN_LOCKOUT_SECONDS`

### 11.7 日志脱敏
- `SENSITIVE_FIELDS`

### 11.8 代理 / Cookie / HTTPS
- `DJANGO_TRUSTED_PROXY`
- `DJANGO_TRUSTED_PROXY_COUNT`
- `SESSION_COOKIE_SECURE`
- `CSRF_COOKIE_SECURE`

### 11.9 默认行为说明
- 开发环境默认 `DEBUG=True`
- 非开发环境必须显式配置：
  - `DJANGO_SECRET_KEY`
  - `DJANGO_ALLOWED_HOSTS`
- 默认 `LANGUAGE_CODE='zh-hans'`
- 默认时区为 `Asia/Shanghai`

---

## 12. 安全设计

当前已启用的基础安全措施包括：

- 登录失败限速
- 登录跳转地址安全校验
- 登出仅允许 `POST`
- 日志敏感字段脱敏
- `SESSION_COOKIE_HTTPONLY`
- `SESSION_COOKIE_SECURE`
- `CSRF_COOKIE_SECURE`
- `SECURE_PROXY_SSL_HEADER`
- `X_FRAME_OPTIONS = 'DENY'`
- `SECURE_CONTENT_TYPE_NOSNIFF`
- `SECURE_REFERRER_POLICY = 'same-origin'`
- `SECURE_CROSS_ORIGIN_OPENER_POLICY = 'same-origin'`

### 12.1 反向代理部署建议
如果使用 Nginx、SLB、Ingress 等反向代理：

- 透传 `X-Forwarded-Proto: https`
- 禁止外部流量绕过代理直接访问 Django
- 仅在确认可信代理链路时启用 `DJANGO_TRUSTED_PROXY`
- 根据代理层数设置 `DJANGO_TRUSTED_PROXY_COUNT`

---

## 13. 日志体系

### 13.1 数据库日志
- `CommandLog`
- `LoginLog`

### 13.2 文件日志
日志目录为：

```text
logs/
```

主要日志器：
- `gmtool.audit`
- `gmtool`

默认输出文件：
- `logs/audit.log`
- `logs/gmtool.log`

### 13.3 日志轮转
使用自定义 `SafeTimedRotatingFileHandler`：

- 适配 Windows 文件锁场景
- 默认按天轮转
- 默认保留 30 份历史日志

---

## 14. 管理命令

### 14.1 同步命令定义
```bash
python manage.py sync_commands
```

### 14.2 格式化命令定义文件
```bash
python manage.py format_idip_commands
```

检查模式：
```bash
python manage.py format_idip_commands --check
```

### 14.3 导出 SQLite 数据
```bash
python manage.py export_db_data
```

常用示例：
```bash
python manage.py export_db_data --db db.sqlite3 --output database_data.sql
python manage.py export_db_data --tables auth_user gmtool_gmcommand
```

### 14.4 导出 SQLite 表结构
```bash
python manage.py export_db_schema
```

常用示例：
```bash
python manage.py export_db_schema --output database_schema.sql
python manage.py export_db_schema --no-format
```

### 14.5 生成 Django 密码哈希
```bash
python manage.py generate_password_hash mypassword
```

交互输入模式：
```bash
python manage.py generate_password_hash
```

仅输出哈希值：
```bash
python manage.py generate_password_hash mypassword --raw
```

### 14.6 生成 Django SECRET_KEY
```bash
python manage.py generate_secret_key
```

指定长度并仅输出密钥内容：
```bash
python manage.py generate_secret_key --length 64 --raw
```

### 14.7 超级管理员说明
使用 Django 自带命令创建超级管理员：

```bash
python manage.py createsuperuser
```

创建后系统会自动补齐 `UserProfile`，且超级管理员可直接访问全部活跃命令。

---

## 15. 本地开发与联调

### 15.1 创建环境
项目提供 `environment.yml`：

```bash
conda env create -f environment.yml
conda activate django-admin
```

### 15.2 初始化数据库
```bash
python manage.py migrate
python manage.py sync_commands
```

### 15.3 创建超级管理员
```bash
python manage.py createsuperuser
```

### 15.4 启动项目
```bash
python manage.py runserver
```

访问地址：

```text
http://127.0.0.1:8000/gmtool/
```

### 15.5 启动本地 Mock IDIP 服务
```bash
python test/mock_idip_server.py
```

默认地址：

```text
http://127.0.0.1:18080/cy_idip
```

---

## 16. MySQL 部署

### 16.1 安装依赖
更新 conda 环境，安装 Django MySQL 驱动：

```bash
conda env update -f environment.yml
conda activate django-admin
```

### 16.2 创建 MySQL 库和账号
建议使用 MySQL 8.0 或更高版本，并启用 `utf8mb4`：

```sql
CREATE DATABASE gmtool CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'gmtool_user'@'%' IDENTIFIED BY 'strong_password_here';
GRANT ALL PRIVILEGES ON gmtool.* TO 'gmtool_user'@'%';
FLUSH PRIVILEGES;
```

如果 Django 与 MySQL 部署在同一台机器，也可以把账号主机限制为 `localhost` 或 `127.0.0.1`。

### 16.3 配置生产环境变量
`.env` 示例：

```env
DJANGO_DEBUG=False
DJANGO_SECRET_KEY=替换为生产密钥
DJANGO_ALLOWED_HOSTS=gm.example.com,admin.example.com

DB_ENGINE=mysql
DB_NAME=gmtool
DB_USER=gmtool_user
DB_PASSWORD=strong_password_here
DB_HOST=127.0.0.1
DB_PORT=3306
DB_CONN_MAX_AGE=600

ENABLE_IDIP_FILE_MONITOR=False
SESSION_COOKIE_SECURE=True
CSRF_COOKIE_SECURE=True
```

MySQL 连接会使用 `utf8mb4`，并在连接初始化时启用 `STRICT_TRANS_TABLES`。

### 16.4 初始化空库
当前流程按空 MySQL 库部署，不迁移现有 SQLite 数据：

```bash
python manage.py migrate
python manage.py sync_commands
python manage.py createsuperuser
python manage.py collectstatic --noinput
python manage.py check --deploy
```

初始化后访问 `/gmtool/`，用新建超级管理员账号登录，并验证命令列表、命令执行、登录日志和命令日志写入正常。

### 16.5 常见问题
- `ModuleNotFoundError: MySQLdb`：确认已执行 `conda env update -f environment.yml`，并在 `django-admin` 环境中运行项目。
- `Access denied for user`：检查 `DB_USER`、`DB_PASSWORD`、账号授权主机和 MySQL 服务端监听地址。
- 中文或 JSON 内容乱码：确认数据库字符集为 `utf8mb4`。
- `check --deploy` 提示 Cookie 或 Host 配置风险：按生产域名和 HTTPS 代理实际情况修正 `.env`。

---

## 17. 生产部署检查清单

### 17.1 基础配置
- 设置 `DJANGO_DEBUG=False`
- 设置安全的 `DJANGO_SECRET_KEY`
- 设置正确的 `DJANGO_ALLOWED_HOSTS`
- MySQL 部署时设置 `DB_ENGINE=mysql` 及完整数据库连接参数

### 17.2 HTTPS / 代理
- 确认 HTTPS 已启用
- 确认代理透传 `X-Forwarded-Proto: https`
- 确认 `SESSION_COOKIE_SECURE=True`
- 确认 `CSRF_COOKIE_SECURE=True`
- 确认 Django 无法被绕过代理直接访问
- 确认 `request.is_secure()` 在线上表现正确

### 17.3 Django 自检
```bash
python manage.py check --deploy
```

### 17.4 命令链路验证
- 验证至少一条成功命令
- 验证至少一条失败命令
- 验证命令日志正常落库
- 验证上传命令定义与同步流程正常

---

## 18. 国际化

当前支持语言：

- `zh-hans`
- `en`

常用命令：

```bash
python manage.py makemessages -l zh_Hans
python manage.py makemessages -l en
python manage.py compilemessages
```

---

## 19. 维护建议

- 新增配置项后，同步更新：
  - `mysite/settings.py`
  - `.env`
  - `README.md`

- 调整命令定义结构后，同步更新：
  - `idip_commands.json`
  - `gmtool/command_parser.py`
  - `README.md`

- 新增路由、模型、管理命令或安全策略后，及时同步更新文档

---

## 20. 备注

- 当前默认数据库为 SQLite，更适合开发与轻量部署
- 生产环境可通过 `DB_ENGINE=mysql` 切换到 MySQL
- 若要进入更高并发或更复杂生产环境，可继续评估 Redis、任务队列等组件
- 当前命令定义字段已统一使用 `response_id`
