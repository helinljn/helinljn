"""
核心处理类模块
实现文件搜索、批量重命名、文件内容替换等核心功能

知识点：
- 面向对象设计（单一职责、开闭原则）
- 数据类（dataclass）
- 类型注解
- 异常处理
- 正则表达式
- pathlib 文件操作
- logging 日志记录
"""

import logging
import re
import shutil
from dataclasses import dataclass, field
from datetime import datetime
from pathlib import Path
from typing import Callable, List, Optional, Tuple

from utils import (
    Timer,
    backup_file,
    collect_files,
    find_duplicates,
    format_file_info,
    format_size,
    safe_rename,
)

# 获取模块日志器
logger = logging.getLogger("file_processor")


# ============================================================
# 数据类：操作结果
# ============================================================

@dataclass
class OperationResult:
    """
    单次文件操作的结果记录

    Attributes:
        success:   操作是否成功
        src:       源文件路径
        dst:       目标文件路径（重命名操作）
        message:   操作描述或错误信息
        dry_run:   是否为演练模式
    """
    success:  bool
    src:      Path
    dst:      Optional[Path] = None
    message:  str = ""
    dry_run:  bool = False


@dataclass
class BatchResult:
    """
    批量操作的汇总结果

    Attributes:
        total:    处理文件总数
        success:  成功数量
        failed:   失败数量
        skipped:  跳过数量
        elapsed:  耗时（秒）
        details:  每个文件的操作结果列表
    """
    total:   int = 0
    success: int = 0
    failed:  int = 0
    skipped: int = 0
    elapsed: float = 0.0
    details: List[OperationResult] = field(default_factory=list)

    @property
    def summary(self) -> str:
        """返回结果摘要字符串"""
        status = "演练" if any(d.dry_run for d in self.details) else "完成"
        return (
            f"[{status}] 合计：{self.total} 个文件  "
            f"成功：{self.success}  失败：{self.failed}  "
            f"跳过：{self.skipped}  耗时：{self.elapsed:.3f} 秒"
        )

    def print_details(self, show_all: bool = False) -> None:
        """
        打印操作详情

        Args:
            show_all: True 则打印所有记录，False 仅打印失败记录
        """
        for r in self.details:
            if show_all or not r.success:
                status = "✓" if r.success else "✗"
                print(f"  [{status}] {r.message}")


# ============================================================
# 文件搜索器
# ============================================================

class FileSearcher:
    """
    文件搜索器

    提供多种条件的文件搜索功能：
    - 按文件名模式（通配符）
    - 按文件大小范围
    - 按修改时间范围
    - 递归/非递归搜索
    - 隐藏文件过滤

    使用示例：
        searcher = FileSearcher()
        files = searcher.search(
            root=Path("./docs"),
            pattern="*.txt",
            recursive=True,
            min_size=1024,
        )
        searcher.print_results(files, base=Path("./docs"))
    """

    def __init__(self, ignore_hidden: bool = True) -> None:
        """
        Args:
            ignore_hidden: 默认是否忽略隐藏文件
        """
        self.ignore_hidden = ignore_hidden

    def search(
        self,
        root: Path,
        pattern: str = "*",
        recursive: bool = True,
        ignore_hidden: Optional[bool] = None,
        min_size: Optional[int] = None,
        max_size: Optional[int] = None,
        modified_after: Optional[datetime] = None,
        modified_before: Optional[datetime] = None,
    ) -> List[Path]:
        """
        搜索符合条件的文件

        Args:
            root:            搜索根目录
            pattern:         文件名通配符，如 "*.txt"、"report_*"
            recursive:       是否递归搜索子目录
            ignore_hidden:   是否忽略隐藏文件，None 则使用实例默认值
            min_size:        最小文件大小（字节）
            max_size:        最大文件大小（字节）
            modified_after:  修改时间下限
            modified_before: 修改时间上限

        Returns:
            满足所有条件的文件路径列表（已排序）
        """
        _ignore = ignore_hidden if ignore_hidden is not None else self.ignore_hidden

        logger.info(
            f"开始搜索：root={root}  pattern={pattern}  "
            f"recursive={recursive}  ignore_hidden={_ignore}"
        )

        with Timer() as t:
            try:
                files = collect_files(
                    root=root,
                    pattern=pattern,
                    recursive=recursive,
                    ignore_hidden=_ignore,
                    min_size=min_size,
                    max_size=max_size,
                    modified_after=modified_after,
                    modified_before=modified_before,
                )
            except (FileNotFoundError, NotADirectoryError) as e:
                logger.error(f"搜索失败：{e}")
                return []

        logger.info(f"搜索完成：找到 {len(files)} 个文件，耗时 {t.elapsed:.3f} 秒")
        return files

    @staticmethod
    def print_results(files: List[Path], base: Optional[Path] = None) -> None:
        """
        格式化打印搜索结果

        Args:
            files: 文件列表
            base:  用于计算相对路径的基准目录
        """
        if not files:
            print("  (无匹配文件)")
            return

        header = f"  {'文件路径':<60s}  {'大小':>10s}  {'修改时间'}"
        print(header)
        print("  " + "-" * (len(header) - 2))

        total_size = 0
        for f in files:
            print(format_file_info(f, base=base))
            try:
                total_size += f.stat().st_size
            except OSError:
                pass

        print("  " + "-" * (len(header) - 2))
        print(f"  共 {len(files)} 个文件，合计大小：{format_size(total_size)}")


