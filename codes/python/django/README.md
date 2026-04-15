# 游戏 GM 命令后台管理系统（Django）

基于 Django 的 GM 命令后台管理平台，提供命令执行、权限控制、日志审计、命令定义同步与中英文国际化等能力。

---

## 1. 项目简介

本项目用于承载游戏 GM 命令的后台管理场景，核心目标是：

- 为不同用户提供受控的 GM 命令执行入口
- 支持命令定义从 `idip_commands.json` 解析并同步到数据库
- 支持命令动态表单渲染与结构化结果展示
- 支持用户、角色、命令权限管理
- 记录登录日志、命令执行日志与审计日志
- 支持通过反向代理部署在 HTTPS 环境下运行
- 支持中英文国际化

---

## 2. 当前特性

### 2.1 认证与权限
- 用户登录 / 登出
- 登录失败限速
- 超级管理员保护
- 用户级命令权限控制（`UserCommandPermission`）
- 用户分组角色（`Role`）与扩展资料（`UserProfile`）

### 2.2 命令管理
- 命令列表按分组展示
- 动态表单执行 GM 命令
- 支持批量执行参数下发
- 命令在线新增
- 命令定义 JSON 上传并自动同步
- 命令停用后自动禁止执行

### 2.3 日志与审计
- 登录日志 `LoginLog`
- 命令执行日志 `CommandLog`
- 审计日志 `gmtool.audit`
- 应用日志 `gmtool`
- 日志详情敏感字段脱敏

### 2.4 运行与配置
- `.env` 驱动核心配置
- 支持 Nginx / 反向代理场景
- 支持 `request.is_secure()` 正确识别 HTTPS
- 支持命令定义文件自动监控同步
- 支持国际化（`zh-hans` / `en`）

---

## 3. 技术栈

| 类别 | 技术 |
|---|---|
| 后端框架 | Django 5.2.12 |
| Python | 3.11 |
| 数据库 | SQLite（当前默认） |
| 前端 UI | Tabler（CDN） / Bootstrap 5 |
| 图标 | Tabler Icons |
| HTTP 客户端 | requests |
| 配置管理 | python-decouple |
| 国际化 | Django i18n |
| 日志轮转 | 自定义 `SafeTimedRotatingFileHandler` |

---

## 4. 项目结构

