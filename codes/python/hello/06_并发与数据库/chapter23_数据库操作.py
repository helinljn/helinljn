# =============================================================================
# 第 23 章：数据库操作（sqlite3 模块）
# =============================================================================
#
# 【学习目标】
#   1. 理解关系型数据库的基本概念（表、行、列、主键、外键）
#   2. 掌握 sqlite3 模块的核心 API：连接、游标、执行 SQL
#   3. 熟练使用增删改查（CRUD）操作
#   4. 理解事务（Transaction）的概念和使用
#   5. 掌握参数化查询，防止 SQL 注入
#   6. 学会处理数据库异常和资源管理
#   7. 了解数据库设计的基本原则
#
# 【sqlite3 模块解决什么问题？】
#
#   SQLite 是什么？
#     - 轻量级的嵌入式关系型数据库（无需独立服务器进程）
#     - 数据存储在单个文件中（.db 或 .sqlite 后缀）
#     - 支持标准 SQL 语法（大部分 SQL-92 标准）
#     - 跨平台、零配置、高可靠性，Android/iOS 内置使用它
#
#   适合的场景：
#     - 桌面应用的本地数据存储（日历、笔记、配置）
#     - 小型 Web 应用（日访问量 < 10 万次写入）
#     - 数据分析管道的中间存储、测试环境替代数据库
#     - 嵌入式设备、移动端应用
#
#   不适合的场景：
#     - 高并发写入（每秒 > 1000 次写入，推荐 PostgreSQL/MySQL）
#     - 需要跨网络多客户端同时读写的服务端数据库
#     - 需要复杂权限控制、存储过程等企业级特性
#
#   sqlite3 模块的优势：
#     - Python 标准库内置，无需安装任何第三方包
#     - API 简洁，易于学习，与 DB-API 2.0 规范兼容
#     - 支持上下文管理器（with 语句自动 commit/rollback）
#
# 【与 C/C++ 对比】
#   C/C++：需要链接 libsqlite3，手动管理内存/错误码/资源释放
#   Python sqlite3：高级封装，自动内存管理，异常机制替代错误码
#   类似关系：C++ ODBC/sqlite3 C API ↔ Python sqlite3/SQLAlchemy
#
# 【运行方式】
#   python chapter23_数据库操作.py
#
# =============================================================================

import pickle
import sqlite3
import sys
from contextlib import closing, contextmanager
from datetime import datetime, date
from pathlib import Path
from typing import Any, Generator, Optional


# 全局数据库路径（整章共用同一个演示文件）
DB_PATH = Path("chapter23_demo.db")


# =============================================================================
# 23.1 数据库基础概念
# =============================================================================

def demo_database_concepts() -> None:
    """讲解关系型数据库的核心概念。"""
    print("=" * 60)
    print("23.1 数据库基础概念")
    print("=" * 60)

    print("【关系型数据库核心概念】")
    print()
    print("  数据库（Database）：存储数据的容器，SQLite 中就是一个文件")
    print("  表（Table）：数据的二维结构，类似 Excel 表格")
    print("  行/记录（Row）：一条数据，如一个用户、一笔订单")
    print("  列/字段（Column）：数据的属性，如用户名、年龄、邮箱")
    print()
    print("  主键（Primary Key）：")
    print("    唯一标识每一行，特点：唯一、非空、不可变")
    print("    通常用 INTEGER PRIMARY KEY AUTOINCREMENT（自增整数）")
    print()
    print("  外键（Foreign Key）：")
    print("    引用另一个表的主键，建立表间关系")
    print("    示例：posts.user_id → users.id（每篇文章属于一个用户）")
    print("    SQLite 默认不启用外键约束！需要 PRAGMA foreign_keys = ON")
    print()
    print("  索引（Index）：加速查询的数据结构，类似书的目录")
    print("    代价：占用额外磁盘空间，降低写入速度")
    print()

    print("【SQL 语言分类】")
    print()
    print("  DDL（定义语言）：CREATE / ALTER / DROP TABLE")
    print("  DML（操作语言）：INSERT / UPDATE / DELETE / SELECT")
    print("  DCL（控制语言）：GRANT / REVOKE（SQLite 不支持）")
    print()

    print("【SQLite 的 5 种存储类型】")
    print()
    print("  NULL     → 空值")
    print("  INTEGER  → 整数（自动选择 1/2/4/6/8 字节）")
    print("  REAL     → 64 位 IEEE 754 浮点数")
    print("  TEXT     → UTF-8/UTF-16 文本字符串")
    print("  BLOB     → 二进制数据（图片、序列化对象等）")
    print()
    print("  注意：SQLite 使用「类型亲和性」（Type Affinity），")
    print("  VARCHAR(100) 实际存储为 TEXT，DATETIME 存储为 TEXT，")
    print("  需要应用层自行管理日期时间的格式化和解析。")
    print()


# =============================================================================
# 工具函数：获取配置好的数据库连接
# =============================================================================

def get_connection(db_path: Path = DB_PATH) -> sqlite3.Connection:
    """
    获取配置好的数据库连接（工厂函数）。

    配置说明：
        row_factory = sqlite3.Row  → 查询结果支持列名访问
        PRAGMA foreign_keys = ON   → 启用外键约束（SQLite 默认关闭！）

    Returns:
        sqlite3.Connection: 配置好的连接对象
    """
    conn = sqlite3.connect(db_path)
    # 设置 row_factory 后，fetchone()/fetchall() 返回 sqlite3.Row 对象
    # sqlite3.Row 同时支持 row['column_name'] 和 row[0] 两种访问方式
    conn.row_factory = sqlite3.Row
    # SQLite 默认不检查外键约束，必须每次连接后显式开启
    conn.execute("PRAGMA foreign_keys = ON")
    return conn


@contextmanager
def managed_db(db_path: Path = DB_PATH) -> Generator[sqlite3.Connection, None, None]:
    """
    数据库连接的上下文管理器（推荐在实际项目中使用）。

    用法：
        with managed_db() as conn:
            conn.execute('INSERT ...')
        # 自动 commit（成功）或 rollback（异常），并 close 连接

    Yields:
        sqlite3.Connection: 配置好的连接对象
    """
    conn = get_connection(db_path)
    try:
        yield conn
        conn.commit()
    except Exception:
        conn.rollback()
        raise
    finally:
        conn.close()


# =============================================================================
# 23.2 连接数据库与建表
# =============================================================================

def demo_connection_and_schema() -> None:
    """演示数据库连接方式和建表操作。"""
    print("\n" + "=" * 60)
    print("23.2 连接数据库与建表")
    print("=" * 60)

    # 清理旧数据库（演示专用）
    if DB_PATH.exists():
        DB_PATH.unlink()

    print("【三种连接方式】")
    print()

    # 方式一：手动管理（需 try/finally 确保 close）
    print("  方式一：手动管理")
    conn = sqlite3.connect(DB_PATH)
    try:
        row = conn.execute("SELECT sqlite_version()").fetchone()
        print(f"    SQLite 版本: {row[0]}")
    finally:
        conn.close()
    print()

    # 方式二：with conn（自动 commit/rollback，但不自动 close）
    print("  方式二：with conn（不自动关闭，仍需手动 close）")
    conn = sqlite3.connect(DB_PATH)
    with conn:
        conn.execute("CREATE TABLE IF NOT EXISTS tmp (x INTEGER)")
    conn.close()
    print("    with 块正常退出 → 自动 commit，异常 → 自动 rollback")
    print()

    # 方式三：closing（自动 close，推荐）
    print("  方式三：contextlib.closing（推荐，自动关闭）")
    with closing(sqlite3.connect(DB_PATH)) as conn:
        with conn:
            conn.execute("DROP TABLE IF EXISTS tmp")
    print("    连接已自动关闭")
    print()

    # 内存数据库（测试用）
    print("  特殊路径：")
    print("    ':memory:'  → 内存数据库，程序退出后消失，适合测试")
    print("    ''          → 临时磁盘数据库，关闭后自动删除")
    print()

    print("【创建演示用表结构（多表关联设计）】")
    print()

    # 使用 executescript 一次性执行多条 DDL 语句
    # executescript 会先 commit 当前未提交的事务，然后执行脚本
    with closing(get_connection()) as conn:
        conn.executescript("""
            -- 用户表
            CREATE TABLE IF NOT EXISTS users (
                id         INTEGER PRIMARY KEY AUTOINCREMENT,
                username   TEXT    NOT NULL UNIQUE,
                email      TEXT    UNIQUE,
                age        INTEGER CHECK(age >= 0 AND age <= 150),
                score      REAL    NOT NULL DEFAULT 0.0,
                created_at TEXT    NOT NULL
                               DEFAULT (datetime('now', 'localtime'))
            );

            -- 文章表（多对一：多篇文章属于一个用户）
            CREATE TABLE IF NOT EXISTS posts (
                id         INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id    INTEGER NOT NULL,
                title      TEXT    NOT NULL,
                content    TEXT,
                created_at TEXT    NOT NULL
                               DEFAULT (datetime('now', 'localtime')),
                -- ON DELETE CASCADE：删除用户时，其文章也自动删除
                FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE
            );

            -- 标签表
            CREATE TABLE IF NOT EXISTS tags (
                id   INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL UNIQUE
            );

            -- 文章-标签关联表（多对多关系）
            CREATE TABLE IF NOT EXISTS post_tags (
                post_id INTEGER NOT NULL,
                tag_id  INTEGER NOT NULL,
                PRIMARY KEY (post_id, tag_id),
                FOREIGN KEY (post_id) REFERENCES posts(id) ON DELETE CASCADE,
                FOREIGN KEY (tag_id)  REFERENCES tags(id)  ON DELETE CASCADE
            );
        """)

    print("  已创建 4 张表：users / posts / tags / post_tags")
    print()

    # 查看表结构
    print("【查看表结构：PRAGMA table_info】")
    print()
    with closing(get_connection()) as conn:
        cols = conn.execute("PRAGMA table_info(users)").fetchall()
        print("  users 表结构：")
        print(f"  {'列名':<12} {'类型':<10} {'非空':<5} {'默认值':<30} {'主键'}")
        print(f"  {'-'*12} {'-'*10} {'-'*5} {'-'*30} {'-'*4}")
        for col in cols:
            dflt = col['dflt_value'] or ''
            print(f"  {col['name']:<12} {col['type']:<10} {col['notnull']:<5} "
                  f"{dflt:<30} {col['pk']}")
    print()


