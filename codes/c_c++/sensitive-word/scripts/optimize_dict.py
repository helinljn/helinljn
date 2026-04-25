#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
敏感词库清洗、精简与分析脚本。

处理流程：
1. 自动检测输入文件编码并读取文本；
2. 跳过空行与注释行；
3. 对词条做归一化：
   - 繁体转简体
   - 全角转半角
   - 转小写
4. 去掉词条中的特殊符号；
5. 过滤无效词条：
   - 纯数字
   - 含 URL
   - 含 IP 地址
   - 纯符号串
   - 数字过长的数字/符号混合串
   - 过短或过长词条
6. 对保留词条去重并排序；
7. 以 UTF-8 无 BOM 输出优化后的词库；
8. 可选输出分析报告：
   - 归一化碰撞报告
   - 包含关系候选报告
   - 结构化统计 JSON

说明：
- 本脚本面向“敏感词词库”场景，不是通用文本处理工具。
- 目标不只是基础清洗，也用于辅助发现词库中的归一化冗余和覆盖关系。
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from collections import defaultdict
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, Iterable, Optional

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


DEFAULT_MIN_LENGTH = 1
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
SPECIAL_CHAR_RE = re.compile(r"[^\w\u4e00-\u9fff]+", re.UNICODE)


@dataclass
class OptimizationStats:
    """词库优化过程中的统计信息。"""

    input_lines: int = 0
    skipped_source_lines: int = 0
    kept_words_before_dedup: int = 0
    unique_words: int = 0

    filtered_too_short: int = 0
    filtered_too_long: int = 0
    filtered_pure_number: int = 0
    filtered_url: int = 0
    filtered_ip: int = 0
    filtered_pure_symbol: int = 0
    filtered_long_digit_symbol_mix: int = 0
    filtered_empty_after_normalization: int = 0

    normalization_collision_groups: int = 0
    normalization_collision_entries: int = 0

    contains_candidate_groups: int = 0
    contains_candidate_pairs: int = 0

    def to_dict(self) -> dict:
        """导出为普通字典，方便打印或写入 JSON。"""
        return {
            "input_lines": self.input_lines,
            "skipped_source_lines": self.skipped_source_lines,
            "kept_words_before_dedup": self.kept_words_before_dedup,
            "unique_words": self.unique_words,
            "filtered_too_short": self.filtered_too_short,
            "filtered_too_long": self.filtered_too_long,
            "filtered_pure_number": self.filtered_pure_number,
            "filtered_url": self.filtered_url,
            "filtered_ip": self.filtered_ip,
            "filtered_pure_symbol": self.filtered_pure_symbol,
            "filtered_long_digit_symbol_mix": self.filtered_long_digit_symbol_mix,
            "filtered_empty_after_normalization": self.filtered_empty_after_normalization,
            "normalization_collision_groups": self.normalization_collision_groups,
            "normalization_collision_entries": self.normalization_collision_entries,
            "contains_candidate_groups": self.contains_candidate_groups,
            "contains_candidate_pairs": self.contains_candidate_pairs,
        }


@dataclass
class OptimizationContext:
    """优化过程中的中间结果与输出数据。"""

    stats: OptimizationStats = field(default_factory=OptimizationStats)
    normalized_to_raws: Dict[str, list[str]] = field(default_factory=lambda: defaultdict(list))
    unique_words: set[str] = field(default_factory=set)


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


def strip_special_chars(text: str) -> str:
    """
    去掉词条中的特殊符号。

    默认保留：
    - 中文
    - 字母
    - 数字
    - 下划线（属于 \\w 范围）

    说明：
    - 该步骤用于折叠“插入符号规避”的变体形式；
    - 放在归一化之后执行，可确保全/半角符号先被统一。
    """
    return SPECIAL_CHAR_RE.sub("", text)


def normalize_word(raw_text: str, converter: OpenCC) -> str:
    """
    将原始词条归一化。

    处理顺序：
    1. 去首尾空白；
    2. 繁体转简体；
    3. 全角转半角；
    4. 转小写；
    5. 去掉特殊符号；
    6. 再次去首尾空白。
    """
    word = raw_text.strip()
    word = converter.convert(word)
    word = full_to_half(word).lower()
    word = strip_special_chars(word)
    word = word.strip()
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