# ============================================================
# 批量重命名器
# ============================================================

class BatchRenamer:
    """
    批量重命名器

    支持多种重命名策略：
    1. 添加前缀/后缀
    2. 替换文件名中的文本（支持正则）
    3. 序号重命名（file_001.txt、file_002.txt……）
    4. 自定义重命名函数

    所有操作均支持：
    - 演练模式（dry_run）
    - 自动备份原文件

    使用示例：
        renamer = BatchRenamer(dry_run=True, backup=False)
        result = renamer.add_prefix(files, "photo_")
        result.print_details(show_all=True)
        print(result.summary)
    """

    def __init__(
        self,
        dry_run: bool = False,
        backup: bool = True,
        backup_suffix: str = ".bak",
    ) -> None:
        """
        Args:
            dry_run:       演练模式（不实际修改文件）
            backup:        是否在重命名前备份原文件
            backup_suffix: 备份文件后缀
        """
        self.dry_run       = dry_run
        self.backup        = backup
        self.backup_suffix = backup_suffix

    def _do_rename(self, src: Path, new_name: str) -> OperationResult:
        """
        内部重命名执行器

        Args:
            src:      源文件路径
            new_name: 新文件名（仅文件名部分，不含目录）

        Returns:
            OperationResult
        """
        dst = src.parent / new_name

        # 名称未变，跳过
        if src.name == new_name:
            return OperationResult(
                success=True,
                src=src,
                dst=src,
                message=f"跳过（名称未变）：{src.name}",
                dry_run=self.dry_run,
            )

        ok, msg = safe_rename(
            src,
            dst,
            dry_run=self.dry_run,
            backup=self.backup and not self.dry_run,
            backup_suffix=self.backup_suffix,
        )

        if ok:
            logger.info(msg)
        else:
            logger.error(msg)

        return OperationResult(
            success=ok,
            src=src,
            dst=dst if ok else None,
            message=msg,
            dry_run=self.dry_run,
        )

    def _batch(self, files: List[Path], name_func: Callable[[Path], str]) -> BatchResult:
        """
        批量重命名的通用框架

        Args:
            files:     要处理的文件列表
            name_func: 接收 Path 返回新文件名字符串的函数

        Returns:
            BatchResult
        """
        result = BatchResult(total=len(files))

        with Timer() as t:
            for f in files:
                try:
                    new_name = name_func(f)
                    op = self._do_rename(f, new_name)
                except Exception as e:
                    op = OperationResult(
                        success=False,
                        src=f,
                        message=f"生成新名称失败：{f.name}  原因：{e}",
                        dry_run=self.dry_run,
                    )
                    logger.exception(f"生成新名称时发生异常：{f}")

                result.details.append(op)
                if op.success:
                    if "跳过" in op.message:
                        result.skipped += 1
                    else:
                        result.success += 1
                else:
                    result.failed += 1

        result.elapsed = t.elapsed
        return result

    # ----------------------------------------------------------
    # 重命名策略 1：添加前缀
    # ----------------------------------------------------------

    def add_prefix(self, files: List[Path], prefix: str) -> BatchResult:
        """
        为所有文件添加统一前缀

        Args:
            files:  文件列表
            prefix: 要添加的前缀字符串

        Returns:
            BatchResult

        示例：
            report.txt  ->  2024_report.txt
        """
        logger.info(f"批量添加前缀：prefix={prefix!r}，共 {len(files)} 个文件")
        return self._batch(files, lambda f: prefix + f.name)

    # ----------------------------------------------------------
    # 重命名策略 2：添加后缀（扩展名前）
    # ----------------------------------------------------------

    def add_suffix(self, files: List[Path], suffix: str) -> BatchResult:
        """
        为所有文件（扩展名前）添加统一后缀

        Args:
            files:  文件列表
            suffix: 要添加的后缀字符串

        Returns:
            BatchResult

        示例：
            report.txt  ->  report_v2.txt
        """
        logger.info(f"批量添加后缀：suffix={suffix!r}，共 {len(files)} 个文件")

        def name_func(f: Path) -> str:
            return f.stem + suffix + f.suffix

        return self._batch(files, name_func)

    # ----------------------------------------------------------
    # 重命名策略 3：文件名文本替换
    # ----------------------------------------------------------

    def replace_in_name(
        self,
        files: List[Path],
        old: str,
        new: str,
        use_regex: bool = False,
        flags: int = 0,
    ) -> BatchResult:
        """
        替换文件名中的指定文本

        Args:
            files:     文件列表
            old:       要替换的文本或正则表达式
            new:       替换为的文本（正则模式下支持 \\1 等反向引用）
            use_regex: 是否将 old 视为正则表达式
            flags:     正则标志（如 re.IGNORECASE）

        Returns:
            BatchResult

        示例：
            report_2023.txt  ->  report_2024.txt  (old="2023", new="2024")
            IMG_001.jpg      ->  photo_001.jpg    (old=r"IMG_(\d+)", new=r"photo_\1", use_regex=True)
        """
        logger.info(
            f"批量替换文件名：old={old!r}  new={new!r}  "
            f"use_regex={use_regex}，共 {len(files)} 个文件"
        )

        if use_regex:
            try:
                pattern = re.compile(old, flags)
            except re.error as e:
                logger.error(f"正则表达式编译失败：{old!r}  原因：{e}")
                return BatchResult(total=len(files), failed=len(files))

            def name_func(f: Path) -> str:
                return pattern.sub(new, f.name)
        else:
            def name_func(f: Path) -> str:
                return f.name.replace(old, new)

        return self._batch(files, name_func)

    # ----------------------------------------------------------
    # 重命名策略 4：序号重命名
    # ----------------------------------------------------------

    def numbered_rename(
        self,
        files: List[Path],
        base_name: str = "file",
        start: int = 1,
        step: int = 1,
        padding: int = 3,
        sep: str = "_",
    ) -> BatchResult:
        """
        按序号重命名文件（保留原扩展名）

        Args:
            files:     文件列表（建议预先排序）
            base_name: 基础文件名
            start:     起始序号
            step:      序号步长
            padding:   序号数字位数（不足时补零）
            sep:       基础名与序号间的分隔符

        Returns:
            BatchResult

        示例：
            a.jpg, b.jpg, c.jpg  ->  photo_001.jpg, photo_002.jpg, photo_003.jpg
        """
        logger.info(
            f"批量序号重命名：base={base_name!r}  start={start}  "
            f"step={step}  padding={padding}，共 {len(files)} 个文件"
        )

        num = start
        num_list = []
        for _ in files:
            num_list.append(num)
            num += step

        def name_func_factory(idx: int) -> Callable[[Path], str]:
            n = num_list[idx]
            def name_func(f: Path) -> str:
                return f"{base_name}{sep}{str(n).zfill(padding)}{f.suffix}"
            return name_func

        result = BatchResult(total=len(files))
        with Timer() as t:
            for i, f in enumerate(files):
                try:
                    new_name = name_func_factory(i)(f)
                    op = self._do_rename(f, new_name)
                except Exception as e:
                    op = OperationResult(
                        success=False,
                        src=f,
                        message=f"生成新名称失败：{f.name}  原因：{e}",
                        dry_run=self.dry_run,
                    )
                    logger.exception(f"序号重命名发生异常：{f}")

                result.details.append(op)
                if op.success:
                    if "跳过" in op.message:
                        result.skipped += 1
                    else:
                        result.success += 1
                else:
                    result.failed += 1

        result.elapsed = t.elapsed
        return result

    # ----------------------------------------------------------
    # 重命名策略 5：自定义函数
    # ----------------------------------------------------------

    def custom_rename(
        self,
        files: List[Path],
        name_func: Callable[[Path], str],
    ) -> BatchResult:
        """
        使用自定义函数进行重命名

        Args:
            files:     文件列表
            name_func: 接收 Path 返回新文件名的函数

        Returns:
            BatchResult

        示例：
            # 将文件名转为大写
            result = renamer.custom_rename(files, lambda f: f.name.upper())
        """
        logger.info(f"自定义重命名，共 {len(files)} 个文件")
        return self._batch(files, name_func)


