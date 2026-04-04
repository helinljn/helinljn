# =============================================================================
# 第 18 章：数据结构工具（collections、itertools、functools）
# =============================================================================
#
# 【学习目标】
#   1. 掌握 collections：高性能容器数据类型
#   2. 掌握 itertools：迭代器工具函数
#   3. 掌握 functools：高阶函数与函数工具
#   4. 学会在实际场景中选择合适的工具
#
# 【这些模块解决什么问题？】
#   collections: 比内置 dict/list 更强大的数据结构（计数、双端队列、有序字典等）
#   itertools:   高效的迭代器组合工具，避免手写复杂循环
#   functools:   函数装饰、缓存、偏函数等函数式编程工具
#
# 【与 C/C++ 的对比】
#   C/C++:  需要 STL 中的 map/multimap/deque，或手写算法
#   Python: 标准库直接提供，语法简洁，性能良好
#
# 【运行方式】
#   python chapter18_数据结构工具.py
#
# =============================================================================

import collections
import itertools
import functools
import operator
from typing import Any


# =============================================================================
# 18.1 collections.Counter：计数器
# =============================================================================

def demo_counter() -> None:
    """演示 collections.Counter 的用法。"""
    print("=" * 60)
    print("18.1 collections.Counter：计数器")
    print("=" * 60)

    # ── 创建 Counter ─────────────────────────────────────
    print("1. 创建 Counter:")

    # 从可迭代对象创建（统计每个元素出现次数）
    c1 = collections.Counter('abracadabra')
    print(f"  字符统计: {c1}")

    # 从列表创建
    words = ['apple', 'banana', 'apple', 'cherry', 'banana', 'apple']
    c2 = collections.Counter(words)
    print(f"  单词统计: {c2}")

    # 从字典创建
    c3 = collections.Counter({'a': 3, 'b': 2, 'c': 1})
    print(f"  从字典创建: {c3}")

    # 空 Counter + 手动更新
    c4 = collections.Counter()
    c4.update('hello')
    c4.update('world')
    print(f"  逐步更新: {c4}")
    print()

    # ── 访问计数 ─────────────────────────────────────────
    print("2. 访问计数:")
    c = collections.Counter('banana')
    print(f"  Counter: {c}")
    print(f"  'a' 的数量: {c['a']}")       # 3
    print(f"  'z' 的数量: {c['z']}")       # 0（不存在返回0，不报错）
    print(f"  最常见的2个: {c.most_common(2)}")  # [('a', 3), ('n', 2)]
    print(f"  所有元素: {list(c.elements())}")   # 按计数重复展开
    print()

    # ── Counter 运算 ─────────────────────────────────────
    print("3. Counter 运算（+、-、&、|）:")
    c_a = collections.Counter(a=3, b=2, c=1)
    c_b = collections.Counter(a=1, b=5, d=2)

    print(f"  A = {c_a}")
    print(f"  B = {c_b}")
    print(f"  A + B = {c_a + c_b}")  # 相加
    print(f"  A - B = {c_a - c_b}")  # 相减（只保留正数）
    print(f"  A & B = {c_a & c_b}")  # 交集（取最小值）
    print(f"  A | B = {c_a | c_b}")  # 并集（取最大值）
    print()

    # ── 实战：词频统计 ───────────────────────────────────
    print("4. 实战：文章词频统计")
    text = """
    Python 是一种解释型高级编程语言 Python 以简洁优雅著称
    Python 广泛用于数据科学 机器学习 Web 开发等领域
    学习 Python 让编程变得有趣而简单
    """
    words = text.split()
    word_count = collections.Counter(words)
    print("  出现频率最高的5个词:")
    for word, count in word_count.most_common(5):
        bar = '█' * count
        print(f"    {word:<10} {bar} ({count})")


# =============================================================================
# 18.2 collections.defaultdict：默认值字典
# =============================================================================

def demo_defaultdict() -> None:
    """演示 collections.defaultdict 的用法。"""
    print("\n" + "=" * 60)
    print("18.2 collections.defaultdict：默认值字典")
    print("=" * 60)

    # ── 普通 dict 的痛点 ─────────────────────────────────
    print("1. 普通 dict vs defaultdict:")

    # 普通 dict：访问不存在的键会报 KeyError
    # 需要用 setdefault 或先判断
    normal_dict: dict[str, list] = {}
    for char in 'hello':
        if char not in normal_dict:        # 需要先检查
            normal_dict[char] = []
        normal_dict[char].append(1)
    print(f"  普通 dict（需要手动初始化）: {normal_dict}")

    # defaultdict：自动初始化默认值
    dd = collections.defaultdict(list)     # 默认值工厂：list
    for char in 'hello':
        dd[char].append(1)                 # 不存在时自动创建空列表
    print(f"  defaultdict（自动初始化）:  {dict(dd)}")
    print()

    # ── 常用默认值类型 ───────────────────────────────────
    print("2. 常用默认值类型:")

    # int → 默认 0（常用于计数）
    count_dd = collections.defaultdict(int)
    for word in 'apple banana apple cherry banana apple'.split():
        count_dd[word] += 1              # 不存在时默认 0，然后 +1
    print(f"  defaultdict(int)  → {dict(count_dd)}")

    # list → 默认 []（常用于分组）
    list_dd = collections.defaultdict(list)
    data = [('水果', '苹果'), ('蔬菜', '胡萝卜'), ('水果', '香蕉'), ('蔬菜', '白菜')]
    for category, item in data:
        list_dd[category].append(item)
    print(f"  defaultdict(list) → {dict(list_dd)}")

    # set → 默认 set()（常用于去重分组）
    set_dd = collections.defaultdict(set)
    for category, item in data:
        set_dd[category].add(item)
    print(f"  defaultdict(set)  → {dict(set_dd)}")

    # 自定义默认值
    def default_value():
        return {'count': 0, 'items': []}

    custom_dd = collections.defaultdict(default_value)
    custom_dd['user1']['count'] += 1
    custom_dd['user1']['items'].append('购买了苹果')
    print(f"  defaultdict(自定义) → {dict(custom_dd)}")
    print()

    # ── 实战：按首字母分组 ───────────────────────────────
    print("3. 实战：按首字母分组单词")
    words = ['apple', 'ant', 'banana', 'bear', 'cat', 'cherry', 'avocado']
    grouped = collections.defaultdict(list)
    for word in sorted(words):
        grouped[word[0]].append(word)

    for letter in sorted(grouped):
        print(f"  {letter}: {grouped[letter]}")


