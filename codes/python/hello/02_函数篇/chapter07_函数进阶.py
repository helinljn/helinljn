# =============================================================================
# 第 7 章：函数进阶（lambda、高阶函数、装饰器）
# =============================================================================
#
# 【学习目标】
#   1. 掌握 lambda 匿名函数的使用
#   2. 理解高阶函数（map、filter、reduce）
#   3. 掌握装饰器的原理和应用
#   4. 了解偏函数的使用
#
# 【运行方式】
#   python chapter07_函数进阶.py
#
# =============================================================================

import time
from functools import wraps, reduce, partial


# =============================================================================
# 7.1 Lambda 匿名函数
# =============================================================================

def demo_lambda() -> None:
    """演示 lambda 匿名函数。"""
    print("=" * 60)
    print("7.1 Lambda 匿名函数")
    print("=" * 60)

    # lambda 语法：lambda 参数: 表达式
    square_lambda = lambda x: x ** 2
    add = lambda a, b: a + b

    print(f"square_lambda(5) = {square_lambda(5)}")
    print(f"add(3, 4) = {add(3, 4)}")

    # 典型应用：排序 key
    students = [
        {"name": "Alice", "score": 95},
        {"name": "Bob",   "score": 87},
        {"name": "Charlie", "score": 92},
    ]
    by_score = sorted(students, key=lambda s: s["score"], reverse=True)
    print(f"\n按分数排序: {[s['name'] for s in by_score]}")

    # 三元表达式（条件表达式）
    abs_val = lambda x: x if x >= 0 else -x
    print(f"abs_val(-7) = {abs_val(-7)}")

    # 字典中的 lambda
    ops = {
        "add": lambda a, b: a + b,
        "sub": lambda a, b: a - b,
        "mul": lambda a, b: a * b,
    }
    print(f"ops['mul'](6, 7) = {ops['mul'](6, 7)}")


# =============================================================================
# 7.2 高阶函数
# =============================================================================

def demo_higher_order_functions() -> None:
    """演示高阶函数（map、filter、reduce）。"""
    print("\n" + "=" * 60)
    print("7.2 高阶函数")
    print("=" * 60)

    numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

    # ── map() ────────────────────────────────────────────
    # map(func, iterable)：对每个元素应用函数
    print("1. map() - 映射转换:")
    squared = list(map(lambda x: x ** 2, numbers))
    print(f"  平方: {squared}")

    # 内置函数直接作为参数
    str_nums = ["10", "20", "30"]
    int_nums = list(map(int, str_nums))
    print(f"  字符串→整数: {int_nums}")

    # 多个可迭代对象
    a, b = [1, 2, 3], [10, 20, 30]
    paired_sums = list(map(lambda x, y: x + y, a, b))
    print(f"  对应相加: {paired_sums}")

    # ── filter() ─────────────────────────────────────────
    # filter(func, iterable)：筛选满足条件的元素
    print("\n2. filter() - 过滤筛选:")
    evens = list(filter(lambda x: x % 2 == 0, numbers))
    print(f"  偶数: {evens}")

    # filter(None, ...) 过滤假值
    data = [0, 1, None, 2, "", 3, False, 4, [], 5]
    valid = list(filter(None, data))
    print(f"  过滤假值: {valid}")

    # ── reduce() ─────────────────────────────────────────
    # reduce(func, iterable)：累积计算
    print("\n3. reduce() - 累积计算:")
    total = reduce(lambda acc, x: acc + x, numbers)
    print(f"  求和: {total}")

    product = reduce(lambda acc, x: acc * x, [1, 2, 3, 4, 5])
    print(f"  求积: {product}")

    # 带初始值的 reduce
    total2 = reduce(lambda acc, x: acc + x, numbers, 100)
    print(f"  从100开始求和: {total2}")

    # ── 自定义高阶函数 ───────────────────────────────────
    print("\n4. 自定义高阶函数:")

    def apply_twice(func, value):
        """对值应用函数两次。"""
        return func(func(value))

    result = apply_twice(lambda x: x * 2, 5)
    print(f"  apply_twice(double, 5) = {result}")  # 20

    def compose(*functions):
        """组合多个函数（从右到左执行）。"""
        def inner(value):
            for func in reversed(functions):
                value = func(value)
            return value
        return inner

    # 执行顺序：-5 → *2 → +10
    pipeline = compose(lambda x: x + 10, lambda x: x * 2, lambda x: x - 5)
    result = pipeline(20)  # ((20-5)*2)+10 = 40
    print(f"  compose(+10, *2, -5)(20) = {result}")

    # ── 推导式 vs map/filter ──────────────────────────────
    print("\n5. 推导式 vs map/filter（Python 风格对比）:")
    nums = range(1, 11)
    # map/filter 风格（函数式）
    result_func = list(filter(lambda x: x % 2 == 0, map(lambda x: x ** 2, nums)))
    # 推导式风格（Python 推荐）
    result_comp = [x ** 2 for x in range(1, 11) if x % 2 == 0]
    print(f"  函数式: {result_func}")
    print(f"  推导式: {result_comp}")
    print(f"  结果相同: {result_func == result_comp}")


