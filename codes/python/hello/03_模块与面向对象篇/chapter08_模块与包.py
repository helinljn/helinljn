# =============================================================================
# 第 8 章：模块与包
# =============================================================================
#
# 【学习目标】
#   1. 理解模块的概念和导入机制
#   2. 掌握 import 的各种用法
#   3. 了解包的组织结构
#   4. 理解 __name__ 和 if __name__ == "__main__"
#   5. 掌握常用的模块管理技巧
#
# 【运行方式】
#   python chapter08_模块与包.py
#
# =============================================================================

import sys
import os
from pathlib import Path


# =============================================================================
# 8.1 模块基础
# =============================================================================

def demo_module_basics() -> None:
    """演示模块的基本概念。"""
    print("=" * 60)
    print("8.1 模块基础")
    print("=" * 60)

    # 模块（Module）：一个 .py 文件就是一个模块
    # 包（Package）：包含 __init__.py 的目录，可以包含多个模块

    print("什么是模块？")
    print("  - 每个 .py 文件都是一个模块")
    print("  - 模块名 = 文件名（不含 .py）")
    print("  - 模块可以包含函数、类、变量")
    print("  - 通过 import 导入使用")

    # ── 导入方式 ─────────────────────────────────────────
    print("\n导入方式：")

    # 1. import 模块名
    import math
    print(f"  import math")
    print(f"    math.pi = {math.pi}")
    print(f"    math.sqrt(16) = {math.sqrt(16)}")

    # 2. from 模块 import 名称
    from math import sqrt, pi
    print(f"\n  from math import sqrt, pi")
    print(f"    sqrt(25) = {sqrt(25)}")
    print(f"    pi = {pi}")

    # 3. import 模块 as 别名
    import datetime as dt
    print(f"\n  import datetime as dt")
    print(f"    dt.date.today() = {dt.date.today()}")

    # 4. from 模块 import *（不推荐，污染命名空间）
    # from math import *  # ❌ 不推荐

    # ── 模块搜索路径 ─────────────────────────────────────
    print("\n模块搜索路径（sys.path）:")
    print("  Python 按以下顺序搜索模块：")
    for i, path in enumerate(sys.path[:5], 1):
        print(f"    {i}. {path}")
    print("    ...")


# =============================================================================
# 8.2 创建自己的模块
# =============================================================================

def demo_custom_module() -> None:
    """演示如何创建和使用自定义模块。"""
    print("\n" + "=" * 60)
    print("8.2 创建自己的模块")
    print("=" * 60)

    # 创建示例模块文件
    module_code = '''"""
mymath.py - 自定义数学工具模块

这是一个示例模块，演示模块的基本结构。
"""

# 模块级变量
PI = 3.14159
VERSION = "1.0.0"

# 私有变量（约定以 _ 开头）
_internal_cache = {}


def add(a: float, b: float) -> float:
    """两数相加。"""
    return a + b


def multiply(a: float, b: float) -> float:
    """两数相乘。"""
    return a * b


def _helper_function():
    """私有辅助函数（约定以 _ 开头，不应被外部使用）。"""
    pass


class Calculator:
    """简单的计算器类。"""

    def __init__(self, name: str = "默认计算器"):
        self.name = name

    def calculate(self, a: float, b: float, op: str) -> float:
        if op == "+":
            return add(a, b)
        elif op == "*":
            return multiply(a, b)
        else:
            raise ValueError(f"不支持的操作: {op}")


# 模块初始化代码（导入时执行）
print(f"[mymath] 模块已加载，版本 {VERSION}")


# 测试代码（只在直接运行时执行）
if __name__ == "__main__":
    print("直接运行 mymath.py")
    print(f"add(3, 5) = {add(3, 5)}")
    calc = Calculator("测试计算器")
    print(f"{calc.name}: 2 * 3 = {calc.calculate(2, 3, '*')}")
'''

    print("示例模块结构（mymath.py）：")
    print(module_code)

    print("\n使用自定义模块：")
    print("  # 假设 mymath.py 在当前目录")
    print("  import mymath")
    print("  result = mymath.add(10, 20)")
    print("  calc = mymath.Calculator('我的计算器')")


# =============================================================================
# 8.3 __name__ 和 __main__
# =============================================================================

