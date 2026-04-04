# =============================================================================
# 第 13 章：迭代器与生成器
# =============================================================================
#
# 【学习目标】
#   1. 理解迭代器协议（__iter__ 和 __next__）
#   2. 掌握生成器函数（yield）
#   3. 理解生成器表达式
#   4. 掌握 itertools 模块
#   5. 了解协程基础（yield from）
#   6. 理解惰性求值的优势
#
# 【与 C++ 的对比】
#   C++:  迭代器是指针的抽象，需要手动管理
#   Python: 迭代器是协议，任何对象都可以实现
#   Python 的生成器提供了更简洁的惰性求值方式
#
# 【运行方式】
#   python chapter13_迭代器与生成器.py
#
# =============================================================================

import itertools
from typing import Iterator, Iterable, Any
from collections.abc import Generator


# =============================================================================
# 13.1 可迭代对象 vs 迭代器
# =============================================================================

def demo_iterable_vs_iterator() -> None:
    """演示可迭代对象和迭代器的区别。"""
    print("=" * 60)
    print("13.1 可迭代对象 vs 迭代器")
    print("=" * 60)

    # ── 可迭代对象（Iterable）────────────────────────────
    print("1. 可迭代对象:")
    lst = [1, 2, 3, 4, 5]
    print(f"  列表: {lst}")
    print(f"  hasattr(lst, '__iter__'): {hasattr(lst, '__iter__')}")
    print(f"  hasattr(lst, '__next__'): {hasattr(lst, '__next__')}")

    # ── 获取迭代器 ───────────────────────────────────────
    print("\n2. 从可迭代对象获取迭代器:")
    it = iter(lst)  # 调用 lst.__iter__()
    print(f"  iter(lst) = {it}")
    print(f"  hasattr(it, '__iter__'): {hasattr(it, '__iter__')}")
    print(f"  hasattr(it, '__next__'): {hasattr(it, '__next__')}")

    # ── 手动迭代 ─────────────────────────────────────────
    print("\n3. 手动迭代:")
    print(f"  next(it) = {next(it)}")  # 1
    print(f"  next(it) = {next(it)}")  # 2
    print(f"  next(it) = {next(it)}")  # 3

    # ── 迭代器耗尽 ───────────────────────────────────────
    print("\n4. 迭代器耗尽:")
    print(f"  next(it) = {next(it)}")  # 4
    print(f"  next(it) = {next(it)}")  # 5
    try:
        next(it)  # 抛出 StopIteration
    except StopIteration:
        print(f"  ✓ StopIteration: 迭代器已耗尽")

    # ── for 循环的本质 ────────────────────────────────────
    print("\n5. for 循环的本质:")
    print("  for 循环会自动:")
    print("    1. 调用 iter() 获取迭代器")
    print("    2. 反复调用 next() 获取元素")
    print("    3. 捕获 StopIteration 结束循环")

    print("\n  等价代码:")
    lst2 = [10, 20, 30]
    it2 = iter(lst2)
    while True:
        try:
            item = next(it2)
            print(f"    {item}")
        except StopIteration:
            break


# =============================================================================
# 13.2 自定义迭代器
# =============================================================================

class Countdown:
    """倒计时迭代器。"""

    def __init__(self, start: int) -> None:
        self.current = start

    def __iter__(self) -> Iterator[int]:
        """返回迭代器对象（自己）。"""
        return self

    def __next__(self) -> int:
        """返回下一个值。"""
        if self.current <= 0:
            raise StopIteration
        self.current -= 1
        return self.current + 1


class Range:
    """自定义 range 类（可迭代对象）。"""

    def __init__(self, start: int, stop: int, step: int = 1) -> None:
        self.start = start
        self.stop = stop
        self.step = step

    def __iter__(self) -> Iterator[int]:
        """返回一个新的迭代器。"""
        return RangeIterator(self.start, self.stop, self.step)


class RangeIterator:
    """Range 的迭代器。"""

    def __init__(self, start: int, stop: int, step: int) -> None:
        self.current = start
        self.stop = stop
        self.step = step

    def __iter__(self) -> Iterator[int]:
        return self

    def __next__(self) -> int:
        if (self.step > 0 and self.current >= self.stop) or \
           (self.step < 0 and self.current <= self.stop):
            raise StopIteration
        value = self.current
        self.current += self.step
        return value


