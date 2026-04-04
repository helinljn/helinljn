# =============================================================================
# 第 9 章：面向对象编程基础（OOP）
# =============================================================================
#
# 【学习目标】
#   1. 理解类和对象的概念
#   2. 掌握 __init__ 构造函数
#   3. 区分实例属性、类属性
#   4. 区分实例方法、类方法、静态方法
#   5. 理解封装和访问控制
#   6. 掌握 @property 属性装饰器
#
# 【与 C++ 的对比】
#   C++:  class MyClass { public: int x; void method() {} };
#   Python: class MyClass: x = 0; def method(self): ...
#   主要区别：Python 方法第一个参数是 self（相当于 this），必须显式声明
#
# 【运行方式】
#   python chapter09_面向对象基础.py
#
# =============================================================================


# =============================================================================
# 9.1 类和对象
# =============================================================================

class Dog:
    """
    一只狗的类 —— 最简单的类示例。

    类（Class）是对象的"蓝图"或"模板"。
    对象（Object）是类的具体"实例"（Instance）。
    """

    # ── 类属性（所有实例共享）────────────────────────────
    species = "Canis familiaris"  # 所有狗都属于这个物种
    count = 0                     # 记录创建了多少只狗

    # ── 构造函数 ─────────────────────────────────────────
    def __init__(self, name: str, age: int, breed: str) -> None:
        """
        构造函数：创建对象时自动调用。
        self 是指向当前对象的引用（类似 C++ 的 this 指针）。

        Args:
            name: 狗的名字
            age:  狗的年龄
            breed: 品种
        """
        # 实例属性（每个对象独立拥有）
        self.name = name    # 等价于 C++ 的 this->name = name
        self.age = age
        self.breed = breed
        Dog.count += 1      # 修改类属性，记录总数

    # ── 实例方法 ─────────────────────────────────────────
    def bark(self) -> str:
        """实例方法：必须有 self 参数。"""
        return f"{self.name}: 汪汪汪！"

    def describe(self) -> str:
        """描述这只狗的信息。"""
        return f"{self.name} 是一只 {self.age} 岁的 {self.breed}"

    def birthday(self) -> None:
        """过生日，年龄加一。"""
        self.age += 1
        print(f"🎂 {self.name} 过生日了！现在 {self.age} 岁。")

    # ── 类方法 ───────────────────────────────────────────
    @classmethod
    def get_count(cls) -> int:
        """
        类方法：第一个参数是 cls（类本身，而非实例）。
        通常用于操作类属性，或提供替代构造函数。
        """
        return cls.count

    @classmethod
    def from_dict(cls, data: dict) -> "Dog":
        """
        替代构造函数（工厂方法）：从字典创建 Dog 对象。
        这是类方法的典型用法。
        """
        return cls(data["name"], data["age"], data["breed"])

    # ── 静态方法 ─────────────────────────────────────────
    @staticmethod
    def is_adult(age: int) -> bool:
        """
        静态方法：既没有 self 也没有 cls。
        不依赖实例或类，只是逻辑上属于这个类的函数。
        """
        return age >= 2  # 狗超过2岁算成年

    # ── 字符串表示 ───────────────────────────────────────
    def __str__(self) -> str:
        """
        str() 或 print() 时调用。
        应该返回对用户友好的字符串。
        """
        return f"Dog(name={self.name!r}, age={self.age}, breed={self.breed!r})"

    def __repr__(self) -> str:
        """
        repr() 时调用，或在交互式终端显示对象时。
        应该返回能重建对象的字符串（如果可能）。
        """
        return f"Dog({self.name!r}, {self.age}, {self.breed!r})"