def demo_name_main() -> None:
    """演示 __name__ 和 if __name__ == '__main__' 的用法。"""
    print("\n" + "=" * 60)
    print("8.3 __name__ 和 __main__")
    print("=" * 60)

    # __name__ 是每个模块的内置变量
    # - 直接运行时：__name__ == "__main__"
    # - 被导入时：__name__ == 模块名

    print(f"当前模块的 __name__ = {__name__!r}")

    print("\n典型用法：")
    example = '''
# mymodule.py
def my_function():
    print("这是模块函数")

# 只在直接运行时执行，被导入时不执行
if __name__ == "__main__":
    print("直接运行 mymodule.py")
    my_function()
    # 测试代码...
'''
    print(example)

    print("好处：")
    print("  1. 模块既可以被导入使用，也可以独立运行测试")
    print("  2. 测试代码不会在被导入时执行")
    print("  3. 可以在模块中编写示例和测试")


# =============================================================================
# 8.4 包（Package）
# =============================================================================

def demo_packages() -> None:
    """演示包的组织结构。"""
    print("\n" + "=" * 60)
    print("8.4 包（Package）")
    print("=" * 60)

    # 包是包含 __init__.py 的目录
    # 用于组织多个相关模块

    package_structure = '''
mypackage/              # 包目录
├── __init__.py         # 包初始化文件（必需，可以为空）
├── module1.py          # 子模块1
├── module2.py          # 子模块2
└── subpackage/         # 子包
    ├── __init__.py
    └── module3.py
'''

    print("包的目录结构：")
    print(package_structure)

    print("\n导入包中的模块：")
    examples = [
        "import mypackage.module1",
        "from mypackage import module1",
        "from mypackage.module1 import some_function",
        "from mypackage.subpackage import module3",
    ]
    for ex in examples:
        print(f"  {ex}")

    print("\n__init__.py 的作用：")
    print("  1. 标识目录为 Python 包")
    print("  2. 包的初始化代码")
    print("  3. 定义 __all__ 控制 from package import *")
    print("  4. 可以为空（Python 3.3+ 支持隐式命名空间包）")

    init_example = '''
# mypackage/__init__.py
"""mypackage - 我的工具包"""

__version__ = "1.0.0"
__all__ = ["module1", "module2"]  # 控制 from mypackage import *

# 包级别的便捷导入
from .module1 import important_function
from .module2 import ImportantClass

print(f"[mypackage] 已加载，版本 {__version__}")
'''
    print("\n__init__.py 示例：")
    print(init_example)


# =============================================================================
# 8.5 相对导入与绝对导入
# =============================================================================

def demo_imports() -> None:
    """演示相对导入和绝对导入。"""
    print("\n" + "=" * 60)
    print("8.5 相对导入与绝对导入")
    print("=" * 60)

    print("绝对导入（推荐）：")
    print("  from mypackage import module1")
    print("  from mypackage.subpackage import module3")
    print("  import mypackage.module1")

    print("\n相对导入（仅在包内使用）：")
    print("  from . import module1          # 当前包")
    print("  from .. import module2         # 上级包")
    print("  from ..sibling import module3  # 兄弟包")

    print("\n注意事项：")
    print("  ❌ 相对导入不能在顶层脚本中使用")
    print("  ✅ 相对导入只能在包的模块中使用")
    print("  ✅ 推荐使用绝对导入（更清晰）")


# =============================================================================
# 8.6 常用内置模块速览
# =============================================================================

def demo_builtin_modules() -> None:
    """演示常用内置模块。"""
    print("\n" + "=" * 60)
    print("8.6 常用内置模块速览")
    print("=" * 60)

    # ── sys ──────────────────────────────────────────────
    print("1. sys - 系统相关:")
    print(f"  Python 版本: {sys.version.split()[0]}")
    print(f"  平台: {sys.platform}")
    print(f"  命令行参数: {sys.argv[0]}")

    # ── os ───────────────────────────────────────────────
    print("\n2. os - 操作系统接口:")
    print(f"  当前目录: {os.getcwd()}")
    print(f"  环境变量 PATH: {os.environ.get('PATH', 'N/A')[:50]}...")

    # ── pathlib ──────────────────────────────────────────
    print("\n3. pathlib - 现代路径操作:")
    current_file = Path(__file__)
    print(f"  当前文件: {current_file.name}")
    print(f"  父目录: {current_file.parent.name}")

    # ── datetime ─────────────────────────────────────────
    from datetime import datetime
    print("\n4. datetime - 日期时间:")
    print(f"  现在: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")

    # ── json ─────────────────────────────────────────────
    import json
    print("\n5. json - JSON 处理:")
    data = {"name": "Python", "version": 3.11}
    print(f"  序列化: {json.dumps(data)}")

    print("\n更多模块将在后续章节详细讲解...")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_module_basics()
    demo_custom_module()
    demo_name_main()
    demo_packages()
    demo_imports()
    demo_builtin_modules()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 导入语法 ──