# =============================================================================
# 18.3 collections.OrderedDict：有序字典
# =============================================================================

def demo_ordereddict() -> None:
    """演示 collections.OrderedDict 的用法。"""
    print("\n" + "=" * 60)
    print("18.3 collections.OrderedDict：有序字典")
    print("=" * 60)

    # 注意：Python 3.7+ 普通 dict 已保持插入顺序
    # OrderedDict 提供额外的有序操作方法

    print("1. OrderedDict 特有方法:")

    od = collections.OrderedDict()
    od['first'] = 1
    od['second'] = 2
    od['third'] = 3
    od['fourth'] = 4
    print(f"  初始顺序: {list(od.keys())}")

    # move_to_end：将某个键移到末尾或开头
    od.move_to_end('first')           # 移到末尾
    print(f"  'first' 移到末尾: {list(od.keys())}")

    od.move_to_end('fourth', last=False)  # 移到开头
    print(f"  'fourth' 移到开头: {list(od.keys())}")

    # popitem：从末尾或开头弹出
    last_item = od.popitem(last=True)     # 弹出末尾
    print(f"  弹出末尾: {last_item}")

    first_item = od.popitem(last=False)   # 弹出开头
    print(f"  弹出开头: {first_item}")
    print()

    # ── 实战：LRU 缓存（最近最少使用）─────────────────
    print("2. 实战：用 OrderedDict 实现 LRU 缓存")

    class LRUCache:
        """LRU（Least Recently Used）缓存：超出容量时淘汰最久未使用的项。"""

        def __init__(self, capacity: int) -> None:
            self.capacity = capacity
            self._cache = collections.OrderedDict()

        def get(self, key: str) -> Any:
            """获取缓存，并将其移到最近使用位置。"""
            if key not in self._cache:
                return None
            self._cache.move_to_end(key)  # 最近使用，移到末尾
            return self._cache[key]

        def put(self, key: str, value: Any) -> None:
            """写入缓存，超容量时淘汰最久未使用的项。"""
            if key in self._cache:
                self._cache.move_to_end(key)
            self._cache[key] = value
            if len(self._cache) > self.capacity:
                # 淘汰最久未使用（开头）的项
                evicted_key, _ = self._cache.popitem(last=False)
                print(f"  ⚡ 淘汰缓存: '{evicted_key}'")

        def __repr__(self) -> str:
            return f"LRU{dict(self._cache)}"

    cache = LRUCache(capacity=3)
    operations = [
        ('put', 'A', 1), ('put', 'B', 2), ('put', 'C', 3),
        ('get', 'A', None),              # 使用 A，A 变为最近
        ('put', 'D', 4),                 # 容量满了，淘汰最久未使用的 B
        ('get', 'B', None),              # B 已被淘汰
        ('put', 'E', 5),                 # 淘汰 C
    ]

    for op in operations:
        if op[0] == 'put':
            _, key, val = op
            cache.put(key, val)
            print(f"  put('{key}', {val}) → {cache}")
        else:
            _, key, _ = op
            val = cache.get(key)
            print(f"  get('{key}') → {val}  缓存: {cache}")


# =============================================================================
# 18.4 collections.deque：双端队列
# =============================================================================

