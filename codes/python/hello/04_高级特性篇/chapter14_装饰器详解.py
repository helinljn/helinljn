# =============================================================================
# 第 14 章：装饰器详解
# =============================================================================
#
# 【学习目标】
#   1. 深入理解装饰器的本质和工作原理
#   2. 掌握函数装饰器的各种形式
#   3. 学会编写带参数的装饰器
#   4. 了解类装饰器的使用
#   5. 掌握装饰器的实际应用场景
#
# 【装饰器是什么？】
#   - 装饰器是一个接受函数作为参数并返回新函数的函数
#   - 本质上是一个闭包，用于在不修改原函数代码的情况下增强函数功能
#   - 使用 @decorator_name 语法糖简化装饰器的应用
#   - 装饰器在函数定义时就会执行，而不是在函数调用时
#
# 【与 C/C++ 的对比】
#   C++:  没有装饰器概念，类似功能需要通过函数指针、宏或模板实现
#   Python: 装饰器更加灵活和优雅，是 Python 的重要特性
#
# 【运行方式】
#   python chapter14_装饰器详解.py
#
# =============================================================================

import time
from functools import wraps, lru_cache
from typing import Callable, Any, TypeVar

# 类型变量
F = TypeVar('F', bound=Callable[..., Any])


# =============================================================================
# 14.1 装饰器基础回顾
# =============================================================================

def demo_basic_decorator() -> None:
    """演示装饰器基础。"""
    print("=" * 60)
    print("14.1 装饰器基础回顾")
    print("=" * 60)

    # ── 最简单的装饰器 ───────────────────────────────────
    def simple_decorator(func: F) -> F:
        """简单装饰器：在函数执行前后打印信息。"""
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            print(f"  [装饰器] 准备调用函数: {func.__name__}")
            result = func(*args, **kwargs)
            print(f"  [装饰器] 函数调用完成: {func.__name__}")
            return result
        return wrapper  # type: ignore

    # ── 方式1：手动装饰 ──────────────────────────────────
    print("1. 手动应用装饰器:")

    def greet(name: str) -> str:
        """问候函数"""
        print(f"    你好, {name}!")
        return f"问候了 {name}"

    greet_decorated = simple_decorator(greet)
    result = greet_decorated("张三")
    print(f"  返回值: {result}\n")

    # ── 方式2：使用 @ 语法糖 ──────────────────────────────
    print("2. 使用 @ 语法糖（等价写法）:")

    @simple_decorator
    def farewell(name: str) -> str:
        """告别函数"""
        print(f"    再见, {name}!")
        return f"告别了 {name}"

    # @ 语法糖等价于: farewell = simple_decorator(farewell)
    result = farewell("李四")
    print(f"  返回值: {result}\n")

    # ── 装饰器执行时机 ───────────────────────────────────
    print("3. 装饰器在定义时执行（不是调用时）:")

    def log_creation(func: F) -> F:
        print(f"  [定义时] 函数 '{func.__name__}' 被装饰了！")
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            return func(*args, **kwargs)
        return wrapper  # type: ignore

    print("  --- 开始定义函数 ---")

    @log_creation
    def my_func() -> None:
        pass

    print("  --- 函数定义完成 ---")
    print("  现在调用函数:")
    my_func()


# =============================================================================
# 14.2 保留函数元信息
# =============================================================================

def demo_functools_wraps() -> None:
    """演示如何保留函数元信息。"""
    print("\n" + "=" * 60)
    print("14.2 保留函数元信息（functools.wraps）")
    print("=" * 60)

    # ── 问题：装饰器会丢失原函数的元信息 ─────────────────
    print("1. 不好的装饰器（丢失元信息）:")

    def bad_decorator(func: F) -> F:
        """不好的装饰器：会丢失原函数信息"""
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            return func(*args, **kwargs)
        return wrapper  # type: ignore

    @bad_decorator
    def my_function() -> None:
        """这是我的函数文档"""
        pass

    print(f"  函数名: {my_function.__name__}")   # 输出: wrapper（错误！）
    print(f"  函数文档: {my_function.__doc__}")  # 输出: None（错误！）

    # ── 解决方案：使用 functools.wraps ───────────────────
    print("\n2. 好的装饰器（保留元信息）:")

    def good_decorator(func: F) -> F:
        """好的装饰器：保留原函数信息"""
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            return func(*args, **kwargs)
        return wrapper  # type: ignore

    @good_decorator
    def my_better_function() -> None:
        """这是我的更好的函数文档"""
        pass

    print(f"  函数名: {my_better_function.__name__}")   # 正确！
    print(f"  函数文档: {my_better_function.__doc__}")  # 正确！

    # ── wraps 保留的属性 ─────────────────────────────────
    print("\n3. wraps 保留的属性:")
    print(f"  __name__:    {my_better_function.__name__}")
    print(f"  __doc__:     {my_better_function.__doc__}")
    print(f"  __module__:  {my_better_function.__module__}")
    print(f"  __wrapped__: {my_better_function.__wrapped__}")