# import module
# import module as alias
# from module import name
# from module import name1, name2
# from module import name as alias
# from package.module import name
#
# ── 相对导入（仅在包内）──
# from . import module        # 当前包
# from .. import module       # 上级包
# from .subpkg import module  # 子包
#
# ── __name__ ──
# if __name__ == "__main__":
#     # 直接运行时执行的代码
#
# ── 包结构 ──
# mypackage/
# ├── __init__.py
# ├── module1.py
# └── subpackage/
#     ├── __init__.py
#     └── module2.py


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：循环导入
# # a.py
# import b
# # b.py
# import a  # ❌ 循环导入
# 解决：重构代码，将共同依赖提取到第三个模块
#
# 错误 2：相对导入在脚本中使用
# # script.py
# from . import module  # ❌ 不能在顶层脚本使用相对导入
#
# 错误 3：忘记 __init__.py
# mypackage/
# ├── module1.py  # ❌ 缺少 __init__.py（Python 2 会报错）
# Python 3.3+ 支持隐式命名空间包，但建议保留 __init__.py
#
# 错误 4：模块名与标准库冲突
# # random.py  # ❌ 与标准库 random 冲突
# import random  # 会导入当前目录的 random.py 而非标准库


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   创建一个 utils 包，包含两个模块：
#   - string_utils.py：提供字符串处理函数
#   - math_utils.py：提供数学计算函数
#   在 __init__.py 中导入常用函数
#
# 练习 2：
#   写一个模块 config.py，包含配置变量和加载配置的函数
#   在另一个模块中导入并使用这些配置
#
# 练习 3：
#   理解并解释以下代码的输出：
#   # module_a.py
#   print("Loading module_a")
#   x = 10
#
#   # module_b.py
#   import module_a
#   print(f"x = {module_a.x}")
#
#   # main.py
#   import module_a
#   import module_b
#   print("Done")


# =============================================================================
# 【练习答案】
# =============================================================================

def exercise1_answer() -> None:
    """练习 1：创建 utils 包（内联演示各模块功能）。"""

    # ── utils/string_utils.py 的内容 ─────────────────────
    def truncate(s: str, max_len: int, suffix: str = "...") -> str:
        """截断字符串到指定长度，超出部分用 suffix 替代。"""
        if len(s) <= max_len:
            return s
        return s[:max_len - len(suffix)] + suffix

    def is_palindrome(s: str) -> bool:
        """判断是否为回文（忽略大小写和非字母数字字符）。"""
        cleaned = "".join(c.lower() for c in s if c.isalnum())
        return cleaned == cleaned[::-1]

    def count_words(s: str) -> dict:
        """统计字符串中每个单词出现的次数。"""
        result: dict[str, int] = {}
        for word in s.lower().split():
            result[word] = result.get(word, 0) + 1
        return result

    # ── utils/math_utils.py 的内容 ───────────────────────
    def clamp(value: float, min_val: float, max_val: float) -> float:
        """将值限制在 [min_val, max_val] 范围内。"""
        return max(min_val, min(max_val, value))

    def is_prime(n: int) -> bool:
        """判断整数是否为质数。"""
        if n < 2:
            return False
        for i in range(2, int(n ** 0.5) + 1):
            if n % i == 0:
                return False
        return True

    def factorial(n: int) -> int:
        """计算 n 的阶乘（n >= 0）。"""
        if n < 0:
            raise ValueError(f"阶乘要求非负整数，得到: {n}")
        result = 1
        for i in range(2, n + 1):
            result *= i
        return result

    # ── utils/__init__.py 的内容（注释说明）─────────────
    # from .string_utils import truncate, is_palindrome, count_words
    # from .math_utils import clamp, is_prime, factorial
    # __all__ = ["truncate", "is_palindrome", "count_words",
    #            "clamp", "is_prime", "factorial"]
    # __version__ = "1.0.0"

    print("string_utils 演示:")
    print(f"  truncate('Hello, Python World!', 12) = "
          f"{truncate('Hello, Python World!', 12)!r}")
    print(f"  is_palindrome('A man a plan a canal Panama') = "
          f"{is_palindrome('A man a plan a canal Panama')}")
    print(f"  count_words('the cat sat on the mat') = "
          f"{count_words('the cat sat on the mat')}")

    print("math_utils 演示:")
    print(f"  clamp(15, 0, 10) = {clamp(15, 0, 10)}")
    print(f"  is_prime(17) = {is_prime(17)}, is_prime(18) = {is_prime(18)}")
    print(f"  factorial(5) = {factorial(5)}")