def demo_class_basics() -> None:
    """演示类和对象的基本用法。"""
    print("=" * 60)
    print("9.1 类和对象基础")
    print("=" * 60)

    # ── 创建对象（实例化）────────────────────────────────
    dog1 = Dog("小白", 3, "金毛")    # 调用 __init__
    dog2 = Dog("大黄", 5, "土狗")
    dog3 = Dog.from_dict({"name": "球球", "age": 1, "breed": "泰迪"})

    print(f"创建了 {Dog.get_count()} 只狗")

    # ── 访问属性和方法 ───────────────────────────────────
    print(f"\n实例属性:")
    print(f"  dog1.name = {dog1.name!r}")
    print(f"  dog1.age  = {dog1.age}")
    print(f"  dog2.breed = {dog2.breed!r}")

    print(f"\n类属性（所有实例共享）:")
    print(f"  Dog.species = {Dog.species!r}")
    print(f"  dog1.species = {dog1.species!r}")  # 也可以通过实例访问

    # ── 调用方法 ─────────────────────────────────────────
    print(f"\n调用实例方法:")
    print(f"  {dog1.bark()}")
    print(f"  {dog2.describe()}")
    dog1.birthday()

    # ── 静态方法 ─────────────────────────────────────────
    print(f"\n静态方法:")
    print(f"  Dog.is_adult(1) = {Dog.is_adult(1)}")
    print(f"  Dog.is_adult(3) = {Dog.is_adult(3)}")

    # ── 字符串表示 ───────────────────────────────────────
    print(f"\n字符串表示:")
    print(f"  str(dog1)  = {str(dog1)}")
    print(f"  repr(dog3) = {repr(dog3)}")

    # ── 动态属性 ─────────────────────────────────────────
    # Python 允许在运行时给对象添加属性（C++ 中不能这样）
    dog1.color = "白色"  # 动态添加属性
    print(f"\n动态属性: dog1.color = {dog1.color!r}")

    # ── 检查类型 ─────────────────────────────────────────
    print(f"\n类型检查:")
    print(f"  type(dog1) = {type(dog1)}")
    print(f"  isinstance(dog1, Dog) = {isinstance(dog1, Dog)}")
    print(f"  hasattr(dog1, 'color') = {hasattr(dog1, 'color')}")
    print(f"  hasattr(dog2, 'color') = {hasattr(dog2, 'color')}")


# =============================================================================
# 9.2 封装与访问控制
# =============================================================================

class BankAccount:
    """
    银行账户 —— 演示封装和访问控制。

    Python 的访问控制约定（非强制）：
      - 普通属性: public，可自由访问
      - _xxx: protected，约定只在类内部和子类中使用
      - __xxx: private，名称重整，外部很难直接访问
    """

    def __init__(self, owner: str, initial_balance: float = 0.0) -> None:
        self.owner = owner               # 公开属性
        self._account_id = id(self)      # 保护属性（约定不从外部访问）
        self.__balance = initial_balance  # 私有属性（名称被重整为 _BankAccount__balance）
        self.__transactions: list[dict] = []

    def deposit(self, amount: float) -> None:
        """存款。"""
        if amount <= 0:
            raise ValueError(f"存款金额必须为正数，得到: {amount}")
        self.__balance += amount
        self.__log_transaction("存款", amount)
        print(f"  💰 存款 ¥{amount:.2f}，余额: ¥{self.__balance:.2f}")

    def withdraw(self, amount: float) -> None:
        """取款。"""
        if amount <= 0:
            raise ValueError(f"取款金额必须为正数，得到: {amount}")
        if amount > self.__balance:
            raise ValueError(f"余额不足！余额: ¥{self.__balance:.2f}，取款: ¥{amount:.2f}")
        self.__balance -= amount
        self.__log_transaction("取款", -amount)
        print(f"  💸 取款 ¥{amount:.2f}，余额: ¥{self.__balance:.2f}")

    def get_balance(self) -> float:
        """获取余额（通过方法访问私有属性）。"""
        return self.__balance

    def get_statement(self) -> list[dict]:
        """获取交易记录。"""
        return list(self.__transactions)  # 返回副本，防止外部修改

    def __log_transaction(self, kind: str, amount: float) -> None:
        """私有方法：记录交易（外部不应调用）。"""
        self.__transactions.append({
            "type": kind,
            "amount": amount,
            "balance_after": self.__balance,
        })

    def __str__(self) -> str:
        return f"BankAccount({self.owner!r}, ¥{self.__balance:.2f})"