# ============================================================
# 文件内容替换器
# ============================================================

class ContentReplacer:
    """
    文件内容批量替换器

    支持：
    - 普通文本替换
    - 正则表达式替换（支持多行、忽略大小写等标志）
    - 演练模式（只展示差异，不实际修改）
    - 自动备份原文件

    使用示例：
        replacer = ContentReplacer(encoding="utf-8", dry_run=True)
        result = replacer.replace(files, old="foo", new="bar")
        print(result.summary)
    """

    def __init__(
        self,
        encoding: str = "utf-8",
        dry_run: bool = False,
        backup: bool = True,
        backup_suffix: str = ".bak",
        errors: str = "replace",
    ) -> None:
        """
        Args:
            encoding:      文本文件编码
            dry_run:       演练模式
            backup:        是否备份原文件
            backup_suffix: 备份文件后缀
            errors:        编码错误处理策略（'strict'/'replace'/'ignore'）
        """
        self.encoding      = encoding
        self.dry_run       = dry_run
        self.backup        = backup
        self.backup_suffix = backup_suffix
        self.errors        = errors

    def _replace_in_file(
        self,
        path: Path,
        replacer: Callable[[str], str],
    ) -> OperationResult:
        """
        在单个文件中执行内容替换

        Args:
            path:     目标文件
            replacer: 接收原文本，返回替换后文本的函数

        Returns:
            OperationResult
        """
        try:
            original = path.read_text(encoding=self.encoding, errors=self.errors)
        except OSError as e:
            msg = f"读取文件失败：{path.name}  原因：{e}"
            logger.error(msg)
            return OperationResult(success=False, src=path, message=msg, dry_run=self.dry_run)

        try:
            replaced = replacer(original)
        except re.error as e:
            msg = f"正则替换失败：{path.name}  原因：{e}"
            logger.error(msg)
            return OperationResult(success=False, src=path, message=msg, dry_run=self.dry_run)

        # 内容未变，跳过
        if original == replaced:
            msg = f"跳过（内容无变化）：{path.name}"
            logger.debug(msg)
            return OperationResult(success=True, src=path, message=msg, dry_run=self.dry_run)

        if self.dry_run:
            # 演练模式：统计变更行数
            orig_lines = original.splitlines()
            repl_lines = replaced.splitlines()
            changed = sum(1 for a, b in zip(orig_lines, repl_lines) if a != b)
            msg = f"[演练] 将替换 {changed} 行：{path.name}"
            logger.info(msg)
            return OperationResult(success=True, src=path, message=msg, dry_run=True)

        # 备份原文件
        backup_path: Optional[Path] = None
        if self.backup:
            try:
                backup_path = backup_file(path, self.backup_suffix)
            except OSError as e:
                msg = f"备份失败：{path.name}  原因：{e}"
                logger.error(msg)
                return OperationResult(success=False, src=path, message=msg, dry_run=False)

        # 写入替换后内容
        try:
            path.write_text(replaced, encoding=self.encoding)
            msg = f"已替换：{path.name}"
            if backup_path:
                msg += f"  (备份：{backup_path.name})"
            logger.info(msg)
            return OperationResult(success=True, src=path, message=msg, dry_run=False)
        except OSError as e:
            msg = f"写入失败：{path.name}  原因：{e}"
            logger.error(msg)
            return OperationResult(success=False, src=path, message=msg, dry_run=False)

    def replace(
        self,
        files: List[Path],
        old: str,
        new: str,
        use_regex: bool = False,
        flags: int = 0,
    ) -> BatchResult:
        """
        在多个文件中批量替换文本

        Args:
            files:     文件列表
            old:       要替换的文本或正则表达式
            new:       替换目标文本（正则模式支持 \\1 等反向引用）
            use_regex: 是否将 old 视为正则表达式
            flags:     正则标志（如 re.IGNORECASE | re.MULTILINE）

        Returns:
            BatchResult

        示例：
            # 普通替换
            result = replacer.replace(files, "version = 1.0", "version = 2.0")

            # 正则替换（忽略大小写）
            result = replacer.replace(
                files,
                old=r"Copyright\s+\d{4}",
                new="Copyright 2024",
                use_regex=True,
                flags=re.IGNORECASE,
            )
        """
        logger.info(
            f"批量内容替换：old={old!r}  new={new!r}  "
            f"use_regex={use_regex}，共 {len(files)} 个文件"
        )

        # 编译正则（提前检测语法错误）
        if use_regex:
            try:
                pattern = re.compile(old, flags)
                replacer_func: Callable[[str], str] = lambda text: pattern.sub(new, text)
            except re.error as e:
                logger.error(f"正则表达式编译失败：{old!r}  原因：{e}")
                return BatchResult(total=len(files), failed=len(files))
        else:
            replacer_func = lambda text: text.replace(old, new)

        result = BatchResult(total=len(files))

        with Timer() as t:
            for f in files:
                op = self._replace_in_file(f, replacer_func)
                result.details.append(op)
                if op.success:
                    if "跳过" in op.message:
                        result.skipped += 1
                    else:
                        result.success += 1
                else:
                    result.failed += 1

        result.elapsed = t.elapsed
        return result

    def replace_multiple(
        self,
        files: List[Path],
        replacements: List[Tuple[str, str]],
        use_regex: bool = False,
        flags: int = 0,
    ) -> BatchResult:
        """
        在多个文件中执行多组替换（一次读写，效率更高）

        Args:
            files:        文件列表
            replacements: [(old1, new1), (old2, new2), ...] 替换对列表
            use_regex:    是否使用正则
            flags:        正则标志

        Returns:
            BatchResult
        """
        logger.info(
            f"批量多组替换：{len(replacements)} 组规则，"
            f"共 {len(files)} 个文件"
        )

        # 预编译所有规则
        if use_regex:
            try:
                compiled = [(re.compile(old, flags), new) for old, new in replacements]
            except re.error as e:
                logger.error(f"正则编译失败：{e}")
                return BatchResult(total=len(files), failed=len(files))

            def multi_replacer(text: str) -> str:
                for pattern, new in compiled:
                    text = pattern.sub(new, text)
                return text
        else:
            def multi_replacer(text: str) -> str:
                for old, new in replacements:
                    text = text.replace(old, new)
                return text

        result = BatchResult(total=len(files))

        with Timer() as t:
            for f in files:
                op = self._replace_in_file(f, multi_replacer)
                result.details.append(op)
                if op.success:
                    if "跳过" in op.message:
                        result.skipped += 1
                    else:
                        result.success += 1
                else:
                    result.failed += 1

        result.elapsed = t.elapsed
        return result


