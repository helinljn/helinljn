# =============================================================================
# 第 10 章：面向对象进阶（继承、多态、魔术方法、dataclass）
# =============================================================================
#
# 【学习目标】
#   1. 掌握继承（单继承、多继承）
#   2. 理解方法重写和 super()
#   3. 理解多态的概念
#   4. 掌握常用魔术方法
#   5. 了解抽象基类（ABC）
#   6. 掌握上下文管理器
#   7. 掌握 dataclasses 数据类
#   8. 理解元类（metaclass）基础——类的"类"
#
# 【运行方式】
#   python chapter10_面向对象进阶.py
#
# =============================================================================

from abc import ABC, abstractmethod
from typing import Any


# =============================================================================
# 10.1 继承基础
# =============================================================================

class Animal:
    """动物基类。"""

    def __init__(self, name: str, age: int) -> None:
        self.name = name
        self.age = age

    def speak(self) -> str:
        """动物叫声（基类提供默认实现）。"""
        return f"{self.name} 发出声音"

    def info(self) -> str:
        """基本信息。"""
        return f"{self.name}，{self.age}岁"


class Dog(Animal):
    """狗类（继承自 Animal）。"""

    def __init__(self, name: str, age: int, breed: str) -> None:
        # 调用父类构造函数
        super().__init__(name, age)
        self.breed = breed

    def speak(self) -> str:
        """重写父类方法。"""
        return f"{self.name}: 汪汪汪！"

    def fetch(self) -> str:
        """狗特有的方法。"""
        return f"{self.name} 去捡球了"


class Cat(Animal):
    """猫类（继承自 Animal）。"""

    def __init__(self, name: str, age: int, color: str) -> None:
        super().__init__(name, age)
        self.color = color

    def speak(self) -> str:
        """重写父类方法。"""
        return f"{self.name}: 喵喵喵~"

    def climb(self) -> str:
        """猫特有的方法。"""
        return f"{self.name} 爬树了"


def demo_inheritance() -> None:
    """演示继承基础。"""
    print("=" * 60)
    print("10.1 继承基础")
    print("=" * 60)

    # 创建对象
    dog = Dog("旺财", 3, "金毛")
    cat = Cat("咪咪", 2, "橘色")

    print(f"Dog: {dog.info()}, 品种: {dog.breed}")
    print(f"  {dog.speak()}")
    print(f"  {dog.fetch()}")

    print(f"\nCat: {cat.info()}, 颜色: {cat.color}")
    print(f"  {cat.speak()}")
    print(f"  {cat.climb()}")

    # 类型检查
    print(f"\n类型检查:")
    print(f"  isinstance(dog, Dog) = {isinstance(dog, Dog)}")
    print(f"  isinstance(dog, Animal) = {isinstance(dog, Animal)}")
    print(f"  issubclass(Dog, Animal) = {issubclass(Dog, Animal)}")


# =============================================================================
# 10.2 多态
# =============================================================================

def make_animal_speak(animal: Animal) -> None:
    """
    多态演示：接受 Animal 类型，但可以传入任何子类。
    运行时根据实际类型调用对应的方法。
    """
    print(f"  {animal.speak()}")


def demo_polymorphism() -> None:
    """演示多态。"""
    print("\n" + "=" * 60)
    print("10.2 多态")
    print("=" * 60)

    animals: list[Animal] = [
        Dog("小白", 2, "柴犬"),
        Cat("小黑", 1, "黑色"),
        Dog("大黄", 4, "土狗"),
        Cat("小花", 3, "三花"),
    ]

    print("让所有动物说话（多态）:")
    for animal in animals:
        make_animal_speak(animal)


# =============================================================================
# 10.3 super() 和方法解析顺序（MRO）
# =============================================================================

class A:
    def method(self):
        print("  A.method()")


class B(A):
    def method(self):
        print("  B.method() 开始")
        super().method()  # 调用父类方法
        print("  B.method() 结束")


class C(A):
    def method(self):
        print("  C.method() 开始")
        super().method()
        print("  C.method() 结束")


class D(B, C):
    """多继承：D 继承自 B 和 C。"""
    def method(self):
        print("  D.method() 开始")
        super().method()  # 按 MRO 顺序调用
        print("  D.method() 结束")


def demo_super_and_mro() -> None:
    """演示 super() 和方法解析顺序。"""
    print("\n" + "=" * 60)
    print("10.3 super() 和 MRO")
    print("=" * 60)

    print("单继承中的 super():")
    b = B()
    b.method()

    print("\n多继承中的 super() 和 MRO:")
    d = D()
    d.method()

    print(f"\nD 的 MRO（方法解析顺序）:")
    for i, cls in enumerate(D.__mro__, 1):
        print(f"  {i}. {cls.__name__}")


# =============================================================================
# 10.4 魔术方法（Magic Methods / Dunder Methods）
# =============================================================================

