# =============================================================================
# 第 5 章：容器数据类型（列表、元组、字典、集合）
# =============================================================================
#
# 【学习目标】
#   1. 掌握列表（list）的创建、操作和推导式
#   2. 理解元组（tuple）的特点和用途
#   3. 掌握字典（dict）的增删改查和推导式
#   4. 掌握集合（set）的操作和集合运算
#   5. 理解各容器的适用场景
#
# 【运行方式】
#   python chapter05_容器数据类型.py
#
# =============================================================================


# =============================================================================
# 5.1 列表（list）
# =============================================================================
#
# 对应 C++：std::vector<T>（但 Python 列表可存放不同类型）
# 特点：有序、可变、允许重复

def demo_list() -> None:
    """演示列表的创建和操作。"""
    print("=" * 60)
    print("5.1 列表（list）")
    print("=" * 60)

    # ── 创建列表 ─────────────────────────────────────────
    print("创建列表:")
    empty = []                            # 空列表
    numbers = [1, 2, 3, 4, 5]            # 整数列表
    mixed = [1, "hello", 3.14, True]      # 混合类型
    nested = [[1, 2], [3, 4], [5, 6]]    # 嵌套列表（二维数组）
    repeated = [0] * 5                    # 重复元素：[0, 0, 0, 0, 0]
    from_range = list(range(1, 6))        # 从 range 创建

    print(f"  空列表:   {empty}")
    print(f"  数字列表: {numbers}")
    print(f"  混合类型: {mixed}")
    print(f"  嵌套列表: {nested}")
    print(f"  重复元素: {repeated}")
    print(f"  from_range: {from_range}")

    # ── 索引和切片（与字符串相同）──────────────────────
    print("\n索引和切片:")
    lst = [10, 20, 30, 40, 50]
    print(f"  lst = {lst}")
    print(f"  lst[0]   = {lst[0]}")       # 10
    print(f"  lst[-1]  = {lst[-1]}")      # 50
    print(f"  lst[1:3] = {lst[1:3]}")     # [20, 30]
    print(f"  lst[::-1]= {lst[::-1]}")    # 反转

    # ── 修改列表 ─────────────────────────────────────────
    print("\n修改列表（列表是可变的）:")
    lst = [1, 2, 3, 4, 5]
    lst[0] = 10                    # 修改单个元素
    print(f"  修改 lst[0]=10: {lst}")

    lst[1:3] = [20, 30]            # 切片赋值
    print(f"  切片赋值[1:3]: {lst}")

    # ── 常用方法 ─────────────────────────────────────────
    print("\n列表方法:")
    lst = [3, 1, 4, 1, 5, 9, 2, 6]
    print(f"  原始: {lst}")

    lst.append(7)           # 末尾添加元素
    print(f"  append(7): {lst}")

    lst.insert(0, 0)        # 在指定位置插入
    print(f"  insert(0,0): {lst}")

    lst.extend([8, 9])      # 末尾添加多个元素（合并另一个列表）
    print(f"  extend([8,9]): {lst}")

    popped = lst.pop()      # 删除并返回最后一个元素
    print(f"  pop(): 弹出 {popped}, 剩余 {lst}")

    popped = lst.pop(0)     # 删除并返回指定位置的元素
    print(f"  pop(0): 弹出 {popped}, 剩余 {lst}")

    lst.remove(1)           # 删除第一个匹配的元素
    print(f"  remove(1): {lst}")

    # 查找
    lst2 = [1, 2, 3, 2, 1]
    print(f"\n  {lst2}.index(2)  = {lst2.index(2)}")    # 第一个2的索引
    print(f"  {lst2}.count(1)  = {lst2.count(1)}")      # 1出现的次数
    print(f"  2 in {lst2}      = {2 in lst2}")           # 成员检测

    # 排序
    nums = [3, 1, 4, 1, 5, 9, 2, 6]
    nums.sort()                             # 原地排序（升序）
    print(f"\n  sort()升序: {nums}")
    nums.sort(reverse=True)                 # 原地排序（降序）
    print(f"  sort()降序: {nums}")

    # sorted() 返回新列表，不修改原列表
    original = [3, 1, 4, 1, 5, 9]
    sorted_list = sorted(original)
    print(f"\n  原列表: {original}（不变）")
    print(f"  sorted(): {sorted_list}")

    # 按自定义规则排序
    words = ["banana", "Apple", "cherry", "date"]
    words.sort(key=str.lower)              # 忽略大小写排序
    print(f"  按字母忽略大小写: {words}")

    students = [("Alice", 90), ("Bob", 85), ("Charlie", 92)]
    students.sort(key=lambda x: x[1], reverse=True)  # 按分数降序
    print(f"  按分数排序: {students}")

    lst3 = [1, 2, 3]
    lst3.reverse()          # 原地反转
    print(f"\n  reverse(): {lst3}")
    lst3.clear()            # 清空列表
    print(f"  clear(): {lst3}")

    # ── 列表推导式 ───────────────────────────────────────
    print("\n列表推导式（List Comprehension）:")
    # 基本形式：[表达式 for 变量 in 可迭代对象]
    squares = [x ** 2 for x in range(1, 6)]
    print(f"  平方数: {squares}")

    # 带条件的推导式：[表达式 for 变量 in 可迭代 if 条件]
    evens = [x for x in range(20) if x % 2 == 0]
    print(f"  偶数: {evens}")

    # 嵌套推导式（生成矩阵）
    matrix = [[i * j for j in range(1, 4)] for i in range(1, 4)]
    print(f"  3×3矩阵: {matrix}")

    # 展平二维列表
    nested = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
    flat = [x for row in nested for x in row]
    print(f"  展平列表: {flat}")

    # ── 常用内置函数 ─────────────────────────────────────
    nums = [3, 1, 4, 1, 5, 9, 2, 6, 5, 3]
    print(f"\n内置函数（列表={nums}）:")
    print(f"  len()  = {len(nums)}")
    print(f"  sum()  = {sum(nums)}")
    print(f"  min()  = {min(nums)}")
    print(f"  max()  = {max(nums)}")
    print(f"  sorted() = {sorted(nums)}")

    # all() 和 any()
    print(f"  all(x>0): {all(x > 0 for x in nums)}")   # 所有元素都>0?
    print(f"  any(x>8): {any(x > 8 for x in nums)}")   # 有元素>8?

    # ── 列表复制（浅拷贝与深拷贝）───────────────────────
    print("\n列表复制注意事项:")
    a = [1, 2, 3]
    b = a               # 不是复制！b 和 a 指向同一对象
    b.append(4)
    print(f"  a = b 后修改 b: a={a}")  # a 也变了！

    a = [1, 2, 3]
    c = a.copy()        # 浅拷贝（方式1）
    d = a[:]            # 浅拷贝（方式2）
    e = list(a)         # 浅拷贝（方式3）
    c.append(4)
    print(f"  浅拷贝后修改: a={a}, c={c}")  # a 不变

    # 注意：浅拷贝对嵌套列表无效
    import copy
    nested = [[1, 2], [3, 4]]
    shallow = nested.copy()
    deep = copy.deepcopy(nested)    # 深拷贝

    shallow[0].append(99)
    print(f"\n  嵌套列表浅拷贝: nested={nested}, shallow={shallow}")  # 都变了

    deep[0].append(88)
    print(f"  嵌套列表深拷贝: nested={nested}, deep={deep}")  # nested 不变


