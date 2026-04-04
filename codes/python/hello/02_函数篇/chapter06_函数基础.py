# =============================================================================
# 第 6 章：函数基础
# =============================================================================
#
# 【学习目标】
#   1. 掌握函数的定义和调用
#   2. 理解各种参数类型（位置、关键字、默认值、可变参数）
#   3. 掌握返回值的使用
#   4. 理解作用域规则（LEGB）
#   5. 了解类型注解（Type Hints）
#
# 【运行方式】
#   python chapter06_函数基础.py
#
# =============================================================================


# =============================================================================
# 6.1 函数定义与调用
# =============================================================================

def demo_function_basics() -> None:
    """演示函数的基本定义和调用。"""
    print("=" * 60)
    print("6.1 函数定义与调用")
    print("=" * 60)

    # ── 基本函数定义 ─────────────────────────────────────
    # Python 中函数用 def 关键字定义
    # 函数体通过缩进区分（和 C++ 的 {} 不同）

    def greet(name: str) -> str:
        """
        生成问候语。

        这是文档字符串（docstring），说明函数用途。
        习惯上写在函数体第一行，用三引号括起来。

        Args:
            name: 要问候的人名。

        Returns:
            包含问候语的字符串。
        """
        return f"你好，{name}！"

    # 调用函数
    result = greet("Alice")
    print(f"greet('Alice') = {result!r}")

    # 访问文档字符串
    print(f"函数文档: {greet.__doc__[:10]}...")

    # ── 无返回值的函数 ───────────────────────────────────
    def print_separator(char: str = "-", width: int = 40) -> None:
        """打印分隔线。None 作为返回类型注解表示无返回值（类似 C++ 的 void）。"""
        print(char * width)

    print_separator()
    print_separator("=", 20)

    # Python 函数总是有返回值，无 return 语句时返回 None
    def no_return():
        x = 1 + 1   # 有代码，但没有 return

    result = no_return()
    print(f"\nno_return() 的返回值: {result}")   # None

    # ── 函数是一等公民 ───────────────────────────────────
    # Python 中函数是对象，可以赋值给变量、作为参数传递、存入容器
    # 这与 C++ 的函数指针类似，但更灵活

    def square(x: int) -> int:
        return x * x

    # 赋值给变量
    my_func = square
    print(f"\n函数赋值: my_func(5) = {my_func(5)}")

    # 存入列表
    operations = [square, abs, lambda x: x + 1]
    results = [f(4) for f in operations]
    print(f"函数列表: {results}")


# =============================================================================
# 6.2 参数类型详解
# =============================================================================

