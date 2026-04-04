# =============================================================================
# 第 19 章：函数式编程工具（operator、heapq、bisect 及 functools 进阶）
# =============================================================================
#
# 【学习目标】
#   1. 掌握 operator 模块：将运算符变成可传递的函数对象
#   2. 掌握 heapq 模块：高效维护有序堆（优先队列）
#   3. 掌握 bisect 模块：在有序序列中进行 O(log n) 查找与插入
#   4. 掌握 functools 进阶：singledispatch、cache、cached_property、cmp_to_key
#   5. 学会将这些工具组合使用，写出高效、优雅的 Python 代码
#
# 【这些模块解决什么问题？】
#   operator:  让 +、-、>、[] 等操作符变成函数，可以作为参数传递
#              （避免写大量只有一行的 lambda）
#   heapq:     用普通 list 维护一个"堆"结构，O(log n) 取出最小元素
#              （适合：任务调度、Top-K 问题、合并有序流）
#   bisect:    对已排序的列表进行高效查找和插入，避免全表扫描
#              （适合：成绩分级、时间区间查找、有序插入）
#   functools: 函数变换工具集（泛型函数、属性缓存、比较函数适配）
#
# 【与 C/C++ 的对比】
#   operator:  类似 C++ 的 std::less<T>、std::plus<T> 等函数对象（仿函数）
#   heapq:     类似 C++ 的 std::priority_queue（但 Python 默认是最小堆）
#   bisect:    类似 C++ 的 std::lower_bound / std::upper_bound
#
# 【注意】
#   第 18 章已介绍 functools 的 reduce、partial、lru_cache、wraps、total_ordering
#   本章聚焦 functools 中尚未介绍的进阶工具
#
# 【运行方式】
#   python chapter19_函数式编程工具.py
#
# =============================================================================

import operator
import heapq
import bisect
import functools
import time
import dataclasses
import random
import itertools
from typing import Any


# =============================================================================
# 19.1 operator 模块：将运算符变成函数
# =============================================================================

def demo_operator_basics() -> None:
    """演示 operator 模块的基础用法：算术、比较运算符函数化。"""
    print("=" * 60)
    print("19.1 operator 模块：将运算符变成函数")
    print("=" * 60)

    # ── 为什么需要 operator ──────────────────────────────
    print("1. 为什么需要 operator 模块？")
    print()
    print("  在 Python 中，sorted()、map()、functools.reduce() 等高阶函数")
    print("  需要传入一个'可调用对象'（callable）。")
    print("  常见做法是写 lambda，但 operator 提供了更清晰的替代方案。")
    print()

    numbers = [3, 1, 4, 1, 5, 9, 2, 6]
    result_lambda = sorted(numbers, key=lambda x: -x)   # 传统 lambda 写法
    result_op     = sorted(numbers, key=operator.neg)   # operator 写法（更语义化）

    print(f"  sorted(key=lambda x: -x)  → {result_lambda}")
    print(f"  sorted(key=operator.neg)  → {result_op}")
    print(f"  两者结果相同: {result_lambda == result_op}")
    print()

    # ── 算术运算符函数 ────────────────────────────────────
    print("2. 算术运算符函数（a=10, b=3）:")
    print()

    a, b = 10, 3
    ops = [
        ('add',      'a + b',   operator.add(a, b)),
        ('sub',      'a - b',   operator.sub(a, b)),
        ('mul',      'a * b',   operator.mul(a, b)),
        ('truediv',  'a / b',   operator.truediv(a, b)),
        ('floordiv', 'a // b',  operator.floordiv(a, b)),
        ('mod',      'a % b',   operator.mod(a, b)),
        ('pow',      'a ** b',  operator.pow(a, b)),
        ('neg',      '-a',      operator.neg(a)),
        ('abs',      'abs(-a)', operator.abs(-a)),
    ]
    for name, expr, result in ops:
        print(f"  operator.{name:<10} ({expr:<10}) = {result}")
    print()

    # ── 比较运算符函数 ────────────────────────────────────
    print("3. 比较运算符函数（a=5, b=8）:")
    print()

    a, b = 5, 8
    cmp_ops = [
        ('lt', 'a < b',  operator.lt(a, b)),
        ('le', 'a <= b', operator.le(a, b)),
        ('eq', 'a == b', operator.eq(a, b)),
        ('ne', 'a != b', operator.ne(a, b)),
        ('ge', 'a >= b', operator.ge(a, b)),
        ('gt', 'a > b',  operator.gt(a, b)),
    ]
    for name, expr, result in cmp_ops:
        print(f"  operator.{name:<5} ({expr:<8}) = {result}")
    print()

    # ── 实际应用：与高阶函数配合 ─────────────────────────
    print("4. 实际应用：配合 functools.reduce:")
    print()

    nums = [1, 2, 3, 4, 5]

    # reduce 需要一个二元函数（接受两个参数的函数）
    # operator.mul 比 lambda x, y: x * y 更简洁且稍快
    product_lambda = functools.reduce(lambda x, y: x * y, nums)
    product_op     = functools.reduce(operator.mul, nums)
    total_op       = functools.reduce(operator.add, nums)

    print(f"  数据: {nums}")
    print(f"  reduce(lambda x,y: x*y) = {product_lambda}")
    print(f"  reduce(operator.mul)    = {product_op}")
    print(f"  reduce(operator.add)    = {total_op}")
    print()

    # 字符串连接
    words = ['Hello', ', ', 'World', '!']
    concat = functools.reduce(operator.add, words)
    print(f"  reduce(operator.add, {words}) = '{concat}'")


# =============================================================================
# 19.2 operator.itemgetter：键/索引访问器
# =============================================================================