def demo_encapsulation() -> None:
    """演示封装和访问控制。"""
    print("\n" + "=" * 60)
    print("9.2 封装与访问控制")
    print("=" * 60)

    account = BankAccount("张三", 1000.0)
    print(f"账户: {account}")

    account.deposit(500)
    account.deposit(200)
    account.withdraw(300)

    print(f"\n当前余额: ¥{account.get_balance():.2f}")
    print("交易记录:")
    for tx in account.get_statement():
        print(f"  {tx}")

    # 演示私有属性的名称重整
    print(f"\n私有属性名称重整:")
    print(f"  account.__dict__ 中的键:")
    for key in account.__dict__:
        print(f"    {key}")
    # 名称重整后变成 _BankAccount__balance
    print(f"  直接访问（不推荐）: {account._BankAccount__balance:.2f}")

    # 错误操作演示
    print("\n错误操作演示:")
    try:
        account.withdraw(10000)
    except ValueError as e:
        print(f"  ✓ 捕获错误: {e}")


# =============================================================================
# 9.3 @property 属性装饰器
# =============================================================================

class Temperature:
    """
    温度类 —— 演示 @property 装饰器。

    @property 让你用"访问属性"的语法来调用方法，
    实现对属性的精细控制（getter/setter/deleter）。
    类似 C++ 的 getter/setter，但语法更优雅。
    """

    def __init__(self, celsius: float = 0.0) -> None:
        # 用 _celsius 存储真实值（保护属性）
        self._celsius = celsius

    @property
    def celsius(self) -> float:
        """Getter：获取摄氏温度。"""
        return self._celsius

    @celsius.setter
    def celsius(self, value: float) -> None:
        """Setter：设置摄氏温度，带验证。"""
        if value < -273.15:
            raise ValueError(f"温度不能低于绝对零度 -273.15°C，得到: {value}")
        self._celsius = value

    @property
    def fahrenheit(self) -> float:
        """只读属性：返回华氏温度（从摄氏计算）。"""
        return self._celsius * 9 / 5 + 32

    @fahrenheit.setter
    def fahrenheit(self, value: float) -> None:
        """通过设置华氏温度来更新摄氏温度。"""
        self.celsius = (value - 32) * 5 / 9  # 调用 celsius.setter（含验证）

    @property
    def kelvin(self) -> float:
        """只读属性：返回开氏温度。"""
        return self._celsius + 273.15

    def __str__(self) -> str:
        return (f"{self._celsius:.1f}°C = "
                f"{self.fahrenheit:.1f}°F = "
                f"{self.kelvin:.2f}K")


class Circle:
    """圆 —— 展示 @property 计算属性。"""

    import math as _math

    def __init__(self, radius: float) -> None:
        self.radius = radius  # 触发 setter

    @property
    def radius(self) -> float:
        return self._radius

    @radius.setter
    def radius(self, value: float) -> None:
        if value < 0:
            raise ValueError(f"半径不能为负，得到: {value}")
        self._radius = value

    @property
    def diameter(self) -> float:
        """直径（计算属性）。"""
        return self._radius * 2

    @property
    def area(self) -> float:
        """面积（计算属性）。"""
        import math
        return math.pi * self._radius ** 2

    @property
    def circumference(self) -> float:
        """周长（计算属性）。"""
        import math
        return 2 * math.pi * self._radius

    def __str__(self) -> str:
        return (f"Circle(r={self._radius:.2f}, "
                f"area={self.area:.2f}, "
                f"circumference={self.circumference:.2f})")


def demo_property() -> None:
    """演示 @property 装饰器。"""
    print("\n" + "=" * 60)
    print("9.3 @property 属性装饰器")
    print("=" * 60)

    # ── Temperature ──────────────────────────────────────
    print("Temperature 示例:")
    temp = Temperature(100)
    print(f"  沸点: {temp}")

    temp.celsius = 0
    print(f"  冰点: {temp}")

    temp.fahrenheit = 98.6  # 用华氏度设置
    print(f"  体温: {temp}")

    try:
        temp.celsius = -300
    except ValueError as e:
        print(f"  ✓ 错误被拦截: {e}")

    # ── Circle ───────────────────────────────────────────
    print("\nCircle 示例:")
    c = Circle(5)
    print(f"  {c}")
    c.radius = 10  # 触发 setter
    print(f"  {c}")
    print(f"  c.diameter = {c.diameter}")

    try:
        c.radius = -1
    except ValueError as e:
        print(f"  ✓ 错误被拦截: {e}")


# =============================================================================
# 9.4 综合示例：学生成绩管理系统
# =============================================================================