# =============================================================================
# 14.3 带参数的装饰器
# =============================================================================

def demo_parameterized_decorator() -> None:
    """演示带参数的装饰器。"""
    print("\n" + "=" * 60)
    print("14.3 带参数的装饰器")
    print("=" * 60)

    # ── 带参数的装饰器需要三层函数 ───────────────────────
    print("1. 重复执行装饰器（带参数）:")

    def repeat(times: int) -> Callable[[F], F]:
        """重复执行装饰器：让函数重复执行指定次数。"""
        def decorator(func: F) -> F:
            @wraps(func)
            def wrapper(*args: Any, **kwargs: Any) -> Any:
                result = None
                for i in range(times):
                    print(f"  第 {i + 1}/{times} 次执行:")
                    result = func(*args, **kwargs)
                return result
            return wrapper  # type: ignore
        return decorator

    @repeat(times=3)
    def say_hello(name: str) -> str:
        """问候函数"""
        print(f"    你好, {name}!")
        return f"问候了 {name}"

    result = say_hello("王五")
    print(f"  最终返回值: {result}\n")

    # ── 三层函数结构说明 ─────────────────────────────────
    print("2. 三层函数结构说明:")
    print("  repeat(times=3)       → 最外层：接收装饰器参数，返回 decorator")
    print("  decorator(func)       → 中间层：接收被装饰函数，返回 wrapper")
    print("  wrapper(*args)        → 最内层：实际执行逻辑")
    print()

    # ── 带可选参数的装饰器 ───────────────────────────────
    print("3. 带默认值的可选参数装饰器:")

    def prefix_print(prefix: str = ">>") -> Callable[[F], F]:
        """给函数输出添加前缀的装饰器。"""
        def decorator(func: F) -> F:
            @wraps(func)
            def wrapper(*args: Any, **kwargs: Any) -> Any:
                print(f"  {prefix} 开始执行 {func.__name__}")
                result = func(*args, **kwargs)
                print(f"  {prefix} 执行完毕")
                return result
            return wrapper  # type: ignore
        return decorator

    @prefix_print()           # 使用默认前缀 ">>"
    def task_a() -> None:
        print("    执行任务A")

    @prefix_print(prefix="★")  # 使用自定义前缀
    def task_b() -> None:
        print("    执行任务B")

    task_a()
    print()
    task_b()


# =============================================================================
# 14.4 实用装饰器示例
# =============================================================================

def demo_practical_decorators() -> None:
    """演示实用装饰器。"""
    print("\n" + "=" * 60)
    print("14.4 实用装饰器示例")
    print("=" * 60)

    # ── 计时装饰器 ───────────────────────────────────────
    print("1. 计时装饰器:")

    def timer(func: F) -> F:
        """计算函数执行时间的装饰器。"""
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            start = time.perf_counter()
            result = func(*args, **kwargs)
            elapsed = time.perf_counter() - start
            print(f"  [计时] {func.__name__} 执行时间: {elapsed:.4f} 秒")
            return result
        return wrapper  # type: ignore

    @timer
    def slow_function() -> str:
        """模拟耗时操作"""
        time.sleep(0.05)
        return "完成"

    result = slow_function()
    print(f"  返回值: {result}\n")

    # ── 重试装饰器 ───────────────────────────────────────
    print("2. 重试装饰器:")

    def retry(max_attempts: int = 3, delay: float = 0.1) -> Callable[[F], F]:
        """自动重试装饰器：失败时自动重试。"""
        def decorator(func: F) -> F:
            @wraps(func)
            def wrapper(*args: Any, **kwargs: Any) -> Any:
                last_error = None
                for attempt in range(1, max_attempts + 1):
                    try:
                        return func(*args, **kwargs)
                    except Exception as e:
                        last_error = e
                        print(f"  [重试] 第 {attempt} 次失败: {e}")
                        if attempt < max_attempts:
                            time.sleep(delay)
                raise RuntimeError(f"重试 {max_attempts} 次后仍然失败") from last_error
            return wrapper  # type: ignore
        return decorator

    call_count = [0]

    @retry(max_attempts=3, delay=0.0)
    def unstable_function() -> str:
        """模拟不稳定的函数（前两次失败）"""
        call_count[0] += 1
        if call_count[0] < 3:
            raise ConnectionError("连接失败")
        return "成功！"

    result = unstable_function()
    print(f"  最终结果: {result}\n")

    # ── 参数验证装饰器 ───────────────────────────────────
    print("3. 参数类型验证装饰器:")

    def validate_types(**expected_types: type) -> Callable[[F], F]:
        """验证函数参数类型的装饰器。"""
        def decorator(func: F) -> F:
            @wraps(func)
            def wrapper(*args: Any, **kwargs: Any) -> Any:
                import inspect
                sig = inspect.signature(func)
                bound = sig.bind(*args, **kwargs)
                bound.apply_defaults()
                for param_name, value in bound.arguments.items():
                    if param_name in expected_types:
                        expected = expected_types[param_name]
                        if not isinstance(value, expected):
                            raise TypeError(
                                f"参数 '{param_name}' 期望类型 {expected.__name__}，"
                                f"实际类型 {type(value).__name__}"
                            )
                return func(*args, **kwargs)
            return wrapper  # type: ignore
        return decorator

    @validate_types(name=str, age=int)
    def create_user(name: str, age: int) -> str:
        return f"用户: {name}, 年龄: {age}"

    print(f"  {create_user('张三', 25)}")
    try:
        create_user("张三", "二十五")  # 类型错误
    except TypeError as e:
        print(f"  ✓ 捕获类型错误: {e}")


