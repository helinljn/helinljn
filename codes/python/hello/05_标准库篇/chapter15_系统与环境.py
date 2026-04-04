# =============================================================================
# 第 15 章：系统与环境（sys、os、platform、subprocess、argparse）
# =============================================================================
#
# 【学习目标】
#   1. 掌握 sys 模块：系统参数、标准流、退出控制
#   2. 掌握 os 模块：文件系统操作、环境变量、进程管理
#   3. 了解 platform 模块：获取平台信息
#   4. 掌握 subprocess 模块：执行外部命令和子进程管理
#   5. 掌握 argparse 模块：构建专业的命令行工具
#
# 【这些模块解决什么问题？】
#   sys:        与 Python 解释器交互，获取系统参数
#   os:         与操作系统交互，进行文件和进程操作
#   platform:   获取平台和系统信息
#   subprocess: 执行外部命令，替代 os.system
#   argparse:   解析命令行参数，构建 CLI 工具
#
# 【与 C/C++ 的对比】
#   C/C++:  需要使用 argc/argv、getenv()、system()、fork() 等底层 API
#   Python: 提供了更高级、跨平台的封装，使用更简单
#
# 【运行方式】
#   python chapter15_系统与环境.py
#   python chapter15_系统与环境.py --help
#   python chapter15_系统与环境.py --name 张三 --age 25
#
# =============================================================================

import sys
import os
import platform
import subprocess
import argparse
from pathlib import Path
from typing import List, Optional, Any


# =============================================================================
# 15.1 sys 模块：系统参数与标准流
# =============================================================================

def demo_sys_module() -> None:
    """演示 sys 模块的常用功能。"""
    print("=" * 60)
    print("15.1 sys 模块：系统参数与标准流")
    print("=" * 60)

    # ── 命令行参数 ───────────────────────────────────────
    print("1. 命令行参数（sys.argv）:")
    print(f"  脚本名称: {sys.argv[0]}")
    print(f"  所有参数: {sys.argv}")
    print(f"  参数个数: {len(sys.argv)}")
    if len(sys.argv) > 1:
        print(f"  第一个参数: {sys.argv[1]}")
    print()

    # ── Python 版本信息 ──────────────────────────────────
    print("2. Python 版本信息:")
    print(f"  版本字符串: {sys.version}")
    print(f"  版本元组:   {sys.version_info}")
    print(f"  主版本号:   {sys.version_info.major}")
    print(f"  次版本号:   {sys.version_info.minor}")
    print(f"  微版本号:   {sys.version_info.micro}")
    print()

    # ── 模块搜索路径 ─────────────────────────────────────
    print("3. 模块搜索路径（sys.path）:")
    print("  Python 会在以下路径中搜索模块:")
    for i, path in enumerate(sys.path[:5], 1):  # 只显示前5个
        print(f"    {i}. {path}")
    print(f"  ... 共 {len(sys.path)} 个路径")
    print()

    # ── 平台信息 ─────────────────────────────────────────
    print("4. 平台信息:")
    print(f"  操作系统:     {sys.platform}")
    print(f"  字节序:       {sys.byteorder}")
    print(f"  最大整数:     {sys.maxsize}")
    print(f"  默认编码:     {sys.getdefaultencoding()}")
    print()

    # ── 标准输入输出流 ───────────────────────────────────
    print("5. 标准流（stdin, stdout, stderr）:")
    print(f"  标准输入:  {sys.stdin}")
    print(f"  标准输出:  {sys.stdout}")
    print(f"  标准错误:  {sys.stderr}")

    # 写入标准错误流
    sys.stderr.write("  [错误示例] 这是一条错误消息\n")
    sys.stderr.flush()
    print()

    # ── 退出程序 ─────────────────────────────────────────
    print("6. 退出程序（sys.exit）:")
    print("  sys.exit(0)   # 正常退出")
    print("  sys.exit(1)   # 异常退出")
    print("  sys.exit('错误消息')  # 打印消息并退出")
    print("  注意：本演示不会真的退出\n")

    # ── 递归深度限制 ─────────────────────────────────────
    print("7. 递归深度限制:")
    print(f"  当前递归深度限制: {sys.getrecursionlimit()}")
    print("  可以用 sys.setrecursionlimit(n) 修改（谨慎使用）")
    print()

    # ── 引用计数 ─────────────────────────────────────────
    print("8. 对象引用计数:")
    obj = [1, 2, 3]
    print(f"  对象: {obj}")
    print(f"  引用计数: {sys.getrefcount(obj)}")  # 会比实际多1（函数参数）
    another_ref = obj
    print(f"  增加引用后: {sys.getrefcount(obj)}")


