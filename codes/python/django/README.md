# 游戏 GM 命令后台管理系统（Django）

基于 Django 的 GM 命令后台管理平台，提供命令执行、权限控制、命令定义同步、日志审计、国际化与基础安全防护能力。

---

## 1. 项目简介

本项目用于承载游戏 GM 命令后台管理场景，核心目标是：

- 为不同用户提供受控的 GM 命令执行入口
- 从 `idip_commands.json` 解析命令定义并同步到数据库
- 根据命令定义动态渲染请求表单并展示响应结果
- 支持用户管理、命令权限分配、审核管理、登录日志与命令执行日志
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
- 在线新增命令并写入 `idip_commands.json`
- 超级管理员可上传命令定义 JSON 并自动同步数据库
- JSON 中不存在的命令自动标记为停用
- 停用命令不可继续执行

### 2.3 公告管理
- 查询、发布、删除游戏公告
- 公告接口独立于 IDIP 命令，调用目录服 Web PHP 接口
- 查询公告支持平台单选、渠道多选；发布公告支持平台单选、渠道多选、公告类型单选；删除在查询结果中进行（单条删除按钮或批量删除），按每条公告自身的渠道、类型、ID 删除，无独立删除页
- 目录服不支持批量提交，多渠道查询会按渠道逐个调用接口并按渠道分组展示；多渠道发布会按渠道逐个提交公告审核
- 发布公告类型包括周更新公告、常驻公告、轮播图；轮播图无需填写标题和正文，周更新公告/常驻公告不填写图片字段
- 查询结果详情按公告类型展示字段：周更新公告、常驻公告只显示标题和正文；轮播图只显示图片链接；预留字段不在详情弹窗中展示
- 周更新公告审核通过发布前会先查询并删除同平台同渠道旧周更新公告
- 超级管理员自动拥有公告管理权限，普通用户需单独授权
- 发布公告页面只提交审核并写 audit log；审核通过/重试后实际发布并写公告操作日志；删除写公告操作日志和 audit log，查询不写操作日志

### 2.4 审核管理
- 公告审核、邮件审核、走马灯审核统一在审核管理导航下
- 公告发布需先提交 `AnnouncementReview`，审核通过后才调用目录服发布
- 邮件命令和走马灯发布命令需先提交 `CommandReview`，审核通过后才调用 IDIP；走马灯查询和删除命令不需要审核
- 邮件/走马灯审核权限来自对应命令权限；公告审核权限来自公告管理权限；提交人不能审核自己的记录
- 审核通过或重试时先标记为处理中，再调用远端服务，最后更新为已通过或失败

### 2.5 日志与审计
- 登录日志 `LoginLog`
- 命令执行日志 `CommandLog`
- 公告操作日志 `AnnouncementLog`
- 审计日志 `gmtool.audit`
- 应用日志 `gmtool`
- 日志详情接口自动脱敏敏感字段
- 若远端命令执行成功但本地日志写入失败，接口会返回 `warning` 与 `log_persisted=false`
- 若远端公告操作成功但本地公告日志写入失败，页面会保留远端成功结果并提示 warning

### 2.6 配置与运行
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
| Python | 3.12.13 |
| 数据库 | SQLite（默认）/ MySQL（可配置） |
| HTTP 客户端 | requests |
| 配置管理 | python-decouple |
| 国际化 | Django i18n |
| 前端 | Django Template + Tabler/Bootstrap 风格页面 |
| 日志轮转 | 自定义 `SafeTimedRotatingFileHandler` |

---

## 4. 项目结构

