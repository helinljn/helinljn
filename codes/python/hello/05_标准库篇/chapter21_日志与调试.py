# =============================================================================
# 第 21 章：日志与调试（logging、traceback、pdb 模块）
# =============================================================================
#
# 【学习目标】
#   1. 理解日志系统的设计思想，掌握日志级别和基本使用
#   2. 掌握 Handler（处理器）和 Formatter（格式化器）的配置
#   3. 学会日志文件轮转（避免日志文件无限增长）
#   4. 理解 Logger 的层次结构与日志传播机制
#   5. 掌握 traceback 模块提取和格式化异常信息
#   6. 学会使用 pdb 调试器定位程序问题
#   7. 能构建一套贴近生产环境的日志系统
#
# 【这些模块解决什么问题？】
#
#   logging 模块：
#     - 替代 print() 进行程序运行状态记录
#     - 支持按级别过滤信息（DEBUG/INFO/WARNING/ERROR/CRITICAL）
#     - 支持同时输出到控制台、文件、网络等多个目标
#     - 支持格式化（时间戳、文件名、行号、线程等上下文信息）
#     - 支持日志文件自动轮转，防止磁盘撑满
#
#   traceback 模块：
#     - 格式化和打印异常堆栈信息
#     - 在异常处理中保存完整的错误追踪链
#     - 用于日志记录、错误报告生成
#
#   pdb 模块：
#     - Python 内置的交互式命令行调试器
#     - 支持断点、单步执行、查看变量、调用栈检查
#     - 无需 IDE 也能精确定位 Bug
#
# 【与 C/C++ 对比】
#   C/C++ 通常使用 printf/fprintf 输出调试信息，或使用 syslog
#   Python 的 logging 模块相当于一个功能完整的企业级日志框架
#   pdb 相当于 gdb 的 Python 版本，使用体验类似
#
# 【运行方式】
#   python chapter21_日志与调试.py
#
# =============================================================================

import logging
import logging.config
import logging.handlers
import os
import sys
import time
import traceback
import tempfile
from pathlib import Path
from typing import Optional


# =============================================================================
# 21.1 logging 模块基础：日志级别与快速入门
# =============================================================================

def demo_logging_basics() -> None:
    """演示 logging 模块的基本用法和日志级别。"""
    print("=" * 60)
    print("21.1 logging 模块基础：日志级别与快速入门")
    print("=" * 60)

    # ── 为什么不用 print？ ────────────────────────────────
    print("【为什么不用 print() 做日志？】")
    print()
    print("  print() 的缺点：")
    print("  1. 无法按重要程度过滤信息（调试信息和错误信息混在一起）")
    print("  2. 无法自动记录时间、文件名、行号等上下文")
    print("  3. 无法同时输出到多个目标（控制台 + 文件 + 远程服务）")
    print("  4. 生产环境想关闭调试输出时，必须逐一删除 print()")
    print()
    print("  logging 的优点：")
    print("  1. 按级别过滤：只需修改一行配置即可关闭所有 DEBUG 输出")
    print("  2. 自动记录上下文：时间、模块名、行号、线程 ID 等")
    print("  3. 灵活输出：同时写文件和控制台，甚至发邮件")
    print("  4. 层次化：不同模块使用不同的 Logger，互相独立")
    print()

    # ── 日志级别 ──────────────────────────────────────────
    print("【日志级别（从低到高）】")
    print()

    levels = [
        (logging.DEBUG,    'DEBUG',    10, '调试信息：开发阶段用于追踪程序执行流程'),
        (logging.INFO,     'INFO',     20, '普通信息：程序正常运行的状态汇报'),
        (logging.WARNING,  'WARNING',  30, '警告信息：不影响运行但需要注意的问题'),
        (logging.ERROR,    'ERROR',    40, '错误信息：某个功能出错但程序还能继续'),
        (logging.CRITICAL, 'CRITICAL', 50, '严重错误：程序无法继续运行的致命问题'),
    ]

    print(f"  {'级别名称':<12} {'数值':<8} 使用场景")
    print(f"  {'-'*12:<12} {'-'*8:<8} {'-'*35}")
    for level, name, value, desc in levels:
        print(f"  {name:<12} {value:<8} {desc}")
    print()
    print("  规则：只有 >= 当前设置级别的日志才会被输出")
    print("  例如：设置为 WARNING，则 DEBUG 和 INFO 不会输出")
    print()

    # ── 最简单的使用方式 ──────────────────────────────────
    print("【最简单的使用方式：模块级函数】")
    print()
    print("  logging 提供了与根 Logger 对应的模块级快捷函数：")
    print("  logging.debug()、logging.info()、logging.warning()")
    print("  logging.error()、logging.critical()")
    print()

    # 演示基本配置
    # basicConfig 只在根 Logger 没有处理器时生效
    # 这里创建一个独立演示，避免影响后续示例
    demo_logger = logging.getLogger('demo_basics')
    demo_logger.setLevel(logging.DEBUG)

    # 移除已有的处理器（防止重复输出）
    demo_logger.handlers.clear()
    demo_logger.propagate = False  # 不向上传播，避免影响根 logger

    # 添加控制台处理器
    handler = logging.StreamHandler()
    handler.setFormatter(logging.Formatter('  [%(levelname)s] %(message)s'))
    demo_logger.addHandler(handler)

    print("  输出示例（当前级别=DEBUG，所有级别都会输出）：")
    demo_logger.debug("这是 DEBUG 信息：正在初始化数据库连接...")
    demo_logger.info("这是 INFO 信息：服务器启动成功，监听端口 8080")
    demo_logger.warning("这是 WARNING 信息：磁盘空间不足，剩余 10%")
    demo_logger.error("这是 ERROR 信息：文件读取失败 - FileNotFoundError")
    demo_logger.critical("这是 CRITICAL 信息：内存溢出，程序即将崩溃！")
    print()

    # 修改级别后的效果
    demo_logger.setLevel(logging.WARNING)
    print("  修改级别为 WARNING 后（DEBUG 和 INFO 不再输出）：")
    demo_logger.debug("这条 DEBUG 信息不会显示")
    demo_logger.info("这条 INFO 信息不会显示")
    demo_logger.warning("这条 WARNING 信息会显示")
    demo_logger.error("这条 ERROR 信息会显示")
    print()

    # ── 日志记录额外信息 ──────────────────────────────────
    print("【在日志中记录异常信息】")
    print()

    error_logger = logging.getLogger('demo_exception')
    error_logger.setLevel(logging.DEBUG)
    error_logger.handlers.clear()
    error_logger.propagate = False
    handler2 = logging.StreamHandler()
    handler2.setFormatter(logging.Formatter('  [%(levelname)s] %(message)s'))
    error_logger.addHandler(handler2)

    print("  使用 exc_info=True 或 exception() 自动附加异常堆栈：")
    try:
        result = 10 / 0
    except ZeroDivisionError:
        # error() + exc_info=True 会自动追加当前异常的 traceback
        error_logger.error("计算失败：除零错误", exc_info=True)
    print()

    # exception() 是 error() + exc_info=True 的快捷方式
    try:
        int("not a number")
    except ValueError:
        error_logger.exception("数值转换失败")  # 等同于 error(..., exc_info=True)
    print()


# =============================================================================
# 21.2 格式化器（Formatter）与处理器（Handler）
# =============================================================================