def classify_useless_pattern(word: str) -> Optional[str]:
    """
    判断归一化后的词条是否属于无效模式。

    返回值：
    - None: 词条有效
    - 其他字符串: 具体过滤原因
    """
    stripped = word.strip()
    if not stripped:
        return "empty_after_normalization"

    if is_pure_number(stripped):
        return "pure_number"

    if has_url(stripped):
        return "url"

    if has_ip_address(stripped):
        return "ip"

    if is_symbol_or_digit_only(stripped):
        digits_count = sum(1 for ch in stripped if ch.isdigit())

        if digits_count == 0:
            return "pure_symbol"

        if digits_count > 4:
            return "long_digit_symbol_mix"

    return None


def should_keep_word(word: str, min_length: int, max_length: int) -> tuple[bool, Optional[str]]:
    """统一判断归一化后的词条是否应保留，并返回过滤原因。"""
    if not word:
        return False, "empty_after_normalization"

    if len(word) < min_length:
        return False, "too_short"

    if len(word) > max_length:
        return False, "too_long"

    useless_reason = classify_useless_pattern(word)
    if useless_reason is not None:
        return False, useless_reason

    return True, None


def record_filter_reason(stats: OptimizationStats, reason: Optional[str]) -> None:
    """按过滤原因更新统计。"""
    if reason == "too_short":
        stats.filtered_too_short += 1
    elif reason == "too_long":
        stats.filtered_too_long += 1
    elif reason == "pure_number":
        stats.filtered_pure_number += 1
    elif reason == "url":
        stats.filtered_url += 1
    elif reason == "ip":
        stats.filtered_ip += 1
    elif reason == "pure_symbol":
        stats.filtered_pure_symbol += 1
    elif reason == "long_digit_symbol_mix":
        stats.filtered_long_digit_symbol_mix += 1
    elif reason == "empty_after_normalization":
        stats.filtered_empty_after_normalization += 1


def sort_words(words: Iterable[str], reverse: bool = False) -> list[str]:
    """按词条内容排序，可选倒序。"""
    return sorted(words, reverse=reverse)


def write_output(output_file: Path, words: list[str], min_length: int, max_length: int) -> None:
    """将结果词条写入输出文件。"""
    with open(output_file, "w", encoding=OUTPUT_ENCODING, newline="") as f:
        f.write("# Optimized Sensitive Word Dictionary\n")
        f.write(
            f"# Auto-generated, Min Length: {min_length}, Max Length: {max_length}\n\n"
        )
        for word in words:
            f.write(f"{word}\n")


def build_normalization_collision_groups(
    normalized_to_raws: Dict[str, list[str]]
) -> dict[str, list[str]]:
    """
    构建归一化碰撞分组。

    仅保留“多个不同原始词条归一化后落到同一个 normalized key”的组。
    """
    groups: dict[str, list[str]] = {}

    for normalized, raws in normalized_to_raws.items():
        unique_raws = sorted(set(raws))
        if len(unique_raws) > 1:
            groups[normalized] = unique_raws

    return groups


def build_contains_candidates(
    words: set[str],
    min_subword_length: int = 2,
) -> dict[str, list[str]]:
    """
    构建“包含关系候选”报告。

    语义：
    - 若词条 A 包含词条 B（A != B，且 B 也存在于词库），则把 B 视为 A 的一个子词候选。
    - 该报告只做分析，不直接删除任何词条。

    复杂度：
    - 通过枚举单个词条的子串来避免 O(n^2) 词条两两比较。
    - 当词条最大长度较小（例如 10）时，该策略对 8W 级词库仍可接受。
    """
    result: dict[str, list[str]] = {}
    word_set = set(words)

    for word in sorted(word_set, key=lambda item: (len(item), item)):
        if len(word) <= min_subword_length:
            continue

        subwords: set[str] = set()
        word_length = len(word)

        for start in range(word_length):
            for end in range(start + min_subword_length, word_length + 1):
                if start == 0 and end == word_length:
                    continue

                sub = word[start:end]
                if sub == word:
                    continue

                if sub in word_set:
                    subwords.add(sub)

        if subwords:
            result[word] = sorted(subwords, key=lambda item: (len(item), item))

    return result


def write_normalization_collision_report(
    report_file: Path,
    collision_groups: dict[str, list[str]],
) -> None:
    """输出归一化碰撞报告。"""
    with open(report_file, "w", encoding=OUTPUT_ENCODING, newline="") as f:
        f.write("# Normalization Collision Report\n")
        f.write(f"# groups={len(collision_groups)}\n\n")

        for normalized in sorted(collision_groups):
            raws = collision_groups[normalized]
            f.write(f"[normalized] {normalized}\n")
            for raw in raws:
                f.write(f"  - {raw}\n")
            f.write("\n")


