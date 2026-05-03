#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Python 学习工程 — 交互式章节导航

运行方式:
    python main.py
"""

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).parent

# 章节列表：(显示名, 文件路径, 阶段)
CHAPTERS: list[tuple[str, str, str]] = [
    # ── 第一阶段：基础 ──
    ("ch01 入门与环境",            "01_基础篇/chapter01_入门与环境.py",                 "基础"),
    ("ch02 基础语法与数据类型",    "01_基础篇/chapter02_基础语法与数据类型.py",         "基础"),
    ("ch03 控制流程",              "01_基础篇/chapter03_控制流程.py",                   "基础"),
    ("ch04 字符串处理",            "01_基础篇/chapter04_字符串处理.py",                 "基础"),
    ("ch05 容器数据类型",          "01_基础篇/chapter05_容器数据类型.py",               "基础"),
    # ── 第二阶段：函数 ──
    ("ch06 函数基础",              "02_函数篇/chapter06_函数基础.py",                   "函数"),
    ("ch07 函数进阶",              "02_函数篇/chapter07_函数进阶.py",                   "函数"),
    # ── 第三阶段：OOP ──
    ("ch08 模块与包",              "03_模块与面向对象篇/chapter08_模块与包.py",         "OOP"),
    ("ch09 面向对象基础",          "03_模块与面向对象篇/chapter09_面向对象基础.py",     "OOP"),
    ("ch10 面向对象进阶",          "03_模块与面向对象篇/chapter10_面向对象进阶.py",     "OOP"),
    ("ch11 异常处理",              "03_模块与面向对象篇/chapter11_异常处理.py",         "OOP"),
    # ── 第四阶段：高级特性 ──
    ("ch12 文件操作",              "04_高级特性篇/chapter12_文件操作.py",               "高级"),
    ("ch13 迭代器与生成器",        "04_高级特性篇/chapter13_迭代器与生成器.py",         "高级"),
    ("ch14 装饰器详解",            "04_高级特性篇/chapter14_装饰器详解.py",             "高级"),
    # ── 第五阶段：标准库 ──
    ("ch15 系统与环境",            "05_标准库篇/chapter15_系统与环境.py",               "标准库"),
    ("ch16 文本处理",              "05_标准库篇/chapter16_文本处理.py",                 "标准库"),
    ("ch17 日期与时间",            "05_标准库篇/chapter17_日期与时间.py",               "标准库"),
    ("ch18 数据结构工具",          "05_标准库篇/chapter18_数据结构工具.py",             "标准库"),
    ("ch19 函数式编程工具",        "05_标准库篇/chapter19_函数式编程工具.py",           "标准库"),
    ("ch20 正则表达式",            "05_标准库篇/chapter20_正则表达式.py",               "标准库"),
    ("ch21 日志与调试",            "05_标准库篇/chapter21_日志与调试.py",               "标准库"),
    # 补充章节
    ("补充01 网络与HTTP基础",       "05_标准库篇/chapter21_补充01_网络与HTTP基础.py",   "标准库补充"),
    ("补充02 数学与数值工具",       "05_标准库篇/chapter21_补充02_数学与数值工具.py",   "标准库补充"),
    ("补充03 文件目录工具",         "05_标准库篇/chapter21_补充03_文件目录工具.py",     "标准库补充"),
    ("补充04 类型注解进阶",         "05_标准库篇/chapter21_补充04_类型注解进阶.py",     "标准库补充"),
    ("补充05 struct二进制数据处理", "05_标准库篇/chapter21_补充05_struct二进制数据处理.py", "标准库补充"),
    ("补充06 hashlib哈希与摘要",    "05_标准库篇/chapter21_补充06_hashlib哈希与摘要.py","标准库补充"),
    ("补充07 bytes与bytearray",     "05_标准库篇/chapter21_补充07_bytes与bytearray深入.py","标准库补充"),
    ("补充08 enum枚举",             "05_标准库篇/chapter21_补充08_enum枚举.py",         "标准库补充"),
    ("补充09 uuid唯一标识符",       "05_标准库篇/chapter21_补充09_uuid唯一标识符.py",   "标准库补充"),
    ("补充10 ctypes调用C动态库",    "05_标准库篇/chapter21_补充10_ctypes调用C动态库.py","标准库补充"),
    # ── 第六阶段：并发与数据库 ──
    ("ch22 并发编程入门",           "06_并发与数据库/chapter22_并发编程入门.py",        "并发"),
    ("ch23 数据库操作",             "06_并发与数据库/chapter23_数据库操作.py",          "并发"),
    # ── 练习 ──
    ("练习01 基础篇",               "练习题与答案/exercises_01_基础.py",                "练习"),
    ("练习02 函数篇",               "练习题与答案/exercises_02_函数.py",                "练习"),
    ("练习03 面向对象",             "练习题与答案/exercises_03_面向对象.py",            "练习"),
    ("练习04 综合篇",               "练习题与答案/exercises_04_综合.py",                "练习"),
]


def run_chapter(path: str) -> None:
    """运行单个章节。"""
    full = ROOT / path
    if not full.exists():
        print(f"  ✗ 文件不存在: {full}")
        return
    print(f"\n{'=' * 60}")
    print(f"  运行: {Path(path).name}")
    print(f"{'=' * 60}\n")
    subprocess.run([sys.executable, "-X", "utf8", str(full)])


def show_numlist(chapters: list[tuple[str, str, str]], start_idx: int) -> None:
    """显示编号列表。"""
    for i, (name, path, stage) in enumerate(chapters, start=start_idx):
        marker = ""
        full = ROOT / path
        if not full.exists():
            marker = "  [缺]"
        print(f"  {i:2d}. {name}{marker}")


def main() -> None:
    print("=" * 60)
    print("  Python 学习工程 — 交互式章节导航")
    print("=" * 60)
    print()
    print("  阶段:")
    print("    [1] 基础 (ch01-05)")
    print("    [2] 函数 (ch06-07)")
    print("    [3] OOP  (ch08-11)")
    print("    [4] 高级 (ch12-14)")
    print("    [5] 标准库 (ch15-21 + 补充01-10)")
    print("    [6] 并发与数据库 (ch22-23)")
    print("    [L] 列出全部章节")
    print("    [A] 运行全部教学章节")
    print("    [Q] 退出")
    print()

    while True:
        try:
            cmd = input("  > ").strip().upper()
        except (EOFError, KeyboardInterrupt):
            print("\n  再见!")
            break

        if cmd in ("Q", "QUIT", "EXIT"):
            print("  再见!")
            break

        elif cmd == "1":
            print()
            show_numlist(CHAPTERS[:5], 1)
            print()
        elif cmd == "2":
            print()
            show_numlist(CHAPTERS[5:7], 6)
            print()
        elif cmd == "3":
            print()
            show_numlist(CHAPTERS[7:11], 8)
            print()
        elif cmd == "4":
            print()
            show_numlist(CHAPTERS[11:14], 12)
            print()
        elif cmd == "5":
            print()
            show_numlist(CHAPTERS[14:30], 15)
            print()
        elif cmd == "6":
            print()
            show_numlist(CHAPTERS[30:33], 31)
            print()
        elif cmd == "L":
            print()
            show_numlist(CHAPTERS, 1)
            print()

        elif cmd == "A":
            for name, path, _ in CHAPTERS[:33]:  # 教学章节 (ch01~ch23 + 补充01~10)
                run_chapter(path)

        elif cmd.isdigit():
            idx = int(cmd) - 1
            if 0 <= idx < len(CHAPTERS):
                run_chapter(CHAPTERS[idx][1])
            else:
                print(f"  无效编号 (1-{len(CHAPTERS)}), 输入 L 查看全部")

        else:
            print("  输入: 1-6 (阶段) / L (全部) / A (运行) / 编号 (单个) / Q (退出)")


if __name__ == "__main__":
    main()