def demo_formatter_and_handler() -> None:
    """演示 Formatter（格式化器）和 Handler（处理器）的配置。"""
    print("\n" + "=" * 60)
    print("21.2 格式化器（Formatter）与处理器（Handler）")
    print("=" * 60)

    print("【核心架构】")
    print()
    print("  Logger（日志器）：日志的入口，负责接收日志调用")
    print("  Handler（处理器）：决定日志发往哪里（控制台/文件/网络）")
    print("  Formatter（格式化器）：决定日志的输出格式")
    print("  Filter（过滤器）：对日志进行额外筛选")
    print()
    print("  关系：Logger → Handler → Formatter")
    print("        一个 Logger 可以有多个 Handler")
    print("        每个 Handler 可以有一个 Formatter")
    print()

    # ── Formatter 格式化字段 ──────────────────────────────
    print("【Formatter 常用格式字段】")
    print()

    fields = [
        ('%(asctime)s',     '日志时间',               '2024-03-15 10:23:45,123'),
        ('%(name)s',        'Logger 名称',             'myapp.database'),
        ('%(levelname)s',   '级别名称',                'INFO'),
        ('%(levelno)s',     '级别数值',                '20'),
        ('%(message)s',     '日志消息',                'Connection established'),
        ('%(filename)s',    '源文件名',                'app.py'),
        ('%(module)s',      '模块名（去掉 .py）',      'app'),
        ('%(funcName)s',    '函数名',                  'connect_db'),
        ('%(lineno)d',      '行号',                    '42'),
        ('%(pathname)s',    '文件完整路径',             '/project/app.py'),
        ('%(process)d',     '进程 ID',                 '12345'),
        ('%(processName)s', '进程名称',                'MainProcess'),
        ('%(thread)d',      '线程 ID',                 '140735...'),
        ('%(threadName)s',  '线程名称',                'MainThread'),
    ]

    print(f"  {'字段':<22} {'说明':<20} 示例值")
    print(f"  {'-'*22:<22} {'-'*20:<20} {'-'*30}")
    for field, desc, example in fields:
        print(f"  {field:<22} {desc:<20} {example}")
    print()

    # ── 创建不同格式的 Formatter ──────────────────────────
    print("【常用日志格式示例】")
    print()

    formats = {
        '简洁格式': '%(levelname)s: %(message)s',
        '带时间格式': '%(asctime)s [%(levelname)s] %(message)s',
        '详细格式': '%(asctime)s [%(levelname)-8s] %(name)s:%(lineno)d - %(message)s',
        '带线程格式': '%(asctime)s [%(levelname)s] [%(threadName)s] %(message)s',
    }

    logger = logging.getLogger('format_demo')
    logger.setLevel(logging.DEBUG)
    logger.handlers.clear()
    logger.propagate = False

    for fmt_name, fmt_str in formats.items():
        handler = logging.StreamHandler()
        formatter = logging.Formatter(fmt_str, datefmt='%H:%M:%S')
        handler.setFormatter(formatter)
        logger.handlers.clear()
        logger.addHandler(handler)
        print(f"  {fmt_name}：{fmt_str}")
        logger.info("用户登录成功")
        print()

    # ── 各种 Handler ──────────────────────────────────────
    print("【常用 Handler 类型】")
    print()

    handlers_info = [
        ('StreamHandler',          '输出到流（默认是 stderr，可改为 stdout）'),
        ('FileHandler',            '输出到文件'),
        ('RotatingFileHandler',    '按文件大小自动轮转'),
        ('TimedRotatingFileHandler', '按时间自动轮转（每天/每小时等）'),
        ('NullHandler',            '不输出任何内容（库开发时使用）'),
        ('MemoryHandler',          '缓存到内存，达到阈值后批量输出'),
        ('SMTPHandler',            '通过邮件发送（适合严重错误通知）'),
        ('SysLogHandler',          '发送到系统日志（Linux syslog）'),
    ]

    for handler_name, desc in handlers_info:
        print(f"  logging.handlers.{handler_name}")
        print(f"    {desc}")
        print()

    # ── 实际配置：同时输出到控制台和文件 ─────────────────
    print("【实战：同时输出到控制台和文件】")
    print()

    # 使用临时文件演示
    with tempfile.NamedTemporaryFile(
        mode='w', suffix='.log', delete=False, prefix='demo_'
    ) as tmp:
        tmp_path = tmp.name

    try:
        app_logger = logging.getLogger('myapp')
        app_logger.setLevel(logging.DEBUG)
        app_logger.handlers.clear()
        app_logger.propagate = False

        # 处理器 1：控制台（只输出 INFO 及以上）
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(logging.INFO)
        console_handler.setFormatter(
            logging.Formatter('  [%(levelname)-8s] %(message)s')
        )

        # 处理器 2：文件（输出 DEBUG 及以上，记录更详细）
        file_handler = logging.FileHandler(tmp_path, encoding='utf-8')
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(
            logging.Formatter(
                '%(asctime)s [%(levelname)-8s] %(funcName)s:%(lineno)d - %(message)s',
                datefmt='%Y-%m-%d %H:%M:%S'
            )
        )

        app_logger.addHandler(console_handler)
        app_logger.addHandler(file_handler)

        print("  日志写入（控制台只显示 INFO+，文件记录全部）：")
        app_logger.debug("正在检查配置文件...")     # 只写文件
        app_logger.info("服务启动成功")             # 控制台 + 文件
        app_logger.warning("配置文件中有废弃选项")  # 控制台 + 文件
        app_logger.error("数据库连接失败")          # 控制台 + 文件
        print()

        # 读取并展示文件内容
        print("  日志文件内容（包含 DEBUG 信息）：")
        with open(tmp_path, encoding='utf-8') as f:
            for line in f:
                print(f"    {line.rstrip()}")
        print()

    finally:
        # 清理临时文件
        try:
            os.unlink(tmp_path)
        except OSError:
            pass


# =============================================================================
# 21.3 Logger 层次结构与日志传播
# =============================================================================

def demo_logger_hierarchy() -> None:
    """演示 Logger 的层次结构和日志传播机制。"""
    print("\n" + "=" * 60)
    print("21.3 Logger 层次结构与日志传播")
    print("=" * 60)

    print("【Logger 的命名与层次结构】")
    print()
    print("  Logger 通过 '.' 分隔的名称形成层次结构，类似 Python 包结构：")
    print()
    print("  root（根 Logger）")
    print("  └── myapp")
    print("      ├── myapp.database")
    print("      │   └── myapp.database.pool")
    print("      └── myapp.api")
    print("          └── myapp.api.auth")
    print()
    print("  logging.getLogger('myapp.database') 会找到（或创建）名为")
    print("  'myapp.database' 的 Logger，其父级是 'myapp'，祖父是 root")
    print()

    print("【日志传播机制（propagate）】")
    print()
    print("  默认情况下，Logger 处理完日志后会向上传播给父 Logger")
    print("  父 Logger 的所有 Handler 也会处理这条日志")
    print("  设置 logger.propagate = False 可以阻止向上传播")
    print()

    # 创建演示 logger（独立的命名空间）
    fmt = logging.Formatter('  %(name)-25s [%(levelname)s] %(message)s')

    # 根 logger 配置
    root_logger = logging.getLogger('hier_demo')
    root_logger.setLevel(logging.DEBUG)
    root_logger.handlers.clear()
    root_logger.propagate = False
    root_handler = logging.StreamHandler(sys.stdout)
    root_handler.setFormatter(fmt)
    root_logger.addHandler(root_handler)

    # 子 logger：myapp.database
    db_logger = logging.getLogger('hier_demo.database')
    db_logger.setLevel(logging.DEBUG)
    # db_logger 没有自己的 Handler，依靠传播到父级处理

    # 孙 logger：myapp.database.pool
    pool_logger = logging.getLogger('hier_demo.database.pool')
    pool_logger.setLevel(logging.DEBUG)

    print("  示例（子 logger 消息传播到父 logger 的 Handler）：")
    root_logger.info("根 Logger 直接记录")
    db_logger.info("database Logger 记录（传播到 hier_demo 处理）")
    pool_logger.warning("database.pool Logger 记录（传播到 hier_demo 处理）")
    print()

    # 演示 propagate = False
    print("  设置 db_logger.propagate = False 后：")
    db_logger.propagate = False

    # db_logger 现在没有 handler 且 propagate=False，消息会丢失
    # 给它加一个 handler 来演示独立性
    db_handler = logging.StreamHandler(sys.stdout)
    db_handler.setFormatter(
        logging.Formatter('  %(name)-25s [%(levelname)s] 独立处理: %(message)s')
    )
    db_logger.addHandler(db_handler)

    db_logger.info("这条消息只由 db_logger 处理，不会传播到根")
    root_logger.info("根 Logger 自己的消息不受影响")
    print()

    print("【在实际项目中的最佳实践】")
    print()
    print("  # 在每个模块顶部创建模块级 Logger（推荐做法）：")
    print("  logger = logging.getLogger(__name__)")
    print()
    print("  好处：")
    print("  1. __name__ 自动成为 'package.module' 格式的层次名称")
    print("  2. 调用者知道日志来自哪个模块")
    print("  3. 可以针对特定模块单独调整日志级别")
    print()
    print("  # 只在程序入口（main.py）配置 Handler")
    print("  # 库代码只使用 NullHandler，避免干扰用户的日志配置：")
    print("  logging.getLogger('mylib').addHandler(logging.NullHandler())")
    print()


