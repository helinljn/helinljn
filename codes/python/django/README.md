# 游戏 GM 命令后台管理系统（Django）

---

## 1. 项目概述

本系统是基于 Django 的 GM 命令后台管理平台，核心能力如下：

- 用户登录与权限控制
- GM 命令列表、动态表单执行、执行日志记录
- 用户管理、角色分组管理（角色仅用于分组标记）
- 用户级命令权限分配（`UserCommandPermission`）
- 命令定义文件（`idip_commands.json`）解析与同步
- 命令定义在线上传/同步与自动文件变更监控
- 操作审计日志与应用日志
- 中英文国际化（`zh-hans` / `en`）

---

## 2. 技术栈

| 项目 | 技术 |
|---|---|
| 后端框架 | Django 5.2.12 |
| 语言 | Python 3.11 |
| 数据库 | SQLite（开发环境） |
| 前端 UI | Tabler（CDN，Bootstrap 5） |
| 图标 | Tabler Icons |
| HTTP 客户端 | requests |
| 国际化 | Django i18n（`zh-hans` / `en`） |
| 日志轮转 | 自定义 `SafeTimedRotatingFileHandler` |

---

## 3. 项目结构（当前仓库）

```text
c:\helin\helinljn\codes\python\django\
├── manage.py
├── db.sqlite3
├── idip_commands.json
├── schema.sql
├── environment.yml
├── 开发文档.md
├── mysite/
│   ├── settings.py
│   ├── urls.py
│   ├── wsgi.py
│   └── asgi.py
├── gmtool/
│   ├── apps.py
│   ├── models.py
│   ├── views.py（统一导出模块，向后兼容）
│   ├── auth_views.py（新增：认证相关视图）
│   ├── command_views.py（新增：命令管理视图）
│   ├── user_views.py（新增：用户和角色管理视图）
│   ├── api_views.py（新增：API相关视图）
│   ├── utils.py（新增：公共辅助函数）
│   ├── urls.py
│   ├── forms.py
│   ├── decorators.py
│   ├── idip_client.py
│   ├── command_parser.py
│   ├── middleware.py
│   ├── signals.py
│   ├── audit_log.py
│   ├── logging_handlers.py
│   ├── management/commands/
│   │   ├── init_roles.py
│   │   └── sync_commands.py
│   ├── migrations/
│   │   ├── 0001_initial.py
│   │   ├── 0002_loginlog.py
│   │   ├── 0003_alter_userprofile_role_usercommandpermission.py
│   │   ├── 0004_remove_rolecommandpermission.py
│   │   ├── 0005_add_request_content.py
│   │   └── 0006_alter_usercommandpermission_unique_together_and_more.py
│   └── templates/gmtool/
│       ├── base.html
│       ├── login.html
│       ├── dashboard.html
│       ├── command_list.html
│       ├── command_execute.html
│       ├── command_add.html
│       ├── user_list.html
│       ├── user_form.html
│       ├── user_permissions.html
│       ├── role_list.html
│       ├── role_form.html
│       ├── command_log.html
│       ├── login_log.html
│       ├── 403.html / 404.html / 500.html
├── locale/
│   ├── en/LC_MESSAGES/django.po(.mo)
│   └── zh_Hans/LC_MESSAGES/django.po(.mo)
└── logs/
    ├── audit.log
    └── gmtool.log
```

---

## 4. 核心业务与权限模型

### 4.1 权限原则

1. **命令权限按用户直接分配**（`UserCommandPermission`）。
2. 角色（`Role`）用于用户分组标记，不直接承载命令授权。
3. 超级管理员判断兼容两种来源：
   - Django 内置：`user.is_superuser`
   - GM 角色：`user.userprofile.role.is_super_admin == True`

### 4.2 超级管理员保护规则（代码已实现）