# =============================================================================
# 5.2 元组（tuple）
# =============================================================================
#
# 元组与列表相似，但是不可变的
# 适合存储不应该被修改的数据（如坐标、RGB颜色、函数多返回值）

def demo_tuple() -> None:
    """演示元组的用法。"""
    print("\n" + "=" * 60)
    print("5.2 元组（tuple）")
    print("=" * 60)

    # ── 创建元组 ─────────────────────────────────────────
    empty = ()                          # 空元组
    single = (42,)                      # 单元素元组（注意必须有逗号！）
    point = (3, 4)                      # 二维坐标
    rgb = (255, 128, 0)                 # RGB 颜色
    mixed = (1, "hello", 3.14)         # 混合类型
    without_parens = 1, 2, 3            # 括号可省略

    print(f"空元组: {empty}")
    print(f"单元素: {single}, 类型: {type(single)}")
    print(f"坐标:   {point}")
    print(f"RGB:    {rgb}")
    print(f"无括号: {without_parens}")

    # 常见错误：单元素元组忘记逗号
    not_tuple = (42)    # 这是整数，不是元组！
    print(f"\n(42) 的类型: {type(not_tuple)}")   # <class 'int'>
    print(f"(42,) 的类型: {type(single)}")        # <class 'tuple'>

    # ── 元组操作 ─────────────────────────────────────────
    print("\n元组操作:")
    t = (10, 20, 30, 40, 50)
    print(f"  t = {t}")
    print(f"  t[0]   = {t[0]}")
    print(f"  t[-1]  = {t[-1]}")
    print(f"  t[1:3] = {t[1:3]}")
    print(f"  len(t) = {len(t)}")
    print(f"  30 in t = {30 in t}")
    print(f"  t.count(20) = {t.count(20)}")
    print(f"  t.index(30) = {t.index(30)}")

    # 元组不可修改
    # t[0] = 100  ❌ TypeError: 'tuple' object does not support item assignment

    # 但如果元组包含可变对象（如列表），可变对象内部可以修改
    t2 = ([1, 2], [3, 4])
    t2[0].append(99)   # 可以！修改的是列表，不是元组结构
    print(f"\n  含列表的元组: {t2}")

    # ── 元组拆包（解包）─────────────────────────────────
    print("\n元组拆包:")
    point = (3, 4)
    x, y = point            # 解包赋值
    print(f"  x={x}, y={y}")

    # 用于函数多返回值
    def min_max(lst: list) -> tuple[int, int]:
        """返回列表的最小值和最大值。"""
        return min(lst), max(lst)   # 返回元组

    nums = [3, 1, 4, 1, 5, 9, 2, 6]
    minimum, maximum = min_max(nums)   # 解包接收
    print(f"  最小值={minimum}, 最大值={maximum}")

    # 星号解包
    first, *rest = (1, 2, 3, 4, 5)
    print(f"  first={first}, rest={rest}")

    # ── 元组 vs 列表 的选择 ─────────────────────────────
    print("\n元组 vs 列表:")
    print("  元组（不可变）适合: 坐标、颜色、配置、函数多返回值、字典的键")
    print("  列表（可变）适合: 需要频繁增删改的数据集合")

    # 元组可以作为字典的键（列表不行）
    locations = {
        (39.9, 116.4): "北京",
        (31.2, 121.5): "上海",
    }
    print(f"  坐标字典: {locations[(39.9, 116.4)]}")

    # 元组通常比列表略快（因为不可变）
    import sys
    lst = [1, 2, 3, 4, 5]
    tpl = (1, 2, 3, 4, 5)
    print(f"\n  列表内存: {sys.getsizeof(lst)} 字节")
    print(f"  元组内存: {sys.getsizeof(tpl)} 字节")