# ============================================================
# 重复文件检测器
# ============================================================

class DuplicateFinder:
    """
    重复文件检测器（基于 MD5 哈希）

    使用示例：
        finder = DuplicateFinder()
        groups = finder.find(files)
        finder.print_report(groups, base=root)
    """

    @staticmethod
    def find(files: List[Path]) -> dict[str, List[Path]]:
        """
        在文件列表中查找重复文件

        Args:
            files: 文件路径列表

        Returns:
            {md5: [文件路径列表]}，仅包含有重复的组
        """
        logger.info(f"开始检测重复文件，共 {len(files)} 个文件")

        with Timer() as t:
            groups = find_duplicates(files)

        total_dup = sum(len(v) for v in groups.values())
        logger.info(
            f"重复文件检测完成：发现 {len(groups)} 组，"
            f"{total_dup} 个重复文件，耗时 {t.elapsed:.3f} 秒"
        )
        return groups

    @staticmethod
    def print_report(
        groups: dict[str, List[Path]],
        base: Optional[Path] = None,
    ) -> None:
        """
        格式化打印重复文件报告

        Args:
            groups: find() 返回的结果字典
            base:   用于计算相对路径的基准目录
        """
        if not groups:
            print("  未发现重复文件。")
            return

        total_dup  = sum(len(v) for v in groups.values())
        total_size = 0

        print(f"\n  发现 {len(groups)} 组重复文件（共 {total_dup} 个）：\n")

        for i, (md5, paths) in enumerate(groups.items(), 1):
            try:
                size = paths[0].stat().st_size
                total_size += size * (len(paths) - 1)  # 冗余占用
            except OSError:
                size = 0

            print(f"  【组 {i}】MD5: {md5}  单文件大小: {format_size(size)}")
            for p in paths:
                display = p.relative_to(base) if base else p
                print(f"    - {display}")
            print()

        print(f"  冗余占用空间（估算）：{format_size(total_size)}")


