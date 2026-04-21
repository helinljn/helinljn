#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
将文件内容统一转换为：简体中文 + UTF-8。

功能：
1. 自动检测源文件编码并读取文本内容；
2. 使用 OpenCC 将繁体中文/异体中文转换为简体中文；
3. 以 UTF-8（默认无 BOM）重新写回文件；
4. 支持单文件或目录递归处理；
5. 可通过扩展名过滤文本文件。

依赖建议：
    pip install opencc-python-reimplemented charset-normalizer

如果你的环境已经安装了其他兼容的 OpenCC Python 包，也可以自行调整导入。
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Iterable, Optional

try:
    from charset_normalizer import from_bytes
except ImportError as exc:
    raise SystemExit(
        "缺少依赖：charset-normalizer\n"
        "请先安装：pip install charset-normalizer"
    ) from exc

try:
    from opencc import OpenCC
except ImportError as exc:
    raise SystemExit(
        "缺少依赖：opencc\n"
        "建议安装：pip install opencc-python-reimplemented"
    ) from exc


TEXT_EXTENSIONS = {
    ".txt",
    ".md",
    ".csv",
    ".json",
    ".yaml",
    ".yml",
    ".xml",
    ".html",
    ".htm",
    ".js",
    ".ts",
    ".jsx",
    ".tsx",
    ".css",
    ".scss",
    ".less",
    ".py",
    ".java",
    ".c",
    ".cc",
    ".cpp",
    ".cxx",
    ".h",
    ".hpp",
    ".hh",
    ".ini",
    ".conf",
    ".cfg",
    ".log",
    ".sql",
    ".toml",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="将文件内容统一转换为：简体中文 + UTF-8。"
    )
    parser.add_argument(
        "paths",
        nargs="+",
        help="待处理的文件或目录路径，可传多个。",
    )
    parser.add_argument(
        "-r",
        "--recursive",
        action="store_true",
        help="递归处理目录下文件。",
    )
    parser.add_argument(
        "--include-ext",
        nargs="*",
        default=None,
        help=(
            "只处理指定扩展名，例如：--include-ext .txt .md .csv。"
            "不传时使用内置常见文本扩展名列表。"
        ),
    )
    parser.add_argument(
        "--all-files",
        action="store_true",
        help="处理目录中的所有文件，不按扩展名过滤。",
    )
    parser.add_argument(
        "--encoding",
        default=None,
        help="强制指定输入编码，指定后将跳过自动检测，例如：gb18030。",
    )
    parser.add_argument(
        "--bom",
        action="store_true",
        help="输出为 UTF-8 with BOM；默认输出 UTF-8 无 BOM。",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="仅打印将要处理的文件，不实际写入。",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="输出更详细的处理信息。",
    )
    return parser.parse_args()


def normalize_extensions(extensions: Optional[Iterable[str]]) -> set[str]:
    if not extensions:
        return set(TEXT_EXTENSIONS)

    normalized = set()
    for ext in extensions:
        if not ext:
            continue
        normalized.add(ext if ext.startswith(".") else f".{ext}")
    return normalized


def should_process_file(file_path: Path, all_files: bool, extensions: set[str]) -> bool:
    if all_files:
        return True
    return file_path.suffix.lower() in extensions


def iter_target_files(
    paths: Iterable[str],
    recursive: bool,
    all_files: bool,
    extensions: set[str],
) -> Iterable[Path]:
    for raw_path in paths:
        path = Path(raw_path)
        if not path.exists():
            print(f"[WARN] 路径不存在，已跳过：{path}")
            continue

        if path.is_file():
            if should_process_file(path, all_files, extensions):
                yield path
            else:
                print(f"[SKIP] 非目标扩展名：{path}")
            continue

        if path.is_dir():
            iterator = path.rglob("*") if recursive else path.glob("*")
            for child in iterator:
                if child.is_file() and should_process_file(child, all_files, extensions):
                    yield child


def detect_encoding(data: bytes) -> Optional[str]:
    result = from_bytes(data).best()
    if result is not None and result.encoding:
        return result.encoding
    return None


def decode_text(data: bytes, forced_encoding: Optional[str] = None) -> tuple[str, str]:
    tried: list[str] = []

    if forced_encoding:
        text = data.decode(forced_encoding)
        return text, forced_encoding

    detected = detect_encoding(data)
    candidate_encodings = []

    if detected:
        candidate_encodings.append(detected)

    candidate_encodings.extend(
        [
            "utf-8",
            "utf-8-sig",
            "gb18030",
            "gbk",
            "gb2312",
            "big5",
            "utf-16",
            "utf-16-le",
            "utf-16-be",
        ]
    )

    seen = set()
    for encoding in candidate_encodings:
        encoding_key = encoding.lower()
        if encoding_key in seen:
            continue
        seen.add(encoding_key)
        tried.append(encoding)
        try:
            return data.decode(encoding), encoding
        except UnicodeDecodeError:
            continue

    raise UnicodeDecodeError(
        "unknown",
        data,
        0,
        len(data),
        f"无法识别文件编码，已尝试：{', '.join(tried)}",
    )


def convert_text_to_simplified(text: str, converter: OpenCC) -> str:
    return converter.convert(text)


def write_utf8_text(file_path: Path, text: str, with_bom: bool = False) -> None:
    encoding = "utf-8-sig" if with_bom else "utf-8"
    file_path.write_text(text, encoding=encoding, newline="")


def process_file(
    file_path: Path,
    converter: OpenCC,
    forced_encoding: Optional[str] = None,
    with_bom: bool = False,
    dry_run: bool = False,
    verbose: bool = False,
) -> bool:
    try:
        raw = file_path.read_bytes()
    except Exception as exc:
        print(f"[ERROR] 读取失败：{file_path} -> {exc}")
        return False

    try:
        original_text, source_encoding = decode_text(raw, forced_encoding)
    except Exception as exc:
        print(f"[ERROR] 解码失败：{file_path} -> {exc}")
        return False

    converted_text = convert_text_to_simplified(original_text, converter)

    if dry_run:
        print(f"[DRY-RUN] {file_path} (源编码: {source_encoding})")
        return True

    try:
        write_utf8_text(file_path, converted_text, with_bom=with_bom)
    except Exception as exc:
        print(f"[ERROR] 写入失败：{file_path} -> {exc}")
        return False

    if verbose:
        changed = "yes" if converted_text != original_text else "no"
        print(
            f"[OK] {file_path} | source={source_encoding} | "
            f"target={'utf-8-sig' if with_bom else 'utf-8'} | changed={changed}"
        )
    else:
        print(f"[OK] {file_path}")

    return True


def main() -> int:
    args = parse_args()
    extensions = normalize_extensions(args.include_ext)
    converter = OpenCC("t2s")

    files = list(
        iter_target_files(
            args.paths,
            recursive=args.recursive,
            all_files=args.all_files,
            extensions=extensions,
        )
    )

    if not files:
        print("未找到可处理的文件。")
        return 1

    success_count = 0
    failure_count = 0

    for file_path in files:
        ok = process_file(
            file_path=file_path,
            converter=converter,
            forced_encoding=args.encoding,
            with_bom=args.bom,
            dry_run=args.dry_run,
            verbose=args.verbose,
        )
        if ok:
            success_count += 1
        else:
            failure_count += 1

    print(
        f"处理完成：成功 {success_count} 个，失败 {failure_count} 个，"
        f"总计 {len(files)} 个。"
    )

    return 0 if failure_count == 0 else 2


if __name__ == "__main__":
    sys.exit(main())
