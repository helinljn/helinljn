-- ============================================================
-- 游戏GM命令后台管理系统 - 数据库表结构
-- 数据库: SQLite3
-- 基于 Django models.py 生成
-- ============================================================

-- ============================================================
-- 一、Django 内置表（auth / django_content_type / django_session 等）
-- ============================================================

-- Django 内置用户表
CREATE TABLE auth_user (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    password        VARCHAR(128)       NOT NULL,
    last_login      DATETIME           NULL,
    is_superuser    BOOLEAN            NOT NULL,
    username        VARCHAR(150)       NOT NULL UNIQUE,
    first_name      VARCHAR(150)       NOT NULL,
    last_name       VARCHAR(150)       NOT NULL,
    email           VARCHAR(254)       NOT NULL,
    is_staff        BOOLEAN            NOT NULL,
    is_active       BOOLEAN            NOT NULL,
    date_joined     DATETIME           NOT NULL
);

-- Django 内置权限表
CREATE TABLE auth_permission (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            VARCHAR(255)       NOT NULL,
    content_type_id INTEGER            NOT NULL REFERENCES django_content_type(id),
    codename        VARCHAR(100)       NOT NULL,
    CONSTRAINT auth_permission_content_type_id_codename_uniq UNIQUE (content_type_id, codename)
);

-- Django 内置组表
CREATE TABLE auth_group (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            VARCHAR(150)       NOT NULL UNIQUE
);

-- 组-权限关联
CREATE TABLE auth_group_permissions (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    group_id        INTEGER            NOT NULL REFERENCES auth_group(id),
    permission_id   INTEGER            NOT NULL REFERENCES auth_permission(id),
    CONSTRAINT auth_group_permissions_group_id_permission_id UNIQUE (group_id, permission_id)
);

-- 用户-组关联
CREATE TABLE auth_user_groups (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER            NOT NULL REFERENCES auth_user(id),
    group_id        INTEGER            NOT NULL REFERENCES auth_group(id),
    CONSTRAINT auth_user_groups_user_id_group_id UNIQUE (user_id, group_id)
);

-- 用户-权限关联
CREATE TABLE auth_user_user_permissions (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER            NOT NULL REFERENCES auth_user(id),
    permission_id   INTEGER            NOT NULL REFERENCES auth_permission(id),
    CONSTRAINT auth_user_user_permissions_user_id_permission_id UNIQUE (user_id, permission_id)
);

-- 内容类型表
CREATE TABLE django_content_type (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    app_label       VARCHAR(100)       NOT NULL,
    model           VARCHAR(100)       NOT NULL,
    CONSTRAINT django_content_type_app_label_model UNIQUE (app_label, model)
);

-- Session 表
CREATE TABLE django_session (
    session_key     VARCHAR(40)        NOT NULL PRIMARY KEY,
    session_data    TEXT               NOT NULL,
    expire_date     DATETIME           NOT NULL
);

-- Django 迁移记录表
CREATE TABLE django_migrations (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    app             VARCHAR(255)       NOT NULL,
    name            VARCHAR(255)       NOT NULL,
    applied         DATETIME           NOT NULL
);

-- ============================================================
-- 二、GM 业务表（gmtool app）
-- ============================================================

-- -----------------------------------------------------------
-- 2.1 GMCommand - GM命令定义
-- 从 idip_commands.json 同步，存储所有可用的GM命令
-- -----------------------------------------------------------
CREATE TABLE gmtool_gmcommand (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    command_id      VARCHAR(20)        NOT NULL UNIQUE,   -- 命令编号，如 "10226000"
    command_name    VARCHAR(200)       NOT NULL,           -- 命令名称，如 "查询个人信息"
    tab             VARCHAR(200)       NOT NULL,           -- 分组标签
    request_name    VARCHAR(100)       NOT NULL,           -- 请求名，如 QueryUsrInfoReq
    request_id      INTEGER            NOT NULL,           -- 协议请求ID，如 4097
    response_name   VARCHAR(100)       NOT NULL,           -- 响应名，如 QueryUsrInfoRsp
    response_id     INTEGER            NOT NULL,           -- 协议响应ID，如 4098
    request_params  JSON               NOT NULL,           -- 请求参数定义（从JSON解析）
    response_params JSON               NOT NULL,           -- 响应参数定义（从JSON解析）
    is_active       BOOLEAN            NOT NULL DEFAULT 1  -- 是否启用
);

-- -----------------------------------------------------------
-- 2.2 Role - 角色定义
-- 仅用于用户分类标记（角色分组），不承载权限
-- -----------------------------------------------------------
CREATE TABLE gmtool_role (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            VARCHAR(50)        NOT NULL UNIQUE,    -- 角色标识，如 super_admin
    display_name    VARCHAR(100)       NOT NULL,            -- 显示名称，如 超级管理员
    description     TEXT               NOT NULL DEFAULT '', -- 角色描述
    is_super_admin  BOOLEAN            NOT NULL DEFAULT 0,  -- 超级管理员标记
    created_at      DATETIME           NOT NULL             -- 创建时间
);