class Vector:
    """二维向量 —— 演示运算符重载。"""

    def __init__(self, x: float, y: float) -> None:
        self.x = x
        self.y = y

    def __str__(self) -> str:
        """str() 和 print() 调用。"""
        return f"Vector({self.x}, {self.y})"

    def __repr__(self) -> str:
        """repr() 调用，应返回可重建对象的字符串。"""
        return f"Vector({self.x!r}, {self.y!r})"

    def __add__(self, other: "Vector") -> "Vector":
        """重载 + 运算符。"""
        if not isinstance(other, Vector):
            return NotImplemented
        return Vector(self.x + other.x, self.y + other.y)

    def __sub__(self, other: "Vector") -> "Vector":
        """重载 - 运算符。"""
        if not isinstance(other, Vector):
            return NotImplemented
        return Vector(self.x - other.x, self.y - other.y)

    def __mul__(self, scalar: float) -> "Vector":
        """重载 * 运算符（标量乘法）。"""
        return Vector(self.x * scalar, self.y * scalar)

    def __eq__(self, other: object) -> bool:
        """重载 == 运算符。"""
        if not isinstance(other, Vector):
            return NotImplemented
        return self.x == other.x and self.y == other.y

    def __abs__(self) -> float:
        """重载 abs() 函数（向量长度）。"""
        return (self.x ** 2 + self.y ** 2) ** 0.5

    def __bool__(self) -> bool:
        """重载 bool() 函数（零向量为 False）。"""
        return abs(self) != 0


class Book:
    """书籍 —— 演示容器相关魔术方法。"""

    def __init__(self, title: str, pages: list[str]) -> None:
        self.title = title
        self._pages = pages

    def __len__(self) -> int:
        """重载 len() 函数。"""
        return len(self._pages)

    def __getitem__(self, index: int) -> str:
        """重载 [] 运算符（读取）。"""
        return self._pages[index]

    def __setitem__(self, index: int, value: str) -> None:
        """重载 [] 运算符（写入）。"""
        self._pages[index] = value

    def __contains__(self, text: str) -> bool:
        """重载 in 运算符。"""
        return any(text in page for page in self._pages)

    def __iter__(self):
        """重载 iter()，使对象可迭代。"""
        return iter(self._pages)


def demo_magic_methods() -> None:
    """演示魔术方法。"""
    print("\n" + "=" * 60)
    print("10.4 魔术方法")
    print("=" * 60)

    # ── Vector 运算符重载 ────────────────────────────────
    print("Vector 运算符重载:")
    v1 = Vector(3, 4)
    v2 = Vector(1, 2)

    print(f"  v1 = {v1}")
    print(f"  v2 = {v2}")
    print(f"  v1 + v2 = {v1 + v2}")
    print(f"  v1 - v2 = {v1 - v2}")
    print(f"  v1 * 2 = {v1 * 2}")
    print(f"  abs(v1) = {abs(v1):.2f}")
    print(f"  v1 == Vector(3, 4) = {v1 == Vector(3, 4)}")
    print(f"  bool(Vector(0, 0)) = {bool(Vector(0, 0))}")

    # ── Book 容器方法 ────────────────────────────────────
    print("\nBook 容器方法:")
    book = Book("Python 教程", ["第1页", "第2页", "第3页"])

    print(f"  len(book) = {len(book)}")
    print(f"  book[0] = {book[0]!r}")
    print(f"  'Python' in book = {'Python' in book}")

    print("  遍历书页:")
    for i, page in enumerate(book, 1):
        print(f"    {i}. {page}")


# =============================================================================
# 10.5 抽象基类（ABC）
# =============================================================================

class Shape(ABC):
    """
    抽象形状类。
    使用 ABC 定义接口，强制子类实现特定方法。
    """

    def __init__(self, name: str) -> None:
        self.name = name

    @abstractmethod
    def area(self) -> float:
        """抽象方法：子类必须实现。"""
        pass

    @abstractmethod
    def perimeter(self) -> float:
        """抽象方法：子类必须实现。"""
        pass

    def describe(self) -> str:
        """具体方法：子类可以继承。"""
        return f"{self.name}: 面积={self.area():.2f}, 周长={self.perimeter():.2f}"


class Rectangle(Shape):
    """矩形（实现抽象基类）。"""

    def __init__(self, width: float, height: float) -> None:
        super().__init__("矩形")
        self.width = width
        self.height = height

    def area(self) -> float:
        return self.width * self.height

    def perimeter(self) -> float:
        return 2 * (self.width + self.height)


class Circle(Shape):
    """圆形（实现抽象基类）。"""

    def __init__(self, radius: float) -> None:
        super().__init__("圆形")
        self.radius = radius

    def area(self) -> float:
        import math
        return math.pi * self.radius ** 2

    def perimeter(self) -> float:
        import math
        return 2 * math.pi * self.radius