def demo_custom_iterator() -> None:
    """演示自定义迭代器。"""
    print("\n" + "=" * 60)
    print("13.2 自定义迭代器")
    print("=" * 60)

    # ── Countdown 迭代器 ──────────────────────────────────
    print("1. Countdown 迭代器:")
    countdown = Countdown(5)
    for num in countdown:
        print(f"  {num}...", end=" ")
    print("发射！")

    # ── Range 可迭代对象 ──────────────────────────────────
    print("\n2. 自定义 Range:")
    r = Range(0, 10, 2)
    print(f"  Range(0, 10, 2): {list(r)}")

    # 可以多次迭代（因为每次都创建新迭代器）
    print(f"  再次迭代: {list(r)}")

    # ── 对比：迭代器只能迭代一次 ──────────────────────────
    print("\n3. 迭代器 vs 可迭代对象:")
    countdown2 = Countdown(3)
    print(f"  第一次: {list(countdown2)}")
    print(f"  第二次: {list(countdown2)} (已耗尽)")


# =============================================================================
# 13.3 生成器函数（yield）
# =============================================================================

def simple_generator() -> Generator[int, None, None]:
    """最简单的生成器。"""
    print("  [生成器] 开始")
    yield 1
    print("  [生成器] 继续")
    yield 2
    print("  [生成器] 再继续")
    yield 3
    print("  [生成器] 结束")


def countdown_generator(n: int) -> Generator[int, None, None]:
    """倒计时生成器（比迭代器类简洁得多）。"""
    while n > 0:
        yield n
        n -= 1


def fibonacci(n: int) -> Generator[int, None, None]:
    """生成前 n 个斐波那契数。"""
    a, b = 0, 1
    count = 0
    while count < n:
        yield a
        a, b = b, a + b
        count += 1


def read_large_file(filename: str) -> Generator[str, None, None]:
    """
    逐行读取大文件（惰性求值，内存高效）。
    不会一次性加载整个文件到内存。
    """
    with open(filename, "r", encoding="utf-8") as f:
        for line in f:
            yield line.rstrip()


def demo_generators() -> None:
    """演示生成器函数。"""
    print("\n" + "=" * 60)
    print("13.3 生成器函数")
    print("=" * 60)

    # ── 简单生成器 ───────────────────────────────────────
    print("1. 简单生成器:")
    gen = simple_generator()
    print(f"  type(gen) = {type(gen)}")
    print(f"  调用 next():")
    print(f"    {next(gen)}")
    print(f"    {next(gen)}")
    print(f"    {next(gen)}")

    # ── 倒计时生成器 ─────────────────────────────────────
    print("\n2. 倒计时生成器:")
    for num in countdown_generator(5):
        print(f"  {num}...", end=" ")
    print("发射！")

    # ── 斐波那契生成器 ───────────────────────────────────
    print("\n\n3. 斐波那契生成器:")
    fib = fibonacci(10)
    print(f"  前10个斐波那契数: {list(fib)}")

    # ── 生成器的惰性求值 ─────────────────────────────────
    print("\n4. 惰性求值（内存高效）:")

    def infinite_sequence() -> Generator[int, None, None]:
        """无限序列生成器。"""
        num = 0
        while True:
            yield num
            num += 1

    gen = infinite_sequence()
    print(f"  前10个: {[next(gen) for _ in range(10)]}")
    print(f"  再取5个: {[next(gen) for _ in range(5)]}")


# =============================================================================
# 13.4 生成器表达式
# =============================================================================

