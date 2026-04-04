"""
第 24 章：代码组织与规范

本章内容：
1. 项目目录结构最佳实践
2. 模块化设计原则
3. PEP 8 代码规范详解
4. 类型注解（Type Hints）
5. 文档字符串规范
6. 单元测试（unittest）
7. 虚拟环境（venv）

学习目标：
- 掌握 Python 项目的标准组织结构
- 理解并应用 PEP 8 代码规范
- 学会使用类型注解提高代码可维护性
- 掌握单元测试的编写方法
- 了解虚拟环境的使用
"""

# ============================================================================
# 1. 项目目录结构最佳实践
# ============================================================================

# 标准 Python 项目结构如下（以注释形式展示）：
#
# my_project/
# │
# ├── README.md              # 项目说明文档
# ├── LICENSE                # 开源协议
# ├── requirements.txt       # 依赖包列表
# ├── setup.py               # 安装配置文件
# ├── .gitignore             # Git 忽略文件
# │
# ├── docs/                  # 文档目录
# │   ├── conf.py
# │   └── index.rst
# │
# ├── tests/                 # 测试目录
# │   ├── __init__.py
# │   ├── test_module1.py
# │   └── test_module2.py
# │
# ├── my_project/            # 主包目录（与项目同名）
# │   ├── __init__.py
# │   ├── __main__.py        # 使包可执行：python -m my_project
# │   ├── core.py            # 核心功能
# │   ├── utils.py           # 工具函数
# │   ├── config.py          # 配置管理
# │   └── subpackage/        # 子包
# │       ├── __init__.py
# │       └── module.py
# │
# └── scripts/               # 辅助脚本目录
#     └── run.py
#
# 小型项目结构（单文件或少量文件）：
#
# simple_project/
# │
# ├── README.md
# ├── requirements.txt
# ├── main.py                # 主程序
# └── utils.py               # 工具函数

print("=" * 60)
print("第 24 章：代码组织与规范")
print("=" * 60)


# ============================================================================
# 2. 模块化设计原则
# ============================================================================

print("\n--- 2. 模块化设计原则 ---")

# 模块化设计的四大核心原则：
#
# 1. 单一职责原则（SRP）：每个模块只负责一个功能领域
# 2. 高内聚低耦合：模块内部紧密相关，模块间依赖最小
# 3. 接口清晰：明确公共接口（__all__），隐藏实现细节（_ 前缀）
# 4. 可测试性：模块易于单独测试，避免全局状态

# 示例：良好的模块化设计（模拟各模块内容）

# ----- 模拟 config.py 模块 -----
class AppConfig:
    """应用配置类（对应实际项目中的 config.py）"""

    DEBUG = False
    DATABASE_URL = "sqlite:///app.db"
    LOG_LEVEL = "INFO"

    @classmethod
    def from_file(cls, filepath: str) -> 'AppConfig':
        """从文件加载配置"""
        instance = cls()
        # 实际项目中：从 .env 或 .ini 文件读取配置
        print(f"  从文件加载配置: {filepath}")
        return instance


# ----- 模拟 database.py 模块 -----
class SimpleDatabase:
    """简单数据库连接管理（对应实际项目中的 database.py）"""

    def __init__(self, url: str):
        self.url = url
        self._connected = False

    def connect(self) -> None:
        """建立数据库连接"""
        self._connected = True
        print(f"  已连接到数据库: {self.url}")

    def close(self) -> None:
        """关闭数据库连接"""
        self._connected = False
        print("  数据库连接已关闭")

    def is_connected(self) -> bool:
        """检查连接状态"""
        return self._connected


# ----- 模拟 utils.py 模块 -----
def format_date(year: int, month: int, day: int) -> str:
    """格式化日期（对应实际项目中的 utils.py）"""
    return f"{year:04d}-{month:02d}-{day:02d}"


def validate_email(email: str) -> bool:
    """验证邮箱格式"""
    import re
    pattern = r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$'
    return bool(re.match(pattern, email))


# ----- 模拟 __init__.py 中的公共接口定义 -----
# 在实际包的 __init__.py 中会写：
# __all__ = ['AppConfig', 'SimpleDatabase', 'format_date', 'validate_email']
# __version__ = '1.0.0'

# 演示模块化设计
print("模块化设计演示:")
config = AppConfig.from_file("app.ini")
db = SimpleDatabase(AppConfig.DATABASE_URL)
db.connect()
print(f"  日期格式化: {format_date(2024, 1, 15)}")
print(f"  邮箱验证: {validate_email('user@example.com')}")
print(f"  邮箱验证: {validate_email('invalid-email')}")
db.close()


# ============================================================================
# 3. PEP 8 代码规范详解
# ============================================================================

print("\n--- 3. PEP 8 代码规范详解 ---")
print("官方文档: https://peps.python.org/pep-0008/")

# 3.1 缩进：使用 4 个空格（禁止使用 Tab）