# =============================================================================
# 5.3 字典（dict）
# =============================================================================
#
# 对应 C++：std::unordered_map<K, V>
# 特点：键值对、键唯一、Python 3.7+ 保持插入顺序

def demo_dict() -> None:
    """演示字典的创建和操作。"""
    print("\n" + "=" * 60)
    print("5.3 字典（dict）")
    print("=" * 60)

    # ── 创建字典 ─────────────────────────────────────────
    empty = {}                                  # 空字典
    student = {"name": "Alice", "age": 20, "grade": "A"}
    from_pairs = dict([("a", 1), ("b", 2)])     # 从键值对列表创建
    from_keys = dict.fromkeys(["x", "y", "z"], 0)  # 所有键初始化为0
    using_dict = dict(name="Bob", age=25)        # 关键字参数创建

    print(f"空字典:   {empty}")
    print(f"学生信息: {student}")
    print(f"从键值对: {from_pairs}")
    print(f"fromkeys: {from_keys}")
    print(f"关键字参: {using_dict}")

    # ── 访问和修改 ───────────────────────────────────────
    print("\n访问和修改:")
    d = {"name": "Alice", "age": 20, "city": "北京"}

    # 访问
    print(f"  d['name'] = {d['name']}")

    # 安全访问：get() 方法，键不存在时不报 KeyError
    print(f"  d.get('age')   = {d.get('age')}")          # 20
    print(f"  d.get('email') = {d.get('email')}")        # None
    print(f"  d.get('email','未知') = {d.get('email','未知')}")  # '未知'

    # 修改
    d["age"] = 21           # 修改已有键
    d["email"] = "a@b.com"  # 添加新键
    print(f"  修改后: {d}")

    # setdefault：键不存在才设置（已存在则返回现有值）
    d.setdefault("score", 95)   # score 不存在，设置为 95
    d.setdefault("age", 100)    # age 已存在，不修改
    print(f"  setdefault: {d}")

    # update：批量更新
    d.update({"city": "上海", "phone": "123"})
    print(f"  update: {d}")

    # ── 删除 ─────────────────────────────────────────────
    print("\n删除操作:")
    d = {"a": 1, "b": 2, "c": 3, "d": 4}
    val = d.pop("b")            # 删除并返回值
    print(f"  pop('b'): 值={val}, 剩余={d}")

    val = d.pop("z", -1)        # 键不存在时返回默认值
    print(f"  pop('z',-1): {val}")

    key, val = d.popitem()      # 删除并返回最后插入的键值对
    print(f"  popitem(): key={key}, val={val}, 剩余={d}")

    del d["a"]                  # 删除指定键
    print(f"  del d['a']: {d}")

    d.clear()                   # 清空字典
    print(f"  clear(): {d}")

    # ── 遍历 ─────────────────────────────────────────────
    print("\n遍历字典:")
    scores = {"Alice": 95, "Bob": 87, "Charlie": 92, "David": 78}

    # 遍历键（默认）
    print("  键:", list(scores.keys()))

    # 遍历值
    print("  值:", list(scores.values()))

    # 遍历键值对（最常用）
    print("  键值对:")
    for name, score in scores.items():
        print(f"    {name}: {score}")

    # ── 字典推导式 ───────────────────────────────────────
    print("\n字典推导式:")
    # 基本形式：{key表达式: value表达式 for 变量 in 可迭代}
    squares = {x: x**2 for x in range(1, 6)}
    print(f"  平方数: {squares}")

    # 带条件的推导式
    high_scores = {name: score for name, score in scores.items() if score >= 90}
    print(f"  高分学生: {high_scores}")

    # 反转键值（将值变为键，键变为值）
    reversed_scores = {score: name for name, score in scores.items()}
    print(f"  反转: {reversed_scores}")

    # ── 字典合并（Python 3.9+）──────────────────────────
    print("\n字典合并:")
    d1 = {"a": 1, "b": 2}
    d2 = {"b": 20, "c": 3}

    # 方式1：update（修改原字典）
    merged = d1.copy()
    merged.update(d2)
    print(f"  update合并: {merged}")

    # 方式2：{**d1, **d2}（解包合并，Python 3.5+）
    merged2 = {**d1, **d2}
    print(f"  解包合并: {merged2}")

    # 方式3：| 运算符（Python 3.9+）
    merged3 = d1 | d2
    print(f"  | 运算符: {merged3}")

    # ── 检查键是否存在 ───────────────────────────────────
    print("\n键的检查:")
    d = {"name": "Alice", "age": 20}
    print(f"  'name' in d      = {'name' in d}")      # True
    print(f"  'email' in d     = {'email' in d}")     # False
    print(f"  'email' not in d = {'email' not in d}") # True