```text
c:\helin\helinljn\codes\python\django\
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
│   ├── __init__.py
│   ├── api_views.py
│   ├── apps.py
│   ├── audit_log.py
│   ├── auth_views.py
│   ├── command_parser.py
│   ├── command_views.py
│   ├── decorators.py
│   ├── forms.py
│   ├── idip_client.py
│   ├── logging_handlers.py
│   ├── middleware.py
│   ├── models.py
│   ├── signals.py
│   ├── urls.py
│   ├── user_views.py
│   ├── utils.py
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
项目级配置与入口：

- `settings.py`：项目配置、环境变量读取、安全设置、日志、缓存等
- `urls.py`：项目级路由与错误处理入口
- `asgi.py` / `wsgi.py`：部署入口

### 5.2 `gmtool/`
业务主应用：

- `auth_views.py`：登录、登出、错误页、CSRF 失败处理
- `command_views.py`：仪表盘、命令列表、命令执行、同步、操作日志
- `user_views.py`：用户管理、角色管理、权限分配、登录日志
- `api_views.py`：日志详情、命令定义上传接口
- `models.py`：命令、角色、用户扩展、日志、权限关联等模型
- `forms.py`：用户、角色、命令新增相关表单
- `decorators.py`：超级管理员与命令权限装饰器
- `command_parser.py`：命令定义解析与同步逻辑
- `idip_client.py`：与 IDIP 服务交互
- `middleware.py`：命令定义文件变更监控
- `signals.py`：超管角色/权限自动补齐
- `audit_log.py`：审计日志封装
- `logging_handlers.py`：Windows 友好的日志轮转处理器

### 5.3 `test/mock_idip_server.py`
本地联调用的 Mock IDIP 服务，用于在开发环境验证命令链路。

---

## 6. 权限模型

### 6.1 设计原则
1. **命令权限按用户直接分配**
2. **角色只做分组标签，不直接承载命令权限**
3. 超级管理员兼容两种判断来源：
   - Django 内置：`user.is_superuser`
   - 业务角色：`user.userprofile.role.is_super_admin`

### 6.2 涉及模型
- `Role`
- `UserProfile`
- `UserCommandPermission`

### 6.3 已实现的保护规则
- 普通用户编辑流程中不能把目标用户提升为超级管理员
- 超级管理员角色不能编辑或删除
- 超级管理员用户不能删除
- 用户不能禁用自己
- 编辑超级管理员时，角色与启用状态不可改
- 超级管理员自动拥有全部活跃命令权限

---

## 7. 数据模型概览

### 7.1 `GMCommand`
GM 命令定义模型，来源于 `idip_commands.json` 同步结果。

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

### 7.2 `Role`
用户分组角色。

关键字段：
- `name`
- `display_name`
- `description`
- `is_super_admin`

### 7.3 `UserCommandPermission`
用户与命令的直接权限关联。

关键约束：
- `UniqueConstraint(fields=['user', 'command'])`

### 7.4 `UserProfile`
用户扩展信息。

关键字段：
- `user`
- `role`
- `phone`

### 7.5 `CommandLog`
命令执行日志。

关键字段：
- `user`
- `command`
- `partition`
- `request_data`
- `request_content`
- `response_data`
- `status`
- `ip_address`
- `created_at`

### 7.6 `LoginLog`
登录行为日志。

关键字段：
- `user`
- `username`
- `action`
- `reason`
- `ip_address`
- `user_agent`
- `created_at`

---

## 8. 命令定义与同步机制

### 8.1 命令定义文件
项目根目录下使用：

```text
idip_commands.json
```

用于定义：
- 命令编号
- 请求/响应协议 ID
- 请求/响应字段结构
- 命令分组
- 展示标签

当前命令对象约定使用统一字段顺序，便于人工维护：

```json
{
  "10226000": {
    "tab": "查询个人信息(Query personal information)",
    "request_desc": "查询个人信息请求(Personal information query request)",
    "request_id": 4097,
    "request": "QueryUsrInfoReq",
    "respone_desc": "查询个人信息应答(Personal information query response)",
    "responseid": 4098,
    "respone": "QueryUsrInfoRsp",
    "QueryUsrInfoReq": [],
    "QueryUsrInfoRsp": []
  }
}
```

说明：
- 顶层请求协议 ID 使用 `request_id`
- 顶层响应协议 ID 当前仍使用历史字段名 `responseid`
- 请求/响应参数列表中的 `id` 表示字段名，例如 `AreaId`、`RoleId`、`Result`，不要改成 `request_id`

### 8.2 同步逻辑
`command_parser.py` 中的同步逻辑会将 JSON 定义映射到数据库：

- JSON 有、数据库无 → 创建命令
- JSON 有、数据库有 → 更新命令
- JSON 无、数据库有 → 标记为停用

### 8.3 触发方式
可通过以下方式触发同步：

- 管理命令：`python manage.py sync_commands`
- 在线上传 JSON 后自动同步
- 文件监控中间件检测变更后自动同步

### 8.4 命令定义格式化
为避免 `idip_commands.json` 中每个命令对象字段顺序变乱，项目提供格式化管理命令：

```bash
python manage.py format_idip_commands
```

仅检查是否符合格式，不写回文件：

```bash
python manage.py format_idip_commands --check
```

格式化规则：
1. `tab`
2. `request_desc`
3. `request_id`
4. `request`
5. `respone_desc`
6. `responseid`
7. `respone`
8. 请求参数定义
9. 响应参数定义
10. 其他结构体 / 扩展字段

该命令只调整 JSON 字段显示顺序，不修改协议 ID、参数定义、命令名或文案内容。

---

## 9. IDIP 调用协议

### 9.1 请求方式
向配置项 `IDIP_API_URL` 发起：

- `POST`
- `Content-Type: application/x-www-form-urlencoded`

请求参数：
- `id`
- `GSA`
- `content`

### 9.2 Python 返回值
`send_idip_command(command, params)` 返回：

```python
(response_data, error_message, request_content_str, error_type)
```

### 9.3 包装响应支持
支持如下格式：

```json
{
  "status": true,
  "id": 123,
  "json": {}
}
```

以及：

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
`mysite/urls.py`

- `/i18n/`
- `/jsi18n/`
- `/gmtool/`
- `/` → 重定向到 `/gmtool/`

### 10.2 gmtool 路由
`gmtool/urls.py`

主要包括：

- 仪表盘：`/gmtool/`
- 登录：`/gmtool/login/`
- 登出：`/gmtool/logout/`
- 命令列表：`/gmtool/commands/`
- 命令执行：`/gmtool/commands/<cmd_id>/execute/`
- 新增命令：`/gmtool/commands/add/`
- 用户管理：`/gmtool/users/...`
- 角色管理：`/gmtool/roles/...`
- 权限分配：`/gmtool/users/<id>/permissions/`
- 命令日志：`/gmtool/logs/`
- 登录日志：`/gmtool/logs/login/`
- 同步接口：`/gmtool/api/v1/commands/sync/`
- 上传接口：`/gmtool/api/v1/commands/upload/`
- 日志详情接口：`/gmtool/api/v1/logs/<id>/`

---

## 11. 配置说明

项目配置主要位于：

- `mysite/settings.py`
- `.env`

### 11.1 配置组织方式
当前 `settings.py` 已按以下结构整理：

1. Core runtime settings
2. Django apps / middleware / templates
3. Database
4. Auth / password validation
5. Internationalization
6. Static files
7. Application settings
8. Proxy / session / CSRF
9. Security headers
10. Cache
11. Logging

`.env` 也按近似顺序进行了分组，便于维护。

### 11.2 常用环境变量
常用环境变量包括：

- `DJANGO_DEBUG`
- `DJANGO_SECRET_KEY`
- `DJANGO_ALLOWED_HOSTS`
- `IDIP_API_URL`
- `IDIP_TIMEOUT`
- `IDIP_JSON_PATH`
- `UPLOAD_MAX_SIZE`
- `BATCH_EXECUTE_MAX_TARGETS`
- `BATCH_EXECUTE_INTERVAL_MS`
- `PAGE_SIZE`
- `ENABLE_IDIP_FILE_MONITOR`
- `IDIP_FILE_CHECK_INTERVAL`
- `IDIP_USE_HASH_CHECK`
- `LOGIN_MAX_ATTEMPTS`
- `LOGIN_LOCKOUT_SECONDS`
- `SENSITIVE_FIELDS`
- `DJANGO_TRUSTED_PROXY`
- `DJANGO_TRUSTED_PROXY_COUNT`
- `SESSION_COOKIE_SECURE`
- `CSRF_COOKIE_SECURE`

### 11.3 Secure Cookie 与 HTTPS 感知
当前项目采用以下策略：

- **反向代理层负责**
  - HTTP → HTTPS 跳转
  - HSTS

- **Django 负责**
  - `SESSION_COOKIE_SECURE`
  - `CSRF_COOKIE_SECURE`
  - `SECURE_PROXY_SSL_HEADER`

这样在 Nginx / SLB / Ingress 场景下，Django 可以正确识别：

```python
request.is_secure()
```

---

## 12. 安全设计

### 12.1 已启用的安全措施
- 登录失败限速
- 登录跳转地址安全校验
- 登出仅允许 `POST`
- 删除确认令牌
- 敏感日志脱敏
- `SESSION_COOKIE_HTTPONLY`
- `SESSION_COOKIE_SECURE`
- `CSRF_COOKIE_SECURE`
- `SECURE_PROXY_SSL_HEADER`
- `X_FRAME_OPTIONS = 'DENY'`
- `SECURE_CONTENT_TYPE_NOSNIFF`
- `SECURE_REFERRER_POLICY = 'same-origin'`

### 12.2 反向代理部署建议
若使用 Nginx 等反向代理：

- 确保代理透传 `X-Forwarded-Proto: https`
- 确保外部流量不能绕过代理直接访问 Django
- 若信任 `X-Forwarded-For`，再开启 `DJANGO_TRUSTED_PROXY=True`
- 根据代理层数设置 `DJANGO_TRUSTED_PROXY_COUNT`

---

## 13. 日志体系

### 13.1 数据库日志
- `CommandLog`
- `LoginLog`

### 13.2 文件日志
- `logs/audit.log`
- `logs/gmtool.log`

### 13.3 日志轮转
使用自定义 `SafeTimedRotatingFileHandler`：

- 适配 Windows 文件锁场景
- 轮转失败时尽量不中断日志写入
- 默认按天轮转，保留 30 份

---

## 14. 管理命令

### 14.1 同步命令定义
```bash
python manage.py sync_commands
```

### 14.2 初始化角色与超管权限
```bash
python manage.py init_roles
```

### 14.3 格式化命令定义文件
格式化 `idip_commands.json` 中每个命令对象的字段顺序：

```bash
python manage.py format_idip_commands
```

仅检查是否需要格式化：

```bash
python manage.py format_idip_commands --check
```

### 14.4 导出 SQLite 数据
导出当前 SQLite 数据到 SQL 文件（仅数据，不含建表语句）：

```bash
python manage.py export_db_data
```

常用参数：

```bash
python manage.py export_db_data --db db.sqlite3 --output database_data.sql
python manage.py export_db_data --tables auth_user gmtool_gmcommand
```

### 14.5 导出 SQLite 表结构
导出 SQLite 数据库表结构到 SQL 文件：

```bash
python manage.py export_db_schema
```

常用参数：

```bash
python manage.py export_db_schema --output database_schema.sql
python manage.py export_db_schema --no-format
```

### 14.6 生成 Django 密码哈希
生成与 Django `auth_user.password` 字段一致格式的 `pbkdf2_sha256` 密码哈希：

```bash
python manage.py generate_password_hash mypassword
```

不在命令行直接传明文密码时，会进入安全输入模式：

```bash
python manage.py generate_password_hash
```

仅输出哈希结果：

```bash
python manage.py generate_password_hash mypassword --raw
```

指定 salt：

```bash
python manage.py generate_password_hash mypassword --salt customsalt
```

### 14.7 生成 Django SECRET_KEY
生成 Django `SECRET_KEY`：

```bash
python manage.py generate_secret_key
```

指定长度并仅输出密钥内容：

```bash
python manage.py generate_secret_key --length 64 --raw
```

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
python manage.py init_roles
```