def demo_generator_expressions() -> None:
    """演示生成器表达式。"""
    print("\n" + "=" * 60)
    print("13.4 生成器表达式")
    print("=" * 60)

    # ── 列表推导式 vs 生成器表达式 ────────────────────────
    print("1. 列表推导式 vs 生成器表达式:")

    # 列表推导式：立即计算，占用内存
    list_comp = [x ** 2 for x in range(10)]
    print(f"  列表推导式: {list_comp}")
    print(f"  type: {type(list_comp)}, size: {len(list_comp)}")

    # 生成器表达式：惰性求值，节省内存
    gen_exp = (x ** 2 for x in range(10))
    print(f"  生成器表达式: {gen_exp}")
    print(f"  type: {type(gen_exp)}")
    print(f"  转为列表: {list(gen_exp)}")

    # ── 内存对比 ─────────────────────────────────────────
    print("\n2. 内存效率对比:")
    import sys

    # 大列表
    big_list = [x for x in range(1000000)]
    print(f"  列表占用: {sys.getsizeof(big_list):,} 字节")

    # 生成器
    big_gen = (x for x in range(1000000))
    print(f"  生成器占用: {sys.getsizeof(big_gen):,} 字节")

    # ── 链式操作 ─────────────────────────────────────────
    print("\n3. 链式操作:")
    result = sum(x ** 2 for x in range(100) if x % 2 == 0)
    print(f"  sum(x²) for 偶数 0-99: {result}")


# =============================================================================
# 13.5 yield from（委托生成器）
# =============================================================================

def chain(*iterables: Iterable[Any]) -> Generator[Any, None, None]:
    """连接多个可迭代对象。"""
    for it in iterables:
        for item in it:
            yield item


def chain_v2(*iterables: Iterable[Any]) -> Generator[Any, None, None]:
    """使用 yield from 的版本（更简洁）。"""
    for it in iterables:
        yield from it


def flatten(nested: list) -> Generator[Any, None, None]:
    """展平嵌套列表。"""
    for item in nested:
        if isinstance(item, list):
            yield from flatten(item)  # 递归
        else:
            yield item


def demo_yield_from() -> None:
    """演示 yield from。"""
    print("\n" + "=" * 60)
    print("13.5 yield from")
    print("=" * 60)

    # ── 基本用法 ─────────────────────────────────────────
    print("1. 连接多个可迭代对象:")
    result1 = list(chain([1, 2], [3, 4], [5, 6]))
    result2 = list(chain_v2([1, 2], [3, 4], [5, 6]))
    print(f"  chain:    {result1}")
    print(f"  chain_v2: {result2}")

    # ── 递归展平 ─────────────────────────────────────────
    print("\n2. 展平嵌套列表:")
    nested = [1, [2, 3, [4, 5]], 6, [7, [8, 9]]]
    flat = list(flatten(nested))
    print(f"  原始: {nested}")
    print(f"  展平: {flat}")


# =============================================================================
# 13.6 itertools 模块
# =============================================================================

def demo_itertools() -> None:
    """演示 itertools 模块的常用函数。"""
    print("\n" + "=" * 60)
    print("13.6 itertools 模块")
    print("=" * 60)

    # ── 无限迭代器 ───────────────────────────────────────
    print("1. 无限迭代器:")

    # count(start, step)
    counter = itertools.count(10, 2)
    print(f"  count(10, 2): {[next(counter) for _ in range(5)]}")

    # cycle(iterable)
    colors = itertools.cycle(['红', '绿', '蓝'])
    print(f"  cycle(['红','绿','蓝']): {[next(colors) for _ in range(7)]}")

    # repeat(obj, times)
    rep = itertools.repeat('A', 5)
    print(f"  repeat('A', 5): {list(rep)}")

    # ── 组合迭代器 ───────────────────────────────────────
    print("\n2. 组合迭代器:")

    # chain - 连接
    c = itertools.chain([1, 2], [3, 4], [5])
    print(f"  chain([1,2], [3,4], [5]): {list(c)}")

    # zip_longest - 补齐短序列
    z = itertools.zip_longest([1, 2], ['a', 'b', 'c'], fillvalue=0)
    print(f"  zip_longest: {list(z)}")

    # ── 筛选迭代器 ───────────────────────────────────────
    print("\n3. 筛选迭代器:")

    # compress - 根据选择器筛选
    data = ['A', 'B', 'C', 'D', 'E']
    selectors = [1, 0, 1, 0, 1]
    result = itertools.compress(data, selectors)
    print(f"  compress({data}, {selectors}): {list(result)}")

    # dropwhile - 丢弃满足条件的元素
    nums = [1, 3, 5, 6, 7, 8]
    result = itertools.dropwhile(lambda x: x < 5, nums)
    print(f"  dropwhile(x<5, {nums}): {list(result)}")

    # takewhile - 保留满足条件的元素
    result = itertools.takewhile(lambda x: x < 5, nums)
    print(f"  takewhile(x<5, {nums}): {list(result)}")

    # ── 排列组合 ─────────────────────────────────────────
    print("\n4. 排列组合:")

    # product - 笛卡尔积
    prod = itertools.product([1, 2], ['a', 'b'])
    print(f"  product([1,2], ['a','b']): {list(prod)}")

    # permutations - 排列
    perm = itertools.permutations([1, 2, 3], 2)
    print(f"  permutations([1,2,3], 2): {list(perm)}")

    # combinations - 组合
    comb = itertools.combinations([1, 2, 3, 4], 2)
    print(f"  combinations([1,2,3,4], 2): {list(comb)}")

    # ── 分组 ─────────────────────────────────────────────
    print("\n5. 分组:")
    data = [('A', 1), ('A', 2), ('B', 3), ('B', 4), ('C', 5)]
    for key, group in itertools.groupby(data, key=lambda x: x[0]):
        print(f"  {key}: {list(group)}")


