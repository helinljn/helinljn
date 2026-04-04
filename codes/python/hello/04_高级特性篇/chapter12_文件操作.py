# =============================================================================
# 第 12 章：文件操作
# =============================================================================
#
# 【学习目标】
#   1. 掌握文件的打开、读取、写入、关闭
#   2. 理解文本模式 vs 二进制模式
#   3. 掌握 with 语句（上下文管理器）
#   4. 学会处理不同编码的文件
#   5. 掌握文件指针操作（seek/tell）
#   6. 了解 pathlib 模块（现代路径操作）
#   7. 掌握常见文件格式（JSON、CSV）
#
# 【与 C++ 的对比】
#   C++:  fstream file("data.txt"); file >> data;
#   Python: with open("data.txt") as f: data = f.read()
#   Python 的 with 语句自动管理资源，更安全
#
# 【运行方式】
#   python chapter12_文件操作.py
#
# =============================================================================

import os
import json
import csv
from pathlib import Path
from typing import Any


# =============================================================================
# 12.1 文件基础操作
# =============================================================================

def demo_file_basics() -> None:
    """演示文件的基本读写操作。"""
    print("=" * 60)
    print("12.1 文件基础操作")
    print("=" * 60)

    # ── 写入文件 ─────────────────────────────────────────
    print("1. 写入文件:")
    filename = "test_file.txt"

    # 方式1：手动关闭（不推荐）
    # file = open(filename, "w", encoding="utf-8")
    # file.write("Hello, World!\n")
    # file.close()

    # 方式2：with 语句（推荐，自动关闭）
    with open(filename, "w", encoding="utf-8") as f:
        f.write("第一行\n")
        f.write("第二行\n")
        f.write("第三行\n")
    print(f"  ✓ 已写入 {filename}")

    # ── 读取整个文件 ─────────────────────────────────────
    print("\n2. 读取整个文件:")
    with open(filename, "r", encoding="utf-8") as f:
        content = f.read()
        print(f"  内容:\n{content}")

    # ── 按行读取 ─────────────────────────────────────────
    print("3. 按行读取:")
    with open(filename, "r", encoding="utf-8") as f:
        for i, line in enumerate(f, 1):
            # line 包含换行符，用 rstrip() 去除
            print(f"  行{i}: {line.rstrip()!r}")

    # ── 读取所有行到列表 ─────────────────────────────────
    print("\n4. 读取所有行到列表:")
    with open(filename, "r", encoding="utf-8") as f:
        lines = f.readlines()
        print(f"  共 {len(lines)} 行")
        for i, line in enumerate(lines, 1):
            print(f"    {i}. {line.rstrip()}")

    # ── 追加内容 ─────────────────────────────────────────
    print("\n5. 追加内容:")
    with open(filename, "a", encoding="utf-8") as f:
        f.write("追加的第四行\n")
    print(f"  ✓ 已追加内容")

    with open(filename, "r", encoding="utf-8") as f:
        print(f"  更新后的内容:\n{f.read()}")

    # 清理
    os.remove(filename)


# =============================================================================
# 12.2 文件模式详解
# =============================================================================

def demo_file_modes() -> None:
    """演示不同的文件打开模式。"""
    print("\n" + "=" * 60)
    print("12.2 文件模式详解")
    print("=" * 60)

    print("常用文件模式:")
    modes = [
        ("r",   "只读（默认），文件必须存在"),
        ("w",   "写入，覆盖已有内容，文件不存在则创建"),
        ("a",   "追加，文件不存在则创建"),
        ("x",   "独占创建，文件已存在则报错"),
        ("r+",  "读写，文件必须存在"),
        ("w+",  "读写，覆盖已有内容"),
        ("a+",  "读写，追加模式"),
        ("rb",  "二进制只读"),
        ("wb",  "二进制写入"),
        ("ab",  "二进制追加"),
    ]
    for mode, desc in modes:
        print(f"  {mode:4s} - {desc}")

    # ── 演示 x 模式（独占创建）───────────────────────────
    print("\n演示 'x' 模式（独占创建）:")
    test_file = "exclusive_test.txt"

    # 第一次创建成功
    try:
        with open(test_file, "x", encoding="utf-8") as f:
            f.write("独占创建的文件\n")
        print(f"  ✓ 首次创建成功")
    except FileExistsError:
        print(f"  文件已存在")

    # 第二次创建失败
    try:
        with open(test_file, "x", encoding="utf-8") as f:
            f.write("不会执行\n")
    except FileExistsError as e:
        print(f"  ✓ 再次创建失败: {e}")

    os.remove(test_file)