# =============================================================================
# 15.2 os 模块：操作系统接口
# =============================================================================

def demo_os_module() -> None:
    """演示 os 模块的常用功能。"""
    print("\n" + "=" * 60)
    print("15.2 os 模块：操作系统接口")
    print("=" * 60)

    # ── 环境变量 ─────────────────────────────────────────
    print("1. 环境变量（os.environ）:")
    print(f"  PATH: {os.environ.get('PATH', '未设置')[:60]}...")
    print(f"  HOME/USERPROFILE: {os.environ.get('HOME') or os.environ.get('USERPROFILE')}")

    # 设置临时环境变量
    os.environ['MY_VAR'] = 'Hello'
    print(f"  自定义变量 MY_VAR: {os.environ['MY_VAR']}")
    print()

    # ── 当前工作目录 ─────────────────────────────────────
    print("2. 当前工作目录:")
    cwd = os.getcwd()
    print(f"  当前目录: {cwd}")
    print()

    # ── 文件和目录操作 ───────────────────────────────────
    print("3. 文件和目录操作:")

    # 创建目录
    test_dir = "temp_test_dir"
    if not os.path.exists(test_dir):
        os.mkdir(test_dir)
        print(f"  ✓ 创建目录: {test_dir}")

    # 检查路径
    print(f"  目录是否存在: {os.path.exists(test_dir)}")
    print(f"  是否为目录:   {os.path.isdir(test_dir)}")
    print(f"  是否为文件:   {os.path.isfile(test_dir)}")

    # 创建文件
    test_file = os.path.join(test_dir, "test.txt")
    with open(test_file, 'w', encoding='utf-8') as f:
        f.write("测试内容")
    print(f"  ✓ 创建文件: {test_file}")

    # 列出目录内容
    print(f"  目录内容: {os.listdir(test_dir)}")

    # 重命名
    new_file = os.path.join(test_dir, "renamed.txt")
    os.rename(test_file, new_file)
    print(f"  ✓ 重命名: test.txt → renamed.txt")

    # 删除文件和目录
    os.remove(new_file)
    os.rmdir(test_dir)
    print(f"  ✓ 清理完成")
    print()

    # ── 路径操作 ─────────────────────────────────────────
    print("4. 路径操作（os.path）:")
    path = "/home/user/documents/file.txt"
    print(f"  原始路径:   {path}")
    print(f"  目录名:     {os.path.dirname(path)}")
    print(f"  文件名:     {os.path.basename(path)}")
    print(f"  分离:       {os.path.split(path)}")
    print(f"  扩展名:     {os.path.splitext(path)}")
    print(f"  拼接路径:   {os.path.join('dir', 'subdir', 'file.txt')}")
    print(f"  绝对路径:   {os.path.abspath('.')}")
    print()

    # ── 进程相关 ─────────────────────────────────────────
    print("5. 进程相关:")
    print(f"  当前进程 ID: {os.getpid()}")
    print(f"  父进程 ID:   {os.getppid()}")
    print()

    # ── 系统信息 ─────────────────────────────────────────
    print("6. 系统信息:")
    print(f"  操作系统名称: {os.name}")  # 'posix', 'nt', 'java'
    if hasattr(os, 'uname'):
        uname = os.uname()
        print(f"  系统详情:     {uname.sysname} {uname.release}")


# =============================================================================
# 15.3 platform 模块：平台信息
# =============================================================================

def demo_platform_module() -> None:
    """演示 platform 模块的功能。"""
    print("\n" + "=" * 60)
    print("15.3 platform 模块：平台信息")
    print("=" * 60)

    print("1. 系统信息:")
    print(f"  系统:         {platform.system()}")        # Windows, Linux, Darwin
    print(f"  版本:         {platform.release()}")
    print(f"  详细版本:     {platform.version()}")
    print(f"  机器类型:     {platform.machine()}")       # x86_64, AMD64
    print(f"  处理器:       {platform.processor()}")
    print(f"  架构:         {platform.architecture()}")
    print()

    print("2. Python 信息:")
    print(f"  Python 版本:  {platform.python_version()}")
    print(f"  Python 实现:  {platform.python_implementation()}")  # CPython, PyPy
    print(f"  编译器:       {platform.python_compiler()}")
    print()

    print("3. 平台标识:")
    print(f"  平台:         {platform.platform()}")
    print(f"  节点名:       {platform.node()}")