```text
C:\helin\helinljn\codes\python\django\
├── .env
├── manage.py
├── db.sqlite3
├── idip_commands.json
├── AGENTS.md
├── README.md
├── scripts/
│   ├── django-manager.sh
│   ├── my.cnf
│   ├── mysql-manager.sh
│   ├── redis.conf
│   └── redis-manager.sh
├── mysite/
│   ├── __init__.py
│   ├── asgi.py
│   ├── settings.py
│   ├── urls.py
│   └── wsgi.py
├── gmtool/
│   ├── announcement_client.py
│   ├── announcement_log_services.py
│   ├── announcement_review_services.py
│   ├── announcement_views.py
│   ├── api_views.py
│   ├── audit_log.py
│   ├── auth_views.py
│   ├── command_parser.py
│   ├── command_review_services.py
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
│   ├── review_views.py
│   ├── security_utils.py
│   ├── signals.py
│   ├── tests.py
│   ├── urls.py
│   ├── utils.py
│   ├── user_views.py
│   ├── views.py
│   ├── management/commands/
│   ├── migrations/
│   ├── static/gmtool/
│   ├── templates/gmtool/
│   └── templatetags/
│       ├── __init__.py
│       └── gmtool_tags.py
├── locale/
├── logs/
└── test/
    ├── __init__.py
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
- `announcement_client.py`：公告目录服 HTTP 调用
- `announcement_log_services.py`：公告远端发布/删除日志写入封装
- `announcement_review_services.py`：公告提交审核、审核通过发布、作废、重试等业务逻辑
- `announcement_views.py`：公告查询、提交发布审核、删除和公告日志列表
- `command_views.py`：仪表盘、命令列表、命令执行、命令新增、命令同步、命令日志
- `command_review_services.py`：邮件和走马灯 GM 命令审核、审核权限判断、审核通过后执行 IDIP
- `review_views.py`：审核管理入口、公告审核、邮件审核、走马灯审核列表与操作
- `user_views.py`：用户管理、用户编辑、删除、权限分配、登录日志查看
- `api_views.py`：命令定义上传接口、命令日志详情接口、公告日志详情接口
- `forms.py`：用户、命令新增、公告查询/发布/删除等页面表单校验
- `decorators.py`：超级管理员、命令执行权限和公告管理权限装饰器
- `command_parser.py`：命令定义文件解析、ID 校验、JSON 快照读写、数据库同步
- `command_services.py`：命令执行前的参数校验与命令查询辅助逻辑
- `query_utils.py`：分页与时间范围筛选辅助函数，供列表页复用
- `permission_service.py`：超级管理员判定等权限辅助逻辑
- `idip_client.py`：与远端 IDIP 服务通信
- `context_processors.py`：模板导航权限上下文
- `middleware.py`：命令定义文件变更监控
- `security_utils.py`：脱敏与安全辅助函数
- `audit_log.py`：审计日志封装
- `logging_handlers.py`：Windows 友好的日志轮转处理器
- `models.py`：命令、权限、用户扩展、日志等模型
- `signals.py`：自动补齐 `UserProfile`
- `views.py`：通用错误页与 CSRF 失败处理
- `utils.py`：模板上下文等轻量辅助函数
- `tests.py`：Django 单元测试
- `static/gmtool/`：项目自定义 CSS 与本地 Tabler Icons 资源
- `templates/gmtool/`：业务页面模板与 400/403/404/500 错误页
- `templatetags/gmtool_tags.py`：模板标签

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
- `request_params` / `response_params` 会保留结构体列表的展开元数据，用于请求表单渲染和响应结果展示

### 6.2 `UserCommandPermission`
普通用户与命令之间的直接授权关系。

关键约束：
- `(user, command)` 唯一

### 6.3 `UserAnnouncementPermission`
普通用户的公告管理权限。

关键约束：
- `user` 唯一

说明：
- 超级管理员自动拥有公告管理权限，不需要写入此表
- 普通用户存在此记录即拥有公告管理权限

### 6.4 `UserProfile`
用户扩展信息模型。

当前字段：
- `user`
- `phone`

### 6.5 `CommandLog`
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

### 6.6 `AnnouncementLog`
公告远端发布、删除操作日志。发布公告审核通过/重试时写入的公告日志按提交人记录操作人，审核动作本身的 audit log 按审核人记录。

关键字段：
- `user`
- `operator_username`
- `action`
- `platform`
- `channel`
- `announcement_type`
- `announcement_id`
- `request_data`
- `response_data`
- `raw_response`
- `status`
- `error_message`
- `ip_address`
- `created_at`

动作枚举：
- `create`
- `delete`

状态枚举：
- `success`
- `failed`
- `timeout`

### 6.7 `AnnouncementReview`
公告待审核记录。发布公告页面只创建审核记录，不直接调用目录服。

关键字段：
- `status`
- `submitter`
- `submitter_username`
- `reviewer`
- `reviewer_username`
- `platform`
- `channel`
- `announcement_type`
- `announcement_id`
- `title`
- `content`
- `priority`
- `image_1` / `image_link_1`
- `image_2` / `image_link_2`
- `image_3` / `image_link_3`
- `reserve_1` / `reserve_2`
- `payload`
- `review_comment`
- `remote_response`
- `raw_response`
- `error_message`
- `created_at`
- `reviewed_at`
- `updated_at`

状态枚举：
- `pending`：待审核
- `processing`：发布中
- `approved`：已通过
- `rejected`：已作废
- `failed`：发布失败

说明：
- 同平台、同渠道、同类型的周更新公告仅允许存在一条待审核记录
- 审核通过和重试会先标记为 `processing`，再在事务外调用目录服，最后更新终态
- 提交人不能审核自己提交的记录

### 6.8 `CommandReview`
邮件/走马灯 GM 命令待审核记录。需要审核的命令在执行页提交后不会立即调用 IDIP。

关键字段：
- `review_type`
- `status`
- `command`
- `command_code`
- `command_name`
- `command_tab`
- `request_name`
- `request_id`
- `response_name`
- `response_id`
- `submitter`
- `submitter_username`
- `reviewer`
- `reviewer_username`
- `partition`
- `params`
- `request_content`
- `response_data`
- `review_comment`
- `error_message`
- `ip_address`
- `created_at`
- `reviewed_at`
- `updated_at`

审核类型：
- `mail`：邮件审核
- `marquee`：走马灯审核

状态枚举：
- `pending`：待审核
- `processing`：执行中
- `approved`：已通过
- `rejected`：已作废
- `failed`：执行失败

说明：
- 邮件命令按命令文本中的邮件/mail 识别
- 走马灯审核复用 `idip_commands.json` 中的公告发布类命令，查询和删除公告命令不需要审核
- 审核通过和重试会先标记为 `processing`，再在事务外调用 IDIP，最后更新终态
- 提交人不能审核自己提交的记录

### 6.9 `LoginLog`
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
3. 公告管理权限使用单独功能权限，不伪装成 GM 命令
4. 公告审核权限来自公告管理权限，邮件/走马灯审核权限来自对应命令权限
5. `UserProfile` 只承载扩展资料，不承担权限职责

### 7.2 行为规则
- 超级管理员自动拥有全部活跃命令权限
- 超级管理员自动拥有公告管理权限
- 超级管理员自动拥有公告审核、邮件审核和走马灯审核权限
- 普通用户只能访问显式授权的命令
- 普通用户需要 `UserAnnouncementPermission` 才能访问公告管理
- 普通用户拥有邮件命令权限时可访问邮件审核，拥有走马灯发布命令权限时可访问走马灯审核
- 走马灯审核只覆盖公告发布类 IDIP 命令，查询和删除公告命令不进入审核
- 公告审核、邮件审核、走马灯审核都不允许提交人审核自己的记录
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
    "response_desc": "查询个人信息应答(Personal information query response)",
    "response_id": 4098,
    "response": "QueryUsrInfoRsp",
    "QueryUsrInfoReq": [],
    "QueryUsrInfoRsp": []
  }
}
```