- 不可通过普通用户编辑流程将用户提升为超级管理员
- 超级管理员角色（`super_admin`）不可在角色管理中编辑/删除
- 超级管理员用户不可删除
- 编辑目标为超级管理员时，角色与启用状态不可更改
- 用户不能禁用自己账号

### 4.3 自动绑定机制（signals）

- `post_save(User)`：当用户是 Django 超管时，自动绑定 `super_admin` 角色并授予全部活跃命令权限
- `post_migrate`（仅 gmtool app）：迁移后为已有 Django 超管补齐角色绑定与权限

---

## 5. 数据模型设计（`gmtool/models.py`）

### 5.1 GMCommand

- 命令基础信息：`command_id`, `command_name`, `tab`
- 协议信息：`request_name`, `request_id`, `response_name`, `response_id`
- 参数定义：`request_params`, `response_params`（JSON）
- 状态：`is_active`
- 索引：`command_id`、`(is_active, command_id)`、`request_id`、`response_id`

### 5.2 Role

- 字段：`name`, `display_name`, `description`, `is_super_admin`, `created_at`
- `name` 唯一；`super_admin` 为系统保留角色名

### 5.3 UserCommandPermission

- 字段：`user`, `command`
- 使用数据库约束 `UniqueConstraint(fields=['user', 'command'])` 保证唯一

### 5.4 UserProfile

- 字段：`user`（一对一）、`role`（可空）、`phone`

### 5.5 CommandLog

- 记录命令执行：
  - `request_data`（表单参数）
  - `request_content`（完整请求 JSON 字符串）
  - `response_data`
  - `status`（`success` / `failed` / `timeout`）
  - `ip_address`, `created_at`
- 索引：按用户/状态/命令 + 时间组合索引

### 5.6 LoginLog

- 登录相关动作：`login` / `logout` / `login_failed`
- 字段：`username`, `reason`, `ip_address`, `user_agent`, `created_at`
- 索引：按动作、用户名 + 时间

---

## 6. 命令定义与同步机制（`idip_commands.json` + `command_parser.py`）

### 6.1 解析

`parse_commands()` 从 JSON 提取命令元信息及请求/响应参数列表。

### 6.2 同步

`sync_commands_to_db()`（事务保护）：

- JSON 有、数据库无：创建并启用命令
- JSON 有、数据库有：更新命令
- JSON 无、数据库有：标记为 `is_active=False`
- 对新增命令自动授予所有 Django 超级管理员用户权限

返回值：`(created_count, updated_count, deactivated_count)`。

### 6.3 ID 冲突校验

- `validate_command_ids()`：校验新增单条命令的 command/request/response ID 冲突
- `validate_json_command_ids()`：校验整个 JSON 文件与数据库冲突

### 6.4 在线新增命令

- 页面/路由：`/gmtool/commands/add/`
- 视图：`add_gm_command`
- 表单：`AddGMCommandForm`
- 流程：
  1. 表单校验 + 协议 ID 校验
  2. 原子写入 `idip_commands.json`
  3. 调用同步逻辑入库
  4. 记录审计日志

---

## 7. IDIP API 转发协议（`idip_client.py`）

### 7.1 请求格式

- `POST {IDIP_API_URL}`
- `Content-Type: application/x-www-form-urlencoded`
- 参数：
  - `id`: 命令编号
  - `GSA`: 固定空字符串
  - `content`: URL 编码后的 JSON 字符串（包含 `head` 与 `body`）

> 使用本仓库的 mock 服务时，`IDIP_API_URL` 应配置为：`http://127.0.0.1:18080/cy_idip`

### 7.2 返回约定

`send_idip_command(command, params)` 返回四元组：

```python
(response_data, error_message, request_content_str, error_type)
```

其中：

- `request_content_str` 始终可用于日志落库（完整请求 JSON）
- `error_type` 典型值：`timeout` / `connection` / `error` / `''`

### 7.3 包装响应兼容（mock 新协议）

`idip_client.py` 兼容以下包装格式：

```json
{
  "status": true/false,
  "id": request_id,
  "json": {...}
}
```