# =============================================================================
# 15.4 subprocess 模块：执行外部命令
# =============================================================================

def demo_subprocess_module() -> None:
    """演示 subprocess 模块的常用功能。"""
    print("\n" + "=" * 60)
    print("15.4 subprocess 模块：执行外部命令")
    print("=" * 60)

    # 根据操作系统选择命令
    is_windows = sys.platform == "win32"

    # ── run（推荐方式）────────────────────────────────────
    print("1. subprocess.run（推荐方式）:")

    # 基本用法
    if is_windows:
        cmd = ["cmd", "/c", "echo", "Hello from subprocess"]
    else:
        cmd = ["echo", "Hello from subprocess"]

    result = subprocess.run(
        cmd,
        capture_output=True,   # 捕获 stdout 和 stderr
        text=True,             # 以字符串形式返回（而不是字节）
        encoding='utf-8',      # 指定编码
    )
    print(f"  返回码:   {result.returncode}")   # 0 表示成功
    print(f"  标准输出: {result.stdout.strip()}")
    print(f"  标准错误: {result.stderr.strip() or '（无）'}")
    print()

    # ── 检查返回码 ────────────────────────────────────────
    print("2. 检查命令是否成功:")

    # check=True 会在命令失败时抛出 CalledProcessError
    try:
        if is_windows:
            fail_cmd = ["cmd", "/c", "exit", "1"]
        else:
            fail_cmd = ["bash", "-c", "exit 1"]
        subprocess.run(fail_cmd, check=True)
    except subprocess.CalledProcessError as e:
        print(f"  ✓ 命令失败，捕获异常: returncode={e.returncode}")
    print()

    # ── 获取命令输出 ─────────────────────────────────────
    print("3. 获取命令输出并处理:")

    if is_windows:
        list_cmd = ["cmd", "/c", "dir", "/b"]
    else:
        list_cmd = ["ls", "-1"]

    try:
        result = subprocess.run(
            list_cmd,
            capture_output=True,
            text=True,
            encoding='utf-8',
            errors='ignore',       # 忽略编码错误（Windows 中文路径）
        )
        if result.returncode == 0:
            lines = result.stdout.strip().split('\n')
            print(f"  当前目录下有 {len(lines)} 个条目")
            for line in lines[:5]:             # 只显示前5个
                print(f"    - {line.strip()}")
            if len(lines) > 5:
                print(f"    ... 还有 {len(lines) - 5} 个")
    except Exception as e:
        print(f"  命令执行失败: {e}")
    print()

    # ── timeout 超时控制 ─────────────────────────────────
    print("4. 超时控制:")
    try:
        if is_windows:
            sleep_cmd = ["cmd", "/c", "timeout", "/t", "10"]
        else:
            sleep_cmd = ["sleep", "10"]
        subprocess.run(sleep_cmd, timeout=0.1)    # 0.1 秒超时
    except subprocess.TimeoutExpired:
        print("  ✓ 命令超时，已捕获 TimeoutExpired 异常")
    print()

    # ── 管道通信（Popen）──────────────────────────────────
    print("5. 使用 Popen 进行管道通信:")
    print("  （适用于需要实时读取输出或双向通信的场景）")

    if is_windows:
        popen_cmd = ["cmd", "/c", "echo", "Popen 测试"]
    else:
        popen_cmd = ["echo", "Popen 测试"]

    with subprocess.Popen(
        popen_cmd,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        encoding='utf-8',
        errors='ignore',
    ) as proc:
        stdout, stderr = proc.communicate(timeout=5)
        print(f"  输出: {stdout.strip()}")
        print(f"  返回码: {proc.returncode}")
    print()

    # ── 安全提示 ─────────────────────────────────────────
    print("6. 安全注意事项:")
    print("  ✓ 推荐：subprocess.run(['ls', '-la'])     # 参数列表形式")
    print("  ✗ 危险：subprocess.run('ls -la', shell=True)  # shell=True 有注入风险")
    print("  如果必须使用 shell=True，请确保输入是可信的！")


# =============================================================================
# 15.5 argparse 模块：命令行参数解析
# =============================================================================