def pep8_indentation_demo():
    """缩进规范演示"""
    if True:
        for i in range(3):
            print(f"  正确缩进（4空格）: {i}")


# 3.2 行长度：每行不超过 79 个字符（注释/文档字符串不超过 72 个字符）
#
# 长函数调用的正确换行方式（使用括号续行）：
#
#   result = some_long_function_name(
#       first_argument, second_argument,
#       third_argument, keyword=value
#   )
#
# 长字符串拼接：
#
#   message = (
#       "这是第一部分文字，"
#       "这是第二部分文字，"
#       "这是第三部分文字。"
#   )

# 3.3 空行规范
# - 顶层定义（类/函数）之间：空 2 行
# - 类内方法之间：空 1 行
# - 函数内逻辑分组：空 1 行

# 3.4 导入规范（顺序：标准库 → 第三方库 → 本地模块，组间空一行）
#
#   import os          # 标准库
#   import sys
#
#   import requests    # 第三方库（需要 pip install）
#
#   from myapp import core   # 本地模块

# 3.5 命名规范汇总

# 模块名：小写字母 + 下划线（my_module.py）
# 包名：小写字母，尽量不用下划线（mypackage/）
# 类名：大驼峰 CapWords
class GoodClassName:
    pass

# 函数名、变量名：小写 + 下划线（snake_case）
def good_function_name():
    pass

good_variable_name = 42

# 常量：全大写 + 下划线
MAX_SIZE = 1000
DEFAULT_ENCODING = "utf-8"

# 私有成员：单下划线前缀（约定私有，可访问）
# 名称混淆：双下划线前缀（触发 name mangling，强制私有）
class NamingConventions:
    public_var = "公共"
    _private_var = "约定私有"
    __mangled_var = "强制私有（名称混淆）"

    def public_method(self):
        return "公共方法"

    def _internal_method(self):
        return "内部方法"


# 3.6 空格使用规范

# ✓ 赋值、算术、比较运算符两边各一个空格
correct_spacing = 1 + 2
is_valid = correct_spacing > 0

# ✓ 逗号后一个空格，冒号后一个空格
coord_list = [1, 2, 3]
info_dict = {"key": "value"}

# ✓ 函数默认参数的 = 两边不加空格
def connect(host, port=8080, timeout=30):
    pass

# ✗ 以下是错误示例（已注释）
# x=1+2            # 运算符周围缺少空格
# def f(x =1):     # 默认参数 = 周围不应有空格
# my_list=[1,2,3]  # 括号内侧不加空格

# 3.7 字符串引号
# Python 中单引号和双引号等价，选择一种并保持一致
# 推荐：优先使用双引号（与其他语言一致）

name = "Alice"
greeting = f"Hello, {name}!"

# 包含单引号时用双引号，包含双引号时用单引号（避免转义）
sentence1 = "It's a beautiful day"
sentence2 = 'She said "hello"'

print("\nPEP 8 规范演示:")
pep8_indentation_demo()
print(f"  命名规范示例: MAX_SIZE={MAX_SIZE}, DEFAULT_ENCODING={DEFAULT_ENCODING}")
print(f"  字符串: {sentence1}")
print(f"  字符串: {sentence2}")


# ============================================================================
# 4. 类型注解（Type Hints）
# ============================================================================

print("\n--- 4. 类型注解（Type Hints）---")

# 类型注解（PEP 484，Python 3.5+）的作用：
# - 提升 IDE 代码补全和错误检测能力
# - 作为代码文档，让读者快速了解接口契约
# - 配合 mypy 等工具进行静态类型检查
# - 不影响运行时性能（纯注解，Python 不强制检查）

from typing import List, Dict, Tuple, Optional, Union, Any, Callable, Set
from typing import TypeVar, Generic, Iterator, Generator

# 4.1 基本类型注解
def greet(name: str) -> str:
    return f"Hello, {name}!"

def add(a: int, b: int) -> int:
    return a + b

def divide(a: float, b: float) -> float:
    return a / b

def is_positive(n: int) -> bool:
    return n > 0

# 返回 None 的函数
def print_message(msg: str) -> None:
    print(f"  [消息] {msg}")

# 4.2 容器类型注解
def sum_numbers(numbers: List[int]) -> int:
    return sum(numbers)

def get_user() -> Dict[str, str]:
    return {"name": "Alice", "role": "admin"}

def get_point() -> Tuple[float, float]:
    return (3.14, 2.71)

def get_unique_tags() -> Set[str]:
    return {"python", "coding", "tutorial"}

# 4.3 Optional（可能为 None 的返回值）
def find_user_by_id(user_id: int) -> Optional[Dict[str, str]]:
    """Optional[X] 等价于 Union[X, None]"""
    users = {1: {"name": "Alice"}, 2: {"name": "Bob"}}
    return users.get(user_id)  # 找不到时返回 None

# 4.4 Union（多种可能的类型）
def stringify(value: Union[int, float, bool]) -> str:
    """将数值类型转换为字符串"""
    return str(value)

