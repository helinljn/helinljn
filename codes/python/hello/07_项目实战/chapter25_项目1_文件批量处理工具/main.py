"""
文件批量处理工具 - 主程序入口

这是一个功能完整的命令行工具，整合了前面学习的多个知识点：
- argparse 命令行参数解析
- logging 日志系统
- pathlib 文件操作
- 面向对象设计
- 异常处理
- 配置管理

使用示例：
    python main.py search --pattern "*.txt" --path ./docs
    python main.py rename --pattern "*.jpg" --prefix "photo_" --path ./images
    python main.py replace --pattern "*.py" --find "old" --replace "new" --path ./src
    python main.py duplicates --path ./downloads

作者：Python 学习者
版本：1.0.0
"""

import argparse
import sys
from pathlib import Path
from typing import Optional

# 导入项目模块
from config import Config, setup_logging, generate_default_config
from file_processor import FileProcessor


# ============================================================
# 命令行参数解析
# ============================================================

def create_parser() -> argparse.ArgumentParser:
    """
    创建命令行参数解析器

    Returns:
        配置好的 ArgumentParser 实例
    """
    parser = argparse.ArgumentParser(
        prog="file_processor",
        description="文件批量处理工具 - 搜索、重命名、内容替换、重复检测",
        epilog="示例：python main.py search --pattern '*.txt' --path ./docs",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    # 全局参数
    parser.add_argument(
        "--config",
        type=str,
        default="config.ini",
        help="配置文件路径（默认：config.ini）",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="演练模式（不实际修改文件，仅显示将要执行的操作）",
    )
    parser.add_argument(
        "--no-backup",
        action="store_true",
        help="禁用备份（重命名/替换前不备份原文件）",
    )
    parser.add_argument(
        "--log-level",
        choices=["DEBUG", "INFO", "WARNING", "ERROR"],
        help="日志级别（覆盖配置文件）",
    )
    parser.add_argument(
        "--encoding",
        type=str,
        default="utf-8",
        help="文本文件编码（默认：utf-8）",
    )

    # 子命令
    subparsers = parser.add_subparsers(
        dest="command",
        title="可用命令",
        description="选择要执行的操作",
        help="使用 'python main.py <命令> --help' 查看详细帮助",
    )

    # ---- 子命令 1：search（搜索文件）----
    search_parser = subparsers.add_parser(
        "search",
        help="搜索符合条件的文件",
        description="在指定目录中搜索符合条件的文件",
    )
    search_parser.add_argument(
        "--path",
        type=str,
        required=True,
        help="搜索根目录",
    )
    search_parser.add_argument(
        "--pattern",
        type=str,
        default="*",
        help="文件名通配符模式（默认：*，匹配所有文件）",
    )
    search_parser.add_argument(
        "--recursive",
        action="store_true",
        default=True,
        help="递归搜索子目录（默认启用）",
    )
    search_parser.add_argument(
        "--no-recursive",
        action="store_true",
        help="不递归搜索子目录",
    )
    search_parser.add_argument(
        "--min-size",
        type=int,
        help="最小文件大小（字节）",
    )
    search_parser.add_argument(
        "--max-size",
        type=int,
        help="最大文件大小（字节）",
    )

    # ---- 子命令 2：rename（批量重命名）----
    rename_parser = subparsers.add_parser(
        "rename",
        help="批量重命名文件",
        description="对匹配的文件执行批量重命名操作",
    )
    rename_parser.add_argument(
        "--path",
        type=str,
        required=True,
        help="目标目录",
    )
    rename_parser.add_argument(
        "--pattern",
        type=str,
        default="*",
        help="文件名通配符模式",
    )
    rename_parser.add_argument(
        "--recursive",
        action="store_true",
        default=True,
        help="递归处理子目录",
    )

    # 重命名策略（互斥）
    rename_group = rename_parser.add_mutually_exclusive_group(required=True)
    rename_group.add_argument(
        "--prefix",
        type=str,
        help="添加前缀",
    )
    rename_group.add_argument(
        "--suffix",
        type=str,
        help="添加后缀（扩展名前）",
    )
    rename_group.add_argument(
        "--replace",
        nargs=2,
        metavar=("OLD", "NEW"),
        help="替换文件名中的文本：--replace OLD NEW",
    )
    rename_group.add_argument(
        "--numbered",
        nargs="?",
        const="file",
        metavar="BASE_NAME",
        help="序号重命名：--numbered [基础名称]",
    )

    rename_parser.add_argument(
        "--regex",
        action="store_true",
        help="使用正则表达式（配合 --replace 使用）",
    )

    # ---- 子命令 3：replace（内容替换）----
    replace_parser = subparsers.add_parser(
        "replace",
        help="批量替换文件内容",
        description="在匹配的文本文件中批量替换内容",
    )
    replace_parser.add_argument(
        "--path",
        type=str,
        required=True,
        help="目标目录",
    )
    replace_parser.add_argument(
        "--pattern",
        type=str,
        default="*",
        help="文件名通配符模式",
    )
    replace_parser.add_argument(
        "--find",
        type=str,
        required=True,
        help="要查找的文本或正则表达式",
    )
    replace_parser.add_argument(
        "--replace",
        type=str,
        required=True,
        help="替换为的文本",
    )
    replace_parser.add_argument(
        "--regex",
        action="store_true",
        help="使用正则表达式",
    )
    replace_parser.add_argument(
        "--ignore-case",
        action="store_true",
        help="忽略大小写",
    )
    replace_parser.add_argument(
        "--recursive",
        action="store_true",
        default=True,
        help="递归处理子目录",
    )

    # ---- 子命令 4：duplicates（重复文件检测）----
    dup_parser = subparsers.add_parser(
        "duplicates",
        help="检测重复文件",
        description="基于 MD5 哈希检测内容完全相同的文件",
    )
    dup_parser.add_argument(
        "--path",
        type=str,
        required=True,
        help="搜索目录",
    )
    dup_parser.add_argument(
        "--pattern",
        type=str,
        default="*",
        help="文件名通配符模式",
    )
    dup_parser.add_argument(
        "--recursive",
        action="store_true",
        default=True,
        help="递归搜索子目录",
    )

    # ---- 子命令 5：init（生成默认配置文件）----
    init_parser = subparsers.add_parser(
        "init",
        help="生成默认配置文件",
        description="在当前目录生成 config.ini 配置文件模板",
    )
    init_parser.add_argument(
        "--output",
        type=str,
        default="config.ini",
        help="输出文件路径（默认：config.ini）",
    )

    return parser


# ============================================================
# 命令处理函数
# ============================================================

def cmd_search(args: argparse.Namespace, processor: FileProcessor) -> int:
    """
    处理 search 命令

    Args:
        args:      解析后的命令行参数
        processor: FileProcessor 实例

    Returns:
        退出码（0 表示成功）
    """
    root = Path(args.path)
    if not root.exists():
        print(f"错误：目录不存在：{root}")
        return 1

    recursive = args.recursive and not args.no_recursive

    print(f"\n搜索文件：{root}")
    print(f"  模式：{args.pattern}")
    print(f"  递归：{'是' if recursive else '否'}")
    if args.min_size:
        print(f"  最小大小：{args.min_size} 字节")
    if args.max_size:
        print(f"  最大大小：{args.max_size} 字节")
    print()

    files = processor.search(
        root=root,
        pattern=args.pattern,
        recursive=recursive,
        min_size=args.min_size,
        max_size=args.max_size,
        print_results=True,
    )

    return 0


def cmd_rename(args: argparse.Namespace, processor: FileProcessor) -> int:
    """处理 rename 命令"""
    root = Path(args.path)
    if not root.exists():
        print(f"错误：目录不存在：{root}")
        return 1

    print(f"\n批量重命名：{root}")
    print(f"  模式：{args.pattern}")
    print(f"  递归：{'是' if args.recursive else '否'}")

    # 搜索文件
    files = processor.search(
        root=root,
        pattern=args.pattern,
        recursive=args.recursive,
        print_results=False,
    )

    if not files:
        print("  未找到匹配的文件。")
        return 0

    print(f"  找到 {len(files)} 个文件\n")

    # 执行重命名
    result = None
    if args.prefix:
        print(f"  策略：添加前缀 '{args.prefix}'")
        result = processor.rename_add_prefix(files, args.prefix)
    elif args.suffix:
        print(f"  策略：添加后缀 '{args.suffix}'")
        result = processor.rename_add_suffix(files, args.suffix)
    elif args.replace:
        old, new = args.replace
        print(f"  策略：替换 '{old}' -> '{new}'  正则：{args.regex}")
        result = processor.rename_replace(files, old, new, use_regex=args.regex)
    elif args.numbered:
        base_name = args.numbered
        print(f"  策略：序号重命名（基础名：{base_name}）")
        result = processor.rename_numbered(files, base_name=base_name)

    if result:
        print()
        result.print_details(show_all=True)
        print(f"\n{result.summary}\n")

    return 0 if result and result.failed == 0 else 1


def cmd_replace(args: argparse.Namespace, processor: FileProcessor) -> int:
    """处理 replace 命令"""
    import re

    root = Path(args.path)
    if not root.exists():
        print(f"错误：目录不存在：{root}")
        return 1

    print(f"\n批量内容替换：{root}")
    print(f"  模式：{args.pattern}")
    print(f"  查找：{args.find}")
    print(f"  替换：{args.replace}")
    print(f"  正则：{args.regex}")
    print(f"  忽略大小写：{args.ignore_case}")

    # 搜索文件
    files = processor.search(
        root=root,
        pattern=args.pattern,
        recursive=args.recursive,
        print_results=False,
    )

    if not files:
        print("  未找到匹配的文件。")
        return 0

    print(f"  找到 {len(files)} 个文件\n")

    # 执行替换
    flags = re.IGNORECASE if args.ignore_case else 0
    result = processor.replace_content(
        files,
        old=args.find,
        new=args.replace,
        use_regex=args.regex,
        flags=flags,
    )

    print()
    result.print_details(show_all=True)
    print(f"\n{result.summary}\n")

    return 0 if result.failed == 0 else 1


def cmd_duplicates(args: argparse.Namespace, processor: FileProcessor) -> int:
    """处理 duplicates 命令"""
    root = Path(args.path)
    if not root.exists():
        print(f"错误：目录不存在：{root}")
        return 1

    print(f"\n检测重复文件：{root}")
    print(f"  模式：{args.pattern}")
    print(f"  递归：{'是' if args.recursive else '否'}\n")

    # 搜索文件
    files = processor.search(
        root=root,
        pattern=args.pattern,
        recursive=args.recursive,
        print_results=False,
    )

    if not files:
        print("  未找到匹配的文件。")
        return 0

    print(f"  正在分析 {len(files)} 个文件...\n")

    # 检测重复
    processor.find_duplicates(files, base=root)

    return 0


def cmd_init(args: argparse.Namespace) -> int:
    """处理 init 命令"""
    output = args.output
    if Path(output).exists():
        response = input(f"文件 {output} 已存在，是否覆盖？(y/N): ")
        if response.lower() not in ("y", "yes"):
            print("已取消。")
            return 0

    generate_default_config(output)
    return 0


# ============================================================
# 主函数
# ============================================================

def main() -> int:
    """
    主函数

    Returns:
        退出码（0 表示成功）
    """
    # 解析命令行参数
    parser = create_parser()
    args = parser.parse_args()

    # 若未指定命令，显示帮助
    if not args.command:
        parser.print_help()
        return 0

    # init 命令不需要配置和日志
    if args.command == "init":
        return cmd_init(args)

    # 加载配置
    config = Config(args.config if Path(args.config).exists() else None)

    # 应用命令行参数覆盖
    if args.dry_run:
        config.set_override("dry_run", "true")
    if args.no_backup:
        config.set_override("backup_enabled", "false")
    if args.log_level:
        config.set_override("log_level", args.log_level)
    if args.encoding:
        config.set_override("encoding", args.encoding)

    # 初始化日志
    logger = setup_logging(config)

    # 创建文件处理器
    processor = FileProcessor(
        dry_run=config.get_bool("dry_run"),
        backup=config.get_bool("backup_enabled"),
        backup_suffix=config.get("backup_suffix"),
        encoding=config.get("encoding"),
        recursive=config.get_bool("recursive"),
        ignore_hidden=config.get_bool("ignore_hidden"),
    )

    # 分发到具体命令处理函数
    try:
        if args.command == "search":
            return cmd_search(args, processor)
        elif args.command == "rename":
            return cmd_rename(args, processor)
        elif args.command == "replace":
            return cmd_replace(args, processor)
        elif args.command == "duplicates":
            return cmd_duplicates(args, processor)
        else:
            print(f"错误：未知命令 '{args.command}'")
            return 1
    except KeyboardInterrupt:
        print("\n\n操作已取消。")
        return 130
    except Exception as e:
        logger.exception("发生未预期的错误")
        print(f"\n错误：{e}")
        return 1


# ============================================================
# 程序入口
# ============================================================

if __name__ == "__main__":
    sys.exit(main())