# =============================================================================
# 21.4 日志配置方式
# =============================================================================

def demo_logging_config() -> None:
    """演示三种日志配置方式：basicConfig、dictConfig、fileConfig。"""
    print("\n" + "=" * 60)
    print("21.4 日志配置方式")
    print("=" * 60)

    # ── 方式一：basicConfig（快速配置，适合小脚本）────────
    print("【方式一：logging.basicConfig()（最简单，适合脚本）】")
    print()
    print("  basicConfig() 一次性配置根 Logger：")
    print()
    print("  logging.basicConfig(")
    print("      level=logging.DEBUG,                    # 日志级别")
    print("      format='%(asctime)s [%(levelname)s] %(message)s',  # 格式")
    print("      datefmt='%Y-%m-%d %H:%M:%S',            # 时间格式")
    print("      filename='app.log',                     # 写入文件（省略则输出到控制台）")
    print("      filemode='w',                           # 'w' 覆盖 / 'a' 追加（默认）")
    print("      encoding='utf-8',                       # 文件编码")
    print("      handlers=[...],                         # 自定义 Handler 列表")
    print("  )")
    print()
    print("  [!] 注意：basicConfig() 只在根 Logger 没有 Handler 时生效")
    print("    如果根 Logger 已经有 Handler（比如已经调用过 basicConfig），")
    print("    再次调用不会有任何效果，除非传入 force=True（Python 3.8+）")
    print()

    # ── 方式二：dictConfig（字典配置，适合中大型项目）─────
    print("【方式二：logging.config.dictConfig()（推荐，灵活且完整）】")
    print()
    print("  使用字典描述完整的日志配置，可来自 JSON/YAML 配置文件：")
    print()

    # 构建配置字典
    LOGGING_CONFIG = {
        'version': 1,                    # 必须为 1（目前只有版本 1）
        'disable_existing_loggers': False,  # 不禁用已存在的 Logger

        # ── 格式化器定义 ──────────────────────────────────
        'formatters': {
            'simple': {
                'format': '[%(levelname)s] %(message)s',
            },
            'detailed': {
                'format': '%(asctime)s [%(levelname)-8s] %(name)s:%(lineno)d - %(message)s',
                'datefmt': '%Y-%m-%d %H:%M:%S',
            },
            'verbose': {
                'format': (
                    '%(asctime)s [%(levelname)-8s] '
                    '[%(process)d:%(threadName)s] '
                    '%(name)s.%(funcName)s:%(lineno)d - %(message)s'
                ),
                'datefmt': '%Y-%m-%d %H:%M:%S',
            },
        },

        # ── 处理器定义 ────────────────────────────────────
        'handlers': {
            'console': {
                'class': 'logging.StreamHandler',
                'level': 'INFO',
                'formatter': 'simple',
                'stream': 'ext://sys.stdout',  # ext:// 引用外部对象
            },
            'file': {
                'class': 'logging.handlers.RotatingFileHandler',
                'level': 'DEBUG',
                'formatter': 'detailed',
                'filename': 'app.log',
                'maxBytes': 10 * 1024 * 1024,  # 10 MB
                'backupCount': 5,
                'encoding': 'utf-8',
            },
            'error_file': {
                'class': 'logging.FileHandler',
                'level': 'ERROR',
                'formatter': 'verbose',
                'filename': 'error.log',
                'encoding': 'utf-8',
            },
        },

        # ── Logger 定义 ───────────────────────────────────
        'loggers': {
            'myapp': {
                'level': 'DEBUG',
                'handlers': ['console', 'file'],
                'propagate': False,     # 不传播到根 Logger
            },
            'myapp.database': {
                'level': 'DEBUG',
                'handlers': ['error_file'],
                'propagate': True,      # 传播到父 Logger（myapp）
            },
        },

        # ── 根 Logger 配置 ────────────────────────────────
        'root': {
            'level': 'WARNING',
            'handlers': ['console'],
        },
    }

    # 格式化展示配置内容（不实际应用，避免污染当前 logger 环境）
    import json
    config_str = json.dumps(LOGGING_CONFIG, indent=2, ensure_ascii=False, default=str)
    print("  配置字典示例（已简化）：")
    # 只显示关键结构
    for line in config_str.split('\n')[:30]:
        print(f"    {line}")
    print("    ...")
    print()
    print("  应用配置：logging.config.dictConfig(LOGGING_CONFIG)")
    print()

    # ── 实际演示 dictConfig ────────────────────────────────
    print("  实际演示（使用临时文件）：")

    with tempfile.TemporaryDirectory() as tmp_dir:
        log_file = os.path.join(tmp_dir, 'app.log')
        err_file = os.path.join(tmp_dir, 'error.log')

        demo_config = {
            'version': 1,
            'disable_existing_loggers': False,
            'formatters': {
                'simple': {
                    'format': '    [%(levelname)-8s] %(name)s - %(message)s',
                },
                'detailed': {
                    'format': '    %(asctime)s [%(levelname)-8s] %(name)s:%(lineno)d - %(message)s',
                    'datefmt': '%H:%M:%S',
                },
            },
            'handlers': {
                'console': {
                    'class': 'logging.StreamHandler',
                    'level': 'INFO',
                    'formatter': 'simple',
                    'stream': 'ext://sys.stdout',
                },
                'file': {
                    'class': 'logging.FileHandler',
                    'level': 'DEBUG',
                    'formatter': 'detailed',
                    'filename': log_file,
                    'encoding': 'utf-8',
                },
            },
            'loggers': {
                'dictcfg_demo': {
                    'level': 'DEBUG',
                    'handlers': ['console', 'file'],
                    'propagate': False,
                },
            },
        }

        logging.config.dictConfig(demo_config)

        cfg_logger = logging.getLogger('dictcfg_demo')
        cfg_logger.debug("调试：连接到数据库 localhost:5432")  # 只写文件
        cfg_logger.info("信息：服务器启动完成")                 # 控制台 + 文件
        cfg_logger.warning("警告：缓存命中率低于 60%")          # 控制台 + 文件
        cfg_logger.error("错误：支付接口超时")                  # 控制台 + 文件
        print()

        # 展示文件中的 DEBUG 信息
        print("  文件中记录了额外的 DEBUG 信息：")
        with open(log_file, encoding='utf-8') as f:
            for line in f:
                print(f"  {line.rstrip()}")
        print()

        # Windows 上必须关闭 Handler，否则文件被锁定无法删除临时目录
        for h in cfg_logger.handlers[:]:
            h.close()
            cfg_logger.removeHandler(h)


# =============================================================================
# 21.5 日志文件轮转
# =============================================================================