# =============================================================================
# 13.7 综合示例：数据流处理管道
# =============================================================================

def demo_pipeline() -> None:
    """演示使用生成器构建数据处理管道。"""
    print("\n" + "=" * 60)
    print("13.7 综合示例：数据流处理管道")
    print("=" * 60)

    # 模拟日志数据
    log_lines = [
        "2024-01-15 ERROR Database connection failed",
        "2024-01-15 INFO User logged in",
        "2024-01-15 WARNING Memory usage high",
        "2024-01-15 ERROR File not found",
        "2024-01-15 INFO Request processed",
        "2024-01-15 ERROR Timeout occurred",
    ]

    def read_logs(lines: list[str]) -> Generator[str, None, None]:
        """读取日志（模拟）。"""
        for line in lines:
            yield line

    def filter_errors(lines: Iterable[str]) -> Generator[str, None, None]:
        """只保留 ERROR 日志。"""
        for line in lines:
            if "ERROR" in line:
                yield line

    def extract_message(lines: Iterable[str]) -> Generator[str, None, None]:
        """提取错误消息。"""
        for line in lines:
            parts = line.split(" ", 3)
            if len(parts) >= 4:
                yield parts[3]

    # 构建管道
    print("构建数据处理管道:")
    print("  read_logs → filter_errors → extract_message\n")

    pipeline = extract_message(filter_errors(read_logs(log_lines)))

    print("错误消息:")
    for i, msg in enumerate(pipeline, 1):
        print(f"  {i}. {msg}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_iterable_vs_iterator()
    demo_custom_iterator()
    demo_generators()
    demo_generator_expressions()
    demo_yield_from()
    demo_itertools()
    demo_pipeline()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 迭代器协议 ──
# __iter__()  返回迭代器对象
# __next__()  返回下一个值，无值时抛出 StopIteration
#
# ── 生成器函数 ──
# def gen():
#     yield value  # 暂停并返回值
#
# ── 生成器表达式 ──
# (expr for item in iterable if condition)
#
# ── yield from ──
# yield from iterable  # 委托给子生成器
#
# ── itertools 常用函数 ──
# count(start, step)           无限计数
# cycle(iterable)              循环迭代
# repeat(obj, times)           重复元素
# chain(*iterables)            连接
# compress(data, selectors)    筛选
# dropwhile(pred, iterable)    丢弃
# takewhile(pred, iterable)    保留
# product(*iterables)          笛卡尔积
# permutations(iterable, r)    排列
# combinations(iterable, r)    组合
# groupby(iterable, key)       分组


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：迭代器只能使用一次
# gen = (x for x in range(5))
# list(gen)  # [0, 1, 2, 3, 4]
# list(gen)  # [] 已耗尽
# 解决：需要多次迭代时，使用列表或重新创建生成器
#
# 错误 2：在生成器中使用 return
# def gen():
#     yield 1
#     return 2  # ❌ 会抛出 StopIteration(2)
# 解决：生成器中 return 用于结束，不返回值
#
# 错误 3：混淆 yield 和 return
# def gen():
#     return [1, 2, 3]  # ❌ 返回列表，不是生成器
# 解决：使用 yield 逐个产生值


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   写一个生成器函数 sliding_window(iterable, size)：
#   对任意可迭代对象产生大小为 size 的滑动窗口（元组）
#   例如 sliding_window([1,2,3,4,5], 3) 依次产生 (1,2,3), (2,3,4), (3,4,5)
#   当剩余元素不足 size 时停止
#
# 练习 2：
#   实现一个自定义迭代器类 CycleN(iterable, times)：
#   对可迭代对象循环 times 次（itertools.cycle 是无限循环，这个有限）
#   例如 list(CycleN([1, 2, 3], 2)) == [1, 2, 3, 1, 2, 3]
#
# 练习 3：
#   写一个生成器函数 group_consecutive(numbers)：
#   将一组已排序整数中连续的部分分组，每组以列表形式产生
#   例如 list(group_consecutive([1,2,3,7,8,10])) == [[1,2,3], [7,8], [10]]
#
# 练习答案提示：
#   练习1：用 collections.deque(maxlen=size) 维护滑动窗口
#   练习2：内部保存 list(iterable)，用 cycle_count 计数控制循环
#   练习3：用 enumerate 为元素加差值标记，再用 itertools.groupby 分组


# =============================================================================
# 【练习答案】
# =============================================================================

from collections import deque
from typing import TypeVar

T = TypeVar("T")


def sliding_window(iterable: Iterable[T], size: int) -> Generator[tuple[T, ...], None, None]:
    """
    练习 1：滑动窗口生成器。

    对任意可迭代对象产生大小为 size 的滑动窗口（元组）。
    当剩余元素不足 size 时停止产生。

    Args:
        iterable: 任意可迭代对象
        size:     窗口大小（必须为正整数）

    Yields:
        长度为 size 的元组，依次向右滑动一格

    Example:
        list(sliding_window([1, 2, 3, 4, 5], 3))
        → [(1, 2, 3), (2, 3, 4), (3, 4, 5)]
    """
    if size <= 0:
        raise ValueError(f"窗口大小必须为正整数，得到 {size}")

    window: deque[T] = deque(maxlen=size)

    for item in iterable:
        window.append(item)
        if len(window) == size:
            yield tuple(window)


# ── 练习 2：有限循环迭代器 ───────────────────────────────────────────────────

class CycleN:
    """
    练习 2：有限循环迭代器。

    对可迭代对象循环 times 次，与 itertools.cycle（无限循环）不同，
    本类在完成指定循环次数后自动停止。

    Args:
        iterable: 任意可迭代对象
        times:    循环次数（0 表示不产生任何元素）

    Example:
        list(CycleN([1, 2, 3], 2)) == [1, 2, 3, 1, 2, 3]
        list(CycleN([], 5))        == []
        list(CycleN([1, 2], 0))    == []
    """

    def __init__(self, iterable: Iterable[Any], times: int) -> None:
        self._data: list[Any] = list(iterable)
        self._times = max(0, times)
        self._cycle_count = 0   # 已完成的循环轮次
        self._index = 0         # 当前元素索引

    def __iter__(self) -> Iterator[Any]:
        """返回迭代器自身（重置状态以支持重新迭代）。"""
        self._cycle_count = 0
        self._index = 0
        return self

    def __next__(self) -> Any:
        """返回下一个元素。"""
        # 空序列或已完成所有循环
        if not self._data or self._cycle_count >= self._times:
            raise StopIteration

        value = self._data[self._index]
        self._index += 1

        # 一轮结束，进入下一轮
        if self._index >= len(self._data):
            self._index = 0
            self._cycle_count += 1

        return value


# ── 练习 3：连续整数分组 ──────────────────────────────────────────────────────

def group_consecutive(numbers: Iterable[int]) -> Generator[list[int], None, None]:
    """
    练习 3：将已排序整数序列中连续的部分分组。

    通过 enumerate 为每个元素计算 (value - index) 差值作为分组键：
    连续整数的差值相同，不连续时差值会改变，从而用 groupby 自动分组。

    Args:
        numbers: 已排序的整数可迭代对象

    Yields:
        将连续整数合并后的子列表，按原顺序产生

    Example:
        list(group_consecutive([1, 2, 3, 7, 8, 10]))
        → [[1, 2, 3], [7, 8], [10]]
    """
    for _, group in itertools.groupby(enumerate(numbers), key=lambda x: x[1] - x[0]):
        yield [v for _, v in group]


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 sliding_window 生成器。"""
    print("sliding_window 演示:")

    # 基本用法
    data = [1, 2, 3, 4, 5, 6]
    size = 3
    windows = list(sliding_window(data, size))
    print(f"  sliding_window({data}, {size}):")
    for w in windows:
        print(f"    {w}")

    # 字符串也可以
    words = ["apple", "banana", "cherry", "date", "elderberry"]
    print(f"\n  sliding_window({words}, 2):")
    for w in sliding_window(words, 2):
        print(f"    {w}")

    # 计算移动平均（实际应用）
    prices = [10, 12, 11, 13, 15, 14, 16, 18]
    window_size = 3
    print(f"\n  {prices} 的 {window_size} 日移动平均:")
    for i, w in enumerate(sliding_window(prices, window_size), start=window_size - 1):
        avg = sum(w) / len(w)
        print(f"    第{i+1}日: {w} → 平均 {avg:.2f}")

    # 边界情况
    print(f"\n  窗口大于序列时，无输出:")
    result = list(sliding_window([1, 2], 5))
    print(f"    sliding_window([1, 2], 5) = {result}")


def exercise2_answer() -> None:
    """练习 2：演示 CycleN 迭代器。"""
    print("CycleN 演示:")

    # 基本用法
    c = CycleN([1, 2, 3], 2)
    print(f"  CycleN([1, 2, 3], 2): {list(c)}")

    # 循环一次
    c = CycleN(["a", "b"], 1)
    print(f"  CycleN(['a','b'], 1): {list(c)}")

    # 循环 0 次
    c = CycleN([1, 2, 3], 0)
    print(f"  CycleN([1, 2, 3], 0): {list(c)}")

    # 空序列
    c = CycleN([], 5)
    print(f"  CycleN([], 5):        {list(c)}")

    # 实际用途：轮询分配任务
    print(f"\n  轮询分配任务演示:")
    workers = ["Worker-A", "Worker-B", "Worker-C"]
    tasks   = ["Task-1", "Task-2", "Task-3", "Task-4", "Task-5", "Task-6"]

    for worker, task in zip(CycleN(workers, 2), tasks):
        print(f"    {worker} 处理 {task}")


def exercise3_answer() -> None:
    """练习 3：演示 group_consecutive 生成器。"""
    print("group_consecutive 演示:")

    # 基本用法
    nums = [1, 2, 3, 7, 8, 10]
    groups = list(group_consecutive(nums))
    print(f"  输入: {nums}")
    print(f"  分组: {groups}")

    # 全部连续
    nums2 = [5, 6, 7, 8, 9]
    print(f"\n  输入: {nums2}")
    print(f"  分组: {list(group_consecutive(nums2))}")

    # 全部不连续
    nums3 = [1, 3, 5, 7, 9]
    print(f"\n  输入: {nums3}")
    print(f"  分组: {list(group_consecutive(nums3))}")

    # 实际应用：统计连续缺勤天数
    print("\n  实际应用：统计连续缺勤天数（月内日期）:")
    absent_days = [3, 4, 5, 12, 18, 19, 20, 21, 28]
    for group in group_consecutive(absent_days):
        if len(group) == 1:
            print(f"    第{group[0]}天：单日缺勤")
        else:
            print(f"    第{group[0]}~{group[-1]}天：连续缺勤 {len(group)} 天")


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
