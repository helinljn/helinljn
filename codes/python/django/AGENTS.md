# AGENTS.md

本文件为后续编码代理提供本仓库的项目上下文和协作规则，帮助代理安全、准确地修改代码。

## 项目概览

这是一个基于 Django 的游戏 GM 命令后台管理系统。

- Django 项目目录：`mysite/`
- 主业务应用：`gmtool/`
- 命令定义源文件：`idip_commands.json`
- 本地 IDIP Mock 服务：`test/mock_idip_server.py`
- 运行日志目录：`logs/`
- 默认语言和时区：`zh-hans`、`Asia/Shanghai`

业务界面基于 Django Template 实现，模板位于 `gmtool/templates/gmtool/`。本项目的日常 GM 操作不依赖 Django Admin。

前端样式基于 Tabler。Tabler Core CSS/JS 当前通过 CDN 引入，Tabler Icons 字体资源和项目自定义 CSS 位于 `gmtool/static/gmtool/`。

## 环境

使用上一层目录定义的 conda 环境。

```bash
conda env create -f ../environment.yml
conda activate py312
```

Linux 部署目标使用 `../environment-linux.yml`。

配置通过 `python-decouple` 从 `.env` 加载。`.env` 属于本地运行配置，不要提交密钥或机器相关配置。关键配置包括：

- `DJANGO_DEBUG`、`DJANGO_SECRET_KEY`、`DJANGO_ALLOWED_HOSTS`
- `DB_ENGINE`、`DB_NAME`、`DB_SQLITE_TIMEOUT`、`DB_USER`、`DB_PASSWORD`、`DB_HOST`、`DB_PORT`、`DB_CONN_MAX_AGE`
- `IDIP_API_URL`、`IDIP_TIMEOUT`、`IDIP_JSON_PATH`、`UPLOAD_MAX_SIZE`
- `ANNOUNCEMENT_BASE_URL`、`ANNOUNCEMENT_TIMEOUT`、`ANNOUNCEMENT_PLATFORMS`、`ANNOUNCEMENT_CHANNELS`
- `PAGE_SIZE`
- `ENABLE_IDIP_FILE_MONITOR`、`IDIP_FILE_CHECK_INTERVAL`、`IDIP_USE_HASH_CHECK`
- `LOGIN_MAX_ATTEMPTS`、`LOGIN_LOCKOUT_SECONDS`
- `SENSITIVE_FIELDS`
- `DJANGO_TRUSTED_PROXY`、`DJANGO_TRUSTED_PROXY_COUNT`
- `SESSION_COOKIE_SECURE`、`CSRF_COOKIE_SECURE`

默认数据库为 SQLite。可通过环境变量切换到 MySQL。

## 常用命令

初始化或更新本地数据库：

```bash
python manage.py migrate
python manage.py sync_commands
```

运行 Django 测试：

```bash
python manage.py test gmtool
```

运行 Django 系统检查：

```bash
python manage.py check
```

检查命令定义文件格式：

```bash
python manage.py format_idip_commands --check
```

格式化命令定义文件：

```bash
python manage.py format_idip_commands
```

使用项目脚本启动应用：

```bash
bash scripts/django-manager.sh start
```

停止或查看状态：

```bash
bash scripts/django-manager.sh stop
bash scripts/django-manager.sh restart
bash scripts/django-manager.sh status
```

启动脚本默认执行 `python manage.py runserver 0.0.0.0:8000 --noreload`，日志写入 `logs/django-server.log`，PID 写入 `logs/django-server.pid`。启动前应激活 `py312`，或通过 `DJANGO_PYTHON_BIN` 指向 `py312` 环境中的 Python；也可通过 `DJANGO_BIND_HOST`、`DJANGO_BIND_PORT`、`DJANGO_LOG_FILE` 等环境变量覆盖。

默认访问地址：

```text
http://127.0.0.1:8000/gmtool/
```

启动本地 IDIP Mock 服务：

```bash
python test/mock_idip_server.py
```

Mock 服务默认地址：

```text
http://127.0.0.1:5510/cy_idip
```

公告目录服接口配置示例（按实际目录服配置调整）：

