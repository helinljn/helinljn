"""
工具函数模块
提供文件批量处理工具所需的通用辅助函数

知识点：
- pathlib 模块的使用
- fnmatch 文件名匹配
- 格式化输出
- 类型注解
- 时间格式化
"""

import fnmatch
import hashlib
import os
import shutil
import time
from datetime import datetime
from pathlib import Path
from typing import List, Optional, Tuple


# ============================================================
# 文件匹配与过滤
# ============================================================

def match_pattern(filename: str, pattern: str) -> bool:
    """
    检查文件名是否匹配指定的通配符模式

    支持的通配符：
    - * ：匹配任意字符（不含路径分隔符）
    - ? ：匹配单个字符
    - [seq] ：匹配括号内任意字符

    Args:
        filename: 文件名（不含路径）
        pattern:  通配符模式，如 "*.txt"、"file_?.py"

    Returns:
        匹配返回 True，否则 False

    示例：
        >>> match_pattern("report.txt", "*.txt")
        True
        >>> match_pattern("image.jpg", "*.txt")
        False
    """
    return fnmatch.fnmatch(filename.lower(), pattern.lower())


def is_hidden(path: Path) -> bool:
    """
    判断文件或目录是否为隐藏文件

    Windows：通过文件属性判断
    Unix/macOS：以 "." 开头

    Args:
        path: 文件或目录路径

    Returns:
        是隐藏文件返回 True
    """
    # Unix/macOS：以 "." 开头
    if path.name.startswith("."):
        return True

    # Windows：检查隐藏属性
    try:
        import ctypes
        attrs = ctypes.windll.kernel32.GetFileAttributesW(str(path))
        if attrs != -1 and (attrs & 2):   # FILE_ATTRIBUTE_HIDDEN = 0x2
            return True
    except (AttributeError, OSError):
        pass

    return False


def collect_files(
    root: Path,
    pattern: str = "*",
    recursive: bool = True,
    ignore_hidden: bool = True,
    min_size: Optional[int] = None,
    max_size: Optional[int] = None,
    modified_after: Optional[datetime] = None,
    modified_before: Optional[datetime] = None,
) -> List[Path]:
    """
    在指定目录下收集满足条件的文件列表

    Args:
        root:            搜索根目录
        pattern:         文件名通配符，如 "*.txt"
        recursive:       是否递归搜索子目录
        ignore_hidden:   是否忽略隐藏文件/目录
        min_size:        最小文件大小（字节），None 表示不限
        max_size:        最大文件大小（字节），None 表示不限
        modified_after:  修改时间下限，None 表示不限
        modified_before: 修改时间上限，None 表示不限

    Returns:
        满足条件的文件路径列表（已排序）

    示例：
        files = collect_files(Path("./docs"), "*.md", recursive=True)
    """
    if not root.exists():
        raise FileNotFoundError(f"目录不存在：{root}")
    if not root.is_dir():
        raise NotADirectoryError(f"路径不是目录：{root}")

    results: List[Path] = []

    # 选择遍历方式
    iterator = root.rglob("*") if recursive else root.glob("*")

    for item in iterator:
        # 只处理文件
        if not item.is_file():
            continue

        # 忽略隐藏文件（包括路径中的隐藏目录）
        if ignore_hidden:
            # 检查路径中的每一个部分
            if any(is_hidden(Path(part)) for part in item.parts):
                continue

        # 文件名模式匹配
        if not match_pattern(item.name, pattern):
            continue

        # 文件大小过滤
        try:
            size = item.stat().st_size
            if min_size is not None and size < min_size:
                continue
            if max_size is not None and size > max_size:
                continue

            # 修改时间过滤
            mtime = datetime.fromtimestamp(item.stat().st_mtime)
            if modified_after is not None and mtime < modified_after:
                continue
            if modified_before is not None and mtime > modified_before:
                continue
        except OSError:
            continue

        results.append(item)

    return sorted(results)


# ============================================================
# 文件备份
# ============================================================

def backup_file(path: Path, suffix: str = ".bak") -> Path:
    """
    创建文件的备份副本

    备份策略：若 file.txt.bak 已存在，则尝试 file.txt.bak1、.bak2……

    Args:
        path:   要备份的文件路径
        suffix: 备份文件后缀，默认 ".bak"

    Returns:
        备份文件的路径

    Raises:
        FileNotFoundError: 源文件不存在
    """
    if not path.exists():
        raise FileNotFoundError(f"源文件不存在：{path}")

    backup = path.with_suffix(path.suffix + suffix)

    # 若备份文件已存在，则追加编号
    if backup.exists():
        counter = 1
        while True:
            backup = path.with_suffix(f"{path.suffix}{suffix}{counter}")
            if not backup.exists():
                break
            counter += 1

    shutil.copy2(str(path), str(backup))
    return backup


