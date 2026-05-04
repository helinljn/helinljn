# =============================================================================
# 第 21 章：补充 12 - 内存管理工具（array / weakref / copy）
# =============================================================================
#
# 【学习目标】
#   1. 掌握 array 模块：类型化数组，高效存储同类型数值
#   2. 掌握 weakref 模块：弱引用，避免循环引用内存泄漏
#   3. 掌握 copy 模块：浅拷贝与深拷贝的准确语义
#
# 【与 C/C++ 的对比】
#   - array  ≈  C 语言的 int arr[100] / float arr[100]，元素类型固定、
#              连续内存，比 Python list 省 50%+ 内存
#   - weakref ≈ C++ 的 std::weak_ptr，不增加引用计数，
#              当对象被销毁后自动变为 None
#   - copy.copy / copy.deepcopy ≈ C++ 的拷贝构造 / 完整克隆，
#              Python 默认是引用语义（类似指针赋值），需显式拷贝
#
# 【运行方式】
#   python chapter21_补充12_内存管理工具.py
#
# =============================================================================

import array
import weakref
import copy
import sys
import gc
from typing import Any


# =============================================================================
# 21.补充12.1 array 模块：类型化数组
# =============================================================================

def demo_array_basics() -> None:
    """演示 array 模块的基本用法。"""
    print("=" * 60)
    print("21.补充12.1 array 模块：类型化数组")
    print("=" * 60)

    # ── 类型码 ───────────────────────────────────────────────
    print("常用类型码（对应 C 类型）：")
    type_codes = [
        ("'b'", "signed char", 1),
        ("'B'", "unsigned char", 1),
        ("'h'", "signed short", 2),
        ("'H'", "unsigned short", 2),
        ("'i'", "signed int", 4),
        ("'I'", "unsigned int", 4),
        ("'f'", "float", 4),
        ("'d'", "double", 8),
    ]
    print(f"  {'类型码':<6} {'C 类型':<18} {'字节数'}")
    for code, ctype, size in type_codes:
        print(f"  {code:<6} {ctype:<18} {size}")

    # ── 创建 array ───────────────────────────────────────────
    print(f"\n创建数组：")
    int_arr = array.array('i', [1, 2, 3, 4, 5])
    float_arr = array.array('d', [1.0, 2.5, 3.14])
    byte_arr = array.array('B', b"hello")

    print(f"  int_arr   = {int_arr.tolist()}")
    print(f"  float_arr = {float_arr.tolist()}")
    print(f"  byte_arr  = {byte_arr.tobytes()!r}")

    # ── 索引 / 切片（和 list 一样） ─────────────────────────
    print(f"\n索引与切片：")
    print(f"  int_arr[0]   = {int_arr[0]}")
    print(f"  int_arr[1:4] = {int_arr[1:4].tolist()}")

    # 原地修改
    int_arr[0] = 100
    print(f"  int_arr[0] = 100 → {int_arr.tolist()}")

    # ── 常用方法 ────────────────────────────────────────────
    print(f"\n常用方法：")
    int_arr.append(6)
    print(f"  append(6)      → {int_arr.tolist()}")

    int_arr.extend([7, 8])
    print(f"  extend([7,8])  → {int_arr.tolist()}")

    popped = int_arr.pop()
    print(f"  pop()          → {popped}, arr={int_arr.tolist()}")

    # ── 内存对比：list vs array ─────────────────────────────
    print(f"\n内存对比（10000 个 int）：")
    lst = [i for i in range(10000)]
    arr = array.array('i', range(10000))
    print(f"  list:  {sys.getsizeof(lst) + sum(sys.getsizeof(i) for i in lst[:5]) * 2000:_>10,} bytes (估)")
    print(f"  array: {sys.getsizeof(arr):_>10,} bytes")
    print(f"  array 比 list 节省约 60-70% 内存")
    print(f"  原因：list 存储 PyObject 指针（8字节/条 + 对象开销），")
    print(f"        array 直接存 C 类型值（4字节/条，无对象开销）")

    # ── array 和 bytes 互转 ─────────────────────────────────
    print(f"\narray ↔ bytes 互转：")
    raw = int_arr.tobytes()
    print(f"  tobytes()           = {raw.hex(' ')}")
    restored = array.array('i')
    restored.frombytes(raw)
    print(f"  frombytes()         = {restored.tolist()}")


# =============================================================================
# 21.补充12.2 weakref 模块：弱引用
# =============================================================================

