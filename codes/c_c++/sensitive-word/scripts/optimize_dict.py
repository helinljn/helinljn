#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
敏感词库清洗与精简脚本。

处理流程：
1. 自动检测输入文件编码并读取文本；
2. 跳过空行与注释行；
3. 对词条做归一化：
   - 繁体转简体
   - 全角转半角
   - 转小写
4. 过滤无效词条：
   - 纯数字
   - 含 URL
   - 含 IP 地址
   - 纯符号串
   - 数字过长的数字/符号混合串
   - 超过长度上限的词条
5. 对结果去重并排序；
6. 以 UTF-8 输出优化后的词库文件。

说明：
- 本脚本面向“敏感词词库”场景，不是通用文本处理工具。
- 已吸收部分通用能力，包括自动编码检测、繁转简、无效模式过滤和最终排序输出。
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

try:
    from opencc import OpenCC
except ImportError as exc:
    raise SystemExit(
        "缺少依赖：opencc\n"
        "建议安装：pip install opencc-python-reimplemented"
    ) from exc


DEFAULT_MAX_LENGTH = 10
DEFAULT_OUTPUT = "optimized_dict.txt"
OUTPUT_ENCODING = "utf-8"

# 常见 URL 形式：
# - http://example.com
# - https://example.com/path
# - ftp://example.com
# - www.example.com
# - example.com/path
URL_RE = re.compile(
    r"(?i)\b("
    r"(?:https?|ftp)://[^\s]+"
    r"|www\.[^\s]+"
    r"|(?:[a-z0-9-]+\.)+[a-z]{2,}(?:/[^\s]*)?"
    r")\b"
)

PURE_NUMBER_RE = re.compile(r"^\d+$")
IP_RE = re.compile(r"\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}")
SYMBOL_AND_DIGIT_ONLY_RE = re.compile(r"^[\d\W_]+$")


def detect_encoding(data: bytes) -> Optional[str]:
    """检测字节串编码。"""
    result = from_bytes(data).best()
    if result is not None and result.encoding:
        return result.encoding
    return None


def decode_text(data: bytes, forced_encoding: Optional[str] = None) -> tuple[str, str]:
    """
    将字节串解码为文本。

    优先级：
    1. 使用强制指定编码；
    2. 使用自动检测出的编码；
    3. 依次尝试常见中文文本编码。
    """
    if forced_encoding:
        return data.decode(forced_encoding), forced_encoding

    candidate_encodings: list[str] = []

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

    tried: set[str] = set()
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


def read_input_lines(input_file: Path) -> tuple[list[str], str]:
    """读取输入文件并返回行列表及识别出的源编码。"""
    raw = input_file.read_bytes()
    text, source_encoding = decode_text(raw)
    return text.splitlines(), source_encoding


def full_to_half(text: str) -> str:
    """将全角字符转换为半角字符。"""
    chars: list[str] = []

    for char in text:
        code = ord(char)
        if code == 0x3000:
            code = 0x0020
        elif 0xFF01 <= code <= 0xFF5E:
            code -= 0xFEE0
        chars.append(chr(code))

    return "".join(chars)


def normalize_word(raw_text: str, converter: OpenCC) -> str:
    """
    将原始词条归一化。

    处理顺序：
    1. 去首尾空白；
    2. 繁体转简体；
    3. 全角转半角；
    4. 转小写。
    """
    word = raw_text.strip()
    word = converter.convert(word)
    word = full_to_half(word).lower()
    return word


def should_skip_source_line(line: str) -> bool:
    """判断原始输入行是否应直接跳过。"""
    stripped = line.strip()
    return not stripped or stripped.startswith("#") or stripped.startswith("//")


def has_url(text: str) -> bool:
    """判断文本中是否包含 URL。"""
    return URL_RE.search(text) is not None


def is_pure_number(text: str) -> bool:
    """判断文本是否为纯数字。"""
    return PURE_NUMBER_RE.fullmatch(text.strip()) is not None


def has_ip_address(text: str) -> bool:
    """判断文本中是否包含 IP 地址形式。"""
    return IP_RE.search(text) is not None


def is_symbol_or_digit_only(text: str) -> bool:
    """判断文本是否仅由数字、符号或下划线构成。"""
    return SYMBOL_AND_DIGIT_ONLY_RE.fullmatch(text) is not None


