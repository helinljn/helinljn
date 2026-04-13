#!/usr/bin/env python3
"""
文件行尾符转换脚本
"""

import os
import sys
import argparse
from pathlib import Path

def to_unix(content: bytes) -> bytes:
    """将Windows行尾符(CRLF)转换为Unix行尾符(LF)"""
    # 将\r\n替换为\n，同时处理可能存在的单个\r（旧Mac风格）
    content = content.replace(b'\r\n', b'\n')
    content = content.replace(b'\r', b'\n')
    return content

def to_dos(content: bytes) -> bytes:
    """将Unix行尾符(LF)转换为Windows行尾符(CRLF)"""
    # 将单独的LF（前面不是CR）替换为CRLF，避免重复转换
    # 使用正则表达式替换不是CRLF的LF
    import re
    # 匹配前面不是\r的\n，将其替换为\r\n
    # 使用负向后顾断言 (?<!\r) 来匹配前面不是\r的\n
    pattern = re.compile(b'(?<!\r)\n')
    result = pattern.sub(b'\r\n', content)
    return result

def convert_file(filepath: Path, to_type: str) -> bool:
    """
    转换单个文件的行尾符

    Args:
        filepath: 文件路径
        to_type: 转换类型，'unix' 或 'dos'

    Returns:
        bool: 转换是否成功
    """
    try:
        # 读取文件内容（二进制模式）
        with open(filepath, 'rb') as f:
            content = f.read()

        # 根据转换类型处理内容
        if to_type == 'unix':
            new_content = to_unix(content)
        elif to_type == 'dos':
            new_content = to_dos(content)
        else:
            print(f"错误：未知的转换类型 {to_type}")
            return False

        # 如果内容有变化，则写回文件
        if new_content != content:
            with open(filepath, 'wb') as f:
                f.write(new_content)
            return True
        return False
    except Exception as e:
        print(f"转换文件 {filepath} 时出错: {e}")
        return False

def get_conversion_type(ext: str) -> str:
    """
    根据文件扩展名确定转换类型

    Args:
        ext: 文件扩展名（小写，包含点号）

    Returns:
        str: 'unix' 或 'dos'，如果不需要转换则返回None
    """
    # Unix转换（CRLF -> LF）
    unix_extensions = {'.py', '.po'}

    # DOS转换（LF -> CRLF）
    dos_extensions = {'.md', '.sql', '.html', '.json'}

    if ext in unix_extensions:
        return 'unix'
    elif ext in dos_extensions:
        return 'dos'
    else:
        return None

def convert_directory(root_dir: Path, verbose: bool = True, dry_run: bool = False) -> dict:
    """
    递归转换目录下所有匹配的文件

    Args:
        root_dir: 根目录路径
        verbose: 是否显示详细信息
        dry_run: 试运行模式，只显示将要转换的文件，不实际修改

    Returns:
        dict: 转换统计信息
    """
    stats = {
        'total_files': 0,
        'converted_files': 0,
        'skipped_files': 0,
        'error_files': 0,
        'by_type': {'unix': 0, 'dos': 0}
    }

    for dirpath, dirnames, filenames in os.walk(root_dir):
        # 跳过.git目录（可选）
        if '.git' in dirnames:
            dirnames.remove('.git')

        for filename in filenames:
            filepath = Path(dirpath) / filename
            ext = filepath.suffix.lower()

            # 获取转换类型
            conv_type = get_conversion_type(ext)
            if conv_type is None:
                continue

            stats['total_files'] += 1

            if verbose:
                type_desc = "Unix (CRLF->LF)" if conv_type == 'unix' else "DOS (LF->CRLF)"
                print(f"发现: {filepath.relative_to(root_dir)} [{type_desc}]")

            if dry_run:
                stats['converted_files'] += 1
                stats['by_type'][conv_type] += 1
                continue

            # 实际转换文件
            try:
                with open(filepath, 'rb') as f:
                    original_content = f.read()

                if conv_type == 'unix':
                    new_content = to_unix(original_content)
                else:  # conv_type == 'dos'
                    new_content = to_dos(original_content)

                if new_content != original_content:
                    # 备份原文件（可选）
                    # backup_path = filepath.with_suffix(filepath.suffix + '.bak')
                    # with open(backup_path, 'wb') as f:
                    #     f.write(original_content)

                    # 写入新内容
                    with open(filepath, 'wb') as f:
                        f.write(new_content)

                    stats['converted_files'] += 1
                    stats['by_type'][conv_type] += 1

                    if verbose:
                        print(f"  已转换: {filepath.relative_to(root_dir)}")
                else:
                    stats['skipped_files'] += 1
                    if verbose:
                        print(f"  跳过（无需转换）: {filepath.relative_to(root_dir)}")

            except Exception as e:
                stats['error_files'] += 1
                print(f"  错误: {filepath.relative_to(root_dir)} - {e}")

    return stats

def main():
    """主函数"""
    parser = argparse.ArgumentParser(
        description='文件行尾符转换工具（替代convert.sh）',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
使用示例：
  %(prog)s                    # 转换当前目录（默认）
  %(prog)s /path/to/dir       # 转换指定目录
  %(prog)s -v                 # 显示详细信息
  %(prog)s --dry-run          # 试运行，不实际修改文件
  %(prog)s -q                 # 安静模式，只显示统计信息
  %(prog)s --help             # 显示帮助信息
        """
    )

    parser.add_argument('directory', nargs='?', default='.',
                       help='要转换的目录（默认：当前目录）')
    parser.add_argument('-v', '--verbose', action='store_true',
                       help='显示详细信息')
    parser.add_argument('-q', '--quiet', action='store_true',
                       help='安静模式，只显示错误信息')
    parser.add_argument('--dry-run', action='store_true',
                       help='试运行，只显示将要转换的文件，不实际修改')
    parser.add_argument('--skip-git', action='store_true',
                       help='跳过.git目录')

    args = parser.parse_args()

    # 设置详细程度
    verbose = args.verbose
    if args.quiet:
        verbose = False

    # 检查目录是否存在
    root_dir = Path(args.directory).resolve()
    if not root_dir.exists():
        print(f"错误：目录不存在 {root_dir}")
        sys.exit(1)

    if not root_dir.is_dir():
        print(f"错误：{root_dir} 不是目录")
        sys.exit(1)

    # 显示开始信息
    if verbose:
        print(f"开始转换目录: {root_dir}")
        if args.dry_run:
            print("试运行模式，不会修改文件")
        print("-" * 50)

    # 执行转换
    stats = convert_directory(
        root_dir,
        verbose=verbose,
        dry_run=args.dry_run
    )

    # 显示统计信息
    print("\n" + "=" * 50)
    print("转换完成！")
    print("=" * 50)
    print(f"扫描文件总数: {stats['total_files']}")
    print(f"成功转换文件: {stats['converted_files']}")
    if stats['by_type']['unix'] > 0:
        print(f"  - Unix转换 (CRLF->LF): {stats['by_type']['unix']}")
    if stats['by_type']['dos'] > 0:
        print(f"  - DOS转换 (LF->CRLF): {stats['by_type']['dos']}")
    print(f"跳过文件: {stats['skipped_files']}")
    print(f"错误文件: {stats['error_files']}")

    if args.dry_run:
        print("\n注意：本次为试运行，未实际修改文件。")
        print("使用 --dry-run 参数查看将要转换的文件。")

    if stats['error_files'] > 0:
        sys.exit(1)

if __name__ == '__main__':
    main()