def demo_argparse_module() -> None:
    """演示 argparse 模块的使用。"""
    print("\n" + "=" * 60)
    print("15.5 argparse 模块：命令行参数解析")
    print("=" * 60)

    print("【说明】argparse 示例会在程序直接运行时通过 main() 展示，")
    print("       以下演示各种参数类型的配置方式。\n")

    # ── 基本参数解析器 ────────────────────────────────────
    print("1. 基本解析器配置:")

    # 创建解析器（description 是 --help 时显示的描述）
    parser = argparse.ArgumentParser(
        description="文件处理工具 - 演示 argparse 的各种功能",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="示例:\n  python script.py input.txt -o output.txt -v\n"
               "  python script.py input.txt --count 5 --format json",
    )

    # ── 位置参数（必填）──────────────────────────────────
    parser.add_argument(
        "input",                       # 参数名（位置参数，无 -- 前缀）
        help="输入文件路径",
        type=str,                      # 参数类型，自动转换
    )

    # ── 可选参数 ─────────────────────────────────────────
    parser.add_argument(
        "-o", "--output",              # 短格式和长格式
        help="输出文件路径（默认: 输入文件名加 _out 后缀）",
        type=str,
        default=None,                  # 默认值
    )

    # ── 整数参数 ─────────────────────────────────────────
    parser.add_argument(
        "-n", "--count",
        help="处理行数（默认: 全部）",
        type=int,
        default=0,
        metavar="N",                   # 在帮助信息中显示的参数名
    )

    # ── 选择参数 ─────────────────────────────────────────
    parser.add_argument(
        "--format",
        help="输出格式（默认: text）",
        choices=["text", "json", "csv"],   # 只允许这几个值
        default="text",
    )

    # ── 布尔标志 ─────────────────────────────────────────
    parser.add_argument(
        "-v", "--verbose",
        help="详细输出模式",
        action="store_true",           # 出现此参数则为 True，否则为 False
    )

    # ── 计数参数 ─────────────────────────────────────────
    parser.add_argument(
        "-q",
        help="安静模式（可叠加: -qq 更安静）",
        action="count",               # 每出现一次 +1
        default=0,
        dest="quiet",
    )

    # ── 多值参数 ─────────────────────────────────────────
    parser.add_argument(
        "--exclude",
        help="排除的关键词（可指定多个）",
        nargs="*",                    # 0 个或多个值
        default=[],
        metavar="WORD",
    )

    print("  已配置参数:")
    print("    input          位置参数（必填）")
    print("    -o/--output    输出路径")
    print("    -n/--count     整数参数")
    print("    --format       选择参数（text/json/csv）")
    print("    -v/--verbose   布尔标志")
    print("    -q             计数参数")
    print("    --exclude      多值参数")
    print()

    # ── 手动解析模拟参数 ─────────────────────────────────
    print("2. 模拟解析各种参数:")

    test_cases: List[List[str]] = [
        ["input.txt"],
        ["data.csv", "-o", "result.txt", "-v", "--format", "json"],
        ["log.txt", "-n", "100", "-q", "-q", "--exclude", "DEBUG", "INFO"],
    ]

    for args_list in test_cases:
        args = parser.parse_args(args_list)  # 传入列表，不读取 sys.argv
        print(f"  输入: {' '.join(args_list)}")
        print(f"  解析结果:")
        print(f"    input={args.input!r}, output={args.output!r}")
        print(f"    count={args.count}, format={args.format!r}")
        print(f"    verbose={args.verbose}, quiet={args.quiet}")
        print(f"    exclude={args.exclude}")
        print()

    # ── 子命令（subparsers）──────────────────────────────
    print("3. 子命令解析器（类似 git commit / git push）:")

    tool_parser = argparse.ArgumentParser(description="工具集")
    subparsers = tool_parser.add_subparsers(dest="command", help="子命令")

    # 子命令 1：convert
    convert_parser = subparsers.add_parser("convert", help="格式转换")
    convert_parser.add_argument("file", help="要转换的文件")
    convert_parser.add_argument("--to", choices=["json", "csv", "xml"], required=True)

    # 子命令 2：report
    report_parser = subparsers.add_parser("report", help="生成报告")
    report_parser.add_argument("--date", help="报告日期", default="today")
    report_parser.add_argument("--type", choices=["daily", "weekly"], default="daily")

    for cmd_args in [
        ["convert", "data.csv", "--to", "json"],
        ["report", "--type", "weekly", "--date", "2024-01-01"],
    ]:
        ns = tool_parser.parse_args(cmd_args)
        print(f"  命令: {' '.join(cmd_args)}")
        print(f"  解析: {vars(ns)}")
        print()