def demo_abc() -> None:
    """演示抽象基类。"""
    print("\n" + "=" * 60)
    print("10.5 抽象基类（ABC）")
    print("=" * 60)

    shapes: list[Shape] = [
        Rectangle(5, 3),
        Circle(4),
        Rectangle(10, 10),
    ]

    for shape in shapes:
        print(f"  {shape.describe()}")

    # 尝试实例化抽象类会报错
    print("\n尝试实例化抽象类:")
    try:
        shape = Shape("测试")  # type: ignore
    except TypeError as e:
        print(f"  ✓ 错误: {e}")


# =============================================================================
# 10.6 上下文管理器
# =============================================================================

class FileManager:
    """
    文件管理器 —— 演示上下文管理器。
    实现 __enter__ 和 __exit__ 方法。
    """

    def __init__(self, filename: str, mode: str = "r") -> None:
        self.filename = filename
        self.mode = mode
        self.file = None

    def __enter__(self):
        """进入 with 块时调用。"""
        print(f"  打开文件: {self.filename}")
        self.file = open(self.filename, self.mode)
        return self.file

    def __exit__(self, exc_type, exc_val, exc_tb):
        """退出 with 块时调用（即使发生异常也会调用）。"""
        if self.file:
            print(f"  关闭文件: {self.filename}")
            self.file.close()
        # 返回 False 表示不抑制异常，返回 True 表示抑制异常
        return False


class Timer:
    """计时器上下文管理器。"""

    def __init__(self, name: str = "操作") -> None:
        self.name = name
        self.start_time = 0.0

    def __enter__(self):
        import time
        self.start_time = time.time()
        print(f"  ⏱️  开始 {self.name}...")
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        import time
        elapsed = time.time() - self.start_time
        print(f"  ✅ {self.name} 完成，耗时: {elapsed:.4f}秒")
        return False


def demo_context_manager() -> None:
    """演示上下文管理器。"""
    print("\n" + "=" * 60)
    print("10.6 上下文管理器")
    print("=" * 60)

    # 使用 Timer
    print("Timer 示例:")
    with Timer("数据处理"):
        total = sum(range(1000000))
        print(f"    计算结果: {total}")

    # 演示文件管理器（需要先创建测试文件）
    print("\nFileManager 示例:")
    test_file = "test_context.txt"

    # 写入文件
    with FileManager(test_file, "w") as f:
        f.write("Hello, Context Manager!\n")

    # 读取文件
    with FileManager(test_file, "r") as f:
        content = f.read()
        print(f"    文件内容: {content.strip()}")

    # 清理
    import os
    if os.path.exists(test_file):
        os.remove(test_file)


# =============================================================================
# 10.7 dataclasses：数据类
# =============================================================================
#
# 【背景】
#   在 C/C++ 中，struct 用于聚合数据；在 Python 3.6 之前，创建一个纯数据类需要写大量的
#   样板代码（__init__、__repr__、__eq__ 等）。Python 3.7 引入的 @dataclass 装饰器
#   自动生成这些方法，让数据类的定义和使用像 C struct 一样简洁。
#

from dataclasses import dataclass, field


# ── 1. @dataclass 基础 ──────────────────────────────────────────────────────

@dataclass
class Weapon:
    """武器数据类 —— 演示 @dataclass 基础功能。

    C/C++ 对比:
      struct Weapon {
          const char* name;
          int damage;
          float weight = 5.0;
          const char* element = nullptr;
      };
    """
    name: str
    damage: int
    weight: float = 5.0
    element: str | None = None


@dataclass
class Player:
    """玩家数据类 —— 演示 field()、default_factory、frozen 等进阶特性。"""
    name: str
    level: int
    # default_factory: 每次创建实例都调用，生成独立的可变默认值
    #   C/C++ 对比: C struct 不能直接写动态默认值，通常需要 malloc + init 函数
    inventory: list[str] = field(default_factory=list)
    # compare=False: 参与 __repr__ 但忽略 __eq__ 比较
    last_login: str = field(default="未知", compare=False)
    # repr=False: 不显示在 __repr__ 输出中（类似日志打印时过滤敏感字段）
    password_hash: str = field(default="", repr=False)

    # ── __post_init__ 用于初始化后验证 ──────────────────────
    def __post_init__(self) -> None:
        """初始化后自动调用 —— 类似 C 中通过 init_struct() 进行数据校验。"""
        if self.level < 1:
            raise ValueError(f"等级必须 >= 1，当前值: {self.level}")


# ── 3. frozen=True（不可变数据类）───────────────────────────────────────────

@dataclass(frozen=True)
class ColorRGB:
    """不可变数据类 —— 类似 C/C++ 的 const struct。

    C/C++ 对比:
      const struct Color { int r, g, b; };
      实例化后所有字段只读，修改会抛出 FrozenInstanceError。
    """
    r: int = 0
    g: int = 0
    b: int = 0

    def as_hex(self) -> str:
        """将 RGB 值转为十六进制颜色码。"""
        return f"#{self.r:02X}{self.g:02X}{self.b:02X}"