# =============================================================================
# 7.3 装饰器
# =============================================================================

def demo_decorators() -> None:
    """演示装饰器的原理和应用。"""
    print("\n" + "=" * 60)
    print("7.3 装饰器")
    print("=" * 60)

    # ── 装饰器原理 ───────────────────────────────────────
    print("1. 装饰器原理（手动版）:")

    def my_decorator(func):
        """最简单的装饰器：在函数前后打印信息。"""
        def wrapper():
            print("  [前] 执行前")
            result = func()
            print("  [后] 执行后")
            return result
        return wrapper

    def say_hello():
        print("  Hello!")

    # 手动装饰
    decorated = my_decorator(say_hello)
    decorated()

    # ── @ 语法糖 ─────────────────────────────────────────
    print("\n2. @ 语法糖（等价写法）:")

    @my_decorator
    def say_hi():
        print("  Hi!")

    say_hi()  # 等价于 my_decorator(say_hi)()

    # ── 通用装饰器（支持任意参数）───────────────────────
    print("\n3. 通用装饰器（*args, **kwargs）:")

    def log_calls(func):
        """记录函数调用信息。"""
        @wraps(func)  # 保留原函数的 __name__、__doc__ 等信息
        def wrapper(*args, **kwargs):
            args_str = ", ".join(str(a) for a in args)
            kwargs_str = ", ".join(f"{k}={v}" for k, v in kwargs.items())
            all_args = ", ".join(filter(None, [args_str, kwargs_str]))
            print(f"  调用 {func.__name__}({all_args})")
            result = func(*args, **kwargs)
            print(f"  返回: {result}")
            return result
        return wrapper

    @log_calls
    def add(a: int, b: int) -> int:
        """两数相加。"""
        return a + b

    @log_calls
    def greet(name: str, greeting: str = "Hello") -> str:
        return f"{greeting}, {name}!"

    add(3, 5)
    greet("Alice", greeting="Hi")
    print(f"  函数名保留: {add.__name__}")  # add（而非 wrapper）

    # ── 计时装饰器（实用示例）───────────────────────────
    print("\n4. 计时装饰器（实用示例）:")

    def timer(func):
        """测量函数执行时间。"""
        @wraps(func)
        def wrapper(*args, **kwargs):
            start = time.perf_counter()
            result = func(*args, **kwargs)
            elapsed = time.perf_counter() - start
            print(f"  {func.__name__} 耗时: {elapsed:.6f} 秒")
            return result
        return wrapper

    @timer
    def slow_sum(n: int) -> int:
        """计算 1 到 n 的和（用循环模拟耗时操作）。"""
        total = 0
        for i in range(n):
            total += i
        return total

    result = slow_sum(100_000)
    print(f"  结果: {result}")

    # ── 带参数的装饰器 ───────────────────────────────────
    print("\n5. 带参数的装饰器（三层嵌套）:")

    def repeat(times: int):
        """
        让函数重复执行 times 次的装饰器工厂。
        注意：这是一个返回装饰器的函数（装饰器工厂）。
        """
        def decorator(func):
            @wraps(func)
            def wrapper(*args, **kwargs):
                result = None
                for i in range(times):
                    print(f"  [第{i+1}次]", end=" ")
                    result = func(*args, **kwargs)
                return result
            return wrapper
        return decorator  # 返回装饰器

    @repeat(3)  # 等价于 say_ok = repeat(3)(say_ok)
    def say_ok():
        print("OK!")

    say_ok()

    def validate_positive(func):
        """验证所有参数都为正数。"""
        @wraps(func)
        def wrapper(*args, **kwargs):
            for arg in args:
                if isinstance(arg, (int, float)) and arg <= 0:
                    raise ValueError(f"参数必须为正数，得到: {arg}")
            return func(*args, **kwargs)
        return wrapper

    @validate_positive
    @timer   # 多个装饰器从下到上依次应用
    def compute_sqrt(n: float) -> float:
        """计算平方根。"""
        return n ** 0.5

    print(f"\n6. 叠加装饰器:")
    result = compute_sqrt(16.0)
    print(f"  compute_sqrt(16) = {result}")

    # ── 类装饰器 ─────────────────────────────────────────
    print("\n7. 用类实现装饰器:")

    class CallCounter:
        """记录函数被调用次数的装饰器（用类实现）。"""

        def __init__(self, func):
            wraps(func)(self)  # 保留函数元信息
            self.func = func
            self.call_count = 0

        def __call__(self, *args, **kwargs):
            self.call_count += 1
            print(f"  [{self.func.__name__} 第{self.call_count}次调用]")
            return self.func(*args, **kwargs)

    @CallCounter
    def add_numbers(a, b):
        return a + b

    add_numbers(1, 2)
    add_numbers(3, 4)
    add_numbers(5, 6)
    print(f"  总共调用 {add_numbers.call_count} 次")