# ============================================================
# 文件处理器（外观类，统一入口）
# ============================================================

class FileProcessor:
    """
    文件批量处理器（外观模式）

    整合 FileSearcher、BatchRenamer、ContentReplacer、DuplicateFinder，
    提供统一的高层接口。

    使用示例：
        processor = FileProcessor(dry_run=True, backup=True)
        files = processor.search(Path("./src"), "*.py")
        result = processor.rename_add_prefix(files, "new_")
        print(result.summary)
    """

    def __init__(
        self,
        dry_run: bool = False,
        backup: bool = True,
        backup_suffix: str = ".bak",
        encoding: str = "utf-8",
        recursive: bool = True,
        ignore_hidden: bool = True,
    ) -> None:
        """
        Args:
            dry_run:       演练模式（不实际修改文件）
            backup:        重命名/替换前是否备份原文件
            backup_suffix: 备份文件后缀
            encoding:      文本文件编码
            recursive:     默认递归搜索
            ignore_hidden: 默认忽略隐藏文件
        """
        self.dry_run       = dry_run
        self.backup        = backup
        self.backup_suffix = backup_suffix
        self.encoding      = encoding
        self.recursive     = recursive
        self.ignore_hidden = ignore_hidden

        # 子组件
        self._searcher  = FileSearcher(ignore_hidden=ignore_hidden)
        self._renamer   = BatchRenamer(dry_run=dry_run, backup=backup, backup_suffix=backup_suffix)
        self._replacer  = ContentReplacer(
            encoding=encoding,
            dry_run=dry_run,
            backup=backup,
            backup_suffix=backup_suffix,
        )
        self._dup_finder = DuplicateFinder()

    # ----------------------------------------------------------
    # 搜索接口
    # ----------------------------------------------------------

    def search(
        self,
        root: Path,
        pattern: str = "*",
        recursive: Optional[bool] = None,
        ignore_hidden: Optional[bool] = None,
        min_size: Optional[int] = None,
        max_size: Optional[int] = None,
        modified_after: Optional[datetime] = None,
        modified_before: Optional[datetime] = None,
        print_results: bool = True,
    ) -> List[Path]:
        """搜索文件并可选择性打印结果"""
        files = self._searcher.search(
            root=root,
            pattern=pattern,
            recursive=recursive if recursive is not None else self.recursive,
            ignore_hidden=ignore_hidden if ignore_hidden is not None else self.ignore_hidden,
            min_size=min_size,
            max_size=max_size,
            modified_after=modified_after,
            modified_before=modified_before,
        )
        if print_results:
            self._searcher.print_results(files, base=root)
        return files

    # ----------------------------------------------------------
    # 重命名接口
    # ----------------------------------------------------------

    def rename_add_prefix(self, files: List[Path], prefix: str) -> BatchResult:
        """批量添加前缀"""
        return self._renamer.add_prefix(files, prefix)

    def rename_add_suffix(self, files: List[Path], suffix: str) -> BatchResult:
        """批量添加后缀"""
        return self._renamer.add_suffix(files, suffix)

    def rename_replace(
        self,
        files: List[Path],
        old: str,
        new: str,
        use_regex: bool = False,
    ) -> BatchResult:
        """批量替换文件名中的文本"""
        return self._renamer.replace_in_name(files, old, new, use_regex=use_regex)

    def rename_numbered(
        self,
        files: List[Path],
        base_name: str = "file",
        start: int = 1,
        padding: int = 3,
    ) -> BatchResult:
        """批量序号重命名"""
        return self._renamer.numbered_rename(files, base_name=base_name, start=start, padding=padding)

    # ----------------------------------------------------------
    # 内容替换接口
    # ----------------------------------------------------------

    def replace_content(
        self,
        files: List[Path],
        old: str,
        new: str,
        use_regex: bool = False,
        flags: int = 0,
    ) -> BatchResult:
        """批量替换文件内容"""
        return self._replacer.replace(files, old, new, use_regex=use_regex, flags=flags)

    def replace_content_multiple(
        self,
        files: List[Path],
        replacements: List[Tuple[str, str]],
        use_regex: bool = False,
    ) -> BatchResult:
        """批量执行多组内容替换"""
        return self._replacer.replace_multiple(files, replacements, use_regex=use_regex)

    # ----------------------------------------------------------
    # 重复文件检测接口
    # ----------------------------------------------------------

    def find_duplicates(
        self,
        files: List[Path],
        print_report: bool = True,
        base: Optional[Path] = None,
    ) -> dict[str, List[Path]]:
        """检测重复文件"""
        groups = self._dup_finder.find(files)
        if print_report:
            self._dup_finder.print_report(groups, base=base)
        return groups