# =============================================================================
# 14.5 类装饰器
# =============================================================================

def demo_class_decorators() -> None:
    """演示类装饰器。"""
    print("\n" + "=" * 60)
    print("14.5 类装饰器")
    print("=" * 60)

    # ── 用类实现装饰器 ───────────────────────────────────
    print("1. 用类实现装饰器:")

    class CallCounter:
        """统计函数调用次数的类装饰器。"""

        def __init__(self, func: Callable[..., Any]) -> None:
            wraps(func)(self)
            self._func = func
            self.call_count = 0

        def __call__(self, *args: Any, **kwargs: Any) -> Any:
            self.call_count += 1
            print(f"  [计数] {self._func.__name__} 被调用第 {self.call_count} 次")
            return self._func(*args, **kwargs)

    @CallCounter
    def add(a: int, b: int) -> int:
        return a + b

    print(f"  add(1, 2) = {add(1, 2)}")
    print(f"  add(3, 4) = {add(3, 4)}")
    print(f"  add(5, 6) = {add(5, 6)}")
    print(f"  总调用次数: {add.call_count}\n")

    # ── 用类实现带状态的装饰器 ───────────────────────────
    print("2. 带状态的计时类装饰器:")

    class TimerDecorator:
        """带统计功能的计时类装饰器。"""

        def __init__(self, func: Callable[..., Any]) -> None:
            wraps(func)(self)
            self._func = func
            self.total_time = 0.0
            self.call_count = 0

        def __call__(self, *args: Any, **kwargs: Any) -> Any:
            start = time.perf_counter()
            result = self._func(*args, **kwargs)
            elapsed = time.perf_counter() - start
            self.total_time += elapsed
            self.call_count += 1
            return result

        @property
        def avg_time(self) -> float:
            """平均执行时间"""
            if self.call_count == 0:
                return 0.0
            return self.total_time / self.call_count

    @TimerDecorator
    def compute(n: int) -> int:
        return sum(range(n))

    for _ in range(5):
        compute(10000)

    print(f"  调用次数: {compute.call_count}")
    print(f"  总耗时:   {compute.total_time:.4f} 秒")
    print(f"  平均耗时: {compute.avg_time:.4f} 秒")


# =============================================================================
# 14.6 装饰器链（叠加多个装饰器）
# =============================================================================