# ── 4. 与普通类的对比（样板代码演示）───────────────────────────────────────

class PlayerRegular:
    """与 Player 等价的普通类 —— 展示 @dataclass 消除了多少样板代码。

    以下是手动编写的 __init__ + __repr__ + __eq__，代码量约为 dataclass 版的 3 倍。
    """

    def __init__(
        self,
        name: str,
        level: int,
        inventory: list[str] | None = None,
        last_login: str = "未知",
        password_hash: str = "",
    ) -> None:
        self.name = name
        self.level = level
        self.inventory = inventory if inventory is not None else []
        self.last_login = last_login
        self.password_hash = password_hash
        if self.level < 1:
            raise ValueError(f"等级必须 >= 1，当前值: {self.level}")

    def __repr__(self) -> str:
        return (
            f"PlayerRegular(name={self.name!r}, level={self.level}, "
            f"inventory={self.inventory!r}, last_login={self.last_login!r})"
        )

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, PlayerRegular):
            return NotImplemented
        return (self.name, self.level, self.inventory) == (
            other.name, other.level, other.inventory)


# ── 5. 结构模式匹配（match/case）───────────────────────────────────────────

@dataclass
class Point2D:
    """二维点。"""
    x: float = 0.0
    y: float = 0.0


@dataclass
class CircleShape:
    """圆形。"""
    radius: float
    center: Point2D


@dataclass
class RectShape:
    """矩形。"""
    width: float
    height: float
    origin: Point2D


def describe_shape(shape: CircleShape | RectShape) -> str:
    """使用 match/case 对 dataclass 进行结构模式匹配。

    match/case 通过 dataclass 的 __match_args__ 自动支持字段解构，
    无需手写 __match_args__ 元组。
    """
    match shape:
        case CircleShape(radius=r, center=Point2D(x=x, y=y)):
            return f"圆形：半径={r}，圆心=({x}, {y})"
        case RectShape(width=w, height=h, origin=Point2D(x=x, y=y)):
            return f"矩形：{w}×{h}，原点=({x}, {y})"
        case _:
            raise ValueError(f"未知形状: {shape}")


def demo_dataclasses() -> None:
    """演示 dataclasses：数据类（自动生成 __init__、__repr__、__eq__ 等）。"""
    print("\n" + "=" * 60)
    print("10.7 dataclasses：数据类")
    print("=" * 60)

    # ── 基础使用 ────────────────────────────────────────────
    print("1. @dataclass 基础:")
    sword = Weapon(name="倚天剑", damage=25)
    bow = Weapon(name="落日弓", damage=18, element="火")

    print(f"  {sword}")
    print(f"  {bow}")
    # 自动生成的 __eq__：按字段值逐一比较
    print(f"  sword == Weapon('倚天剑', 25) = {sword == Weapon('倚天剑', 25)}")
    print(f"  sword == bow = {sword == bow}")

    # ── field() 与 default_factory ─────────────────────────
    print("\n2. field() 函数 (default_factory / compare / repr):")
    p1 = Player(name="亚瑟", level=30, inventory=["长剑", "盾牌"])
    p2 = Player(name="亚瑟", level=30)  # inventory 自动初始化为 []
    p2.last_login = p1.last_login  # 对齐 compare=False 的字段

    print(f"  p1 = {p1}")
    print(f"  p2 = {p2}")
    print(f"  ⚠ password_hash 因 repr=False 不显示")
    # compare=False 的 last_login 被忽略，所以 p1 == p2
    print(f"  p1 == p2（忽略 last_login）= {p1 == p2}")

    # ── frozen=True ─────────────────────────────────────────
    print("\n3. frozen=True（不可变数据类）:")
    red = ColorRGB(255, 0, 0)
    white = ColorRGB(255, 255, 255)
    print(f"  red   = {red}  ->  {red.as_hex()}")
    print(f"  white = {white}  ->  {white.as_hex()}")

    try:
        red.r = 128  # type: ignore
    except Exception as e:
        print(f"  ✓ 修改 frozen 实例 -> {type(e).__name__}: {e}")

    # ── __post_init__ 验证 ──────────────────────────────────
    print("\n4. __post_init__ 数据验证:")
    try:
        bad_player = Player(name="菜鸟", level=-5)
    except ValueError as e:
        print(f"  ✓ 验证失败: {e}")

    # ── 样板代码对比 ────────────────────────────────────────
    print("\n5. 样板代码对比（dataclass vs 普通类）:")
    print(f"  Player (@dataclass) 字段数 = {len(Player.__dataclass_fields__)}")
    # 用正则类实例化一次证明功能等价
    pr = PlayerRegular(name="测试", level=1)
    print(f"  PlayerRegular (普通类) 输出: {pr}")
    print(f"  💡 @dataclass 消除了 ~70% 的重复样板代码")

    # ── match/case 模式匹配 ─────────────────────────────────
    print("\n6. match/case 结构模式匹配:")
    shapes: list[CircleShape | RectShape] = [
        CircleShape(radius=5.0, center=Point2D(0, 0)),
        RectShape(width=4.0, height=3.0, origin=Point2D(1, 2)),
        CircleShape(radius=2.5, center=Point2D(-3, 4)),
    ]
    for shape in shapes:
        print(f"  {describe_shape(shape)}")