# =============================================================================
# 23.3 CRUD 操作：增删改查
# =============================================================================

def demo_crud_operations() -> None:
    """演示完整的增删改查（CRUD）操作。"""
    print("\n" + "=" * 60)
    print("23.3 CRUD 操作：增删改查")
    print("=" * 60)

    print("【⚠️  重要：参数化查询（防止 SQL 注入）】")
    print()
    print("  危险写法（字符串拼接）：")
    print("    name = \"'; DROP TABLE users; --\"")
    print("    sql = f\"SELECT * FROM users WHERE username='{name}'\"")
    print("    # 攻击者可以注入任意 SQL，破坏数据库！")
    print()
    print("  安全写法（参数化查询）：")
    print("    cursor.execute('SELECT * FROM users WHERE username=?', (name,))")
    print("    # sqlite3 自动转义参数，彻底防止注入")
    print()
    print("  两种占位符：")
    print("    ? 位置参数 ：execute('WHERE id=? AND age>?', (1, 18))")
    print("    :key 命名参数：execute('WHERE id=:id', {'id': 1})")
    print()

    # ── C：插入数据 ──────────────────────────────────────
    print("【C（Create）— 插入数据：INSERT】")
    print()

    with closing(get_connection()) as conn:
        # 单条插入
        cur = conn.execute(
            "INSERT INTO users (username, email, age, score) VALUES (?, ?, ?, ?)",
            ("alice", "alice@example.com", 28, 95.5),
        )
        # cursor.lastrowid：本次插入的自增主键值
        alice_id = cur.lastrowid
        print(f"  单条插入 alice，lastrowid = {alice_id}")

        # 批量插入（executemany 比循环 execute 快得多，只编译一次 SQL）
        users_data = [
            ("bob",     "bob@example.com",     32, 87.0),
            ("charlie", "charlie@example.com", 25, 92.3),
            ("diana",   "diana@example.com",   29, 78.6),
            ("eve",     None,                  22, 65.0),  # email 为 NULL
        ]
        conn.executemany(
            "INSERT INTO users (username, email, age, score) VALUES (?, ?, ?, ?)",
            users_data,
        )
        print(f"  批量插入 {len(users_data)} 个用户（executemany）")
        conn.commit()

        # 插入关联数据
        posts_data = [
            (1, "Python 入门指南",  "Python 是一门优雅的语言..."),
            (1, "深入理解装饰器",  "装饰器本质是高阶函数..."),
            (2, "数据结构与算法",  "算法是程序的灵魂..."),
            (3, "Web 开发实践",    "Flask 是轻量级框架..."),
            (3, "异步编程详解",    "asyncio 让并发更简单..."),
        ]
        conn.executemany(
            "INSERT INTO posts (user_id, title, content) VALUES (?, ?, ?)",
            posts_data,
        )
        conn.executemany(
            "INSERT INTO tags (name) VALUES (?)",
            [("Python",), ("算法",), ("Web",), ("异步",), ("入门",)],
        )
        conn.executemany(
            "INSERT INTO post_tags (post_id, tag_id) VALUES (?, ?)",
            [(1,1),(1,5),(2,1),(3,2),(4,3),(5,1),(5,4)],
        )
        conn.commit()
        print("  关联数据（posts/tags/post_tags）插入完成")
    print()

    # ── R：查询数据 ──────────────────────────────────────
    print("【R（Read）— 查询数据：SELECT】")
    print()

    with closing(get_connection()) as conn:
        # fetchall()：获取全部结果（小数据集适用）
        rows = conn.execute(
            "SELECT id, username, age, score FROM users ORDER BY id"
        ).fetchall()
        print("  所有用户（fetchall）：")
        print(f"  {'ID':<4} {'用户名':<10} {'年龄':<6} {'分数'}")
        for row in rows:
            # row_factory=sqlite3.Row 后，支持 row['列名'] 访问
            print(f"  {row['id']:<4} {row['username']:<10} {row['age']:<6} {row['score']}")
        print()

        # fetchone()：获取单条结果（None 表示无结果）
        alice = conn.execute(
            "SELECT * FROM users WHERE username = ?", ("alice",)
        ).fetchone()
        print(f"  fetchone(alice): id={alice['id']}, email={alice['email']}")
        print()

        # 条件/排序/分页
        print("  条件查询：年龄>=28，按分数降序，LIMIT 3")
        rows = conn.execute("""
            SELECT username, age, score FROM users
            WHERE age >= ? ORDER BY score DESC LIMIT 3
        """, (28,)).fetchall()
        for row in rows:
            print(f"    {row['username']}: age={row['age']}, score={row['score']}")
        print()

        # LIKE 模糊查询（% 匹配任意字符串，_ 匹配单个字符）
        print("  LIKE 模糊查询（用户名含 'a'）：")
        rows = conn.execute(
            "SELECT username FROM users WHERE username LIKE ?", ("%a%",)
        ).fetchall()
        print("   ", [r['username'] for r in rows])
        print()

        # 聚合函数：COUNT/AVG/MAX/MIN/SUM
        stats = conn.execute("""
            SELECT COUNT(*) AS cnt, AVG(age) AS avg_age,
                   MAX(score) AS max_s, MIN(score) AS min_s
            FROM users
        """).fetchone()
        print(f"  聚合统计: 共{stats['cnt']}人, "
              f"均龄{stats['avg_age']:.1f}, "
              f"最高分{stats['max_s']}, 最低分{stats['min_s']}")
        print()

        # GROUP BY + HAVING
        print("  GROUP BY 按年龄段分组（HAVING 过滤分组后的结果）：")
        rows = conn.execute("""
            SELECT
                CASE WHEN age < 25 THEN '青年(<25)'
                     WHEN age < 30 THEN '壮年(25-29)'
                     ELSE               '中年(30+)'
                END AS age_group,
                COUNT(*) AS cnt,
                ROUND(AVG(score), 1) AS avg_score
            FROM users
            GROUP BY age_group
            HAVING cnt >= 1
            ORDER BY avg_score DESC
        """).fetchall()
        for row in rows:
            print(f"    {row['age_group']}: {row['cnt']}人, 均分{row['avg_score']}")
        print()

        # INNER JOIN：多表关联查询
        print("  INNER JOIN：查询文章及其作者：")
        rows = conn.execute("""
            SELECT p.id, p.title, u.username AS author
            FROM posts p
            INNER JOIN users u ON p.user_id = u.id
            ORDER BY p.id
        """).fetchall()
        for row in rows:
            print(f"    [{row['id']}] 《{row['title']}》 by {row['author']}")
        print()

        # 三表 JOIN（多对多）+ GROUP_CONCAT
        print("  三表 JOIN：文章及其标签（多对多）：")
        rows = conn.execute("""
            SELECT p.title, GROUP_CONCAT(t.name, ', ') AS tags
            FROM posts p
            LEFT JOIN post_tags pt ON p.id = pt.post_id
            LEFT JOIN tags t       ON pt.tag_id = t.id
            GROUP BY p.id
            ORDER BY p.id
        """).fetchall()
        for row in rows:
            print(f"    《{row['title']}》 → {row['tags'] or '无标签'}")
        print()

        # 逐行迭代游标（大数据集省内存，避免一次性 fetchall）
        print("  迭代游标（逐行处理，内存高效）：")
        cursor = conn.execute("SELECT username, score FROM users ORDER BY score DESC")
        for rank, row in enumerate(cursor, 1):  # cursor 是可迭代对象
            print(f"    第{rank}名: {row['username']} ({row['score']:.1f})")
        print()

    # ── U：更新数据 ──────────────────────────────────────
    print("【U（Update）— 更新数据：UPDATE】")
    print()

    with closing(get_connection()) as conn:
        # 更新单条
        cur = conn.execute(
            "UPDATE users SET score = score + ? WHERE username = ?",
            (5.0, "alice"),
        )
        print(f"  alice 分数+5，rowcount = {cur.rowcount}")

        # 批量条件更新
        cur = conn.execute(
            "UPDATE users SET score = score * 1.1 WHERE age < ?", (25,)
        )
        print(f"  年龄<25 的用户分数x1.1，rowcount = {cur.rowcount}")

        conn.commit()

        # 验证
        rows = conn.execute(
            "SELECT username, age, score FROM users ORDER BY id"
        ).fetchall()
        print("  更新后：")
        for row in rows:
            print(f"    {row['username']}: age={row['age']}, score={row['score']:.1f}")
    print()

    # ── D：删除数据 ──────────────────────────────────────
    print("【D（Delete）— 删除数据：DELETE】")
    print()

    with closing(get_connection()) as conn:
        # 删除单条
        cur = conn.execute("DELETE FROM users WHERE username = ?", ("eve",))
        print(f"  删除 eve，rowcount = {cur.rowcount}")

        # 条件删除
        cur = conn.execute(
            "DELETE FROM users WHERE age > ? AND score < ?", (30, 85.0)
        )
        print(f"  删除 age>30 且 score<85 的用户，rowcount = {cur.rowcount}")

        conn.commit()

        cnt = conn.execute("SELECT COUNT(*) FROM users").fetchone()[0]
        print(f"  删除后剩余用户数: {cnt}")
        print()
        print("  注意区别：")
        print("    DELETE FROM t        → 删除所有行，保留表结构")
        print("    DROP TABLE IF EXISTS t → 删除整张表（含结构）")
    print()