处理规则：

1. `status=true`：
   - `json` 为业务 JSON 对象（兼容历史字符串并自动解析）
   - 客户端标准化后将其返回给页面

2. `status=false`：
   - `json` 为对象，包含 `Result` / `RetMsg`
   - 客户端提取错误并按失败流程返回（写日志 + 前端提示）

3. 非包装格式：
   - 视为旧协议，自动包装成新结构后再返回，保证上层处理一致

---

## 8. 路由设计（当前代码）

### 8.1 项目级路由（`mysite/urls.py`）

| 路径 | 说明 |
|---|---|
| `/i18n/` | Django 语言切换相关路由 |
| `/gmtool/` | gmtool 子路由入口 |
| `/` | 重定向至 `/gmtool/` |

错误处理器已绑定：`handler403` / `handler404` / `handler500`。

### 8.2 gmtool 路由（`gmtool/urls.py`）

视图模块化拆分后，路由映射保持不变，但视图函数的导入来源已更新：

| 路径 | 视图 | 权限 | 所属模块 |
|---|---|---|---|
| `/gmtool/` | `dashboard` | 登录 | `command_views` |
| `/gmtool/login/` | `login_view` | 匿名 | `auth_views` |
| `/gmtool/logout/` | `logout_view` | 登录（POST） | `auth_views` |
| `/gmtool/commands/` | `command_list` | 登录 | `command_views` |
| `/gmtool/commands/add/` | `add_gm_command` | 超管 | `command_views` |
| `/gmtool/commands/<cmd_id>/execute/` | `command_execute` | 命令权限 | `command_views` |
| `/gmtool/users/...` | 用户管理相关 | 超管 | `user_views` |
| `/gmtool/roles/...` | 角色管理相关 | 超管 | `user_views` |
| `/gmtool/users/<id>/permissions/` | `user_permissions` | 超管 | `user_views` |
| `/gmtool/logs/` | `command_log_list` | 登录（普通用户仅看自己） | `command_views` |
| `/gmtool/logs/login/` | `login_log_list` | 超管 | `user_views` |
| `/gmtool/api/commands/sync/` | `sync_commands_api` | 超管（POST） | `command_views` |
| `/gmtool/api/commands/upload/` | `upload_commands_api` | 超管（POST） | `api_views` |
| `/gmtool/api/logs/<id>/` | `log_detail_api` | 登录（含权限校验） | `api_views` |

> **向后兼容性**：原有的 `views.py` 作为统一导出模块，仍然提供所有视图函数的导出，确保第三方代码导入不受影响。

---

## 9. 视图模块化设计（2026年4月重构）

为了提高代码可维护性，原有的 `gmtool/views.py`（约1000行）已拆分为四个功能模块，并保持了向后兼容性：

| 模块 | 功能 | 包含视图函数示例 |
|---|---|---|
| `auth_views.py` | 认证相关 | `login_view`, `logout_view`, `custom_404`, `custom_500`, `custom_403`, `csrf_failure` |
| `command_views.py` | 命令管理 | `dashboard`, `command_list`, `command_execute`, `add_gm_command`, `sync_commands_api`, `command_log_list` |
| `user_views.py` | 用户和角色管理 | `user_list`, `user_create`, `user_edit`, `user_delete`, `role_list`, `role_create`, `role_edit`, `user_permissions`, `role_delete`, `login_log_list` |
| `api_views.py` | API接口 | `log_detail_api`, `upload_commands_api` |
| `utils.py` | 公共辅助函数 | `get_client_ip`, `_group_commands_by_tab`, `_validate_command_params_basic`, `_mask_sensitive_data` |

### 9.1 登录与安全（`auth_views.py`）

- 已登录访问登录页：显示“已登录提示页”，非静默跳转
- 登录失败限速：同 IP 连续失败 5 次，锁定 5 分钟（缓存实现），支持 IP 和 IP+用户名双维度限速
- 登出仅允许 POST，避免 CSRF 强制登出