# =============================================================================
# 7.4 偏函数（partial）
# =============================================================================

def demo_partial() -> None:
    """演示偏函数的使用。"""
    print("\n" + "=" * 60)
    print("7.4 偏函数（functools.partial）")
    print("=" * 60)

    # partial 允许你"固定"函数的部分参数，创建新函数
    # 类似 C++ 的 std::bind

    # ── 基本用法 ─────────────────────────────────────────
    def power(base: float, exponent: int) -> float:
        return base ** exponent

    square = partial(power, exponent=2)   # 固定 exponent=2
    cube = partial(power, exponent=3)     # 固定 exponent=3

    print(f"square(5) = {square(5)}")    # power(5, 2) = 25
    print(f"cube(3)   = {cube(3)}")      # power(3, 3) = 27

    # ── 实际应用场景 ─────────────────────────────────────
    print("\n实际应用场景:")

    # 1. 固定日志级别
    def log(level: str, message: str, *details) -> None:
        detail_str = " | ".join(str(d) for d in details)
        print(f"  [{level}] {message}" + (f" | {detail_str}" if details else ""))

    info = partial(log, "INFO")
    error = partial(log, "ERROR")
    warning = partial(log, "WARNING")

    info("服务启动成功")
    error("数据库连接失败", "host=localhost", "port=5432")
    warning("内存使用率超过80%")

    # 2. 固定编码参数
    import json
    dump_compact = partial(json.dumps, separators=(",", ":"), ensure_ascii=False)
    dump_pretty = partial(json.dumps, indent=2, ensure_ascii=False)

    data = {"name": "张三", "age": 30, "skills": ["Python", "Go"]}
    print(f"\n  紧凑格式: {dump_compact(data)}")
    print(f"  美化格式:\n{dump_pretty(data)}")

    # 3. 批量转换
    numbers = ["1", "2", "3", "4", "5"]
    hex_to_int = partial(int, base=16)   # 十六进制字符串转整数
    hex_numbers = ["ff", "1a", "2b", "0c", "3f"]
    int_values = list(map(hex_to_int, hex_numbers))
    print(f"\n  十六进制转整数: {int_values}")