# =============================================================================
# 10.9 综合示例：图形类层次结构
# =============================================================================

class Point:
    """点。"""

    def __init__(self, x: float, y: float) -> None:
        self.x = x
        self.y = y

    def distance_to(self, other: "Point") -> float:
        """计算到另一个点的距离。"""
        return ((self.x - other.x) ** 2 + (self.y - other.y) ** 2) ** 0.5

    def __str__(self) -> str:
        return f"({self.x}, {self.y})"


class GeometricShape(ABC):
    """几何图形抽象基类。"""

    @abstractmethod
    def area(self) -> float:
        pass

    @abstractmethod
    def perimeter(self) -> float:
        pass

    @abstractmethod
    def contains_point(self, point: Point) -> bool:
        """判断点是否在图形内。"""
        pass


class Triangle(GeometricShape):
    """三角形。"""

    def __init__(self, p1: Point, p2: Point, p3: Point) -> None:
        self.p1 = p1
        self.p2 = p2
        self.p3 = p3

    def area(self) -> float:
        """使用海伦公式计算面积。"""
        a = self.p1.distance_to(self.p2)
        b = self.p2.distance_to(self.p3)
        c = self.p3.distance_to(self.p1)
        s = (a + b + c) / 2
        return (s * (s - a) * (s - b) * (s - c)) ** 0.5

    def perimeter(self) -> float:
        a = self.p1.distance_to(self.p2)
        b = self.p2.distance_to(self.p3)
        c = self.p3.distance_to(self.p1)
        return a + b + c

    def contains_point(self, point: Point) -> bool:
        """简化实现：总是返回 False。"""
        return False

    def __str__(self) -> str:
        return f"Triangle[{self.p1}, {self.p2}, {self.p3}]"


# =============================================================================
# 10.8 元类（metaclass）—— 类的"类"
# =============================================================================