# =============================================================================
# 5.4 集合（set）
# =============================================================================
#
# 对应 C++：std::unordered_set<T>
# 特点：无序、元素唯一（自动去重）、不支持索引

def demo_set() -> None:
    """演示集合的用法。"""
    print("\n" + "=" * 60)
    print("5.4 集合（set）")
    print("=" * 60)

    # ── 创建集合 ─────────────────────────────────────────
    empty_set = set()               # 空集合（注意：{} 创建的是空字典！）
    nums = {1, 2, 3, 4, 5}
    from_list = set([1, 2, 2, 3, 3, 3])   # 自动去重
    from_str = set("hello")                 # 从字符串创建（去重单个字符）

    print(f"空集合:   {empty_set}")
    print(f"数字集合: {nums}")
    print(f"列表去重: {from_list}")        # {1, 2, 3}
    print(f"字符串集合: {from_str}")       # {'h','e','l','o'}（无重复）

    # ── 集合操作 ─────────────────────────────────────────
    print("\n集合操作:")
    s = {3, 1, 4, 1, 5, 9, 2, 6}
    print(f"  原始（自动去重）: {s}")

    s.add(7)            # 添加元素
    print(f"  add(7): {s}")

    s.discard(1)        # 删除元素（不存在时不报错）
    print(f"  discard(1): {s}")

    s.discard(99)       # 删除不存在的元素（不会报错）
    print(f"  discard(99)（不报错）: {s}")

    # s.remove(99)      # remove 删除不存在的元素会 KeyError

    popped = s.pop()    # 随机删除并返回一个元素
    print(f"  pop(): 弹出 {popped}")

    # 成员检测（O(1) 时间复杂度，比列表快）
    print(f"  4 in s: {4 in s}")

    # ── 集合运算 ─────────────────────────────────────────
    print("\n集合运算:")
    A = {1, 2, 3, 4, 5}
    B = {4, 5, 6, 7, 8}
    print(f"  A = {A}")
    print(f"  B = {B}")

    # 并集：A ∪ B（两个集合的所有元素）
    print(f"  A | B（并集）     = {A | B}")
    print(f"  A.union(B)       = {A.union(B)}")

    # 交集：A ∩ B（两个集合共有的元素）
    print(f"  A & B（交集）     = {A & B}")
    print(f"  A.intersection(B)= {A.intersection(B)}")

    # 差集：A - B（在 A 中但不在 B 中）
    print(f"  A - B（差集）     = {A - B}")
    print(f"  A.difference(B)  = {A.difference(B)}")

    # 对称差集：A △ B（在 A 或 B 中，但不在两者中）
    print(f"  A ^ B（对称差集） = {A ^ B}")
    print(f"  A.symmetric_difference(B) = {A.symmetric_difference(B)}")

    # 子集和超集检查
    print(f"\n  {{1,2}} <= A（子集）  = {{{1, 2} <= A}}")
    print(f"  {{1,2}} < A（真子集） = {{{1, 2} < A}}")
    print(f"  A >= {{1,2}}（超集）  = {A >= {1, 2}}")

    # 不相交判断
    C = {10, 11, 12}
    print(f"  A.isdisjoint(C) = {A.isdisjoint(C)}")  # True（没有共同元素）

    # ── 集合推导式 ───────────────────────────────────────
    print("\n集合推导式:")
    squares_set = {x**2 for x in range(-3, 4)}  # 自动去重
    print(f"  -3到3的平方（去重）: {squares_set}")

    # ── 实用场景：去重 ───────────────────────────────────
    print("\n集合的实用场景:")
    data = [1, 2, 2, 3, 3, 3, 4, 4, 4, 4]
    unique = list(set(data))    # 去重（但会丢失顺序）
    unique.sort()               # 重新排序
    print(f"  列表去重: {unique}")

    # 找出两个列表的公共元素
    list1 = [1, 2, 3, 4, 5, 6]
    list2 = [4, 5, 6, 7, 8, 9]
    common = set(list1) & set(list2)
    print(f"  公共元素: {sorted(common)}")

    # 找出在 list1 中但不在 list2 中的元素
    only_in_list1 = set(list1) - set(list2)
    print(f"  仅在list1中: {sorted(only_in_list1)}")