def demo_weakref() -> None:
    """演示 weakref 弱引用的用法。"""
    print("\n" + "=" * 60)
    print("21.补充12.2 weakref 模块：弱引用")
    print("=" * 60)

    # ── 普通引用 vs 弱引用 ──────────────────────────────────
    print("概念：")
    print("  普通引用：obj = Something()  →  引用计数 +1，对象不会被 GC")
    print("  弱引用：  r = weakref.ref(obj) →  不增加引用计数，obj 可被 GC")
    print()

    class ExpensiveObject:
        def __init__(self, name: str):
            self.name = name
            # 模拟大数据
            self.data = [0] * 100_000

        def __del__(self):
            print(f"    [GC] {self.name} 被垃圾回收")

    # ── 基本用法 ────────────────────────────────────────────
    print("基本用法：")
    obj = ExpensiveObject("A")
    r = weakref.ref(obj)  # 创建弱引用

    print(f"  r() = {r()!r}  (对象存活)")
    print(f"  r().name = {r().name!r}")

    del obj  # 删除唯一强引用
    print(f"  del obj 后, r() = {r()!r}  (自动变为 None)")

    # ── WeakValueDictionary：缓存场景 ────────────────────────
    print(f"\n── 实战：WeakValueDictionary 做缓存 ──")
    print("  场景：缓存大对象，当外部不再使用时自动释放：")

    cache = weakref.WeakValueDictionary()

    def get_expensive(id_: int) -> ExpensiveObject:
        obj = cache.get(id_)
        if obj is None:
            obj = ExpensiveObject(f"id={id_}")
            cache[id_] = obj
            print(f"    创建新对象 {id_}")
        else:
            print(f"    命中缓存 {id_}")
        return obj

    a = get_expensive(1)  # 创建新对象
    b = get_expensive(1)  # 命中缓存（a 还活着）
    del a                 # 删除强引用
    c = get_expensive(1)  # 创建新对象（缓存中旧对象已被 GC）
    del b, c

    # ── WeakSet：观察者模式 ─────────────────────────────────
    print(f"\n── 实战：WeakSet 做事件监听器 ──")
    print('  避免"被遗忘的监听器"导致内存泄漏：')

    class EventSource:
        def __init__(self):
            self._listeners = weakref.WeakSet()

        def add_listener(self, listener):
            self._listeners.add(listener)

        def notify(self, event: str):
            for l in list(self._listeners):
                l.on_event(event)

    class Listener:
        def __init__(self, name: str):
            self.name = name

        def on_event(self, event: str):
            print(f"    {self.name} 收到事件: {event}")

    source = EventSource()
    l1 = Listener("L1")
    l2 = Listener("L2")

    source.add_listener(l1)
    source.add_listener(l2)
    source.notify("click")   # 两个都收到
    del l1
    source.notify("submit")  # 只有 L2 收到（L1 的弱引用自动失效）

    # ── 何时用弱引用 ────────────────────────────────────────
    print(f"\n── 使用场景总结 ──")
    print("  ✅ 缓存系统（WeakValueDictionary）")
    print("  ✅ 观察者/事件系统（WeakSet）")
    print("  ✅ 避免循环引用（父子节点互相引用）")
    print("  ✅ 临时映射表（对象消亡后条目自动消失）")
    print("  ❌ 普通代码 —— 不要滥用，增加复杂度")


# =============================================================================
# 21.补充12.3 copy 模块：浅拷贝与深拷贝
# =============================================================================