def demo_metaclasses() -> None:
    """演示元类基础：理解 type 创建类的机制及 metaclass 的应用。"""
    print("\n" + "=" * 60)
    print("10.8 元类（metaclass）—— 类的工厂")
    print("=" * 60)

    # ── 概念：元类是什么 ─────────────────────────────────────
    print("概念：")
    print("  类（class）是创建对象的模板")
    print("  元类（metaclass）是创建类的模板——即'类的类'")
    print("  Python 中默认的元类是 type")
    print("  类比 C++ 的模板元编程（TMP），但元类在运行时而非编译期工作")
    print()

    # ── type 的两种用法 ──────────────────────────────────────
    print("1. type 的两种用法：")

    # 用法一：获取对象的类型
    print(f"  type(42) = {type(42)}     (获取对象的类型)")
    print(f"  type(int) = {type(int)}   (类的类型是 type！)")

    # 用法二：动态创建类
    print(f"\n2. type(name, bases, namespace) —— 动态创建类：")

    # 等价于: class Dog(Animal): species = "Canine"; def bark(self): ...
    Dog = type("Dog", (), {
        "species": "Canine",
        "bark": lambda self: f"{self.name} says Woof!",
        "__init__": lambda self, name: setattr(self, "name", name),
    })

    d = Dog("Buddy")
    print(f"  Dog 类由 type 动态创建: {Dog}")
    print(f"  d = Dog('Buddy')")
    print(f"  d.bark()  = {d.bark()!r}")
    print(f"  d.species = {d.species!r}")

    # ── 自定义元类 ───────────────────────────────────────────
    print(f"\n3. 自定义元类 —— 拦截类的创建过程：")

    class LoggingMeta(type):
        """元类：在类创建时自动记录日志，并可注入属性。"""

        def __new__(mcs, name, bases, namespace):
            print(f"    [LoggingMeta] 正在创建类: {name}")
            print(f"      基类: {[b.__name__ for b in bases]}")
            # 可以在类创建前修改 namespace
            namespace["created_by"] = "LoggingMeta"
            # 自动为所有方法添加调用计数
            for attr_name, attr_value in namespace.items():
                if callable(attr_value) and not attr_name.startswith("__"):
                    namespace[attr_name] = mcs._wrap_with_log(attr_name, attr_value)
            return super().__new__(mcs, name, bases, namespace)

        @staticmethod
        def _wrap_with_log(name, func):
            def wrapper(self, *args, **kwargs):
                print(f"    → 调用 {name}({args}, {kwargs})")
                result = func(self, *args, **kwargs)
                print(f"    ← {name} 返回 {result!r}")
                return result
            return wrapper

    class Service(metaclass=LoggingMeta):
        """业务类——其创建过程被 LoggingMeta 拦截。"""

        def process(self, data: str) -> str:
            return data.upper()

        def status(self) -> str:
            return "ok"

    print(f"  Service.created_by = {Service.created_by!r}  (元类注入的属性)")
    print()
    svc = Service()
    print(f"  svc.process('hello') = {svc.process('hello')!r}")
    print(f"  svc.status() = {svc.status()!r}")

    # ── 常见应用：注册表模式 ─────────────────────────────────
    print(f"\n4. 实战：用元类实现自动注册表：")

    class PluginRegistry(type):
        """自动将所有子类注册到 _plugins 字典中。"""
        _plugins: dict[str, type] = {}

        def __new__(mcs, name, bases, namespace):
            cls = super().__new__(mcs, name, bases, namespace)
            if bases:  # 不注册基类本身
                mcs._plugins[name.lower()] = cls
            return cls

        @classmethod
        def list_plugins(mcs):
            return list(mcs._plugins.keys())

    class Plugin(metaclass=PluginRegistry):
        """插件基类——所有具体插件自动注册。"""
        def run(self):
            raise NotImplementedError

    class EmailPlugin(Plugin):
        def run(self):
            return "发送邮件通知"

    class SMSPlugin(Plugin):
        def run(self):
            return "发送短信通知"

    class WebhookPlugin(Plugin):
        def run(self):
            return "触发 Webhook"

    print(f"  已注册的插件: {PluginRegistry.list_plugins()}")
    for name, cls in PluginRegistry._plugins.items():
        instance = cls()
        print(f"    {name}: {instance.run()}")

    # ── __init_subclass__：更简单的替代方案 ──────────────────
    print(f"\n5. __init_subclass__ —— 元类的轻量替代（Python 3.6+）：")

    class Base:
        _subclasses: list[type] = []

        def __init_subclass__(cls, **kwargs):
            """当子类被定义时自动调用——不需要元类！"""
            super().__init_subclass__(**kwargs)
            Base._subclasses.append(cls)
            print(f"    自动注册子类: {cls.__name__}")

    class HandlerA(Base):
        pass

    class HandlerB(Base):
        pass

    print(f"  所有子类: {[c.__name__ for c in Base._subclasses]}")

    # ── 何时使用元类 ─────────────────────────────────────────
    print(f"\n── 使用指南 ──")
    print("  元类 vs __init_subclass__：")
    print("    大多数'拦截子类创建'的需求用 __init_subclass__ 就够了")
    print("    元类用于需要修改类本身的行为（方法注入、属性校验等）")
    print()
    print("  使用场景：")
    print("    ✅ 自动注册子类（ORM 的 Model、插件系统）")
    print("    ✅ 自动为方法添加日志/计时/权限检查")
    print("    ✅ 校验类定义是否完整（如必须实现某些方法）")
    print("    ✅ 自动生成 API 文档（扫描类的结构）")
    print("    ❌ 90% 场景不需要元类——先考虑装饰器和 __init_subclass__")