-- -----------------------------------------------------------
-- 2.3 UserCommandPermission - 用户-命令权限关联
-- 每个用户的命令权限直接通过此表关联
-- -----------------------------------------------------------
CREATE TABLE gmtool_usercommandpermission (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER            NOT NULL REFERENCES auth_user(id),       -- 关联用户
    command_id      INTEGER            NOT NULL REFERENCES gmtool_gmcommand(id), -- 关联命令
    CONSTRAINT gmtool_usercommandpermission_user_id_command_id UNIQUE (user_id, command_id)
);

-- -----------------------------------------------------------
-- 2.4 UserProfile - 用户扩展信息
-- 关联 Django 内置 User，添加角色分组和联系电话
-- -----------------------------------------------------------
CREATE TABLE gmtool_userprofile (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER            NOT NULL UNIQUE REFERENCES auth_user(id), -- 关联Django用户（1:1）
    role_id         INTEGER            NULL REFERENCES gmtool_role(id),           -- 角色分组（仅分类标记）
    phone           VARCHAR(20)        NOT NULL DEFAULT ''                         -- 联系电话
);

-- -----------------------------------------------------------
-- 2.5 CommandLog - 命令执行日志
-- 记录所有GM命令执行的详细日志
-- -----------------------------------------------------------
CREATE TABLE gmtool_commandlog (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER            NULL REFERENCES auth_user(id),             -- 操作用户
    command_id      INTEGER            NULL REFERENCES gmtool_gmcommand(id),       -- 执行的命令
    partition       INTEGER            NOT NULL,                                    -- 服务器组号
    request_data    JSON               NOT NULL,                                    -- 发送的完整请求体
    response_data   JSON               NULL,                                        -- API返回的响应（可为空）
    status          VARCHAR(20)        NOT NULL,                                    -- 状态：success/failed/timeout
    ip_address      CHAR(39)           NOT NULL,                                    -- 请求IP
    created_at      DATETIME           NOT NULL                                     -- 执行时间
);

-- -----------------------------------------------------------
-- 2.6 LoginLog - 登录日志
-- 记录用户登录/登出/登录失败
-- -----------------------------------------------------------
CREATE TABLE gmtool_loginlog (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id         INTEGER            NULL REFERENCES auth_user(id),              -- 关联用户（登录失败时为NULL）
    username        VARCHAR(150)       NOT NULL,                                     -- 登录时输入的用户名
    action          VARCHAR(20)        NOT NULL,                                     -- 操作类型：login/logout/login_failed
    ip_address      CHAR(39)           NOT NULL,                                     -- 客户端IP地址
    user_agent      VARCHAR(500)       NOT NULL DEFAULT '',                          -- 浏览器标识
    reason          VARCHAR(200)       NOT NULL DEFAULT '',                          -- 失败原因（账号禁用/密码错误）
    created_at      DATETIME           NOT NULL                                      -- 操作时间
);

-- ============================================================
-- 三、索引
-- ============================================================

CREATE INDEX idx_gmcommand_command_id    ON gmtool_gmcommand (command_id);
CREATE INDEX idx_gmcommand_is_active     ON gmtool_gmcommand (is_active);
CREATE INDEX idx_role_is_super_admin     ON gmtool_role (is_super_admin);
CREATE INDEX idx_userprofile_user_id     ON gmtool_userprofile (user_id);
CREATE INDEX idx_userprofile_role_id     ON gmtool_userprofile (role_id);
CREATE INDEX idx_usercmdperm_user_id     ON gmtool_usercommandpermission (user_id);
CREATE INDEX idx_usercmdperm_command_id  ON gmtool_usercommandpermission (command_id);
CREATE INDEX idx_commandlog_user_id      ON gmtool_commandlog (user_id);
CREATE INDEX idx_commandlog_command_id   ON gmtool_commandlog (command_id);
CREATE INDEX idx_commandlog_status       ON gmtool_commandlog (status);
CREATE INDEX idx_commandlog_created_at   ON gmtool_commandlog (created_at);
CREATE INDEX idx_loginlog_user_id        ON gmtool_loginlog (user_id);
CREATE INDEX idx_loginlog_action         ON gmtool_loginlog (action);
CREATE INDEX idx_loginlog_created_at     ON gmtool_loginlog (created_at);
CREATE INDEX idx_session_expire_date     ON django_session (expire_date);

-- ============================================================
-- 四、表关系说明
-- ============================================================
--
-- auth_user (Django内置用户)
--   │
--   ├── 1:1 ── gmtool_userprofile
--   │             │
--   │             └── N:1 ── gmtool_role（角色分组，仅标记分类）
--   │
--   ├── M:N ── gmtool_gmcommand（通过 gmtool_usercommandpermission，核心权限模型）
--   │
--   ├── 1:N ── gmtool_commandlog
--   │             │
--   │             └── N:1 ── gmtool_gmcommand
--   │
--   └── 1:N ── gmtool_loginlog
--
-- gmtool_role
