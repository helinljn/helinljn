-- ============================================================================
-- SQLite数据库表结构导出
-- 数据库文件: db.sqlite3
-- 生成时间: 2026-04-13 18:43:09
-- 总表数: 15
-- ============================================================================

-- ============================================================================
-- 表 1: auth_group
-- ============================================================================

CREATE TABLE "auth_group" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "name"                         varchar(150) NOT NULL UNIQUE
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    name                 varchar(150)    YES    NO     NULL

-- 行数: 0


-- ============================================================================
-- 表 2: auth_group_permissions
-- ============================================================================

CREATE TABLE "auth_group_permissions" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "group_id"                     integer NOT NULL REFERENCES "auth_group" ("id") DEFERRABLE INITIALLY DEFERRED,
  "permission_id"                integer NOT NULL REFERENCES "auth_permission" ("id") DEFERRABLE INITIALLY DEFERRED
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    group_id             INTEGER         YES    NO     NULL
-- 2    permission_id        INTEGER         YES    NO     NULL

-- 索引:
CREATE UNIQUE INDEX "auth_group_permissions_group_id_permission_id_0cd325b0_uniq"
  ON "auth_group_permissions" ("group_id", "permission_id");
CREATE INDEX "auth_group_permissions_group_id_b120cbf9"
  ON "auth_group_permissions" ("group_id");
CREATE INDEX "auth_group_permissions_permission_id_84c5c92e"
  ON "auth_group_permissions" ("permission_id");

-- 行数: 0


-- ============================================================================
-- 表 3: auth_permission
-- ============================================================================

CREATE TABLE "auth_permission" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "content_type_id"              integer NOT NULL REFERENCES "django_content_type" ("id") DEFERRABLE INITIALLY DEFERRED,
  "codename"                     varchar(100) NOT NULL,
  "name"                         varchar(255) NOT NULL
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    content_type_id      INTEGER         YES    NO     NULL
-- 2    codename             varchar(100)    YES    NO     NULL
-- 3    name                 varchar(255)    YES    NO     NULL

-- 索引:
CREATE UNIQUE INDEX "auth_permission_content_type_id_codename_01ab375a_uniq"
  ON "auth_permission" ("content_type_id", "codename");
CREATE INDEX "auth_permission_content_type_id_2f476e4b"
  ON "auth_permission" ("content_type_id");

-- 行数: 44


-- ============================================================================
-- 表 4: auth_user
-- ============================================================================

CREATE TABLE "auth_user" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "password"                     varchar(128) NOT NULL,
  "last_login"                   datetime NULL,
  "is_superuser"                 bool NOT NULL,
  "username"                     varchar(150) NOT NULL UNIQUE,
  "last_name"                    varchar(150) NOT NULL,
  "email"                        varchar(254) NOT NULL,
  "is_staff"                     bool NOT NULL,
  "is_active"                    bool NOT NULL,
  "date_joined"                  datetime NOT NULL,
  "first_name"                   varchar(150) NOT NULL
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    password             varchar(128)    YES    NO     NULL
-- 2    last_login           datetime        NO     NO     NULL
-- 3    is_superuser         bool            YES    NO     NULL
-- 4    username             varchar(150)    YES    NO     NULL
-- 5    last_name            varchar(150)    YES    NO     NULL
-- 6    email                varchar(254)    YES    NO     NULL
-- 7    is_staff             bool            YES    NO     NULL
-- 8    is_active            bool            YES    NO     NULL
-- 9    date_joined          datetime        YES    NO     NULL
-- 10   first_name           varchar(150)    YES    NO     NULL

-- 行数: 2


-- ============================================================================
-- 表 5: auth_user_groups
-- ============================================================================

CREATE TABLE "auth_user_groups" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "user_id"                      integer NOT NULL REFERENCES "auth_user" ("id") DEFERRABLE INITIALLY DEFERRED,
  "group_id"                     integer NOT NULL REFERENCES "auth_group" ("id") DEFERRABLE INITIALLY DEFERRED
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    user_id              INTEGER         YES    NO     NULL
-- 2    group_id             INTEGER         YES    NO     NULL