# =============================================================================
# 23.4 事务管理
# =============================================================================

def demo_transactions() -> None:
    """演示事务的概念和 SQLite 中的事务管理。"""
    print("\n" + "=" * 60)
    print("23.4 事务管理（Transaction）")
    print("=" * 60)

    print("【事务的 ACID 属性】")
    print()
    print("  A（Atomicity  原子性）：事务内所有操作作为整体，要么全成功要么全失败")
    print("  C（Consistency 一致性）：事务前后数据库保持一致状态（满足所有约束）")
    print("  I（Isolation   隔离性）：并发事务互不干扰")
    print("  D（Durability  持久性）：提交后数据永久保存，不因宕机丢失")
    print()
    print("  经典例子：银行转账")
    print("    步骤1：从 Alice 账户扣 1000 元")
    print("    步骤2：向 Bob  账户加 1000 元")
    print("    若步骤1成功步骤2失败 → 必须回滚步骤1，保持账户总额不变！")
    print()

    print("【sqlite3 的事务模式】")
    print()
    print("  默认（isolation_level 不为 None）：")
    print("    遇到 DML 语句自动开始事务，需手动 commit() 提交")
    print("    不 commit 则程序结束或连接关闭时数据丢失")
    print()
    print("  with conn 语句：")
    print("    正常退出 → 自动 commit；异常退出 → 自动 rollback")
    print("    但注意：with conn 不会自动 close 连接！")
    print()

    # 建立演示账户表
    with closing(get_connection()) as conn:
        conn.execute("""
            CREATE TABLE IF NOT EXISTS accounts (
                id      INTEGER PRIMARY KEY,
                name    TEXT    NOT NULL,
                balance REAL    NOT NULL CHECK(balance >= 0)
            )
        """)
        with conn:
            conn.execute("INSERT OR REPLACE INTO accounts VALUES (1, 'Alice', 5000.0)")
            conn.execute("INSERT OR REPLACE INTO accounts VALUES (2, 'Bob',   3000.0)")

    def show_balances(label: str) -> None:
        with closing(get_connection()) as c:
            rows = c.execute(
                "SELECT name, balance FROM accounts ORDER BY id"
            ).fetchall()
            balances = ", ".join(f"{r['name']}: {r['balance']:.2f}" for r in rows)
            print(f"  {label}: {balances}")

    show_balances("初始余额")

    # ── 正常转账（commit）────────────────────────────────
    print()
    print("【演示：成功转账（自动 commit）】")
    amount = 1000.0
    with closing(get_connection()) as conn:
        try:
            with conn:  # 正常退出 → commit
                conn.execute(
                    "UPDATE accounts SET balance = balance - ? WHERE name = 'Alice'",
                    (amount,),
                )
                conn.execute(
                    "UPDATE accounts SET balance = balance + ? WHERE name = 'Bob'",
                    (amount,),
                )
                # 验证余额不为负（CHECK 约束也会在此阶段触发）
                bal = conn.execute(
                    "SELECT balance FROM accounts WHERE name='Alice'"
                ).fetchone()['balance']
                if bal < 0:
                    raise ValueError("余额不足！")
            print(f"  转账 {amount:.2f} 成功（已自动 commit）")
        except ValueError as e:
            print(f"  转账失败（已自动 rollback）: {e}")

    show_balances("转账后余额")

    # ── 失败转账（rollback）──────────────────────────────
    print()
    print("【演示：失败转账（自动 rollback）】")
    amount = 99999.0
    with closing(get_connection()) as conn:
        try:
            with conn:  # 异常退出 → rollback
                conn.execute(
                    "UPDATE accounts SET balance = balance - ? WHERE name = 'Alice'",
                    (amount,),
                )
                conn.execute(
                    "UPDATE accounts SET balance = balance + ? WHERE name = 'Bob'",
                    (amount,),
                )
                bal = conn.execute(
                    "SELECT balance FROM accounts WHERE name='Alice'"
                ).fetchone()['balance']
                if bal < 0:
                    raise ValueError(f"余额不足（当前 {bal:.2f}，需要 {amount:.2f}）")
        except (ValueError, sqlite3.IntegrityError) as e:
            print(f"  转账失败（已自动 rollback）: {e}")

    show_balances("回滚后余额")

    # ── SAVEPOINT 保存点 ─────────────────────────────────
    print()
    print("【SAVEPOINT：事务内的局部回滚点】")
    print()
    print("  适合一个大事务中，某个子步骤失败只回滚该步骤的场景")
    print()

    with closing(get_connection()) as conn:
        conn.execute("BEGIN")

        # 操作1：给 Alice +200（先做）
        conn.execute("UPDATE accounts SET balance = balance + 200 WHERE name='Alice'")
        conn.execute("SAVEPOINT sp_after_bonus")  # 创建保存点

        # 操作2：尝试扣款 999999（会失败）
        # SQLite 的 CHECK(balance >= 0) 约束在 UPDATE 时立即检查，
        # 余额不足时直接抛出 IntegrityError，需用 try/except 捕获
        try:
            conn.execute(
                "UPDATE accounts SET balance = balance - 999999 WHERE name='Alice'"
            )
            # 如果没有 CHECK 约束，也可以在这里手动检查：
            # bal = conn.execute(...).fetchone()['balance']
            # if bal < 0: raise ValueError(...)
        except sqlite3.IntegrityError as e:
            # 只回滚到保存点，+200 的操作保留
            conn.execute("ROLLBACK TO SAVEPOINT sp_after_bonus")
            print(f"  操作2失败（{e}），回滚到保存点，操作1(+200)保留")

        conn.commit()  # 提交操作1的 +200

    show_balances("保存点演示后")
    print()


# =============================================================================
# 23.5 高级查询技巧
# =============================================================================