# =============================================================================
# 12.3 二进制文件操作
# =============================================================================

def demo_binary_files() -> None:
    """演示二进制文件操作。"""
    print("\n" + "=" * 60)
    print("12.3 二进制文件操作")
    print("=" * 60)

    # ── 写入二进制数据 ───────────────────────────────────
    print("1. 写入二进制数据:")
    binary_file = "test_binary.bin"

    data = bytes([0x48, 0x65, 0x6C, 0x6C, 0x6F])  # "Hello" 的 ASCII
    with open(binary_file, "wb") as f:
        f.write(data)
        f.write(b"\x00\xFF\xAA")  # 直接写入字节
    print(f"  ✓ 已写入 {len(data) + 3} 字节")

    # ── 读取二进制数据 ───────────────────────────────────
    print("\n2. 读取二进制数据:")
    with open(binary_file, "rb") as f:
        content = f.read()
        print(f"  读取 {len(content)} 字节:")
        print(f"    十六进制: {content.hex()}")
        print(f"    字节列表: {list(content)}")
        # 前5个字节是 ASCII 文本
        print(f"    文本部分: {content[:5].decode('ascii')!r}")

    # ── 复制二进制文件 ───────────────────────────────────
    print("\n3. 复制二进制文件:")
    copy_file = "test_binary_copy.bin"
    with open(binary_file, "rb") as src:
        with open(copy_file, "wb") as dst:
            dst.write(src.read())
    print(f"  ✓ 已复制到 {copy_file}")

    # 清理
    os.remove(binary_file)
    os.remove(copy_file)


# =============================================================================
# 12.4 文件指针操作（seek/tell）
# =============================================================================

def demo_file_pointer() -> None:
    """演示文件指针操作。"""
    print("\n" + "=" * 60)
    print("12.4 文件指针操作")
    print("=" * 60)

    filename = "test_seek.txt"
    content = "0123456789ABCDEFGHIJ"

    # 创建测试文件
    with open(filename, "w", encoding="utf-8") as f:
        f.write(content)

    print(f"文件内容: {content!r}")
    print(f"文件大小: {len(content)} 字节\n")

    # 注意：文本模式下 seek 只支持 seek(0) 或 tell() 返回的绝对位置，
    # 不支持非零的末端相对定位（whence=2），因此改用二进制模式演示。
    with open(filename, "rb") as f:
        # tell() 返回当前位置
        print(f"1. 初始位置: {f.tell()}")

        # 读取5个字节
        data = f.read(5)
        print(f"2. 读取5个字节: {data.decode()!r}, 当前位置: {f.tell()}")

        # seek(offset, whence)
        # whence: 0=从头开始, 1=从当前位置, 2=从末尾
        f.seek(0)  # 回到开头
        print(f"3. seek(0) 后位置: {f.tell()}")

        f.seek(10)  # 跳到第10个字节
        data = f.read(5)
        print(f"4. seek(10) 后读取: {data.decode()!r}")

        # 跳到末尾
        f.seek(0, 2)
        print(f"5. seek(0, 2) 到末尾: {f.tell()}")

        # 从末尾往前5个字节（二进制模式支持此操作）
        f.seek(-5, 2)
        data = f.read()
        print(f"6. seek(-5, 2) 后读取: {data.decode()!r}")

    os.remove(filename)