def demo_parameters() -> None:
    """演示各种参数类型。"""
    print("\n" + "=" * 60)
    print("6.2 参数类型详解")
    print("=" * 60)

    # ── 位置参数 ─────────────────────────────────────────
    print("1. 位置参数（按顺序传入）:")

    def power(base: float, exponent: int) -> float:
        """计算 base 的 exponent 次方。"""
        return base ** exponent

    print(f"  power(2, 3) = {power(2, 3)}")    # 8
    print(f"  power(3, 2) = {power(3, 2)}")    # 9

    # ── 关键字参数 ───────────────────────────────────────
    print("\n2. 关键字参数（按名字传入，顺序可变）:")
    print(f"  power(exponent=3, base=2) = {power(exponent=3, base=2)}")  # 8
    print(f"  power(2, exponent=3)      = {power(2, exponent=3)}")       # 混合

    # ── 默认参数 ─────────────────────────────────────────
    print("\n3. 默认参数（有默认值的参数）:")

    def connect(host: str, port: int = 8080, timeout: float = 30.0) -> str:
        """模拟连接服务器。"""
        return f"连接 {host}:{port}（超时={timeout}s）"

    print(f"  {connect('example.com')}")              # 使用默认值
    print(f"  {connect('example.com', 443)}")          # 覆盖 port
    print(f"  {connect('example.com', timeout=5.0)}")  # 跳过 port，覆盖 timeout

    # ⚠️ 重要警告：不要用可变对象作为默认参数！
    print("\n  ⚠️ 默认参数陷阱:")

    def bad_append(item, lst=[]):  # ❌ 默认参数只创建一次！
        """错误示例：以可变对象作为默认参数。"""
        lst.append(item)
        return lst

    print(f"  bad_append(1) = {bad_append(1)}")   # [1]
    print(f"  bad_append(2) = {bad_append(2)}")   # [1, 2]（不是 [2]！）
    print(f"  bad_append(3) = {bad_append(3)}")   # [1, 2, 3]（越来越多！）

    def good_append(item, lst: list | None = None) -> list:  # ✅ 正确做法
        """正确示例：用 None 作为默认值，函数内部创建列表。"""
        if lst is None:
            lst = []
        lst.append(item)
        return lst

    print(f"  good_append(1) = {good_append(1)}")  # [1]
    print(f"  good_append(2) = {good_append(2)}")  # [2]（正确！）

    # ── 可变位置参数（*args）────────────────────────────
    print("\n4. *args（收集多余的位置参数为元组）:")

    def summarize(*args: int) -> dict:
        """统计任意个数字。"""
        if not args:
            return {"count": 0, "sum": 0, "avg": 0}
        return {
            "count": len(args),
            "sum":   sum(args),
            "avg":   sum(args) / len(args),
        }

    print(f"  summarize() = {summarize()}")
    print(f"  summarize(1,2,3) = {summarize(1, 2, 3)}")
    print(f"  summarize(1,2,3,4,5) = {summarize(1, 2, 3, 4, 5)}")

    # 解包传入：用 * 展开列表/元组作为位置参数
    nums = [10, 20, 30, 40]
    print(f"  *解包: summarize(*nums) = {summarize(*nums)}")

    # ── 可变关键字参数（**kwargs）───────────────────────
    print("\n5. **kwargs（收集多余的关键字参数为字典）:")

    def create_user(name: str, **kwargs) -> dict:
        """创建用户，支持任意附加属性。"""
        user = {"name": name}
        user.update(kwargs)
        return user

    user1 = create_user("Alice")
    user2 = create_user("Bob", age=25, city="上海", role="admin")
    print(f"  user1 = {user1}")
    print(f"  user2 = {user2}")

    # **解包：用 ** 展开字典作为关键字参数
    params = {"age": 30, "city": "北京"}
    user3 = create_user("Charlie", **params)
    print(f"  **解包: user3 = {user3}")

    # ── 组合所有参数类型 ─────────────────────────────────
    print("\n6. 组合所有参数类型:")

    def full_example(pos1, pos2, /, normal, *, kw_only, **kwargs):
        """
        参数类型综合示例。
        /  之前：仅限位置参数（Python 3.8+）
        *  之后：仅限关键字参数
        """
        return f"pos1={pos1}, pos2={pos2}, normal={normal}, kw_only={kw_only}, extras={kwargs}"

    result = full_example(1, 2, normal=3, kw_only=4, extra1=5, extra2=6)
    print(f"  {result}")

    # 常用的参数顺序：
    # def func(位置参数, 有默认值的参数, *args, 仅关键字参数, **kwargs)
    def log_message(level: str, message: str, *args,
                    timestamp: bool = True, **meta) -> str:
        """通用日志函数示例。"""
        parts = [f"[{level}]"]
        if timestamp:
            parts.append("[有时间戳]")
        parts.append(message)
        if args:
            parts.append(f"({', '.join(str(a) for a in args)})")
        if meta:
            parts.append(str(meta))
        return " ".join(parts)

    print(f"  {log_message('INFO', '服务启动')}")
    print(f"  {log_message('ERROR', '连接失败', 'addr=192.168.1.1', timestamp=False, code=503)}")


# =============================================================================
# 6.3 返回值
# =============================================================================