def demo_advanced_queries() -> None:
    """演示子查询、CTE、窗口函数、UPSERT 等高级用法。"""
    print("\n" + "=" * 60)
    print("23.5 高级查询技巧")
    print("=" * 60)

    with closing(get_connection()) as conn:

        # ── 子查询 ───────────────────────────────────────
        print("【子查询（Subquery）】")
        print()

        # 标量子查询：返回单个值，用于 WHERE 条件
        rows = conn.execute("""
            SELECT username, score
            FROM users
            WHERE score > (SELECT AVG(score) FROM users)
            ORDER BY score DESC
        """).fetchall()
        print("  分数高于平均分的用户：")
        for row in rows:
            print(f"    {row['username']}: {row['score']:.1f}")
        print()

        # IN 子查询
        rows = conn.execute("""
            SELECT username FROM users
            WHERE id IN (SELECT DISTINCT user_id FROM posts)
        """).fetchall()
        print(f"  有发文章的用户: {[r['username'] for r in rows]}")
        print()

        # ── CTE（公用表表达式）───────────────────────────
        print("【CTE（WITH 公用表表达式）】")
        print()
        print("  CTE 相当于给子查询起名字，让复杂 SQL 更易读")
        print()

        rows = conn.execute("""
            WITH
            -- 每个用户的文章数
            post_counts AS (
                SELECT user_id, COUNT(*) AS cnt
                FROM posts GROUP BY user_id
            ),
            -- 活跃用户（发文 > 1 篇）
            active AS (
                SELECT u.username, u.score, pc.cnt
                FROM users u
                INNER JOIN post_counts pc ON u.id = pc.user_id
                WHERE pc.cnt > 1
            )
            SELECT username, score, cnt FROM active
            ORDER BY cnt DESC, score DESC
        """).fetchall()

        print("  活跃用户（发文 > 1 篇）：")
        for row in rows:
            print(f"    {row['username']}: 分数={row['score']:.1f}, 文章={row['cnt']}")
        print()

        # ── 窗口函数（SQLite 3.25+）─────────────────────
        print("【窗口函数（Window Functions，SQLite 3.25+）】")
        print()
        print("  窗口函数与 GROUP BY 的核心区别：")
        print("    GROUP BY → 折叠行（多行变一行）")
        print("    OVER()   → 不折叠行（每行保留，同时计算跨行统计）")
        print()

        rows = conn.execute("""
            SELECT
                username,
                score,
                ROW_NUMBER() OVER (ORDER BY score DESC)  AS row_num,
                RANK()       OVER (ORDER BY score DESC)  AS rank,
                ROUND(AVG(score) OVER (), 2)             AS overall_avg,
                ROUND(score - AVG(score) OVER (), 2)     AS diff
            FROM users
            ORDER BY score DESC
        """).fetchall()

        if rows:
            print(f"  {'用户名':<10} {'分数':<8} {'行号':<5} {'排名':<5} "
                  f"{'均值':<8} {'差值'}")
            print(f"  {'-'*10} {'-'*8} {'-'*5} {'-'*5} {'-'*8} {'-'*8}")
            for row in rows:
                print(f"  {row['username']:<10} {row['score']:<8.1f} "
                      f"#{row['row_num']:<4} #{row['rank']:<4} "
                      f"{row['overall_avg']:<8} {row['diff']:+.2f}")
        print()

        # ── 索引 ─────────────────────────────────────────
        print("【索引（INDEX）优化查询速度】")
        print()

        conn.execute("CREATE INDEX IF NOT EXISTS idx_score ON users(score DESC)")
        conn.execute("CREATE INDEX IF NOT EXISTS idx_posts_uid ON posts(user_id)")
        conn.commit()

        print("  已建索引：idx_score（users.score降序），idx_posts_uid（posts.user_id）")
        print()

        # EXPLAIN QUERY PLAN 查看查询是否使用了索引
        plan = conn.execute("""
            EXPLAIN QUERY PLAN
            SELECT username, score FROM users WHERE score > 80 ORDER BY score DESC
        """).fetchall()
        print("  EXPLAIN QUERY PLAN 结果：")
        for row in plan:
            print(f"    {dict(row)}")
        print()

        # ── UPSERT ───────────────────────────────────────
        print("【UPSERT：冲突时更新（ON CONFLICT）】")
        print()

        # INSERT OR IGNORE：唯一键冲突时静默跳过
        conn.execute("""
            INSERT OR IGNORE INTO users (username, email, age, score)
            VALUES (?, ?, ?, ?)
        """, ("alice", "alice2@example.com", 99, 1.0))
        print("  INSERT OR IGNORE：alice 已存在，静默跳过（不报错）")

        # ON CONFLICT DO UPDATE（SQLite 3.24+ UPSERT 语法）
        conn.execute("""
            INSERT INTO users (username, email, age, score)
            VALUES (?, ?, ?, ?)
            ON CONFLICT(username) DO UPDATE SET
                score = excluded.score,
                age   = excluded.age
        """, ("diana", "diana@example.com", 30, 88.8))
        print("  ON CONFLICT DO UPDATE：diana 存在则更新 score 和 age")
        conn.commit()

        diana = conn.execute(
            "SELECT age, score FROM users WHERE username='diana'"
        ).fetchone()
        print(f"  diana 更新后: age={diana['age']}, score={diana['score']}")
    print()


# =============================================================================
# 23.6 Row Factory 与自定义类型
# =============================================================================

def demo_row_factory_and_types() -> None:
    """演示 row_factory 和自定义类型的适配器/转换器。"""
    print("\n" + "=" * 60)
    print("23.6 Row Factory 与自定义类型")
    print("=" * 60)

    print("【row_factory：控制查询结果的格式】")
    print()

    # 默认：返回普通 tuple，只能用下标访问
    print("  1. 默认（tuple）—— 只能下标访问：")
    with closing(sqlite3.connect(DB_PATH)) as conn:
        row = conn.execute("SELECT id, username FROM users LIMIT 1").fetchone()
        print(f"     类型: {type(row)}, row[0]={row[0]}, row[1]={row[1]}")
    print()

    # sqlite3.Row：支持列名访问 + 下标 + 转字典
    print("  2. sqlite3.Row —— 支持列名/下标/转字典：")
    with closing(sqlite3.connect(DB_PATH)) as conn:
        conn.row_factory = sqlite3.Row
        row = conn.execute("SELECT id, username, score FROM users LIMIT 1").fetchone()
        print(f"     类型: {type(row)}")
        print(f"     row['username'] = {row['username']}")
        print(f"     row[0]          = {row[0]}")
        print(f"     dict(row)       = {dict(row)}")
    print()

    # 自定义 row_factory：直接返回 dict
    print("  3. 自定义 row_factory（返回 dict）：")

    def dict_factory(cursor: sqlite3.Cursor, row: tuple) -> dict[str, Any]:
        """将查询行转换为 dict（列名 → 值）。"""
        return {description[0]: row[i] for i, description in enumerate(cursor.description)}

    with closing(sqlite3.connect(DB_PATH)) as conn:
        conn.row_factory = dict_factory
        row = conn.execute("SELECT id, username FROM users LIMIT 1").fetchone()
        print(f"     类型: {type(row)}, 内容: {row}")
    print()

    # ── 自定义类型：Adapter 和 Converter ────────────────
    print("【Adapter / Converter：Python 对象 ↔ SQLite 值】")
    print()
    print("  SQLite 只支持 NULL/INTEGER/REAL/TEXT/BLOB 5种原生类型")
    print("  Adapter  ：Python 对象 → SQLite 存储值（写入时自动转换）")
    print("  Converter：SQLite 值  → Python 对象（读取时自动转换）")
    print()

    # 注册 datetime 的适配器和转换器
    sqlite3.register_adapter(datetime, lambda dt: dt.isoformat())
    sqlite3.register_converter(
        "DATETIME",
        lambda val: datetime.fromisoformat(val.decode()),
    )

    with closing(
        sqlite3.connect(DB_PATH, detect_types=sqlite3.PARSE_DECLTYPES)
    ) as conn:
        conn.row_factory = sqlite3.Row
        conn.execute("""
            CREATE TABLE IF NOT EXISTS events (
                id         INTEGER  PRIMARY KEY,
                name       TEXT,
                event_time DATETIME
            )
        """)
        now = datetime.now()
        conn.execute(
            "INSERT OR REPLACE INTO events VALUES (1, ?, ?)",
            ("Python 大会", now),
        )
        conn.commit()

        row = conn.execute("SELECT name, event_time FROM events WHERE id=1").fetchone()
        print(f"  写入: {now}")
        print(f"  读取: {row['event_time']} （类型: {type(row['event_time']).__name__}）")
        diff = abs((row['event_time'] - now).total_seconds())
        print(f"  时间差: {diff:.6f} 秒（精度损失极小）")
    print()

    # 存储任意 Python 对象（BLOB + pickle）
    print("【存储 Python 对象：BLOB + pickle】")
    print()
    print("  ⚠️  安全警告：仅对可信来源的 pickle 数据调用 loads！")
    print()

    sqlite3.register_adapter(dict, pickle.dumps)
    sqlite3.register_adapter(list, pickle.dumps)
    sqlite3.register_converter("PICKLE", pickle.loads)

    with closing(
        sqlite3.connect(":memory:", detect_types=sqlite3.PARSE_DECLTYPES)
    ) as conn:
        conn.execute("CREATE TABLE kv (key TEXT PRIMARY KEY, value PICKLE)")
        data = {"scores": [95, 87, 92], "tags": ["python", "db"]}
        conn.execute("INSERT INTO kv VALUES (?, ?)", ("cfg", data))
        conn.commit()

        row = conn.execute("SELECT value FROM kv WHERE key='cfg'").fetchone()
        retrieved = row[0]
        print(f"  存储: {data}")
        print(f"  取回: {retrieved}")
        print(f"  一致: {data == retrieved}")
    print()


# =============================================================================
# 23.7 异常处理
# =============================================================================