说明：
- 顶层请求协议 ID 使用 `request_id`
- 顶层响应协议 ID 使用 `response_id`
- 请求/响应参数列表中的 `id` 表示字段名，不要与协议 ID 字段混淆
- `command_name` 的解析优先级为 `name` -> `command_name` -> `tab`
- `request_desc` / `response_desc` 为可选描述字段，上传时不强制要求，Web 端新增命令时会写入；这两个字段仅作为 JSON 描述信息保留，不同步到 `GMCommand` 数据库字段
- 请求参数中的结构体列表需要同时提供 `*_count` 字段和列表字段；列表字段的 `type` 指向同命令内的结构体定义，并且必须配置正整数 `max_size`
- 响应参数中的结构体列表由游戏服务器返回数量决定，不要求配置 `max_size`

请求结构体列表示例：

```json
{
  "isnull": "false",
  "id": "ItemList1",
  "type": "SItemInfo",
  "name": "道具列表1(Item List 1)",
  "max_size": 5
}
```

### 8.3 同步逻辑
`gmtool.command_parser` 会将 JSON 定义同步到数据库：

- JSON 有、数据库无 → 创建命令
- JSON 有、数据库有 → 更新命令
- JSON 无、数据库有 → 标记命令为停用

在线新增命令、上传命令定义文件后，也会自动触发同步。