class Student:
    """学生类。"""

    def __init__(self, name: str, student_id: str, grade: int) -> None:
        self.name = name
        self.student_id = student_id
        self.grade = grade  # 年级
        self._scores: dict[str, float] = {}

    def add_score(self, subject: str, score: float) -> None:
        """添加或更新科目成绩。"""
        if not 0 <= score <= 100:
            raise ValueError(f"成绩必须在 0-100 之间，得到: {score}")
        self._scores[subject] = score

    @property
    def average(self) -> float:
        """平均分（计算属性）。"""
        if not self._scores:
            return 0.0
        return sum(self._scores.values()) / len(self._scores)

    @property
    def grade_letter(self) -> str:
        """等级（计算属性）。"""
        avg = self.average
        if avg >= 90:
            return "A"
        elif avg >= 80:
            return "B"
        elif avg >= 70:
            return "C"
        elif avg >= 60:
            return "D"
        return "F"

    def report(self) -> str:
        """生成成绩报告。"""
        lines = [f"学生: {self.name} ({self.student_id}) - {self.grade}年级"]
        for subject, score in sorted(self._scores.items()):
            lines.append(f"  {subject}: {score:.1f}")
        lines.append(f"  平均分: {self.average:.1f} ({self.grade_letter})")
        return "\n".join(lines)

    def __str__(self) -> str:
        return f"Student({self.name!r}, avg={self.average:.1f})"

    def __repr__(self) -> str:
        return f"Student({self.name!r}, {self.student_id!r}, {self.grade})"

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, Student):
            return NotImplemented
        return self.student_id == other.student_id

    def __lt__(self, other: "Student") -> bool:
        return self.average < other.average


class Classroom:
    """班级类，管理多个学生。"""

    def __init__(self, name: str) -> None:
        self.name = name
        self._students: dict[str, Student] = {}

    def add_student(self, student: Student) -> None:
        """添加学生。"""
        if student.student_id in self._students:
            raise ValueError(f"学生 ID {student.student_id!r} 已存在")
        self._students[student.student_id] = student
        print(f"  ✓ 添加学生: {student.name}")

    def get_student(self, student_id: str) -> Student:
        """根据 ID 获取学生。"""
        if student_id not in self._students:
            raise KeyError(f"未找到 ID 为 {student_id!r} 的学生")
        return self._students[student_id]

    @property
    def student_count(self) -> int:
        return len(self._students)

    @property
    def class_average(self) -> float:
        """全班平均分。"""
        if not self._students:
            return 0.0
        return sum(s.average for s in self._students.values()) / self.student_count

    def top_students(self, n: int = 3) -> list[Student]:
        """获取成绩前 n 名学生。"""
        return sorted(self._students.values(), reverse=True)[:n]

    def failed_students(self) -> list[Student]:
        """获取不及格学生。"""
        return [s for s in self._students.values() if s.grade_letter == "F"]

    def print_report(self) -> None:
        """打印班级报告。"""
        print(f"\n{'='*50}")
        print(f"班级: {self.name} | 人数: {self.student_count}")
        print(f"全班平均分: {self.class_average:.1f}")
        print(f"{'='*50}")

        print("\n成绩排名（前3名）:")
        for i, s in enumerate(self.top_students(3), 1):
            print(f"  第{i}名: {s.name} - {s.average:.1f}分 ({s.grade_letter})")

        failed = self.failed_students()
        if failed:
            print(f"\n⚠️ 不及格学生 ({len(failed)}人):")
            for s in failed:
                print(f"  {s.name} - {s.average:.1f}分")
        else:
            print("\n✅ 全班同学均及格！")


def demo_student_system() -> None:
    """演示学生管理系统。"""
    print("\n" + "=" * 60)
    print("9.4 综合示例：学生成绩管理系统")
    print("=" * 60)

    # 创建班级
    classroom = Classroom("初三(2)班")

    # 创建学生并添加成绩
    students_data = [
        ("张小明", "S001", 9, {"语文": 88, "数学": 92, "英语": 85, "物理": 90}),
        ("李小红", "S002", 9, {"语文": 95, "数学": 87, "英语": 92, "物理": 88}),
        ("王小刚", "S003", 9, {"语文": 72, "数学": 65, "英语": 70, "物理": 68}),
        ("赵小芳", "S004", 9, {"语文": 55, "数学": 48, "英语": 60, "物理": 52}),
        ("陈小波", "S005", 9, {"语文": 80, "数学": 85, "英语": 78, "物理": 82}),
    ]

    print("添加学生:")
    for name, sid, grade, scores in students_data:
        s = Student(name, sid, grade)
        for subject, score in scores.items():
            s.add_score(subject, score)
        classroom.add_student(s)

    # 打印班级报告
    classroom.print_report()

    # 单个学生报告
    print(f"\n{'-'*50}")
    print("单个学生详细报告:")
    alice = classroom.get_student("S001")
    print(alice.report())


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_class_basics()
    demo_encapsulation()
    demo_property()
    demo_student_system()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 定义类 ──