# Python 3.10+ 可以用 | 语法代替 Union
# def stringify(value: int | float | bool) -> str:

# 4.5 Callable（函数类型）
def apply(func: Callable[[int, int], int], a: int, b: int) -> int:
    """接受一个二元整数函数并应用它"""
    return func(a, b)

# 4.6 TypeVar（泛型）
T = TypeVar('T')

def first_element(items: List[T]) -> Optional[T]:
    """返回列表第一个元素，适用于任意类型"""
    return items[0] if items else None

# 4.7 Python 3.9+ 新语法（直接使用内置类型）
# Python 3.9+ 无需从 typing 导入 List/Dict/Tuple/Set
# 可以直接写 list[int], dict[str, int], tuple[float, float]

def modern_sum(numbers: list[int]) -> int:
    """Python 3.9+ 风格的类型注解"""
    return sum(numbers)

def modern_lookup(data: dict[str, list[int]], key: str) -> list[int]:
    """嵌套的容器类型注解"""
    return data.get(key, [])

# 4.8 类型注解在类中的使用
class Point:
    """带类型注解的坐标类"""
    x: float
    y: float

    def __init__(self, x: float, y: float) -> None:
        self.x = x
        self.y = y

    def distance_to(self, other: 'Point') -> float:
        """计算到另一个点的距离"""
        import math
        return math.sqrt((self.x - other.x) ** 2 + (self.y - other.y) ** 2)

    def __repr__(self) -> str:
        return f"Point({self.x}, {self.y})"

# 4.9 使用 reveal_type（仅供 mypy 静态检查，运行时需注释掉）
# reveal_type(greet("Alice"))  # mypy 会输出推断的类型

# 演示
print("类型注解演示:")
print(f"  greet: {greet('World')}")
print(f"  add: {add(3, 4)}")
print(f"  find_user_by_id(1): {find_user_by_id(1)}")
print(f"  find_user_by_id(99): {find_user_by_id(99)}")
print(f"  apply(add, 10, 20): {apply(add, 10, 20)}")
print(f"  first_element([1,2,3]): {first_element([1, 2, 3])}")
print(f"  first_element([]): {first_element([])}")

p1 = Point(0.0, 0.0)
p2 = Point(3.0, 4.0)
print(f"  {p1} 到 {p2} 的距离: {p1.distance_to(p2)}")


# ============================================================================
# 5. 文档字符串（Docstring）规范
# ============================================================================

print("\n--- 5. 文档字符串规范 ---")

# Python 常见文档字符串风格：
# 1. Google 风格（推荐，简洁易读）
# 2. NumPy 风格（适合科学计算）
# 3. reStructuredText 风格（Sphinx 默认）

# ----- 5.1 Google 风格 -----

def google_style_example(
    name: str,
    age: int,
    scores: List[int],
    active: bool = True
) -> Optional[str]:
    """
    Google 风格文档字符串示例。

    一句话描述函数功能。之后可以有更详细的说明，
    包括使用场景、注意事项等内容。

    Args:
        name: 用户名称，非空字符串。
        age: 用户年龄，必须是正整数。
        scores: 成绩列表，允许为空。
        active: 是否处于激活状态，默认为 True。

    Returns:
        激活用户的摘要字符串，格式为 "name(age)"。
        如果用户未激活则返回 None。

    Raises:
        ValueError: 当 age 不是正整数时抛出。
        TypeError: 当 name 不是字符串时抛出。

    Examples:
        >>> google_style_example("Alice", 25, [90, 85])
        'Alice(25)'
        >>> google_style_example("Bob", 30, [], active=False)

    Note:
        此函数不会修改传入的列表。
    """
    if not isinstance(name, str):
        raise TypeError(f"name 必须是字符串，收到: {type(name)}")
    if age <= 0:
        raise ValueError(f"age 必须是正整数，收到: {age}")
    if not active:
        return None
    avg = sum(scores) / len(scores) if scores else 0
    return f"{name}({age}), 平均分: {avg:.1f}"


# ----- 5.2 类的文档字符串 -----

