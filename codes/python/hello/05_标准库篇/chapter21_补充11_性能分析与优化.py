# =============================================================================
# 第 21 章：补充 11 - 性能分析与优化（timeit / cProfile）
# =============================================================================
#
# 【学习目标】
#   1. 掌握 timeit 模块：精确测量小段代码的执行时间
#   2. 掌握 cProfile 模块：性能剖析与瓶颈定位
#   3. 理解 Python 代码性能优化的基本思路
#   4. 能将性能分析应用到实际项目中
#
# 【与 C/C++ 的对比】
#   C/C++ 中性能分析常用 gprof、perf、Valgrind/callgrind 等外部工具。
#   Python 标准库内置了 timeit 和 cProfile，无需额外安装即可使用。
#   注意：Python 是解释型语言，优化思路与 C/C++ 不同——
#   优先优化算法复杂度，其次减少 Python 层的调用次数，最后才考虑
#   C 扩展（ctypes/Cython）。
#
# 【运行方式】
#   python chapter21_补充11_性能分析与优化.py
#
# =============================================================================

import timeit
import cProfile
import pstats
import io
import math
import functools
from pathlib import Path


# =============================================================================
# 21.补充11.1 timeit 模块：测量代码执行时间
# =============================================================================

def demo_timeit_basics() -> None:
    """演示 timeit 模块的基本用法。"""
    print("=" * 60)
    print("21.补充11.1 timeit 模块：测量代码执行时间")
    print("=" * 60)

    # ── 命令行 vs 模块 ──────────────────────────────────────
    print("命令行用法（仅供了解）：")
    print("  python -m timeit -s \"setup code\" \"test code\"")
    print("  python -m timeit -n 10000 -r 5 \"sum(range(100))\"")
    print()

    # ── timeit.timeit() ─────────────────────────────────────
    print("timeit.timeit() — 测量语句的执行时间：")

    # 测量列表推导式
    t1 = timeit.timeit("[x**2 for x in range(1000)]", number=10000)
    print(f"  列表推导式: {t1:.4f}s (10000次执行)")

    # 测量 map + lambda
    t2 = timeit.timeit("list(map(lambda x: x**2, range(1000)))", number=10000)
    print(f"  map+lambda:  {t2:.4f}s (10000次执行)")

    # ── timeit.repeat()：多次重复以获得更稳定的结果 ─────────
    print(f"\ntimeit.repeat() — 重复 5 次，取最小值：")
    results = timeit.repeat("[x**2 for x in range(1000)]", number=5000, repeat=5)
    print(f"  5 次结果: {[f'{r:.4f}s' for r in results]}")
    print(f"  最小值:   {min(results):.4f}s  (最接近真实性能)")

    # ── setup 参数：准备环境但不计入测量 ────────────────────
    print(f"\n使用 setup 参数准备测试环境：")
    t3 = timeit.timeit(
        "data.count(0)",
        setup="import random; data = [random.randint(0, 100) for _ in range(10000)]",
        number=10000,
    )
    print(f"  list.count(0) on 10000项: {t3:.4f}s (10000次)")

    # ── 对比：不同实现方式的性能差异 ────────────────────────
    print(f"\n性能对比 — 多种方式求 0..9999 的平方和：")

    # 方式 1：生成器表达式
    t_gen = timeit.timeit("sum(x**2 for x in range(10000))", number=1000)
    print(f"  生成器表达式:   {t_gen:.4f}s")

    # 方式 2：列表推导式（先构造列表）
    t_list = timeit.timeit("sum([x**2 for x in range(10000)])", number=1000)
    print(f"  列表推导式:     {t_list:.4f}s")

    # 方式 3：map + lambda
    t_map = timeit.timeit("sum(map(lambda x: x**2, range(10000)))", number=1000)
    print(f"  map + lambda:    {t_map:.4f}s")

    # 方式 4：纯数学公式 O(1)
    t_formula = timeit.timeit("9999 * 10000 * 19999 // 6", number=1000)
    print(f"  数学公式 O(1):  {t_formula:.4f}s  (快几个数量级!)")

    print(f"\n  结论：算法优化 > 语法选择 > 微观优化")