def demo_decorator_chain() -> None:
    """演示多个装饰器叠加使用。"""
    print("\n" + "=" * 60)
    print("14.6 装饰器链")
    print("=" * 60)

    # ── 定义几个简单装饰器 ───────────────────────────────
    def bold(func: F) -> F:
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> str:
            return f"**{func(*args, **kwargs)}**"
        return wrapper  # type: ignore

    def italic(func: F) -> F:
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> str:
            return f"_{func(*args, **kwargs)}_"
        return wrapper  # type: ignore

    def uppercase(func: F) -> F:
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> str:
            return func(*args, **kwargs).upper()
        return wrapper  # type: ignore

    # ── 叠加多个装饰器 ───────────────────────────────────
    print("1. 装饰器的叠加顺序（从下到上应用）:")

    @bold
    @italic
    @uppercase
    def get_text(text: str) -> str:
        return text

    # 等价于: bold(italic(uppercase(get_text)))
    result = get_text("hello python")
    print(f"  @bold @italic @uppercase 的结果: '{result}'")
    print(f"  执行顺序: uppercase → italic → bold\n")

    # ── 顺序对结果的影响 ─────────────────────────────────
    print("2. 顺序不同，结果不同:")

    @uppercase
    @bold
    @italic
    def get_text2(text: str) -> str:
        return text

    result2 = get_text2("hello python")
    print(f"  @uppercase @bold @italic 的结果: '{result2}'")

    # ── 实用组合示例 ─────────────────────────────────────
    print("\n3. 实用组合示例（计时 + 重试）:")

    def timer(func: F) -> F:
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            start = time.perf_counter()
            result = func(*args, **kwargs)
            elapsed = time.perf_counter() - start
            print(f"  [计时] 耗时: {elapsed:.4f} 秒")
            return result
        return wrapper  # type: ignore

    def log_call(func: F) -> F:
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            print(f"  [日志] 调用 {func.__name__}, 参数: {args}")
            result = func(*args, **kwargs)
            print(f"  [日志] 返回: {result}")
            return result
        return wrapper  # type: ignore

    @timer
    @log_call
    def multiply(a: int, b: int) -> int:
        return a * b

    multiply(6, 7)


# =============================================================================
# 14.7 functools 内置装饰器
# =============================================================================

def demo_functools_builtins() -> None:
    """演示 functools 模块中的内置装饰器。"""
    print("\n" + "=" * 60)
    print("14.7 functools 内置装饰器")
    print("=" * 60)

    # ── lru_cache ────────────────────────────────────────
    print("1. @lru_cache（最近最少使用缓存）:")

    @lru_cache(maxsize=128)
    def fib(n: int) -> int:
        """带缓存的斐波那契（极大提升性能）"""
        if n < 2:
            return n
        return fib(n - 1) + fib(n - 2)

    start = time.perf_counter()
    result = fib(35)
    elapsed = time.perf_counter() - start
    print(f"  fib(35) = {result}")
    print(f"  耗时: {elapsed:.6f} 秒")
    print(f"  缓存信息: {fib.cache_info()}\n")

    # ── cache（Python 3.9+，无大小限制）────────────────
    print("2. @cache（无大小限制的缓存，Python 3.9+）:")
    from functools import cache

    @cache
    def factorial(n: int) -> int:
        """带缓存的阶乘"""
        if n <= 1:
            return 1
        return n * factorial(n - 1)

    print(f"  10! = {factorial(10)}")
    print(f"  缓存信息: {factorial.cache_info()}\n")

    # ── cached_property ──────────────────────────────────
    print("3. @cached_property（缓存属性）:")
    from functools import cached_property

    class Circle:
        """圆形类"""

        def __init__(self, radius: float) -> None:
            self.radius = radius

        @cached_property
        def area(self) -> float:
            """计算面积（只计算一次，结果被缓存）"""
            import math
            print(f"    [计算] 面积计算中...")
            return math.pi * self.radius ** 2

    c = Circle(5.0)
    print(f"  第一次访问 area: {c.area:.4f}")
    print(f"  第二次访问 area: {c.area:.4f} (来自缓存，无需重新计算)")

    # ── singledispatch ───────────────────────────────────
    print("\n4. @singledispatch（单分派泛型函数）:")
    from functools import singledispatch

    @singledispatch
    def process(data: Any) -> str:
        """默认处理"""
        return f"未知类型: {type(data).__name__}"

    @process.register(int)
    def _(data: int) -> str:
        return f"整数: {data * 2}"

    @process.register(str)
    def _(data: str) -> str:
        return f"字符串: {data.upper()}"

    @process.register(list)
    def _(data: list) -> str:
        return f"列表: 共 {len(data)} 个元素"

    print(f"  process(42)        = {process(42)}")
    print(f"  process('hello')   = {process('hello')}")
    print(f"  process([1, 2, 3]) = {process([1, 2, 3])}")
    print(f"  process(3.14)      = {process(3.14)}")


# =============================================================================
# 14.8 综合示例：实战装饰器
# =============================================================================