def is_useless_pattern(word: str) -> bool:
    """
    判断归一化后的词条是否属于无效词条。

    过滤规则：
    - 纯数字
    - 含 URL
    - 含 IP 地址
    - 纯符号串
    - 数字过长的数字/符号混合串
    """
    stripped = word.strip()
    if not stripped:
        return True

    if is_pure_number(stripped):
        return True

    if has_url(stripped):
        return True

    if has_ip_address(stripped):
        return True

    if is_symbol_or_digit_only(stripped):
        digits_count = sum(1 for ch in stripped if ch.isdigit())

        if digits_count == 0:
            return True

        if digits_count > 4:
            return True

    return False


def should_keep_word(word: str, max_length: int) -> bool:
    """统一判断归一化后的词条是否应保留。"""
    if not word:
        return False

    if len(word) > max_length:
        return False

    if is_useless_pattern(word):
        return False

    return True


def sort_words(words: set[str], reverse: bool = False) -> list[str]:
    """按词条内容排序，可选倒序。"""
    return sorted(words, reverse=reverse)


def write_output(output_file: Path, words: list[str], max_length: int) -> None:
    """将结果词条写入输出文件。"""
    with open(output_file, "w", encoding=OUTPUT_ENCODING) as f:
        f.write("# Optimized Sensitive Word Dictionary\n")
        f.write(f"# Auto-generated, Max Length: {max_length}\n\n")
        for word in words:
            f.write(f"{word}\n")


def optimize_dictionary(
    input_file: Path,
    output_file: Path,
    max_length: int = DEFAULT_MAX_LENGTH,
    reverse: bool = False,
) -> None:
    """执行词库清洗、去重、排序和输出。"""
    converter = OpenCC("t2s")

    print(f"正在读取文件: {input_file}")
    try:
        lines, source_encoding = read_input_lines(input_file)
        print(f"检测到输入编码: {source_encoding}")
    except Exception as exc:
        print(f"文件读取失败: {exc}")
        return

    original_count = len(lines)
    unique_words: set[str] = set()

    print("正在进行归一化清洗与精简...")
    for line in lines:
        if should_skip_source_line(line):
            continue

        normalized = normalize_word(line, converter)
        if not should_keep_word(normalized, max_length):
            continue

        unique_words.add(normalized)

    sorted_words = sort_words(unique_words, reverse=reverse)
    optimized_count = len(sorted_words)

    print(f"清洗完成！准备写入: {output_file}")
    write_output(output_file, sorted_words, max_length)

    print("-" * 40)
    print("【精简统计报告】")
    print(f"原始记录数: {original_count:>8} 条")
    print(f"清洗后词条: {optimized_count:>8} 条")

    if original_count > 0:
        reduced_count = original_count - optimized_count
        reduced_ratio = reduced_count / original_count * 100
        print(f"共计削减了: {reduced_count:>8} 条 (冗余率: {reduced_ratio:.2f}%)")

    print("-" * 40)


def parse_args() -> argparse.Namespace:
    """解析命令行参数。"""
    parser = argparse.ArgumentParser(description="敏感词库高级清洗与精简脚本")
    parser.add_argument("input", help="输入词库文件路径")
    parser.add_argument(
        "-o",
        "--output",
        default=DEFAULT_OUTPUT,
        help=f"输出词库文件路径 (默认: {DEFAULT_OUTPUT})",
    )
    parser.add_argument(
        "--max-len",
        type=int,
        default=DEFAULT_MAX_LENGTH,
        help=f"保留的最大词条长度 (默认: {DEFAULT_MAX_LENGTH})",
    )
    parser.add_argument(
        "--reverse",
        action="store_true",
        help="按倒序输出词条。",
    )
    return parser.parse_args()


def main() -> int:
    """程序入口。"""
    args = parse_args()

    input_path = Path(args.input)
    output_path = Path(args.output)

    if not input_path.exists():
        print(f"错误: 找不到输入文件 {input_path}")
        return 1

    if not input_path.is_file():
        print(f"错误: 输入路径不是文件 {input_path}")
        return 1

    optimize_dictionary(
        input_path,
        output_path,
        max_length=args.max_len,
        reverse=args.reverse,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