### 9.2 命令执行（`command_views.py`）

- GET：动态渲染请求参数表单，并下发响应 schema（`response_params`）
- POST：JSON 请求体校验（含 `Partition` 类型/范围校验）
- 若命令已停用/删除：
  - POST 返回 `410` + `command_deactivated=true`
  - GET 消息提示后重定向列表
- 记录 `CommandLog` 与审计日志

#### 执行结果展示（动态列表化）

`command_execute.html` 的单次执行结果已改为结构化展示，不再只显示原始 JSON：

1. 顶层响应信息（如 `Result/RetMsg/CommandId/...`）显示为键值列表
2. `Data` 区域按 `idip_commands.json` 同步到数据库的 `response_params` 动态渲染
3. 数组/对象字段自动展开为子表格
4. 若 schema 与实际返回不匹配，自动回退到“原始JSON（兜底）”区域

该机制确保不同 GM 命令可根据各自响应定义动态调整展示列表。

### 9.3 数据库查询优化

- **超级管理员**：直接获取所有活跃命令，避免权限检查开销
- **普通用户**：通过关联表查询 `GMCommand.objects.filter(usercommandpermission__user=request.user)`，避免 `command_id__in` 子查询
- **关联查询**：在关键视图使用 `select_related` 和 `prefetch_related` 减少 N+1 查询
- **分页优化**：对日志列表等进行适当分页控制

### 9.4 批量执行参数（配置注入前端）

- `BATCH_EXECUTE_MAX_TARGETS`
- `BATCH_EXECUTE_INTERVAL_MS`

### 9.5 日志详情脱敏（`api_views.py`）

`log_detail_api` 对敏感字段递归脱敏（例如 password/token/cookie/authorization 等），并对 `request_content` 做结构化脱敏后返回。

### 9.6 命令定义文件上传（`api_views.py`）

`upload_commands_api` 包含：

- 扩展名、大小、JSON 结构、关键字段校验
- ID 冲突校验
- 临时文件 + `os.replace` 原子覆盖
- 成功后自动同步并记录审计日志

---

## 10. 表单规则（`gmtool/forms.py`）

- `UserCreateForm`：
  - 必填密码与确认密码一致
  - 角色下拉过滤 `is_super_admin=False`
- `UserEditForm`：
  - 可修改密码（可选）
  - 编辑自己：禁用 `is_active`
  - 编辑超管目标：禁用角色与 `is_active`
- `RoleForm`：
  - 不包含 `is_super_admin` 字段
  - 禁止 `name=super_admin`
- `AddGMCommandForm`：
  - 支持请求/响应参数 JSON 输入
  - 校验 ID 唯一性与 request/response 冲突

---

## 11. 中间件与装饰器

### 11.1 `IDIPFileMonitorMiddleware`（已增强）

- 检查间隔：30 秒（可通过 `IDIP_FILE_CHECK_INTERVAL` 配置，缓存节流）
- 双模式变更检测：
  - **修改时间检测**（默认）：快速轻量，适合大多数场景
  - **文件哈希检测**（`IDIP_USE_HASH_CHECK=True`）：通过 MD5 哈希精确检测文件内容变更，避免误报
- 线程锁避免并发重复同步
- 可配置是否启用（`ENABLE_IDIP_FILE_MONITOR`）
- 自动调用 `sync_commands_to_db()`

### 11.2 权限装饰器（`decorators.py`）

- `is_super_admin(user)`
- `super_admin_required`
- `command_permission_required`（AJAX 返回 JSON 错误）
- `get_user_permissions(user)`

---

## 12. 配置说明（`mysite/settings.py`）

### 12.1 关键配置