def demo_log_rotation() -> None:
    """演示日志文件轮转：按大小轮转和按时间轮转。"""
    print("\n" + "=" * 60)
    print("21.5 日志文件轮转（Log Rotation）")
    print("=" * 60)

    print("【为什么需要日志轮转？】")
    print()
    print("  如果日志不断写入同一个文件：")
    print("  - 文件会无限增长，最终耗尽磁盘空间")
    print("  - 文件过大导致日志难以查阅和分析")
    print("  - 日志轮转可以自动切换到新文件，并保留指定数量的旧文件")
    print()

    # ── RotatingFileHandler：按大小轮转 ──────────────────
    print("【RotatingFileHandler：按文件大小轮转】")
    print()
    print("  参数：")
    print("    filename    ：日志文件路径")
    print("    maxBytes    ：单个文件最大字节数（达到后自动切换）")
    print("    backupCount ：保留的旧文件数量（超出则删除最老的）")
    print()
    print("  轮转过程（backupCount=3）：")
    print("    app.log → app.log.1 → app.log.2 → app.log.3 → 删除")
    print("    新日志始终写入 app.log")
    print()

    with tempfile.TemporaryDirectory() as tmp_dir:
        log_path = os.path.join(tmp_dir, 'rotating.log')

        rot_logger = logging.getLogger('rotating_demo')
        rot_logger.setLevel(logging.DEBUG)
        rot_logger.handlers.clear()
        rot_logger.propagate = False

        # 设置很小的 maxBytes 来演示轮转效果
        rot_handler = logging.handlers.RotatingFileHandler(
            filename=log_path,
            maxBytes=200,        # 演示用，设置为 200 字节（实际应设为 MB 级）
            backupCount=3,
            encoding='utf-8',
        )
        rot_handler.setFormatter(
            logging.Formatter('%(asctime)s [%(levelname)s] %(message)s',
                              datefmt='%H:%M:%S')
        )
        rot_logger.addHandler(rot_handler)

        # 写入足够多的日志触发轮转
        for i in range(20):
            rot_logger.info(f"这是第 {i+1:02d} 条日志消息，用于触发文件轮转")

        # Windows 上必须先 close() 再 remove，否则文件被锁定无法删除临时目录
        for h in rot_logger.handlers[:]:
            h.close()
            rot_logger.removeHandler(h)

        # 展示轮转结果
        files = sorted(
            [f for f in os.listdir(tmp_dir) if f.startswith('rotating')],
            reverse=True
        )
        print(f"  写入 20 条日志后，生成的文件（maxBytes=200, backupCount=3）：")
        for fname in files:
            fpath = os.path.join(tmp_dir, fname)
            size = os.path.getsize(fpath)
            print(f"    {fname:<25} ({size} 字节)")
        print()

        # 查看最新文件内容
        if os.path.exists(log_path):
            with open(log_path, encoding='utf-8') as f:
                lines = f.readlines()
            print(f"  当前 rotating.log 内容（最新的日志）：")
            for line in lines:
                print(f"    {line.rstrip()}")
        print()

    # ── TimedRotatingFileHandler：按时间轮转 ─────────────
    print("【TimedRotatingFileHandler：按时间轮转】")
    print()
    print("  参数：")
    print("    when        ：轮转时间单位")
    print("      'S'       - 每秒")
    print("      'M'       - 每分钟")
    print("      'H'       - 每小时")
    print("      'D'       - 每天（推荐）")
    print("      'midnight' - 每天午夜")
    print("      'W0'~'W6' - 每周一到周日")
    print("    interval    ：间隔数量（如 when='H', interval=6 表示每 6 小时）")
    print("    backupCount ：保留文件数量")
    print("    encoding    ：文件编码")
    print()

    with tempfile.TemporaryDirectory() as tmp_dir:
        timed_path = os.path.join(tmp_dir, 'timed.log')

        timed_logger = logging.getLogger('timed_demo')
        timed_logger.setLevel(logging.DEBUG)
        timed_logger.handlers.clear()
        timed_logger.propagate = False

        # 每秒轮转一次（演示用）
        timed_handler = logging.handlers.TimedRotatingFileHandler(
            filename=timed_path,
            when='S',            # 每秒轮转（演示用，实际用 'midnight' 或 'D'）
            interval=1,
            backupCount=2,
            encoding='utf-8',
        )
        timed_handler.setFormatter(
            logging.Formatter('%(asctime)s [%(levelname)s] %(message)s',
                              datefmt='%H:%M:%S')
        )
        timed_logger.addHandler(timed_handler)

        # 写日志并暂停触发时间轮转
        for i in range(3):
            timed_logger.info(f"第 {i+1} 批日志")
            time.sleep(1.1)  # 超过 1 秒触发轮转

        # Windows 上必须先 close() 再 remove，否则文件被锁定无法删除临时目录
        for h in timed_logger.handlers[:]:
            h.close()
            timed_logger.removeHandler(h)

        files = sorted(
            [f for f in os.listdir(tmp_dir) if f.startswith('timed')],
        )
        print(f"  写入 3 批日志（每批间隔 1 秒）后生成的文件：")
        for fname in files:
            fpath = os.path.join(tmp_dir, fname)
            size = os.path.getsize(fpath)
            print(f"    {fname:<40} ({size} 字节)")
        print()

    print("【实际生产环境推荐配置】")
    print()
    print("  # 每天午夜轮转，保留 30 天：")
    print("  handler = logging.handlers.TimedRotatingFileHandler(")
    print("      filename='logs/app.log',")
    print("      when='midnight',")
    print("      interval=1,")
    print("      backupCount=30,")
    print("      encoding='utf-8',")
    print("  )")
    print()
    print("  # 按大小轮转，每个文件最大 50MB，保留 10 个：")
    print("  handler = logging.handlers.RotatingFileHandler(")
    print("      filename='logs/app.log',")
    print("      maxBytes=50 * 1024 * 1024,")
    print("      backupCount=10,")
    print("      encoding='utf-8',")
    print("  )")
    print()


# =============================================================================
# 21.6 traceback 模块：异常追踪
# =============================================================================

def demo_traceback() -> None:
    """演示 traceback 模块的用法：格式化和提取异常信息。"""
    print("\n" + "=" * 60)
    print("21.6 traceback 模块：异常追踪")
    print("=" * 60)

    print("【traceback 模块解决什么问题？】")
    print()
    print("  当程序抛出异常时，Python 自动打印堆栈信息")
    print("  但有时需要：")
    print("  1. 捕获异常后，把堆栈信息写入日志文件（而不是输出到屏幕）")
    print("  2. 把堆栈信息转换成字符串，存入数据库或发送到监控系统")
    print("  3. 在嵌套异常中还原完整的调用链")
    print()

    # ── traceback.print_exc ────────────────────────────────
    print("1. traceback.print_exc()：打印当前异常的堆栈信息")
    print()

    def level3_function():
        """最内层函数，会抛出异常。"""
        raise ValueError("这是一个测试异常：数值无效")

    def level2_function():
        """中间层函数。"""
        level3_function()

    def level1_function():
        """外层函数。"""
        level2_function()

    print("  调用链：level1 → level2 → level3 → raise ValueError")
    print()
    print("  traceback.print_exc() 输出：")
    try:
        level1_function()
    except ValueError:
        traceback.print_exc()
    print()

    # ── traceback.format_exc ──────────────────────────────
    print("2. traceback.format_exc()：获取堆栈信息字符串")
    print()
    print("  与 print_exc() 不同，format_exc() 返回字符串，")
    print("  可以写入日志、存入数据库，或发送给监控系统：")
    print()

    try:
        level1_function()
    except ValueError:
        tb_str = traceback.format_exc()
        print(f"  type(tb_str) = {type(tb_str)}")
        print(f"  len(tb_str) = {len(tb_str)}")
        print()
        print("  前 3 行：")
        for line in tb_str.strip().split('\n')[:3]:
            print(f"    {line}")
        print("    ...")
        print()
        print("  写入日志示例：")
        print("    logger.error('处理请求失败:\\n' + traceback.format_exc())")
    print()

    # ── traceback.format_exception ────────────────────────
    print("3. traceback.format_exception()：格式化异常对象")
    print()
    print("  可以格式化已经保存的异常对象（不需要在 except 块内）：")
    print()

    saved_exc = None
    try:
        int("not a number")
    except ValueError as e:
        saved_exc = e  # 保存异常对象

    if saved_exc:
        # 格式化已保存的异常
        lines = traceback.format_exception(
            type(saved_exc),
            saved_exc,
            saved_exc.__traceback__
        )
        print("  格式化结果（列表形式）：")
        for line in lines:
            for sub_line in line.split('\n'):
                if sub_line:
                    print(f"    {sub_line}")
    print()

    # ── traceback.extract_tb ──────────────────────────────
    print("4. traceback.extract_tb()：提取堆栈帧信息")
    print()
    print("  可以获取每个堆栈帧的结构化信息（文件、行号、函数名、代码）：")
    print()

    try:
        level1_function()
    except ValueError as e:
        tb = e.__traceback__
        frames = traceback.extract_tb(tb)
        print(f"  堆栈帧数量：{len(frames)}")
        print(f"  每帧信息：")
        for i, frame in enumerate(frames):
            print(f"    帧 {i+1}：")
            print(f"      文件名：{Path(frame.filename).name}")
            print(f"      行号：  {frame.lineno}")
            print(f"      函数名：{frame.name}")
            print(f"      代码行：{frame.line}")
    print()

    # ── 异常链中的 traceback ──────────────────────────────
    print("5. 异常链与 __cause__：追踪嵌套异常")
    print()

    def connect_database(host: str) -> None:
        """模拟数据库连接失败。"""
        try:
            raise ConnectionError(f"无法连接到 {host}:5432")
        except ConnectionError as e:
            # raise ... from e 建立异常链，保留原始异常
            raise RuntimeError("数据库初始化失败") from e

    print("  使用 'raise B from A' 建立异常链：")
    try:
        connect_database("db.example.com")
    except RuntimeError:
        traceback.print_exc()
    print()