def demo_real_world() -> None:
    """演示实际项目中常用的装饰器。"""
    print("\n" + "=" * 60)
    print("14.8 综合示例：实战装饰器")
    print("=" * 60)

    # ── 日志装饰器 ───────────────────────────────────────
    print("1. 日志装饰器:")

    def log(level: str = "INFO") -> Callable[[F], F]:
        """带日志级别的日志装饰器。"""
        def decorator(func: F) -> F:
            @wraps(func)
            def wrapper(*args: Any, **kwargs: Any) -> Any:
                import datetime
                timestamp = datetime.datetime.now().strftime("%H:%M:%S")
                args_repr = [repr(a) for a in args]
                kwargs_repr = [f"{k}={v!r}" for k, v in kwargs.items()]
                signature = ", ".join(args_repr + kwargs_repr)
                print(f"  [{timestamp}][{level}] 调用 {func.__name__}({signature})")
                try:
                    result = func(*args, **kwargs)
                    print(f"  [{timestamp}][{level}] {func.__name__} 返回: {result!r}")
                    return result
                except Exception as e:
                    print(f"  [{timestamp}][ERROR] {func.__name__} 异常: {e}")
                    raise
            return wrapper  # type: ignore
        return decorator

    @log(level="DEBUG")
    def divide(a: float, b: float) -> float:
        if b == 0:
            raise ZeroDivisionError("除数不能为零")
        return a / b

    divide(10.0, 3.0)
    try:
        divide(5.0, 0.0)
    except ZeroDivisionError:
        pass

    # ── 权限检查装饰器 ───────────────────────────────────
    print("\n2. 权限检查装饰器:")

    # 模拟当前用户
    current_user = {"name": "张三", "role": "user"}

    def require_role(*roles: str) -> Callable[[F], F]:
        """权限检查装饰器：只允许指定角色访问。"""
        def decorator(func: F) -> F:
            @wraps(func)
            def wrapper(*args: Any, **kwargs: Any) -> Any:
                user_role = current_user.get("role", "")
                if user_role not in roles:
                    raise PermissionError(
                        f"用户 '{current_user['name']}' (角色: {user_role}) "
                        f"无权访问 '{func.__name__}'，需要角色: {roles}"
                    )
                return func(*args, **kwargs)
            return wrapper  # type: ignore
        return decorator

    @require_role("admin", "superuser")
    def delete_database() -> str:
        return "数据库已删除"

    @require_role("user", "admin")
    def read_data() -> str:
        return "数据读取成功"

    # 普通用户可以读取
    print(f"  {read_data()}")

    # 普通用户无法删除
    try:
        delete_database()
    except PermissionError as e:
        print(f"  ✓ 权限拒绝: {e}")

    # 切换为管理员
    current_user["role"] = "admin"
    print(f"  切换为 admin 后: {delete_database()}")

    # ── 性能监控装饰器 ───────────────────────────────────
    print("\n3. 性能监控装饰器:")

    class PerformanceMonitor:
        """性能监控：统计函数的调用次数、总耗时、平均耗时。"""

        _stats: dict[str, dict[str, Any]] = {}

        @classmethod
        def monitor(cls, func: F) -> F:
            """监控装饰器"""
            name = func.__qualname__
            cls._stats[name] = {"calls": 0, "total_time": 0.0, "errors": 0}

            @wraps(func)
            def wrapper(*args: Any, **kwargs: Any) -> Any:
                start = time.perf_counter()
                try:
                    result = func(*args, **kwargs)
                    return result
                except Exception:
                    cls._stats[name]["errors"] += 1
                    raise
                finally:
                    cls._stats[name]["calls"] += 1
                    cls._stats[name]["total_time"] += time.perf_counter() - start
            return wrapper  # type: ignore

        @classmethod
        def report(cls) -> None:
            """打印性能报告"""
            print("  性能报告:")
            for name, stats in cls._stats.items():
                calls = stats["calls"]
                avg = stats["total_time"] / calls if calls > 0 else 0
                print(f"    {name}:")
                print(f"      调用次数: {calls}, 错误次数: {stats['errors']}")
                print(f"      总耗时: {stats['total_time']:.4f}s, 平均: {avg:.4f}s")

    @PerformanceMonitor.monitor
    def fast_task(n: int) -> int:
        return sum(range(n))

    for i in range(1, 6):
        fast_task(i * 1000)

    PerformanceMonitor.report()


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_basic_decorator()
    demo_functools_wraps()
    demo_parameterized_decorator()
    demo_practical_decorators()
    demo_class_decorators()
    demo_decorator_chain()
    demo_functools_builtins()
    demo_real_world()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 基本装饰器 ──