- `DEBUG`：由 `DJANGO_DEBUG` 控制
- `ALLOWED_HOSTS`：由 `DJANGO_ALLOWED_HOSTS` 解析（开发默认回退 `127.0.0.1, localhost`）
- `LOGIN_URL = '/gmtool/login/'`
- `CSRF_FAILURE_VIEW = 'gmtool.views.csrf_failure'`
- `IDIP_API_URL`, `IDIP_TIMEOUT`
- `TRUSTED_PROXY`：控制是否信任 `X-Forwarded-For`
- `BATCH_EXECUTE_MAX_TARGETS`, `BATCH_EXECUTE_INTERVAL_MS`
- `X_FRAME_OPTIONS = 'DENY'`

### 12.2 国际化

- `LANGUAGE_CODE = 'zh-hans'`
- `LANGUAGES = [('zh-hans', '简体中文'), ('en', '英文')]`
- `LOCALE_PATHS = [BASE_DIR / 'locale']`
- 启用 `LocaleMiddleware`

### 12.3 生产安全（`DEBUG=False`）

- 强制安全检查：`SECRET_KEY`、`ALLOWED_HOSTS`
- `SESSION_COOKIE_SECURE`, `CSRF_COOKIE_SECURE`
- `SECURE_SSL_REDIRECT`
- HSTS（`SECURE_HSTS_*`）
- `SECURE_REFERRER_POLICY = 'same-origin'`
- `SECURE_CONTENT_TYPE_NOSNIFF = True`

---

## 13. 日志体系

### 13.1 数据库日志

- `CommandLog`：命令执行日志
- `LoginLog`：登录行为日志

### 13.2 文件日志

- `logs/audit.log`：审计日志（`gmtool.audit`）
- `logs/gmtool.log`：应用日志（`gmtool`）

当前代码中两个日志文件 `backupCount` 均为 `30`（按天轮转）。

### 13.3 安全轮转 Handler

`SafeTimedRotatingFileHandler` 针对 Windows 文件锁导致轮转失败场景做容错：

- `rotate` 失败不阻断后续日志写入
- 强制恢复流句柄
- 始终推进 `rolloverAt`

---

## 14. 管理命令

### 14.1 同步命令定义

```bash
python manage.py sync_commands
```

### 14.2 初始化角色并绑定超管

```bash
python manage.py init_roles
```

行为包括：

1. 同步命令定义
2. 确保 `super_admin` 角色存在
3. 为所有 Django 超管绑定 GM 超管角色
4. 授予超管用户级命令权限

---

## 15. 国际化维护流程

```bash
python manage.py makemessages -l zh_Hans
python manage.py makemessages -l en
python manage.py compilemessages
```

模板中使用 `{% trans %}` / `{% blocktrans %}`，Python 中按场景使用 `gettext_lazy` 或 `gettext`。

### 15.1 本次新增国际化点（命令执行页）

- 结构化结果区域新增文案（如“响应概览”“原始JSON（兜底）”等）已纳入 `django.po`
- 中英文语言包均已补充对应词条
- 修改模板文案后需重新执行 `makemessages + compilemessages`，避免页面出现未翻译文本

---

## 16. 完整部署与初始化指南

### 16.1 环境准备（基于 Conda）

#### 16.1.1 安装 Miniconda（Windows）

如果您尚未安装 Conda，请按以下步骤安装 Miniconda：

1. **下载 Miniconda**（推荐版本）：
   - 下载地址：https://mirrors.tuna.tsinghua.edu.cn/anaconda/miniconda/?C=M&O=D
   - 首选版本：`Miniconda3-py38_23.11.0-2-Windows-x86_64.exe`

2. **安装选项**：
   - 运行安装程序，选择"Install for all users"
   - 勾选所有选项（特别是"Add Miniconda3 to my PATH environment variable"）
   - 按照默认设置完成安装

3. **验证安装**：
   ```bash
   conda --version
   python --version
   ```

4. **初始化Conda**：
   安装完成后，需要执行以下命令初始化Conda，以便在终端中使用`conda activate`命令：
   ```bash
   conda init
   ```
   执行后**关闭并重新打开终端**，然后就可以使用`conda activate`命令激活环境了。