def demo_itemgetter() -> None:
    """演示 operator.itemgetter 的用法：代替 lambda x: x[key]。"""
    print("\n" + "=" * 60)
    print("19.2 operator.itemgetter：键/索引访问器")
    print("=" * 60)

    # ── 基础用法 ──────────────────────────────────────────
    print("1. 基础用法（单键）:")
    print()

    # itemgetter(key) 返回一个函数，调用时返回 obj[key]
    get_0    = operator.itemgetter(0)       # 等价于 lambda x: x[0]
    get_name = operator.itemgetter('name')  # 等价于 lambda x: x['name']

    data_list = [10, 20, 30, 40, 50]
    data_dict = {'name': '张三', 'age': 25, 'score': 90}

    print(f"  itemgetter(0)([10..50])      = {get_0(data_list)}")
    print(f"  itemgetter('name')(dict)     = {get_name(data_dict)}")
    print()

    # ── 多键提取（返回元组）──────────────────────────────
    print("2. 多键提取（同时返回多个字段，结果是元组）:")
    print()

    get_name_age = operator.itemgetter('name', 'age')
    get_0_2      = operator.itemgetter(0, 2)

    print(f"  itemgetter('name','age')(dict) = {get_name_age(data_dict)}")
    print(f"  itemgetter(0, 2)([10..50])     = {get_0_2(data_list)}")
    print()

    # ── 配合 sorted 排序 ─────────────────────────────────
    print("3. 配合 sorted 对字典列表排序（最常见用法）:")
    print()

    students = [
        {'name': '张三', 'age': 22, 'score': 85},
        {'name': '李四', 'age': 20, 'score': 92},
        {'name': '王五', 'age': 23, 'score': 78},
        {'name': '赵六', 'age': 21, 'score': 92},
        {'name': '孙七', 'age': 20, 'score': 88},
    ]

    # 按 score 降序
    by_score = sorted(students, key=operator.itemgetter('score'), reverse=True)
    print("  按 score 降序:")
    for s in by_score:
        print(f"    {s['name']}: {s['score']}")
    print()

    # 多字段排序：先按 score 降序，再按 age 升序
    # 注意：itemgetter 多键返回元组，但无法直接实现"一升一降"
    # 此时仍需 lambda 稍加处理
    by_score_age = sorted(
        students,
        key=lambda s: (-s['score'], s['age'])
    )
    print("  按 score 降序、age 升序（score 相同时）:")
    for s in by_score_age:
        print(f"    {s['name']}: score={s['score']}, age={s['age']}")
    print()

    # ── 配合 map 批量提取字段 ────────────────────────────
    print("4. 配合 map 批量提取字段:")
    print()

    names       = list(map(operator.itemgetter('name'), students))
    name_scores = list(map(operator.itemgetter('name', 'score'), students))

    print(f"  所有学生姓名:       {names}")
    print(f"  (姓名, 分数) 元组: {name_scores}")
    print()

    # ── 性能对比 ──────────────────────────────────────────
    print("5. itemgetter vs lambda 性能对比（10万条数据）:")
    print()

    big_list = [{'key': i, 'val': i * 2} for i in range(100_000)]

    t1 = time.perf_counter()
    sorted(big_list, key=lambda x: x['key'])
    t1 = time.perf_counter() - t1

    t2 = time.perf_counter()
    sorted(big_list, key=operator.itemgetter('key'))
    t2 = time.perf_counter() - t2

    print(f"  lambda:      {t1*1000:.2f} ms")
    print(f"  itemgetter:  {t2*1000:.2f} ms")
    print(f"  → itemgetter 是 C 实现，通常快 10-30%")


# =============================================================================
# 19.3 operator.attrgetter 与 methodcaller
# =============================================================================

def demo_attrgetter_methodcaller() -> None:
    """演示 operator.attrgetter 和 operator.methodcaller 的用法。"""
    print("\n" + "=" * 60)
    print("19.3 operator.attrgetter 与 methodcaller")
    print("=" * 60)

    # ── attrgetter：属性访问器 ────────────────────────────
    print("1. operator.attrgetter：代替 lambda x: x.attr")
    print()

    @dataclasses.dataclass
    class Employee:
        """员工数据类。"""
        name: str
        department: str
        salary: float
        years: int  # 工龄

        def get_info(self) -> str:
            """返回员工简介字符串。"""
            return f"{self.name}({self.department})"

    employees = [
        Employee('张三', '研发', 15000.0, 5),
        Employee('李四', '市场', 12000.0, 3),
        Employee('王五', '研发', 18000.0, 8),
        Employee('赵六', '运营', 10000.0, 2),
        Employee('孙七', '市场', 13000.0, 6),
    ]

    # 按 salary 降序
    by_salary = sorted(employees, key=operator.attrgetter('salary'), reverse=True)
    print("  按薪资降序（attrgetter）:")
    for e in by_salary:
        print(f"    {e.name:<6} ¥{e.salary:,.0f}")
    print()

    # 多属性升序排序（按 department 升序，department 相同时按 salary 升序）
    by_dept_salary = sorted(employees, key=operator.attrgetter('department', 'salary'))
    print("  按部门和薪资升序（多属性 attrgetter）:")
    for e in by_dept_salary:
        print(f"    {e.department:<6} {e.name:<6} ¥{e.salary:,.0f}")
    print()

    # 访问嵌套属性（用点号分隔属性名）
    @dataclasses.dataclass
    class Address:
        """地址数据类。"""
        city: str
        district: str

    @dataclasses.dataclass
    class Person:
        """人员数据类（包含嵌套 Address）。"""
        name: str
        address: Address

    persons = [
        Person('张三', Address('北京', '朝阳')),
        Person('李四', Address('上海', '浦东')),
        Person('王五', Address('北京', '海淀')),
    ]

    # 用 'address.city' 访问嵌套属性
    get_city = operator.attrgetter('address.city')
    by_city  = sorted(persons, key=get_city)
    print("  按城市排序（嵌套属性 attrgetter）:")
    for p in by_city:
        print(f"    {p.name}: {p.address.city} {p.address.district}")
    print()

    # ── methodcaller：方法调用器 ──────────────────────────
    print("2. operator.methodcaller：代替 lambda x: x.method(args)")
    print()

    # methodcaller('method', *args, **kwargs) 创建一个调用器
    # 使用时相当于 obj.method(*args, **kwargs)

    words = ['Hello', 'world', 'PYTHON', 'coding', 'FUN']
    print(f"  words = {words}")

    # 等价于 lambda s: s.lower()
    to_lower = operator.methodcaller('lower')
    lowered  = list(map(to_lower, words))
    print(f"  map(methodcaller('lower'))        = {lowered}")

    # 等价于 lambda s: s.ljust(10, '-')
    ljust_10 = operator.methodcaller('ljust', 10, '-')
    padded   = list(map(ljust_10, words))
    print(f"  map(methodcaller('ljust',10,'-')) =")
    for s in padded:
        print(f"    '{s}'")
    print()

    # 对对象列表批量调用方法
    get_info = operator.methodcaller('get_info')
    infos    = list(map(get_info, employees))
    print(f"  批量 get_info(): {infos}")
    print()

    # ── 三者对比总结 ────────────────────────────────────
    print("3. 三种 operator 访问器总结:")
    print()
    print("  ┌──────────────────┬────────────────────┬─────────────────────┐")
    print("  │ 工具              │ 等价 lambda          │ 适用场景            │")
    print("  ├──────────────────┼────────────────────┼─────────────────────┤")
    print("  │ itemgetter('k')  │ lambda x: x['k']   │ 字典/列表元素访问   │")
    print("  │ attrgetter('a')  │ lambda x: x.a      │ 对象属性访问        │")
    print("  │ methodcaller('m')│ lambda x: x.m()    │ 对象方法调用        │")
    print("  └──────────────────┴────────────────────┴─────────────────────┘")


# =============================================================================
# 19.4 heapq 模块：堆队列（优先队列）
# =============================================================================