# =============================================================================
# 12.5 文件编码处理
# =============================================================================

def demo_file_encoding() -> None:
    """演示文件编码处理。"""
    print("\n" + "=" * 60)
    print("12.5 文件编码处理")
    print("=" * 60)

    # ── UTF-8 编码（推荐）────────────────────────────────
    print("1. UTF-8 编码:")
    utf8_file = "test_utf8.txt"
    text = "你好，世界！Hello, World! 🐍"

    with open(utf8_file, "w", encoding="utf-8") as f:
        f.write(text)
    print(f"  ✓ 写入 UTF-8: {text!r}")

    with open(utf8_file, "r", encoding="utf-8") as f:
        content = f.read()
        print(f"  ✓ 读取 UTF-8: {content!r}")

    # ── GBK 编码 ─────────────────────────────────────────
    print("\n2. GBK 编码:")
    gbk_file = "test_gbk.txt"
    text_cn = "中文测试"

    with open(gbk_file, "w", encoding="gbk") as f:
        f.write(text_cn)
    print(f"  ✓ 写入 GBK: {text_cn!r}")

    with open(gbk_file, "r", encoding="gbk") as f:
        content = f.read()
        print(f"  ✓ 读取 GBK: {content!r}")

    # ── 编码错误处理 ─────────────────────────────────────
    print("\n3. 编码错误处理:")
    try:
        # 用错误的编码读取 UTF-8 文件
        with open(utf8_file, "r", encoding="ascii") as f:
            f.read()
    except UnicodeDecodeError as e:
        print(f"  ✓ UnicodeDecodeError: 读取编码错误")

    # errors 参数：处理编码错误的策略
    with open(utf8_file, "r", encoding="ascii", errors="ignore") as f:
        content = f.read()
        print(f"  errors='ignore': {content!r}")

    with open(utf8_file, "r", encoding="ascii", errors="replace") as f:
        content = f.read()
        print(f"  errors='replace': {content!r}")

    # 清理
    os.remove(utf8_file)
    os.remove(gbk_file)


# =============================================================================
# 12.6 pathlib 模块（现代路径操作）
# =============================================================================

def demo_pathlib() -> None:
    """演示 pathlib 模块。"""
    print("\n" + "=" * 60)
    print("12.6 pathlib 模块")
    print("=" * 60)

    # ── 基本路径操作 ──────────────────────────────────────
    print("1. 基本路径操作:")
    p = Path("some/dir/file.txt")
    print(f"  路径:    {p}")
    print(f"  父目录:  {p.parent}")
    print(f"  文件名:  {p.name}")
    print(f"  主文件名: {p.stem}")
    print(f"  扩展名:  {p.suffix}")
    print(f"  各组件:  {p.parts}")

    # ── 路径拼接 ──────────────────────────────────────────
    print("\n2. 路径拼接:")
    base = Path("data")
    sub = base / "2024" / "report.csv"  # / 运算符拼接路径
    print(f"  base / '2024' / 'report.csv' = {sub}")

    # ── 当前目录和绝对路径 ────────────────────────────────
    print("\n3. 当前目录:")
    cwd = Path.cwd()
    print(f"  当前目录: {cwd}")

    home = Path.home()
    print(f"  用户主目录: {home}")

    # ── 创建目录 ──────────────────────────────────────────
    print("\n4. 创建目录和文件:")
    test_dir = Path("test_pathlib_dir") / "subdir"
    test_dir.mkdir(parents=True, exist_ok=True)
    print(f"  ✓ 创建目录: {test_dir}")

    test_file = test_dir / "hello.txt"
    test_file.write_text("Hello, pathlib!\n你好", encoding="utf-8")
    print(f"  ✓ 创建文件: {test_file}")

    # ── 读取文件 ──────────────────────────────────────────
    content = test_file.read_text(encoding="utf-8")
    print(f"  读取内容: {content!r}")

    # ── 文件信息 ──────────────────────────────────────────
    print("\n5. 文件信息:")
    print(f"  exists(): {test_file.exists()}")
    print(f"  is_file(): {test_file.is_file()}")
    print(f"  is_dir(): {test_dir.is_dir()}")
    stat = test_file.stat()
    print(f"  文件大小: {stat.st_size} 字节")

    # ── 遍历目录 ──────────────────────────────────────────
    print("\n6. 遍历目录:")
    root = Path("test_pathlib_dir")

    # 创建几个测试文件
    for name in ["a.txt", "b.py", "c.txt"]:
        (root / name).write_text(f"内容: {name}")

    print("  iterdir() - 列出目录内容:")
    for item in sorted(root.iterdir()):
        tag = "[目录]" if item.is_dir() else "[文件]"
        print(f"    {tag} {item.name}")

    print("  glob('*.txt') - 匹配 txt 文件:")
    for txt_file in sorted(root.glob("*.txt")):
        print(f"    {txt_file.name}")

    print("  rglob('*.txt') - 递归匹配 txt 文件:")
    for txt_file in sorted(root.rglob("*.txt")):
        print(f"    {txt_file.relative_to(root)}")

    # ── 重命名和删除 ──────────────────────────────────────
    print("\n7. 重命名:")
    old = root / "a.txt"
    new = root / "renamed.txt"
    old.rename(new)
    print(f"  ✓ {old.name} → {new.name}")

    # 清理测试目录
    import shutil
    shutil.rmtree(root)
    print(f"  ✓ 清理完毕")