def demo_exception_handling() -> None:
    """演示 sqlite3 的异常层次和常见错误的捕获处理。"""
    print("\n" + "=" * 60)
    print("23.7 异常处理")
    print("=" * 60)

    print("【sqlite3 异常层次结构】")
    print()
    print("  Exception")
    print("  └── sqlite3.Error                  （所有 sqlite3 错误基类）")
    print("      ├── sqlite3.DatabaseError")
    print("      │   ├── sqlite3.IntegrityError （违反约束：唯一/非空/外键/CHECK）")
    print("      │   ├── sqlite3.OperationalError（SQL语法错误、表不存在等）")
    print("      │   ├── sqlite3.ProgrammingError（参数数量错误等编程错误）")
    print("      │   └── sqlite3.DataError      （数据格式/范围错误）")
    print("      └── sqlite3.Warning")
    print()

    print("【常见异常演示】")
    print()

    with closing(get_connection()) as conn:
        # 1. 违反 UNIQUE 约束
        print("  1. IntegrityError（违反 UNIQUE 约束）：")
        try:
            conn.execute(
                "INSERT INTO users (username, email, age, score) VALUES (?,?,?,?)",
                ("alice", "x@x.com", 20, 0.0),
            )
        except sqlite3.IntegrityError as e:
            print(f"     {type(e).__name__}: {e}")
        finally:
            conn.rollback()

        # 2. 违反 NOT NULL 约束
        print("  2. IntegrityError（违反 NOT NULL 约束）：")
        try:
            conn.execute(
                "INSERT INTO users (username, email, age, score) VALUES (?,?,?,?)",
                (None, "y@y.com", 20, 0.0),
            )
        except sqlite3.IntegrityError as e:
            print(f"     {type(e).__name__}: {e}")
        finally:
            conn.rollback()

        # 3. 违反外键约束
        print("  3. IntegrityError（违反外键约束）：")
        try:
            conn.execute(
                "INSERT INTO posts (user_id, title) VALUES (?, ?)",
                (99999, "不存在用户的文章"),
            )
        except sqlite3.IntegrityError as e:
            print(f"     {type(e).__name__}: {e}")
        finally:
            conn.rollback()

        # 4. SQL 语法错误
        print("  4. OperationalError（SQL 语法错误）：")
        try:
            conn.execute("SELEKT * FORM users")
        except sqlite3.OperationalError as e:
            print(f"     {type(e).__name__}: {e}")

        # 5. 表不存在
        print("  5. OperationalError（表不存在）：")
        try:
            conn.execute("SELECT * FROM no_such_table")
        except sqlite3.OperationalError as e:
            print(f"     {type(e).__name__}: {e}")

        # 6. 参数数量错误
        print("  6. ProgrammingError（参数数量错误）：")
        try:
            conn.execute("SELECT * FROM users WHERE id = ?", ())
        except (sqlite3.ProgrammingError, sqlite3.OperationalError) as e:
            print(f"     {type(e).__name__}: {e}")
    print()

    print("【正确的资源管理模式（推荐）】")
    print()
    print("  # 最推荐：managed_db 上下文管理器（见本章 get_connection 区域）")
    print("  with managed_db() as conn:")
    print("      conn.execute('INSERT ...')")
    print("  # 自动 commit（成功）/ rollback（异常）/ close（退出）")
    print()
    print("  # 也可使用 with closing + with conn：")
    print("  with closing(get_connection()) as conn:")
    print("      with conn:")
    print("          conn.execute('INSERT ...')")
    print("  # with conn 自动 commit/rollback，closing 自动 close")
    print()


# =============================================================================
# 23.8 实战项目：个人任务管理器
# =============================================================================

def demo_task_manager() -> None:
    """实战项目：用 sqlite3 构建完整的个人任务管理器（CLI 风格）。"""
    print("\n" + "=" * 60)
    print("23.8 实战项目：个人任务管理器")
    print("=" * 60)

    print("【项目设计】")
    print()
    print("  功能：添加/更新/删除任务，按条件查询，统计报告")
    print("  数据库：内存数据库（:memory:），演示完整 CRUD 流程")
    print()

    class TaskManager:
        """
        基于 SQLite 的任务管理器。

        表结构：
            tasks(id, title, description, priority, status,
                  due_date, created_at, completed_at)

        优先级：1=低，2=中，3=高
        状态：  pending / in_progress / done
        """

        PRIORITY = {1: "低", 2: "中", 3: "高"}
        STATUS_LABEL = {
            "pending":     "待处理",
            "in_progress": "进行中",
            "done":        "已完成",
        }

        def __init__(self, db_path: str = ":memory:") -> None:
            self._db = db_path
            # 内存数据库需要保持连接，否则关闭后数据消失
            self._conn_obj = sqlite3.connect(self._db)
            self._conn_obj.row_factory = sqlite3.Row
            self._conn_obj.execute("PRAGMA foreign_keys = ON")
            self._setup()

        def _conn(self) -> sqlite3.Connection:
            """返回持久连接（内存数据库）或新连接（文件数据库）。"""
            if self._db == ":memory:":
                return self._conn_obj
            else:
                conn = sqlite3.connect(self._db)
                conn.row_factory = sqlite3.Row
                conn.execute("PRAGMA foreign_keys = ON")
                return conn

        def _setup(self) -> None:
            """建表。"""
            conn = self._conn()
            conn.execute("""
                CREATE TABLE IF NOT EXISTS tasks (
                    id           INTEGER PRIMARY KEY AUTOINCREMENT,
                    title        TEXT    NOT NULL,
                    description  TEXT    DEFAULT '',
                    priority     INTEGER NOT NULL DEFAULT 2
                                     CHECK(priority BETWEEN 1 AND 3),
                    status       TEXT    NOT NULL DEFAULT 'pending'
                                     CHECK(status IN
                                       ('pending','in_progress','done')),
                    due_date     TEXT,
                    created_at   TEXT NOT NULL
                                     DEFAULT (datetime('now','localtime')),
                    completed_at TEXT
                )
            """)
            # 常用查询字段建索引
            conn.execute("""
                CREATE INDEX IF NOT EXISTS idx_tasks_status
                ON tasks(status, priority DESC)
            """)
            conn.commit()

        def add(
            self,
            title: str,
            description: str = "",
            priority: int = 2,
            due_date: Optional[str] = None,
        ) -> int:
            """
            添加新任务。

            Args:
                title:       任务标题（必填）
                description: 任务描述
                priority:    优先级 1/2/3
                due_date:    截止日期（'YYYY-MM-DD' 格式）

            Returns:
                新任务的 ID
            """
            if priority not in (1, 2, 3):
                raise ValueError(f"优先级必须是 1/2/3，收到: {priority}")

            conn = self._conn()
            cur = conn.execute(
                """
                INSERT INTO tasks (title, description, priority, due_date)
                VALUES (?, ?, ?, ?)
                """,
                (title, description, priority, due_date),
            )
            conn.commit()
            return cur.lastrowid

        def update_status(self, task_id: int, status: str) -> bool:
            """
            更新任务状态。

            Args:
                task_id: 任务 ID
                status:  新状态（'pending'/'in_progress'/'done'）

            Returns:
                True 表示更新成功，False 表示任务不存在
            """
            valid = ("pending", "in_progress", "done")
            if status not in valid:
                raise ValueError(f"状态必须是 {valid}，收到: {status!r}")

            completed_at = (
                datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                if status == "done" else None
            )

            conn = self._conn()
            cur = conn.execute(
                """
                UPDATE tasks
                SET status = ?, completed_at = ?
                WHERE id = ?
                """,
                (status, completed_at, task_id),
            )
            conn.commit()
            return cur.rowcount > 0

        def delete(self, task_id: int) -> bool:
            """删除任务。返回 True 表示成功，False 表示任务不存在。"""
            conn = self._conn()
            cur = conn.execute("DELETE FROM tasks WHERE id = ?", (task_id,))
            conn.commit()
            return cur.rowcount > 0

        def list_tasks(
            self,
            status: Optional[str] = None,
            priority: Optional[int] = None,
            order_by: str = "priority DESC, created_at",
        ) -> list[dict[str, Any]]:
            """
            查询任务列表（支持按状态/优先级过滤）。

            Args:
                status:   过滤状态（None 表示不过滤）
                priority: 过滤优先级（None 表示不过滤）
                order_by: 排序字段（SQL 片段）

            Returns:
                任务字典列表
            """
            conditions = []
            params: list[Any] = []

            if status is not None:
                conditions.append("status = ?")
                params.append(status)
            if priority is not None:
                conditions.append("priority = ?")
                params.append(priority)

            where = f"WHERE {' AND '.join(conditions)}" if conditions else ""

            conn = self._conn()
            rows = conn.execute(
                f"""
                SELECT id, title, priority, status, due_date,
                       created_at, completed_at
                FROM tasks
                {where}
                ORDER BY {order_by}
                """,
                params,
            ).fetchall()
            return [dict(row) for row in rows]

        def search(self, keyword: str) -> list[dict[str, Any]]:
            """
            全文搜索（标题和描述中包含关键词）。

            Args:
                keyword: 搜索关键词

            Returns:
                匹配的任务字典列表
            """
            pattern = f"%{keyword}%"
            conn = self._conn()
            rows = conn.execute(
                """
                SELECT id, title, priority, status, due_date
                FROM tasks
                WHERE title LIKE ? OR description LIKE ?
                ORDER BY priority DESC
                """,
                (pattern, pattern),
            ).fetchall()
            return [dict(row) for row in rows]

        def stats(self) -> dict[str, Any]:
            """
            生成任务统计报告。

            Returns:
                包含各类统计数据的字典
            """
            conn = self._conn()
            # 总体统计
            total_row = conn.execute(
                "SELECT COUNT(*) AS total FROM tasks"
            ).fetchone()

            # 按状态统计
            status_rows = conn.execute(
                """
                SELECT status, COUNT(*) AS cnt
                FROM tasks GROUP BY status
                """
            ).fetchall()

            # 按优先级统计
            priority_rows = conn.execute(
                """
                SELECT priority, COUNT(*) AS cnt
                FROM tasks GROUP BY priority ORDER BY priority DESC
                """
            ).fetchall()

            # 已逾期的未完成任务
            overdue = conn.execute(
                """
                SELECT COUNT(*) AS cnt FROM tasks
                WHERE status != 'done'
                  AND due_date IS NOT NULL
                  AND due_date < date('now')
                """
            ).fetchone()['cnt']

            return {
                "total":    total_row['total'],
                "by_status": {r['status']: r['cnt'] for r in status_rows},
                "by_priority": {r['priority']: r['cnt'] for r in priority_rows},
                "overdue":  overdue,
            }

        def print_tasks(self, tasks: list[dict[str, Any]], title: str = "") -> None:
            """格式化打印任务列表。"""
            if title:
                print(f"\n  ── {title} ──")
            if not tasks:
                print("  （无任务）")
                return

            for t in tasks:
                pri_label = self.PRIORITY.get(t['priority'], '?')
                sta_label = self.STATUS_LABEL.get(t['status'], t['status'])
                due = f"，截止:{t['due_date']}" if t.get('due_date') else ""
                print(f"  [{t['id']:2d}] {'⭐'*t['priority']} {t['title']}")
                print(f"        状态:{sta_label}  优先级:{pri_label}{due}")

    # ── 演示任务管理器的完整使用流程 ─────────────────────
    print("【演示：TaskManager 完整使用流程】")
    print()

    tm = TaskManager(":memory:")  # 使用内存数据库

    # 添加任务
    print("  添加任务：")
    ids = []
    ids.append(tm.add("学习 Python 基础语法",
                       "完成章节1-5的学习和练习", priority=3,
                       due_date="2025-12-31"))
    ids.append(tm.add("搭建开发环境",
                       "安装 Python 3.11、VSCode、配置虚拟环境", priority=3))
    ids.append(tm.add("阅读 Python 官方文档",
                       "重点看标准库部分", priority=2,
                       due_date="2025-06-30"))
    ids.append(tm.add("完成综合实战项目",
                       "任务管理器 CLI 工具", priority=2))
    ids.append(tm.add("整理学习笔记",
                       "按章节整理知识点", priority=1))

    print(f"  已添加 {len(ids)} 个任务，ID: {ids}")

    # 查看所有任务
    all_tasks = tm.list_tasks()
    tm.print_tasks(all_tasks, "所有任务")

    # 更新状态
    print()
    print("  更新任务状态：")
    tm.update_status(ids[0], "done")         # 任务1完成
    tm.update_status(ids[1], "in_progress")  # 任务2进行中
    print(f"  任务 {ids[0]} → done")
    print(f"  任务 {ids[1]} → in_progress")

    # 按状态筛选
    pending = tm.list_tasks(status="pending")
    tm.print_tasks(pending, "待处理任务")

    done = tm.list_tasks(status="done")
    tm.print_tasks(done, "已完成任务")

    # 按优先级筛选
    high_pri = tm.list_tasks(priority=3)
    tm.print_tasks(high_pri, "高优先级任务")

    # 搜索
    print()
    results = tm.search("Python")
    tm.print_tasks(results, "搜索 'Python' 的结果")

    # 统计报告
    print()
    print("  ── 统计报告 ──")
    s = tm.stats()
    print(f"  总任务数: {s['total']}")
    print(f"  按状态: ", end="")
    for status, cnt in s['by_status'].items():
        label = TaskManager.STATUS_LABEL.get(status, status)
        print(f"{label}:{cnt}", end="  ")
    print()
    print(f"  按优先级: ", end="")
    for pri, cnt in s['by_priority'].items():
        label = TaskManager.PRIORITY.get(pri, str(pri))
        print(f"{label}:{cnt}", end="  ")
    print()
    print(f"  已逾期未完成: {s['overdue']}")

    # 删除任务
    print()
    deleted = tm.delete(ids[-1])
    print(f"  删除任务 {ids[-1]}: {'成功' if deleted else '失败（不存在）'}")
    print(f"  删除后剩余: {tm.stats()['total']} 个任务")
    print()


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示函数。"""
    demo_database_concepts()
    demo_connection_and_schema()
    demo_crud_operations()
    demo_transactions()
    demo_advanced_queries()
    demo_row_factory_and_types()
    demo_exception_handling()
    demo_task_manager()

    # 清理演示数据库文件
    if DB_PATH.exists():
        DB_PATH.unlink()
        print(f"\n  演示数据库 {DB_PATH} 已清理")


if __name__ == "__main__":
    main()


# =============================================================================
# 语法速查表（注释形式）
# =============================================================================

"""
【sqlite3 核心 API 速查】