def exercise2_answer() -> None:
    """练习 2：config 模块（内联演示）。"""
    import os
    from typing import Any

    # ── config.py 的内容 ─────────────────────────────────
    class Config:
        """配置管理器，演示 config.py 模块的典型设计。"""

        _DEFAULTS: dict[str, Any] = {
            "DEBUG": False,
            "HOST": "127.0.0.1",
            "PORT": 8080,
            "DATABASE_URL": "sqlite:///app.db",
            "MAX_RETRIES": 3,
            "LOG_LEVEL": "INFO",
        }

        def __init__(self) -> None:
            self._data: dict[str, Any] = dict(self._DEFAULTS)

        def load_from_env(self) -> int:
            """从环境变量加载配置，返回实际加载的条目数。"""
            count = 0
            for key in self._DEFAULTS:
                env_val = os.environ.get(key)
                if env_val is not None:
                    self._data[key] = env_val
                    count += 1
            return count

        def load_from_dict(self, settings: dict[str, Any]) -> None:
            """从字典批量更新配置。"""
            self._data.update(settings)

        def get(self, key: str, default: Any = None) -> Any:
            """安全获取配置值，键不存在时返回 default。"""
            return self._data.get(key, default)

        def reset(self) -> None:
            """重置为默认配置。"""
            self._data = dict(self._DEFAULTS)

        def __repr__(self) -> str:
            return f"Config({self._data})"

    config = Config()
    print("config 模块演示:")
    print(f"  默认: HOST={config.get('HOST')!r}, PORT={config.get('PORT')}, "
          f"DEBUG={config.get('DEBUG')}")

    config.load_from_dict({"HOST": "0.0.0.0", "DEBUG": True, "PORT": 9090})
    print(f"  更新: HOST={config.get('HOST')!r}, PORT={config.get('PORT')}, "
          f"DEBUG={config.get('DEBUG')}")

    config.reset()
    print(f"  重置: HOST={config.get('HOST')!r}, PORT={config.get('PORT')}, "
          f"DEBUG={config.get('DEBUG')}")
    print(f"  缺失键: get('TIMEOUT', 30) = {config.get('TIMEOUT', 30)}")


def exercise3_answer() -> None:
    """练习 3：解释模块导入顺序的输出。"""
    print("模块导入机制分析:")
    print()
    print("  # module_a.py")
    print("  print('Loading module_a')   # 模块级代码，首次导入时执行")
    print("  x = 10")
    print()
    print("  # module_b.py")
    print("  import module_a             # 首次导入：已在 sys.modules 中则跳过")
    print("  print(f'x = {module_a.x}')")
    print()
    print("  # main.py")
    print("  import module_a             # 1. 首次加载 module_a")
    print("  import module_b             # 2. 加载 module_b，触发其内部 import")
    print("  print('Done')               # 3. 最后打印")
    print()
    print("  运行 main.py 的输出：")
    print("    Loading module_a   ← main.py 首次 import module_a，执行模块体")
    print("    x = 10             ← module_b 内 import module_a 命中缓存（不重复执行），")
    print("                          然后打印 module_a.x 的值")
    print("    Done               ← main.py 继续执行 print('Done')")
    print()
    print("  关键结论：")
    print("  ✅ Python 用 sys.modules 缓存已导入的模块")
    print("  ✅ 同一模块无论被 import 多少次，模块级代码只执行一次")
    print("  ✅ 因此要谨慎在模块顶层写有副作用的代码（如 print、网络请求等）")


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