# =============================================================================
# 12.7 JSON 文件操作
# =============================================================================

def demo_json_files() -> None:
    """演示 JSON 文件的读写。"""
    print("\n" + "=" * 60)
    print("12.7 JSON 文件操作")
    print("=" * 60)

    # ── 写入 JSON ─────────────────────────────────────────
    print("1. 写入 JSON 文件:")
    data: dict[str, Any] = {
        "app_name": "MyApp",
        "version": "1.0.0",
        "debug": True,
        "max_workers": 4,
        "database": {
            "host": "localhost",
            "port": 5432,
            "name": "myapp_db",
        },
        "allowed_hosts": ["127.0.0.1", "localhost"],
        "tags": None,
    }

    json_file = "test_config.json"
    with open(json_file, "w", encoding="utf-8") as f:
        # indent=2 使 JSON 格式化，ensure_ascii=False 支持中文
        json.dump(data, f, indent=2, ensure_ascii=False)
    print(f"  ✓ 已写入 {json_file}")

    # ── 读取 JSON ─────────────────────────────────────────
    print("\n2. 读取 JSON 文件:")
    with open(json_file, "r", encoding="utf-8") as f:
        loaded: dict = json.load(f)

    print(f"  app_name: {loaded['app_name']}")
    print(f"  version:  {loaded['version']}")
    print(f"  debug:    {loaded['debug']} (类型: {type(loaded['debug']).__name__})")
    print(f"  database: {loaded['database']}")
    print(f"  hosts:    {loaded['allowed_hosts']}")
    print(f"  tags:     {loaded['tags']} (类型: {type(loaded['tags']).__name__})")

    # ── 字符串与 JSON 互转 ────────────────────────────────
    print("\n3. 字符串与 JSON 互转:")
    obj = {"name": "张三", "age": 30, "scores": [95, 87, 92]}

    # dict → JSON 字符串
    json_str = json.dumps(obj, ensure_ascii=False)
    print(f"  dumps: {json_str!r}")

    # JSON 字符串 → dict
    parsed = json.loads(json_str)
    print(f"  loads: {parsed}")

    # ── 处理 JSON 错误 ────────────────────────────────────
    print("\n4. 处理 JSON 错误:")
    bad_json = '{"name": "test", invalid}'
    try:
        json.loads(bad_json)
    except json.JSONDecodeError as e:
        print(f"  ✓ JSONDecodeError: {e}")

    os.remove(json_file)