# =============================================================================
# 21.补充11.2 cProfile 模块：性能剖析
# =============================================================================

def demo_cprofile_basics() -> None:
    """演示 cProfile 模块的基本用法。"""
    print("\n" + "=" * 60)
    print("21.补充11.2 cProfile 模块：性能剖析")
    print("=" * 60)

    # ── 构建一个故意有性能问题的函数 ────────────────────────
    def slow_func(n: int) -> int:
        """故意低效的函数：重复计算 + 过多函数调用。"""
        total = 0
        for i in range(n):
            # 每次循环都重新算斐波那契（无缓存），O(2^n)
            total += _fib(i % 25)
        return total

    def _fib(n: int) -> int:
        """递归斐波那契（无优化，故意慢）。"""
        if n <= 1:
            return n
        return _fib(n - 1) + _fib(n - 2)

    # ── 使用 cProfile.run() ────────────────────────────────
    print("cProfile 剖析 slow_func(2000)：")
    print()

    profiler = cProfile.Profile()
    profiler.enable()
    result = slow_func(2000)
    profiler.disable()

    # 将统计输出到 StringIO，按累计时间排序
    stream = io.StringIO()
    stats = pstats.Stats(profiler, stream=stream)
    stats.strip_dirs().sort_stats("cumulative")
    stats.print_stats(10)  # 只打印前 10 行

    print(stream.getvalue())

    print(f"  结果: slow_func(2000) = {result}")
    print(f"  瓶颈分析：_fib() 被反复递归调用，占了绝大部分时间")
    print(f"  优化方案：加 @functools.lru_cache(maxsize=None) 缓存")


# =============================================================================
# 21.补充11.3 优化实战演示
# =============================================================================

def demo_optimization() -> None:
    """演示一个完整的"分析→优化→验证"流程。"""
    print("=" * 60)
    print("21.补充11.3 优化实战演示")
    print("=" * 60)

    # ── 场景：统计文本中每个单词的出现次数 ─────────────────

    # 准备测试数据
    words = ["hello", "world", "python", "hello", "world",
             "hello", "code", "hello", "python", "hello"] * 10000  # 10万条

    # ---- 版本 1：手动循环 ----
    def count_v1(word_list: list[str]) -> dict[str, int]:
        result: dict[str, int] = {}
        for w in word_list:
            if w in result:
                result[w] += 1
            else:
                result[w] = 1
        return result

    # ---- 版本 2：dict.get() ----
    def count_v2(word_list: list[str]) -> dict[str, int]:
        result: dict[str, int] = {}
        for w in word_list:
            result[w] = result.get(w, 0) + 1
        return result

    # ---- 版本 3：collections.Counter（C 优化） ----
    import collections
    def count_v3(word_list: list[str]) -> dict[str, int]:
        return dict(collections.Counter(word_list))

    # ---- 测量对比 ----
    print(f"\n单词计数 — 10 万条数据，3 种实现:")
    for name, fn in [("手动 if-in", count_v1), ("dict.get()", count_v2),
                      ("Counter", count_v3)]:
        t = timeit.timeit(lambda: fn(words), number=50)
        print(f"  {name:<16} {t:.4f}s")

    # ── 缓存装饰器效果展示 ──────────────────────────────────
    print(f"\n缓存装饰器的加速效果 — fib(30)：")

    def fib_naive(n: int) -> int:
        if n <= 1:
            return n
        return fib_naive(n - 1) + fib_naive(n - 2)

    @functools.lru_cache(maxsize=None)
    def fib_cached(n: int) -> int:
        if n <= 1:
            return n
        return fib_cached(n - 1) + fib_cached(n - 2)

    # 测量（注意：朴素版本只测 1 次，因为太慢了）
    t_naive = timeit.timeit(lambda: fib_naive(30), number=1)
    t_cached = timeit.timeit(lambda: fib_cached(30), number=10000)
    print(f"  朴素递归:   {t_naive:.4f}s (1次)")
    print(f"  LRU缓存:    {t_cached:.4f}s (10000次)")
    print(f"  加速比:     ~{t_naive * 10000 / t_cached:.0f}x")

    # ── 避免不必要的属性查找 ────────────────────────────────
    print(f"\n局部变量绑定 — 减少属性查找开销：")
    data = [math.sin(x) + math.cos(x) + math.tan(x) for x in range(1000)]

    def using_global_attr():
        total = 0.0
        for x in data:
            total += math.sin(x) + math.cos(x) + math.tan(x)
        return total

    def using_local_bind():
        total = 0.0
        s, c, t = math.sin, math.cos, math.tan  # 局部变量绑定
        for x in data:
            total += s(x) + c(x) + t(x)
        return total

    t_global = timeit.timeit(using_global_attr, number=500)
    t_local = timeit.timeit(using_local_bind, number=500)
    print(f"  全局属性查找: {t_global:.4f}s")
    print(f"  局部变量绑定: {t_local:.4f}s")


