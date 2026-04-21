#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
将文件内容按行排序。

功能：
1. 支持单文件输入；
2. 自动检测输入文件编码；
3. 按行排序，默认升序；
4. 输出到新文件，或可选原地覆盖；
5. 可选倒序排序；
6. 可选排序前去重；
7. 可选跳过空行；
8. 默认输出 UTF-8（无 BOM）。

说明：
- 默认按“去掉行尾换行符后的整行内容”作为排序键；
- 不修改每一行的文本内容本身，不做 trim、不改大小写；
- 仅在排序比较时忽略行尾换行符差异；
- 输出时统一保证每一行有换行符。
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Optional

try:
    from charset_normalizer import from_bytes
except ImportError as exc:
    raise SystemExit(
        "缺少依赖：charset-normalizer\n"
        "请先安装：pip install charset-normalizer"
    ) from exc


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="将文件内容按行排序。")
    parser.add_argument(
        "input",
        help="输入文件路径。",
    )
    parser.add_argument(
        "-o",
        "--output",
        default=None,
        help="输出文件路径。不传时默认生成同目录新文件。",
    )
    parser.add_argument(
        "--in-place",
        action="store_true",
        help="直接覆盖原文件。",
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
        "--reverse",
        action="store_true",
        help="按倒序排序。",
    )
    parser.add_argument(
        "--unique",
        action="store_true",
        help="排序前按整行内容去重，保留一份。",
    )
    parser.add_argument(
        "--skip-empty-lines",
        action="store_true",
        help="跳过空行。",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="仅预览结果，不实际写入文件。",
    )
    return parser.parse_args()


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


def line_key(line: str) -> str:
    return line.rstrip("\r\n")


def ensure_trailing_newline(line: str) -> str:
    if line.endswith("\r\n") or line.endswith("\n") or line.endswith("\r"):
        return line
    return line + "\n"


def build_output_path(input_path: Path, output: Optional[str], in_place: bool) -> Path:
    if in_place:
        return input_path
    if output:
        return Path(output)
    return input_path.with_name(f"{input_path.stem}.sorted{input_path.suffix}")


def process_lines(
    lines: list[str],
    unique: bool,
    skip_empty_lines: bool,
    reverse: bool,
) -> list[str]:
    processed_lines: list[str] = []
    seen: set[str] = set()

    for line in lines:
        key = line_key(line)

        if skip_empty_lines and key == "":
            continue

        if unique:
            if key in seen:
                continue
            seen.add(key)

        processed_lines.append(ensure_trailing_newline(line))

    processed_lines.sort(key=line_key, reverse=reverse)
    return processed_lines


def main() -> int:
    args = parse_args()
    input_path = Path(args.input)

    if not input_path.exists() or not input_path.is_file():
        print(f"输入文件不存在或不是文件：{input_path}")
        return 1

    output_path = build_output_path(input_path, args.output, args.in_place)

    try:
        lines, source_encoding = read_lines(input_path, args.encoding)
    except Exception as exc:
        print(f"读取失败：{input_path} -> {exc}")
        return 2

    result_lines = process_lines(
        lines=lines,
        unique=args.unique,
        skip_empty_lines=args.skip_empty_lines,
        reverse=args.reverse,
    )

    if args.dry_run:
        print(
            f"[DRY-RUN] 输入行数 {len(lines)}，输出行数 {len(result_lines)}，"
            f"源编码 {source_encoding}，输出文件 {output_path}"
        )
        return 0

    try:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            "".join(result_lines),
            encoding=args.output_encoding,
            newline="",
        )
    except Exception as exc:
        print(f"写入失败：{output_path} -> {exc}")
        return 3

    print(
        f"处理完成：输入行数 {len(lines)}，输出行数 {len(result_lines)}，"
        f"源编码 {source_encoding}，输出文件 {output_path}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