class StudentRecord:
    """
    学生记录类。

    管理单个学生的基本信息和成绩数据，
    提供成绩统计和报告生成功能。

    Attributes:
        name: 学生姓名。
        student_id: 学生学号（唯一标识）。
        scores: 各科成绩字典，键为科目名称，值为分数。

    Examples:
        >>> student = StudentRecord("Alice", "S001")
        >>> student.add_score("数学", 95)
        >>> student.get_average()
        95.0
    """

    def __init__(self, name: str, student_id: str) -> None:
        """
        初始化学生记录。

        Args:
            name: 学生姓名，不能为空。
            student_id: 学生学号，不能为空。
        """
        self.name = name
        self.student_id = student_id
        self.scores: Dict[str, float] = {}

    def add_score(self, subject: str, score: float) -> None:
        """
        添加科目成绩。

        Args:
            subject: 科目名称。
            score: 分数，范围 0-100。

        Raises:
            ValueError: 当分数不在 [0, 100] 范围内时抛出。
        """
        if not 0 <= score <= 100:
            raise ValueError(f"分数必须在 0-100 之间，收到: {score}")
        self.scores[subject] = score

    def get_average(self) -> float:
        """
        计算平均分。

        Returns:
            所有科目的平均分。如果没有成绩则返回 0.0。
        """
        if not self.scores:
            return 0.0
        return sum(self.scores.values()) / len(self.scores)

    def get_report(self) -> str:
        """
        生成成绩报告字符串。

        Returns:
            格式化的成绩报告，包含姓名、学号和各科成绩。
        """
        lines = [
            f"学生报告",
            f"  姓名: {self.name}",
            f"  学号: {self.student_id}",
            f"  成绩:",
        ]
        for subject, score in self.scores.items():
            lines.append(f"    {subject}: {score:.1f}")
        lines.append(f"  平均分: {self.get_average():.1f}")
        return "\n".join(lines)

    def __repr__(self) -> str:
        return f"StudentRecord(name={self.name!r}, id={self.student_id!r})"


# 演示文档字符串
print("文档字符串演示:")
result = google_style_example("Alice", 25, [90, 85, 92])
print(f"  google_style_example: {result}")

student = StudentRecord("Bob", "S001")
student.add_score("数学", 88)
student.add_score("英语", 92)
student.add_score("Python", 95)
print(student.get_report())

# 访问文档字符串
print(f"\n  函数 __doc__ 预览:")
doc_lines = StudentRecord.get_average.__doc__.strip().split('\n')
print(f"    {doc_lines[0]}")  # 打印第一行


# ============================================================================
# 6. 单元测试（unittest）
# ============================================================================

print("\n--- 6. 单元测试（unittest）---")

import unittest
import math

# ----- 6.1 被测试的功能模块（通常在独立文件中）-----

def calculate_bmi(weight_kg: float, height_m: float) -> float:
    """
    计算 BMI（体质指数）。

    Args:
        weight_kg: 体重，单位千克，必须大于 0。
        height_m: 身高，单位米，必须大于 0。

    Returns:
        BMI 值，保留两位小数。

    Raises:
        ValueError: 当体重或身高不合法时抛出。
    """
    if weight_kg <= 0:
        raise ValueError(f"体重必须大于 0，收到: {weight_kg}")
    if height_m <= 0:
        raise ValueError(f"身高必须大于 0，收到: {height_m}")
    return round(weight_kg / (height_m ** 2), 2)


def bmi_category(bmi: float) -> str:
    """
    根据 BMI 值返回健康分类。

    Args:
        bmi: BMI 值。

    Returns:
        分类字符串：偏瘦/正常/超重/肥胖。
    """
    if bmi < 18.5:
        return "偏瘦"
    elif bmi < 24.0:
        return "正常"
    elif bmi < 28.0:
        return "超重"
    else:
        return "肥胖"


class TextProcessor:
    """文本处理类（用于演示类的单元测试）"""

    @staticmethod
    def count_words(text: str) -> int:
        """统计单词数（按空格分隔）"""
        if not text.strip():
            return 0
        return len(text.split())

    @staticmethod
    def reverse_string(text: str) -> str:
        """反转字符串"""
        return text[::-1]

    @staticmethod
    def is_palindrome(text: str) -> bool:
        """判断是否为回文（忽略大小写和空格）"""
        cleaned = text.lower().replace(" ", "")
        return cleaned == cleaned[::-1]

    @staticmethod
    def truncate(text: str, max_length: int, suffix: str = "...") -> str:
        """
        截断文本到指定长度。

        Args:
            text: 原始文本。
            max_length: 最大长度（包含后缀）。
            suffix: 截断后追加的后缀。

        Returns:
            截断后的文本。
        """
        if len(text) <= max_length:
            return text
        return text[:max_length - len(suffix)] + suffix


# ----- 6.2 编写单元测试 -----

class TestCalculateBMI(unittest.TestCase):
    """BMI 计算函数的单元测试"""

    def test_normal_bmi(self):
        """测试正常 BMI 计算"""
        # 70kg, 1.75m => 22.86
        result = calculate_bmi(70, 1.75)
        self.assertAlmostEqual(result, 22.86, places=1)

    def test_bmi_precision(self):
        """测试 BMI 返回两位小数"""
        result = calculate_bmi(60, 1.70)
        # 结果应该是浮点数
        self.assertIsInstance(result, float)

    def test_invalid_weight_zero(self):
        """测试体重为 0 时抛出 ValueError"""
        with self.assertRaises(ValueError) as ctx:
            calculate_bmi(0, 1.75)
        self.assertIn("体重", str(ctx.exception))

    def test_invalid_weight_negative(self):
        """测试负体重时抛出 ValueError"""
        with self.assertRaises(ValueError):
            calculate_bmi(-10, 1.75)

    def test_invalid_height(self):
        """测试无效身高时抛出 ValueError"""
        with self.assertRaises(ValueError):
            calculate_bmi(70, 0)

    def test_bmi_boundary_normal(self):
        """测试 BMI 正常范围边界"""
        # 恰好 18.5
        weight = 18.5 * (1.0 ** 2)
        result = calculate_bmi(weight, 1.0)
        self.assertEqual(result, 18.5)