# =============================================================================
# 21.7 pdb 模块：交互式调试器
# =============================================================================

def demo_pdb_usage() -> None:
    """演示 pdb 调试器的使用方法（文档说明，不实际运行交互式调试）。"""
    print("\n" + "=" * 60)
    print("21.7 pdb 模块：交互式调试器")
    print("=" * 60)

    print("【pdb 是什么？】")
    print()
    print("  pdb（Python Debugger）是 Python 内置的命令行调试器")
    print("  类似 C/C++ 中的 gdb，支持：")
    print("  - 设置断点（breakpoint）")
    print("  - 单步执行（step into / step over）")
    print("  - 查看变量值和调用栈")
    print("  - 在运行时修改变量")
    print("  - 在任意位置进入调试模式")
    print()

    # ── 启动 pdb 的几种方式 ──────────────────────────────
    print("【启动 pdb 的三种方式】")
    print()

    print("  方式一：命令行启动（调试整个脚本）")
    print("  ─────────────────────────────────")
    print("  $ python -m pdb my_script.py")
    print("  程序会在第一行暂停，等待调试命令")
    print()

    print("  方式二：在代码中插入断点（最常用）")
    print("  ─────────────────────────────────")
    print("  # Python 3.7+：内置 breakpoint() 函数")
    print("  def my_function(data):")
    print("      result = process(data)")
    print("      breakpoint()    # 程序在此暂停，进入 pdb 交互模式")
    print("      return result")
    print()
    print("  # 旧写法（等效）：")
    print("  import pdb")
    print("  pdb.set_trace()    # 与 breakpoint() 等效")
    print()

    print("  方式三：捕获异常后进入调试（事后分析）")
    print("  ─────────────────────────────────────")
    print("  try:")
    print("      risky_operation()")
    print("  except Exception:")
    print("      import pdb, traceback")
    print("      traceback.print_exc()")
    print("      pdb.post_mortem()    # 进入事后调试，分析异常时的状态")
    print()

    # ── pdb 命令速查 ─────────────────────────────────────
    print("【pdb 常用命令速查表】")
    print()

    commands = [
        # (命令, 简写, 说明)
        ('help',        'h',    '显示帮助（help <cmd> 查看具体命令）'),
        ('list',        'l',    '显示当前位置附近的源代码'),
        ('next',        'n',    '执行下一行（不进入函数内部，Step Over）'),
        ('step',        's',    '进入函数内部（Step Into）'),
        ('return',      'r',    '运行到当前函数返回（Step Out）'),
        ('continue',    'c',    '继续执行到下一个断点'),
        ('break',       'b',    'b [行号] 设置断点；b [文件:行号]'),
        ('clear',       'cl',   '清除断点；cl [断点编号]'),
        ('where',       'w',    '显示当前调用栈（Call Stack）'),
        ('up',          'u',    '向上移动一个堆栈帧'),
        ('down',        'd',    '向下移动一个堆栈帧'),
        ('print',       'p',    'p <表达式> 打印变量/表达式的值'),
        ('pp',          'pp',   'pp <表达式> 美观打印（pprint）'),
        ('args',        'a',    '打印当前函数的所有参数'),
        ('locals',      '',     '查看局部变量（用 p locals() 或直接输入变量名）'),
        ('jump',        'j',    'j <行号> 跳转到指定行执行（谨慎使用）'),
        ('quit',        'q',    '退出调试器，终止程序'),
        ('!',           '',     '!<Python 表达式> 在调试器中执行任意 Python 代码'),
    ]

    print(f"  {'命令':<12} {'简写':<6} 说明")
    print(f"  {'-'*12:<12} {'-'*6:<6} {'-'*40}")
    for cmd, short, desc in commands:
        short_str = f"({short})" if short else ""
        print(f"  {cmd:<12} {short_str:<6} {desc}")
    print()

    # ── pdb 使用演示（静态代码展示）─────────────────────
    print("【pdb 调试示例（代码+命令序列）】")
    print()

    example_code = '''
  # 问题代码：计算平均值时出现 ZeroDivisionError
  def calculate_average(numbers):
      total = sum(numbers)
      count = len(numbers)
      breakpoint()    # ← 在这里暂停
      return total / count

  result = calculate_average([])   # 传入空列表
  print(result)
    '''

    debug_session = [
        ("(Pdb) l",        "查看源代码"),
        ("(Pdb) p total",  "→ 0（sum 返回 0）"),
        ("(Pdb) p count",  "→ 0（空列表长度为 0）"),
        ("(Pdb) p numbers","→ []（确认参数为空列表）"),
        ("(Pdb) n",        "执行 return total / count，触发异常"),
        ("(Pdb) q",        "退出调试"),
    ]

    print("  问题代码：")
    for line in example_code.strip().split('\n'):
        print(f"  {line}")
    print()
    print("  pdb 调试会话：")
    for cmd, note in debug_session:
        print(f"    {cmd:<25} # {note}")
    print()

    # ── breakpoint() 与环境变量 ──────────────────────────
    print("【breakpoint() 的高级用法（Python 3.7+）】")
    print()
    print("  breakpoint() 实际调用 sys.breakpointhook()")
    print("  可以通过环境变量 PYTHONBREAKPOINT 控制行为：")
    print()
    print("  # 禁用所有断点（不修改代码）：")
    print("  $ PYTHONBREAKPOINT=0 python my_script.py")
    print()
    print("  # 使用第三方调试器（如 ipdb）：")
    print("  $ PYTHONBREAKPOINT=ipdb.set_trace python my_script.py")
    print()
    print("  # 使用 VS Code 调试器（在 launch.json 中配置即可）")
    print("  VS Code 用户通常直接用 IDE 断点，不需要手写 breakpoint()")
    print()


# =============================================================================
# 21.8 实战：构建实用日志系统
# =============================================================================