# ============================================================
# 模块自测
# ============================================================

if __name__ == "__main__":
    import tempfile
    import sys
    import os

    # 确保能导入同目录的 utils
    sys.path.insert(0, str(Path(__file__).parent))

    # 初始化日志（自测时直接输出到控制台）
    logging.basicConfig(
        level=logging.INFO,
        format="%(asctime)s [%(levelname)s] %(name)s - %(message)s",
        datefmt="%H:%M:%S",
    )

    print("=" * 70)
    print("file_processor.py 模块自测")
    print("=" * 70)

    with tempfile.TemporaryDirectory() as tmpdir:
        root = Path(tmpdir)

        # ---------- 准备测试文件 ----------
        test_data = {
            "report_2023_jan.txt": "版本：1.0\n作者：张三\nCopyright 2023\n内容略",
            "report_2023_feb.txt": "版本：1.0\n作者：李四\nCopyright 2023\n内容略",
            "report_2024_jan.txt": "版本：2.0\n作者：王五\nCopyright 2024\n内容略",
            "image_001.jpg":       b"\xff\xd8\xff\xe0",   # JPEG 魔数（二进制）
            "image_002.jpg":       b"\xff\xd8\xff\xe0",   # 内容相同（重复文件）
            "data.csv":            "name,age\n张三,25\n李四,30\n",
            "notes.txt":           "这是一个备注文件。",
            ".gitignore":          "*.pyc\n__pycache__/\n",   # 隐藏文件
        }

        sub = root / "subdir"
        sub.mkdir()

        for fname, content in test_data.items():
            f = root / fname
            if isinstance(content, bytes):
                f.write_bytes(content)
            else:
                f.write_text(content, encoding="utf-8")

        # 子目录文件
        (sub / "sub_report.txt").write_text("子目录报告\n版本：1.0", encoding="utf-8")

        # ---------- 1. 搜索测试 ----------
        print("\n【1】搜索 *.txt（递归，忽略隐藏）")
        processor = FileProcessor(dry_run=True, backup=False)
        txt_files = processor.search(root, "*.txt", recursive=True)

        print("\n【2】搜索 *.jpg（仅当前目录）")
        jpg_files = processor.search(root, "*.jpg", recursive=False)

        # ---------- 2. 批量重命名（演练模式）----------
        print("\n【3】批量添加前缀 'backup_'（演练）")
        result = processor.rename_add_prefix(txt_files, "backup_")
        result.print_details(show_all=True)
        print(result.summary)

        print("\n【4】批量序号重命名 jpg 文件（演练）")
        result = processor.rename_numbered(jpg_files, base_name="photo", start=1, padding=3)
        result.print_details(show_all=True)
        print(result.summary)

        print("\n【5】正则替换文件名（2023 -> 2024）（演练）")
        result = processor.rename_replace(
            txt_files,
            old=r"(\d{4})",
            new=r"[\1]",
            use_regex=True,
        )
        result.print_details(show_all=True)
        print(result.summary)

        # ---------- 3. 内容替换（演练模式）----------
        print("\n【6】批量替换文件内容：'1.0' -> '2.0'（演练）")
        result = processor.replace_content(txt_files, old="版本：1.0", new="版本：2.0")
        result.print_details(show_all=True)
        print(result.summary)

        print("\n【7】多组内容替换（演练）")
        replacements = [
            ("张三", "Alice"),
            ("李四", "Bob"),
            (r"Copyright\s+(\d{4})", r"(c) \1"),
        ]
        result = processor.replace_content_multiple(
            txt_files, replacements, use_regex=True
        )
        result.print_details(show_all=True)
        print(result.summary)

        # ---------- 4. 重复文件检测 ----------
        print("\n【8】重复文件检测")
        all_files = processor.search(root, "*", recursive=True, print_results=False)
        processor.find_duplicates(all_files, base=root)

    print("\n" + "=" * 70)
    print("模块自测完成！")
    print("=" * 70)