class TestBMICategory(unittest.TestCase):
    """BMI 分类函数的单元测试"""

    def test_underweight(self):
        self.assertEqual(bmi_category(17.0), "偏瘦")
        self.assertEqual(bmi_category(18.4), "偏瘦")

    def test_normal(self):
        self.assertEqual(bmi_category(18.5), "正常")
        self.assertEqual(bmi_category(22.0), "正常")
        self.assertEqual(bmi_category(23.9), "正常")

    def test_overweight(self):
        self.assertEqual(bmi_category(24.0), "超重")
        self.assertEqual(bmi_category(27.9), "超重")

    def test_obese(self):
        self.assertEqual(bmi_category(28.0), "肥胖")
        self.assertEqual(bmi_category(35.0), "肥胖")


class TestTextProcessor(unittest.TestCase):
    """TextProcessor 类的单元测试"""

    def setUp(self):
        """每个测试方法执行前调用（测试夹具）"""
        self.processor = TextProcessor()

    def tearDown(self):
        """每个测试方法执行后调用（清理资源）"""
        # 当前例子无需清理
        pass

    # ---- count_words ----
    def test_count_words_normal(self):
        self.assertEqual(TextProcessor.count_words("hello world"), 2)
        self.assertEqual(TextProcessor.count_words("one two three four"), 4)

    def test_count_words_empty(self):
        self.assertEqual(TextProcessor.count_words(""), 0)
        self.assertEqual(TextProcessor.count_words("   "), 0)

    def test_count_words_single(self):
        self.assertEqual(TextProcessor.count_words("python"), 1)

    # ---- reverse_string ----
    def test_reverse_normal(self):
        self.assertEqual(TextProcessor.reverse_string("hello"), "olleh")
        self.assertEqual(TextProcessor.reverse_string("Python"), "nohtyP")

    def test_reverse_empty(self):
        self.assertEqual(TextProcessor.reverse_string(""), "")

    def test_reverse_single_char(self):
        self.assertEqual(TextProcessor.reverse_string("a"), "a")

    # ---- is_palindrome ----
    def test_palindrome_true(self):
        self.assertTrue(TextProcessor.is_palindrome("racecar"))
        self.assertTrue(TextProcessor.is_palindrome("A man a plan a canal Panama"))
        self.assertTrue(TextProcessor.is_palindrome("level"))

    def test_palindrome_false(self):
        self.assertFalse(TextProcessor.is_palindrome("hello"))
        self.assertFalse(TextProcessor.is_palindrome("python"))

    def test_palindrome_single_char(self):
        self.assertTrue(TextProcessor.is_palindrome("x"))

    # ---- truncate ----
    def test_truncate_no_truncation_needed(self):
        self.assertEqual(TextProcessor.truncate("hello", 10), "hello")

    def test_truncate_exact_length(self):
        self.assertEqual(TextProcessor.truncate("hello", 5), "hello")

    def test_truncate_with_suffix(self):
        result = TextProcessor.truncate("hello world", 8)
        self.assertEqual(result, "hello...")
        self.assertEqual(len(result), 8)

    def test_truncate_custom_suffix(self):
        result = TextProcessor.truncate("hello world", 7, suffix="…")
        self.assertEqual(result, "hello w…")


# ----- 6.3 更多断言方法展示 -----

class TestAssertMethods(unittest.TestCase):
    """展示 unittest 常用断言方法"""

    def test_equality_assertions(self):
        """相等性断言"""
        self.assertEqual(1 + 1, 2)           # 相等
        self.assertNotEqual(1 + 1, 3)        # 不相等
        self.assertAlmostEqual(0.1 + 0.2, 0.3, places=1)  # 近似相等

    def test_boolean_assertions(self):
        """布尔断言"""
        self.assertTrue(1 < 2)              # 为真
        self.assertFalse(1 > 2)             # 为假

    def test_none_assertions(self):
        """None 断言"""
        self.assertIsNone(None)             # 是 None
        self.assertIsNotNone(42)            # 不是 None

    def test_membership_assertions(self):
        """成员关系断言"""
        self.assertIn(3, [1, 2, 3])         # 在容器中
        self.assertNotIn(4, [1, 2, 3])      # 不在容器中

    def test_type_assertions(self):
        """类型断言"""
        self.assertIsInstance("hello", str)  # 类型检查
        self.assertIsInstance(42, (int, float))

    def test_comparison_assertions(self):
        """比较断言"""
        self.assertGreater(5, 3)            # 大于
        self.assertLess(3, 5)               # 小于
        self.assertGreaterEqual(5, 5)       # 大于等于
        self.assertLessEqual(3, 5)          # 小于等于

    def test_container_assertions(self):
        """容器断言"""
        self.assertListEqual([1, 2, 3], [1, 2, 3])
        self.assertDictEqual({"a": 1}, {"a": 1})
        self.assertSetEqual({1, 2, 3}, {3, 2, 1})
        self.assertTupleEqual((1, 2), (1, 2))


