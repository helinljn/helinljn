#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
合并文件内容，并在不修改行内容的情况下按行排重，输出到一个新文件。

默认行为：
1. 支持输入多个文件或目录；
2. 目录可选递归扫描；
3. 按“行”合并内容；
4. 按“行内容”排重，保留首次出现顺序；
5. 输出到新文件；
6. 默认输出 UTF-8（无 BOM）。

说明：
- “不修改内容”指不改写每一行的文本内容，不做 trim、不改空格、不改大小写。
- 排重时默认按“去掉行尾换行符后的整行内容”判断是否重复；
  这样可避免同一行因为 CRLF/LF 差异而无法排重。
- 输出时保留首次出现那一行原本的行内容，并统一保证写入结构正确。
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


TEXT_EXTENSIONS = {
    ".txt",
    ".md",
    ".csv",
    ".log",
    ".json",
    ".yaml",
    ".yml",
    ".xml",
    ".ini",
    ".conf",
    ".cfg",
}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="合并文件内容，并在不修改行内容的情况下按行排重，输出到新文件。"
    )
    parser.add_argument(
        "inputs",
        nargs="+",
        help="输入文件或目录路径，可传多个。",
    )
    parser.add_argument(
        "-o",
        "--output",
        required=True,
        help="输出文件路径。",
    )
    parser.add_argument(
        "-r",
        "--recursive",
        action="store_true",
        help="递归扫描目录。",
    )
    parser.add_argument(
        "--include-ext",
        nargs="*",
        default=None,
        help="只处理指定扩展名，例如：--include-ext .txt .md",
    )
    parser.add_argument(
        "--all-files",
        action="store_true",
        help="处理目录中的所有文件，不按扩展名过滤。",
    )
    parser.add_argument(
        "--encoding",
        default=None,
        help="强制指定输入编码，指定后跳过自动检测，例如：gb18030。",
    )
    parser.add_argument(
        "--output-encoding",
        default="utf-8",
        help="输出编码，默认 utf-8。可设为 utf-8-sig。",
    )
    parser.add_argument(
        "--no-dedup",
        action="store_true",
        help="仅合并，不排重。",
    )
    parser.add_argument(
        "--skip-empty-lines",
        action="store_true",
        help="跳过空行。",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="仅预览处理结果，不实际写入输出文件。",
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


def iter_input_files(
    inputs: Iterable[str],
    recursive: bool,
    all_files: bool,
    extensions: set[str],
    output_path: Path,
) -> Iterable[Path]:
    seen = set()

    for raw_path in inputs:
        path = Path(raw_path)
        if not path.exists():
            print(f"[WARN] 路径不存在，已跳过：{path}")
            continue

        if path.is_file():
            resolved = path.resolve()
            if resolved == output_path.resolve():
                print(f"[SKIP] 跳过输出文件自身：{path}")
                continue
            if should_process_file(path, all_files, extensions) and resolved not in seen:
                seen.add(resolved)
                yield path
            elif not should_process_file(path, all_files, extensions):
                print(f"[SKIP] 非目标扩展名：{path}")
            continue

        if path.is_dir():
            iterator = path.rglob("*") if recursive else path.glob("*")
            for child in iterator:
                if not child.is_file():
                    continue
                resolved = child.resolve()
                if resolved == output_path.resolve():
                    print(f"[SKIP] 跳过输出文件自身：{child}")
                    continue
                if resolved in seen:
                    continue
                if should_process_file(child, all_files, extensions):
                    seen.add(resolved)
                    yield child


def detect_encoding(data: bytes) -> Optional[str]:
    result = from_bytes(data).best()
    if result is not None and result.encoding:
        return result.encoding
    return None


def decode_text(data: bytes, forced_encoding: Optional[str] = None) -> tuple[str, str]:
    if forced_encoding:
        return data.decode(forced_encoding), forced_encoding

    candidate_encodings = []
    detected = detect_encoding(data)
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

    tried = set()
    for encoding in candidate_encodings:
        key = encoding.lower()
        if key in tried:
            continue
        tried.add(key)
        try:
            return data.decode(encoding), encoding
        except UnicodeDecodeError:
            continue

    raise UnicodeDecodeError("unknown", data, 0, len(data), "无法识别文件编码")


def read_lines(file_path: Path, forced_encoding: Optional[str] = None) -> tuple[list[str], str]:
    raw = file_path.read_bytes()
    text, source_encoding = decode_text(raw, forced_encoding)
    return text.splitlines(keepends=True), source_encoding


def dedup_key(line: str) -> str:
    return line.rstrip("\r\n")


def ensure_trailing_newline(line: str) -> str:
    if line.endswith("\r\n") or line.endswith("\n") or line.endswith("\r"):
        return line
    return line + "\n"


def merge_files(
    files: Iterable[Path],
    forced_encoding: Optional[str],
    deduplicate: bool,
    skip_empty_lines: bool,
    verbose: bool,
) -> tuple[list[str], int]:
    merged_lines: list[str] = []
    seen_keys: set[str] = set()
    total_input_lines = 0

    for file_path in files:
        try:
            lines, source_encoding = read_lines(file_path, forced_encoding)
        except Exception as exc:
            print(f"[ERROR] 读取失败：{file_path} -> {exc}")
            continue

        if verbose:
            print(f"[READ] {file_path} | encoding={source_encoding} | lines={len(lines)}")

        for line in lines:
            total_input_lines += 1

            if skip_empty_lines and dedup_key(line) == "":
                continue

            if deduplicate:
                key = dedup_key(line)
                if key in seen_keys:
                    continue
                seen_keys.add(key)

            merged_lines.append(ensure_trailing_newline(line))

    return merged_lines, total_input_lines


def main() -> int:
    args = parse_args()
    output_path = Path(args.output)
    extensions = normalize_extensions(args.include_ext)

    files = list(
        iter_input_files(
            inputs=args.inputs,
            recursive=args.recursive,
            all_files=args.all_files,
            extensions=extensions,
            output_path=output_path,
        )
    )

    if not files:
        print("未找到可处理的输入文件。")
        return 1

    merged_lines, total_input_lines = merge_files(
        files=files,
        forced_encoding=args.encoding,
        deduplicate=not args.no_dedup,
        skip_empty_lines=args.skip_empty_lines,
        verbose=args.verbose,
    )

    if args.dry_run:
        print(
            f"[DRY-RUN] 输入文件 {len(files)} 个，输入总行数 {total_input_lines}，"
            f"输出行数 {len(merged_lines)}，输出文件：{output_path}"
        )
        return 0

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("".join(merged_lines), encoding=args.output_encoding, newline="")

    print(
        f"处理完成：输入文件 {len(files)} 个，输入总行数 {total_input_lines}，"
        f"输出行数 {len(merged_lines)}，输出文件：{output_path}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