-- 索引:
CREATE UNIQUE INDEX "auth_user_groups_user_id_group_id_94350c0c_uniq"
  ON "auth_user_groups" ("user_id", "group_id");
CREATE INDEX "auth_user_groups_user_id_6a12ed8b"
  ON "auth_user_groups" ("user_id");
CREATE INDEX "auth_user_groups_group_id_97559544"
  ON "auth_user_groups" ("group_id");

-- 行数: 0


-- ============================================================================
-- 表 6: auth_user_user_permissions
-- ============================================================================

CREATE TABLE "auth_user_user_permissions" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "user_id"                      integer NOT NULL REFERENCES "auth_user" ("id") DEFERRABLE INITIALLY DEFERRED,
  "permission_id"                integer NOT NULL REFERENCES "auth_permission" ("id") DEFERRABLE INITIALLY DEFERRED
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    user_id              INTEGER         YES    NO     NULL
-- 2    permission_id        INTEGER         YES    NO     NULL

-- 索引:
CREATE UNIQUE INDEX "auth_user_user_permissions_user_id_permission_id_14a6b632_uniq"
  ON "auth_user_user_permissions" ("user_id", "permission_id");
CREATE INDEX "auth_user_user_permissions_user_id_a95ead1b"
  ON "auth_user_user_permissions" ("user_id");
CREATE INDEX "auth_user_user_permissions_permission_id_1fbb5f2c"
  ON "auth_user_user_permissions" ("permission_id");

-- 行数: 0


-- ============================================================================
-- 表 7: django_content_type
-- ============================================================================

CREATE TABLE "django_content_type" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "app_label"                    varchar(100) NOT NULL,
  "model"                        varchar(100) NOT NULL
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    app_label            varchar(100)    YES    NO     NULL
-- 2    model                varchar(100)    YES    NO     NULL

-- 索引:
CREATE UNIQUE INDEX "django_content_type_app_label_model_76bd3d3b_uniq"
  ON "django_content_type" ("app_label", "model");

-- 行数: 11


-- ============================================================================
-- 表 8: django_migrations
-- ============================================================================

CREATE TABLE "django_migrations" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "app"                          varchar(255) NOT NULL,
  "name"                         varchar(255) NOT NULL,
  "applied"                      datetime NOT NULL
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    app                  varchar(255)    YES    NO     NULL
-- 2    name                 varchar(255)    YES    NO     NULL
-- 3    applied              datetime        YES    NO     NULL

-- 行数: 21


-- ============================================================================
-- 表 9: django_session
-- ============================================================================