def demo_practical_logging_system() -> None:
    """构建一套贴近生产环境的日志系统。"""
    print("\n" + "=" * 60)
    print("21.8 实战：构建实用日志系统")
    print("=" * 60)

    print("【目标】")
    print("  构建一个可复用的日志配置模块，满足以下需求：")
    print("  1. 控制台输出彩色日志（用 ANSI 颜色区分级别）")
    print("  2. 文件输出详细的结构化日志")
    print("  3. 错误单独写入 error.log 方便运维排查")
    print("  4. 支持通过参数灵活调整（日志级别、输出目录等）")
    print()

    # ── 彩色控制台 Formatter ──────────────────────────────

    class ColorFormatter(logging.Formatter):
        """带 ANSI 颜色的日志格式化器，使控制台输出更易读。"""

        # ANSI 颜色代码
        COLORS = {
            'DEBUG':    '\033[36m',     # 青色
            'INFO':     '\033[32m',     # 绿色
            'WARNING':  '\033[33m',     # 黄色
            'ERROR':    '\033[31m',     # 红色
            'CRITICAL': '\033[35m',     # 紫色
        }
        RESET = '\033[0m'
        BOLD  = '\033[1m'

        def format(self, record: logging.LogRecord) -> str:
            """重写 format 方法，在级别名称前后添加颜色代码。"""
            color = self.COLORS.get(record.levelname, '')
            record.levelname = (
                f"{self.BOLD}{color}{record.levelname:<8}{self.RESET}"
            )
            return super().format(record)

    # ── 日志系统初始化函数 ───────────────────────────────

    def setup_logging(
        name: str,
        level: int = logging.DEBUG,
        log_dir: Optional[str] = None,
        console_level: int = logging.INFO,
        max_bytes: int = 10 * 1024 * 1024,
        backup_count: int = 5,
    ) -> logging.Logger:
        """
        初始化并返回一个配置完善的 Logger。

        Args:
            name:          Logger 名称（推荐使用 __name__）
            level:         Logger 整体级别（最低记录级别）
            log_dir:       日志文件目录（None 则不写文件）
            console_level: 控制台输出级别
            max_bytes:     单个日志文件最大字节数
            backup_count:  保留的旧日志文件数量

        Returns:
            配置好的 Logger 实例
        """
        logger = logging.getLogger(name)
        logger.setLevel(level)
        logger.handlers.clear()
        logger.propagate = False

        # ── 控制台处理器（彩色输出）──────────────────────
        console_handler = logging.StreamHandler(sys.stdout)
        console_handler.setLevel(console_level)
        console_handler.setFormatter(
            ColorFormatter(
                fmt='  %(asctime)s [%(levelname)s] %(name)s - %(message)s',
                datefmt='%H:%M:%S',
            )
        )
        logger.addHandler(console_handler)

        # ── 文件处理器（详细日志，按大小轮转）────────────
        if log_dir:
            log_dir_path = Path(log_dir)
            log_dir_path.mkdir(parents=True, exist_ok=True)

            # 主日志文件
            file_handler = logging.handlers.RotatingFileHandler(
                filename=log_dir_path / f'{name}.log',
                maxBytes=max_bytes,
                backupCount=backup_count,
                encoding='utf-8',
            )
            file_handler.setLevel(logging.DEBUG)
            file_handler.setFormatter(
                logging.Formatter(
                    fmt=(
                        '%(asctime)s [%(levelname)-8s] '
                        '[%(process)d] %(name)s.%(funcName)s:%(lineno)d'
                        ' - %(message)s'
                    ),
                    datefmt='%Y-%m-%d %H:%M:%S',
                )
            )
            logger.addHandler(file_handler)

            # 错误日志文件（单独记录 ERROR 和 CRITICAL）
            error_handler = logging.handlers.RotatingFileHandler(
                filename=log_dir_path / f'{name}.error.log',
                maxBytes=max_bytes,
                backupCount=backup_count,
                encoding='utf-8',
            )
            error_handler.setLevel(logging.ERROR)
            error_handler.setFormatter(
                logging.Formatter(
                    fmt=(
                        '%(asctime)s [%(levelname)-8s] '
                        '[%(process)d:%(threadName)s] '
                        '%(name)s.%(funcName)s:%(lineno)d - %(message)s'
                    ),
                    datefmt='%Y-%m-%d %H:%M:%S',
                )
            )
            logger.addHandler(error_handler)

        return logger

    # ── 演示实际使用 ─────────────────────────────────────
    print("  演示（使用临时目录存储日志文件）：")
    print()

    with tempfile.TemporaryDirectory() as tmp_dir:
        # 初始化日志系统
        logger = setup_logging(
            name='myapp',
            level=logging.DEBUG,
            log_dir=tmp_dir,
            console_level=logging.DEBUG,
        )

        # 模拟一个 Web 请求处理流程
        def handle_request(user_id: int, action: str) -> dict:
            """模拟 Web 请求处理，包含完整的日志记录。"""
            logger.info("收到请求: user_id=%d, action=%s", user_id, action)

            if user_id <= 0:
                logger.warning("无效的 user_id: %d，使用默认值 1", user_id)
                user_id = 1

            logger.debug("开始处理 action='%s' for user_id=%d", action, user_id)

            if action == 'query':
                # 模拟数据库查询
                logger.debug("执行 SQL: SELECT * FROM users WHERE id=%d", user_id)
                result = {'user_id': user_id, 'name': 'Alice', 'status': 'active'}
                logger.info("查询成功，返回用户信息: %s", result)
                return result

            elif action == 'delete':
                logger.error("拒绝删除操作：用户 %d 无权限执行 delete", user_id)
                raise PermissionError(f"用户 {user_id} 无权限执行 delete 操作")

            else:
                logger.warning("未知 action: '%s'，忽略处理", action)
                return {}

        # 正常请求
        result = handle_request(42, 'query')

        # 无效参数（触发 WARNING）
        handle_request(-1, 'query')

        # 权限错误（触发 ERROR）
        try:
            handle_request(42, 'delete')
        except PermissionError:
            logger.exception("请求处理失败")

        # 未知操作（触发 WARNING）
        handle_request(42, 'unknown')

        print()
        print("  日志文件内容（myapp.log，包含所有级别）：")
        log_file = Path(tmp_dir) / 'myapp.log'
        if log_file.exists():
            with open(log_file, encoding='utf-8') as f:
                for line in f:
                    print(f"    {line.rstrip()}")

        print()
        print("  错误日志文件内容（myapp.error.log，只含 ERROR+）：")
        error_file = Path(tmp_dir) / 'myapp.error.log'
        if error_file.exists():
            with open(error_file, encoding='utf-8') as f:
                for line in f:
                    print(f"    {line.rstrip()}")
        print()

        # Windows 上必须关闭所有 Handler，否则文件被锁定无法删除临时目录
        for h in logger.handlers[:]:
            h.close()
            logger.removeHandler(h)


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示函数。"""
    demo_logging_basics()
    demo_formatter_and_handler()
    demo_logger_hierarchy()
    demo_logging_config()
    demo_log_rotation()
    demo_traceback()
    demo_pdb_usage()
    demo_practical_logging_system()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── logging 模块核心概念 ─────────────────────────────────
#
#   日志级别（数值从低到高）：
#     logging.DEBUG    = 10   调试信息
#     logging.INFO     = 20   普通信息
#     logging.WARNING  = 30   警告信息
#     logging.ERROR    = 40   错误信息
#     logging.CRITICAL = 50   严重错误
#
#   模块级快捷函数：
#     logging.debug(msg, *args, **kwargs)
#     logging.info(msg, *args, **kwargs)
#     logging.warning(msg, *args, **kwargs)
#     logging.error(msg, *args, **kwargs)
#     logging.critical(msg, *args, **kwargs)
#     logging.exception(msg, *args, **kwargs)  # 等同于 error(..., exc_info=True)
#
#   获取 Logger：
#     logger = logging.getLogger(name)         # 获取或创建 Logger
#     logger = logging.getLogger(__name__)     # 推荐：使用模块名作为 Logger 名
#
#   Logger 方法：
#     logger.setLevel(level)                   # 设置日志级别
#     logger.addHandler(handler)               # 添加处理器
#     logger.removeHandler(handler)            # 移除处理器
#     logger.debug/info/warning/error/critical(msg, *args)
#     logger.exception(msg, *args)             # 记录异常（自动附加 traceback）
#
#   Handler 类型：
#     logging.StreamHandler(stream)            # 输出到流（控制台）
#     logging.FileHandler(filename)            # 输出到文件
#     logging.handlers.RotatingFileHandler     # 按大小轮转
#     logging.handlers.TimedRotatingFileHandler # 按时间轮转
#     logging.NullHandler()                    # 不输出（库开发用）
#
#   Formatter 格式化：
#     formatter = logging.Formatter(fmt, datefmt)
#     handler.setFormatter(formatter)
#
#   配置方式：
#     logging.basicConfig(**kwargs)            # 快速配置根 Logger
#     logging.config.dictConfig(config_dict)   # 字典配置（推荐）
#     logging.config.fileConfig(filename)      # 从配置文件加载
#
# ── traceback 模块核心函数 ───────────────────────────────
#
#   traceback.print_exc(limit=None, file=None)
#     打印当前异常的堆栈信息到 file（默认 sys.stderr）
#
#   traceback.format_exc(limit=None)
#     返回当前异常的堆栈信息字符串
#
#   traceback.format_exception(exc_type, exc_value, exc_tb)
#     格式化指定的异常对象
#
#   traceback.extract_tb(tb, limit=None)
#     提取堆栈帧信息，返回 StackSummary 对象
#
#   traceback.print_stack(limit=None, file=None)
#     打印当前调用栈（不需要异常）
#
# ── pdb 调试器常用命令 ───────────────────────────────────
#
#   启动方式：
#     python -m pdb script.py                  # 命令行启动
#     breakpoint()                             # 代码中插入断点（Python 3.7+）
#     import pdb; pdb.set_trace()              # 旧写法
#     pdb.post_mortem()                        # 事后调试
#
#   导航命令：
#     l(ist)                                   # 显示源代码
#     w(here)                                  # 显示调用栈
#     u(p)                                     # 向上移动堆栈帧
#     d(own)                                   # 向下移动堆栈帧
#
#   执行命令：
#     n(ext)                                   # 执行下一行（Step Over）
#     s(tep)                                   # 进入函数（Step Into）
#     r(eturn)                                 # 运行到函数返回（Step Out）
#     c(ontinue)                               # 继续执行到下一个断点
#     j(ump) lineno                            # 跳转到指定行
#
#   断点命令：
#     b(reak) [lineno]                         # 设置断点
#     cl(ear) [bpnumber]                       # 清除断点
#
#   查看命令：
#     p(rint) expr                             # 打印表达式
#     pp expr                                  # 美观打印
#     a(rgs)                                   # 打印函数参数
#     !statement                               # 执行 Python 语句
#
#   退出命令：
#     q(uit)                                   # 退出调试器
#
# =============================================================================
# 【常见错误与解决方案】
# =============================================================================
#
# 1. 日志重复输出
#    问题：同一条日志被打印多次
#    原因：多次调用 addHandler() 或 basicConfig()
#    解决：在添加 Handler 前先清空：logger.handlers.clear()
#
# 2. basicConfig() 不生效
#    问题：调用 basicConfig() 后日志配置没有变化
#    原因：根 Logger 已经有 Handler（basicConfig 只在首次生效）
#    解决：使用 force=True（Python 3.8+）或手动配置 Logger
#
# 3. 日志级别设置无效
#    问题：设置了 logger.setLevel(DEBUG) 但 DEBUG 日志不显示
#    原因：Handler 也有级别，两者都要满足才会输出
#    解决：同时设置 Logger 和 Handler 的级别
#
# 4. 日志文件编码错误
#    问题：日志文件中中文显示为乱码
#    原因：FileHandler 默认使用系统编码（Windows 可能是 GBK）
#    解决：明确指定 encoding='utf-8'
#
# 5. 日志文件被锁定无法删除
#    问题：程序运行时无法删除或移动日志文件
#    原因：FileHandler 持有文件句柄
#    解决：关闭 Handler：handler.close() 或 logger.removeHandler(handler)
#
# 6. 在多进程中日志混乱
#    问题：多个进程同时写日志导致内容交错
#    原因：FileHandler 不是进程安全的
#    解决：使用 QueueHandler + QueueListener 或每个进程独立日志文件
#
# 7. traceback 信息丢失
#    问题：logger.error() 没有记录异常堆栈
#    原因：忘记传入 exc_info=True
#    解决：使用 logger.exception() 或 logger.error(..., exc_info=True)
#
# 8. pdb 调试时修改变量无效
#    问题：在 pdb 中修改变量后程序行为没变
#    原因：直接输入变量名会被当作 pdb 命令
#    解决：使用 !variable = value 语法（感叹号开头）
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   编写函数 setup_dual_logger(name, log_file)，创建并返回一个同时输出到
#   控制台和文件的 Logger：
#   - 控制台只显示 WARNING 及以上级别
#   - 文件记录 DEBUG 及以上级别
#   - 格式：时间 [级别] 消息（时间格式 %Y-%m-%d %H:%M:%S）
#   返回格式：logging.Logger 实例
#   例如：
#     logger = setup_dual_logger('myapp', 'app.log')
#     logger.debug('debug 消息')    # 只写文件
#     logger.warning('警告消息')    # 控制台 + 文件
#     # 控制台输出：2024-03-15 10:23:45 [WARNING] 警告消息
#
# 练习 2（基础）：
#   编写函数 setup_rotating_logger(name, log_file, max_bytes=1048576, backup_count=3)，
#   配置并返回一个按大小轮转的日志系统：
#   - 单个文件最大 max_bytes 字节
#   - 保留最近 backup_count 个文件
#   - 格式：时间 [级别] 消息
#   返回格式：logging.Logger 实例
#   例如：
#     logger = setup_rotating_logger('app', 'app.log', max_bytes=500)
#     for i in range(100):
#         logger.info(f'第 {i+1} 条日志')
#     # 生成 app.log、app.log.1、app.log.2、app.log.3
#
# 练习 3（进阶）：
#   编写函数 safe_divide(a, b)，执行除法并记录异常信息：
#   - 捕获 ZeroDivisionError 和 TypeError
#   - 使用 traceback.format_exc() 获取完整堆栈并记录到日志
#   - 正常时返回结果，失败时返回 None
#   返回格式：计算结果（int/float）或 None
#   例如：
#     safe_divide(10, 2)
#     # 5.0
#     safe_divide(10, 0)
#     # None（并记录 ZeroDivisionError 堆栈到日志）
#     safe_divide('a', 2)
#     # None（并记录 TypeError 堆栈到日志）
#
# 练习 4（进阶）：
#   编写三个函数模拟多模块日志传播：
#   - db_query(sql)：Logger 名为 'myapp.database'，记录查询日志
#   - api_request(path)：Logger 名为 'myapp.api'，记录请求日志
#   - demo_module_logging()：配置根 Logger 'myapp'，调用以上函数演示传播
#   返回格式：None（控制台输出演示）
#   例如：
#     demo_module_logging()
#     # [myapp.database] 执行查询: SELECT * FROM users
#     # [myapp.api]      处理请求: /api/users
#
# 练习答案提示：
#   练习1：用 logging.StreamHandler 配置控制台，用 logging.FileHandler 配置文件
#   练习2：用 logging.handlers.RotatingFileHandler，设置 maxBytes 和 backupCount
#   练习3：在 except 块中用 traceback.format_exc() 获取堆栈，logger.error() 记录
#   练习4：用 logging.getLogger('myapp.database') 等创建子 Logger，propagate=True


# =============================================================================
# 【练习答案】
# =============================================================================


def setup_dual_logger(name: str, log_file: str) -> logging.Logger:
    """
    练习 1：创建同时输出到控制台和文件的 Logger。

    Args:
        name:     Logger 名称
        log_file: 日志文件路径

    Returns:
        配置好的 Logger 实例（控制台 WARNING+，文件 DEBUG+）

    Example:
        logger = setup_dual_logger('myapp', 'app.log')
        logger.debug('debug 消息')    # 只写文件
        logger.warning('警告消息')    # 控制台 + 文件
    """
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)
    logger.handlers.clear()
    logger.propagate = False

    fmt = logging.Formatter(
        '%(asctime)s [%(levelname)s] %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S',
    )

    # 控制台处理器：WARNING 及以上
    console_handler = logging.StreamHandler(sys.stdout)
    console_handler.setLevel(logging.WARNING)
    console_handler.setFormatter(fmt)

    # 文件处理器：DEBUG 及以上
    file_handler = logging.FileHandler(log_file, encoding='utf-8')
    file_handler.setLevel(logging.DEBUG)
    file_handler.setFormatter(fmt)

    logger.addHandler(console_handler)
    logger.addHandler(file_handler)
    return logger


def setup_rotating_logger(
    name: str,
    log_file: str,
    max_bytes: int = 1024 * 1024,
    backup_count: int = 3,
) -> logging.Logger:
    """
    练习 2：配置并返回按大小轮转的日志系统。

    Args:
        name:         Logger 名称
        log_file:     日志文件路径
        max_bytes:    单个文件最大字节数（默认 1MB）
        backup_count: 保留的旧文件数量（默认 3）

    Returns:
        配置好的 Logger 实例

    Example:
        logger = setup_rotating_logger('app', 'app.log', max_bytes=500)
        for i in range(100):
            logger.info(f'第 {i+1} 条日志')
        # 生成 app.log、app.log.1、app.log.2、app.log.3
    """
    logger = logging.getLogger(name)
    logger.setLevel(logging.DEBUG)
    logger.handlers.clear()
    logger.propagate = False

    handler = logging.handlers.RotatingFileHandler(
        filename=log_file,
        maxBytes=max_bytes,
        backupCount=backup_count,
        encoding='utf-8',
    )
    handler.setFormatter(
        logging.Formatter(
            '%(asctime)s [%(levelname)s] %(message)s',
            datefmt='%Y-%m-%d %H:%M:%S',
        )
    )
    logger.addHandler(handler)
    return logger


# ── 练习 3 辅助：模块级 Logger（使用 NullHandler 作为默认）────────────────────

_sd_logger = logging.getLogger('ex3.safe_divide')
_sd_logger.addHandler(logging.NullHandler())


def safe_divide(a, b):
    """
    练习 3：执行除法并用 traceback 记录异常信息。

    Args:
        a: 被除数
        b: 除数

    Returns:
        计算结果，失败时返回 None

    Example:
        safe_divide(10, 2)
        # 5.0
        safe_divide(10, 0)
        # None（并记录 ZeroDivisionError 堆栈到日志）
        safe_divide('a', 2)
        # None（并记录 TypeError 堆栈到日志）
    """
    try:
        return a / b
    except ZeroDivisionError:
        _sd_logger.error("除零错误:\n%s", traceback.format_exc())
        return None
    except TypeError:
        _sd_logger.error("类型错误:\n%s", traceback.format_exc())
        return None


# ── 练习 4 辅助：模块化日志传播 ─────────────────────────────────────────────

_ex4_db_logger  = logging.getLogger('ex4.myapp.database')
_ex4_api_logger = logging.getLogger('ex4.myapp.api')


def db_query(sql: str) -> None:
    """模拟数据库查询（Logger 名：'ex4.myapp.database'）。"""
    _ex4_db_logger.info("执行查询: %s", sql)


def api_request(path: str) -> None:
    """模拟 API 请求处理（Logger 名：'ex4.myapp.api'）。"""
    _ex4_api_logger.info("处理请求: %s", path)


def demo_module_logging() -> None:
    """
    练习 4：演示 Logger 层次结构与日志传播机制。

    配置根 Logger 'ex4.myapp'，调用 db_query 和 api_request 演示
    子 Logger 的日志传播。

    Example:
        demo_module_logging()
        # [ex4.myapp.database] 执行查询: SELECT * FROM users
        # [ex4.myapp.api]      处理请求: /api/users
    """
    # 配置根 Logger
    root_logger = logging.getLogger('ex4.myapp')
    root_logger.setLevel(logging.DEBUG)
    root_logger.handlers.clear()
    root_logger.propagate = False

    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter(
        logging.Formatter('[%(name)s] %(message)s')
    )
    root_logger.addHandler(handler)

    # 调用子模块函数，日志会传播到根 Logger
    db_query("SELECT * FROM users")
    api_request("/api/users")

    # 清理
    for h in root_logger.handlers[:]:
        h.close()
        root_logger.removeHandler(h)


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 setup_dual_logger 函数。"""
    print("setup_dual_logger 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    with tempfile.NamedTemporaryFile(
        mode='w', suffix='.log', delete=False, prefix='ex1_'
    ) as tmp:
        tmp_path = tmp.name

    try:
        logger = setup_dual_logger('ex1_app', tmp_path)
        print(f"  Logger 名称: '{logger.name}'（Handler 数量: {len(logger.handlers)}）")
        print()
        print("  写入各级别日志（控制台只显示 WARNING+）：")
        logger.debug('DEBUG: 正在检查配置文件...')    # 只写文件
        logger.info('INFO: 服务器启动成功')           # 只写文件
        logger.warning('WARNING: 缓存空间不足 20%')   # 控制台 + 文件
        logger.error('ERROR: 数据库连接超时')         # 控制台 + 文件
        print()
        print("  文件内容（包含全部级别）：")
        with open(tmp_path, encoding='utf-8') as f:
            for line in f:
                print(f"    {line.rstrip()}")

        for h in logger.handlers[:]:
            h.close()
            logger.removeHandler(h)
    finally:
        try:
            os.unlink(tmp_path)
        except OSError:
            pass