def demo_deque() -> None:
    """演示 collections.deque 的用法。"""
    print("\n" + "=" * 60)
    print("18.4 collections.deque：双端队列")
    print("=" * 60)

    # ── 基本操作 ─────────────────────────────────────────
    print("1. 基本操作（两端都能快速插入/删除）:")

    dq = collections.deque([1, 2, 3, 4, 5])
    print(f"  初始: {dq}")

    dq.append(6)          # 右端添加（O(1)）
    dq.appendleft(0)      # 左端添加（O(1)）
    print(f"  两端各加一个: {dq}")

    right = dq.pop()      # 右端弹出（O(1)）
    left = dq.popleft()   # 左端弹出（O(1)）
    print(f"  弹出右={right}, 左={left}: {dq}")

    # 对比：list 在左端操作是 O(n)，deque 是 O(1)
    dq.extend([7, 8])         # 右端批量添加
    dq.extendleft([-1, -2])   # 左端批量添加（注意顺序是反的）
    print(f"  批量添加: {dq}")
    print()

    # ── 旋转 ─────────────────────────────────────────────
    print("2. 旋转（rotate）:")
    dq = collections.deque([1, 2, 3, 4, 5])
    print(f"  原始: {dq}")

    dq.rotate(2)   # 向右旋转2位
    print(f"  向右旋转2位: {dq}")

    dq.rotate(-2)  # 向左旋转2位（还原）
    print(f"  向左旋转2位（还原）: {dq}")
    print()

    # ── 固定大小的 deque ─────────────────────────────────
    print("3. 固定大小的 deque（maxlen）:")
    print("  （超出长度时，自动丢弃另一端的元素）")

    # 应用场景：保留最近 N 条日志
    recent_logs = collections.deque(maxlen=5)
    for i in range(1, 9):
        recent_logs.append(f"日志{i}")
        print(f"  添加日志{i}: {list(recent_logs)}")
    print()

    # ── 实战：滑动窗口最大值 ────────────────────────────
    print("4. 实战：滑动窗口计算移动平均")

    def moving_average(data: list[float], window: int) -> list[float]:
        """计算移动平均值。"""
        dq: collections.deque = collections.deque(maxlen=window)
        averages = []
        for val in data:
            dq.append(val)
            if len(dq) == window:  # 窗口满了才开始计算
                averages.append(sum(dq) / window)
        return averages

    prices = [10.0, 11.5, 10.8, 12.3, 11.9, 13.0, 12.5, 14.2, 13.8, 15.0]
    ma3 = moving_average(prices, 3)
    print(f"  股价数据: {prices}")
    print(f"  3日移动平均: {[round(v, 2) for v in ma3]}")


# =============================================================================
# 18.5 collections.namedtuple：具名元组
# =============================================================================

def demo_namedtuple() -> None:
    """演示 collections.namedtuple 的用法。"""
    print("\n" + "=" * 60)
    print("18.5 collections.namedtuple：具名元组")
    print("=" * 60)

    # ── 创建具名元组 ─────────────────────────────────────
    print("1. 创建具名元组（比普通元组更易读）:")

    # 定义：像定义类一样，但不可变、内存效率高
    Point = collections.namedtuple('Point', ['x', 'y'])
    Color = collections.namedtuple('Color', 'r g b')  # 也可以用空格分隔的字符串

    p = Point(3, 4)
    c = Color(255, 128, 0)

    print(f"  Point: {p}")
    print(f"  Color: {c}")

    # 既可以用名称访问，也可以用索引访问
    print(f"  p.x={p.x}, p.y={p.y}")
    print(f"  p[0]={p[0]}, p[1]={p[1]}")
    print(f"  c.r={c.r}, c.g={c.g}, c.b={c.b}")
    print()

    # ── 具名元组的方法 ───────────────────────────────────
    print("2. 具名元组的内置方法:")

    # _asdict：转为有序字典
    print(f"  p._asdict() = {p._asdict()}")

    # _replace：创建修改后的新元组（不可变，返回新对象）
    p2 = p._replace(x=10)
    print(f"  p._replace(x=10) = {p2}（原 p={p} 不变）")

    # _fields：字段名称
    print(f"  Point._fields = {Point._fields}")

    # _make：从可迭代对象创建
    coords = [5, 6]
    p3 = Point._make(coords)
    print(f"  Point._make([5, 6]) = {p3}")
    print()

    # ── 实战：坐标点计算 ────────────────────────────────
    print("3. 实战：使用具名元组表示学生成绩")

    import math
    Student = collections.namedtuple('Student', ['name', 'math', 'english', 'science'])

    students = [
        Student('张三', 85, 92, 78),
        Student('李四', 90, 78, 95),
        Student('王五', 72, 88, 85),
    ]

    print(f"  {'姓名':<6} {'数学':<6} {'英语':<6} {'理综':<6} {'平均分'}")
    print("  " + "-" * 40)
    for s in students:
        avg = (s.math + s.english + s.science) / 3
        print(f"  {s.name:<6} {s.math:<6} {s.english:<6} {s.science:<6} {avg:.1f}")

    # 按平均分排序
    ranked = sorted(students, key=lambda s: s.math + s.english + s.science, reverse=True)
    print(f"\n  排名第一: {ranked[0].name}")


# =============================================================================
# 18.6 itertools：迭代器工具
# =============================================================================