同步时会校验请求结构体列表的 `max_size`。缺失、非正整数或非法值会导致同步失败；响应结构体列表不受该规则限制。

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
5. `response_desc`
6. `response_id`
7. `response`
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

`content` 是扁平的原始参数 JSON 字符串，由 `requests.post(..., data=...)` 按 `application/x-www-form-urlencoded` 统一编码。抓包看到的 HTTP body 中 `content` 应只经过一次 URL 编码；服务端表单解析后应直接得到类似 `{"AreaId": 1001, "Partition": 1001, "PlatId": 1001, "OpenId": "1001"}` 的 JSON 字符串。
- `GSA` 当前固定为空串，后续如启用签名或透传值再单独调整

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

### 9.4 命令审核执行
命令执行页会先判断命令是否需要审核：

- 普通命令直接调用 `send_idip_command`
- 邮件命令创建 `CommandReview(type=mail)`，审核通过或重试时再调用 IDIP
- 走马灯发布命令创建 `CommandReview(type=marquee)`，审核通过或重试时再调用 IDIP
- 走马灯查询和删除命令不创建审核记录

审核通过或重试执行成功/失败后写 `CommandLog`，日志操作人按提交人记录；审核动作本身写 audit log，操作人按审核人记录。

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

#### 首页 / 认证
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

#### 公告管理
- `/gmtool/announcements/`：兼容入口，默认跳转查询公告页
- `/gmtool/announcements/query/`：查询公告（查询结果内进行单条/批量删除）
- `/gmtool/announcements/create/`：提交发布公告审核
- `/gmtool/announcements/batch-delete/`：批量删除公告（POST，复用为单条删除入口）
- `/gmtool/announcements/logs/`：公告日志

#### 审核管理
- `/gmtool/reviews/`：审核管理入口，跳转到当前用户可访问的第一个审核页
- `/gmtool/reviews/mail/`：邮件审核列表
- `/gmtool/reviews/marquee/`：走马灯审核列表
- `/gmtool/reviews/commands/<review_type>/<review_id>/approve/`：通过并执行邮件/走马灯审核记录（POST）
- `/gmtool/reviews/commands/<review_type>/<review_id>/reject/`：作废邮件/走马灯审核记录（POST）
- `/gmtool/reviews/commands/<review_type>/<review_id>/retry/`：重试执行失败的邮件/走马灯审核记录（POST）
- `/gmtool/reviews/announcements/`：公告审核列表
- `/gmtool/reviews/announcements/<review_id>/approve/`：通过并发布公告审核记录（POST）
- `/gmtool/reviews/announcements/<review_id>/reject/`：作废公告审核记录（POST）
- `/gmtool/reviews/announcements/<review_id>/retry/`：重试发布失败的公告审核记录（POST）

#### API
- `/gmtool/api/v1/commands/sync/`
- `/gmtool/api/v1/commands/upload/`
- `/gmtool/api/v1/logs/<log_id>/`
- `/gmtool/api/v1/announcements/logs/<log_id>/`

说明：
- `/gmtool/api/v1/commands/sync/` 与 `/gmtool/api/v1/commands/upload/` 均仅超级管理员可用

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

### 11.4 公告目录服接口
- `ANNOUNCEMENT_BASE_URL`：目录服 Web 基础地址，不包含具体 PHP 路径
- `ANNOUNCEMENT_TIMEOUT`：公告接口超时时间，默认 30 秒
- `ANNOUNCEMENT_PLATFORMS`：英文逗号分隔的平台列表，默认 `PS`
- `ANNOUNCEMENT_CHANNELS`：英文逗号分隔的渠道列表，默认 `1001`

`.env` 示例（按实际目录服配置调整）：

```env
ANNOUNCEMENT_BASE_URL=http://example.com
ANNOUNCEMENT_TIMEOUT=5
ANNOUNCEMENT_PLATFORMS=Android,IOS,OpenHarmony
ANNOUNCEMENT_CHANNELS=小米,VIVO,OPPO
```

公告调用路径由客户端拼接：
- 发布：`{ANNOUNCEMENT_BASE_URL}/server/announcementSave.php`
- 删除：`{ANNOUNCEMENT_BASE_URL}/server/announcementDelete.php`
- 查询：`{ANNOUNCEMENT_BASE_URL}/client/announcement.php`