def demo_return_values() -> None:
    """演示返回值的各种用法。"""
    print("\n" + "=" * 60)
    print("6.3 返回值")
    print("=" * 60)

    # ── 单个返回值 ───────────────────────────────────────
    def add(a: int, b: int) -> int:
        return a + b

    print(f"add(3, 4) = {add(3, 4)}")

    # ── 多个返回值（实际上是返回元组）──────────────────
    def divide(dividend: float, divisor: float) -> tuple[float, float]:
        """返回商和余数。"""
        quotient = dividend // divisor
        remainder = dividend % divisor
        return quotient, remainder   # 实际上是返回元组

    q, r = divide(17, 5)   # 解包接收
    print(f"\ndivide(17, 5): 商={q}, 余数={r}")

    result_tuple = divide(17, 5)
    print(f"不解包时: {result_tuple}（类型: {type(result_tuple).__name__}）")

    # ── 提前返回（卫语句）───────────────────────────────
    def safe_sqrt(x: float) -> float | None:
        """计算平方根，输入为负数时返回 None。"""
        if x < 0:
            return None   # 提前返回（卫语句），避免深层嵌套
        return x ** 0.5

    print(f"\nsafe_sqrt(9) = {safe_sqrt(9)}")
    print(f"safe_sqrt(-1) = {safe_sqrt(-1)}")

    # ── 返回函数（高阶函数）──────────────────────────────
    def make_multiplier(factor: int):
        """返回一个乘法函数（闭包）。"""
        def multiplier(x: int) -> int:
            return x * factor   # 访问外层函数的变量 factor
        return multiplier       # 返回函数对象

    double = make_multiplier(2)
    triple = make_multiplier(3)
    print(f"\ndouble(5) = {double(5)}")
    print(f"triple(5) = {triple(5)}")
    print(f"double(triple(4)) = {double(triple(4))}")


# =============================================================================
# 6.4 作用域（LEGB 规则）
# =============================================================================

def demo_scope() -> None:
    """演示变量作用域规则。"""
    print("\n" + "=" * 60)
    print("6.4 作用域（LEGB 规则）")
    print("=" * 60)

    # Python 的作用域查找顺序：
    # L（Local）    → 本地（函数内部）
    # E（Enclosing）→ 外层函数（闭包）
    # G（Global）   → 模块全局
    # B（Built-in） → 内置（如 len、print）

    # ── 全局变量 ─────────────────────────────────────────
    global_var = "我是全局变量"

    def show_global():
        print(f"函数内访问全局变量: {global_var}")   # 可以读取

    show_global()

    def modify_global():
        global global_var   # 声明要修改全局变量（不推荐滥用）
        global_var = "被修改的全局变量"

    modify_global()
    print(f"修改后全局变量: {global_var}")

    # ── 局部变量 ─────────────────────────────────────────
    def local_demo():
        local_var = "我是局部变量"
        print(f"  函数内: {local_var}")

    local_demo()
    # print(local_var)  ❌ NameError：函数外无法访问局部变量

    # ── 闭包（Enclosing 作用域）─────────────────────────
    print("\n闭包演示:")

    def outer():
        count = 0   # 外层函数的局部变量

        def inner():
            nonlocal count   # 声明要修改外层变量
            count += 1
            return count

        return inner

    counter = outer()
    print(f"  counter() = {counter()}")   # 1
    print(f"  counter() = {counter()}")   # 2
    print(f"  counter() = {counter()}")   # 3

    counter2 = outer()   # 新的闭包，count 从0开始
    print(f"  counter2() = {counter2()}")  # 1

    # ── 变量遮蔽 ─────────────────────────────────────────
    print("\n变量遮蔽:")
    x = "全局x"

    def shadow():
        x = "局部x"    # 遮蔽全局变量（不是修改）
        print(f"  函数内 x = {x!r}")

    shadow()
    print(f"  函数外 x = {x!r}")   # 全局的 x 未受影响


# =============================================================================
# 6.5 类型注解（Type Hints）
# =============================================================================