def demo_heapq() -> None:
    """演示 heapq 模块：Python 的最小堆实现。"""
    print("\n" + "=" * 60)
    print("19.4 heapq 模块：堆队列（优先队列）")
    print("=" * 60)

    print("【概念】")
    print("  堆是满足'堆属性'的完全二叉树：")
    print("  - 最小堆：父节点 ≤ 子节点，堆顶 heap[0] 永远是最小值")
    print("  - 插入/删除 O(log n)，访问最小值 O(1)")
    print("  Python heapq 用普通 list 存储堆（不需要额外的树结构）")
    print("  C++ std::priority_queue 默认最大堆，Python 默认最小堆")
    print()

    # ── 基本操作 ──────────────────────────────────────────
    print("1. 基本操作：heappush、heappop、heapify")
    print()

    # 方法一：从空列表逐个 push，每次 push 都自动维护堆序
    heap: list[int] = []
    for val in [5, 3, 8, 1, 9, 2, 7, 4, 6]:
        heapq.heappush(heap, val)

    print(f"  逐个 heappush 后: {heap}")
    print(f"  heap[0] = {heap[0]}（始终是最小值）")
    print()

    # 方法二：heapify 原地转换，O(n) 时间（比逐个 push 的 O(n log n) 更快）
    data = [5, 3, 8, 1, 9, 2, 7, 4, 6]
    heapq.heapify(data)
    print(f"  heapify([5,3,8,1,9,2,7,4,6]): {data}")
    print(f"  heap[0] = {data[0]}")
    print()

    # 依次 pop 得到升序序列（堆排序）
    temp = heap.copy()
    popped = []
    while temp:
        popped.append(heapq.heappop(temp))
    print(f"  依次 heappop 结果（升序）: {popped}")
    print()

    # ── 高效组合操作 ──────────────────────────────────────
    print("2. 高效组合操作：heappushpop 和 heapreplace")
    print()

    heap2 = [1, 3, 5, 7, 9]
    heapq.heapify(heap2)
    print(f"  初始堆: {heap2}")

    # heappushpop：先 push 新值，再 pop 最小值（只做一次堆调整）
    # 等价于 heappush + heappop，但更高效
    val = heapq.heappushpop(heap2, 4)  # push 4（新最小仍是 1），pop 1
    print(f"  heappushpop(heap, 4) → 弹出={val}，堆={heap2}")

    # heapreplace：先 pop 最小值，再 push 新值（堆不能为空）
    # 比 heappushpop 稍快，因为跳过"新值是否比堆顶小"的判断
    val = heapq.heapreplace(heap2, 0)  # pop 最小的 3，push 0
    print(f"  heapreplace(heap, 0)  → 弹出={val}，堆={heap2}")
    print()

    # ── nlargest / nsmallest：Top-K 问题 ─────────────────
    print("3. nlargest / nsmallest：高效获取 Top-K 元素")
    print()

    scores = [85, 92, 78, 95, 88, 72, 99, 65, 80, 91]
    print(f"  成绩列表: {scores}")
    print(f"  最高3名: {heapq.nlargest(3, scores)}")
    print(f"  最低3名: {heapq.nsmallest(3, scores)}")
    print()

    # 配合 key 参数对字典列表操作
    products = [
        {'name': '苹果', 'price': 5.5,  'stock': 100},
        {'name': '香蕉', 'price': 3.2,  'stock': 50},
        {'name': '葡萄', 'price': 12.8, 'stock': 30},
        {'name': '西瓜', 'price': 8.5,  'stock': 80},
        {'name': '芒果', 'price': 15.0, 'stock': 20},
        {'name': '梨',   'price': 4.5,  'stock': 120},
    ]

    top3_price = heapq.nlargest(3, products, key=operator.itemgetter('price'))
    least2_stock = heapq.nsmallest(2, products, key=operator.itemgetter('stock'))

    print("  最贵的3种商品:")
    for p in top3_price:
        print(f"    {p['name']}: ¥{p['price']}")
    print("  库存最少的2种商品:")
    for p in least2_stock:
        print(f"    {p['name']}: {p['stock']}件")
    print()

    # ── heapq.merge：合并有序流 ───────────────────────────
    print("4. heapq.merge：惰性合并多个有序序列")
    print()

    stream_a = [1, 4, 7, 10, 13]
    stream_b = [2, 5, 8, 11]
    stream_c = [3, 6, 9, 12, 15, 18]

    # merge 返回生成器，不会一次性把所有数据读入内存
    # 适合合并大型有序文件流
    merged = list(heapq.merge(stream_a, stream_b, stream_c))
    print(f"  流A: {stream_a}")
    print(f"  流B: {stream_b}")
    print(f"  流C: {stream_c}")
    print(f"  merge 结果: {merged}")
    print()

    # ── 最大堆技巧 ────────────────────────────────────────
    print("5. 实现最大堆：存负值技巧")
    print()
    print("  Python heapq 只有最小堆。实现最大堆的技巧：存入元素的负值")

    max_heap: list[int] = []
    for val in [5, 3, 8, 1, 9, 2, 7]:
        heapq.heappush(max_heap, -val)  # 存负值

    max_vals = []
    temp = max_heap.copy()
    while temp:
        max_vals.append(-heapq.heappop(temp))  # 弹出时取反
    print(f"  输入: [5, 3, 8, 1, 9, 2, 7]")
    print(f"  最大堆依次弹出（降序）: {max_vals}")
    print()

    # ── 用元组实现优先队列 ────────────────────────────────
    print("6. 用 (priority, item) 元组实现优先队列")
    print()
    print("  元组比较规则：先比第一个元素（优先级），相同时比第二个")
    print("  优先级数字越小 → 越优先（最小堆特性）")
    print()

    pq: list[tuple] = []
    tasks = [
        (3, '发送报告邮件'),
        (1, '修复生产环境 Bug'),
        (2, '代码审查'),
        (1, '服务器告警响应'),
        (4, '更新 README 文档'),
    ]

    for priority, task in tasks:
        heapq.heappush(pq, (priority, task))
        print(f"  添加: [{priority}级] {task}")

    print()
    print("  按优先级处理（数字越小越先）:")
    while pq:
        priority, task = heapq.heappop(pq)
        print(f"  ✓ 处理 [{priority}级] {task}")


# =============================================================================
# 19.5 bisect 模块：二分查找与有序插入
# =============================================================================