# =============================================================================
# 15.6 综合实战：跨平台系统信息工具
# =============================================================================

def demo_real_world() -> None:
    """演示综合实战：系统信息收集工具。"""
    print("\n" + "=" * 60)
    print("15.6 综合实战：系统信息收集工具")
    print("=" * 60)

    def collect_system_info() -> dict[str, Any]:
        """收集系统信息，返回结构化数据。"""
        info: dict[str, Any] = {}

        # 操作系统信息
        info["os"] = {
            "name": platform.system(),
            "release": platform.release(),
            "version": platform.version()[:50],
            "machine": platform.machine(),
            "hostname": platform.node(),
        }

        # Python 信息
        info["python"] = {
            "version": platform.python_version(),
            "implementation": platform.python_implementation(),
            "path": sys.executable,
            "encoding": sys.getdefaultencoding(),
        }

        # 进程信息
        info["process"] = {
            "pid": os.getpid(),
            "cwd": os.getcwd(),
            "script": sys.argv[0],
        }

        # 环境变量（只取部分安全的）
        safe_vars = ["HOME", "USERPROFILE", "USERNAME", "USER", "LANG", "TZ"]
        info["env"] = {
            var: os.environ.get(var, "未设置")
            for var in safe_vars
        }

        return info

    def print_system_report(info: dict[str, Any]) -> None:
        """格式化打印系统信息报告。"""
        print("  ╔══════════════════════════════════════╗")
        print("  ║           系统信息报告               ║")
        print("  ╚══════════════════════════════════════╝")

        sections = {
            "os":      "操作系统",
            "python":  "Python 环境",
            "process": "进程信息",
            "env":     "环境变量",
        }

        for key, title in sections.items():
            print(f"\n  ▶ {title}:")
            for field, value in info[key].items():
                print(f"    {field:<20} {value}")

    # 收集并展示
    system_info = collect_system_info()
    print_system_report(system_info)
    print()

    # ── 运行外部命令获取额外信息 ─────────────────────────
    print("\n  ▶ 外部命令信息:")
    is_windows = sys.platform == "win32"

    commands: dict[str, list[str]] = {}
    if is_windows:
        commands["Python 可执行文件"] = ["python", "--version"]
    else:
        commands["Python 可执行文件"] = ["python3", "--version"]

    for name, cmd in commands.items():
        try:
            result = subprocess.run(
                cmd, capture_output=True, text=True, timeout=5,
                encoding='utf-8', errors='ignore',
            )
            output = (result.stdout + result.stderr).strip()
            print(f"    {name:<20} {output}")
        except (subprocess.TimeoutExpired, FileNotFoundError) as e:
            print(f"    {name:<20} 获取失败: {e}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """主程序：运行所有演示。"""
    # 检查是否有实际命令行参数（排除脚本名本身）
    # 如果用户传入了 --name 等参数，展示 argparse 完整用法
    if len(sys.argv) > 1 and sys.argv[1].startswith("--"):
        # ── 完整 argparse 示例 ──────────────────────────
        parser = argparse.ArgumentParser(
            description="Python 学习示例 - chapter15",
        )
        parser.add_argument("--name", default="学员", help="你的名字")
        parser.add_argument("--age", type=int, default=0, help="你的年龄")
        parser.add_argument("--verbose", action="store_true", help="详细模式")
        args = parser.parse_args()

        print(f"你好, {args.name}!")
        if args.age > 0:
            print(f"你今年 {args.age} 岁。")
        if args.verbose:
            print("已开启详细模式。")
        return

    demo_sys_module()
    demo_os_module()
    demo_platform_module()
    demo_subprocess_module()
    demo_argparse_module()
    demo_real_world()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── sys 常用属性 ──
# sys.argv          命令行参数列表，argv[0] 是脚本名
# sys.path          模块搜索路径列表（可修改）
# sys.version       Python 版本字符串
# sys.version_info  版本信息命名元组
# sys.platform      操作系统标识（'win32', 'linux', 'darwin'）
# sys.stdout/stderr 标准输出/错误流
# sys.exit(code)    退出程序，code=0 正常，非0 异常
# sys.getrefcount() 获取对象引用计数
# sys.getrecursionlimit()  获取递归深度限制
#
# ── os 常用函数 ──
# os.getcwd()            获取当前工作目录
# os.chdir(path)         切换当前工作目录
# os.listdir(path)       列出目录内容
# os.mkdir(path)         创建单层目录
# os.makedirs(path)      递归创建多层目录
# os.remove(path)        删除文件
# os.rmdir(path)         删除空目录
# os.rename(src, dst)    重命名文件/目录
# os.environ             环境变量字典
# os.getpid()            获取当前进程 ID
# os.path.join()         跨平台路径拼接
# os.path.exists()       检查路径是否存在
# os.path.isfile()       检查是否为文件
# os.path.isdir()        检查是否为目录
# os.path.basename()     获取文件名部分
# os.path.dirname()      获取目录部分
# os.path.abspath()      获取绝对路径
#
# ── subprocess.run ──
# subprocess.run(
#     args,              # 命令列表（推荐）或字符串
#     capture_output,    # True = 捕获 stdout 和 stderr
#     text,              # True = 以字符串返回（否则为 bytes）
#     encoding,          # 字符编码
#     check,             # True = 失败时抛出异常
#     timeout,           # 超时秒数
#     shell,             # True = 通过 shell 执行（有安全风险）
# )
# result.returncode      返回码（0 = 成功）
# result.stdout          标准输出内容
# result.stderr          标准错误内容
#
# ── argparse 常用配置 ──
# parser = argparse.ArgumentParser(description="...")
# parser.add_argument("name")              位置参数（必填）
# parser.add_argument("-o", "--output")    可选参数
# parser.add_argument("--count", type=int) 整数类型
# parser.add_argument("--format", choices=[...])  选择参数
# parser.add_argument("--verbose", action="store_true")  布尔标志
# parser.add_argument("--items", nargs="*")  多值参数
# args = parser.parse_args()               解析 sys.argv
# args = parser.parse_args(["--foo", "1"]) 解析指定列表


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：直接修改 sys.argv
# sys.argv[0] = "new_name"   # ❌ 不应直接修改
# 解决：只读取 sys.argv，用 argparse 处理参数
#
# 错误 2：os.mkdir 创建多层目录失败
# os.mkdir("a/b/c")          # ❌ 如果 a/b 不存在会报错
# 解决：os.makedirs("a/b/c", exist_ok=True)  # ✓
#
# 错误 3：subprocess.run 命令作为字符串传入
# subprocess.run("ls -la")   # ❌ 在 shell=False 时无法解析
# 解决：subprocess.run(["ls", "-la"])  # ✓ 用列表
#
# 错误 4：subprocess 忘记处理编码
# result = subprocess.run(cmd, capture_output=True)
# result.stdout.upper()      # ❌ stdout 是 bytes，没有 upper
# 解决：添加 text=True, encoding='utf-8'
#
# 错误 5：argparse 位置参数与可选参数混淆
# parser.add_argument("--file")  # 可选参数，有 -- 前缀
# parser.add_argument("file")    # 位置参数，没有 -- 前缀
# 解决：位置参数不加 --，可选参数加 - 或 --


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   实现一个函数 find_txt_files(directory)：
#   接受目录路径作为参数，返回该目录中所有 .txt 文件的完整路径列表（已排序）
#   使用 os.listdir 和 os.path.splitext 实现，不区分大小写（.TXT 也算）
#   若 directory 不是有效目录，抛出 ValueError
#   例如：
#     # 目录中有：a.txt, b.txt, c.py, d.TXT, e.md
#     find_txt_files("/tmp/test")
#     # ["/tmp/test/a.txt", "/tmp/test/b.txt", "/tmp/test/d.TXT"]
#     find_txt_files("/not/exist")
#     # ValueError: '/not/exist' 不是有效的目录
#
# 练习 2（进阶）：
#   使用 argparse 实现一个类似 Unix head 命令的工具：
#   先实现 build_head_parser() 构建解析器，再实现 head_command(args) 执行逻辑
#   支持以下参数：
#     - input（位置参数）：输入文件路径
#     - --lines / -n（整数）：显示前 n 行，默认 10
#     - --count / -c（布尔标志）：只显示总行数，不显示内容
#   例如：
#     parser = build_head_parser()
#     args = parser.parse_args(["file.txt", "-n", "5"])
#     head_command(args)    # 打印文件前 5 行内容
#     args = parser.parse_args(["file.txt", "--count"])
#     head_command(args)    # 打印: "  共 20 行"
#
# 练习 3（综合）：
#   实现一个函数 run_with_timeout(cmd, timeout_sec)：
#   运行外部命令，成功时返回标准输出字符串；
#   超时时打印警告并返回 None，命令不存在时打印错误并返回 None
#   例如：
#     run_with_timeout(["python", "--version"], 5.0)   # "Python 3.x.x\n"
#     run_with_timeout(["sleep", "100"], 0.1)          # None（超时，打印警告）
#     run_with_timeout(["no_such_cmd"], 5.0)           # None（命令不存在，打印错误）
#
# 练习答案提示：
#   练习1：遍历 os.listdir(directory)，用 os.path.splitext 取扩展名，
#          用 .lower() 转小写后与 ".txt" 比较，将 os.path.join 的完整路径存入列表
#   练习2：build_head_parser 用 add_argument 添加三个参数；head_command 用
#          open(args.input) 读取所有行，--count 分支打印总行数，否则打印 lines[:args.lines]
#   练习3：用 try/except 包裹 subprocess.run(cmd, capture_output=True, text=True,
#          timeout=timeout_sec)，分别捕获 TimeoutExpired 和 FileNotFoundError


# =============================================================================
# 【练习答案】
# =============================================================================

import tempfile


def find_txt_files(directory: str) -> List[str]:
    """
    练习 1：返回目录中所有 .txt 文件的完整路径列表（已排序，不区分大小写）。

    Args:
        directory: 要搜索的目录路径

    Returns:
        该目录下所有扩展名为 .txt（不区分大小写）的文件完整路径列表，已排序

    Raises:
        ValueError: 若 directory 不是有效目录

    Example:
        # 目录中有：a.txt, b.txt, c.py, d.TXT, e.md
        find_txt_files("/tmp/test")
        # ["/tmp/test/a.txt", "/tmp/test/b.txt", "/tmp/test/d.TXT"]
    """
    if not os.path.isdir(directory):
        raise ValueError(f"'{directory}' 不是有效的目录")
    result = []
    for filename in os.listdir(directory):
        _, ext = os.path.splitext(filename)
        if ext.lower() == ".txt":
            result.append(os.path.join(directory, filename))
    return sorted(result)


# ── 练习 2：argparse head 命令 ───────────────────────────────────────────────

def build_head_parser() -> argparse.ArgumentParser:
    """构建 head 命令的参数解析器。"""
    parser = argparse.ArgumentParser(
        description="显示文件的前 N 行（类似 Unix head 命令）",
    )
    parser.add_argument(
        "input",
        help="输入文件路径",
        type=str,
    )
    parser.add_argument(
        "-n", "--lines",
        help="显示前 N 行（默认: 10）",
        type=int,
        default=10,
        metavar="N",
    )
    parser.add_argument(
        "-c", "--count",
        help="只显示总行数，不显示内容",
        action="store_true",
    )
    return parser


def head_command(args: argparse.Namespace) -> None:
    """
    练习 2：类似 Unix head 命令的实现。

    Args:
        args: argparse 解析后的命名空间，包含：
              args.input  - 输入文件路径（str）
              args.lines  - 显示前 N 行（int，默认 10）
              args.count  - 是否只显示总行数（bool）

    Example:
        parser = build_head_parser()

        args = parser.parse_args(["file.txt", "-n", "5"])
        head_command(args)      # 打印文件前 5 行

        args = parser.parse_args(["file.txt", "--count"])
        head_command(args)      # 打印: "  共 20 行"
    """
    try:
        with open(args.input, encoding="utf-8") as f:
            lines = f.readlines()
    except FileNotFoundError:
        print(f"  错误：文件 '{args.input}' 不存在")
        return
    except OSError as e:
        print(f"  错误：无法读取文件: {e}")
        return

    if args.count:
        print(f"  共 {len(lines)} 行")
    else:
        for line in lines[:args.lines]:
            print(f"  {line}", end="")
        if lines and not lines[min(args.lines, len(lines)) - 1].endswith("\n"):
            print()


# ── 练习 3：带超时的命令执行 ─────────────────────────────────────────────────

def run_with_timeout(cmd: List[str], timeout_sec: float) -> Optional[str]:
    """
    练习 3：运行外部命令，超时或出错时返回 None。

    Args:
        cmd: 要执行的命令列表，如 ["python", "--version"]
        timeout_sec: 超时秒数，超过此时间则中断并返回 None

    Returns:
        命令的标准输出字符串（成功时），或 None（超时/命令不存在时）

    Example:
        run_with_timeout(["python", "--version"], 5.0)
        # "Python 3.12.0\n"

        run_with_timeout(["sleep", "100"], 0.1)
        # None（超时，并打印警告）

        run_with_timeout(["no_such_cmd"], 5.0)
        # None（命令不存在，并打印错误）
    """
    try:
        result = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=timeout_sec,
            encoding="utf-8",
            errors="ignore",
        )
        return result.stdout
    except subprocess.TimeoutExpired:
        print(f"  ⚠ 命令 {cmd} 执行超时（>{timeout_sec}s）")
        return None
    except FileNotFoundError:
        print(f"  ✗ 命令不存在: {cmd[0]}")
        return None


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 find_txt_files 函数。"""
    print("find_txt_files 演示:")

    # ── 创建临时目录并写入测试文件 ────────────────────────
    with tempfile.TemporaryDirectory() as tmpdir:
        for name in ["a.txt", "b.txt", "c.py", "d.TXT", "e.md"]:
            Path(tmpdir, name).write_text("test", encoding="utf-8")

        txt_files = find_txt_files(tmpdir)
        print(f"  目录中有 5 个文件: a.txt, b.txt, c.py, d.TXT, e.md")
        print(f"  找到 {len(txt_files)} 个 .txt 文件（不区分大小写）:")
        for f in txt_files:
            print(f"    - {os.path.basename(f)}")

    # ── 空目录 ───────────────────────────────────────────
    with tempfile.TemporaryDirectory() as tmpdir:
        result = find_txt_files(tmpdir)
        print(f"\n  空目录结果: {result}")

    # ── 无效目录 ─────────────────────────────────────────
    try:
        find_txt_files("/not/a/real/directory")
    except ValueError as e:
        print(f"\n  ✓ 无效目录捕获: {e}")


def exercise2_answer() -> None:
    """练习 2：演示 head_command（argparse）。"""
    print("head_command 演示:")

    # ── 创建临时文件（20 行）─────────────────────────────
    content = "\n".join(f"第 {i:02d} 行内容" for i in range(1, 21)) + "\n"
    with tempfile.NamedTemporaryFile(
        mode="w", suffix=".txt", encoding="utf-8", delete=False
    ) as tmp:
        tmp.write(content)
        tmp_path = tmp.name

    try:
        parser = build_head_parser()

        # ── 显示前 5 行 ───────────────────────────────────
        print(f"\n  head -n 5 {os.path.basename(tmp_path)}:")
        args = parser.parse_args([tmp_path, "-n", "5"])
        head_command(args)

        # ── 只显示总行数 ──────────────────────────────────
        print(f"\n  head -c {os.path.basename(tmp_path)}:")
        args = parser.parse_args([tmp_path, "--count"])
        head_command(args)

        # ── 默认显示前 10 行 ──────────────────────────────
        print(f"\n  head {os.path.basename(tmp_path)}（默认 10 行）:")
        args = parser.parse_args([tmp_path])
        head_command(args)

        # ── 文件不存在 ────────────────────────────────────
        print(f"\n  head no_such_file.txt:")
        args = parser.parse_args(["no_such_file.txt"])
        head_command(args)

    finally:
        os.unlink(tmp_path)


def exercise3_answer() -> None:
    """练习 3：演示 run_with_timeout 函数。"""
    print("run_with_timeout 演示:")

    is_windows = sys.platform == "win32"

    # ── 成功执行 ──────────────────────────────────────────
    cmd = ["python", "--version"] if is_windows else ["python3", "--version"]
    out = run_with_timeout(cmd, timeout_sec=5.0)
    if out:
        print(f"  成功: {out.strip()}")

    # ── 超时 ─────────────────────────────────────────────
    print()
    sleep_cmd = ["cmd", "/c", "timeout", "/t", "10"] if is_windows else ["sleep", "10"]
    result = run_with_timeout(sleep_cmd, timeout_sec=0.1)
    print(f"  超时返回值: {result}")

    # ── 命令不存在 ────────────────────────────────────────
    print()
    result = run_with_timeout(["no_such_command_xyz"], timeout_sec=5.0)
    print(f"  命令不存在返回值: {result}")


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