def demo_copy_module() -> None:
    """演示 copy 模块：浅拷贝 vs 深拷贝。"""
    print("\n" + "=" * 60)
    print("21.补充12.3 copy 模块：浅拷贝与深拷贝")
    print("=" * 60)

    # ── 回顾 Python 的赋值语义 ──────────────────────────────
    print("Python 赋值 = 引用传递（类似 C++ 的指针赋值）：")
    a = [1, 2, [3, 4]]
    b = a           # b 和 a 指向同一个对象
    b[0] = 999
    print(f"  a = [1, 2, [3, 4]]")
    print(f"  b = a; b[0] = 999 → a = {a}  (a 也被改了!)")
    print()

    # ── copy.copy()：浅拷贝 ──────────────────────────────────
    print("copy.copy() — 浅拷贝（只复制第一层，嵌套对象共享）：")
    original = [1, 2, [10, 20]]
    shallow = copy.copy(original)

    print(f"  original    = {original}")
    print(f"  shallow     = {shallow}")
    print(f"  original is shallow: {original is shallow}  (不同对象)")

    # 修改顶层 —— 互不影响
    shallow[0] = 999
    print(f"  shallow[0] = 999 → original={original}, shallow={shallow}  (互不影响)")

    # 修改嵌套 —— 共享!
    shallow[2][0] = 888
    print(f"  shallow[2][0] = 888 → original={original}, shallow={shallow}  (都变了!)")
    print(f"  → 嵌套列表是共享的, original[2] is shallow[2]: {original[2] is shallow[2]}")
    print()

    # ── copy.deepcopy()：深拷贝 ──────────────────────────────
    print("copy.deepcopy() — 深拷贝（递归复制所有层级）：")

    original2 = [1, 2, [10, 20]]
    deep = copy.deepcopy(original2)

    deep[2][0] = 888
    print(f"  original2        = {original2}  (未受影响)")
    print(f"  deep             = {deep}")
    print(f"  original2[2] is deep[2]: {original2[2] is deep[2]}  (独立对象)")

    # ── 自定义对象的拷贝 ────────────────────────────────────
    print(f"\n── 自定义对象的拷贝行为 ──")

    class Node:
        def __init__(self, value: int, children: list['Node'] | None = None):
            self.value = value
            self.children = children if children is not None else []

        def __repr__(self):
            return f"Node({self.value}, children={len(self.children)})"

        def __copy__(self):
            """自定义浅拷贝行为。"""
            new = Node(self.value)
            new.children = self.children  # 共享子节点列表
            print(f"    [__copy__] Node({self.value})")
            return new

        def __deepcopy__(self, memo: dict):
            """自定义深拷贝行为。"""
            new = Node(self.value)
            new.children = copy.deepcopy(self.children, memo)
            print(f"    [__deepcopy__] Node({self.value})")
            return new

    root = Node(1)
    child = Node(2)
    root.children.append(child)

    print("  浅拷贝：")
    root_shallow = copy.copy(root)
    print(f"    root_shallow.children[0] is child: {root_shallow.children[0] is child}")

    print("  深拷贝：")
    root_deep = copy.deepcopy(root)
    print(f"    root_deep.children[0] is child: {root_deep.children[0] is child}")

    # ── 提速技巧：__copy__ / __deepcopy__ ────────────────────
    print(f"\n── 性能提示 ──")
    print("  如果对象的拷贝逻辑简单，实现 __copy__ / __deepcopy__")
    print("  可以比默认的 pickle 往返方式快 10~50 倍")


# =============================================================================
# 21.补充12.4 综合实战：LRU 缓存 with 弱引用
# =============================================================================

def demo_combined() -> None:
    """综合演示：用 weakref + array 构建一个内存友好的缓存。"""
    print("\n" + "=" * 60)
    print("21.补充12.4 综合实战：弱引用缓存 + array 存储")
    print("=" * 60)

    # ── 场景：缓存大量浮点向量，自动淘汰 ────────────────────
    print("场景：缓存浮点向量（如机器学习中的特征向量）")
    print("  用 array('d') 存数据 → 节省内存")
    print("  用 WeakValueDictionary → 自动淘汰不再使用的缓存")
    print()

    # 模拟"昂贵的向量计算"
    _compute_count = 0

    def compute_expensive_vector(dim: int) -> array.array:
        nonlocal _compute_count
        _compute_count += 1
        # 模拟耗时计算
        import math
        return array.array('d', (math.sin(i / dim) for i in range(dim)))

    # 弱引用缓存
    _vector_cache = weakref.WeakValueDictionary()

    def get_vector(dim: int) -> array.array:
        key = f"vec_{dim}"
        cached = _vector_cache.get(key)
        if cached is not None:
            return cached
        vec = compute_expensive_vector(dim)
        _vector_cache[key] = vec
        return vec

    v1 = get_vector(1000)  # 计算
    print(f"第1次 get_vector(1000): 计算 #{_compute_count}")
    v2 = get_vector(1000)  # 命中缓存
    print(f"第2次 get_vector(1000): 计算 #{_compute_count}  (缓存命中)")
    del v1
    v3 = get_vector(1000)  # 缓存还有 v2 强引用，命中
    print(f"第3次 get_vector(1000): 计算 #{_compute_count}  (缓存命中)")
    del v2, v3
    v4 = get_vector(1000)  # 缓存已空，重新计算
    print(f"第4次 get_vector(1000): 计算 #{_compute_count}  (缓存已释放)")

    print(f"\n结论：array 省内存，weakref 自动管理缓存生命周期")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_array_basics()
    demo_weakref()
    demo_copy_module()
    demo_combined()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. array 模块