# =============================================================================
# 5.5 冻结集合（frozenset）与选择建议
# =============================================================================

def demo_frozenset_and_summary() -> None:
    """演示 frozenset 并给出容器选择建议。"""
    print("\n" + "=" * 60)
    print("5.5 frozenset 与容器选择指南")
    print("=" * 60)

    # frozenset：不可变的集合（可以作为字典的键）
    fs = frozenset({1, 2, 3, 4, 5})
    print(f"frozenset: {fs}")
    print(f"类型: {type(fs)}")

    # frozenset 可以作为字典的键
    graph = {
        frozenset({1, 2}): "边A",
        frozenset({2, 3}): "边B",
    }
    print(f"以frozenset为键: {graph[frozenset({1, 2})]}")

    # ── 容器选择指南 ─────────────────────────────────────
    print("\n" + "─" * 40)
    print("容器选择指南：")
    print("─" * 40)
    guidelines = [
        ("list  []",      "有序、可变、允许重复 → 最通用，需要频繁增删元素时"),
        ("tuple ()",      "有序、不可变、允许重复 → 固定数据、函数多返回值、字典键"),
        ("dict  {}",      "键值对、键唯一 → 快速按名字查找、配置项、分组数据"),
        ("set   {}",      "无序、不重复 → 去重、快速成员检测、集合运算"),
        ("frozenset",     "不可变集合 → 需要作为字典键的集合"),
    ]
    for container, desc in guidelines:
        print(f"  {container:15} → {desc}")

    # ── 综合示例：学生成绩管理 ───────────────────────────
    print("\n综合示例：学生成绩管理系统")
    print("─" * 40)

    def analyze_scores(score_dict: dict[str, list[int]]) -> None:
        """
        分析学生各科成绩。

        Args:
            score_dict: {学生姓名: [科目1成绩, 科目2成绩, ...]}
        """
        subjects = ["语文", "数学", "英语"]

        # 计算每个学生的平均分
        averages = {
            name: sum(scores) / len(scores)
            for name, scores in score_dict.items()
        }

        # 找出平均分最高的学生
        top_student = max(averages, key=lambda name: averages[name])

        # 找出每科的最高分
        for i, subject in enumerate(subjects):
            subject_scores = {
                name: scores[i]
                for name, scores in score_dict.items()
            }
            best = max(subject_scores, key=lambda n: subject_scores[n])
            print(f"  {subject}最高分: {best}（{subject_scores[best]}分）")

        print(f"\n  各学生平均分:")
        for name, avg in sorted(averages.items(), key=lambda x: x[1], reverse=True):
            print(f"    {name}: {avg:.1f}")
        print(f"  综合最优: {top_student}（{averages[top_student]:.1f}分）")

    scores_data = {
        "Alice":   [92, 88, 95],
        "Bob":     [78, 95, 82],
        "Charlie": [88, 76, 90],
        "David":   [95, 91, 87],
    }
    analyze_scores(scores_data)


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_list()
    demo_tuple()
    demo_dict()
    demo_set()
    demo_frozenset_and_summary()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 列表 list ──
