#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
按行去除文件内容中的“网址行”和“纯数字行”。

默认规则：
1. 如果一行中包含网址，则整行删除；
2. 如果一行去掉首尾空白后是纯数字，则整行删除；
3. 其他行保留原内容；
4. 默认输出 UTF-8（无 BOM）；
5. 支持输出到新文件，或可选原地覆盖。

说明：
- “按行去除”表示以整行为单位进行过滤，不是在行内局部替换。
- 对“纯数字”的判断默认仅接受阿拉伯数字，如：123、00045。
- 对网址的识别覆盖常见形式：
  - http://example.com
  - https://example.com
  - ftp://example.com
  - www.example.com
  - example.com/path
"""

from __future__ import annotations

import argparse
import re
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


URL_RE = re.compile(
    r"(?i)\b("
    r"(?:https?|ftp)://[^\s]+"
    r"|www\.[^\s]+"
    r"|(?:[a-z0-9-]+\.)+[a-z]{2,}(?:/[^\s]*)?"
    r")\b"
)

PURE_NUMBER_RE = re.compile(r"^\d+$")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="按行去除文件内容中的网址和纯数字。")
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
        "--keep-empty-lines",
        action="store_true",
        help="保留空行；默认保留，当前参数仅用于语义明确。",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="仅预览结果，不实际写入文件。",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="输出被删除的行及原因。",
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


def ensure_trailing_newline(line: str) -> str:
    if line.endswith("\r\n") or line.endswith("\n") or line.endswith("\r"):
        return line
    return line + "\n"


def line_has_url(line: str) -> bool:
    return URL_RE.search(line) is not None


def line_is_pure_number(line: str) -> bool:
    return PURE_NUMBER_RE.fullmatch(line.strip()) is not None


def should_remove_line(line: str) -> tuple[bool, str]:
    if line_has_url(line):
        return True, "url"
    if line_is_pure_number(line):
        return True, "number"
    return False, ""


def build_output_path(input_path: Path, output: Optional[str], in_place: bool) -> Path:
    if in_place:
        return input_path
    if output:
        return Path(output)
    return input_path.with_name(f"{input_path.stem}.cleaned{input_path.suffix}")


def filter_lines(lines: list[str], verbose: bool) -> tuple[list[str], int, int]:
    kept_lines: list[str] = []
    removed_url_count = 0
    removed_number_count = 0

    for index, line in enumerate(lines, start=1):
        remove, reason = should_remove_line(line)
        if remove:
            if reason == "url":
                removed_url_count += 1
            elif reason == "number":
                removed_number_count += 1

            if verbose:
                print(f"[REMOVE] line={index} reason={reason} content={line.rstrip()}")
            continue

        kept_lines.append(ensure_trailing_newline(line))

    return kept_lines, removed_url_count, removed_number_count


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

    kept_lines, removed_url_count, removed_number_count = filter_lines(
        lines=lines,
        verbose=args.verbose,
    )

    if args.dry_run:
        print(
            f"[DRY-RUN] 输入行数 {len(lines)}，保留行数 {len(kept_lines)}，"
            f"删除网址行 {removed_url_count}，删除纯数字行 {removed_number_count}，"
            f"源编码 {source_encoding}，输出文件 {output_path}"
        )
        return 0

    try:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(
            "".join(kept_lines),
            encoding=args.output_encoding,
            newline="",
        )
    except Exception as exc:
        print(f"写入失败：{output_path} -> {exc}")
        return 3

    print(
        f"处理完成：输入行数 {len(lines)}，保留行数 {len(kept_lines)}，"
        f"删除网址行 {removed_url_count}，删除纯数字行 {removed_number_count}，"
        f"源编码 {source_encoding}，输出文件 {output_path}"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