def demo_type_hints() -> None:
    """演示 Python 的类型注解。"""
    print("\n" + "=" * 60)
    print("6.5 类型注解（Type Hints）")
    print("=" * 60)

    # 类型注解是 Python 3.5+ 引入的特性
    # 注意：Python 是动态语言，类型注解只是"提示"，不强制检查
    # 类型注解的作用：提高代码可读性、IDE 智能提示、静态检查工具

    # ── 基本类型注解 ─────────────────────────────────────
    def add(a: int, b: int) -> int:
        return a + b

    def greet(name: str) -> str:
        return f"Hello, {name}"

    def is_adult(age: int) -> bool:
        return age >= 18

    # ── 集合类型注解 ─────────────────────────────────────
    # Python 3.9+ 可以直接用 list[int]、dict[str, int] 等
    # 3.9 之前需要 from typing import List, Dict

    def process_names(names: list[str]) -> dict[str, int]:
        """返回每个名字的长度。"""
        return {name: len(name) for name in names}

    result = process_names(["Alice", "Bob", "Charlie"])
    print(f"process_names: {result}")

    # ── 可选类型（值可以为 None）────────────────────────
    # Python 3.10+ 用 X | None，之前用 Optional[X]

    def find_user(user_id: int) -> dict | None:
        """查找用户，找不到时返回 None。"""
        users = {1: {"name": "Alice"}, 2: {"name": "Bob"}}
        return users.get(user_id)

    print(f"find_user(1)  = {find_user(1)}")
    print(f"find_user(99) = {find_user(99)}")

    # ── Union 类型（值可以是多种类型之一）──────────────
    def format_value(value: int | float | str) -> str:
        """格式化各种类型的值。"""
        if isinstance(value, (int, float)):
            return f"{value:.2f}"
        return str(value)

    print(f"\nformat_value(42)   = {format_value(42)!r}")
    print(f"format_value(3.14) = {format_value(3.14)!r}")
    print(f"format_value('hi') = {format_value('hi')!r}")

    # ── 类型注解只是提示，不强制执行 ────────────────────
    def typed_add(a: int, b: int) -> int:
        return a + b

    # Python 不会阻止你这样调用：
    result2 = typed_add("hello", " world")   # 实际运行不报错！
    print(f"\n类型注解不强制: typed_add('hello',' world') = {result2!r}")

    # 要进行严格类型检查，需要使用 mypy 等工具（本课程不涉及）


# =============================================================================
# 6.6 综合示例：计算器函数库
# =============================================================================