# class MyClass:
#     class_var = "共享"            # 类属性
#
#     def __init__(self, x):        # 构造函数
#         self.x = x                # 实例属性
#         self._protected = 1       # 保护属性（约定）
#         self.__private = 2        # 私有属性（名称重整）
#
#     def method(self):             # 实例方法
#         return self.x
#
#     @classmethod
#     def class_method(cls):        # 类方法
#         return cls.class_var
#
#     @staticmethod
#     def static_method(x):         # 静态方法
#         return x * 2
#
#     @property
#     def prop(self):               # 只读属性
#         return self._prop
#
#     @prop.setter
#     def prop(self, value):        # 可写属性
#         self._prop = value
#
#     def __str__(self):            # str() 调用
#         return f"MyClass({self.x})"
#
#     def __repr__(self):           # repr() 调用
#         return f"MyClass({self.x!r})"
#
# ── 创建对象 ──
# obj = MyClass(42)
# obj.method()
# MyClass.class_method()
# MyClass.static_method(10)


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：忘记 self 参数
# class MyClass:
#     def method():     # ❌ 缺少 self
#         pass
#
# 错误 2：把类属性和实例属性混淆
# class Counter:
#     count = 0
#     def increment(self):
#         self.count += 1  # ⚠️ 这会创建实例属性，不会修改类属性
#                          # 正确：Counter.count += 1
#
# 错误 3：可变类属性（著名陷阱）
# class MyClass:
#     items = []          # ❌ 所有实例共享同一个列表
#
#     def __init__(self):
#         self.items = [] # ✅ 在 __init__ 中定义实例属性


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   创建一个 Rectangle（矩形）类，包含：
#   - width, height 属性（用 @property 验证必须为正数）
#   - area（面积）、perimeter（周长）只读属性
#   - is_square（是否为正方形）只读属性
#   - __str__ 方法
#
# 练习 2：
#   创建一个 Stack（栈）类，包含：
#   - push(item)：入栈
#   - pop()：出栈（空栈时抛出异常）
#   - peek()：查看栈顶
#   - is_empty：只读属性
#   - size：只读属性
#   - __str__、__len__ 方法
#
# 练习 3：
#   修改 BankAccount，添加：
#   - transfer(target_account, amount) 方法（向另一个账户转账）
#   - @classmethod create_joint_account(owner1, owner2, balance) 方法


# =============================================================================
# 【练习答案】
# =============================================================================

class Rectangle:
    """练习 1：矩形类（含 @property 验证和计算属性）。"""

    def __init__(self, width: float, height: float) -> None:
        # 通过 setter 触发验证
        self.width = width
        self.height = height

    @property
    def width(self) -> float:
        return self._width

    @width.setter
    def width(self, value: float) -> None:
        if value <= 0:
            raise ValueError(f"宽度必须为正数，得到: {value}")
        self._width = value

    @property
    def height(self) -> float:
        return self._height

    @height.setter
    def height(self, value: float) -> None:
        if value <= 0:
            raise ValueError(f"高度必须为正数，得到: {value}")
        self._height = value

    @property
    def area(self) -> float:
        """面积（只读计算属性）。"""
        return self._width * self._height

    @property
    def perimeter(self) -> float:
        """周长（只读计算属性）。"""
        return 2 * (self._width + self._height)

    @property
    def is_square(self) -> bool:
        """是否为正方形（只读计算属性）。"""
        return self._width == self._height

    def __str__(self) -> str:
        return (f"Rectangle(width={self._width}, height={self._height}, "
                f"area={self.area:.2f}, is_square={self.is_square})")