# =============================================================================
# 21.补充11.4 cProfile 文件剖析与 pstats 分析
# =============================================================================

def _workload_for_profile(n: int) -> list[int]:
    """模拟真实工作负载。"""
    result = []
    for i in range(n):
        val = math.factorial(i % 15)
        result.append(val)
    return result


def _heavy_string_ops_for_profile(data: list[int]) -> list[str]:
    """字符串格式化（相对慢）。"""
    return [f"value = {x:,}" for x in data]


def demo_cprofile_file() -> None:
    """演示 cProfile 的 .prof 文件剖析和 pstats 详细分析。"""
    print("\n" + "=" * 60)
    print("21.补充11.4 cProfile 文件剖析与 pstats")
    print("=" * 60)

    # ── 将被剖析的函数定义在模块级（cProfile 需要能访问） ──
    # ⚠ 不在这里定义嵌套函数——cProfile.run() 用 exec() 执行字符串，
    # 看不到闭包中的局部变量。改用 Profile() API 直接调用。

    prof_path = Path(__file__).parent / "_demo_profile.prof"

    profiler = cProfile.Profile()
    profiler.enable()
    _workload_result = _workload_for_profile(500)
    _ = _heavy_string_ops_for_profile(_workload_result)
    profiler.disable()

    # 导出到 .prof 文件
    profiler.dump_stats(str(prof_path))
    print(f"剖析文件已写入: {prof_path}")

    # ── 用 pstats 读取并分析 ───────────────────────────────
    stats = pstats.Stats(str(prof_path))
    stream = io.StringIO()

    # 按总时间排序，显示前 8 个函数
    stats.strip_dirs().sort_stats("cumulative")
    stats.print_stats(8)

    print()
    print(stream.getvalue())

    # 按调用次数排序
    print("按调用次数排序（前 5）：")
    stats.sort_stats("calls")
    stats.print_stats(5)

    # ── 按被调用者/调用者分析 ────────────────────────────────
    print(f"\n查看 math.factorial 的调用者：")
    stats.print_callers("math.factorial")

    # 清理
    prof_path.unlink(missing_ok=True)


# =============================================================================
# 21.补充11.5 性能优化原则总结
# =============================================================================