# ----- 6.4 测试套件的组织方式 -----
# 实际项目中，测试文件放在 tests/ 目录：
#
# tests/
# ├── __init__.py
# ├── test_bmi.py         # 对应测试 bmi 模块
# └── test_text.py        # 对应测试 text 模块
#
# 命名规范：
# - 测试文件：test_<模块名>.py
# - 测试类：Test<被测类名>
# - 测试方法：test_<功能描述>（必须以 test_ 开头）
#
# 运行方式：
# python -m unittest discover tests/    # 自动发现并运行所有测试
# python -m unittest test_bmi           # 运行单个测试文件
# python -m pytest                      # 使用 pytest（第三方，更强大）


# ----- 6.5 运行测试并展示结果 -----

def run_tests():
    """运行所有单元测试并打印结果"""
    # 创建测试套件
    loader = unittest.TestLoader()
    suite = unittest.TestSuite()

    # 添加各测试类
    suite.addTests(loader.loadTestsFromTestCase(TestCalculateBMI))
    suite.addTests(loader.loadTestsFromTestCase(TestBMICategory))
    suite.addTests(loader.loadTestsFromTestCase(TestTextProcessor))
    suite.addTests(loader.loadTestsFromTestCase(TestAssertMethods))

    # 使用详细模式运行（verbosity=0 静默, 1 简洁, 2 详细）
    runner = unittest.TextTestRunner(verbosity=1)
    result = runner.run(suite)

    # 输出统计
    total = result.testsRun
    failed = len(result.failures)
    errored = len(result.errors)
    passed = total - failed - errored
    print(f"\n  测试统计: 共 {total} 个测试 | ✓ 通过 {passed} | ✗ 失败 {failed} | ⚠ 错误 {errored}")
    return result.wasSuccessful()


print("运行单元测试:")
print("-" * 40)
test_success = run_tests()
print(f"  测试{'全部通过 ✓' if test_success else '存在失败 ✗'}")


# ============================================================================
# 7. 虚拟环境（venv）
# ============================================================================

print("\n--- 7. 虚拟环境（venv）---")

import sys
import os

# 虚拟环境是独立的 Python 运行环境，用于：
# 1. 隔离项目依赖，避免全局包冲突
# 2. 固定项目的依赖版本
# 3. 便于在不同机器上重现开发环境
#
# Python 3.3+ 内置 venv 模块，无需额外安装。

# ----- 7.1 虚拟环境常用命令 -----
#
# 创建虚拟环境：
#   python -m venv venv              # 在当前目录创建 venv/ 目录
#   python -m venv .venv             # 创建隐藏目录（推荐）
#
# 激活虚拟环境：
#   Windows:
#     venv\Scripts\activate.bat      # CMD
#     venv\Scripts\Activate.ps1      # PowerShell
#   Linux/macOS:
#     source venv/bin/activate       # bash/zsh
#
# 退出虚拟环境：
#   deactivate
#
# 安装依赖：
#   pip install requests             # 安装单个包
#   pip install requests==2.31.0     # 安装指定版本
#   pip install -r requirements.txt  # 从文件批量安装
#
# 导出依赖列表：
#   pip freeze > requirements.txt    # 导出所有已安装包
#   pip freeze | grep -i requests    # 查找特定包版本

# ----- 7.2 requirements.txt 格式 -----
#
# 简单格式（指定精确版本，推荐生产环境）：
#   requests==2.31.0
#   numpy==1.26.2
#   pandas==2.1.4
#
# 灵活格式（指定最低版本）：
#   requests>=2.28.0
#   numpy>=1.24.0
#
# 开发依赖单独维护（requirements-dev.txt）：
#   pytest==7.4.3
#   mypy==1.8.0
#   flake8==7.0.0
#   black==24.1.1

# ----- 7.3 检测当前 Python 环境信息 -----
print("当前 Python 环境信息:")
print(f"  Python 版本: {sys.version.split()[0]}")
print(f"  Python 解释器路径: {sys.executable}")

# 检测是否处于虚拟环境中
in_venv = hasattr(sys, 'real_prefix') or (
    hasattr(sys, 'base_prefix') and sys.base_prefix != sys.prefix
)
print(f"  是否在虚拟环境中: {'是' if in_venv else '否'}")
print(f"  sys.prefix: {sys.prefix}")

# ----- 7.4 使用 venv 模块的 API（程序化创建虚拟环境）-----
import venv