# def decorator(func):
#     @wraps(func)
#     def wrapper(*args, **kwargs):
#         # 前置操作
#         result = func(*args, **kwargs)
#         # 后置操作
#         return result
#     return wrapper
#
# ── 带参数的装饰器 ──
# def decorator_factory(param):
#     def decorator(func):
#         @wraps(func)
#         def wrapper(*args, **kwargs):
#             return func(*args, **kwargs)
#         return wrapper
#     return decorator
#
# ── 类装饰器 ──
# class Decorator:
#     def __init__(self, func):
#         wraps(func)(self)
#         self.func = func
#     def __call__(self, *args, **kwargs):
#         return self.func(*args, **kwargs)
#
# ── 装饰器叠加 ──
# @decorator_a    # 最后应用（最外层）
# @decorator_b    # 先应用（最内层）
# def func(): ...
# # 等价于: decorator_a(decorator_b(func))
#
# ── functools 内置装饰器 ──
# @lru_cache(maxsize=128)     缓存函数结果（有大小限制）
# @cache                      缓存函数结果（无大小限制，Python 3.9+）
# @cached_property            缓存属性计算结果
# @singledispatch             单分派泛型函数
# @wraps(func)                保留被装饰函数的元信息


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：忘记使用 @wraps 导致元信息丢失
# def decorator(func):
#     def wrapper(*args, **kwargs):   # ❌ 丢失 func 的 __name__, __doc__
#         return func(*args, **kwargs)
#     return wrapper
# 解决：始终在 wrapper 上使用 @wraps(func)
#
# 错误 2：带参数的装饰器漏掉一层
# def repeat(times):
#     @wraps(func)                    # ❌ func 未定义！
#     def wrapper(*args, **kwargs):
#         ...
# 解决：带参数装饰器需要三层：工厂函数 → decorator → wrapper
#
# 错误 3：装饰器返回了 None
# def decorator(func):
#     def wrapper(*args, **kwargs):
#         return func(*args, **kwargs)
#     # ❌ 忘记 return wrapper
# 解决：确保装饰器函数返回 wrapper
#
# 错误 4：类装饰器忘记实现 __call__
# class Decorator:
#     def __init__(self, func):
#         self.func = func
#     # ❌ 没有 __call__，调用时会报错
# 解决：类装饰器必须实现 __call__ 方法
#
# 错误 5：误解装饰器执行时机
# @my_decorator   # ← 装饰器在这里就执行了（模块加载时）
# def func(): ...
# func()          # ← 这里执行的是 wrapper
# 解决：理解装饰器在定义函数时就会执行，而非调用时


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   实现一个 memoize 装饰器（不带参数）：
#   缓存函数的返回值，相同参数再次调用时直接返回缓存结果，避免重复计算
#   需在 wrapper 上额外挂载 cache_clear() 和 cache_info() 两个方法
#   例如：
#     @memoize
#     def fib(n): return n if n < 2 else fib(n-1) + fib(n-2)
#     fib(35)               # 计算并缓存
#     fib(35)               # 直接返回缓存，不重新计算
#     fib.cache_info()      # {"hits": 1, "misses": 36, "size": 36}
#     fib.cache_clear()     # 清空缓存
#
# 练习 2：
#   实现一个带参数的装饰器 validate_positive(*param_names)：
#   验证指定的函数参数必须为正数（> 0），否则分别抛出 TypeError 或 ValueError
#   例如：
#     @validate_positive("width", "height")
#     def create_rect(width, height, color="red"): ...
#     create_rect(10, 5)       # 正常
#     create_rect(-1, 5)       # 抛出 ValueError: 参数 'width' 必须为正数
#     create_rect("ten", 5)    # 抛出 TypeError: 参数 'width' 必须是数值类型
#
# 练习 3：
#   实现一个 Singleton 类装饰器：
#   确保被装饰的类只能有一个实例，多次实例化始终返回同一对象
#   并提供 reset() 方法以便测试时重置单例
#   例如：
#     @Singleton
#     class Config:
#         def __init__(self, path="config.json"): self.path = path
#     a, b = Config("a.json"), Config("b.json")
#     assert a is b            # True，同一个实例
#     assert a.path == "a.json"# 首次参数生效，后续忽略
#
# 练习答案提示：
#   练习1：用 dict 保存 {(args, tuple(sorted(kwargs.items()))): result}，
#          用列表 [0] 包装计数器以在闭包中修改，再将方法绑定到 wrapper
#   练习2：用 inspect.signature(func).bind() 绑定实参，遍历 param_names
#          先检查类型是否为 int/float，再检查值是否 > 0
#   练习3：用类实现装饰器，__init__ 保存被装饰类，__call__ 检查 _instance
#          是否为 None 来决定是否创建新实例；reset() 将 _instance 置为 None


# =============================================================================
# 【练习答案】
# =============================================================================

import inspect