def demo_bisect() -> None:
    """演示 bisect 模块：在有序序列中高效查找和插入。"""
    print("\n" + "=" * 60)
    print("19.5 bisect 模块：二分查找与有序插入")
    print("=" * 60)

    print("【概念】")
    print("  bisect（二分法）对已排序列表提供 O(log n) 操作：")
    print("  - bisect_left / bisect_right：查找插入位置")
    print("  - insort_left / insort_right：插入并保持有序")
    print("  C++ 对应：std::lower_bound（left）/ std::upper_bound（right）")
    print()

    # ── bisect_left 和 bisect_right ───────────────────────
    print("1. bisect_left 和 bisect_right 的区别:")
    print()

    sl = [1, 3, 5, 5, 5, 7, 9]
    x  = 5

    pos_left  = bisect.bisect_left(sl, x)   # 插入到所有 5 的最左边
    pos_right = bisect.bisect_right(sl, x)  # 插入到所有 5 的最右边（默认）

    print(f"  列表:         {sl}")
    print(f"  查找值:       {x}")
    print(f"  bisect_left  → 位置 {pos_left}  （插入到现有 5 的最左侧）")
    print(f"  bisect_right → 位置 {pos_right}  （插入到现有 5 的最右侧）")
    print()
    print("  图示:")
    print("  索引: 0  1  2  3  4  5  6")
    print(f"  列表: {sl}")
    print(f"              ↑           ↑")
    print(f"          bisect_left=2  bisect_right=5")
    print()

    # ── 用 bisect 做 O(log n) 存在性查找 ─────────────────
    print("2. 用 bisect 进行 O(log n) 存在性查找:")
    print()

    def contains_sorted(sorted_seq: list, target: Any) -> bool:
        """在有序列表中查找 target 是否存在，O(log n)。"""
        idx = bisect.bisect_left(sorted_seq, target)
        return idx < len(sorted_seq) and sorted_seq[idx] == target

    sd = [1, 3, 5, 7, 9, 11, 13, 15]
    print(f"  有序列表: {sd}")
    for val in [7, 6, 15, 16]:
        found = contains_sorted(sd, val)
        print(f"  contains({val:>2}) → {found}")
    print()

    # ── insort：插入并保持有序 ────────────────────────────
    print("3. insort：插入元素并保持列表有序:")
    print()

    ordered: list[int] = []
    for val in [5, 3, 8, 1, 9, 2, 7, 4, 6]:
        bisect.insort(ordered, val)  # 等价于 insort_right
        print(f"  insort({val}) → {ordered}")
    print()

    # ── 经典应用：成绩等级查表 ───────────────────────────
    print("4. 经典应用：成绩等级查表（O(log n) 替代 if-elif）:")
    print()

    # 分界点列表（升序），bisect_right 找到的索引正好对应等级
    breakpoints = [60, 70, 80, 90]
    grade_names = ['F（不及格）', 'D（及格）', 'C（中等）', 'B（良好）', 'A（优秀）']

    def get_grade(score: int) -> str:
        """O(log n) 成绩等级查找，无需 if-elif 链。"""
        idx = bisect.bisect_right(breakpoints, score)
        return grade_names[idx]

    test_scores = [45, 60, 70, 75, 80, 85, 90, 95, 100]
    print(f"  {'分数':<6} 等级")
    print("  " + "-" * 25)
    for s in test_scores:
        print(f"  {s:<6} {get_grade(s)}")
    print()

    # ── 查找最近邻 ────────────────────────────────────────
    print("5. 实战：在有序列表中查找最近邻:")
    print()

    def find_nearest(sorted_seq: list[float], target: float) -> float:
        """在有序列表中找距离 target 最近的元素，O(log n)。"""
        if not sorted_seq:
            raise ValueError("列表不能为空")
        idx = bisect.bisect_left(sorted_seq, target)
        if idx == 0:
            return sorted_seq[0]
        if idx == len(sorted_seq):
            return sorted_seq[-1]
        left  = sorted_seq[idx - 1]
        right = sorted_seq[idx]
        # 哪边距离更近就返回哪边
        return left if (target - left) <= (right - target) else right

    temps = [15.2, 17.8, 20.1, 22.5, 25.0, 28.3, 31.0]
    print(f"  温度记录: {temps}")
    for q in [16.0, 23.7, 30.0, 10.0, 35.0]:
        nearest = find_nearest(temps, q)
        print(f"  离 {q}°C 最近的记录: {nearest}°C")
    print()

    # ── 性能对比 ──────────────────────────────────────────
    print("6. bisect vs 线性搜索性能对比（100万元素 × 100次查询）:")
    print()

    big = sorted(random.randint(0, 10_000_000) for _ in range(1_000_000))
    queries = [random.choice(big) for _ in range(100)]

    t1 = time.perf_counter()
    for q in queries:
        _ = q in big             # list in 操作：O(n) 线性扫描
    time_linear = time.perf_counter() - t1

    t2 = time.perf_counter()
    for q in queries:
        _ = contains_sorted(big, q)  # bisect：O(log n)
    time_bisect = time.perf_counter() - t2

    print(f"  线性搜索（100次）: {time_linear*1000:.2f} ms")
    print(f"  bisect  搜索（100次）: {time_bisect*1000:.4f} ms")
    if time_bisect > 0:
        speedup = time_linear / time_bisect
        print(f"  bisect 快约 {speedup:.0f} 倍")


# =============================================================================
# 19.6 functools 进阶：singledispatch、cache、cached_property、cmp_to_key
# =============================================================================

