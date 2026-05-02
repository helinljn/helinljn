# =============================================================================
# 综合练习 02：函数篇
# 覆盖：ch06 函数基础、ch07 函数进阶（lambda、高阶函数、装饰器、闭包）
# =============================================================================

from functools import wraps, reduce
from typing import Callable, Any


# =============================================================================
# 练习 1：自定义管道（Pipeline）函数
# =============================================================================
# 实现一个 pipe 函数，它接受任意数量的单参数函数，
# 返回一个新函数，该函数会将输入依次通过所有函数处理。
#
# 要求：
#   - 写函数 pipe(*funcs: Callable) -> Callable
#   - 返回的函数接受一个参数 x，依次执行 funcs[0](funcs[1](...(x)))
#     即 funcs 从左到右执行
#   - 示例：
#       f = pipe(lambda x: x + 1, lambda x: x * 2, str)
#       f(5) → "12"  # 5 + 1 = 6, 6 * 2 = 12, str(12) = "12"
#   - 如果没有传入函数，返回恒等函数（返回输入本身）


# =============================================================================
# 练习 2：缓存装饰器（带过期时间）
# =============================================================================
# 实现一个 @cache_with_ttl(seconds) 装饰器，
# 缓存函数调用结果，在指定秒数后过期。
#
# 要求：
#   - 写装饰器函数 cache_with_ttl(seconds: int)
#   - 缓存 key 基于 (*args, **kwargs) 生成
#   - 过期后重新调用原函数并更新缓存
#   - 使用 functools.wraps 保留原函数元信息
#   - 示例：
#       @cache_with_ttl(seconds=5)
#       def slow_add(a, b):
#           import time; time.sleep(1)
#           return a + b


# =============================================================================
# 练习 3：函数组合（Compose）
# =============================================================================
# 实现一个 compose 函数，与 pipe 相反，从右到左执行。
#
# 要求：
#   - 写函数 compose(*funcs: Callable) -> Callable
#   - funcs 从右到左应用：compose(f, g, h)(x) = f(g(h(x)))
#   - 示例：compose(str, lambda x: x * 2, lambda x: x + 1)(5)
#     → h(5)=6, g(6)=12, f(12)="12"
#   - 使用 reduce 实现


# =============================================================================
# 练习 4：参数验证装饰器
# =============================================================================
# 实现一个 @validate 装饰器，在调用函数前验证参数类型。
#
# 要求：
#   - validate(**type_hints) — 接收参数名到类型的映射
#   - 在调用函数前检查指定参数的类型
#   - 类型不匹配时抛出 TypeError
#   - 使用闭包保存类型约束


# =============================================================================
# 练习 5：重试装饰器
# =============================================================================
# 实现一个 @retry(max_attempts, delay=0, exceptions=(Exception,)) 装饰器，
# 在函数抛出指定异常时自动重试。
#
# 要求：
#   - 默认重试 3 次，间隔 0 秒
#   - 只捕获指定的异常类型
#   - 超过最大重试次数后抛出原始异常
#   - 每次重试时打印提示信息
#   - 使用闭包保存重试状态


# =============================================================================
# ============================= 答案分隔线 ====================================
# =============================================================================


# =============================================================================
# 练习 1 答案：Pipeline
# =============================================================================

def pipe(*funcs: Callable) -> Callable:
    """从左到右组合多个函数。"""
    if not funcs:
        return lambda x: x

    def piped(x: Any) -> Any:
        result = x
        for f in funcs:
            result = f(result)
        return result

    return piped


# =============================================================================
# 练习 2 答案：带 TTL 的缓存装饰器
# =============================================================================

import time


def cache_with_ttl(seconds: int) -> Callable:
    """带过期时间的函数调用缓存装饰器。"""

    def decorator(func: Callable) -> Callable:
        cache: dict = {}  # 闭包变量存储缓存

        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            # 生成缓存 key（kwargs 需要可哈希化处理）
            key = (args, tuple(sorted(kwargs.items())))
            now = time.time()

            if key in cache:
                result, timestamp = cache[key]
                if now - timestamp < seconds:
                    return result

            # 缓存不存在或已过期，重新计算
            result = func(*args, **kwargs)
            cache[key] = (result, now)
            return result

        wrapper.cache = cache  # 暴露缓存供调试
        return wrapper

    return decorator


# =============================================================================
# 练习 3 答案：Compose
# =============================================================================

def compose(*funcs: Callable) -> Callable:
    """从右到左组合多个函数。使用 reduce 实现。"""
    if not funcs:
        return lambda x: x

    def composed(x: Any) -> Any:
        return reduce(lambda val, f: f(val), reversed(funcs), x)

    return composed