```env
ANNOUNCEMENT_BASE_URL=http://example.com
ANNOUNCEMENT_TIMEOUT=5
ANNOUNCEMENT_PLATFORMS=Android,IOS,OpenHarmony
ANNOUNCEMENT_CHANNELS=小米,VIVO,OPPO
```

`ANNOUNCEMENT_BASE_URL` 只保存目录服基础地址，不包含 `/server/announcementSave.php` 等具体路径。

## 重要文件与职责

- `mysite/settings.py`：项目配置、环境变量、数据库选择、日志与安全开关。
- `mysite/urls.py`：项目级 URL 路由。
- `gmtool/apps.py`：应用配置，启动时注册 signals。
- `gmtool/models.py`：GM 命令、用户命令权限、公告权限、用户扩展资料、命令日志、公告日志、公告审核、命令审核、登录日志。
- `gmtool/auth_views.py`：登录、登出、登录失败限速与登录日志。
- `gmtool/announcement_client.py`：公告目录服 HTTP 调用。
- `gmtool/announcement_log_services.py`：公告远端发布/删除日志写入封装。
- `gmtool/announcement_review_services.py`：公告提交审核、审核通过发布、作废、重试等业务逻辑。
- `gmtool/announcement_views.py`：公告查询、提交发布审核、删除、公告日志列表；`/announcements/` 为兼容入口，默认跳转 `/announcements/query/`。
- `gmtool/command_parser.py`：解析 `idip_commands.json`、校验协议 ID、补充 schema 元数据、同步命令到数据库。
- `gmtool/command_review_services.py`：邮件和走马灯 GM 命令审核、审核权限判断、审核通过后执行 IDIP。
- `gmtool/command_services.py`：命令查询与执行前参数校验。
- `gmtool/command_views.py`：仪表盘、命令列表、命令执行、命令新增、命令同步、命令日志。
- `gmtool/review_views.py`：审核管理入口、公告审核、邮件审核、走马灯审核列表与操作。
- `gmtool/user_views.py`：用户管理、用户编辑、删除、权限分配、登录日志列表。
- `gmtool/api_views.py`：命令上传/同步接口、命令日志详情接口、公告日志详情接口。
- `gmtool/forms.py`：用户、命令新增、公告查询/发布/删除等页面表单校验。
- `gmtool/decorators.py`：超级管理员、命令执行权限与公告管理权限装饰器。
- `gmtool/context_processors.py`：模板导航权限上下文。
- `gmtool/permission_service.py`：超级管理员判定等权限辅助逻辑。
- `gmtool/query_utils.py`：分页与时间范围筛选辅助函数。
- `gmtool/audit_log.py`：审计日志封装。
- `gmtool/idip_client.py`：远端 IDIP HTTP 调用。
- `gmtool/security_utils.py`：敏感字段脱敏、客户端 IP 辅助函数。
- `gmtool/middleware.py`：可选的命令定义文件变更监控。
- `gmtool/logging_handlers.py`：兼容 Windows 文件锁场景的按时间轮转日志处理器。
- `gmtool/signals.py`：自动补齐 `UserProfile`。
- `gmtool/views.py`：通用错误页与 CSRF 失败处理。
- `gmtool/utils.py`：模板上下文等轻量辅助函数。
- `gmtool/management/commands/`：自定义维护命令。
- `gmtool/static/gmtool/`：项目 CSS 与本地 Tabler Icons 资源。
- `gmtool/templates/gmtool/`：业务页面模板与错误页模板。
- `gmtool/templatetags/gmtool_tags.py`：模板标签。
- `gmtool/tests.py`：当前 Django 单元测试。
- `locale/`：国际化翻译文件。
- `test/README.md`：IDIP Mock 服务使用说明。

## 开发准则