`ANNOUNCEMENT_BASE_URL` 只允许 `http://` 或 `https://`，不能带 query string 或 fragment。未配置或配置非法时，公告页会展示配置错误；查询不会请求目录服；提交发布公告审核不要求目录服地址可用，审核通过/重试发布和删除公告时会调用目录服并按结果写公告日志用于审计。

### 11.5 命令执行与展示
- `PAGE_SIZE`

### 11.6 文件监控
- `ENABLE_IDIP_FILE_MONITOR`
- `IDIP_FILE_CHECK_INTERVAL`
- `IDIP_USE_HASH_CHECK`

### 11.7 登录安全
- `LOGIN_MAX_ATTEMPTS`
- `LOGIN_LOCKOUT_SECONDS`

### 11.8 日志脱敏
- `SENSITIVE_FIELDS`

### 11.9 代理 / Cookie / HTTPS
- `DJANGO_TRUSTED_PROXY`
- `DJANGO_TRUSTED_PROXY_COUNT`
- `SESSION_COOKIE_AGE`
- `SESSION_EXPIRE_AT_BROWSER_CLOSE`
- `SESSION_COOKIE_SECURE`
- `CSRF_COOKIE_SECURE`

### 11.10 默认行为说明
- 开发环境默认 `DEBUG=True`
- 非开发环境必须显式配置：
  - `DJANGO_SECRET_KEY`
  - `DJANGO_ALLOWED_HOSTS`
- SQLite 默认数据库文件为 `db.sqlite3`，默认连接超时为 20 秒
- 上传命令定义文件默认大小限制为 10MB
- 文件监控默认跟随 `DEBUG`：开发环境默认开启，生产环境默认关闭
- `SESSION_COOKIE_SECURE` 与 `CSRF_COOKIE_SECURE` 默认在生产环境开启，开发环境关闭
- 登录失败限速使用 Django 本地内存缓存；多进程或多实例部署时需注意限速计数不跨进程共享
- 默认 `LANGUAGE_CODE='zh-hans'`
- 默认时区为 `Asia/Shanghai`

---

## 12. 安全设计

当前已启用的基础安全措施包括：

- 登录失败限速
- 登录跳转地址安全校验
- 登出仅允许 `POST`
- 公告管理使用独立功能权限，未授权用户不能通过 URL 或 POST 绕过
- 审核管理按公告权限或命令权限控制，提交人不能审核自己的记录
- 审核通过和重试会先标记为 `processing`，再在事务外调用远端服务，避免长事务包裹远端调用
- 公告日志详情接口按 `SENSITIVE_FIELDS` 脱敏，非 JSON 原始响应和错误文本也会脱敏
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
- `AnnouncementLog`
- `LoginLog`

说明：
- 邮件/走马灯审核通过或重试后写入的 `CommandLog` 按提交人记录操作人，审核动作本身的 audit log 按审核人记录
- 公告审核通过或重试后写入的 `AnnouncementLog` 按提交人记录操作人，审核动作本身的 audit log 按审核人记录

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
Windows 或当前开发环境可使用上一层目录的 `../environment.yml`：

```bash
conda env create -f ../environment.yml
conda activate py312
```