# []  /  list()  /  [x for x in ...]
# append / insert / extend / pop / remove / sort / reverse / clear
# 切片: lst[a:b:step]   可变
#
# ── 元组 tuple ──
# ()  /  (x,)  /  tuple()
# 索引和切片（只读）  不可变  可解包
#
# ── 字典 dict ──
# {} / dict() / {k:v for k,v in ...}
# d[key] / d.get(key, default) / d.setdefault(key, default)
# d.keys() / d.values() / d.items()
# d.pop(key) / d.update(other) / d | other (Python 3.9+)
#
# ── 集合 set ──
# {x,y,z} / set() / {x for x in ...}
# add / discard / remove / pop
# | 并集 / & 交集 / - 差集 / ^ 对称差集
# issubset / issuperset / isdisjoint


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：空集合用 {} 创建
# s = {}    ❌ 这是空字典，不是空集合！
# s = set() ✅
#
# 错误 2：单元素元组忘记逗号
# t = (42)   ❌ 这是整数 42，不是元组
# t = (42,)  ✅
#
# 错误 3：用 = 赋值以为是复制列表
# b = a      ❌ b 和 a 指向同一对象
# b = a.copy() / b = a[:] ✅
#
# 错误 4：在遍历时修改列表/字典
# for item in lst:
#     lst.remove(item)   ❌ 行为未定义
# 修正：for item in lst.copy(): ... 或用推导式重建
#
# 错误 5：字典访问不存在的键
# d["missing"]    ❌ KeyError
# d.get("missing", default)  ✅


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   给定一个整数列表，找出所有出现超过一次的元素
#   例如：[1,2,2,3,3,3,4] → [2, 3]
#
# 练习 2：
#   写一个函数 group_by(lst, key_func)，
#   将列表按照 key_func 的返回值分组
#   例如：group_by([1,2,3,4,5,6], lambda x: x%3)
#   → {1:[1,4], 2:[2,5], 0:[3,6]}
#
# 练习 3：
#   写一个函数 merge_dicts(*dicts) -> dict，
#   合并多个字典，相同键的值累加（假设值都是数字）
#   例如：merge_dicts({"a":1,"b":2}, {"b":3,"c":4}, {"a":5})
#   → {"a": 6, "b": 5, "c": 4}
#
# 练习 4：
#   给定两个列表，找出它们的对称差（在其中一个列表中出现但不在两者中出现）
#   用集合操作实现，结果排序后返回
#   例如：[1,2,3,4], [3,4,5,6] → [1, 2, 5, 6]