def memoize(func: F) -> F:
    """
    练习 1：通用记忆化（memoize）装饰器。

    缓存函数的返回值，当以相同参数再次调用时直接返回缓存结果。
    在 wrapper 上挂载 cache_clear() 和 cache_info() 两个辅助方法。

    Args:
        func: 被装饰的函数（必须是纯函数，且所有参数均可哈希）

    Returns:
        带缓存能力的包装函数，额外提供：
          wrapper.cache_clear()  → 清空缓存并重置统计
          wrapper.cache_info()   → 返回 {"hits": int, "misses": int, "size": int}

    Example:
        @memoize
        def fib(n):
            return n if n < 2 else fib(n - 1) + fib(n - 2)

        fib(35)           # 计算并缓存
        fib(35)           # 缓存命中，直接返回
        fib.cache_info()  # {"hits": 1, "misses": 36, "size": 36}
    """
    _cache: dict[tuple, Any] = {}
    _hits   = [0]   # 用列表包装，以便在嵌套函数中修改
    _misses = [0]

    @wraps(func)
    def wrapper(*args: Any, **kwargs: Any) -> Any:
        # 构造缓存键：位置参数 + 排序后的关键字参数项
        try:
            key = (args, tuple(sorted(kwargs.items())))
        except TypeError as e:
            raise TypeError(f"memoize 要求参数可哈希: {e}") from e

        if key in _cache:
            _hits[0] += 1
            return _cache[key]

        _misses[0] += 1
        result = func(*args, **kwargs)
        _cache[key] = result
        return result

    def cache_clear() -> None:
        """清空缓存及命中/未命中统计。"""
        _cache.clear()
        _hits[0]   = 0
        _misses[0] = 0

    def cache_info() -> dict[str, int]:
        """返回当前缓存统计信息。"""
        return {"hits": _hits[0], "misses": _misses[0], "size": len(_cache)}

    wrapper.cache_clear = cache_clear  # type: ignore
    wrapper.cache_info  = cache_info   # type: ignore
    return wrapper  # type: ignore


# ── 练习 2：参数正数验证装饰器 ─────────────────────────────────────────────

def validate_positive(*param_names: str) -> Callable[[F], F]:
    """
    练习 2：验证指定参数必须为正数的带参数装饰器。

    在函数调用时，对 param_names 中列出的每个参数：
      1. 检查类型是否为 int 或 float，否则抛出 TypeError
      2. 检查值是否 > 0，否则抛出 ValueError

    Args:
        *param_names: 需要验证为正数的参数名称（字符串）

    Returns:
        装饰器函数

    Example:
        @validate_positive("width", "height")
        def create_rect(width, height, color="red"):
            return width * height

        create_rect(10, 5)      # 正常返回 50
        create_rect(-1, 5)      # 抛出 ValueError
        create_rect("ten", 5)   # 抛出 TypeError
    """
    def decorator(func: F) -> F:
        sig = inspect.signature(func)   # 在装饰时提前获取签名，避免每次调用重复计算

        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            bound = sig.bind(*args, **kwargs)
            bound.apply_defaults()
            for name in param_names:
                if name in bound.arguments:
                    value = bound.arguments[name]
                    if not isinstance(value, (int, float)):
                        raise TypeError(
                            f"参数 '{name}' 必须是数值类型，"
                            f"实际类型: {type(value).__name__}"
                        )
                    if value <= 0:
                        raise ValueError(
                            f"参数 '{name}' 必须为正数（> 0），实际值: {value}"
                        )
            return func(*args, **kwargs)
        return wrapper  # type: ignore
    return decorator


# ── 练习 3：Singleton 类装饰器 ────────────────────────────────────────────

