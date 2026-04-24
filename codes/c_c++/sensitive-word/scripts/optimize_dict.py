#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
敏感词库高级清洗与精简脚本

利用 C++ 引擎运行时强大的归一化能力（大小写折叠、全半角折叠、繁简转换等），
在离线阶段对冗余词库进行“降维打击”。

功能：
1. 繁简转换：将繁体中文统一转换为简体。
2. 全半角转换与小写：将全角字母数字转为半角，并转为小写。
3. 长度过滤：移除过长的词条（默认 > 10 个字符），过长的长句命中率极低且浪费内存。
4. 无效模式过滤：移除纯数字、纯网址等无意义词汇。
5. 深度去重：基于归一化后的词根进行精确去重。
"""

import argparse
import re
import sys
from pathlib import Path

try:
    from opencc import OpenCC
except ImportError as exc:
    raise SystemExit(
        "缺少依赖：opencc\n"
        "建议安装：pip install opencc-python-reimplemented"
    ) from exc

def full_to_half(text: str) -> str:
    """全角转半角"""
    chars = []
    for char in text:
        code = ord(char)
        if code == 0x3000:
            code = 0x0020
        elif 0xFF01 <= code <= 0xFF5E:
            code -= 0xFEE0
        chars.append(chr(code))
    return "".join(chars)

def is_useless_pattern(text: str) -> bool:
    """检查是否为无效模式（纯数字、网址、IP等）"""

    # 1 & 2. 检查是否只包含数字和特殊符号（无英文字母、无中文等）
    # \d 匹配数字，\W 匹配非单词字符（标点、空格等），_ 匹配下划线
    if re.match(r'^[\d\W_]+$', text):
        # 统计其中的数字个数
        digits_count = sum(1 for c in text if c.isdigit())
        # 如果完全没有数字，说明是纯符号串，过滤
        if digits_count == 0:
            return True
        # 如果数字个数超过 4 个（例如电话号码、长QQ号、群号等），过滤
        if digits_count > 4:
            return True
        # 数字个数 <= 4 的纯数字/符号组合（如 110, 911, 1-2），保留继续向下检测

    # 3. 过滤 IP 格式 (如 192.168.1.1)
    if re.search(r'\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}', text):
        return True

    # 4. 简单的 URL 匹配 (黑产经常在词库里塞大量具体的引流网址)
    if re.match(r'^(https?://|www\.)[a-zA-Z0-9_-]+(\.[a-zA-Z0-9_-]+)+', text):
        return True

    return False

def optimize_dictionary(
    input_file: Path,
    output_file: Path,
    max_length: int = 10
) -> None:
    converter = OpenCC('t2s')

    print(f"正在读取文件: {input_file}")
    try:
        with open(input_file, 'r', encoding='utf-8-sig') as f:
            lines = f.readlines()
    except UnicodeDecodeError:
        try:
            # 尝试兼容国标编码
            with open(input_file, 'r', encoding='gb18030') as f:
                lines = f.readlines()
        except Exception as e:
            print(f"文件读取失败: {e}")
            return

    original_count = len(lines)
    unique_words = set()

    print("正在进行归一化清洗与精简...")
    for line in lines:
        line = line.strip()
        # 跳过空行和注释
        if not line or line.startswith('#') or line.startswith('//'):
            continue

        # 1. 繁体转简体
        word = converter.convert(line)

        # 2. 全角转半角，大写转小写
        word = full_to_half(word).lower()

        # 3. 长度截断
        if len(word) > max_length:
            continue

        # 4. 无效模式过滤
        if is_useless_pattern(word):
            continue

        unique_words.add(word)

    optimized_count = len(unique_words)

    # 排序输出以保证字典的稳定性
    sorted_words = sorted(list(unique_words))

    print(f"清洗完成！准备写入: {output_file}")
    with open(output_file, 'w', encoding='utf-8') as f:
        # 在文件头加入注释说明
        f.write("# Optimized Sensitive Word Dictionary\n")
        f.write(f"# Auto-generated, Max Length: {max_length}\n\n")
        for word in sorted_words:
            f.write(f"{word}\n")

    print("-" * 40)
    print("【精简统计报告】")
    print(f"原始记录数: {original_count:>8} 条")
    print(f"清洗后词条: {optimized_count:>8} 条")

    if original_count > 0:
        reduced_ratio = (original_count - optimized_count) / original_count * 100
        print(f"共计削减了: {original_count - optimized_count:>8} 条 (冗余率: {reduced_ratio:.2f}%)")
    print("-" * 40)

def main():
    parser = argparse.ArgumentParser(description="敏感词库高级清洗与精简脚本")
    parser.add_argument("input", help="输入词库文件路径")
    parser.add_argument("-o", "--output", default="optimized_dict.txt", help="输出词库文件路径 (默认: optimized_dict.txt)")
    parser.add_argument("--max-len", type=int, default=10, help="保留的最大词条长度 (默认: 10)")

    args = parser.parse_args()

    input_path = Path(args.input)
    output_path = Path(args.output)

    if not input_path.exists():
        print(f"错误: 找不到输入文件 {input_path}")
        sys.exit(1)

    optimize_dictionary(input_path, output_path, args.max_len)

if __name__ == "__main__":
    main()