def demo_itertools() -> None:
    """演示 itertools 模块的常用函数。"""
    print("\n" + "=" * 60)
    print("18.6 itertools：迭代器工具")
    print("=" * 60)

    # ── 无限迭代器 ───────────────────────────────────────
    print("1. 无限迭代器（使用 itertools.islice 截断）:")

    # count：从 n 开始无限计数
    counter = itertools.count(start=10, step=2)
    print(f"  count(10, 2): {list(itertools.islice(counter, 6))}")

    # cycle：无限循环一个序列
    cycler = itertools.cycle(['A', 'B', 'C'])
    print(f"  cycle(['A','B','C']): {list(itertools.islice(cycler, 8))}")

    # repeat：重复一个值（n 次或无限）
    repeater = itertools.repeat('hello', 4)
    print(f"  repeat('hello', 4): {list(repeater)}")
    print()

    # ── 有限迭代器 ───────────────────────────────────────
    print("2. 组合与分组迭代器:")

    # chain：连接多个迭代器
    result = list(itertools.chain([1, 2], [3, 4], [5, 6]))
    print(f"  chain([1,2],[3,4],[5,6]): {result}")

    # chain.from_iterable：连接嵌套迭代器（扁平化一层）
    nested = [[1, 2, 3], [4, 5], [6, 7, 8, 9]]
    flat = list(itertools.chain.from_iterable(nested))
    print(f"  chain.from_iterable: {flat}")

    # zip_longest：与 zip 类似，但以最长序列为准
    a = [1, 2, 3, 4, 5]
    b = ['a', 'b', 'c']
    zipped = list(itertools.zip_longest(a, b, fillvalue=0))
    print(f"  zip_longest([1..5], ['a','b','c']): {zipped}")

    # islice：截取迭代器的一段
    data = range(20)
    sliced = list(itertools.islice(data, 5, 15, 2))  # [5:15:2]
    print(f"  islice(range(20), 5, 15, 2): {sliced}")
    print()

    # ── 组合工具 ─────────────────────────────────────────
    print("3. 组合工具（排列、组合、笛卡尔积）:")

    items = ['A', 'B', 'C']

    # product：笛卡尔积
    prod = list(itertools.product([0, 1], repeat=3))
    print(f"  product([0,1], repeat=3): {prod}")

    # permutations：排列（有序，不重复）
    perms = list(itertools.permutations(items, 2))
    print(f"  permutations({items}, 2): {perms}")

    # combinations：组合（无序，不重复）
    combs = list(itertools.combinations(items, 2))
    print(f"  combinations({items}, 2): {combs}")

    # combinations_with_replacement：允许重复
    combs_r = list(itertools.combinations_with_replacement(['A', 'B'], 2))
    print(f"  combinations_with_replacement(['A','B'], 2): {combs_r}")
    print()

    # ── 分组工具 ─────────────────────────────────────────
    print("4. 分组工具（groupby）:")
    print("  ⚠️  groupby 要求数据必须先按分组键排序！")

    data = [
        {'name': '张三', 'dept': '研发'},
        {'name': '李四', 'dept': '研发'},
        {'name': '王五', 'dept': '市场'},
        {'name': '赵六', 'dept': '市场'},
        {'name': '孙七', 'dept': '运营'},
    ]

    # 先排序再分组
    sorted_data = sorted(data, key=lambda x: x['dept'])
    for dept, members in itertools.groupby(sorted_data, key=lambda x: x['dept']):
        names = [m['name'] for m in members]
        print(f"  {dept}: {names}")
    print()

    # ── 过滤工具 ─────────────────────────────────────────
    print("5. 过滤工具（compress、filterfalse、takewhile、dropwhile）:")

    data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

    # compress：按 bool 序列过滤
    selectors = [1, 0, 1, 0, 1, 0, 1, 0, 1, 0]
    print(f"  compress: {list(itertools.compress(data, selectors))}")

    # takewhile：取满足条件的前缀
    print(f"  takewhile(<5): {list(itertools.takewhile(lambda x: x < 5, data))}")

    # dropwhile：丢弃满足条件的前缀，取后续所有
    print(f"  dropwhile(<5): {list(itertools.dropwhile(lambda x: x < 5, data))}")

    # filterfalse：取不满足条件的元素（与 filter 相反）
    evens = list(itertools.filterfalse(lambda x: x % 2, data))  # 过滤掉奇数
    print(f"  filterfalse(奇数): {evens}")


# =============================================================================
# 18.7 functools：函数工具
# =============================================================================