CREATE TABLE "django_session" (
  "session_key"                  varchar(40) NOT NULL PRIMARY KEY,
  "session_data"                 text NOT NULL,
  "expire_date"                  datetime NOT NULL
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    session_key          varchar(40)     YES    YES    NULL
-- 1    session_data         TEXT            YES    NO     NULL
-- 2    expire_date          datetime        YES    NO     NULL

-- 索引:
CREATE INDEX "django_session_expire_date_a5c62663"
  ON "django_session" ("expire_date");

-- 行数: 2


-- ============================================================================
-- 表 10: gmtool_commandlog
-- ============================================================================

CREATE TABLE "gmtool_commandlog" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "partition"                    integer NOT NULL,
  "request_data"                 text NOT NULL CHECK ((JSON_VALID("request_data") OR "request_data" IS NULL)),
  "response_data"                text NULL CHECK ((JSON_VALID("response_data") OR "response_data" IS NULL)),
  "status"                       varchar(20) NOT NULL,
  "ip_address"                   char(39) NOT NULL,
  "created_at"                   datetime NOT NULL,
  "user_id"                      integer NULL REFERENCES "auth_user" ("id") DEFERRABLE INITIALLY DEFERRED,
  "command_id"                   bigint NULL REFERENCES "gmtool_gmcommand" ("id") DEFERRABLE INITIALLY DEFERRED,
  "request_content"              text NOT NULL
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    partition            INTEGER         YES    NO     NULL
-- 2    request_data         TEXT            YES    NO     NULL
-- 3    response_data        TEXT            NO     NO     NULL
-- 4    status               varchar(20)     YES    NO     NULL
-- 5    ip_address           char(39)        YES    NO     NULL
-- 6    created_at           datetime        YES    NO     NULL
-- 7    user_id              INTEGER         NO     NO     NULL
-- 8    command_id           bigint          NO     NO     NULL
-- 9    request_content      TEXT            YES    NO     NULL

-- 索引:
CREATE INDEX "gmtool_commandlog_user_id_ffe0400f"
  ON "gmtool_commandlog" ("user_id");
CREATE INDEX "gmtool_commandlog_command_id_f4f77914"
  ON "gmtool_commandlog" ("command_id");
CREATE INDEX "cmdlog_user_created_idx"
  ON "gmtool_commandlog" ("user_id", "created_at" DESC);
CREATE INDEX "cmdlog_status_created_idx"
  ON "gmtool_commandlog" ("status", "created_at" DESC);
CREATE INDEX "cmdlog_cmd_created_idx"
  ON "gmtool_commandlog" ("command_id", "created_at" DESC);

-- 行数: 42


-- ============================================================================
-- 表 11: gmtool_gmcommand
-- ============================================================================

CREATE TABLE "gmtool_gmcommand" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "command_id"                   varchar(20) NOT NULL UNIQUE,
  "command_name"                 varchar(200) NOT NULL,
  "tab"                          varchar(200) NOT NULL,
  "request_name"                 varchar(100) NOT NULL,
  "request_id"                   integer NOT NULL,
  "response_name"                varchar(100) NOT NULL,
  "response_id"                  integer NOT NULL,
  "request_params"               text NOT NULL CHECK ((JSON_VALID("request_params") OR "request_params" IS NULL)),
  "response_params"              text NOT NULL CHECK ((JSON_VALID("response_params") OR "response_params" IS NULL)),
  "is_active"                    bool NOT NULL
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    command_id           varchar(20)     YES    NO     NULL
-- 2    command_name         varchar(200)    YES    NO     NULL
-- 3    tab                  varchar(200)    YES    NO     NULL
-- 4    request_name         varchar(100)    YES    NO     NULL
-- 5    request_id           INTEGER         YES    NO     NULL
-- 6    response_name        varchar(100)    YES    NO     NULL
-- 7    response_id          INTEGER         YES    NO     NULL
-- 8    request_params       TEXT            YES    NO     NULL
-- 9    response_params      TEXT            YES    NO     NULL
-- 10   is_active            bool            YES    NO     NULL

-- 索引:
CREATE INDEX "gmcmd_cmdid_idx"
  ON "gmtool_gmcommand" ("command_id");
CREATE INDEX "gmcmd_active_cmd_idx"
  ON "gmtool_gmcommand" ("is_active", "command_id");
CREATE INDEX "gmcmd_reqid_idx"
  ON "gmtool_gmcommand" ("request_id");
CREATE INDEX "gmcmd_respid_idx"
  ON "gmtool_gmcommand" ("response_id");

-- 行数: 12


-- ============================================================================
-- 表 12: gmtool_loginlog
-- ============================================================================

CREATE TABLE "gmtool_loginlog" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "username"                     varchar(150) NOT NULL,
  "action"                       varchar(20) NOT NULL,
  "ip_address"                   char(39) NOT NULL,
  "user_agent"                   varchar(500) NOT NULL,
  "reason"                       varchar(200) NOT NULL,
  "created_at"                   datetime NOT NULL,
  "user_id"                      integer NULL REFERENCES "auth_user" ("id") DEFERRABLE INITIALLY DEFERRED
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    username             varchar(150)    YES    NO     NULL
-- 2    action               varchar(20)     YES    NO     NULL
-- 3    ip_address           char(39)        YES    NO     NULL
-- 4    user_agent           varchar(500)    YES    NO     NULL
-- 5    reason               varchar(200)    YES    NO     NULL
-- 6    created_at           datetime        YES    NO     NULL
-- 7    user_id              INTEGER         NO     NO     NULL

-- 索引:
CREATE INDEX "gmtool_loginlog_user_id_d0a342d6"
  ON "gmtool_loginlog" ("user_id");
CREATE INDEX "loginlog_action_created_idx"
  ON "gmtool_loginlog" ("action", "created_at" DESC);
CREATE INDEX "loginlog_user_created_idx"
  ON "gmtool_loginlog" ("username", "created_at" DESC);

-- 行数: 38


-- ============================================================================
-- 表 13: gmtool_role
-- ============================================================================

CREATE TABLE "gmtool_role" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "name"                         varchar(50) NOT NULL UNIQUE,
  "display_name"                 varchar(100) NOT NULL,
  "description"                  text NOT NULL,
  "is_super_admin"               bool NOT NULL,
  "created_at"                   datetime NOT NULL
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    name                 varchar(50)     YES    NO     NULL
-- 2    display_name         varchar(100)    YES    NO     NULL
-- 3    description          TEXT            YES    NO     NULL
-- 4    is_super_admin       bool            YES    NO     NULL
-- 5    created_at           datetime        YES    NO     NULL

-- 行数: 2


-- ============================================================================
-- 表 14: gmtool_usercommandpermission
-- ============================================================================

CREATE TABLE "gmtool_usercommandpermission" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "command_id"                   bigint NOT NULL REFERENCES "gmtool_gmcommand" ("id") DEFERRABLE INITIALLY DEFERRED,
  "user_id"                      integer NOT NULL REFERENCES "auth_user" ("id") DEFERRABLE INITIALLY DEFERRED,
  CONSTRAINT                     "uniq_user_command_permission" UNIQUE ("user_id", "command_id")
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    command_id           bigint          YES    NO     NULL
-- 2    user_id              INTEGER         YES    NO     NULL

-- 索引:
CREATE INDEX "gmtool_usercommandpermission_command_id_91c9204b"
  ON "gmtool_usercommandpermission" ("command_id");
CREATE INDEX "gmtool_usercommandpermission_user_id_15b4cb9f"
  ON "gmtool_usercommandpermission" ("user_id");

-- 行数: 13


-- ============================================================================
-- 表 15: gmtool_userprofile
-- ============================================================================

CREATE TABLE "gmtool_userprofile" (
  "id"                           integer NOT NULL PRIMARY KEY AUTOINCREMENT,
  "phone"                        varchar(20) NOT NULL,
  "role_id"                      bigint NULL REFERENCES "gmtool_role" ("id") DEFERRABLE INITIALLY DEFERRED,
  "user_id"                      integer NOT NULL UNIQUE REFERENCES "auth_user" ("id") DEFERRABLE INITIALLY DEFERRED
);

-- 字段信息:
-- ID   字段名                  类型              非空     主键     默认值
-- ---- -------------------- --------------- ------ ------ --------------------
-- 0    id                   INTEGER         YES    YES    NULL
-- 1    phone                varchar(20)     YES    NO     NULL
-- 2    role_id              bigint          NO     NO     NULL
-- 3    user_id              INTEGER         YES    NO     NULL

-- 索引:
CREATE INDEX "gmtool_userprofile_role_id_1e579274"
  ON "gmtool_userprofile" ("role_id");

-- 行数: 2


-- ============================================================================
-- 数据库统计信息
-- ============================================================================

-- auth_group                    :      0 行
-- auth_group_permissions        :      0 行
-- auth_permission               :     44 行
-- auth_user                     :      2 行
-- auth_user_groups              :      0 行
-- auth_user_user_permissions    :      0 行
-- django_content_type           :     11 行
-- django_migrations             :     21 行
-- django_session                :      2 行
-- gmtool_commandlog             :     42 行
-- gmtool_gmcommand              :     12 行
-- gmtool_loginlog               :     38 行
-- gmtool_role                   :      2 行
-- gmtool_usercommandpermission  :     13 行
-- gmtool_userprofile            :      2 行

-- 总行数: 189
-- 平均每表行数: 12