1. 连接数据库
   conn = sqlite3.connect('db.db')        # 文件数据库
   conn = sqlite3.connect(':memory:')     # 内存数据库
   conn.row_factory = sqlite3.Row         # 启用列名访问
   conn.execute("PRAGMA foreign_keys=ON") # 启用外键约束

2. 执行 SQL
   cursor = conn.cursor()
   cursor.execute(sql, params)            # 单条 SQL
   cursor.executemany(sql, seq_of_params) # 批量执行
   cursor.executescript(sql_script)       # 执行多条 SQL 脚本

3. 获取结果
   row = cursor.fetchone()                # 单行（None 表示无结果）
   rows = cursor.fetchall()               # 所有行
   for row in cursor: ...                 # 逐行迭代（省内存）

4. 事务管理
   conn.commit()                          # 提交事务
   conn.rollback()                        # 回滚事务
   with conn: ...                         # 自动 commit/rollback

5. 资源管理
   conn.close()                           # 关闭连接
   with closing(conn): ...                # 自动关闭

6. 参数化查询（防 SQL 注入）
   cursor.execute('SELECT * FROM t WHERE id=?', (1,))
   cursor.execute('SELECT * FROM t WHERE id=:id', {'id': 1})

7. 常用属性
   cursor.lastrowid                       # 最后插入的主键 ID
   cursor.rowcount                        # 受影响的行数
   cursor.description                     # 列信息（元组列表）

【常用 SQL 语句】

-- DDL（定义）
CREATE TABLE t (id INTEGER PRIMARY KEY, name TEXT NOT NULL);
ALTER TABLE t ADD COLUMN age INTEGER;
DROP TABLE IF EXISTS t;

-- DML（操作）
INSERT INTO t (name) VALUES ('Alice');
UPDATE t SET name='Bob' WHERE id=1;
DELETE FROM t WHERE id=1;
SELECT * FROM t WHERE age>18 ORDER BY age DESC LIMIT 10;

-- 约束
PRIMARY KEY                    -- 主键
UNIQUE                         -- 唯一
NOT NULL                       -- 非空
CHECK(age >= 0)                -- 检查约束
FOREIGN KEY (uid) REFERENCES users(id) ON DELETE CASCADE

-- 索引
CREATE INDEX idx_name ON t(name);
DROP INDEX IF EXISTS idx_name;

-- 事务
BEGIN;
SAVEPOINT sp1;
ROLLBACK TO SAVEPOINT sp1;
COMMIT;

-- 聚合函数
COUNT(*), AVG(col), MAX(col), MIN(col), SUM(col)

-- 分组
SELECT age, COUNT(*) FROM t GROUP BY age HAVING COUNT(*) > 1;

-- 连接
SELECT * FROM t1 INNER JOIN t2 ON t1.id = t2.t1_id;
SELECT * FROM t1 LEFT JOIN t2 ON t1.id = t2.t1_id;

-- 子查询
SELECT * FROM t WHERE age > (SELECT AVG(age) FROM t);

-- CTE（公用表表达式）
WITH cte AS (SELECT * FROM t WHERE age > 18)
SELECT * FROM cte;

-- 窗口函数（SQLite 3.25+）
SELECT name, ROW_NUMBER() OVER (ORDER BY age) AS rank FROM t;
"""


# =============================================================================
# 常见错误与解决方案（注释形式）
# =============================================================================

"""
【常见错误 1：忘记 commit，数据未持久化】

错误代码：
    conn = sqlite3.connect('db.db')
    conn.execute("INSERT INTO t VALUES (1, 'Alice')")
    conn.close()  # 数据丢失！

正确做法：
    conn = sqlite3.connect('db.db')
    conn.execute("INSERT INTO t VALUES (1, 'Alice')")
    conn.commit()  # 必须提交
    conn.close()