def demo_functools() -> None:
    """演示 functools 模块的常用函数。"""
    print("\n" + "=" * 60)
    print("18.7 functools：函数工具")
    print("=" * 60)

    # ── functools.reduce ─────────────────────────────────
    print("1. functools.reduce（累积归约）:")
    print("  相当于 C++ 中的 std::accumulate")

    numbers = [1, 2, 3, 4, 5]

    # 求累积乘积：((((1*2)*3)*4)*5) = 120
    product = functools.reduce(lambda x, y: x * y, numbers)
    print(f"  reduce(*, {numbers}) = {product}")

    # 求最大值
    max_val = functools.reduce(lambda x, y: x if x > y else y, numbers)
    print(f"  reduce(max, {numbers}) = {max_val}")

    # 带初始值
    total = functools.reduce(lambda x, y: x + y, numbers, 100)
    print(f"  reduce(+, {numbers}, 初始值=100) = {total}")

    # 使用 operator 模块代替 lambda
    import operator
    product2 = functools.reduce(operator.mul, numbers)
    print(f"  reduce(operator.mul, {numbers}) = {product2}")
    print()

    # ── functools.partial：偏函数 ────────────────────────
    print("2. functools.partial：偏函数（固定部分参数）:")
    print("  相当于 C++ 中的 std::bind")

    def power(base: float, exponent: float) -> float:
        """计算 base 的 exponent 次方。"""
        return base ** exponent

    # 固定 exponent=2，创建平方函数
    square = functools.partial(power, exponent=2)
    # 固定 exponent=3，创建立方函数
    cube = functools.partial(power, exponent=3)
    # 固定 base=2，创建2的n次方
    power_of_2 = functools.partial(power, 2)

    print(f"  square(5) = {square(5)}")
    print(f"  cube(3) = {cube(3)}")
    print(f"  power_of_2(10) = {power_of_2(10)}")

    # 实际应用：简化重复的函数调用
    import json
    # 创建默认输出 JSON 的函数（中文不转义，带缩进）
    dump_json = functools.partial(json.dumps, ensure_ascii=False, indent=2)
    data = {'name': '张三', 'age': 25}
    print(f"\n  dump_json({data}) =")
    print(f"  {dump_json(data)}")
    print()

    # ── functools.lru_cache：缓存装饰器 ─────────────────
    print("3. functools.lru_cache：函数结果缓存（记忆化）:")

    import time

    # 没有缓存的斐波那契（指数级递归）
    # 使用类来避免 global 变量问题
    class FibCounter:
        def __init__(self):
            self.count = 0

        def fib_no_cache(self, n: int) -> int:
            """无缓存的斐波那契（非常慢）。"""
            self.count += 1
            if n <= 1:
                return n
            return self.fib_no_cache(n - 1) + self.fib_no_cache(n - 2)

    counter = FibCounter()
    start = time.perf_counter()
    result = counter.fib_no_cache(30)
    t1 = time.perf_counter() - start
    calls1 = counter.count
    print(f"  无缓存 fib(30) = {result}，调用次数={calls1:,}，耗时={t1*1000:.1f}ms")

    # 有缓存的斐波那契（线性）
    @functools.lru_cache(maxsize=None)  # maxsize=None 表示无限缓存
    def fib_cached(n: int) -> int:
        """有缓存的斐波那契（非常快）。"""
        if n <= 1:
            return n
        return fib_cached(n - 1) + fib_cached(n - 2)

    start = time.perf_counter()
    result = fib_cached(30)
    t2 = time.perf_counter() - start
    print(f"  有缓存 fib(30) = {result}，缓存信息={fib_cached.cache_info()}，耗时={t2*1000:.4f}ms")
    print(f"  速度提升约 {t1/max(t2, 0.000001):.0f} 倍！")
    print()

    # ── functools.wraps ──────────────────────────────────
    print("4. functools.wraps：保留被装饰函数的元信息:")

    def bad_decorator(func):
        """不保留元信息的装饰器（反例）。"""
        def wrapper(*args, **kwargs):
            return func(*args, **kwargs)
        return wrapper

    def good_decorator(func):
        """正确保留元信息的装饰器。"""
        @functools.wraps(func)   # 关键：保留 __name__, __doc__ 等
        def wrapper(*args, **kwargs):
            return func(*args, **kwargs)
        return wrapper

    @bad_decorator
    def my_func_bad():
        """这是一个函数的文档字符串。"""
        pass

    @good_decorator
    def my_func_good():
        """这是一个函数的文档字符串。"""
        pass

    print(f"  bad_decorator  → __name__='{my_func_bad.__name__}',"
          f" __doc__='{my_func_bad.__doc__}'")
    print(f"  good_decorator → __name__='{my_func_good.__name__}',"
          f" __doc__='{my_func_good.__doc__}'")
    print()

    # ── functools.total_ordering ─────────────────────────
    print("5. functools.total_ordering：自动补全比较方法:")
    print("  只需定义 __eq__ 和一个比较方法，其余自动推导")

    @functools.total_ordering
    class Version:
        """版本号比较（如 1.2.3 < 1.3.0）。"""

        def __init__(self, version_str: str) -> None:
            self.parts = tuple(int(x) for x in version_str.split('.'))
            self.version_str = version_str

        def __eq__(self, other: object) -> bool:
            if not isinstance(other, Version):
                return NotImplemented
            return self.parts == other.parts

        def __lt__(self, other: 'Version') -> bool:
            return self.parts < other.parts

        def __repr__(self) -> str:
            return f"Version('{self.version_str}')"

    v1 = Version('1.2.3')
    v2 = Version('1.3.0')
    v3 = Version('1.2.3')

    print(f"  v1={v1}, v2={v2}, v3={v3}")
    print(f"  v1 < v2: {v1 < v2}")
    print(f"  v1 > v2: {v1 > v2}")   # 自动推导
    print(f"  v1 <= v3: {v1 <= v3}")  # 自动推导
    print(f"  v1 >= v2: {v1 >= v2}")  # 自动推导
    print(f"  v1 == v3: {v1 == v3}")
    versions = [v2, v1, Version('2.0.0'), v3]
    print(f"  排序: {sorted(versions)}")


# =============================================================================
# 18.8 综合实战：学生成绩分析系统
# =============================================================================