#### 16.1.2 创建项目环境

项目提供了 `environment.yml` 文件用于创建 Conda 环境：

```bash
# 创建并激活环境（环境名：django-admin）
conda env create -f environment.yml
conda activate django-admin

# 如果已存在环境，可以更新：
conda env update -f environment.yml --prune
```

环境文件中已包含所有必需的依赖：
- Django 5.2.12
- Python 3.11
- SQLite 3
- 国际化工具（gettext）
- Python-decouple（环境变量管理）
- requests（HTTP 客户端）

### 16.2 数据库初始化

#### 16.2.1 SQLite（默认，开发环境）

项目默认使用SQLite数据库，无需额外配置：

```bash
# 1. 应用数据库迁移
python manage.py migrate

# 2. 同步命令定义（从 idip_commands.json 同步到数据库）
python manage.py sync_commands

# 3. 初始化角色并绑定超级管理员
python manage.py init_roles
```

#### 16.2.2 MySQL（生产环境）

如果需要切换到MySQL数据库，请按以下步骤操作：

##### 步骤1：安装MySQL客户端和Python驱动

```bash
# 安装MySQL客户端库（Windows）
# 从 https://dev.mysql.com/downloads/installer/ 下载并安装MySQL

# 或者使用conda安装
conda install mysqlclient
```

##### 步骤2：创建MySQL数据库和用户

```sql
-- 登录MySQL
mysql -u root -p

-- 创建数据库
CREATE DATABASE gmtool CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci;

-- 创建用户并授权
CREATE USER 'gmtool_user'@'localhost' IDENTIFIED BY 'your_password_here';
GRANT ALL PRIVILEGES ON gmtool.* TO 'gmtool_user'@'localhost';
FLUSH PRIVILEGES;
```

##### 步骤3：配置Django连接MySQL

修改 `mysite/settings.py` 中的数据库配置：

```python
# 注释或替换原有的SQLite配置
# DATABASES = {
#     'default': {
#         'ENGINE': 'django.db.backends.sqlite3',
#         'NAME': BASE_DIR / 'db.sqlite3',
#     }
# }

# 添加MySQL配置
DATABASES = {
    'default': {
        'ENGINE': 'django.db.backends.mysql',
        'NAME': 'gmtool',                     # 数据库名
        'USER': 'gmtool_user',                # 用户名
        'PASSWORD': 'your_password_here',     # 密码
        'HOST': 'localhost',                  # 数据库主机
        'PORT': '3306',                       # 端口（默认3306）
        'OPTIONS': {
            'charset': 'utf8mb4',             # 字符集
            'init_command': "SET sql_mode='STRICT_TRANS_TABLES'",
        }
    }
}
```

##### 步骤4：执行数据库迁移

```bash
# 应用所有迁移
python manage.py migrate

# 同步命令定义
python manage.py sync_commands

# 初始化角色
python manage.py init_roles
```

##### 步骤5：注意事项

1. **字符集问题**：
   - MySQL需要配置为 `utf8mb4` 字符集以支持完整的Unicode字符（包括emoji）
   - Django默认使用 `utf8mb4`，确保MySQL服务器也使用相同配置

2. **时区设置**：
   - 确保MySQL时区与Django时区一致
   - 可在MySQL中执行：`SET GLOBAL time_zone = '+8:00';`（根据实际时区调整）

3. **性能优化**：
   - 生产环境建议启用连接池
   - 考虑使用 `django-db-connection-pool` 等库

4. **备份与恢复**：
   ```bash
   # 备份MySQL数据库
   mysqldump -u gmtool_user -p gmtool > backup_$(date +%Y%m%d).sql

   # 恢复数据库
   mysql -u gmtool_user -p gmtool < backup_file.sql
   ```

### 16.3 创建超级管理员

系统提供两种超级管理员创建方式：