def demo_principles() -> None:
    """总结 Python 性能优化的核心原则。"""
    print("\n" + "=" * 60)
    print("21.补充11.5 性能优化原则总结")
    print("=" * 60)

    print("""
Python 性能优化黄金法则：

1. 先分析，后优化
   - 用 cProfile 找出真正的瓶颈（不要凭直觉猜）
   - 优化那 20% 的代码，它们消耗了 80% 的时间

2. 算法优先
   - O(n²) → O(n log n) 的改进远大于微观优化
   - 数学公式 (O(1)) > 查表 > 循环

3. 减少 Python 层调用
   - 用内置函数（C 实现）：sum() 比 for 循环快
   - 用标准库容器（C 优化）：collections.Counter
   - 避免循环中的属性查找：绑定到局部变量

4. 善用缓存
   - @functools.lru_cache / cached_property
   - 手动缓存昂贵计算结果

5. 惰性求值
   - 生成器表达式代替列表推导式（节省内存）
   - itertools 而不是手写循环

6. 循环中避免重复计算
   - 把不变的计算提到循环外
   - 局部变量 > 全局变量 > 属性查找

7. 注意 GIL
   - CPU 密集型 → multiprocessing
   - I/O 密集型 → threading 或 asyncio

8. 最后才考虑 C 扩展
   - ctypes（见补充 10）
   - Cython / pybind11（需额外安装）

更多信息：python -m timeit -h / python -m cProfile -h
""")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_timeit_basics()
    demo_cprofile_basics()
    demo_optimization()
    demo_cprofile_file()
    demo_principles()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. 测量小段代码
#    timeit.timeit(stmt, setup, number=N)
#    timeit.repeat(stmt, setup, repeat=R, number=N)
#    python -m timeit "code"
#
# 2. 性能剖析
#    cProfile.run(stmt)             输出到 stdout
#    cProfile.run(stmt, "file.prof") 输出到文件
#    profiler = cProfile.Profile()
#    profiler.enable() / .disable()
#
# 3. 分析剖析结果
#    stats = pstats.Stats("file.prof")
#    stats.strip_dirs().sort_stats("cumulative")
#    stats.print_stats(N)
#    stats.print_callers("func_name")
#
# 4. 常用排序方式
#    "cumulative"  - 累计时间（含子调用）
#    "time"        - 自身时间（不含子调用）
#    "calls"       - 调用次数
#    "ncalls"      - 调用次数
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个函数 compare_sort_speed(n: int)，使用 timeit 对比：
#   - sorted() 内置排序
#   - list.sort() 原地排序
#   比较它们的执行时间，并解释差异。
#
# 练习 2（进阶）：
#   使用 cProfile 剖析你之前写的某个练习脚本（如日志解析器），
#   找出 3 个最耗时的函数，分析原因并尝试优化。
#
# 练习提示：
#   练习1：sorted() 返回新列表，list.sort() 原地修改，前者多一次内存分配
#   练习2：python -m cProfile -s cumulative your_script.py


# =============================================================================
# 【练习答案】
# =============================================================================


def compare_sort_speed(n: int) -> dict[str, float]:
    """练习 1 答案：对比 sorted() 和 list.sort() 的性能。

    结论：list.sort() 略快，因为它原地排序，不需要分配新列表。
    """
    import random
    setup = f"import random; data = [random.random() for _ in range({n})]"

    t_sorted = timeit.timeit("sorted(data)", setup=setup, number=100)
    t_sort = timeit.timeit("data.sort()", setup=setup, number=100)

    print(f"数据量 {n:_} 项，各执行 100 次：")
    print(f"  sorted()  : {t_sorted:.4f}s  (新建列表)")
    print(f"  list.sort():{t_sort:.4f}s  (原地排序)")
    print(f"  差异: {t_sorted - t_sort:.4f}s (sorted 多花 {(t_sorted/t_sort - 1)*100:.0f}%)")
    return {"sorted": t_sorted, "sort": t_sort}


# 练习 2 答案提示（不提供完整代码，因为是开放题）：
# 命令：python -m cProfile -s cumulative your_script.py
# 或者用下列代码包装：
#
#   import cProfile, pstats
#   cProfile.run("your_function()", "output.prof")
#   with open("output.prof") as f:
#       stats = pstats.Stats("output.prof", stream=sys.stdout)
#   stats.strip_dirs().sort_stats("cumulative").print_stats(20)
#
# 常见瓶颈：
#   1. 重复的正则编译 — 使用 re.compile() 预编译
#   2. 循环中的字符串拼接 — 用 "".join() 代替 +=
#   3. 重复的 I/O 操作 — 批量读写，减少 open() 次数


# 取消注释以运行练习：
# if __name__ == "__main__":
#     compare_sort_speed(100_000)