- 修改范围应聚焦于当前任务，不要重构无关模块。
- 遵循现有 Django 代码风格：函数视图、service/helper 模块、Django `TestCase`、模板位于 `gmtool/templates/gmtool/`。
- 共享业务逻辑应放入 service、parser 或 helper 模块，避免在 view 中重复实现。
- 保持现有命令定义工作流：`idip_commands.json` 是源文件，`sync_commands` 负责同步到数据库。
- 修改命令协议 ID 时要格外谨慎。`request_id` 和 `response_id` 必须唯一，且不能互相重叠。
- 命令 JSON 中 `command_name` 的解析优先级为 `name` -> `command_name` -> `tab`；`request_desc` 和 `response_desc` 只作为 JSON 描述保留，不同步到数据库字段。
- 请求结构体列表必须有对应 `*_count` 字段和正整数 `max_size`；响应结构体列表不要求 `max_size`。
- JSON 解析会拒绝重复键；上传命令定义和在线新增命令失败时会尝试回滚 `idip_commands.json` 快照。
- 上传命令定义必须是 `.json` 文件，大小受 `UPLOAD_MAX_SIZE` 限制，且顶层必须是对象。
- IDIP 请求使用 `application/x-www-form-urlencoded`，字段为 `id`、`GSA`、`content`；`content` 是扁平 JSON 字符串，避免手工双重 URL 编码。
- `send_idip_command(command, params)` 返回 `(response_data, error_message, request_content_str, error_type)`，兼容 `status/id/json` 包装响应和旧版直接 JSON 响应。
- 公告功能独立于 IDIP 命令，不写入 `idip_commands.json`，不参与 `sync_commands`，也不伪装成 `GMCommand`。
- 公告请求使用 `application/x-www-form-urlencoded`。目录服发布/删除为 POST form data，查询为 GET query params；发布公告页面只提交 `AnnouncementReview`，不直接调用目录服。
- 公告管理使用二级页签：查询公告、发布公告、公告日志；没有独立的删除公告页，删除在查询结果中进行：单条删除按钮和「批量删除」都复用 `announcement_batch_delete` 端点（POST `items` 四元组列表），不再跳转独立删除页。
- 查询公告只使用平台单选下拉、渠道多选，不提交公告类型参数；发布公告渠道也为多选，公告类型仍单选。删除复用 `AnnouncementDeleteForm` 校验每个 `items` 四元组（单渠道、类型三选一：周更新公告、常驻公告、轮播图），不在页面上单独选渠道。
- 渠道多选仅作用于查询公告与发布公告表单（`_MultiChannelMixin`，`CheckboxSelectMultiple` 行内复选框组 + 全选开关，共享模板 `_announcement_channel_field.html`）；目录服不支持批量提交，多渠道查询按渠道逐个调用 `query_announcements`，每渠道各产出一个查询结果分组；多渠道发布按渠道逐个创建 `AnnouncementReview`，审核通过/重试时再逐条调用 `create_announcement` 并写 `AnnouncementLog`。`AnnouncementDeleteForm` 必须保持单渠道。
- `ANNOUNCEMENT_PLATFORMS` 和 `ANNOUNCEMENT_CHANNELS` 在 settings 层解析为去重列表；页面提交值必须严格来自配置原值。
- 周更新公告审核通过发布前会自动查询并删除同平台同渠道旧周更新公告；该行为已确认，修改时必须保留页面风险提示和后端顺序保护。
- 仅常驻公告显示并使用显示优先级；轮播图隐藏公告标题和公告正文，后端不要求填写并向目录服传空字符串；周更新公告和常驻公告隐藏图片字段，后端向目录服传空字符串。
- 查询公告详情弹窗按类型展示字段：周更新公告、常驻公告只显示标题和正文；轮播图只显示图片链接；预留字段不展示。
- 发布公告页面提交 `AnnouncementReview` 并写 audit log，不写 `AnnouncementLog`；审核通过/重试实际发布时写 `AnnouncementLog`，日志操作人按提交人记录，audit log 按审核人记录；直接删除公告写 `AnnouncementLog` 和 audit log；查询不写公告操作日志和 audit log。
- `AnnouncementReview` 状态包括 `pending`、`processing`、`approved`、`rejected`、`failed`。审核通过和重试要在短事务内先置为 `processing`，再在事务外调用目录服，最后更新终态。
- 公告审核不能由提交人审核自己的记录；失败记录可重试或作废，待审核记录可通过或作废。
- 邮件审核和走马灯审核使用 `CommandReview`。邮件命令按命令文本中的邮件/mail 识别；走马灯审核复用 `idip_commands.json` 中的公告发布类命令，查询和删除公告命令不需要审核。
- 邮件/走马灯审核权限来自对应命令的 `UserCommandPermission`，超级管理员自动拥有；具有发邮件或发走马灯命令权限的用户可审核同类记录，但不能审核自己提交的记录。
- `CommandReview` 状态包括 `pending`、`processing`、`approved`、`rejected`、`failed`。审核通过和重试要在短事务内先置为 `processing`，再在事务外调用 IDIP，最后更新终态。
- 邮件/走马灯命令审核通过后写 `CommandLog`，日志操作人按提交人记录，audit log 按审核人记录。
- 公告日志详情接口必须继续对配置的敏感字段进行脱敏；非 JSON 的原始响应和错误文本也要用 `mask_sensitive_text` 脱敏。
- 修改 `idip_commands.json` 后，运行 `python manage.py format_idip_commands --check` 或 `python manage.py format_idip_commands`。
- 修改模型后，需要添加 migration，并运行 `python manage.py test gmtool`。
- 修改用户可见文案时，必要时同步更新翻译并运行 `makemessages` 与 `compilemessages`。
- 修改模板或前端静态资源时，注意 `base.html`、400/403/404/500 错误页和 `collectstatic` 部署链路。
- 如果生产环境不能访问外网 CDN，需要将 Tabler Core CSS/JS 本地化，并同步改模板引用。
- 除非任务明确要求，不要移除日志、审计日志、敏感字段脱敏、登录锁定、CSRF 处理或权限检查。
- 不要在日志或响应中输出密钥、完整 `.env`、密码、token 或未经脱敏的敏感请求内容。