# ============================================================
# 格式化辅助
# ============================================================

def format_size(size_bytes: int) -> str:
    """
    将字节数格式化为人类可读的大小字符串

    Args:
        size_bytes: 文件大小（字节）

    Returns:
        格式化后的字符串，如 "1.23 MB"

    示例：
        >>> format_size(1048576)
        '1.00 MB'
        >>> format_size(1500)
        '1.46 KB'
    """
    if size_bytes < 0:
        return "0 B"
    for unit in ("B", "KB", "MB", "GB", "TB"):
        if size_bytes < 1024:
            return f"{size_bytes:.2f} {unit}" if unit != "B" else f"{size_bytes} B"
        size_bytes /= 1024
    return f"{size_bytes:.2f} PB"


def format_mtime(path: Path) -> str:
    """
    获取并格式化文件的修改时间

    Args:
        path: 文件路径

    Returns:
        格式化的时间字符串，如 "2024-01-15 14:30:00"
    """
    try:
        mtime = path.stat().st_mtime
        return datetime.fromtimestamp(mtime).strftime("%Y-%m-%d %H:%M:%S")
    except OSError:
        return "N/A"


def format_file_info(path: Path, base: Optional[Path] = None) -> str:
    """
    格式化单个文件的信息字符串（用于搜索结果展示）

    Args:
        path: 文件路径
        base: 显示相对路径的基准目录，None 则显示绝对路径

    Returns:
        格式化后的文件信息行
    """
    try:
        stat = path.stat()
        size_str  = format_size(stat.st_size)
        mtime_str = datetime.fromtimestamp(stat.st_mtime).strftime("%Y-%m-%d %H:%M:%S")
        display   = path.relative_to(base) if base else path
        return f"  {str(display):<60s}  {size_str:>10s}  {mtime_str}"
    except OSError:
        return f"  {str(path)}  [无法读取]"


# ============================================================
# 安全重命名
# ============================================================

def safe_rename(
    src: Path,
    dst: Path,
    dry_run: bool = False,
    backup: bool = False,
    backup_suffix: str = ".bak",
) -> Tuple[bool, str]:
    """
    安全地将文件从 src 重命名为 dst

    - 若目标已存在，自动追加 _1、_2 等编号
    - 支持演练模式（dry_run）
    - 支持重命名前备份

    Args:
        src:           源文件路径
        dst:           目标文件路径
        dry_run:       演练模式（不实际操作）
        backup:        是否备份原文件
        backup_suffix: 备份文件后缀

    Returns:
        (成功标志, 消息字符串)
    """
    if not src.exists():
        return False, f"源文件不存在：{src}"

    # 若目标文件已存在，追加编号避免覆盖
    final_dst = dst
    if final_dst.exists() and final_dst != src:
        stem    = dst.stem
        suffix  = dst.suffix
        counter = 1
        while final_dst.exists():
            final_dst = dst.with_name(f"{stem}_{counter}{suffix}")
            counter += 1

    if dry_run:
        return True, f"[演练] {src.name}  ->  {final_dst.name}"

    try:
        if backup:
            backup_path = backup_file(src, backup_suffix)

        src.rename(final_dst)
        msg = f"重命名：{src.name}  ->  {final_dst.name}"
        if backup:
            msg += f"  (备份：{backup_path.name})"
        return True, msg
    except OSError as e:
        return False, f"重命名失败：{src.name} -> {final_dst.name}  原因：{e}"


# ============================================================
# 文件哈希
# ============================================================

def file_md5(path: Path, chunk_size: int = 8192) -> str:
    """
    计算文件的 MD5 哈希值

    对于大文件采用分块读取，避免占用过多内存。

    Args:
        path:       文件路径
        chunk_size: 分块大小（字节）

    Returns:
        MD5 十六进制字符串

    示例：
        >>> file_md5(Path("test.txt"))
        'd8e8fca2dc0f896fd7cb4cb0031ba249'
    """
    hasher = hashlib.md5()
    with open(path, "rb") as f:
        while chunk := f.read(chunk_size):
            hasher.update(chunk)
    return hasher.hexdigest()


def find_duplicates(files: List[Path]) -> dict[str, List[Path]]:
    """
    在给定文件列表中找出内容完全相同的文件（基于 MD5）

    Args:
        files: 文件路径列表

    Returns:
        字典 {md5: [重复文件路径列表]}，只包含有重复的组

    示例：
        duplicates = find_duplicates(collect_files(Path(".")))
        for md5, paths in duplicates.items():
            print(f"重复文件组 ({md5[:8]}...)：")
            for p in paths:
                print(f"  {p}")
    """
    md5_map: dict[str, List[Path]] = {}

    for f in files:
        try:
            digest = file_md5(f)
            md5_map.setdefault(digest, []).append(f)
        except OSError:
            continue

    # 只保留有重复的分组
    return {k: v for k, v in md5_map.items() if len(v) > 1}