def demo_grade_analysis() -> None:
    """综合实战：使用 collections + itertools + functools 分析成绩。"""
    print("\n" + "=" * 60)
    print("18.8 综合实战：学生成绩分析系统")
    print("=" * 60)

    # 模拟成绩数据
    records = [
        {'name': '张三', 'class': 'A班', 'subject': '数学', 'score': 85},
        {'name': '张三', 'class': 'A班', 'subject': '英语', 'score': 92},
        {'name': '张三', 'class': 'A班', 'subject': '物理', 'score': 78},
        {'name': '李四', 'class': 'A班', 'subject': '数学', 'score': 90},
        {'name': '李四', 'class': 'A班', 'subject': '英语', 'score': 75},
        {'name': '李四', 'class': 'A班', 'subject': '物理', 'score': 95},
        {'name': '王五', 'class': 'B班', 'subject': '数学', 'score': 70},
        {'name': '王五', 'class': 'B班', 'subject': '英语', 'score': 85},
        {'name': '王五', 'class': 'B班', 'subject': '物理', 'score': 80},
        {'name': '赵六', 'class': 'B班', 'subject': '数学', 'score': 95},
        {'name': '赵六', 'class': 'B班', 'subject': '英语', 'score': 88},
        {'name': '赵六', 'class': 'B班', 'subject': '物理', 'score': 92},
    ]

    # ── 按学生分组统计 ───────────────────────────────────
    print("1. 按学生统计总分和平均分:")
    student_scores = collections.defaultdict(list)
    for r in records:
        student_scores[r['name']].append(r['score'])

    student_stats = []
    for name, scores in student_scores.items():
        total = functools.reduce(operator.add, scores)
        avg = total / len(scores)
        student_stats.append({'name': name, 'total': total, 'avg': avg})

    # 按总分降序排序
    student_stats.sort(key=lambda x: x['total'], reverse=True)

    print(f"  {'排名':<5} {'姓名':<8} {'总分':<8} {'平均分'}")
    print("  " + "-" * 35)
    for i, s in enumerate(student_stats, 1):
        print(f"  {i:<5} {s['name']:<8} {s['total']:<8} {s['avg']:.1f}")
    print()

    # ── 按科目统计 ───────────────────────────────────────
    print("2. 按科目统计平均分:")
    subject_scores = collections.defaultdict(list)
    for r in records:
        subject_scores[r['subject']].append(r['score'])

    for subject in sorted(subject_scores):
        scores = subject_scores[subject]
        avg = sum(scores) / len(scores)
        max_s = max(scores)
        min_s = min(scores)
        print(f"  {subject}: 平均={avg:.1f}, 最高={max_s}, 最低={min_s}")
    print()

    # ── 分数段统计（Counter + defaultdict）───────────────
    print("3. 分数段分布（所有成绩）:")
    all_scores = [r['score'] for r in records]

    def grade_level(score: int) -> str:
        """将分数转为等级。"""
        if score >= 90:
            return 'A（优秀）'
        elif score >= 80:
            return 'B（良好）'
        elif score >= 70:
            return 'C（中等）'
        else:
            return 'D（需努力）'

    grade_counter = collections.Counter(grade_level(s) for s in all_scores)
    for grade in ['A（优秀）', 'B（良好）', 'C（中等）', 'D（需努力）']:
        count = grade_counter.get(grade, 0)
        bar = '█' * count
        print(f"  {grade}: {bar} ({count}人)")
    print()

    # ── 使用 itertools 生成所有可能的学生配对 ────────────
    print("4. 使用 itertools 生成学习伙伴配对:")
    student_names = list(student_scores.keys())
    pairs = list(itertools.combinations(student_names, 2))
    print(f"  {len(student_names)} 名学生可以组成 {len(pairs)} 种配对:")
    for s1, s2 in pairs:
        print(f"    {s1} ↔ {s2}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """主程序：运行所有演示。"""
    demo_counter()
    demo_defaultdict()
    demo_ordereddict()
    demo_deque()
    demo_namedtuple()
    demo_itertools()
    demo_functools()
    demo_grade_analysis()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── collections 速查 ──
# Counter(iterable)           计数器
# counter.most_common(n)      最常见的 n 个
# counter.elements()          按计数展开所有元素
# defaultdict(factory)        带默认值的字典
# OrderedDict()               有序字典
# od.move_to_end(key)         移动键到末尾
# deque(iterable, maxlen=N)   双端队列
# dq.appendleft(x)            左端添加
# dq.popleft()                左端弹出
# dq.rotate(n)                旋转
# namedtuple('Name', fields)  具名元组
# nt._asdict()                转为字典
# nt._replace(field=val)      创建修改副本
#
# ── itertools 速查 ──
# count(start, step)          无限计数
# cycle(iterable)             无限循环
# repeat(val, n)              重复 n 次
# chain(*iterables)           连接多个迭代器
# islice(iterable, n)         截取前 n 个
# product(*iterables)         笛卡尔积
# permutations(it, r)         r个元素的排列
# combinations(it, r)         r个元素的组合
# groupby(iterable, key)      按 key 分组
# takewhile(pred, it)         取前缀满足条件的元素
# dropwhile(pred, it)         丢弃前缀满足条件的元素
# zip_longest(*its, fillvalue) 以最长为准的 zip
#
# ── functools 速查 ──
# reduce(func, iterable)      累积归约
# partial(func, *args)        偏函数（固定部分参数）
# lru_cache(maxsize=128)      LRU 缓存装饰器
# cache                       无限 LRU 缓存（Python 3.9+）
# wraps(func)                 保留函数元信息
# total_ordering              自动补全比较方法


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：groupby 前没有排序
# data = [{'dept': 'B'}, {'dept': 'A'}, {'dept': 'B'}]
# for dept, group in itertools.groupby(data, key=lambda x: x['dept']):
#     ...  # ❌ 结果: B组, A组, B组（B被分成了两组！）
# 解决：sorted(data, key=...) 后再 groupby
#
# 错误 2：Counter 相减可能得到负数
# c = Counter(a=1) - Counter(a=5)
# print(c)  # Counter() ← 负数被丢弃，不是 Counter({'a': -4})
#
# 错误 3：lru_cache 用于可变参数（列表、字典）会报错
# @lru_cache
# def f(lst: list): ...     # ❌ list 不可哈希
# 解决：改用 tuple 或 frozenset 传入
#
# 错误 4：partial 不能和某些内置函数一起用
# p = functools.partial(print, end='')  # ✓ 可以
# 但注意：partial 的位置参数是前置固定的
#
# 错误 5：deque 随机访问慢（O(n)）
# dq = deque([1, 2, 3])
# dq[1000]  # 需要遍历到第 1000 位，比 list 慢
# 解决：需要随机访问时用 list，需要快速两端插入时用 deque


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   实现函数 count_top_chars(text, n)：
#   接受一段中文文本和整数 n，返回出现最多的 n 个汉字及其次数
#   返回格式：[(字符, 次数), ...]
#   例如：
#     text = "中国人民站起来了，中国人民万岁！中华人民共和国万岁！"
#     count_top_chars(text, 3)
#     # [('人', 4), ('中', 3), ('国', 3)]
#
# 练习 2（进阶）：
#   实现函数 group_by_grade(students)：
#   接受 (学生名, 分数) 元组列表，按分数段分组返回字典
#   分数段：90+ → '优秀', 80-89 → '良好', 70-79 → '中等', <70 → '需努力'
#   返回格式：{'优秀': [学生名列表], '良好': [...], ...}
#   例如：
#     students = [('张三', 95), ('李四', 82), ('王五', 68), ('赵六', 90)]
#     group_by_grade(students)
#     # {'优秀': ['张三', '赵六'], '良好': ['李四'], '中等': [], '需努力': ['王五']}
#
# 练习 3（综合）：
#   实现函数 generate_poker_deck()：
#   生成一副完整的扑克牌（不含大小王），返回 (花色, 点数) 元组列表
#   花色：['♠', '♥', '♦', '♣']，点数：['A', '2', ..., '10', 'J', 'Q', 'K']
#   使用 itertools.product 实现
#   例如：
#     deck = generate_poker_deck()
#     len(deck)  # 52
#     deck[:3]   # [('♠', 'A'), ('♠', '2'), ('♠', '3')]
#
# 练习 4（高级）：
#   实现装饰器 memoize（不使用 functools.lru_cache）：
#   用字典缓存函数调用结果，支持任意位置参数
#   要求：使用 functools.wraps 保留函数元信息
#   测试：装饰斐波那契函数并验证缓存效果
#   例如：
#     @memoize
#     def fib(n): ...
#     fib(35)  # 第一次调用较慢
#     fib(35)  # 第二次调用从缓存读取，极快
#
# 练习答案提示：
#   练习1：用 Counter(text) 统计所有字符；用 most_common(n) 获取前 n 个；
#          可用正则过滤只保留汉字：re.findall(r'[\u4e00-\u9fff]', text)
#   练习2：用 defaultdict(list) 创建分组字典；遍历学生列表，根据分数判断等级并添加到对应列表
#   练习3：suits = ['♠', '♥', '♦', '♣']，ranks = ['A', '2', ..., 'K']；
#          return list(itertools.product(suits, ranks))
#   练习4：创建 cache = {} 字典；在 wrapper 中检查 args 是否在 cache 中；
#          不在则调用原函数并存入 cache；使用 @functools.wraps(func) 装饰 wrapper


# =============================================================================
# 【练习答案】
# =============================================================================


def count_top_chars(text: str, n: int) -> list[tuple[str, int]]:
    """
    练习 1：统计文本中出现最多的 n 个汉字。

    Args:
        text: 输入文本
        n: 返回前 n 个最常见的字符

    Returns:
        [(字符, 次数), ...] 列表，按出现次数降序排列

    Example:
        text = "中国人民站起来了，中国人民万岁！中华人民共和国万岁！"
        count_top_chars(text, 3)
        # [('人', 4), ('中', 3), ('国', 3)]
    """
    # 只保留汉字
    import re
    chinese_chars = re.findall(r'[\u4e00-\u9fff]', text)
    counter = collections.Counter(chinese_chars)
    return counter.most_common(n)


def group_by_grade(students: list[tuple[str, int]]) -> dict[str, list[str]]:
    """
    练习 2：按分数段分组学生。

    Args:
        students: (学生名, 分数) 元组列表

    Returns:
        字典，键为等级，值为学生名列表
        {'优秀': [...], '良好': [...], '中等': [...], '需努力': [...]}

    Example:
        students = [('张三', 95), ('李四', 82), ('王五', 68), ('赵六', 90)]
        group_by_grade(students)
        # {'优秀': ['张三', '赵六'], '良好': ['李四'], '中等': [], '需努力': ['王五']}
    """
    groups = collections.defaultdict(list)

    for name, score in students:
        if score >= 90:
            grade = '优秀'
        elif score >= 80:
            grade = '良好'
        elif score >= 70:
            grade = '中等'
        else:
            grade = '需努力'
        groups[grade].append(name)

    # 确保所有等级都存在（即使为空列表）
    for grade in ['优秀', '良好', '中等', '需努力']:
        if grade not in groups:
            groups[grade] = []

    return dict(groups)


def generate_poker_deck() -> list[tuple[str, str]]:
    """
    练习 3：生成一副完整的扑克牌（不含大小王）。

    Returns:
        (花色, 点数) 元组列表，共 52 张牌

    Example:
        deck = generate_poker_deck()
        len(deck)  # 52
        deck[:3]   # [('♠', 'A'), ('♠', '2'), ('♠', '3')]
    """
    suits = ['♠', '♥', '♦', '♣']
    ranks = ['A', '2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K']
    return list(itertools.product(suits, ranks))


def memoize(func):
    """
    练习 4：自定义记忆化装饰器（缓存函数调用结果）。

    Args:
        func: 要装饰的函数

    Returns:
        装饰后的函数，带有缓存功能

    Example:
        @memoize
        def fib(n):
            if n <= 1:
                return n
            return fib(n - 1) + fib(n - 2)

        fib(35)  # 第一次调用较慢
        fib(35)  # 第二次调用从缓存读取，极快
    """
    cache: dict = {}

    @functools.wraps(func)
    def wrapper(*args):
        if args not in cache:
            cache[args] = func(*args)
        return cache[args]

    # 暴露缓存供外部查看和清理
    wrapper.cache = cache
    wrapper.cache_clear = lambda: cache.clear()
    return wrapper


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 count_top_chars 函数。"""
    print("count_top_chars 演示:")

    # ── 中文文本统计 ─────────────────────────────────────
    text = "中国人民站起来了，中国人民万岁！中华人民共和国万岁！"
    print(f"  文本: {text}")

    for n in [3, 5]:
        result = count_top_chars(text, n)
        print(f"  出现最多的 {n} 个汉字: {result}")

    # ── 较长文本 ─────────────────────────────────────────
    long_text = """
    Python 是一种解释型高级编程语言，以简洁优雅著称。
    Python 广泛用于数据科学、机器学习、Web 开发等领域。
    学习 Python 让编程变得有趣而简单，Python 社区非常活跃。
    """
    print(f"\n  较长文本中出现最多的 5 个汉字:")
    for char, count in count_top_chars(long_text, 5):
        bar = '█' * count
        print(f"    '{char}' {bar} ({count}次)")


def exercise2_answer() -> None:
    """练习 2：演示 group_by_grade 函数。"""
    print("group_by_grade 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    students = [
        ('张三', 95),
        ('李四', 82),
        ('王五', 68),
        ('赵六', 90),
        ('孙七', 75),
        ('周八', 55),
        ('吴九', 88),
    ]

    print(f"  输入学生数据: {students}")
    result = group_by_grade(students)
    print("  分组结果:")
    for grade in ['优秀', '良好', '中等', '需努力']:
        names = result.get(grade, [])
        print(f"    {grade}: {names}")

    # ── 边界分数验证 ──────────────────────────────────────
    boundary_students = [
        ('边界90', 90),
        ('边界80', 80),
        ('边界70', 70),
        ('边界69', 69),
    ]
    print(f"\n  边界分数验证:")
    boundary_result = group_by_grade(boundary_students)
    for grade in ['优秀', '良好', '中等', '需努力']:
        names = boundary_result.get(grade, [])
        if names:
            print(f"    {grade}: {names}")


def exercise3_answer() -> None:
    """练习 3：演示 generate_poker_deck 函数。"""
    print("generate_poker_deck 演示:")

    # ── 生成扑克牌 ───────────────────────────────────────
    deck = generate_poker_deck()
    print(f"  总共 {len(deck)} 张牌")
    print(f"  前5张: {deck[:5]}")
    print(f"  后5张: {deck[-5:]}")

    # ── 按花色展示 ───────────────────────────────────────
    print(f"\n  按花色分组展示:")
    suits = ['♠', '♥', '♦', '♣']
    suit_names = {'♠': '黑桃', '♥': '红心', '♦': '方块', '♣': '梅花'}
    for suit in suits:
        cards = [rank for s, rank in deck if s == suit]
        print(f"    {suit_names[suit]}({suit}): {' '.join(cards)}")


def exercise4_answer() -> None:
    """练习 4：演示 memoize 装饰器。"""
    print("memoize 演示:")

    import time

    # ── 装饰斐波那契函数 ──────────────────────────────────
    @memoize
    def fib(n: int) -> int:
        """斐波那契数（使用自定义 memoize）。"""
        if n <= 1:
            return n
        return fib(n - 1) + fib(n - 2)

    # 第一次调用（计算并缓存）
    start = time.perf_counter()
    result = fib(35)
    t1 = time.perf_counter() - start
    print(f"  fib(35) = {result}")
    print(f"  第一次调用耗时: {t1*1000:.4f} ms，缓存条数: {len(fib.cache)}")

    # 第二次调用（从缓存读取）
    start = time.perf_counter()
    result2 = fib(35)
    t2 = time.perf_counter() - start
    print(f"  第二次调用耗时: {t2*1000:.6f} ms（从缓存读取，极快）")

    # ── 验证元信息保留 ────────────────────────────────────
    print(f"\n  装饰后函数名: {fib.__name__}")
    print(f"  装饰后文档: {fib.__doc__}")

    # ── 缓存清理 ─────────────────────────────────────────
    fib.cache_clear()
    print(f"  清理缓存后，缓存条数: {len(fib.cache)}")


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