def create_virtual_env_info(env_path: str) -> Dict[str, str]:
    """
    获取虚拟环境相关路径信息（不实际创建）。

    Args:
        env_path: 虚拟环境目录路径。

    Returns:
        包含各路径信息的字典。
    """
    # 使用 EnvBuilder 获取上下文信息（仅计算路径，不创建）
    builder = venv.EnvBuilder()
    context = builder.ensure_directories(env_path)
    return {
        "env_dir": context.env_dir,
        "python": context.env_exe,
        "bin_path": context.bin_path,
    }

# 展示虚拟环境路径（在临时路径上演示）
demo_env_path = os.path.join(os.getcwd(), "demo_venv")
env_info = create_virtual_env_info(demo_env_path)
print("\n  虚拟环境路径示例（未实际创建）:")
print(f"    环境目录: {env_info['env_dir']}")
print(f"    Python:   {env_info['python']}")
print(f"    Bin 目录: {env_info['bin_path']}")

# ----- 7.5 pip 常用命令 -----
#
# 查看已安装包：
#   pip list
#   pip show requests        # 查看某个包的详细信息
#
# 升级包：
#   pip install --upgrade pip
#   pip install --upgrade requests
#
# 卸载包：
#   pip uninstall requests
#
# 搜索包：
#   pip index versions requests   # 查看所有可用版本
#
# 更换国内镜像源（加速下载）：
#   pip install requests -i https://pypi.tuna.tsinghua.edu.cn/simple
#
# 永久配置镜像：
#   pip config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple


# ============================================================================
# 综合实战：应用所有规范编写一个小型计算器模块
# ============================================================================

print("\n--- 综合实战：规范化的计算器模块 ---")

from typing import Optional
import math


class CalculatorError(Exception):
    """计算器专用异常基类"""
    pass


class DivisionByZeroError(CalculatorError):
    """除数为零异常"""
    pass


class InvalidOperationError(CalculatorError):
    """无效操作异常"""
    pass


class Calculator:
    """
    科学计算器类。

    提供基本四则运算和常用数学函数，
    支持历史记录查询和结果清除。

    Attributes:
        precision: 结果保留的小数位数。
        _history: 计算历史记录列表。

    Examples:
        >>> calc = Calculator(precision=2)
        >>> calc.add(3, 4)
        7
        >>> calc.divide(10, 3)
        3.33
        >>> len(calc.history)
        2
    """

    def __init__(self, precision: int = 4) -> None:
        """
        初始化计算器。

        Args:
            precision: 浮点结果保留的小数位数，默认 4 位。
        """
        self.precision = precision
        self._history: List[str] = []

    @property
    def history(self) -> List[str]:
        """只读的历史记录属性"""
        return self._history.copy()

    def _record(self, expression: str, result: float) -> None:
        """记录计算历史（内部方法）"""
        self._history.append(f"{expression} = {result}")

    def _round(self, value: float) -> float:
        """按精度四舍五入（内部方法）"""
        if value == int(value):
            return int(value)
        return round(value, self.precision)

    def add(self, a: float, b: float) -> float:
        """
        加法。

        Args:
            a: 被加数。
            b: 加数。

        Returns:
            a + b 的结果。
        """
        result = self._round(a + b)
        self._record(f"{a} + {b}", result)
        return result

    def subtract(self, a: float, b: float) -> float:
        """
        减法。

        Args:
            a: 被减数。
            b: 减数。

        Returns:
            a - b 的结果。
        """
        result = self._round(a - b)
        self._record(f"{a} - {b}", result)
        return result

    def multiply(self, a: float, b: float) -> float:
        """
        乘法。

        Args:
            a: 被乘数。
            b: 乘数。

        Returns:
            a * b 的结果。
        """
        result = self._round(a * b)
        self._record(f"{a} × {b}", result)
        return result

    def divide(self, a: float, b: float) -> float:
        """
        除法。

        Args:
            a: 被除数。
            b: 除数，不能为 0。

        Returns:
            a / b 的结果。

        Raises:
            DivisionByZeroError: 当 b 为 0 时抛出。
        """
        if b == 0:
            raise DivisionByZeroError(f"除数不能为零: {a} / {b}")
        result = self._round(a / b)
        self._record(f"{a} ÷ {b}", result)
        return result

    def sqrt(self, n: float) -> float:
        """
        平方根。

        Args:
            n: 被开方数，必须非负。

        Returns:
            n 的平方根。

        Raises:
            InvalidOperationError: 当 n 为负数时抛出。
        """
        if n < 0:
            raise InvalidOperationError(f"不能对负数开方: sqrt({n})")
        result = self._round(math.sqrt(n))
        self._record(f"√{n}", result)
        return result

    def power(self, base: float, exp: float) -> float:
        """
        幂运算。

        Args:
            base: 底数。
            exp: 指数。

        Returns:
            base 的 exp 次方。
        """
        result = self._round(base ** exp)
        self._record(f"{base}^{exp}", result)
        return result

    def clear_history(self) -> None:
        """清除计算历史"""
        self._history.clear()

    def print_history(self) -> None:
        """打印计算历史"""
        if not self._history:
            print("  （无历史记录）")
            return
        for i, record in enumerate(self._history, 1):
            print(f"  [{i:2d}] {record}")

    def __repr__(self) -> str:
        return f"Calculator(precision={self.precision}, history_count={len(self._history)})"