# =============================================================================
# 12.8 CSV 文件操作
# =============================================================================

def demo_csv_files() -> None:
    """演示 CSV 文件的读写。"""
    print("\n" + "=" * 60)
    print("12.8 CSV 文件操作")
    print("=" * 60)

    csv_file = "test_students.csv"

    # ── 写入 CSV ──────────────────────────────────────────
    print("1. 写入 CSV 文件:")
    students = [
        ["姓名", "年龄", "成绩", "班级"],
        ["张三", 18, 95.5, "A班"],
        ["李四", 19, 87.0, "B班"],
        ["王五", 17, 92.3, "A班"],
        ["赵六", 18, 78.8, "C班"],
    ]

    # newline="" 避免 Windows 上出现多余空行
    with open(csv_file, "w", newline="", encoding="utf-8-sig") as f:
        # utf-8-sig 带 BOM，Excel 打开不会乱码
        writer = csv.writer(f)
        writer.writerows(students)  # 一次写入多行
    print(f"  ✓ 已写入 {len(students) - 1} 条记录")

    # ── 读取 CSV ──────────────────────────────────────────
    print("\n2. 读取 CSV 文件:")
    with open(csv_file, "r", encoding="utf-8-sig") as f:
        reader = csv.reader(f)
        header = next(reader)  # 读取表头
        print(f"  表头: {header}")
        for row in reader:
            print(f"  行: {row}")

    # ── 使用 DictReader/DictWriter ────────────────────────
    print("\n3. 使用 DictReader（字典格式）:")
    with open(csv_file, "r", encoding="utf-8-sig") as f:
        reader = csv.DictReader(f)
        for row in reader:
            # 每行是一个 dict，键是列名
            print(f"  {row['姓名']}: 年龄={row['年龄']}, 成绩={row['成绩']}")

    print("\n4. 使用 DictWriter 写入:")
    csv_file2 = "test_employees.csv"
    fieldnames = ["id", "name", "department", "salary"]
    employees = [
        {"id": 1, "name": "Alice", "department": "Engineering", "salary": 80000},
        {"id": 2, "name": "Bob",   "department": "Marketing",   "salary": 65000},
        {"id": 3, "name": "Carol", "department": "Engineering", "salary": 90000},
    ]

    with open(csv_file2, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()     # 写入表头
        writer.writerows(employees)
    print(f"  ✓ 已写入 {len(employees)} 条员工记录")

    with open(csv_file2, "r", encoding="utf-8") as f:
        print(f"  文件内容:\n{f.read()}")

    # 清理
    os.remove(csv_file)
    os.remove(csv_file2)


# =============================================================================
# 12.9 综合示例：日志文件分析器
# =============================================================================

def demo_log_analyzer() -> None:
    """综合示例：分析日志文件。"""
    print("\n" + "=" * 60)
    print("12.9 综合示例：日志文件分析器")
    print("=" * 60)

    # 生成模拟日志文件
    log_file = "test_app.log"
    log_content = """\
2024-01-15 08:01:23 INFO  服务器启动，监听端口 8080
2024-01-15 08:05:11 INFO  用户 alice 登录成功
2024-01-15 08:06:33 DEBUG 查询数据库: SELECT * FROM users
2024-01-15 08:07:45 WARNING 请求超时，重试中...
2024-01-15 08:08:12 ERROR 数据库连接失败: Connection refused
2024-01-15 08:09:01 INFO  用户 bob 登录成功
2024-01-15 08:10:55 ERROR 用户 alice 权限不足，访问被拒绝
2024-01-15 08:11:30 INFO  缓存刷新完成
2024-01-15 08:12:44 WARNING 内存使用率达到 85%
2024-01-15 08:13:02 ERROR 文件 /data/report.pdf 不存在
2024-01-15 08:14:18 INFO  定时任务执行完成
2024-01-15 08:15:00 INFO  服务器正常关闭
"""

    with open(log_file, "w", encoding="utf-8") as f:
        f.write(log_content)
    print(f"已生成测试日志（{log_content.count(chr(10))} 行）\n")

    # 分析日志
    stats: dict[str, int] = {"INFO": 0, "DEBUG": 0, "WARNING": 0, "ERROR": 0}
    errors: list[str] = []
    warnings: list[str] = []

    with open(log_file, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            parts = line.split(maxsplit=3)
            if len(parts) < 4:
                continue
            date, time_str, level, message = parts
            if level in stats:
                stats[level] += 1
            if level == "ERROR":
                errors.append(f"  [{date} {time_str}] {message}")
            elif level == "WARNING":
                warnings.append(f"  [{date} {time_str}] {message}")

    # 输出统计
    print("日志统计:")
    total = sum(stats.values())
    for level, count in stats.items():
        bar = "█" * count
        print(f"  {level:8s} {count:3d} {bar}")
    print(f"  {'总计':8s} {total:3d}")

    print(f"\n错误列表 ({len(errors)} 条):")
    for err in errors:
        print(err)

    print(f"\n警告列表 ({len(warnings)} 条):")
    for warn in warnings:
        print(warn)

    # 将分析结果保存为 JSON
    result_file = "test_log_report.json"
    report = {
        "stats": stats,
        "error_count": len(errors),
        "warning_count": len(warnings),
        "errors": errors,
        "warnings": warnings,
    }
    with open(result_file, "w", encoding="utf-8") as f:
        json.dump(report, f, indent=2, ensure_ascii=False)
    print(f"\n✓ 分析报告已保存至 {result_file}")

    # 清理
    os.remove(log_file)
    os.remove(result_file)


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_file_basics()
    demo_file_modes()
    demo_binary_files()
    demo_file_pointer()
    demo_file_encoding()
    demo_pathlib()
    demo_json_files()
    demo_csv_files()
    demo_log_analyzer()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 打开文件 ──
# open(file, mode='r', encoding=None, errors=None, newline=None)
#
# ── 读取方法 ──
# f.read()          读取全部内容（字符串）
# f.read(n)         读取 n 个字符
# f.readline()      读取一行
# f.readlines()     读取所有行，返回列表
# for line in f:    逐行迭代（推荐，内存高效）
#
# ── 写入方法 ──
# f.write(str)      写入字符串
# f.writelines(lst) 写入字符串列表（不自动加换行）
# f.flush()         刷新缓冲区
#
# ── 指针操作 ──
# f.tell()          返回当前位置
# f.seek(offset, whence=0)  移动指针
#
# ── pathlib 常用操作 ──
# Path(str)         创建路径对象
# p / "subdir"      路径拼接
# p.parent          父目录
# p.name            文件名
# p.stem            主文件名（无扩展名）
# p.suffix          扩展名
# p.exists()        是否存在
# p.is_file()       是否是文件
# p.is_dir()        是否是目录
# p.mkdir(parents=True, exist_ok=True)  创建目录
# p.read_text(encoding)     读取文本
# p.write_text(str, encoding) 写入文本
# p.glob(pattern)   模式匹配
# p.rglob(pattern)  递归模式匹配
# p.stat()          文件统计信息


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：忘记关闭文件（内存泄漏）
# f = open("file.txt")  # ❌ 没有 with
# data = f.read()
# # 如果发生异常，文件不会被关闭
# 解决：始终使用 with open(...) as f:
#
# 错误 2：CSV 文件在 Windows 出现空行
# with open("file.csv", "w") as f:  # ❌ 缺少 newline=""
# 解决：with open("file.csv", "w", newline="") as f:
#
# 错误 3：中文乱码
# with open("file.txt", "r") as f:  # ❌ 没有指定编码
# 解决：with open("file.txt", "r", encoding="utf-8") as f:
#
# 错误 4：seek 在文本模式下的限制
# # 文本模式下，seek 只能使用 seek(0) 或 f.seek(n) 其中 n 来自 f.tell()
# # 二进制模式没有此限制


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   写一个函数 word_count(filename)：
#   读取文本文件，统计文件中每个单词出现的次数
#   返回 {word: count} 字典，按出现次数降序排列
#
# 练习 2：
#   写一个函数 merge_csv(files, output)：
#   合并多个 CSV 文件（假设有相同的列），写入到 output 文件
#   处理表头重复的问题（只写一次）
#
# 练习 3：
#   使用 pathlib 写一个函数 find_large_files(directory, min_size_kb)：
#   递归查找目录下大于 min_size_kb 的文件
#   返回 (文件路径, 大小) 的列表，按大小降序
#
# 练习答案提示：
#   练习1：用 split() 分词，用 dict 或 Counter 计数，用 sorted() 排序
#   练习2：用 csv.DictReader 读，写第一个文件时写表头，后续跳过
#   练习3：用 p.rglob('*')，用 p.stat().st_size，用 sorted() 排序


# =============================================================================
# 【练习答案】
# =============================================================================

def word_count(filename: str) -> dict[str, int]:
    """
    练习 1：统计文本文件中每个单词出现的次数。

    读取文本文件，将内容拆分为单词（以空白字符分隔），
    忽略大小写和常见标点，返回按出现次数降序排列的字典。

    Args:
        filename: 要统计的文件路径

    Returns:
        {word: count} 字典，按出现次数降序排列
    """
    import re
    from collections import Counter

    with open(filename, "r", encoding="utf-8") as f:
        content = f.read()

    # 用正则提取单词（字母和数字组成），统一转小写
    words = re.findall(r'\b\w+\b', content.lower())

    # Counter 自动计数
    counter = Counter(words)

    # 按出现次数降序排列，返回普通 dict
    return dict(sorted(counter.items(), key=lambda x: x[1], reverse=True))


# ── 练习 2：合并 CSV 文件 ────────────────────────────────────────────────────

def merge_csv(files: list[str], output: str) -> int:
    """
    练习 2：合并多个 CSV 文件（相同列结构）到一个输出文件。

    只写入一次表头，后续文件的表头行自动跳过。

    Args:
        files:  要合并的 CSV 文件路径列表
        output: 输出文件路径

    Returns:
        合并后的数据行总数（不含表头）
    """
    total_rows = 0

    with open(output, "w", newline="", encoding="utf-8") as out_f:
        writer = None

        for idx, filepath in enumerate(files):
            with open(filepath, "r", encoding="utf-8") as in_f:
                reader = csv.DictReader(in_f)

                # 第一个文件：初始化 writer 并写入表头
                if idx == 0:
                    writer = csv.DictWriter(out_f, fieldnames=reader.fieldnames)
                    writer.writeheader()

                # 写入数据行（自动跳过表头）
                for row in reader:
                    writer.writerow(row)
                    total_rows += 1

    return total_rows


# ── 练习 3：查找大文件 ───────────────────────────────────────────────────────

def find_large_files(directory: str, min_size_kb: float) -> list[tuple[Path, int]]:
    """
    练习 3：递归查找目录下大于指定大小的文件。

    使用 pathlib 递归遍历目录，筛选出大于 min_size_kb 的文件，
    返回按文件大小降序排列的 (文件路径, 大小字节数) 列表。

    Args:
        directory:    要搜索的目录路径
        min_size_kb:  最小文件大小（KB）

    Returns:
        [(Path对象, 文件字节数), ...] 按大小降序排列
    """
    min_size_bytes = min_size_kb * 1024
    root = Path(directory)

    results: list[tuple[Path, int]] = []

    for item in root.rglob("*"):
        # 只处理文件（跳过目录、符号链接等）
        if not item.is_file():
            continue
        try:
            size = item.stat().st_size
            if size >= min_size_bytes:
                results.append((item, size))
        except (PermissionError, OSError):
            # 跳过无法访问的文件
            continue

    # 按文件大小降序排列
    results.sort(key=lambda x: x[1], reverse=True)
    return results


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 word_count 函数。"""
    print("word_count 演示:")

    # 创建测试文件
    test_file = "test_word_count.txt"
    content = """\
Python is a great language. Python is easy to learn.
Python is used for web development, data science, and automation.
Learning Python is fun. Python Python Python!
The quick brown fox jumps over the lazy dog.
"""
    with open(test_file, "w", encoding="utf-8") as f:
        f.write(content)

    result = word_count(test_file)

    print(f"  文件共 {sum(result.values())} 个单词，{len(result)} 种不同单词")
    print("  出现次数最多的前 10 个单词:")
    for word, count in list(result.items())[:10]:
        bar = "█" * count
        print(f"    {word:12s} {count:3d} {bar}")

    # 清理
    os.remove(test_file)


def exercise2_answer() -> None:
    """练习 2：演示 merge_csv 函数。"""
    print("merge_csv 演示:")

    # 创建测试 CSV 文件
    file1 = "test_part1.csv"
    file2 = "test_part2.csv"
    file3 = "test_part3.csv"
    output = "test_merged.csv"

    with open(file1, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=["id", "name", "score"])
        writer.writeheader()
        writer.writerows([
            {"id": 1, "name": "张三", "score": 95},
            {"id": 2, "name": "李四", "score": 87},
        ])

    with open(file2, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=["id", "name", "score"])
        writer.writeheader()
        writer.writerows([
            {"id": 3, "name": "王五", "score": 92},
            {"id": 4, "name": "赵六", "score": 78},
        ])

    with open(file3, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=["id", "name", "score"])
        writer.writeheader()
        writer.writerows([
            {"id": 5, "name": "孙七", "score": 88},
        ])

    total = merge_csv([file1, file2, file3], output)
    print(f"  ✓ 合并 3 个文件，共 {total} 条数据记录")

    print("  合并后的文件内容:")
    with open(output, "r", encoding="utf-8") as f:
        for line in f:
            print(f"    {line.rstrip()}")

    # 清理
    for fname in [file1, file2, file3, output]:
        os.remove(fname)


def exercise3_answer() -> None:
    """练习 3：演示 find_large_files 函数。"""
    print("find_large_files 演示:")

    # 创建测试目录结构
    test_dir = Path("test_find_files")
    test_dir.mkdir(exist_ok=True)
    (test_dir / "sub").mkdir(exist_ok=True)

    # 创建不同大小的测试文件
    files_info = [
        (test_dir / "small.txt",        500),      # ~500 B
        (test_dir / "medium.py",        2048),     # ~2 KB
        (test_dir / "large.log",        5120),     # ~5 KB
        (test_dir / "sub" / "data.csv", 1024),     # ~1 KB
        (test_dir / "sub" / "huge.bin", 10240),    # ~10 KB
    ]

    for path, size in files_info:
        path.write_bytes(b"x" * size)
        print(f"  创建: {path.relative_to(test_dir)} ({size / 1024:.1f} KB)")

    print(f"\n  查找大于 1 KB 的文件:")
    results = find_large_files(str(test_dir), min_size_kb=1)
    for file_path, size in results:
        rel = file_path.relative_to(test_dir)
        print(f"    {str(rel):20s}  {size / 1024:6.1f} KB")

    print(f"\n  查找大于 3 KB 的文件:")
    results = find_large_files(str(test_dir), min_size_kb=3)
    if results:
        for file_path, size in results:
            rel = file_path.relative_to(test_dir)
            print(f"    {str(rel):20s}  {size / 1024:6.1f} KB")
    else:
        print("    （无匹配文件）")

    # 清理
    import shutil
    shutil.rmtree(test_dir)
    print(f"\n  ✓ 清理完毕")


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 40)
#     exercise1_answer()
#
#     print("=" * 40)
#     exercise2_answer()
#
#     print("=" * 40)
#     exercise3_answer()