def exercise2_answer() -> None:
    """练习 2：演示 setup_rotating_logger 函数。"""
    print("setup_rotating_logger 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    with tempfile.TemporaryDirectory() as tmp_dir:
        log_path = os.path.join(tmp_dir, 'ex2_app.log')
        logger = setup_rotating_logger(
            'ex2_app', log_path, max_bytes=300, backup_count=2
        )
        print(f"  日志文件: {Path(log_path).name}")
        print(f"  maxBytes=300, backupCount=2")
        print()
        print("  写入 15 条日志（触发多次轮转）...")
        for i in range(15):
            logger.info(f'第 {i+1:02d} 条日志消息，用于测试轮转功能')

        for h in logger.handlers[:]:
            h.close()
            logger.removeHandler(h)

        files = sorted([f for f in os.listdir(tmp_dir) if f.startswith('ex2_app')])
        print()
        print("  生成的文件：")
        for fname in files:
            fpath = os.path.join(tmp_dir, fname)
            size = os.path.getsize(fpath)
            print(f"    {fname:<30} ({size} 字节)")


def exercise3_answer() -> None:
    """练习 3：演示 safe_divide 函数。"""
    print("safe_divide 演示:")

    # ── 配置 logger 以便看到输出 ─────────────────────────
    _sd_logger.handlers.clear()
    _sd_logger.setLevel(logging.DEBUG)
    handler = logging.StreamHandler(sys.stdout)
    handler.setFormatter(logging.Formatter('  [%(levelname)s] %(message)s'))
    _sd_logger.addHandler(handler)

    # ── 基本示例 ─────────────────────────────────────────
    test_cases = [
        (10,   2,   10.0 / 2),
        (10,   0,   None),
        ('a',  2,   None),
        (7.5,  2.5, 7.5 / 2.5),
        (10,   'b', None),
    ]

    print(f"  {'参数 a':<8} {'参数 b':<8} {'结果'}")
    print("  " + "-" * 35)
    for a, b, expected in test_cases:
        result = safe_divide(a, b)
        status = '✓' if result == expected else '✗'
        print(f"  {status} {str(a):<8} {str(b):<8} → {result}")

    for h in _sd_logger.handlers[:]:
        h.close()
        _sd_logger.removeHandler(h)


def exercise4_answer() -> None:
    """练习 4：演示 demo_module_logging 函数。"""
    print("demo_module_logging 演示:")
    print()
    print("  Logger 层次结构：")
    print("    ex4.myapp （根 Logger，配置了 Handler）")
    print("    ├── ex4.myapp.database （子 Logger，无 Handler，依靠传播）")
    print("    └── ex4.myapp.api      （子 Logger，无 Handler，依靠传播）")
    print()
    print("  日志输出（所有子 Logger 消息传播到根 Logger 处理）：")
    demo_module_logging()


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 40)
#     exercise1_answer()
#
#     print("\n" + "=" * 40)
#     exercise2_answer()
#
#     print("\n" + "=" * 40)
#     exercise3_answer()
#
#     print("\n" + "=" * 40)
#     exercise4_answer()