# 计算器单元测试
class TestCalculator(unittest.TestCase):
    """Calculator 类的单元测试"""

    def setUp(self):
        self.calc = Calculator(precision=2)

    def test_add(self):
        self.assertEqual(self.calc.add(3, 4), 7)
        self.assertAlmostEqual(self.calc.add(0.1, 0.2), 0.3, places=1)

    def test_subtract(self):
        self.assertEqual(self.calc.subtract(10, 3), 7)

    def test_multiply(self):
        self.assertEqual(self.calc.multiply(6, 7), 42)

    def test_divide_normal(self):
        self.assertEqual(self.calc.divide(10, 2), 5)
        self.assertAlmostEqual(self.calc.divide(10, 3), 3.33, places=2)

    def test_divide_by_zero(self):
        with self.assertRaises(DivisionByZeroError):
            self.calc.divide(10, 0)

    def test_sqrt_positive(self):
        self.assertEqual(self.calc.sqrt(9), 3)
        self.assertAlmostEqual(self.calc.sqrt(2), 1.41, places=2)

    def test_sqrt_negative(self):
        with self.assertRaises(InvalidOperationError):
            self.calc.sqrt(-1)

    def test_power(self):
        self.assertEqual(self.calc.power(2, 10), 1024)

    def test_history_recorded(self):
        self.calc.add(1, 2)
        self.calc.multiply(3, 4)
        self.assertEqual(len(self.calc.history), 2)

    def test_clear_history(self):
        self.calc.add(1, 2)
        self.calc.clear_history()
        self.assertEqual(len(self.calc.history), 0)


# 演示计算器
print("计算器模块演示:")
calc = Calculator(precision=4)

try:
    print(f"  3 + 4 = {calc.add(3, 4)}")
    print(f"  10.5 - 3.2 = {calc.subtract(10.5, 3.2)}")
    print(f"  6 × 7 = {calc.multiply(6, 7)}")
    print(f"  22 ÷ 7 = {calc.divide(22, 7)}")
    print(f"  √2 = {calc.sqrt(2)}")
    print(f"  2^10 = {calc.power(2, 10)}")
    print(f"  1 ÷ 0 = ", end="")
    calc.divide(1, 0)
except DivisionByZeroError as e:
    print(f"错误: {e}")

print("\n计算历史:")
calc.print_history()

# 运行计算器测试
print("\n运行计算器单元测试:")
print("-" * 40)
calc_suite = unittest.TestLoader().loadTestsFromTestCase(TestCalculator)
calc_runner = unittest.TextTestRunner(verbosity=1)
calc_result = calc_runner.run(calc_suite)
total = calc_result.testsRun
passed = total - len(calc_result.failures) - len(calc_result.errors)
print(f"\n  计算器测试: {passed}/{total} 通过")


# ============================================================================
# 本章总结
# ============================================================================

print("\n" + "=" * 60)
print("第 24 章总结")
print("=" * 60)

summary = """
1. 项目目录结构
   - 标准结构：src 包目录 + tests/ + docs/ + README.md
   - 小型项目：main.py + utils.py 即可
   - __init__.py 定义包的公共接口（__all__）

2. 模块化设计原则
   - 单一职责：每模块只做一件事
   - 高内聚低耦合：模块内紧密，模块间松散
   - 接口清晰：用 __all__ 明确公共 API
   - 可测试性：避免全局状态，便于独立测试

3. PEP 8 核心规范
   - 缩进：4 空格
   - 行长：≤79 字符
   - 命名：类用 CapWords，函数/变量用 snake_case，常量全大写
   - 导入：标准库 → 第三方 → 本地，每组空一行
   - 注释：# 后跟一个空格

4. 类型注解（Type Hints）
   - 基本类型：int, str, float, bool
   - 容器：List[T], Dict[K, V], Tuple, Set
   - 特殊：Optional[T], Union[A, B], Callable, Any
   - Python 3.9+：直接用 list[int], dict[str, int]
   - 配合 mypy 进行静态检查

5. 文档字符串（Docstring）
   - 推荐 Google 风格
   - 必要字段：描述、Args、Returns、Raises
   - 可选字段：Examples、Note、Warning

6. 单元测试（unittest）
   - 测试类继承 unittest.TestCase
   - 测试方法以 test_ 开头
   - setUp/tearDown 管理测试夹具
   - 丰富的 assertXxx 断言方法
   - 运行：python -m unittest discover

7. 虚拟环境（venv）
   - 创建：python -m venv venv
   - 激活：Windows: venv\\Scripts\\activate
   - 导出依赖：pip freeze > requirements.txt
   - 安装依赖：pip install -r requirements.txt
"""
print(summary)