# ============================================================
# 计时器（用于性能统计）
# ============================================================

class Timer:
    """
    简单计时器，支持上下文管理器使用

    使用示例：
        with Timer("批量重命名") as t:
            do_batch_rename()
        print(f"耗时：{t.elapsed:.3f} 秒")

        # 或手动调用
        timer = Timer()
        timer.start()
        ...
        print(timer.elapsed)
    """

    def __init__(self, name: str = "") -> None:
        self.name    = name
        self._start  = 0.0
        self.elapsed = 0.0

    def start(self) -> "Timer":
        self._start = time.perf_counter()
        return self

    def stop(self) -> float:
        self.elapsed = time.perf_counter() - self._start
        return self.elapsed

    def __enter__(self) -> "Timer":
        return self.start()

    def __exit__(self, *args: object) -> None:
        self.stop()
        if self.name:
            print(f"[Timer] {self.name} 耗时：{self.elapsed:.3f} 秒")


# ============================================================
# 模块自测
# ============================================================

if __name__ == "__main__":
    import tempfile

    print("=" * 60)
    print("工具函数模块自测")
    print("=" * 60)

    # 创建临时测试目录
    with tempfile.TemporaryDirectory() as tmpdir:
        root = Path(tmpdir)

        # 创建测试文件
        test_files = [
            root / "report_2024.txt",
            root / "report_2023.txt",
            root / "image.jpg",
            root / "data.csv",
            root / ".hidden_file",
            root / "sub" / "nested.txt",
            root / "sub" / "nested.py",
        ]
        for f in test_files:
            f.parent.mkdir(parents=True, exist_ok=True)
            f.write_text(f"内容：{f.name}\n" * 10, encoding="utf-8")

        # ---- 1. 文件匹配 ----
        print("\n1. 文件名匹配测试")
        print(f"  'report.txt' 匹配 '*.txt': {match_pattern('report.txt', '*.txt')}")
        print(f"  'image.jpg'  匹配 '*.txt': {match_pattern('image.jpg', '*.txt')}")
        print(f"  'file_1.py'  匹配 'file_?.py': {match_pattern('file_1.py', 'file_?.py')}")

        # ---- 2. 文件收集 ----
        print("\n2. 文件收集测试（*.txt，递归，忽略隐藏）")
        files = collect_files(root, "*.txt", recursive=True, ignore_hidden=True)
        for f in files:
            print(f"  {f.relative_to(root)}")

        # ---- 3. 格式化大小 ----
        print("\n3. 文件大小格式化")
        for sz in [0, 512, 1024, 1500, 1_048_576, 1_073_741_824]:
            print(f"  {sz:>12d} bytes  ->  {format_size(sz)}")

        # ---- 4. 格式化文件信息 ----
        print("\n4. 文件信息格式化")
        for f in files:
            print(format_file_info(f, base=root))

        # ---- 5. 备份文件 ----
        print("\n5. 文件备份测试")
        src = root / "report_2024.txt"
        bak = backup_file(src)
        print(f"  备份：{src.name}  ->  {bak.name}")
        bak2 = backup_file(src)   # 再次备份，名称应不同
        print(f"  再次备份：{src.name}  ->  {bak2.name}")

        # ---- 6. 安全重命名 ----
        print("\n6. 安全重命名测试")
        ok, msg = safe_rename(root / "data.csv", root / "data_new.csv", dry_run=True)
        print(f"  演练: {msg}")
        ok, msg = safe_rename(root / "data.csv", root / "data_new.csv")
        print(f"  实际: {msg}")

        # ---- 7. MD5 ----
        print("\n7. 文件 MD5")
        for f in collect_files(root, "*.txt", recursive=False, ignore_hidden=True):
            print(f"  {f.name:<30s}  MD5: {file_md5(f)}")

        # ---- 8. 重复文件检测 ----
        print("\n8. 重复文件检测")
        # 创建两个内容相同的文件
        dup_a = root / "dup_a.txt"
        dup_b = root / "dup_b.txt"
        dup_a.write_text("重复内容", encoding="utf-8")
        dup_b.write_text("重复内容", encoding="utf-8")
        all_files = collect_files(root, "*", recursive=True, ignore_hidden=True)
        dups = find_duplicates(all_files)
        if dups:
            for md5, paths in dups.items():
                print(f"  重复组 ({md5[:8]}...):")
                for p in paths:
                    print(f"    {p.relative_to(root)}")
        else:
            print("  未发现重复文件")

        # ---- 9. 计时器 ----
        print("\n9. 计时器测试")
        with Timer("测试操作") as t:
            time.sleep(0.05)
        print(f"  手动读取耗时：{t.elapsed:.3f} 秒")

    print("\n工具函数模块自测完成！")