Ubuntu 24.04 部署使用上一层目录的 `../environment-linux.yml`，见第 16 节。部署时如已将环境文件复制到项目根目录，可按实际路径调整命令。

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
bash scripts/django-manager.sh start
```

停止项目：

```bash
bash scripts/django-manager.sh stop
```

重启或查看状态：

```bash
bash scripts/django-manager.sh restart
bash scripts/django-manager.sh status
```

默认监听地址：

```text
0.0.0.0:8000
```

`scripts/django-manager.sh` 支持以下环境变量覆盖默认值：

- `DJANGO_MANAGE_PY`：`manage.py` 路径
- `DJANGO_PYTHON_BIN`：Python 可执行文件
- `DJANGO_BIND_HOST`：监听地址，默认 `0.0.0.0`
- `DJANGO_BIND_PORT`：监听端口，默认 `8000`
- `DJANGO_LOG_DIR`：运行日志目录，默认 `logs/`
- `DJANGO_LOG_FILE`：Django 运行日志文件，默认 `logs/django-server.log`
- `DJANGO_PID_FILE`：PID 文件，默认 `logs/django-server.pid`

访问地址按实际主机 IP 选择，例如：

```text
http://127.0.0.1:8000/gmtool/
```

### 15.5 启动本地 Mock IDIP 服务
```bash
python test/mock_idip_server.py
```

默认地址：

```text
http://127.0.0.1:5510/cy_idip
```

### 15.6 公告功能本地配置与使用
1. 在 `.env` 中配置 `ANNOUNCEMENT_BASE_URL`、`ANNOUNCEMENT_PLATFORMS` 和 `ANNOUNCEMENT_CHANNELS`，修改后重启 Django 进程。
2. 使用超级管理员登录 `/gmtool/`；普通用户需要先在用户权限页勾选“公告管理权限”。
3. 进入 `/gmtool/announcements/` 或 `/gmtool/announcements/query/`，在“查询公告”子页签选择平台、渠道（渠道可多选）后查询现有公告；多选渠道时每个渠道按目录服接口逐个查询，结果按渠道分组展示。
4. 查询结果详情中，周更新公告、常驻公告只显示标题和正文；轮播图只显示图片链接；预留字段不展示。
5. 在“发布公告”子页签提交公告审核，渠道可多选；多选渠道时每个渠道各创建一条 `AnnouncementReview`。发布轮播图时无需填写标题和正文；发布周更新公告或常驻公告时不填写图片字段。
6. 进入“审核管理 > 公告审核”处理待审核记录；审核通过或重试时才调用目录服发布并写公告日志。发布周更新公告会先查询并删除同平台同渠道旧周更新公告，再发布新公告。
7. 删除公告在“查询公告”结果中进行：每条公告有单条删除按钮，每个渠道分组上方有“批量删除”按钮，按所选公告自身的 `Channel`、`AnnouncementType` 和 `AnnouncementId` 删除，无独立删除子页签。
8. 进入“公告日志”子页签或 `/gmtool/announcements/logs/` 查看发布、删除日志和脱敏后的日志详情。

---

## 16. Ubuntu 24.04 部署账号建议

生产环境建议使用独立 Linux 用户运行项目。该用户只负责安装 Miniconda、维护 conda 环境、执行 Django 初始化命令和运行最终 Web 服务，不需要授予 sudo 提权。

### 16.1 创建 django 用户
以下命令由已有运维账号或 root 执行：

```bash
sudo adduser --disabled-password --gecos "" django
```

不要把 `django` 用户加入 `sudo` 组：

```bash
# 不要执行
sudo usermod -aG sudo django
```

### 16.2 准备项目目录
项目目录可按实际部署路径调整，示例使用 `/opt/gmtool`：

```bash
sudo mkdir -p /opt/gmtool
sudo chown -R django:django /opt/gmtool
```

项目代码、`.env`、`idip_commands.json`、`logs/`、`staticfiles/` 等运行期需要读写的内容，应确保归属或写入权限对 `django` 用户可用。

### 16.3 切换到 django 用户
后续 Miniconda 安装、conda 环境创建、数据库初始化和 Web 服务启动均在 `django` 用户下执行：

```bash
sudo -iu django
```

### 16.4 安装 Miniconda
建议把 Miniconda 安装到 `django` 用户家目录，例如：

```text
/home/django/miniconda3
```

在 x86_64 Ubuntu 24.04 上，可由 `django` 用户执行：

```bash
cd /home/django
wget https://mirrors.tuna.tsinghua.edu.cn/anaconda/miniconda/Miniconda3-py38_23.11.0-2-Linux-x86_64.sh -O miniconda.sh
bash miniconda.sh -b -p /home/django/miniconda3
/home/django/miniconda3/bin/conda init bash
source /home/django/.bashrc
```

其他 CPU 架构请替换为对应的 Miniconda Linux 安装脚本。

安装完成后，用 `django` 用户进入项目目录并创建或更新环境：

```bash
cd /opt/gmtool
conda env create -f ../environment-linux.yml
conda activate py312
```

如果环境已存在，使用：

```bash
conda env update -f ../environment-linux.yml
conda activate py312
```

### 16.5 权限边界
- 运维账号或 root 只负责创建系统用户、准备目录、调整目录归属和配置系统级服务。
- `django` 用户负责安装 Miniconda、运行 Django 管理命令和启动最终 Web 服务。
- 最终 Web 服务不要以 root 用户运行。
- `django` 用户不需要 sudo 权限，也不应依赖 sudo 完成日常部署和运行。

---

## 17. MySQL 部署

### 17.1 安装 MySQL 服务端
在全新的 Ubuntu 24.04 上，先安装 MySQL 服务端：

```bash
sudo apt update
sudo apt install mysql-server
sudo service mysql start
```

### 17.2 安装 MySQL 配置文件
项目提供 MySQL 8.0.45 示例配置：

```text
scripts/my.cnf
```

在 Ubuntu 24.04 上部署时，应把它安装为 MySQL 会读取的 `.cnf` 文件，例如：

```bash
sudo cp scripts/my.cnf /etc/mysql/conf.d/gmtool.cnf
sudo chown root:root /etc/mysql/conf.d/gmtool.cnf
sudo chmod 644 /etc/mysql/conf.d/gmtool.cnf
sudo service mysql restart
```

注意：MySQL 的 `!includedir` 通常只加载 `.cnf` 后缀文件，不要把配置文件安装成 `.conf` 后缀。

### 17.3 安装依赖
更新 conda 环境，安装 Django MySQL 驱动：

```bash
conda env update -f ../environment-linux.yml
conda activate py312
```

### 17.4 创建 MySQL 库和账号
建议数据库名、业务账号和 Django `.env` 保持一致。示例使用：

- 数据库：`gmtool`
- 业务账号：`gmtool`
- 连接主机：`127.0.0.1`

先用本机 root 身份进入 MySQL：

```bash
sudo mysql
```

创建数据库和业务账号：

```sql
CREATE DATABASE gmtool CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;
CREATE USER 'gmtool'@'127.0.0.1' IDENTIFIED BY '111111';
GRANT ALL PRIVILEGES ON *.* TO 'gmtool'@'127.0.0.1';
FLUSH PRIVILEGES;
```

验证业务账号可以连接并访问目标库：

```bash
mysql -h127.0.0.1 -ugmtool -p111111
```

如果 WSL 环境使用 `gmtool` 登录失败，可以先在 Windows 侧执行 `wsl --shutdown`，再重新启动 WSL 后重试。

如果 Django 和 MySQL 不在同一台机器，把账号主机从 `127.0.0.1` 改成 Django 服务器的内网 IP 或受控网段，并同步调整 `.env` 中的 `DB_HOST`。

### 17.5 配置生产环境变量
`.env` 示例：

```env
DJANGO_DEBUG=False
DJANGO_SECRET_KEY=替换为生产密钥
DJANGO_ALLOWED_HOSTS=gm.example.com,admin.example.com