def demo_comprehensive() -> None:
    """综合示例。"""
    print("\n" + "=" * 60)
    print("10.9 综合示例：图形类层次结构")
    print("=" * 60)

    triangle = Triangle(
        Point(0, 0),
        Point(4, 0),
        Point(2, 3)
    )

    print(f"三角形: {triangle}")
    print(f"  面积: {triangle.area():.2f}")
    print(f"  周长: {triangle.perimeter():.2f}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_inheritance()
    demo_polymorphism()
    demo_super_and_mro()
    demo_magic_methods()
    demo_abc()
    demo_context_manager()
    demo_dataclasses()
    demo_metaclasses()
    demo_comprehensive()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 继承 ──
# class Child(Parent):
#     def __init__(self, ...):
#         super().__init__(...)  # 调用父类构造函数
#
# ── 多继承 ──
# class Child(Parent1, Parent2):
#     pass
#
# ── 抽象基类 ──
# from abc import ABC, abstractmethod
# class MyABC(ABC):
#     @abstractmethod
#     def method(self):
#         pass
#
# ── 常用魔术方法 ──
# __init__      构造函数
# __str__       str() 调用
# __repr__      repr() 调用
# __len__       len() 调用
# __getitem__   obj[key] 读取
# __setitem__   obj[key] = value 写入
# __contains__  item in obj
# __iter__      for item in obj
# __add__       obj1 + obj2
# __eq__        obj1 == obj2
# __lt__        obj1 < obj2
# __bool__      bool(obj)
#
# ── 上下文管理器 ──
# class MyContext:
#     def __enter__(self):
#         return self
#     def __exit__(self, exc_type, exc_val, exc_tb):
#         return False
#
# with MyContext() as ctx:
#     ...
#
# ── dataclasses ──
# from dataclasses import dataclass, field
# @dataclass
# class Data:
#     name: str
#     value: int = 0                # 带默认值的字段
#     items: list[str] = field(default_factory=list)  # 可变默认值
#     _secret: str = field(repr=False)                # 不参与 __repr__
#     _tag: str = field(compare=False)                # 不参与 __eq__
#
# @dataclass(frozen=True)
# class Immutable:
#     x: int
#     y: int
#
# # 初始化后验证
# def __post_init__(self):
#     if self.value < 0:
#         raise ValueError(...)
#
# ── 元类（metaclass）──
# # 方式一：type(name, bases, namespace)  —— 动态创建类
# # 方式二：class MyMeta(type): ...  —— 自定义元类
# # 方式三：__init_subclass__  —— 元类的轻量替代（Python 3.6+）
#
# class MyMeta(type):
#     def __new__(mcs, name, bases, ns):
#         # 在类创建时修改 namespace
#         return super().__new__(mcs, name, bases, ns)
#
# class MyClass(metaclass=MyMeta):
#     pass
#
# class Base:
#     def __init_subclass__(cls, **kwargs):
#         # 子类定义时自动调用，不需要元类
#         super().__init_subclass__(**kwargs)


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：忘记调用 super().__init__()
# class Child(Parent):
#     def __init__(self):
#         # ❌ 忘记调用 super().__init__()
#         self.x = 1
#
# 错误 2：多继承的钻石问题
# 使用 super() 而非直接调用父类方法，让 MRO 正确处理
#
# 错误 3：魔术方法应返回 NotImplemented 而非 raise
# def __eq__(self, other):
#     if not isinstance(other, MyClass):
#         return NotImplemented  # ✅ 正确
#         # raise TypeError(...)  # ❌ 不推荐
#
# 错误 4：忘记实现抽象方法
# class ConcreteShape(Shape):
#     def area(self):
#         return 1.0
#     # ❌ 忘记实现 perimeter，实例化时会报错


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   创建 Employee（员工）基类和 Manager（经理）、Developer（开发者）子类：
#   - Employee: name, salary, 以及 get_info() 方法
#   - Manager: 额外有 department 属性、bonus 属性，重写 get_info()
#   - Developer: 额外有 language 属性，重写 get_info()
#   - 演示多态：用一个列表存放所有员工，统一调用 get_info()
#
# 练习 2：
#   创建一个 Matrix（矩阵）类，实现：
#   - __add__：矩阵加法
#   - __mul__：矩阵乘法
#   - __getitem__：matrix[i][j] 访问
#   - __str__：格式化打印矩阵
#
# 练习 3：
#   用 @contextmanager（from contextlib）实现一个计时器：
#   from contextlib import contextmanager
#   @contextmanager
#   def timer(name):
#       # 记录开始时间
#       yield
#       # 打印耗时
#
# 练习答案提示：
#   练习1：注意 super().__init__() 的调用，多态通过统一的 for 循环体现
#   练习2：Matrix 内部用二维列表存储，__getitem__ 返回行列表
#   练习3：contextlib.contextmanager 让普通函数变成上下文管理器


# =============================================================================
# 【练习答案】
# =============================================================================

# ── 练习 1：Employee 类层次结构 ─────────────────────────────────────────────

class Employee:
    """练习 1：员工基类。"""

    def __init__(self, name: str, salary: float) -> None:
        self.name = name
        self.salary = salary

    def get_info(self) -> str:
        return f"员工: {self.name}，薪资: ¥{self.salary:,.2f}"

    def __str__(self) -> str:
        return self.get_info()


class Manager(Employee):
    """经理：继承 Employee，额外有 department 和 bonus 属性。"""

    def __init__(
        self, name: str, salary: float, department: str, bonus: float = 0.0
    ) -> None:
        super().__init__(name, salary)
        self.department = department
        self.bonus = bonus

    def get_info(self) -> str:
        total = self.salary + self.bonus
        return (f"经理: {self.name}，部门: {self.department}，"
                f"薪资: ¥{self.salary:,.2f}，奖金: ¥{self.bonus:,.2f}，"
                f"合计: ¥{total:,.2f}")


class Developer(Employee):
    """开发者：继承 Employee，额外有 language 属性。"""

    def __init__(self, name: str, salary: float, language: str) -> None:
        super().__init__(name, salary)
        self.language = language

    def get_info(self) -> str:
        return (f"开发者: {self.name}，主要语言: {self.language}，"
                f"薪资: ¥{self.salary:,.2f}")


# ── 练习 2：Matrix 矩阵类 ────────────────────────────────────────────────────

class Matrix:
    """练习 2：矩阵类（支持加法、乘法、下标访问和格式化打印）。"""

    def __init__(self, data: list[list[float]]) -> None:
        if not data or not data[0]:
            raise ValueError("矩阵不能为空")
        self._data = [row[:] for row in data]  # 深拷贝，防止外部修改
        self.rows = len(data)
        self.cols = len(data[0])

    def __add__(self, other: "Matrix") -> "Matrix":
        """矩阵加法：对应元素相加，要求维度相同。"""
        if self.rows != other.rows or self.cols != other.cols:
            raise ValueError(
                f"矩阵维度不匹配：({self.rows}×{self.cols}) vs "
                f"({other.rows}×{other.cols})"
            )
        result = [
            [self._data[i][j] + other._data[i][j] for j in range(self.cols)]
            for i in range(self.rows)
        ]
        return Matrix(result)

    def __mul__(self, other: "Matrix") -> "Matrix":
        """矩阵乘法：self 的列数必须等于 other 的行数。"""
        if self.cols != other.rows:
            raise ValueError(
                f"矩阵维度不匹配：({self.rows}×{self.cols}) × "
                f"({other.rows}×{other.cols})"
            )
        result = [
            [
                sum(self._data[i][k] * other._data[k][j] for k in range(self.cols))
                for j in range(other.cols)
            ]
            for i in range(self.rows)
        ]
        return Matrix(result)

    def __getitem__(self, index: int) -> list[float]:
        """支持 matrix[i] 访问第 i 行，再用 matrix[i][j] 访问元素。"""
        return self._data[index]

    def __str__(self) -> str:
        """格式化打印矩阵。"""
        # 计算每列最大宽度，使列对齐
        col_widths = [
            max(len(f"{self._data[i][j]:.1f}") for i in range(self.rows))
            for j in range(self.cols)
        ]
        lines = []
        for row in self._data:
            row_str = "  [ " + "  ".join(
                f"{v:{w}.1f}" for v, w in zip(row, col_widths)
            ) + " ]"
            lines.append(row_str)
        return "\n".join(lines)

    def __repr__(self) -> str:
        return f"Matrix({self._data!r})"


# ── 练习 3：@contextmanager 计时器 ──────────────────────────────────────────

from contextlib import contextmanager
import time as _time


@contextmanager
def timed(name: str = "操作"):
    """
    练习 3：用 @contextmanager 实现的计时器上下文管理器。

    用法：
        with timed("数据处理"):
            do_something()
    """
    start = _time.perf_counter()
    print(f"  ⏱️  开始 {name}...")
    try:
        yield  # with 块中的代码在此处执行
    finally:
        elapsed = _time.perf_counter() - start
        print(f"  ✅ {name} 完成，耗时: {elapsed:.4f} 秒")


def exercise1_answer() -> None:
    """练习 1：演示 Employee 类层次与多态。"""
    print("Employee 多态演示:")

    # 用列表统一存放所有员工（多态）
    staff: list[Employee] = [
        Employee("普通员工甲", 8000),
        Manager("王经理", 20000, "研发部", bonus=5000),
        Developer("张开发", 18000, "Python"),
        Manager("李总监", 30000, "市场部", bonus=10000),
        Developer("陈工程师", 22000, "Go"),
    ]

    print(f"  {'─' * 60}")
    for person in staff:
        print(f"  {person.get_info()}")  # 多态：调用各自的 get_info()
    print(f"  {'─' * 60}")

    # 统计各类员工薪资总和
    total_salary = sum(e.salary for e in staff)
    print(f"  薪资总计: ¥{total_salary:,.2f}")

    managers = [e for e in staff if isinstance(e, Manager)]
    print(f"  经理人数: {len(managers)}")


def exercise2_answer() -> None:
    """练习 2：演示 Matrix 类。"""
    print("Matrix 演示:")

    m1 = Matrix([[1, 2, 3], [4, 5, 6]])        # 2×3
    m2 = Matrix([[7, 8, 9], [10, 11, 12]])      # 2×3
    m3 = Matrix([[1, 0], [0, 1], [2, 3]])       # 3×2

    print("  m1 =")
    print(m1)
    print("  m2 =")
    print(m2)

    print("  m1 + m2 =")
    print(m1 + m2)

    print("  m1 × m3 (2×3 × 3×2 = 2×2) =")
    print(m1 * m3)

    print(f"  m1[0] = {m1[0]}")
    print(f"  m1[1][2] = {m1[1][2]}")

    # 维度不匹配时抛出错误
    try:
        _ = m1 * m2   # 3 != 2，不能相乘
    except ValueError as e:
        print(f"  ✓ 维度错误: {e}")


def exercise3_answer() -> None:
    """练习 3：演示 @contextmanager 计时器。"""
    print("timed 上下文管理器演示:")

    with timed("列表求和"):
        result = sum(range(1_000_000))
        print(f"    结果: {result}")

    with timed("排序"):
        import random
        data = [random.randint(0, 1000) for _ in range(10_000)]
        data.sort()
        print(f"    排序完成，首个元素: {data[0]}")

    # 演示异常时 finally 仍然执行（计时器仍然打印）
    print("  异常场景（计时器的 finally 仍会执行）:")
    try:
        with timed("会出错的操作"):
            raise RuntimeError("模拟错误")
    except RuntimeError as e:
        print(f"    捕获异常: {e}")


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 60)
#     exercise1_answer()
#
#     print("=" * 60)
#     exercise2_answer()
#
#     print("=" * 60)
#     exercise3_answer()