def write_contains_candidate_report(
    report_file: Path,
    contains_candidates: dict[str, list[str]],
) -> None:
    """输出包含关系候选报告。"""
    with open(report_file, "w", encoding=OUTPUT_ENCODING, newline="") as f:
        f.write("# Contains Candidate Report\n")
        f.write(f"# groups={len(contains_candidates)}\n\n")

        for word in sorted(contains_candidates, key=lambda item: (len(item), item)):
            subs = contains_candidates[word]
            f.write(f"[word] {word}\n")
            for sub in subs:
                f.write(f"  - contains: {sub}\n")
            f.write("\n")


def write_stats_json(stats_file: Path, stats: OptimizationStats) -> None:
    """输出结构化统计 JSON。"""
    stats_file.write_text(
        json.dumps(stats.to_dict(), ensure_ascii=False, indent=2),
        encoding=OUTPUT_ENCODING,
        newline="",
    )


def ensure_report_dir(report_dir: Optional[str]) -> Optional[Path]:
    """准备报告输出目录。"""
    if not report_dir:
        return None

    path = Path(report_dir)
    path.mkdir(parents=True, exist_ok=True)
    return path


def optimize_dictionary(
    input_file: Path,
    output_file: Path,
    min_length: int = DEFAULT_MIN_LENGTH,
    max_length: int = DEFAULT_MAX_LENGTH,
    reverse: bool = False,
    report_dir: Optional[str] = None,
    report_normalization_collisions: bool = False,
    report_contains_candidates: bool = False,
    contains_min_subword_length: int = 2,
    stats_json: Optional[str] = None,
) -> None:
    """执行词库清洗、去重、排序、统计与分析报告输出。"""
    converter = OpenCC("t2s")
    context = OptimizationContext()

    print(f"正在读取文件: {input_file}")
    try:
        lines, source_encoding = read_input_lines(input_file)
        print(f"检测到输入编码: {source_encoding}")
    except Exception as exc:
        print(f"文件读取失败: {exc}")
        return

    context.stats.input_lines = len(lines)
    report_path = ensure_report_dir(report_dir)

    print("正在进行归一化清洗与精简...")
    for line in lines:
        if should_skip_source_line(line):
            context.stats.skipped_source_lines += 1
            continue

        raw_word = line.strip()
        normalized = normalize_word(line, converter)
        keep, reason = should_keep_word(normalized, min_length, max_length)

        if not keep:
            record_filter_reason(context.stats, reason)
            continue

        context.stats.kept_words_before_dedup += 1
        context.normalized_to_raws[normalized].append(raw_word)
        context.unique_words.add(normalized)

    sorted_words = sort_words(context.unique_words, reverse=reverse)
    context.stats.unique_words = len(sorted_words)

    print(f"清洗完成！准备写入: {output_file}")
    write_output(output_file, sorted_words, min_length, max_length)

    if report_path is not None:
        print(f"正在输出分析报告目录: {report_path}")

        if report_normalization_collisions:
            collision_groups = build_normalization_collision_groups(
                context.normalized_to_raws
            )
            context.stats.normalization_collision_groups = len(collision_groups)
            context.stats.normalization_collision_entries = sum(
                len(raws) for raws in collision_groups.values()
            )
            write_normalization_collision_report(
                report_path / "normalization_collisions.txt",
                collision_groups,
            )

        if report_contains_candidates:
            contains_candidates = build_contains_candidates(
                context.unique_words,
                min_subword_length=contains_min_subword_length,
            )
            context.stats.contains_candidate_groups = len(contains_candidates)
            context.stats.contains_candidate_pairs = sum(
                len(items) for items in contains_candidates.values()
            )
            write_contains_candidate_report(
                report_path / "contains_candidates.txt",
                contains_candidates,
            )

    if stats_json:
        stats_path = Path(stats_json)
        stats_path.parent.mkdir(parents=True, exist_ok=True)
        write_stats_json(stats_path, context.stats)

    print("-" * 40)
    print("【精简统计报告】")
    print(f"原始记录数: {context.stats.input_lines:>8} 条")
    print(f"跳过注释/空行: {context.stats.skipped_source_lines:>8} 条")
    print(f"保留(去重前): {context.stats.kept_words_before_dedup:>8} 条")
    print(f"清洗后词条: {context.stats.unique_words:>8} 条")

    total_filtered = (
        context.stats.filtered_too_short
        + context.stats.filtered_too_long
        + context.stats.filtered_pure_number
        + context.stats.filtered_url
        + context.stats.filtered_ip
        + context.stats.filtered_pure_symbol
        + context.stats.filtered_long_digit_symbol_mix
        + context.stats.filtered_empty_after_normalization
    )

    print(f"过滤词条总数: {total_filtered:>8} 条")
    print(f"  - 过短词条: {context.stats.filtered_too_short:>8} 条")
    print(f"  - 过长词条: {context.stats.filtered_too_long:>8} 条")
    print(f"  - 纯数字词条: {context.stats.filtered_pure_number:>6} 条")
    print(f"  - 含 URL 词条: {context.stats.filtered_url:>8} 条")
    print(f"  - 含 IP 词条: {context.stats.filtered_ip:>9} 条")
    print(f"  - 纯符号词条: {context.stats.filtered_pure_symbol:>6} 条")
    print(
        f"  - 长数字/符号混合词条: "
        f"{context.stats.filtered_long_digit_symbol_mix:>4} 条"
    )
    print(
        f"  - 归一化后为空: "
        f"{context.stats.filtered_empty_after_normalization:>8} 条"
    )

    dedup_reduction = context.stats.kept_words_before_dedup - context.stats.unique_words
    print(f"归一化去重减少: {dedup_reduction:>8} 条")

    if context.stats.input_lines > 0:
        reduced_count = context.stats.input_lines - context.stats.unique_words
        reduced_ratio = reduced_count / context.stats.input_lines * 100
        print(f"总削减数量: {reduced_count:>8} 条 (压缩率: {reduced_ratio:.2f}%)")

    if report_normalization_collisions:
        print(
            f"归一化碰撞组数: {context.stats.normalization_collision_groups:>6} 组，"
            f"涉及原始词条 {context.stats.normalization_collision_entries:>6} 条"
        )

    if report_contains_candidates:
        print(
            f"包含关系候选: {context.stats.contains_candidate_groups:>8} 组，"
            f"候选关系 {context.stats.contains_candidate_pairs:>8} 条"
        )

    print("-" * 40)