# =============================================================================
# 练习 4 答案：参数验证装饰器
# =============================================================================

def validate(**type_hints: type) -> Callable:
    """参数类型验证装饰器。"""

    def decorator(func: Callable) -> Callable:
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            # 获取函数参数名
            import inspect
            sig = inspect.signature(func)
            bound = sig.bind(*args, **kwargs)
            bound.apply_defaults()

            for param_name, expected_type in type_hints.items():
                if param_name in bound.arguments:
                    value = bound.arguments[param_name]
                    if not isinstance(value, expected_type):
                        raise TypeError(
                            f"参数 '{param_name}' 期望类型 {expected_type.__name__}，"
                            f"但收到了 {type(value).__name__}"
                        )

            return func(*args, **kwargs)

        return wrapper

    return decorator


# =============================================================================
# 练习 5 答案：重试装饰器
# =============================================================================

def retry(max_attempts: int = 3, delay: float = 0,
          exceptions: tuple = (Exception,)) -> Callable:
    """函数调用失败时自动重试的装饰器。"""

    def decorator(func: Callable) -> Callable:
        @wraps(func)
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            last_exception = None
            for attempt in range(1, max_attempts + 1):
                try:
                    return func(*args, **kwargs)
                except exceptions as e:
                    last_exception = e
                    if attempt < max_attempts:
                        print(f"  [重试] {func.__name__} 第 {attempt} 次失败: {e}，"
                              f"等待 {delay}s 后重试...")
                        if delay > 0:
                            time.sleep(delay)
            # 所有重试都失败
            raise last_exception  # type: ignore

        return wrapper

    return decorator


# =============================================================================
# 运行所有练习
# =============================================================================

if __name__ == "__main__":
    print("=" * 60)
    print("综合练习 02：函数篇")
    print("=" * 60)

    # 练习 1
    print("\n【练习 1】Pipeline")
    f = pipe(lambda x: x + 1, lambda x: x * 2, str)
    print(f"  pipe(+1, *2, str)(5) = {f(5)!r}")
    g = pipe()
    print(f"  空 pipe(10) = {g(10)}")

    # 练习 2
    print("\n【练习 2】带 TTL 的缓存装饰器")
    call_count = [0]  # 用列表包装以在闭包中修改

    @cache_with_ttl(seconds=2)
    def slow_add(a: int, b: int) -> int:
        call_count[0] += 1
        return a + b

    print(f"  第1次调用: {slow_add(1, 2)} (调用次数: {call_count[0]})")
    print(f"  第2次调用: {slow_add(1, 2)} (调用次数: {call_count[0]})")  # 缓存命中
    print(f"  不同参数: {slow_add(3, 4)} (调用次数: {call_count[0]})")    # 不同参数，重新调用

    # 练习 3
    print("\n【练习 3】Compose")
    h = compose(str, lambda x: x * 2, lambda x: x + 1)
    print(f"  compose(str, *2, +1)(5) = {h(5)!r}")
    identity = compose()
    print(f"  空 compose(42) = {identity(42)}")

    # 练习 4
    print("\n【练习 4】参数验证装饰器")

    @validate(name=str, age=int)
    def register(name: str, age: int) -> str:
        return f"注册成功: {name}, {age}岁"

    print(f"  合法调用: {register('Alice', 28)}")
    try:
        register("Bob", "三十岁")
    except TypeError as e:
        print(f"  非法调用: {e}")

    # 练习 5
    print("\n【练习 5】重试装饰器")
    fail_counts: dict[str, int] = {}  # 记录每个 key 的失败次数

    @retry(max_attempts=4, delay=0.1)
    def unreliable_api(key: str) -> str:
        """模拟不稳定的 API 调用（前3次失败，第4次成功）。"""
        fail_counts[key] = fail_counts.get(key, 0) + 1
        if fail_counts[key] < 4:
            raise ConnectionError(f"API 调用失败 (第{fail_counts[key]}次)")
        return f"API 返回: {key}=42"

    try:
        result = unreliable_api("answer")
        print(f"  最终结果: {result}")
    except ConnectionError as e:
        print(f"  最终失败: {e}")

    # 验证重试装饰器在超过最大次数后抛出异常
    fail_counts.clear()

    @retry(max_attempts=2, delay=0.05)
    def always_fail() -> None:
        raise ValueError("永远失败")

    try:
        always_fail()
    except ValueError as e:
        print(f"  超过最大重试: {e}")