def demo_functools_advanced() -> None:
    """演示 functools 的进阶工具（第18章未涵盖的部分）。"""
    print("\n" + "=" * 60)
    print("19.6 functools 进阶：泛型函数与属性缓存")
    print("=" * 60)

    # ── singledispatch：基于类型的泛型函数 ───────────────
    print("1. functools.singledispatch：基于参数类型的泛型函数")
    print()
    print("  【解决什么问题？】")
    print("  需要对不同类型的参数执行不同逻辑时，传统写法是")
    print("  一大串 if isinstance(...) 判断，难以扩展。")
    print("  singledispatch 让你为每种类型注册专门的处理函数，")
    print("  新增类型支持时只需添加一个注册函数，不用改原有代码。")
    print()

    # ── 定义泛型函数 ──────────────────────────────────────
    @functools.singledispatch
    def serialize(data: Any) -> str:
        """默认序列化（兜底处理，任何未注册类型都会调用这里）。"""
        return f"<未知类型 {type(data).__name__}: {data}>"

    # 为 int 注册专门的处理函数
    @serialize.register(int)
    def _(data: int) -> str:
        return f"INT:{data}"

    # 为 float 注册
    @serialize.register(float)
    def _(data: float) -> str:
        return f"FLOAT:{data:.4f}"

    # 为 str 注册
    @serialize.register(str)
    def _(data: str) -> str:
        return f"STR:'{data}'"

    # 为 list 注册
    @serialize.register(list)
    def _(data: list) -> str:
        items = ', '.join(serialize(item) for item in data)
        return f"LIST:[{items}]"

    # 为 dict 注册
    @serialize.register(dict)
    def _(data: dict) -> str:
        pairs = ', '.join(f"{k}={serialize(v)}" for k, v in data.items())
        return f"DICT:{{{pairs}}}"

    # 测试各种类型
    test_values: list[Any] = [42, 3.14, "hello", [1, "two", 3.0], {'a': 1, 'b': "x"}, True]
    print("  测试 singledispatch（自动根据类型分发）:")
    for val in test_values:
        print(f"    serialize({repr(val):<22}) → {serialize(val)}")

    print()
    # 查看已注册的类型
    registered = [t.__name__ for t in serialize.registry if t is not object]
    print(f"  已注册类型: {registered}")
    print()

    # ── functools.cache：无限缓存（Python 3.9+）──────────
    print("2. functools.cache：无限 LRU 缓存（Python 3.9+）")
    print()
    print("  等价于 @lru_cache(maxsize=None)，但实现更简洁高效")
    print("  （不需要维护缓存大小和 LRU 淘汰逻辑，稍快于有界 lru_cache）")
    print()

    call_counter: dict[str, int] = {'count': 0}

    @functools.cache
    def fib(n: int) -> int:
        """有缓存的斐波那契数列。每个 n 只计算一次。"""
        call_counter['count'] += 1
        if n <= 1:
            return n
        return fib(n - 1) + fib(n - 2)

    t_start = time.perf_counter()
    result  = fib(40)
    elapsed = time.perf_counter() - t_start

    print(f"  fib(40) = {result}")
    print(f"  实际函数调用次数: {call_counter['count']} 次")
    print(f"  （无缓存的递归需要调用约 {2**20:,} 次以上）")
    print(f"  耗时: {elapsed*1000:.4f} ms")
    print(f"  缓存信息: {fib.cache_info()}")
    print()

    # ── functools.cached_property：懒计算属性缓存 ────────
    print("3. functools.cached_property：懒计算 + 实例级缓存")
    print()
    print("  【@property vs @cached_property】")
    print("  @property：每次访问都重新计算（适合快速属性）")
    print("  @cached_property：第一次计算后缓存在实例 __dict__ 中")
    print("                     后续访问直接读缓存，不再调用函数")
    print("  （适合计算代价高、结果不变的属性）")
    print()

    class Circle:
        """圆形类，演示 cached_property 的懒计算效果。"""

        def __init__(self, radius: float) -> None:
            self.radius = radius
            self._calc_count = 0  # 记录实际计算次数

        @property
        def diameter_normal(self) -> float:
            """普通 property：每次访问都计算。"""
            import math
            self._calc_count += 1
            return 2 * math.pi * self.radius  # 模拟复杂计算

        @functools.cached_property
        def area(self) -> float:
            """面积：只计算一次，之后从缓存返回。"""
            import math
            self._calc_count += 1
            print(f"    [计算] area 被计算（第 {self._calc_count} 次调用函数）")
            return math.pi * self.radius ** 2

        @functools.cached_property
        def circumference(self) -> float:
            """周长：只计算一次。"""
            import math
            self._calc_count += 1
            print(f"    [计算] circumference 被计算（第 {self._calc_count} 次调用函数）")
            return 2 * math.pi * self.radius

    c = Circle(5.0)
    print(f"  创建 Circle(radius=5.0)")
    print()

    print(f"  第1次访问 area:")
    print(f"    c.area = {c.area:.4f}")   # 触发计算
    print(f"  第2次访问 area:")
    print(f"    c.area = {c.area:.4f}")   # 直接从缓存读取，不再打印"[计算]"
    print(f"  第3次访问 area:")
    print(f"    c.area = {c.area:.4f}")   # 同上
    print()

    print(f"  访问 circumference:")
    print(f"    c.circumference = {c.circumference:.4f}")  # 触发计算
    print(f"  再次访问 circumference:")
    print(f"    c.circumference = {c.circumference:.4f}")  # 从缓存读取
    print()
    print(f"  总函数调用次数: {c._calc_count} 次")
    print(f"  （若无缓存，5次访问应触发 5 次计算）")
    print()

    # 查看缓存存储在实例的 __dict__ 中
    print(f"  c.__dict__ = {c.__dict__}")
    print("  （可以看到 area 和 circumference 已被存入实例字典）")
    print()

    # 清除缓存：删除实例字典中的对应键
    del c.__dict__['area']
    print("  删除 c.__dict__['area'] 后再次访问：")
    print(f"    c.area = {c.area:.4f}")  # 重新计算
    print()

    # ── functools.cmp_to_key：旧式比较函数转换 ───────────
    print("4. functools.cmp_to_key：将 C 风格比较函数转为 key 函数")
    print()
    print("  Python 3 的 sorted() 只接受 key 函数（返回排序键）")
    print("  Python 2 支持 cmp 函数（接受两个参数，返回 -1/0/1）")
    print("  cmp_to_key 将旧式 cmp 函数适配为 key 函数")
    print()
    print("  适用场景：实现复杂排序逻辑，或移植旧代码")
    print()

    # 示例：对版本号字符串排序（"1.10.2" > "1.9.3"，但字符串比较不对）
    versions = ['1.9.3', '1.10.2', '2.0.0', '1.10.1', '1.2.0', '10.0.0']

    # 错误做法：字符串排序（"1.10.2" < "1.9.3"，因为 '1' < '9'）
    wrong_sort = sorted(versions)
    print(f"  版本号列表: {versions}")
    print(f"  字符串排序（错误）: {wrong_sort}")

    # 正确做法1：key 函数（推荐）
    correct_key = sorted(versions, key=lambda v: tuple(int(x) for x in v.split('.')))
    print(f"  key 函数排序（正确）: {correct_key}")

    # 正确做法2：cmp_to_key（演示 cmp_to_key 的用法）
    def version_cmp(v1: str, v2: str) -> int:
        """版本号比较函数（C 风格：返回 -1/0/1）。"""
        parts1 = [int(x) for x in v1.split('.')]
        parts2 = [int(x) for x in v2.split('.')]
        for p1, p2 in zip(parts1, parts2):
            if p1 < p2:
                return -1
            elif p1 > p2:
                return 1
        return len(parts1) - len(parts2)

    correct_cmp = sorted(versions, key=functools.cmp_to_key(version_cmp))
    print(f"  cmp_to_key 排序（正确）: {correct_cmp}")


# =============================================================================
# 19.7 综合实战：任务调度系统
# =============================================================================