def parse_args() -> argparse.Namespace:
    """解析命令行参数。"""
    parser = argparse.ArgumentParser(description="敏感词库清洗、精简与分析脚本")
    parser.add_argument("input", help="输入词库文件路径")
    parser.add_argument(
        "-o",
        "--output",
        default=DEFAULT_OUTPUT,
        help=f"输出词库文件路径 (默认: {DEFAULT_OUTPUT})",
    )
    parser.add_argument(
        "--min-len",
        type=int,
        default=DEFAULT_MIN_LENGTH,
        help=f"保留的最小词条长度 (默认: {DEFAULT_MIN_LENGTH})",
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
    parser.add_argument(
        "--report-dir",
        default=None,
        help="分析报告输出目录。",
    )
    parser.add_argument(
        "--report-normalization-collisions",
        action="store_true",
        help="输出归一化碰撞报告。",
    )
    parser.add_argument(
        "--report-contains-candidates",
        action="store_true",
        help="输出包含关系候选报告。",
    )
    parser.add_argument(
        "--contains-min-subword-len",
        type=int,
        default=2,
        help="包含关系分析时，参与判断的最短子词长度 (默认: 2)",
    )
    parser.add_argument(
        "--stats-json",
        default=None,
        help="将统计结果输出为 JSON 文件。",
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

    if args.min_len < 1:
        print("错误: --min-len 不能小于 1")
        return 1

    if args.max_len < args.min_len:
        print("错误: --max-len 不能小于 --min-len")
        return 1

    if args.contains_min_subword_len < 1:
        print("错误: --contains-min-subword-len 不能小于 1")
        return 1

    optimize_dictionary(
        input_path,
        output_path,
        min_length=args.min_len,
        max_length=args.max_len,
        reverse=args.reverse,
        report_dir=args.report_dir,
        report_normalization_collisions=args.report_normalization_collisions,
        report_contains_candidates=args.report_contains_candidates,
        contains_min_subword_length=args.contains_min_subword_len,
        stats_json=args.stats_json,
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