# =============================================================================
# 【进阶话题】
# =============================================================================
#
# 1. 异步日志（避免 I/O 阻塞主线程）
#    - 使用 QueueHandler + QueueListener
#    - 日志写入队列，后台线程异步处理
#
# 2. 分布式日志收集
#    - 使用 SysLogHandler 发送到 syslog 服务器
#    - 使用 SocketHandler 发送到日志收集系统（如 ELK）
#    - 集成第三方服务（Sentry、Datadog）
#
# 3. 日志采样（高并发场景）
#    - 只记录部分日志（如 1%）降低 I/O 压力
#    - 使用自定义 Filter 实现采样逻辑
#
# 4. 日志脱敏
#    - 自动过滤敏感信息（密码、身份证号、手机号）
#    - 使用正则表达式替换敏感字段
#
# 5. 性能监控日志
#    - 记录函数执行时间
#    - 使用装饰器自动记录慢查询
#
# 6. 日志分析工具
#    - 使用 grep/awk 分析日志文件
#    - 使用 Python 脚本统计错误频率
#    - 集成 ELK（Elasticsearch + Logstash + Kibana）
#
# 7. 调试技巧
#    - 使用 logging.DEBUG 替代 print() 调试
#    - 条件断点：pdb.set_trace() if condition else None
#    - 远程调试：使用 rpdb 或 IDE 的远程调试功能
#
# 8. 日志最佳实践
#    - 使用 __name__ 作为 Logger 名称
#    - 库代码使用 NullHandler
#    - 避免在循环中频繁记录日志
#    - 使用 lazy formatting：logger.info("User %s", user_id)
#    - 生产环境关闭 DEBUG 日志
#    - 定期清理旧日志文件