或使用 with conn：
    with closing(sqlite3.connect('db.db')) as conn:
        with conn:  # 自动 commit
            conn.execute("INSERT INTO t VALUES (1, 'Alice')")


【常见错误 2：SQL 注入漏洞】

危险代码：
    name = input("输入用户名: ")
    sql = f"SELECT * FROM users WHERE name='{name}'"
    conn.execute(sql)  # 用户输入 "'; DROP TABLE users; --" 会破坏数据库！

正确做法：
    name = input("输入用户名: ")
    conn.execute("SELECT * FROM users WHERE name=?", (name,))


【常见错误 3：外键约束未启用】

问题：
    SQLite 默认不检查外键约束，删除父表记录时子表不会级联删除

解决：
    conn = sqlite3.connect('db.db')
    conn.execute("PRAGMA foreign_keys = ON")  # 每次连接后都要执行


【常见错误 4：忘记关闭连接】

问题：
    conn = sqlite3.connect('db.db')
    conn.execute("INSERT ...")
    conn.commit()
    # 忘记 conn.close()，文件可能被锁定

解决：
    with closing(sqlite3.connect('db.db')) as conn:
        with conn:
            conn.execute("INSERT ...")
    # 自动关闭


【常见错误 5：并发写入冲突】

问题：
    SQLite 同一时刻只允许一个写事务，多进程/线程同时写入会报错：
    sqlite3.OperationalError: database is locked

解决方案：
    1. 使用 WAL 模式：conn.execute("PRAGMA journal_mode=WAL")
    2. 设置超时：sqlite3.connect('db.db', timeout=10.0)
    3. 减少事务持续时间，尽快 commit
    4. 考虑使用队列串行化写入
    5. 高并发场景改用 PostgreSQL/MySQL


【常见错误 6：类型转换问题】

问题：
    SQLite 存储 datetime 为 TEXT，读取时仍是字符串

解决：
    # 注册适配器和转换器
    sqlite3.register_adapter(datetime, lambda dt: dt.isoformat())
    sqlite3.register_converter("DATETIME", lambda s: datetime.fromisoformat(s.decode()))

    # 连接时启用类型检测
    conn = sqlite3.connect('db.db', detect_types=sqlite3.PARSE_DECLTYPES)


【常见错误 7：executemany 参数格式错误】

错误：
    data = [1, 2, 3]
    conn.executemany("INSERT INTO t VALUES (?)", data)  # 报错！

正确：
    data = [(1,), (2,), (3,)]  # 每个元素必须是元组
    conn.executemany("INSERT INTO t VALUES (?)", data)
"""


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：学生成绩管理系统
#   要求：
#   1. 创建 students 表（id, name, age）和 scores 表（id, student_id, subject, score）
#   2. 插入至少 5 个学生和 15 条成绩记录
#   3. 查询每个学生的平均分，按平均分降序排列
#   4. 查询数学成绩 > 90 的学生姓名
#   5. 统计每门课程的平均分、最高分、最低分
#   6. 实现删除学生时，其成绩记录自动删除（外键级联）
#
# 练习 2（进阶）：图书借阅系统
#   要求：
#   1. 设计表结构：books（图书）、users（用户）、borrows（借阅记录）
#   2. 实现借书功能（检查库存，更新借阅记录）
#   3. 实现还书功能（更新借阅状态）
#   4. 查询某用户的借阅历史
#   5. 查询当前未归还的图书列表
#   6. 统计每本书的借阅次数，找出最受欢迎的前 5 本书
#   7. 使用事务确保借书/还书操作的原子性
#
# 练习 3（综合）：博客系统（多对多关系）
#   要求：
#   1. 设计表：users、posts、tags、post_tags（文章-标签关联表）
#   2. 实现发布文章功能（同时关联多个标签）
#   3. 查询某个标签下的所有文章
#   4. 查询某篇文章的所有标签
#   5. 统计每个标签的文章数量
#   6. 实现全文搜索（标题或内容包含关键词）
#   7. 使用 CTE 查询发文超过 3 篇的活跃用户
#
# 练习 4（高级）：性能优化实战
#   要求：
#   1. 创建包含 10 万条记录的 users 表
#   2. 对比有索引和无索引时的查询速度（WHERE、ORDER BY）
#   3. 使用 EXPLAIN QUERY PLAN 分析查询计划
#   4. 测试 executemany 与循环 execute 的性能差异
#   5. 测试事务批量提交与逐条提交的性能差异
#
# 练习 5（综合）：数据迁移工具
#   要求：
#   1. 从 CSV 文件导入数据到 SQLite
#   2. 将 SQLite 数据导出为 JSON 格式
#   3. 实现两个 SQLite 数据库之间的表复制
#   4. 处理导入时的数据清洗（去重、格式转换）
#   5. 使用事务确保导入失败时回滚
#
# 练习 6（高级）：实现简易 ORM
#   要求：
#   1. 设计 Model 基类，支持定义表结构
#   2. 实现 save() 方法（INSERT 或 UPDATE）
#   3. 实现 delete() 方法
#   4. 实现 find_by_id()、find_all()、find_by() 等查询方法
#   5. 支持链式查询（where().order_by().limit()）
#   6. 自动处理类型转换（datetime、bool 等）
#   示例用法：
#     class User(Model):
#         table_name = 'users'
#         fields = {'id': int, 'name': str, 'age': int}
#
#     user = User(name='Alice', age=25)
#     user.save()
#
#     users = User.find_by(age=25)
#
# 练习答案提示：
#   练习1：用 FOREIGN KEY ... ON DELETE CASCADE 实现级联删除
#          用 GROUP BY + AVG() 计算平均分
#   练习2：借书时在事务中检查 stock > 0，然后减库存并插入借阅记录
#          还书时更新 borrows.returned_at，增加 books.stock
#   练习3：多对多通过 post_tags 关联表实现，使用 GROUP_CONCAT 聚合标签
#          全文搜索用 LIKE '%keyword%' 匹配标题和内容
#   练习4：对比前用 time.perf_counter() 计时，EXPLAIN QUERY PLAN 查看索引使用
#          批量事务：将 N 条插入放在一个 BEGIN/COMMIT 块中
#   练习5：用 csv 模块读取 CSV，json.dumps 导出，ATTACH DATABASE 复制表
#          清洗阶段用 INSERT OR IGNORE 去重
#   练习6：基类维护连接，__init_subclass__ 自动建表
#          save() 根据是否有 id 决定 INSERT 或 UPDATE
#          find_by() 动态构建 WHERE 子句


# =============================================================================
# 【练习答案】
# =============================================================================


# ── 练习 1：学生成绩管理系统 ────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示学生成绩管理系统。"""
    print("exercise1_answer 演示: 学生成绩管理系统")
    print()

    with closing(sqlite3.connect(":memory:")) as conn:
        conn.execute("PRAGMA foreign_keys = ON")
        conn.row_factory = sqlite3.Row

        # ── 建表 ──────────────────────────────────────────
        conn.executescript("""
            CREATE TABLE students (
                id   INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL,
                age  INTEGER
            );
            CREATE TABLE scores (
                id         INTEGER PRIMARY KEY AUTOINCREMENT,
                student_id INTEGER NOT NULL,
                subject    TEXT    NOT NULL,
                score      REAL    NOT NULL,
                FOREIGN KEY (student_id) REFERENCES students(id)
                    ON DELETE CASCADE
            );
        """)

        # ── 插入数据 ───────────────────────────────────────
        conn.executemany(
            "INSERT INTO students (name, age) VALUES (?, ?)",
            [('Alice',20),('Bob',21),('Charlie',19),('Diana',22),('Eve',20)],
        )
        conn.executemany(
            "INSERT INTO scores (student_id, subject, score) VALUES (?, ?, ?)",
            [
                (1,'数学',95),(1,'英语',88),(1,'Python',92),
                (2,'数学',78),(2,'英语',85),(2,'Python',80),
                (3,'数学',91),(3,'英语',76),(3,'Python',88),
                (4,'数学',85),(4,'英语',92),(4,'Python',95),
                (5,'数学',72),(5,'英语',68),(5,'Python',75),
            ],
        )
        conn.commit()

        # ── 1. 每个学生的平均分（降序）────────────────────
        print("  ── 学生平均分（降序）──")
        rows = conn.execute("""
            SELECT s.name, ROUND(AVG(sc.score), 1) AS avg_score
            FROM students s
            INNER JOIN scores sc ON s.id = sc.student_id
            GROUP BY s.id
            ORDER BY avg_score DESC
        """).fetchall()
        for row in rows:
            print(f"  {row['name']:<10} 均分: {row['avg_score']}")
        print()

        # ── 2. 数学成绩 > 90 的学生 ────────────────────────
        print("  ── 数学成绩 > 90 的学生 ──")
        rows = conn.execute("""
            SELECT s.name, sc.score
            FROM students s
            INNER JOIN scores sc ON s.id = sc.student_id
            WHERE sc.subject = '数学' AND sc.score > 90
        """).fetchall()
        for row in rows:
            print(f"  {row['name']}: {row['score']}")
        print()

        # ── 3. 各科目统计 ──────────────────────────────────
        print("  ── 各科目统计 ──")
        rows = conn.execute("""
            SELECT subject,
                   ROUND(AVG(score), 1) AS avg_s,
                   MAX(score) AS max_s,
                   MIN(score) AS min_s
            FROM scores
            GROUP BY subject
        """).fetchall()
        for row in rows:
            print(f"  {row['subject']}: 均分={row['avg_s']}, "
                  f"最高={row['max_s']}, 最低={row['min_s']}")
        print()

        # ── 4. 测试级联删除 ────────────────────────────────
        print("  ── 测试级联删除（删除 Eve，其成绩自动删除）──")
        conn.execute("DELETE FROM students WHERE name='Eve'")
        conn.commit()
        cnt = conn.execute("SELECT COUNT(*) FROM scores WHERE student_id=5").fetchone()[0]
        print(f"  Eve 的成绩记录数: {cnt} （应为 0）")
    print()