# =============================================================================
# 7.5 综合示例：函数式数据处理管道
# =============================================================================

def demo_pipeline() -> None:
    """演示使用函数式编程处理数据。"""
    print("\n" + "=" * 60)
    print("7.5 综合示例：数据处理管道")
    print("=" * 60)

    # 模拟学生成绩数据
    raw_data = [
        "Alice,85,92,78,90",
        "Bob,70,65,80,75",
        "Charlie,95,88,92,97",
        "David,60,72,68,55",
        "Eve,88,91,85,93",
        "Frank,45,50,60,55",
    ]

    # ── 步骤1：解析原始数据 ──────────────────────────────
    def parse_record(line: str) -> dict:
        """将 CSV 行解析为字典。"""
        parts = line.split(",")
        name = parts[0]
        scores = list(map(int, parts[1:]))
        return {
            "name": name,
            "scores": scores,
            "average": sum(scores) / len(scores),
        }

    # ── 步骤2：计算等级 ──────────────────────────────────
    def assign_grade(record: dict) -> dict:
        """根据平均分分配等级。"""
        avg = record["average"]
        grade = (
            "A" if avg >= 90 else
            "B" if avg >= 80 else
            "C" if avg >= 70 else
            "D" if avg >= 60 else
            "F"
        )
        return {**record, "grade": grade}

    # ── 步骤3：添加状态 ──────────────────────────────────
    def assign_status(record: dict) -> dict:
        """根据等级分配通过/失败状态。"""
        status = "通过" if record["grade"] != "F" else "不通过"
        return {**record, "status": status}

    # ── 构建处理管道 ─────────────────────────────────────
    # 逐步处理数据
    students = list(map(parse_record, raw_data))
    students = list(map(assign_grade, students))
    students = list(map(assign_status, students))

    # 筛选通过的学生，按平均分排序
    passed = sorted(
        filter(lambda s: s["status"] == "通过", students),
        key=lambda s: s["average"],
        reverse=True,
    )

    # ── 输出报告 ─────────────────────────────────────────
    print("成绩报告（通过学生，按平均分排名）:")
    print(f"  {'排名':<4} {'姓名':<10} {'平均分':<8} {'等级':<4} {'状态'}")
    print("  " + "-" * 40)
    for rank, s in enumerate(passed, 1):
        print(f"  {rank:<4} {s['name']:<10} {s['average']:<8.1f} {s['grade']:<4} {s['status']}")

    # 统计各等级人数
    grade_counts: dict[str, int] = {}
    for s in students:
        grade_counts[s["grade"]] = grade_counts.get(s["grade"], 0) + 1

    print(f"\n各等级分布: {grade_counts}")

    # 使用 reduce 计算全班平均分
    total_avg = reduce(lambda acc, s: acc + s["average"], students, 0.0)
    class_avg = total_avg / len(students)
    print(f"全班平均分: {class_avg:.1f}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_lambda()
    demo_higher_order_functions()
    demo_decorators()
    demo_partial()
    demo_pipeline()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── lambda ──
# f = lambda x: x * 2
# f = lambda x, y: x + y
# f = lambda x: x if x > 0 else -x  # 三元表达式
#
# ── 高阶函数 ──
# map(func, iterable)          → 映射
# filter(func, iterable)       → 过滤
# reduce(func, iterable, init) → 累积（from functools）
# sorted(lst, key=func, reverse=True)
#
# ── 装饰器 ──
# def decorator(func):
#     @wraps(func)
#     def wrapper(*args, **kwargs):
#         # 前置操作
#         result = func(*args, **kwargs)
#         # 后置操作
#         return result
#     return wrapper
#
# @decorator  # 等价于 func = decorator(func)
# def func(): ...
#
# ── 带参数的装饰器（三层）──
# def decorator_factory(param):
#     def decorator(func):
#         @wraps(func)
#         def wrapper(*args, **kwargs):
#             return func(*args, **kwargs)
#         return wrapper
#     return decorator
#
# @decorator_factory(value)
# def func(): ...
#
# ── 偏函数 ──
# from functools import partial
# new_func = partial(original_func, fixed_arg1, keyword=fixed_val)


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：装饰器忘记 @wraps(func)
# def decorator(func):
#     def wrapper(*args, **kwargs):  ❌ func.__name__ 会变成 'wrapper'
#         ...
#
# 错误 2：带参数装饰器少写一层
# def repeat(n):
#     @wraps(func)  ❌ func 未定义
#     def wrapper(...): ...
# 正确：repeat → decorator → wrapper 三层
#
# 错误 3：lambda 中使用循环变量（闭包陷阱）
# funcs = [lambda x: x + i for i in range(3)]  # ❌ 所有 i 都是 2
# funcs = [lambda x, i=i: x + i for i in range(3)]  # ✅ 用默认参数捕获


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   使用 map 和 filter 实现：给定一个字符串列表，
#   将所有字符串转为大写，然后只保留长度大于 3 的字符串
#
# 练习 2：
#   写一个装饰器 retry(times=3, delay=0)，
#   当被装饰函数抛出异常时，自动重试最多 times 次
#
# 练习 3：
#   写一个装饰器 cache，实现函数结果缓存（类似 functools.lru_cache）
#   提示：用字典存储参数→结果的映射


# =============================================================================
# 【练习答案】
# =============================================================================

def exercise1_answer():
    """练习 1：map + filter 处理字符串。"""
    words = ["hi", "hello", "python", "go", "java", "c"]
    result = list(filter(
        lambda s: len(s) > 3,
        map(str.upper, words)
    ))
    # 推导式等价写法：
    result2 = [s.upper() for s in words if len(s) > 3]
    return result


def retry(times: int = 3, delay: float = 0):
    """练习 2：重试装饰器。"""
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            last_exception = None
            for attempt in range(1, times + 1):
                try:
                    return func(*args, **kwargs)
                except Exception as e:
                    last_exception = e
                    print(f"  第 {attempt} 次失败: {e}")
                    if attempt < times and delay > 0:
                        time.sleep(delay)
            raise last_exception
        return wrapper
    return decorator


def simple_cache(func):
    """练习 3：简单缓存装饰器。"""
    _cache: dict = {}

    @wraps(func)
    def wrapper(*args, **kwargs):
        # 将参数转为可哈希的键
        key = (args, tuple(sorted(kwargs.items())))
        if key not in _cache:
            _cache[key] = func(*args, **kwargs)
            print(f"  [缓存] 计算并存储 {func.__name__}{args}")
        else:
            print(f"  [缓存] 命中缓存 {func.__name__}{args}")
        return _cache[key]

    wrapper.cache = _cache
    wrapper.cache_clear = lambda: _cache.clear()
    return wrapper


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print(exercise1_answer())  # ['HELLO', 'PYTHON', 'JAVA']
#
#     @retry(times=3)
#     def unstable_func():
#         import random
#         if random.random() < 0.7:
#             raise ValueError("随机失败")
#         return "成功"
#
#     try:
#         print(unstable_func())
#     except ValueError:
#         print("三次都失败了")
#
#     @simple_cache
#     def fib(n):
#         if n <= 1: return n
#         return fib(n - 1) + fib(n - 2)
#     print(fib(10))