def demo_calculator() -> None:
    """演示综合示例：构建一个简单的计算器函数库。"""
    print("\n" + "=" * 60)
    print("6.6 综合示例：计算器函数库")
    print("=" * 60)

    def validate_numbers(*args: float) -> None:
        """验证所有参数都是数字，否则抛出异常（第11章会详细讲异常）。"""
        for arg in args:
            if not isinstance(arg, (int, float)):
                raise TypeError(f"期望数字，得到 {type(arg).__name__}: {arg!r}")

    def safe_divide(a: float, b: float) -> float:
        """安全除法：除数为0时返回 0。"""
        validate_numbers(a, b)
        if b == 0:
            print("  ⚠️ 警告：除数为0，返回0")
            return 0.0
        return a / b

    def calculate(operation: str, *args: float) -> float:
        """
        通用计算函数。

        Args:
            operation: 操作符（'add', 'sub', 'mul', 'div', 'avg'）
            *args:      参与计算的数字

        Returns:
            计算结果

        Raises:
            ValueError: 操作符不支持或参数数量不足
        """
        validate_numbers(*args)
        if not args:
            raise ValueError("至少需要一个数字")

        operations = {
            "add": lambda nums: sum(nums),
            "sub": lambda nums: nums[0] - sum(nums[1:]),
            "mul": lambda nums: __import__('math').prod(nums),
            "div": lambda nums: safe_divide(nums[0], nums[1]) if len(nums) >= 2 else nums[0],
            "avg": lambda nums: sum(nums) / len(nums),
            "max": lambda nums: max(nums),
            "min": lambda nums: min(nums),
        }

        if operation not in operations:
            raise ValueError(f"不支持的操作: {operation!r}。支持: {list(operations.keys())}")

        return operations[operation](args)

    # 测试计算器
    test_cases = [
        ("add", 1, 2, 3, 4, 5),
        ("sub", 100, 20, 30),
        ("avg", 85, 90, 78, 92, 88),
        ("max", 3, 1, 4, 1, 5, 9, 2, 6),
        ("div", 10, 3),
        ("div", 10, 0),   # 除零测试
    ]

    for case in test_cases:
        op = case[0]
        nums = case[1:]
        result = calculate(op, *nums)
        print(f"  {op}({', '.join(str(n) for n in nums)}) = {result:.2f}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_function_basics()
    demo_parameters()
    demo_return_values()
    demo_scope()
    demo_type_hints()
    demo_calculator()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 函数定义 ──
# def func_name(参数) -> 返回类型:
#     """文档字符串"""
#     函数体
#     return 值
#
# ── 参数类型（按顺序）──
# def f(pos_only, /, normal, default=val, *args, kw_only, **kwargs):
#   pos_only:  仅位置参数（/ 之前，Python 3.8+）
#   normal:    普通参数（可位置可关键字）
#   default:   有默认值的参数
#   *args:     可变位置参数（打包为元组）
#   kw_only:   仅关键字参数（* 之后）
#   **kwargs:  可变关键字参数（打包为字典）
#
# ── 解包传参 ──
# func(*list_or_tuple)   展开为位置参数
# func(**dict)           展开为关键字参数
#
# ── 作用域（LEGB）──
# Local → Enclosing → Global → Built-in
# global x    → 声明修改全局变量
# nonlocal x  → 声明修改外层函数变量
#
# ── 类型注解（Python 3.10+ 语法）──
# x: int = 5
# def f(a: int, b: str = "hi") -> list[int]: ...
# def f(x: int | None) -> dict[str, list[int]]: ...


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：可变对象作为默认参数
# def f(lst=[]):  ❌
# def f(lst=None): if lst is None: lst = []  ✅
#
# 错误 2：在函数外访问局部变量
# def f(): x = 1
# print(x)  ❌ NameError
#
# 错误 3：修改全局变量忘记 global 声明
# x = 0
# def f(): x = 1  ❌（只是创建了局部变量，没有修改全局的 x）
# def f(): global x; x = 1  ✅
#
# 错误 4：*args 和 **kwargs 之前忘记 *
# def f(a, b, args):  # args 是普通参数，不是可变参数
# def f(a, b, *args): ✅


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   写一个函数 flatten(lst, depth=None)，
#   将嵌套列表展平（depth 控制展开深度，None 表示完全展平）
#   例如：flatten([1, [2, [3, [4]]]]) → [1, 2, 3, 4]
#          flatten([1, [2, [3]]], depth=1) → [1, 2, [3]]
#
# 练习 2：
#   写一个函数 memoize(func)，
#   为任意函数添加缓存功能（相同参数只计算一次）
#   测试：用它包装斐波那契函数，观察性能差异
#
# 练习 3：
#   写一个 make_counter(start=0, step=1) 函数，
#   返回一个 counter 函数，每次调用 counter() 返回当前值并增加 step
#   counter 还支持 counter(reset=True) 重置到 start


# =============================================================================
# 【练习答案】
# =============================================================================

def flatten(lst: list, depth: int | None = None) -> list:
    """练习 1：展平嵌套列表。"""
    result = []
    for item in lst:
        if isinstance(item, list) and (depth is None or depth > 0):
            new_depth = None if depth is None else depth - 1
            result.extend(flatten(item, new_depth))
        else:
            result.append(item)
    return result


def memoize(func):
    """练习 2：为函数添加缓存。"""
    cache: dict = {}

    def wrapper(*args):
        if args not in cache:
            cache[args] = func(*args)
        return cache[args]

    wrapper.cache = cache   # 暴露缓存字典，方便调试
    return wrapper


def make_counter(start: int = 0, step: int = 1):
    """练习 3：创建计数器闭包。"""
    current = start

    def counter(reset: bool = False) -> int:
        nonlocal current
        if reset:
            current = start
            return current
        value = current
        current += step
        return value

    return counter


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print(flatten([1, [2, [3, [4]]]]))           # [1,2,3,4]
#     print(flatten([1, [2, [3]]], depth=1))        # [1,2,[3]]
#
#     @memoize
#     def fib(n):
#         if n <= 1: return n
#         return fib(n-1) + fib(n-2)
#     print([fib(i) for i in range(10)])            # [0,1,1,2,3,5,8,13,21,34]
#     print(fib.cache)
#
#     c = make_counter(0, 2)
#     print(c(), c(), c())   # 0, 2, 4
#     print(c(reset=True))   # 0
#     print(c())             # 2