def demo_task_scheduler() -> None:
    """综合实战：用 heapq + operator + functools 实现任务调度系统。"""
    print("\n" + "=" * 60)
    print("19.7 综合实战：带优先级的任务调度系统")
    print("=" * 60)

    print("【场景说明】")
    print("  模拟一个简单的任务调度器：")
    print("  - 任务有优先级（1=最高，5=最低）和类型（CPU密集/IO密集）")
    print("  - 按优先级顺序处理任务")
    print("  - 统计各类型任务的处理情况")
    print()

    # ── 任务数据结构 ──────────────────────────────────────
    @dataclasses.dataclass
    class Task:
        """任务数据类。"""
        task_id:  int
        priority: int           # 1(最高) ~ 5(最低)
        task_type: str          # 'CPU' 或 'IO'
        name:     str
        duration: float         # 模拟执行时长（秒）

        def __lt__(self, other: 'Task') -> bool:
            """用于堆比较：优先级数字小的排在堆顶。"""
            return self.priority < other.priority

    # ── 调度器实现 ────────────────────────────────────────
    class TaskScheduler:
        """基于堆的任务调度器。"""

        def __init__(self) -> None:
            self._heap: list[tuple[int, int, Task]] = []  # (priority, seq, task)
            self._seq  = 0          # 序列号（相同优先级时保证 FIFO）
            self._done: list[Task]  = []
            self._total_duration    = 0.0

        def submit(self, task: Task) -> None:
            """提交任务到调度队列。"""
            # 用 (priority, seq, task) 三元组，避免 Task 对象直接比较
            # 相同优先级时，seq 小的先处理（FIFO 语义）
            heapq.heappush(self._heap, (task.priority, self._seq, task))
            self._seq += 1
            print(f"  [提交] [{task.priority}级] {task.name} ({task.task_type})")

        def run_all(self) -> None:
            """按优先级顺序运行所有任务。"""
            print()
            print("  ── 开始调度执行 ──")
            while self._heap:
                priority, _, task = heapq.heappop(self._heap)
                print(f"  [执行] [{priority}级] {task.name:<20} "
                      f"({task.task_type}, {task.duration:.1f}s)")
                self._done.append(task)
                self._total_duration += task.duration

        def report(self) -> None:
            """生成调度报告。"""
            print()
            print("  ── 调度报告 ──")

            # 按任务类型统计（用 defaultdict + sorted + groupby 思路）
            import collections
            type_stats: dict[str, dict] = collections.defaultdict(
                lambda: {'count': 0, 'total_duration': 0.0}
            )
            for task in self._done:
                type_stats[task.task_type]['count'] += 1
                type_stats[task.task_type]['total_duration'] += task.duration

            print(f"  总任务数: {len(self._done)}")
            print(f"  总耗时:   {self._total_duration:.1f}s")
            print()

            for task_type, stats in sorted(type_stats.items()):
                avg = stats['total_duration'] / stats['count']
                print(f"  {task_type} 任务: {stats['count']}个, "
                      f"总耗时={stats['total_duration']:.1f}s, "
                      f"平均={avg:.1f}s")

            # 找出最高优先级任务（用 heapq.nsmallest 取 priority 最小的）
            if self._done:
                top_tasks = heapq.nsmallest(
                    3, self._done,
                    key=operator.attrgetter('priority')
                )
                print()
                print("  最高优先级的3个任务:")
                for t in top_tasks:
                    print(f"    [{t.priority}级] {t.name}")

    # ── 构造测试数据 ──────────────────────────────────────
    scheduler = TaskScheduler()

    task_data = [
        Task(1,  3, 'CPU', '数据压缩',          2.5),
        Task(2,  1, 'IO',  '生产环境告警响应',   0.5),
        Task(3,  5, 'IO',  '发送周报邮件',       1.0),
        Task(4,  2, 'CPU', '机器学习模型训练',   8.0),
        Task(5,  1, 'CPU', '修复崩溃 Bug',       3.0),
        Task(6,  4, 'IO',  '备份数据库',         5.0),
        Task(7,  3, 'CPU', '生成报表',           2.0),
        Task(8,  2, 'IO',  '同步用户数据',       1.5),
        Task(9,  5, 'CPU', '更新文档索引',       4.0),
        Task(10, 1, 'IO',  '紧急数据恢复',       6.0),
    ]

    print("  提交任务（按提交顺序）:")
    for task in task_data:
        scheduler.submit(task)

    scheduler.run_all()
    scheduler.report()

    # ── 用 bisect 实现有序日志时间线 ──────────────────────
    print()
    print("  ── 附加演示：用 bisect 维护有序时间线 ──")
    print()

    # 模拟日志时间戳（乱序到达）
    log_timeline: list[float] = []
    log_times = [1.5, 0.3, 2.8, 1.2, 0.8, 3.5, 2.1, 0.5]

    for t in log_times:
        bisect.insort(log_timeline, t)  # 插入并保持有序

    print(f"  乱序到达的时间戳: {log_times}")
    print(f"  bisect insort 后（有序）: {log_timeline}")

    # 查找某个时间点之后的所有日志
    query_time = 1.5
    idx = bisect.bisect_left(log_timeline, query_time)
    after_query = log_timeline[idx:]
    print(f"  {query_time}s 之后（含）的日志: {after_query}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示函数。"""
    demo_operator_basics()
    demo_itemgetter()
    demo_attrgetter_methodcaller()
    demo_heapq()
    demo_bisect()
    demo_functools_advanced()
    demo_task_scheduler()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── operator 速查 ──────────────────────────────────────────
# operator.add(a, b)              a + b
# operator.sub(a, b)              a - b
# operator.mul(a, b)              a * b
# operator.truediv(a, b)          a / b
# operator.floordiv(a, b)         a // b
# operator.mod(a, b)              a % b
# operator.pow(a, b)              a ** b
# operator.neg(a)                 -a
# operator.abs(a)                 abs(a)
# operator.lt/le/eq/ne/ge/gt      比较运算符
# operator.itemgetter(key)        lambda x: x[key]（字典/列表）
# operator.attrgetter(attr)       lambda x: x.attr（对象属性）
# operator.methodcaller(m, *a)    lambda x: x.m(*a)（方法调用）
#
# ── heapq 速查 ────────────────────────────────────────────
# heapq.heappush(heap, item)      插入元素，维护堆序
# heapq.heappop(heap)             弹出并返回最小元素
# heapq.heapify(lst)              原地将列表转为堆，O(n)
# heapq.heappushpop(heap, item)   push 后 pop，单次调整
# heapq.heapreplace(heap, item)   pop 后 push，单次调整
# heapq.nlargest(n, it, key=None) 返回最大的 n 个元素
# heapq.nsmallest(n, it, key=None)返回最小的 n 个元素
# heapq.merge(*its, key=None)     惰性合并多个有序序列
#
# ── bisect 速查 ───────────────────────────────────────────
# bisect.bisect_left(a, x)        x 的最左插入位置（≥x的第一个）
# bisect.bisect_right(a, x)       x 的最右插入位置（>x的第一个）
# bisect.bisect(a, x)             同 bisect_right
# bisect.insort_left(a, x)        插入到最左位置，保持有序
# bisect.insort_right(a, x)       插入到最右位置，保持有序
# bisect.insort(a, x)             同 insort_right
#
# ── functools 进阶速查 ────────────────────────────────────
# @functools.singledispatch        泛型函数，按第一个参数类型分发
# func.register(type)              为指定类型注册处理函数
# @functools.cache                 无限缓存（Python 3.9+）
# func.cache_info()                查看缓存命中/未命中统计
# func.cache_clear()               清空缓存
# @functools.cached_property       懒计算属性，结果缓存在实例 __dict__
# del obj.__dict__['attr']         手动清除 cached_property 缓存
# functools.cmp_to_key(cmp_func)   将旧式比较函数（返回-1/0/1）转为 key 函数


# =============================================================================
# 【常见错误】
# =============================================================================
#
# ❌ 错误 1：heapq 元素之间无法比较时报错
# tasks = [Task('B'), Task('A')]
# heapq.heappush(tasks, Task('C'))  # ❌ 如果 Task 未实现 __lt__，会报 TypeError
# 解决：给 Task 实现 __lt__，或使用 (priority, seq, task) 元组包装
#
# ❌ 错误 2：bisect 用于未排序的列表
# lst = [3, 1, 4, 1, 5]
# bisect.bisect_left(lst, 3)  # ❌ 结果未定义（列表必须是有序的）
# 解决：使用前先 sort()，或用 insort 维护有序性
#
# ❌ 错误 3：heapq.heapreplace 用于空堆
# heap = []
# heapq.heapreplace(heap, 1)  # ❌ IndexError
# 解决：检查 if heap: 再调用
#
# ❌ 错误 4：cached_property 用于不可变/has __slots__ 的类
# class MyClass:
#     __slots__ = ['x']     # ❌ cached_property 需要写入 __dict__，但 __slots__ 类没有
#     @functools.cached_property
#     def val(self): return 42
# 解决：去掉 __slots__，或改用 lru_cache
#
# ❌ 错误 5：singledispatch 的第一个参数类型不匹配
# @functools.singledispatch
# def f(x): ...
# @f.register(int)
# def _(x, y): ...   # ❌ 忘记了 singledispatch 只看第一个参数
# f(1, 2)            # ❌ 会调用到错误的函数签名
# 解决：注册的函数签名要与调用保持一致
#
# ❌ 错误 6：operator.itemgetter 用于没有该键的字典
# get_x = operator.itemgetter('x')
# get_x({'y': 1})  # ❌ KeyError: 'x'
# 解决：确保字典包含所需键，或改用 .get() + lambda


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   有一组产品字典列表，每个字典有 name、price、rating（评分）字段。
#   用 operator.itemgetter 完成以下排序：
#   (a) 按 price 升序
#   (b) 按 rating 降序
#   (c) 按 (rating 降序, price 升序) 双字段排序
#   例如：
#     products = [
#         {'name': '耳机', 'price': 299.0, 'rating': 4.5},
#         {'name': '键盘', 'price': 159.0, 'rating': 4.8},
#         ...
#     ]
#     # (a) 按 price 升序: [鼠标(89), 键盘(159), ...]
#     # (b) 按 rating 降序: [键盘(4.8), 显示器(4.7), ...]
#
# 练习 2（进阶）：
#   实现一个"K路归并"函数 k_way_merge(sorted_lists)：
#   给定 k 个已排序的列表，将它们合并为一个有序列表
#   （不使用 heapq.merge，自己用 heappush/heappop 实现）。
#   返回格式：合并后的有序列表
#   例如：
#     lists = [[1, 4, 7], [2, 5, 8], [3, 6, 9]]
#     k_way_merge(lists)
#     # [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ...]
#
# 练习 3（综合）：
#   给定一个有序的考试截止时间列表（float 时间戳），
#   以及一批学生提交时间，用 bisect 判断每个学生是否按时提交。
#   返回格式：[(学生名, 提交时间, 是否按时), ...]
#   例如：
#     deadlines = [10.0, 20.0, 30.0]  # 三次作业的截止时间
#     submissions = [('张三', 9.5), ('李四', 15.0), ('王五', 25.0)]
#     check_submissions(deadlines, submissions)
#     # [('张三', 9.5, True), ('李四', 15.0, True), ('王五', 25.0, True)]
#
# 练习 4（高级）：
#   用 functools.singledispatch 实现一个 flatten 函数：
#   - 输入 int/float/str → 直接返回 [val]
#   - 输入 list → 递归展平所有元素
#   - 输入 dict → 展平所有 value
#   要求：使用 singledispatch 装饰器为不同类型注册处理函数
#   例如：
#     flatten([[1, [2, 3]], 4, {'a': [5, 6]}])
#     # [1, 2, 3, 4, 5, 6]
#
# 练习答案提示：
#   练习1：(a) sorted(products, key=operator.itemgetter('price'))
#          (b) sorted(products, key=operator.itemgetter('rating'), reverse=True)
#          (c) sorted(products, key=lambda p: (-p['rating'], p['price']))
#   练习2：用堆存储 (当前值, 列表索引, 元素索引) 三元组；
#          初始时从每个列表取第一个元素入堆；弹出最小值后从同一列表取下一个元素
#   练习3：用 bisect_right 找到提交时间对应的截止时间索引；
#          如果索引为0说明在第一个截止时间之前，否则检查是否 <= deadlines[idx-1]
#   练习4：@functools.singledispatch 定义默认处理；
#          @func.register(list) 注册列表处理；@func.register(dict) 注册字典处理


# =============================================================================
# 【练习答案】
# =============================================================================


def sort_products_by_fields(
    products: list[dict],
) -> tuple[list[dict], list[dict], list[dict]]:
    """
    练习 1：用 operator.itemgetter 对产品列表进行多字段排序。

    Args:
        products: 包含 name、price、rating 字段的字典列表

    Returns:
        三个有序列表组成的元组：
        (按 price 升序, 按 rating 降序, 按 rating 降序+price 升序)

    Example:
        products = [
            {'name': '耳机', 'price': 299.0, 'rating': 4.5},
            {'name': '键盘', 'price': 159.0, 'rating': 4.8},
            ...
        ]
        by_price, by_rating, by_rating_price = sort_products_by_fields(products)
        # by_price[0]['name']  → '鼠标'（最便宜）
        # by_rating[0]['name'] → '键盘'（评分最高）
    """
    # (a) 按 price 升序
    by_price = sorted(products, key=operator.itemgetter('price'))

    # (b) 按 rating 降序
    by_rating = sorted(products, key=operator.itemgetter('rating'), reverse=True)

    # (c) 按 rating 降序、price 升序（rating 相同时按 price 升序）
    by_rating_price = sorted(products, key=lambda p: (-p['rating'], p['price']))

    return by_price, by_rating, by_rating_price


def k_way_merge(sorted_lists: list[list[int]]) -> list[int]:
    """
    练习 2：K 路归并——将 k 个已排序的列表合并为一个有序列表。

    Args:
        sorted_lists: k 个已按升序排列的整数列表

    Returns:
        合并后的有序列表

    Example:
        lists = [[1, 4, 7, 10], [2, 5, 8, 11, 14], [3, 6, 9]]
        k_way_merge(lists)
        # [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14]
    """
    # 堆中存储 (当前最小值, 来自哪个列表, 该列表的下一个索引)
    heap: list[tuple[int, int, int]] = []

    # 初始化：从每个列表取第一个元素入堆
    for list_idx, lst in enumerate(sorted_lists):
        if lst:
            heapq.heappush(heap, (lst[0], list_idx, 0))

    result: list[int] = []
    while heap:
        val, list_idx, item_idx = heapq.heappop(heap)
        result.append(val)

        # 从同一个列表取下一个元素继续入堆
        next_idx = item_idx + 1
        if next_idx < len(sorted_lists[list_idx]):
            next_val = sorted_lists[list_idx][next_idx]
            heapq.heappush(heap, (next_val, list_idx, next_idx))

    return result


def check_submissions(
    deadlines: list[float],
    submissions: list[tuple[str, float]],
) -> list[tuple[str, float, bool]]:
    """
    练习 3：用 bisect 判断学生是否按时提交作业。

    Args:
        deadlines:   有序的截止时间列表（float 时间戳，升序）
        submissions: (学生名, 提交时间) 元组列表

    Returns:
        [(学生名, 提交时间, 是否按时), ...] 列表

    Example:
        deadlines = [10.0, 20.0, 30.0]
        submissions = [('张三', 9.5), ('李四', 15.0), ('王五', 35.0)]
        check_submissions(deadlines, submissions)
        # [('张三', 9.5, True), ('李四', 15.0, True), ('王五', 35.0, False)]
    """
    results = []
    for name, submit_time in submissions:
        # bisect_right 找到提交时间在 deadlines 中的插入位置
        # 若 idx == 0：提交时间早于所有截止时间 → 按时
        # 若 idx == len(deadlines)：提交时间晚于最后一个截止时间 → 逾期
        # 否则：提交时间 <= deadlines[idx] → 按时
        idx = bisect.bisect_right(deadlines, submit_time)
        on_time = idx < len(deadlines)
        results.append((name, submit_time, on_time))
    return results


@functools.singledispatch
def flatten(data: Any) -> list:
    """
    练习 4：用 singledispatch 实现递归展平函数。

    默认处理：将任意标量值包装为单元素列表（int、float、str 等）。

    Args:
        data: 任意类型的数据

    Returns:
        展平后的列表

    Example:
        flatten([[1, [2, 3]], 4, {'a': [5, 6]}])
        # [1, 2, 3, 4, 5, 6]
    """
    return [data]


@flatten.register(list)
def _(data: list) -> list:
    """展平列表：递归展平每个元素。"""
    result = []
    for item in data:
        result.extend(flatten(item))
    return result


@flatten.register(dict)
def _(data: dict) -> list:
    """展平字典：递归展平所有 value（忽略 key）。"""
    result = []
    for val in data.values():
        result.extend(flatten(val))
    return result


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 sort_products_by_fields 函数。"""
    print("sort_products_by_fields 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    products = [
        {'name': '耳机',  'price': 299.0,  'rating': 4.5},
        {'name': '键盘',  'price': 159.0,  'rating': 4.8},
        {'name': '鼠标',  'price': 89.0,   'rating': 4.5},
        {'name': '显示器', 'price': 1299.0, 'rating': 4.7},
        {'name': '摄像头', 'price': 199.0,  'rating': 4.3},
    ]

    by_price, by_rating, by_rating_price = sort_products_by_fields(products)

    # (a) 按 price 升序
    print("  (a) 按 price 升序:")
    for p in by_price:
        print(f"    {p['name']:<8} ¥{p['price']}")

    # (b) 按 rating 降序
    print("  (b) 按 rating 降序:")
    for p in by_rating:
        print(f"    {p['name']:<8} ★{p['rating']}")

    # (c) 按 rating 降序 + price 升序
    print("  (c) 按 rating 降序 + price 升序（rating 相同时）:")
    for p in by_rating_price:
        print(f"    {p['name']:<8} ★{p['rating']} ¥{p['price']}")


def exercise2_answer() -> None:
    """练习 2：演示 k_way_merge 函数。"""
    print("k_way_merge 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    lists = [
        [1, 4, 7, 10],
        [2, 5, 8, 11, 14],
        [3, 6, 9],
        [0, 12, 15],
    ]
    print(f"  输入列表:")
    for i, lst in enumerate(lists):
        print(f"    列表{i}: {lst}")

    merged = k_way_merge(lists)
    print(f"  合并结果: {merged}")

    # 验证与 heapq.merge 结果一致
    expected = list(heapq.merge(*lists))
    print(f"  heapq.merge 结果: {expected}")
    print(f"  结果正确: {merged == expected}")

    # ── 边界情况 ─────────────────────────────────────────
    print(f"\n  边界情况：含空列表:")
    lists2 = [[1, 3, 5], [], [2, 4, 6], []]
    merged2 = k_way_merge(lists2)
    print(f"    输入: {lists2}")
    print(f"    结果: {merged2}")


def exercise3_answer() -> None:
    """练习 3：演示 check_submissions 函数。"""
    print("check_submissions 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    deadlines = [10.0, 20.0, 30.0]   # 三次作业的截止时间
    submissions = [
        ('张三', 9.5),    # 第1次作业截止前提交 → 按时
        ('李四', 15.0),   # 第2次作业截止前提交 → 按时
        ('王五', 25.0),   # 第3次作业截止前提交 → 按时
        ('赵六', 31.0),   # 超过最后一次截止时间 → 逾期
        ('孙七', 10.0),   # 恰好在第1次截止时间 → 逾期（bisect_right）
    ]

    print(f"  截止时间: {deadlines}")
    print(f"\n  {'学生':<6} {'提交时间':<10} {'是否按时'}")
    print("  " + "-" * 30)

    results = check_submissions(deadlines, submissions)
    for name, submit_time, on_time in results:
        status = '✓ 按时' if on_time else '✗ 逾期'
        print(f"  {name:<6} {submit_time:<10} {status}")

    # ── 验证边界：恰好在截止时间 ──────────────────────────
    print(f"\n  注意：bisect_right 判断 submit_time <= deadline")
    print(f"  提交时间=10.0，截止=10.0 → 逾期（10.0 不在 deadline 区间内）")


def exercise4_answer() -> None:
    """练习 4：演示 flatten 函数（singledispatch 实现）。"""
    print("flatten 演示:")

    # ── 测试各种输入 ──────────────────────────────────────
    test_cases = [
        [[1, [2, 3]], 4, {'a': [5, 6]}],
        [1, 2, 3],
        [[1, [2, [3, [4]]]]],
        {'x': [1, 2], 'y': {'z': [3, 4]}},
        42,
        'hello',
    ]

    for tc in test_cases:
        result = flatten(tc)
        print(f"  flatten({tc!r})")
        print(f"    → {result}")

    # ── 验证已注册类型 ────────────────────────────────────
    print(f"\n  已注册类型: "
          f"{[t.__name__ for t in flatten.registry if t is not object]}")


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
#
#     print("=" * 40)
#     exercise4_answer()