# ── 练习 2：图书借阅系统 ────────────────────────────────────────────────────

def exercise2_answer() -> None:
    """练习 2：演示图书借阅系统。"""
    print("exercise2_answer 演示: 图书借阅系统")
    print()

    with closing(sqlite3.connect(":memory:")) as conn:
        conn.execute("PRAGMA foreign_keys = ON")
        conn.row_factory = sqlite3.Row

        # ── 建表 ──────────────────────────────────────────
        conn.executescript("""
            CREATE TABLE books (
                id    INTEGER PRIMARY KEY AUTOINCREMENT,
                title TEXT NOT NULL,
                stock INTEGER NOT NULL DEFAULT 0 CHECK(stock >= 0)
            );
            CREATE TABLE users (
                id   INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL
            );
            CREATE TABLE borrows (
                id          INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id     INTEGER NOT NULL,
                book_id     INTEGER NOT NULL,
                borrowed_at TEXT NOT NULL DEFAULT (datetime('now','localtime')),
                returned_at TEXT,
                FOREIGN KEY (user_id) REFERENCES users(id),
                FOREIGN KEY (book_id) REFERENCES books(id)
            );
        """)

        # ── 插入数据 ───────────────────────────────────────
        conn.executemany(
            "INSERT INTO books (title, stock) VALUES (?, ?)",
            [('Python入门',3),('算法导论',2),('设计模式',1)],
        )
        conn.executemany(
            "INSERT INTO users (name) VALUES (?)",
            [('Alice',),('Bob',)],
        )
        conn.commit()

        # ── 借书功能（事务）────────────────────────────────
        print("  ── Alice 借《Python入门》──")
        try:
            with conn:
                # 检查库存
                book = conn.execute(
                    "SELECT id, stock FROM books WHERE title=?", ('Python入门',)
                ).fetchone()
                if book['stock'] <= 0:
                    raise ValueError("库存不足")
                # 减库存
                conn.execute("UPDATE books SET stock=stock-1 WHERE id=?", (book['id'],))
                # 插入借阅记录
                conn.execute(
                    "INSERT INTO borrows (user_id, book_id) VALUES (?, ?)",
                    (1, book['id']),
                )
            print("  借书成功")
        except ValueError as e:
            print(f"  借书失败: {e}")

        # ── 查询未归还图书 ─────────────────────────────────
        print()
        print("  ── 当前未归还图书 ──")
        rows = conn.execute("""
            SELECT b.title, u.name, br.borrowed_at
            FROM borrows br
            INNER JOIN books b ON br.book_id = b.id
            INNER JOIN users u ON br.user_id = u.id
            WHERE br.returned_at IS NULL
        """).fetchall()
        for row in rows:
            print(f"  《{row['title']}》 by {row['name']}, 借于 {row['borrowed_at']}")
        print()

        # ── 还书功能 ───────────────────────────────────────
        print("  ── Alice 还《Python入门》──")
        with conn:
            conn.execute("""
                UPDATE borrows
                SET returned_at = datetime('now','localtime')
                WHERE user_id=1 AND book_id=1 AND returned_at IS NULL
            """)
            conn.execute("UPDATE books SET stock=stock+1 WHERE id=1")
        print("  还书成功")
    print()


# ── 练习 3：博客系统 ────────────────────────────────────────────────────────

def exercise3_answer() -> None:
    """练习 3：演示博客系统（多对多关系）。"""
    print("exercise3_answer 演示: 博客系统")
    print()

    with closing(sqlite3.connect(":memory:")) as conn:
        conn.execute("PRAGMA foreign_keys = ON")
        conn.row_factory = sqlite3.Row

        # ── 建表（与本章演示相同）──────────────────────────
        conn.executescript("""
            CREATE TABLE users (
                id   INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL
            );
            CREATE TABLE posts (
                id      INTEGER PRIMARY KEY AUTOINCREMENT,
                user_id INTEGER NOT NULL,
                title   TEXT NOT NULL,
                content TEXT,
                FOREIGN KEY (user_id) REFERENCES users(id)
            );
            CREATE TABLE tags (
                id   INTEGER PRIMARY KEY AUTOINCREMENT,
                name TEXT NOT NULL UNIQUE
            );
            CREATE TABLE post_tags (
                post_id INTEGER NOT NULL,
                tag_id  INTEGER NOT NULL,
                PRIMARY KEY (post_id, tag_id),
                FOREIGN KEY (post_id) REFERENCES posts(id) ON DELETE CASCADE,
                FOREIGN KEY (tag_id) REFERENCES tags(id) ON DELETE CASCADE
            );
        """)

        # ── 插入数据 ───────────────────────────────────────
        conn.executemany("INSERT INTO users (name) VALUES (?)",
                         [('Alice',),('Bob',),('Charlie',)])
        conn.executemany("INSERT INTO tags (name) VALUES (?)",
                         [('Python',),('Web',),('数据库',),('算法',)])
        conn.executemany(
            "INSERT INTO posts (user_id, title, content) VALUES (?, ?, ?)",
            [(1,'Python基础','内容1'),(1,'Web开发','内容2'),
             (1,'数据库入门','内容3'),(2,'算法学习','内容4')],
        )
        conn.executemany("INSERT INTO post_tags VALUES (?, ?)",
                         [(1,1),(2,1),(2,2),(3,1),(3,3),(4,4)])
        conn.commit()

        # ── 查询某标签下的文章 ─────────────────────────────
        print("  ── 标签'Python'下的文章 ──")
        rows = conn.execute("""
            SELECT p.title, u.name AS author
            FROM posts p
            INNER JOIN post_tags pt ON p.id = pt.post_id
            INNER JOIN tags t ON pt.tag_id = t.id
            INNER JOIN users u ON p.user_id = u.id
            WHERE t.name = 'Python'
        """).fetchall()
        for row in rows:
            print(f"  《{row['title']}》 by {row['author']}")
        print()

        # ── CTE 查询活跃用户 ───────────────────────────────
        print("  ── 发文 >= 3 篇的活跃用户（CTE）──")
        rows = conn.execute("""
            WITH post_counts AS (
                SELECT user_id, COUNT(*) AS cnt
                FROM posts GROUP BY user_id
            )
            SELECT u.name, pc.cnt
            FROM users u
            INNER JOIN post_counts pc ON u.id = pc.user_id
            WHERE pc.cnt >= 3
        """).fetchall()
        for row in rows:
            print(f"  {row['name']}: {row['cnt']} 篇")
    print()


# ── 练习 4-6：提示性实现 ────────────────────────────────────────────────────
#
# 练习 4-6 的完整实现较为复杂，这里提供实现思路：
#
# 练习 4（性能优化）：
#   - 使用 time.perf_counter() 计时
#   - 生成 10 万条数据：executemany + 事务批量提交
#   - 对比：无索引查询 vs 有索引查询的耗时
#
# 练习 5（数据迁移）：
#   - CSV 导入：csv.DictReader 读取，executemany 批量插入
#   - JSON 导出：fetchall() + json.dumps()
#   - 跨库复制：ATTACH DATABASE 'other.db' AS other; INSERT INTO ... SELECT ...
#
# 练习 6（简易 ORM）：
#   - Model 基类：__init_subclass__ 钩子自动建表
#   - save()：if self.id: UPDATE else: INSERT
#   - 链式查询：返回 self，支持 Model.where(...).order_by(...).limit(...)


# =============================================================================
# 练习运行器（取消注释以运行练习答案）
# =============================================================================

def run_exercises() -> None:
    """运行所有练习答案。"""
    print("\n" + "=" * 60)
    print("运行练习答案")
    print("=" * 60)
    print()

    exercise1_answer()
    exercise2_answer()
    exercise3_answer()

    print("=" * 60)
    print("练习答案演示完成")
    print("=" * 60)


# 取消下面的注释以运行练习答案
# if __name__ == "__main__":
#     run_exercises()