class Singleton:
    """
    练习 3：单例类装饰器。

    确保被装饰的类只能存在一个实例。无论调用多少次构造函数，
    始终返回第一次创建的同一个实例对象；首次调用时传入的参数生效，
    后续调用忽略参数。

    提供 reset() 方法以便在测试场景中重置单例状态。

    Usage:
        @Singleton
        class Config:
            def __init__(self, path="config.json"):
                self.path = path

        a = Config("app.json")
        b = Config("other.json")
        assert a is b            # True，同一个实例
        assert a.path == "app.json"

        Config.reset()           # 重置，下次调用将创建新实例
    """

    def __init__(self, cls: type) -> None:
        wraps(cls, updated=[])(self)    # 保留被装饰类的 __name__、__doc__ 等元信息
        self._cls      = cls
        self._instance: Any = None

    def __call__(self, *args: Any, **kwargs: Any) -> Any:
        """首次调用时创建实例并缓存，后续调用直接返回已有实例。"""
        if self._instance is None:
            self._instance = self._cls(*args, **kwargs)
        return self._instance

    def reset(self) -> None:
        """重置单例，下次调用将重新创建实例（主要用于测试场景）。"""
        self._instance = None

    def __repr__(self) -> str:
        return f"Singleton({self._cls.__name__})"


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 memoize 装饰器。"""
    print("memoize 演示:")

    # ── 斐波那契（经典递归，无缓存极慢）──────────────────
    @memoize
    def fib(n: int) -> int:
        if n < 2:
            return n
        return fib(n - 1) + fib(n - 2)

    start   = time.perf_counter()
    result  = fib(35)
    elapsed = time.perf_counter() - start
    print(f"  fib(35) = {result}")
    print(f"  耗时: {elapsed:.6f} 秒")
    print(f"  缓存信息: {fib.cache_info()}")

    # 再次调用相同参数，命中缓存
    fib(35)
    fib(20)
    print(f"  再次调用后: {fib.cache_info()}")

    # 清空缓存
    fib.cache_clear()
    print(f"  清空后:     {fib.cache_info()}")

    # ── 支持关键字参数 ────────────────────────────────────
    @memoize
    def power(base: int, exp: int = 2) -> int:
        print(f"    [计算] {base}^{exp}")
        return base ** exp

    print(f"\n  power(3, exp=4) = {power(3, exp=4)}")
    print(f"  power(3, exp=4) = {power(3, exp=4)} (缓存命中，无 [计算] 输出)")
    print(f"  缓存信息: {power.cache_info()}")

    # ── 不可哈希参数给出友好报错 ──────────────────────────
    @memoize
    def bad(lst: list) -> int:
        return sum(lst)

    try:
        bad([1, 2, 3])
    except TypeError as e:
        print(f"\n  ✓ 不可哈希参数捕获: {e}")


def exercise2_answer() -> None:
    """练习 2：演示 validate_positive 装饰器。"""
    print("validate_positive 演示:")

    @validate_positive("width", "height")
    def create_rect(width: float, height: float, color: str = "red") -> float:
        """创建矩形并返回面积"""
        area = width * height
        print(f"  创建 {color} 矩形 {width}×{height}，面积 = {area}")
        return area

    # ── 正常调用 ─────────────────────────────────────────
    create_rect(10.0, 5.0)
    create_rect(3.0, 4.0, color="blue")

    # ── 负数参数 ─────────────────────────────────────────
    try:
        create_rect(-1.0, 5.0)
    except ValueError as e:
        print(f"  ✓ ValueError: {e}")

    # ── 零值参数 ─────────────────────────────────────────
    try:
        create_rect(10.0, 0.0)
    except ValueError as e:
        print(f"  ✓ ValueError: {e}")

    # ── 类型错误 ─────────────────────────────────────────
    try:
        create_rect("ten", 5.0)  # type: ignore
    except TypeError as e:
        print(f"  ✓ TypeError:  {e}")

    # ── 验证单个参数（关键字传参也生效）──────────────────
    @validate_positive("n")
    def repeat_str(s: str, n: int) -> str:
        return s * n

    print(f"\n  repeat_str('ab', 3)  = {repeat_str('ab', 3)}")
    print(f"  repeat_str('ab', n=3)= {repeat_str('ab', n=3)}")
    try:
        repeat_str("ab", -2)
    except ValueError as e:
        print(f"  ✓ ValueError: {e}")


def exercise3_answer() -> None:
    """练习 3：演示 Singleton 类装饰器。"""
    print("Singleton 演示:")

    @Singleton
    class AppConfig:
        """应用程序配置（全局唯一）"""

        def __init__(self, path: str = "config.json") -> None:
            self.path     = path
            self.settings: dict[str, Any] = {}
            print(f"  [Config] 初始化，配置文件: {path}")

        def set(self, key: str, value: Any) -> None:
            self.settings[key] = value

        def get(self, key: str, default: Any = None) -> Any:
            return self.settings.get(key, default)

    # ── 第一次创建 ───────────────────────────────────────
    cfg1 = AppConfig("app.json")
    cfg1.set("debug", True)

    # ── 第二次"创建"：返回同一实例，不再打印初始化信息 ──
    cfg2 = AppConfig("other.json")

    print(f"\n  cfg1 is cfg2:          {cfg1 is cfg2}")
    print(f"  cfg2.path:             '{cfg2.path}' (仍是首次传入的路径)")
    print(f"  cfg2.get('debug'):     {cfg2.get('debug')} (共享同一份 settings)")

    # ── reset 后可重新创建 ────────────────────────────────
    AppConfig.reset()
    cfg3 = AppConfig("new.json")
    print(f"\n  重置后重新创建:")
    print(f"  cfg3 is cfg1:          {cfg3 is cfg1}")
    print(f"  cfg3.path:             '{cfg3.path}'")

    # ── 元信息保留 ────────────────────────────────────────
    print(f"\n  AppConfig.__name__:    {AppConfig.__name__}")
    print(f"  repr(AppConfig):       {AppConfig!r}")


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