# =============================================================================
# 【练习答案】
# =============================================================================

def find_duplicates(lst: list[int]) -> list[int]:
    """练习 1：找出所有出现超过一次的元素。"""
    counts: dict[int, int] = {}
    for x in lst:
        counts[x] = counts.get(x, 0) + 1
    return sorted(k for k, v in counts.items() if v > 1)


def group_by(lst: list, key_func) -> dict:
    """练习 2：按 key_func 分组。"""
    result: dict = {}
    for item in lst:
        key = key_func(item)
        result.setdefault(key, []).append(item)
    return result


def merge_dicts(*dicts: dict) -> dict:
    """练习 3：合并多个字典，相同键的值累加。"""
    result: dict = {}
    for d in dicts:
        for key, value in d.items():
            result[key] = result.get(key, 0) + value
    return result


def symmetric_difference(lst1: list, lst2: list) -> list:
    """练习 4：求两个列表的对称差。"""
    return sorted(set(lst1) ^ set(lst2))


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print(find_duplicates([1, 2, 2, 3, 3, 3, 4]))  # [2, 3]
#     print(group_by([1,2,3,4,5,6], lambda x: x%3))  # {1:[1,4],2:[2,5],0:[3,6]}
#     print(merge_dicts({"a":1,"b":2},{"b":3,"c":4},{"a":5}))  # {'a':6,'b':5,'c':4}
#     print(symmetric_difference([1,2,3,4],[3,4,5,6]))  # [1,2,5,6]