DB_ENGINE=mysql
DB_NAME=gmtool
DB_USER=gmtool
DB_PASSWORD=strong_password_here
DB_HOST=127.0.0.1
DB_PORT=3306
DB_CONN_MAX_AGE=600

ENABLE_IDIP_FILE_MONITOR=False
SESSION_COOKIE_SECURE=True
CSRF_COOKIE_SECURE=True
```

MySQL 连接会使用 `utf8mb4`，并在连接初始化时启用 `STRICT_TRANS_TABLES`。

### 17.6 初始化空库
当前流程按空 MySQL 库部署，不迁移现有 SQLite 数据：

```bash
python manage.py migrate
python manage.py sync_commands
python manage.py createsuperuser
python manage.py collectstatic --noinput
python manage.py check --deploy
```

初始化后访问 `/gmtool/`，用新建超级管理员账号登录，并验证命令列表、命令执行、登录日志和命令日志写入正常。

### 17.7 重置或卸载 MySQL
如果只是想清空当前 MySQL 实例，优先使用“重置数据目录”。它会删除所有数据库、账号和权限，但保留已安装的软件包以及 `/etc/mysql/` 下的配置文件。

```bash
sudo service mysql stop
sudo rm -rf /var/lib/mysql
sudo install -d -o mysql -g mysql -m 750 /var/lib/mysql
sudo mysqld --initialize-insecure --user=mysql --datadir=/var/lib/mysql
sudo service mysql start
```

如果需要连 MySQL 软件包、系统配置和数据一起清理，再使用“完整卸载重装”：

```bash
sudo service mysql stop
sudo apt purge mysql-server mysql-client mysql-common mysql-server-core-* mysql-client-core-*
sudo apt autoremove --purge
sudo rm -rf /etc/mysql /var/lib/mysql /var/lib/mysql-files /var/log/mysql
sudo apt install mysql-server
sudo service mysql start
```

说明：

- 两种操作都会删除当前 MySQL 实例中的数据库、账号和权限，执行前必须确认没有需要保留的数据。
- `--initialize-insecure` 会创建可通过本机系统 root 维护的空实例，仅建议用于测试、重置或受控初始化流程。
- 完整卸载重装会删除 `/etc/mysql/`，因此需要重新安装 `scripts/my.cnf` 并重启 MySQL。
- 不建议手动删除 `/etc/apparmor.d/` 下的 MySQL 规则文件；如果软件包仍然需要它们，应由 `apt` 安装、卸载或修复。

重置或重装后，重新执行本节的配置文件安装、建库授权和 Django 初始化步骤。

### 17.8 常见问题
- `ModuleNotFoundError: MySQLdb`：确认已执行 `conda env update -f ../environment-linux.yml`，并在 `py312` 环境中运行项目。
- `Access denied for user`：检查 `DB_USER`、`DB_PASSWORD`、账号授权主机和 MySQL 服务端监听地址。
- 中文或 JSON 内容乱码：确认数据库字符集为 `utf8mb4`。
- `check --deploy` 提示 Cookie 或 Host 配置风险：按生产域名和 HTTPS 代理实际情况修正 `.env`。

---

## 18. 生产部署检查清单

### 18.1 基础配置
- 设置 `DJANGO_DEBUG=False`
- 设置安全的 `DJANGO_SECRET_KEY`
- 设置正确的 `DJANGO_ALLOWED_HOSTS`
- MySQL 部署时设置 `DB_ENGINE=mysql` 及完整数据库连接参数
- 使用独立 `django` Linux 用户运行 Web 服务
- 确认 `django` 用户不具备 sudo 权限
- 确认项目目录、日志目录和静态文件目录对 `django` 用户可写
- 确认生产 Web 服务未以 root 用户运行

### 18.2 HTTPS / 代理
- 确认 HTTPS 已启用
- 确认代理透传 `X-Forwarded-Proto: https`
- 确认 `SESSION_COOKIE_SECURE=True`
- 确认 `CSRF_COOKIE_SECURE=True`
- 确认 Django 无法被绕过代理直接访问
- 确认 `request.is_secure()` 在线上表现正确

### 18.3 Django 自检
```bash
python manage.py check --deploy
```

### 18.4 命令链路验证
- 验证至少一条成功命令
- 验证至少一条失败命令
- 验证命令日志正常落库
- 验证上传命令定义与同步流程正常

---

## 19. 国际化

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

## 20. 前端与静态资源

页面基于 Django Template 与 Tabler 样式构建：

- Tabler Core CSS/JS 当前通过 CDN 引入
- Tabler Icons 字体资源已放在 `gmtool/static/gmtool/vendor/tabler-icons/`
- 项目自定义样式位于 `gmtool/static/gmtool/css/app.css`
- 生产部署时执行 `python manage.py collectstatic --noinput` 收集静态文件

如果生产环境不能访问外网 CDN，需要将 Tabler Core CSS/JS 本地化，并同步修改 `gmtool/templates/gmtool/base.html` 以及错误页模板中的资源引用。

---

## 21. 维护建议

- 新增配置项后，同步更新：
  - `mysite/settings.py`
  - `.env`
  - `README.md`
  - `AGENTS.md`（若会影响后续代理工作方式）

- 调整命令定义结构后，同步更新：
  - `idip_commands.json`
  - `gmtool/command_parser.py`
  - `README.md`
  - `AGENTS.md`（若会影响校验或同步流程）

- 新增路由、模型、管理命令或安全策略后，及时同步更新文档

---

## 22. 备注

- 当前默认数据库为 SQLite，更适合开发与轻量部署
- 生产环境可通过 `DB_ENGINE=mysql` 切换到 MySQL
- 若要进入更高并发或更复杂生产环境，可继续评估 Redis、任务队列等组件
- 当前命令定义中的协议响应 ID 字段统一使用 `response_id`