#### 方式一：使用 Django 内置命令（推荐）
```bash
# 创建 Django 超级管理员（自动绑定 GM 超级管理员角色和权限）
python manage.py createsuperuser

# 按照提示输入用户名、邮箱和密码
```

#### 方式二：通过信号自动绑定
如果已有 Django 超级管理员用户（`is_superuser=True`），系统会自动：
1. 绑定 `super_admin` 角色
2. 授予所有活跃命令的权限

### 16.4 运行开发服务器

```bash
# 启动开发服务器（默认端口 8000）
python manage.py runserver

# 指定端口启动
python manage.py runserver 8080
```

访问地址：`http://localhost:8000/gmtool/`

### 16.5 本地 Mock IDIP 服务（`test/mock_idip_server.py`）

仓库已新增可直接联调用的 Mock 服务脚本：`test/mock_idip_server.py`。

#### 启动方式

```bash
python test/mock_idip_server.py
```

可选参数：

```bash
python test/mock_idip_server.py --host 127.0.0.1 --port 18080
```

#### 联调配置

在 Django 配置中将：

- `IDIP_API_URL = http://127.0.0.1:18080/cy_idip`

与当前 `idip_client.py` 的调用协议保持一致（`application/x-www-form-urlencoded`，字段 `id/GSA/content`）。

#### 提供接口

- `POST /cy_idip`：模拟 IDIP 命令执行
- `GET /health`：健康检查，返回 `{"status":"ok","service":"mock-idip-server"}`

#### 返回协议（统一包装）

- 成功：

```json
{"status": true, "id": request_id, "json": {...}}
```

- 失败：

```json
{"status": false, "id": request_id, "json": {"Result": error_code, "RetMsg": "error_msg"}}
```

#### 数据来源与生成规则

1. 启动时读取项目根目录 `idip_commands.json`
2. 按命令定义自动生成响应字段（含结构体/列表）
3. 支持按 `command_id` 固定覆盖（见脚本内 `CUSTOM_RESPONSES`）

#### 失败注入（联调建议）

可通过以下任一方式主动触发失败返回（`Result=5001`）：

- 表单字段：`__mock_fail=1`
- `content.body` 中传：`"_mock_fail": true`

---

### 16.6 部署前安全检查清单（建议）

在生产环境（`DJANGO_DEBUG=False`）上线前，至少完成以下检查：

1. 必填环境变量
   - `DJANGO_SECRET_KEY`：必须显式配置，禁止使用默认/临时值
   - `DJANGO_ALLOWED_HOSTS`：必须配置为实际域名或主机列表（逗号分隔）

2. 传输与 Cookie 安全
   - 确认已启用 HTTPS
   - 确认 `SESSION_COOKIE_SECURE=True`、`CSRF_COOKIE_SECURE=True`
   - 反向代理场景按需配置 `DJANGO_TRUSTED_PROXY=True`

3. Django 部署自检
   - 执行：
     ```bash
     python manage.py check --deploy
     ```
   - 目标：高危告警清零后再上线

4. 命令链路联调
   - 配置正确的 `IDIP_API_URL`
   - 使用 mock 或测试环境完成至少一条成功调用与一条失败调用验证（含日志落库）

---

## 17. 维护规范（建议）

1. 新增路由、视图、配置项后，优先更新本文第 8/9/12 章。
2. 新增模型字段、索引或约束后，更新第 5 章并注明迁移文件。
3. 调整日志策略（保留天数、格式、处理器）后，更新第 13 章。
4. 避免在文档中写死“固定数量”（命令数/模板数/翻译条目数），改为“以仓库当前代码为准”。

---

## 18. 备注

- 本项目不依赖 Django Admin 进行业务管理，主要后台能力通过 `gmtool` 页面提供。
- 角色是分组标签，命令权限以 `UserCommandPermission` 为准。
- 命令同步与权限分配关键路径已加入事务或差集更新策略，降低数据不一致风险。