## 测试说明

大多数后端变更至少应执行：

```bash
python manage.py test gmtool
python manage.py check
```

生产部署相关变更还应执行：

```bash
python manage.py check --deploy
```

涉及命令定义变更时，还应执行：

```bash
python manage.py format_idip_commands --check
python manage.py sync_commands
```

涉及 IDIP 集成时，在独立终端启动 Mock 服务：

```bash
python test/mock_idip_server.py
```

并将 `IDIP_API_URL` 指向：

```text
http://127.0.0.1:5510/cy_idip
```

涉及公告目录服集成时，优先 mock 调用点：查询/直接删除相关测试 mock `gmtool.announcement_views.query_announcements`、`gmtool.announcement_views.delete_announcement`；审核发布相关测试 mock `gmtool.announcement_review_services.query_announcements`、`gmtool.announcement_review_services.create_announcement`、`gmtool.announcement_review_services.delete_announcement`；客户端底层测试可 mock `gmtool.announcement_client._get_requests`。不要依赖真实目录服跑单元测试。

## 安全与数据处理

本项目涉及 GM 操作和审计数据。权限检查与日志处理都属于安全敏感逻辑。

- 超级管理员拥有全部活跃命令访问权限；普通用户需要显式的 `UserCommandPermission` 授权。
- 命令执行日志应尽量保证可持久化。当前行为允许远端执行成功但本地日志写入失败时返回 warning。
- 公告发布/删除日志也应尽量保证可持久化。远端公告操作成功但本地 `AnnouncementLog` 写入失败时，页面仍按远端成功处理并提示 warning。
- 邮件/走马灯审核通过后的命令执行日志也应尽量保证可持久化。远端 IDIP 执行成功但本地 `CommandLog` 写入失败时，页面仍按远端成功处理并提示 warning。
- 日志详情接口必须继续对配置的敏感字段进行脱敏，包括结构化 JSON、公告原始响应文本和错误文本。
- 公告日志页面和详情接口必须受公告管理权限保护；超级管理员和拥有 `UserAnnouncementPermission` 的普通用户可查看全部公告日志。
- 登出应保持仅允许 POST。
- 需要谨慎校验跳转目标、客户端 IP 提取、上传 JSON 大小与内容、命令参数。
- 生产模式必须显式配置 `DJANGO_SECRET_KEY` 和 `DJANGO_ALLOWED_HOSTS`。
- 项目级路由包含兜底 404；新增路由时避免破坏 `/gmtool/`、`/i18n/`、`/jsi18n/` 和根路径重定向。

## Git 协作

- 工作区可能存在无关本地改动。除非用户明确要求，不要回滚或覆盖这些改动。
- 忽略日志、SQLite 数据库、本地环境文件等运行期生成文件，除非任务明确涉及它们。
- 当项目命令、结构或必要验证步骤发生变化时，同步更新 `AGENTS.md`。