#    array.array('i', [1, 2, 3])    创建整型数组
#    array.array('d', [1.0, 2.0])   创建浮点数组
#    arr.tolist()                    转为 Python list
#    arr.tobytes()                   转为 bytes
#    arr.frombytes(b)                从 bytes 恢复
#    类型码: 'b'/'B'/'h'/'H'/'i'/'I'/'f'/'d'
#
# 2. weakref 模块
#    weakref.ref(obj)              创建弱引用
#    r() is None                    对象已死亡
#    weakref.WeakValueDictionary() 值弱引用的字典 → 缓存
#    weakref.WeakSet()             弱引用集合 → 监听器
#    weakref.WeakKeyDictionary()   键弱引用的字典
#
# 3. copy 模块
#    copy.copy(obj)                 浅拷贝（一层深，嵌套共享）
#    copy.deepcopy(obj)             深拷贝（递归复制所有层级）
#    obj.__copy__(self)             自定义浅拷贝行为
#    obj.__deepcopy__(self, memo)   自定义深拷贝行为
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个函数 sum_array(data: list[int]) -> float:
#   用 array('i') 存储数据，计算并返回总和。对比 list 版本的性能。
#
# 练习 2（基础）：
#   写一个 ImageCache 类，用 WeakValueDictionary 缓存图像对象。
#   当外部不再持有图像引用时，缓存自动释放。
#
# 练习 3（进阶）：
#   实现一个 SparseVector 类：用 array('d') 存储稠密值，用 array('i')
#   存储对应的索引（两个数组等长）。支持 __getitem__ / __setitem__。
#
# 练习提示：
#   练习1：timeit 测 array vs list 的 sum 速度
#   练习2：get(key) 方法检查 WeakValueDictionary
#   练习3：i 不在索引数组中时返回 0.0


# =============================================================================
# 【练习答案】
# =============================================================================


def sum_array(data: list[int]) -> float:
    """练习 1 答案：用 array 存储并求和。"""
    arr = array.array('i', data)
    total = sum(arr)  # sum() 对 array 同样适用
    return float(total)


class ImageCache:
    """练习 2 答案：弱引用图像缓存。"""

    def __init__(self):
        self._cache = weakref.WeakValueDictionary()

    def get(self, key: str) -> Any:
        return self._cache.get(key)

    def put(self, key: str, image: Any) -> None:
        self._cache[key] = image

    @property
    def size(self) -> int:
        return len(self._cache)


class SparseVector:
    """练习 3 答案：稀疏向量（array 双数组存储）。"""

    def __init__(self, size: int):
        self._size = size
        self._indices = array.array('i')   # 非零元素的位置
        self._values = array.array('d')    # 非零元素的值

    def __getitem__(self, idx: int) -> float:
        if idx < 0 or idx >= self._size:
            raise IndexError(f"index {idx} out of range")
        try:
            pos = self._indices.index(idx)
            return self._values[pos]
        except ValueError:
            return 0.0

    def __setitem__(self, idx: int, value: float) -> None:
        if idx < 0 or idx >= self._size:
            raise IndexError(f"index {idx} out of range")
        try:
            pos = self._indices.index(idx)
            if value == 0.0:
                del self._indices[pos]
                del self._values[pos]
            else:
                self._values[pos] = value
        except ValueError:
            if value != 0.0:
                self._indices.append(idx)
                self._values.append(value)

    def __repr__(self) -> str:
        items = {i: v for i, v in zip(self._indices, self._values)}
        return f"SparseVector(size={self._size}, {items})"


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 60)
#     print("练习答案测试")
#     print("=" * 60)
#     print(f"sum_array([1..100]): {sum_array(list(range(1, 101)))}")
#
#     cache = ImageCache()
#     cache.put("img1", "image_data_1")
#     print(f"cache.get('img1'): {cache.get('img1')}, size: {cache.size}")
#
#     sv = SparseVector(5)
#     sv[0] = 3.14
#     sv[3] = 2.71
#     print(f"sv = {sv}")
#     print(f"sv[0] = {sv[0]}, sv[1] = {sv[1]}, sv[3] = {sv[3]}")