class Stack:
    """练习 2：栈（先进后出数据结构）。"""

    def __init__(self) -> None:
        self._items: list = []

    def push(self, item) -> None:
        """入栈：将元素压入栈顶。"""
        self._items.append(item)

    def pop(self):
        """出栈：移除并返回栈顶元素，空栈时抛出 IndexError。"""
        if self.is_empty:
            raise IndexError("pop from empty stack")
        return self._items.pop()

    def peek(self):
        """查看栈顶元素（不移除），空栈时抛出 IndexError。"""
        if self.is_empty:
            raise IndexError("peek from empty stack")
        return self._items[-1]

    @property
    def is_empty(self) -> bool:
        """栈是否为空（只读属性）。"""
        return len(self._items) == 0

    @property
    def size(self) -> int:
        """栈中元素数量（只读属性）。"""
        return len(self._items)

    def __len__(self) -> int:
        """支持 len(stack)。"""
        return len(self._items)

    def __str__(self) -> str:
        """栈顶在右侧显示。"""
        return f"Stack({self._items})"

    def __repr__(self) -> str:
        return f"Stack({self._items!r})"


class ExtendedBankAccount(BankAccount):
    """
    练习 3：扩展 BankAccount，添加 transfer 和 create_joint_account。
    继承自文件中已定义的 BankAccount 类。
    """

    def transfer(self, target: "ExtendedBankAccount", amount: float) -> None:
        """
        向另一个账户转账。
        先从本账户取款（含余额验证），再存入目标账户。
        """
        if amount <= 0:
            raise ValueError(f"转账金额必须为正数，得到: {amount}")
        self.withdraw(amount)   # 复用已有的 withdraw（含验证）
        target.deposit(amount)  # 复用已有的 deposit
        print(f"  ↔️  转账 ¥{amount:.2f}：{self.owner} → {target.owner}")

    @classmethod
    def create_joint_account(
        cls,
        owner1: str,
        owner2: str,
        initial_balance: float = 0.0,
    ) -> "ExtendedBankAccount":
        """
        工厂方法：创建联名账户。
        联名账户的 owner 显示为 "owner1 & owner2"。
        """
        joint_name = f"{owner1} & {owner2}"
        return cls(joint_name, initial_balance)


def exercise1_answer() -> None:
    """练习 1：演示 Rectangle 类。"""
    print("Rectangle 演示:")
    r = Rectangle(5, 3)
    print(f"  {r}")
    print(f"  r.area = {r.area}, r.perimeter = {r.perimeter}")

    r.width = 4
    print(f"  修改宽度为4: is_square = {r.is_square}")

    sq = Rectangle(6, 6)
    print(f"  {sq}")

    try:
        Rectangle(-1, 3)
    except ValueError as e:
        print(f"  ✓ 错误被拦截: {e}")


def exercise2_answer() -> None:
    """练习 2：演示 Stack 类。"""
    print("Stack 演示:")
    s = Stack()
    print(f"  初始: {s}, is_empty={s.is_empty}")

    for item in [10, 20, 30, 40]:
        s.push(item)
    print(f"  push 4个元素后: {s}, size={s.size}, len={len(s)}")
    print(f"  peek() = {s.peek()}")
    print(f"  pop() = {s.pop()}, 之后: {s}")

    try:
        empty_s = Stack()
        empty_s.pop()
    except IndexError as e:
        print(f"  ✓ 空栈 pop 抛出: {e}")


def exercise3_answer() -> None:
    """练习 3：演示 ExtendedBankAccount 的 transfer 和 create_joint_account。"""
    print("ExtendedBankAccount 演示:")

    alice = ExtendedBankAccount("Alice", 1000.0)
    bob = ExtendedBankAccount("Bob", 500.0)
    print(f"  {alice}")
    print(f"  {bob}")

    alice.transfer(bob, 300.0)
    print(f"  转账后 Alice: ¥{alice.get_balance():.2f}")
    print(f"  转账后 Bob:   ¥{bob.get_balance():.2f}")

    # 余额不足的转账
    try:
        bob.transfer(alice, 10000.0)
    except ValueError as e:
        print(f"  ✓ 余额不足: {e}")

    # 联名账户
    joint = ExtendedBankAccount.create_joint_account("张三", "李四", 2000.0)
    print(f"\n  联名账户: {joint}")
    joint.deposit(500.0)
    print(f"  存款后余额: ¥{joint.get_balance():.2f}")


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