### 15.3 创建管理员
```bash
python manage.py createsuperuser
```

### 15.4 启动项目
```bash
python manage.py runserver
```

访问：

```text
http://127.0.0.1:8000/gmtool/
```

### 15.5 启动 Mock IDIP 服务
```bash
python test/mock_idip_server.py
```

默认联调地址：

```text
http://127.0.0.1:18080/cy_idip
```

---

## 16. 生产部署检查清单

上线前建议至少检查以下内容：

### 16.1 基础配置
- 设置 `DJANGO_DEBUG=False`
- 设置安全的 `DJANGO_SECRET_KEY`
- 设置正确的 `DJANGO_ALLOWED_HOSTS`

### 16.2 HTTPS / 代理
- 确认 HTTPS 已启用
- 确认代理透传 `X-Forwarded-Proto: https`
- 确认 `SESSION_COOKIE_SECURE=True`
- 确认 `CSRF_COOKIE_SECURE=True`
- 确认 Django 无法被绕过代理直接访问
- 确认 `request.is_secure()` 在线上表现正确

### 16.3 Django 自检
```bash
python manage.py check --deploy
```

### 16.4 命令链路验证
- 验证至少一条成功命令
- 验证至少一条失败命令
- 验证日志是否正常落库
- 验证上传命令定义与同步是否正常

---

## 17. 国际化

当前支持：

- `zh-hans`
- `en`

常用命令：

```bash
python manage.py makemessages -l zh_Hans
python manage.py makemessages -l en
python manage.py compilemessages
```

---

## 18. 维护建议

- 新增配置项后，同步更新：
  - `mysite/settings.py`
  - `.env`
  - `README.md`

- 新增路由、视图、模型后，同步更新文档说明
- 调整安全策略后，同步更新部署章节
- 调整日志策略后，同步更新日志章节

---

## 19. 备注

- 本项目不依赖 Django Admin 进行业务管理
- 角色仅用于分组，不直接承载命令权限
- 命令权限以 `UserCommandPermission` 为准
- 当前默认数据库为 SQLite，更适合开发或轻量部署场景
- 若进入更高并发或生产化环境，建议评估 MySQL / PostgreSQL、Redis、任务队列等组件
