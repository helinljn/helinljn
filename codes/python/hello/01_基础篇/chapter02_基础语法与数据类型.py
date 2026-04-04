# =============================================================================
# 第 2 章：基础语法与数据类型
# =============================================================================
#
# 【学习目标】
#   1. 掌握 Python 变量的声明与使用方式
#   2. 理解 Python 的 4 种基本数据类型：int、float、bool、None
#   3. 掌握类型转换和类型检查
#   4. 熟练使用各种运算符
#   5. 掌握 input() 和 print() 的常见用法
#
# 【运行方式】
#   python chapter02_基础语法与数据类型.py
#
# =============================================================================

import math  # 导入数学模块（放在文件顶部，符合 PEP 8）


# =============================================================================
# 2.1 变量与动态类型
# =============================================================================
#
# C/C++ 中：变量必须先声明类型
#   int x = 10;       float y = 3.14;
#
# Python 中：直接赋值即可，类型由值自动决定（动态类型）
#   x = 10            y = 3.14
#
# 变量命名规则：
#   1. 只能包含字母、数字、下划线
#   2. 不能以数字开头
#   3. 区分大小写（name 和 Name 是不同的变量）
#   4. 不能使用 Python 关键字（if、for、class 等）

def demo_variables() -> None:
    """演示变量与动态类型。"""
    print("=" * 60)
    print("2.1 变量与动态类型")
    print("=" * 60)

    # 动态类型：同一个变量可以先后保存不同类型的值
    x = 10
    print(f"x = {x}, 类型: {type(x)}")   # <class 'int'>
    x = "hello"                            # 重新赋值为字符串
    print(f"x = {x}, 类型: {type(x)}")   # <class 'str'>
    x = 3.14                               # 重新赋值为浮点数
    print(f"x = {x}, 类型: {type(x)}")   # <class 'float'>

    # 多重赋值：同时给多个变量赋相同的值
    a = b = c = 0
    print(f"\na={a}, b={b}, c={c}")

    # 元组解包：同时给多个变量赋不同的值
    x, y, z = 1, 2, 3
    print(f"x={x}, y={y}, z={z}")

    # 交换变量（Python 特有，无需临时变量）
    a, b = 10, 20
    print(f"\n交换前: a={a}, b={b}")
    a, b = b, a
    print(f"交换后: a={a}, b={b}")

    # 带星号的解包（Python 3+）
    first, *middle, last = [1, 2, 3, 4, 5]
    print(f"\n解包: first={first}, middle={middle}, last={last}")
    # first=1, middle=[2, 3, 4], last=5


# =============================================================================
# 2.2 基本数据类型
# =============================================================================

def demo_int() -> None:
    """演示整数类型（int）。"""
    print("\n" + "─" * 40)
    print("2.2.1 整数类型（int）")
    print("─" * 40)

    # Python 的 int 可以表示任意大的整数，没有溢出问题
    # C/C++ 需要区分 int / long / long long，Python 只有一种 int

    num_decimal = 42           # 十进制
    num_binary  = 0b1010       # 二进制（0b 前缀），值为 10
    num_octal   = 0o12         # 八进制（0o 前缀），值为 10
    num_hex     = 0xFF         # 十六进制（0x 前缀），值为 255

    print(f"十进制   42 = {num_decimal}")
    print(f"二进制 0b1010 = {num_binary}")
    print(f"八进制 0o12  = {num_octal}")
    print(f"十六进制 0xFF = {num_hex}")

    # Python int 无溢出
    huge = 123456789012345678901234567890
    print(f"\n超大整数: {huge}")
    print(f"它的平方: {huge ** 2}")

    # 下划线分隔数字（提高可读性，Python 3.6+）
    million = 1_000_000
    print(f"\n1_000_000 = {million:,}")  # :, 格式化为千分位显示

    # 内置进制转换函数
    n = 255
    print(f"\n255 的各种进制表示:")
    print(f"  bin(255) = {bin(n)}")      # '0b11111111'
    print(f"  oct(255) = {oct(n)}")      # '0o377'
    print(f"  hex(255) = {hex(n)}")      # '0xff'


def demo_float() -> None:
    """演示浮点数类型（float）。"""
    print("\n" + "─" * 40)
    print("2.2.2 浮点数类型（float）")
    print("─" * 40)

    # Python 的 float 等同于 C/C++ 的 double（64位双精度）
    pi = 3.14159
    scientific = 1.5e3     # 科学计数法：1.5 × 10³ = 1500.0
    small = 1.5e-3         # 1.5 × 10⁻³ = 0.0015

    print(f"π = {pi}")
    print(f"1.5e3  = {scientific}")
    print(f"1.5e-3 = {small}")

    # 特殊浮点值
    pos_inf = float("inf")    # 正无穷大
    neg_inf = float("-inf")   # 负无穷大
    nan = float("nan")        # 非数字（Not a Number）
    print(f"\n正无穷大: {pos_inf}")
    print(f"负无穷大: {neg_inf}")
    print(f"NaN: {nan}")
    print(f"isinf: {math.isinf(pos_inf)}")
    print(f"isnan: {math.isnan(nan)}")

    # 浮点精度问题（与 C/C++ 相同，这是 IEEE 754 标准的限制）
    result = 0.1 + 0.2
    print(f"\n0.1 + 0.2 = {result}")            # 0.30000000000000004
    print(f"0.1 + 0.2 == 0.3? {result == 0.3}")  # False

    # 解决浮点精度问题的两种方式：
    print(f"round(result, 1) = {round(result, 1)}")           # 0.3
    print(f"math.isclose(result, 0.3) = {math.isclose(result, 0.3)}")  # True


def demo_bool() -> None:
    """演示布尔类型（bool）。"""
    print("\n" + "─" * 40)
    print("2.2.3 布尔类型（bool）")
    print("─" * 40)

    # Python: True / False（首字母大写）
    # C/C++: true / false（全小写）

    is_on = True
    is_off = False
    print(f"is_on = {is_on}, 类型: {type(is_on)}")

    # bool 是 int 的子类：True=1, False=0
    print(f"\nTrue + True = {True + True}")     # 2
    print(f"True * 5   = {True * 5}")           # 5
    print(f"False + 1  = {False + 1}")          # 1

    # 真值测试（以下值为 False，其余都为 True）：
    # None, False, 0, 0.0, '', [], (), {}, set()
    falsy_values = [None, False, 0, 0.0, "", [], (), {}]
    print(f"\n假值列表:")
    for val in falsy_values:
        print(f"  bool({val!r:10}) = {bool(val)}")  # !r 使用 repr 格式


def demo_none() -> None:
    """演示空值类型（None）。"""
    print("\n" + "─" * 40)
    print("2.2.4 空值类型（None）")
    print("─" * 40)

    # None 类似于 C/C++ 的 NULL/nullptr，但它是一个完整的对象
    nothing = None
    print(f"nothing = {nothing}, 类型: {type(nothing)}")

    # None 的常见用途：
    # 1. 函数无返回值时自动返回 None
    def no_return():
        pass  # pass 是占位语句，什么都不做

    ret = no_return()
    print(f"无返回值函数的返回值: {ret}")  # None

    # 2. 初始化"暂时没有值"的变量
    user_input: str | None = None   # 用 None 表示"还未输入"

    # 3. 检查 None（用 is，不用 ==）
    if user_input is None:
        print("用户还未输入")

    # ✅ 正确：if x is None:
    # ❌ 不推荐：if x == None:（可能被重载，行为不一致）


# =============================================================================
# 2.3 类型转换
# =============================================================================

def demo_type_conversion() -> None:
    """演示类型转换。"""
    print("\n" + "=" * 60)
    print("2.3 类型转换")
    print("=" * 60)

    # ── int() 转换 ──────────────────────────────────
    print("int() 转换:")
    print(f"  int(3.99)     = {int(3.99)}")         # 3（直接截断，不四舍五入）
    print(f"  int('123')    = {int('123')}")         # 123
    print(f"  int(True)     = {int(True)}")          # 1
    print(f"  int('0xFF',16)= {int('0xFF', 16)}")    # 255（第二参数指定进制）
    print(f"  int('1010',2) = {int('1010', 2)}")     # 10（二进制字符串转十进制）

    # ── float() 转换 ─────────────────────────────────
    print("\nfloat() 转换:")
    print(f"  float(10)    = {float(10)}")           # 10.0
    print(f"  float('3.14')= {float('3.14')}")       # 3.14
    print(f"  float('1e3') = {float('1e3')}")        # 1000.0

    # ── str() 转换 ───────────────────────────────────
    print("\nstr() 转换:")
    print(f"  str(123)  = {str(123)!r}")             # '123'
    print(f"  str(3.14) = {str(3.14)!r}")            # '3.14'
    print(f"  str(True) = {str(True)!r}")            # 'True'
    print(f"  str(None) = {str(None)!r}")            # 'None'

    # ── 转换失败时的处理 ─────────────────────────────
    print("\n转换失败时捕获异常:")
    for value in ["abc", "12.5.6", ""]:
        try:
            result = int(value)
            print(f"  int({value!r}) = {result}")
        except ValueError as e:
            print(f"  int({value!r}) 失败: {e}")


# =============================================================================
# 2.4 类型检查
# =============================================================================

def demo_type_checking() -> None:
    """演示类型检查方法。"""
    print("\n" + "=" * 60)
    print("2.4 类型检查")
    print("=" * 60)

    # type()：返回对象的精确类型
    print("type() 函数:")
    print(f"  type(42)      = {type(42)}")
    print(f"  type(3.14)    = {type(3.14)}")
    print(f"  type('hello') = {type('hello')}")
    print(f"  type(True)    = {type(True)}")
    print(f"  type(None)    = {type(None)}")

    # isinstance()：检查对象是否属于某类型（包括子类）
    # 推荐使用 isinstance 而不是 type() == 某类型
    print("\nisinstance() 函数:")
    print(f"  isinstance(42, int)        = {isinstance(42, int)}")        # True
    print(f"  isinstance(3.14, float)    = {isinstance(3.14, float)}")    # True
    print(f"  isinstance('hi', str)      = {isinstance('hi', str)}")      # True
    print(f"  isinstance(True, bool)     = {isinstance(True, bool)}")     # True
    print(f"  isinstance(True, int)      = {isinstance(True, int)}")      # True（bool 是 int 的子类）

    # isinstance 支持检查多个类型（传入元组）
    value = 3.14
    print(f"\n  isinstance(3.14, (int, float)) = {isinstance(value, (int, float))}")  # True

    # type() 和 isinstance() 的区别：
    print("\ntype() vs isinstance() 区别:")
    print(f"  type(True) == int       → {type(True) == int}")      # False（type 精确匹配）
    print(f"  isinstance(True, int)   → {isinstance(True, int)}")  # True（isinstance 考虑继承）


# =============================================================================
# 2.5 运算符
# =============================================================================

def demo_operators() -> None:
    """演示各种运算符。"""
    print("\n" + "=" * 60)
    print("2.5 运算符")
    print("=" * 60)

    # ── 算术运算符 ───────────────────────────────────────────────────────────
    print("算术运算符:")
    a, b = 17, 5
    print(f"  a={a}, b={b}")
    print(f"  a + b  = {a + b}")    # 加法: 22
    print(f"  a - b  = {a - b}")    # 减法: 12
    print(f"  a * b  = {a * b}")    # 乘法: 85
    print(f"  a / b  = {a / b}")    # 除法: 3.4（总是返回 float！）
    print(f"  a // b = {a // b}")   # 整除（地板除）: 3（类似 C/C++ 的 /）
    print(f"  a % b  = {a % b}")    # 取余: 2
    print(f"  a ** b = {a ** b}")   # 幂运算: 17^5 = 1419857（C++ 用 pow()）

    # 【重要差异】Python 的 / 总是返回 float
    print(f"\n  10 / 3  = {10 / 3}")   # 3.3333...（不像 C/C++ 的整数除法）
    print(f"  10 // 3 = {10 // 3}")   # 3（整除才是 C/C++ 的行为）
    print(f"  -7 // 2 = {-7 // 2}")   # -4（向下取整，而非截断！）
    # C/C++ 中 -7 / 2 = -3（向零截断）
    # Python 中 -7 // 2 = -4（向负无穷方向）

    # ── 比较运算符 ───────────────────────────────────────────────────────────
    print("\n比较运算符（返回 bool 值）:")
    x, y = 10, 20
    print(f"  x={x}, y={y}")
    print(f"  x == y → {x == y}")   # 等于
    print(f"  x != y → {x != y}")   # 不等于（C/C++ 也是 !=）
    print(f"  x < y  → {x < y}")    # 小于
    print(f"  x > y  → {x > y}")    # 大于
    print(f"  x <= y → {x <= y}")   # 小于等于
    print(f"  x >= y → {x >= y}")   # 大于等于

    # Python 支持链式比较（C/C++ 不支持）
    age = 25
    print(f"\n  链式比较: 18 < age < 60 → {18 < age < 60}")  # True
    # 相当于 C/C++ 中的：(18 < age) && (age < 60)

    # ── 逻辑运算符 ───────────────────────────────────────────────────────────
    print("\n逻辑运算符:")
    # Python：and, or, not
    # C/C++：&&,  ||, !
    print(f"  True and False → {True and False}")   # False
    print(f"  True or False  → {True or False}")    # True
    print(f"  not True       → {not True}")         # False

    # 短路求值（与 C/C++ 相同）
    # and: 左边为 False，不再计算右边
    # or:  左边为 True，不再计算右边
    x = 0
    result = x != 0 and (10 / x > 1)  # x=0，左边为 False，右边不计算（不会除零）
    print(f"  0 != 0 and (10/0 > 1) → {result}")   # False（没有报错）

    # 逻辑运算符返回操作数本身，而不一定是 True/False
    print(f"\n  0 or 'default'  → {0 or 'default'}")    # 'default'（0 是假值）
    print(f"  'hello' or 'x' → {'hello' or 'x'}")        # 'hello'（非空字符串是真值）
    print(f"  None or 42     → {None or 42}")             # 42

    # ── 赋值运算符 ───────────────────────────────────────────────────────────
    print("\n赋值运算符:")
    n = 10
    print(f"  初始 n = {n}")
    n += 5;  print(f"  n += 5  → {n}")   # 15（等同于 n = n + 5）
    n -= 3;  print(f"  n -= 3  → {n}")   # 12
    n *= 2;  print(f"  n *= 2  → {n}")   # 24
    n //= 5; print(f"  n //= 5 → {n}")   # 4（整除赋值）
    n **= 3; print(f"  n **= 3 → {n}")   # 64（幂运算赋值）
    n %= 10; print(f"  n %%= 10 → {n}")  # 4

    # ── 位运算符（与 C/C++ 相同）────────────────────────────────────────────
    print("\n位运算符:")
    a, b = 0b1100, 0b1010   # a=12, b=10
    print(f"  a      = {a:04b} ({a})")
    print(f"  b      = {b:04b} ({b})")
    print(f"  a & b  = {(a & b):04b} ({a & b})")   # 按位与: 1000 = 8
    print(f"  a | b  = {(a | b):04b} ({a | b})")   # 按位或: 1110 = 14
    print(f"  a ^ b  = {(a ^ b):04b} ({a ^ b})")   # 按位异或: 0110 = 6
    print(f"  ~a     = {~a}")                         # 按位取反: -13
    print(f"  a << 1 = {a << 1}")                    # 左移: 24
    print(f"  a >> 1 = {a >> 1}")                    # 右移: 6

    # ── 身份运算符 ───────────────────────────────────────────────────────────
    print("\n身份运算符（is / is not）:")
    # is  检查两个变量是否指向同一个对象（内存地址相同）
    # ==  检查两个变量的值是否相等
    a = [1, 2, 3]
    b = [1, 2, 3]
    c = a

    print(f"  a = {a}, b = {b}, c = a")
    print(f"  a == b → {a == b}")    # True（值相等）
    print(f"  a is b → {a is b}")    # False（不是同一对象）
    print(f"  a is c → {a is c}")    # True（c 是 a 的引用）

    # None 比较必须用 is
    value = None
    print(f"  value is None  → {value is None}")    # True
    print(f"  value is not None → {value is not None}")  # False

    # ── 成员运算符 ───────────────────────────────────────────────────────────
    print("\n成员运算符（in / not in）:")
    fruits = ["apple", "banana", "cherry"]
    print(f"  'apple' in {fruits}  → {'apple' in fruits}")       # True
    print(f"  'mango' in {fruits}  → {'mango' in fruits}")       # False
    print(f"  'mango' not in list  → {'mango' not in fruits}")   # True

    # in 也可用于字符串、字典（检查键）等
    text = "Hello, World!"
    print(f"  'World' in '{text}' → {'World' in text}")          # True


# =============================================================================
# 2.6 输入与输出
# =============================================================================

def demo_input_output() -> None:
    """演示 input() 和 print() 的用法。"""
    print("\n" + "=" * 60)
    print("2.6 输入与输出")
    print("=" * 60)

    # ── print() 进阶用法 ─────────────────────────────
    print("print() 进阶用法:")

    # 基本输出
    print("Hello, Python!")

    # 多个值，默认用空格分隔
    print("苹果", "香蕉", "橙子")              # 苹果 香蕉 橙子

    # 自定义分隔符
    print("苹果", "香蕉", "橙子", sep=", ")    # 苹果, 香蕉, 橙子

    # 自定义结尾（默认是换行 \n）
    print("第一部分", end=" ")
    print("第二部分")                           # 第一部分 第二部分

    # 打印到标准错误
    import sys
    print("这是错误信息", file=sys.stderr)      # 输出到 stderr

    # ── input() 用法 ─────────────────────────────────
    # input() 从键盘读取用户输入，返回字符串
    # 注意：input() 总是返回字符串，需要手动转换类型
    #
    # 示例（不在演示中运行，避免需要键盘输入）：
    # name = input("请输入你的名字: ")         # 返回字符串
    # age = int(input("请输入你的年龄: "))     # 需要转换为 int
    # price = float(input("请输入价格: "))     # 需要转换为 float

    # ── 格式化输出（详细内容见第 4 章）──────────────
    print("\n格式化输出示例:")
    name = "Alice"
    score = 95.678

    # 方式 1：% 格式化（旧式，类似 C 的 printf）
    print("姓名: %s, 分数: %.2f" % (name, score))

    # 方式 2：str.format()
    print("姓名: {}, 分数: {:.2f}".format(name, score))

    # 方式 3：f-string（推荐，Python 3.6+）
    print(f"姓名: {name}, 分数: {score:.2f}")

    # f-string 中可以进行表达式计算
    x = 10
    print(f"x 的平方 = {x ** 2}")            # x 的平方 = 100
    print(f"2 + 3 = {2 + 3}")                 # 2 + 3 = 5

    # 格式化数字
    n = 1234567.89
    print(f"\n千分位格式: {n:,.2f}")          # 1,234,567.89
    print(f"科学计数法: {n:.2e}")             # 1.23e+06
    print(f"宽度10右对齐: {42:>10}")          # '        42'
    print(f"宽度10左对齐: {42:<10}|")         # '42        |'
    print(f"补零: {42:05d}")                   # '00042'
    print(f"二进制: {255:b}")                  # 11111111
    print(f"十六进制: {255:x}")                # ff
    print(f"百分比: {0.856:.1%}")              # 85.6%


# =============================================================================
# 2.7 综合示例：简单计算器
# =============================================================================

def simple_calculator(a: float, b: float, op: str) -> float | None:
    """
    简单计算器函数。

    Args:
        a:  第一个操作数
        b:  第二个操作数
        op: 运算符（+、-、*、/、//、%、**）

    Returns:
        计算结果，运算符不支持时返回 None
    """
    if op == "+":
        return a + b
    elif op == "-":
        return a - b
    elif op == "*":
        return a * b
    elif op == "/":
        if b == 0:
            print("错误：除数不能为零")
            return None
        return a / b
    elif op == "//":
        if b == 0:
            print("错误：除数不能为零")
            return None
        return a // b
    elif op == "%":
        if b == 0:
            print("错误：除数不能为零")
            return None
        return a % b
    elif op == "**":
        return a ** b
    else:
        print(f"错误：不支持的运算符 '{op}'")
        return None


def demo_calculator() -> None:
    """演示计算器函数。"""
    print("\n" + "=" * 60)
    print("2.7 综合示例：简单计算器")
    print("=" * 60)

    test_cases = [
        (10, 3, "+"),
        (10, 3, "-"),
        (10, 3, "*"),
        (10, 3, "/"),
        (10, 3, "//"),
        (10, 3, "%"),
        (2, 10, "**"),
        (10, 0, "/"),    # 除以零的情况
    ]

    for a, b, op in test_cases:
        result = simple_calculator(a, b, op)
        if result is not None:
            print(f"  {a} {op} {b} = {result}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_variables()
    demo_int()
    demo_float()
    demo_bool()
    demo_none()
    demo_type_conversion()
    demo_type_checking()
    demo_operators()
    demo_input_output()
    demo_calculator()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 数据类型 ──
# int        整数，任意大小，无溢出
# float      浮点数（64位双精度），类似 C++ 的 double
# bool       布尔值：True / False（首字母大写）
# None       空值，类似 C/C++ 的 NULL，但是对象
#
# ── 运算符差异（Python vs C/C++）──
# Python /   总是浮点除法（10/3 = 3.333...）
# Python //  整除（向下取整，10//3 = 3, -7//2 = -4）
# Python **  幂运算（C/C++ 需要 pow()）
# Python and/or/not  ↔  C/C++ &&/||/!
# Python is  身份比较（内存地址）
# Python in  成员检查
# Python 链式比较：1 < x < 10  ↔  C/C++ (1<x)&&(x<10)
#
# ── 类型转换 ──
# int(), float(), str(), bool()
#
# ── 类型检查 ──
# type(x)            返回精确类型
# isinstance(x, T)   检查 x 是否是 T 类型或其子类（推荐）


# =============================================================================
# 【运行结果示例（部分）】
# =============================================================================
#
# ============================================================
# 2.1 变量与动态类型
# ============================================================
# x = 10, 类型: <class 'int'>
# x = hello, 类型: <class 'str'>
# x = 3.14, 类型: <class 'float'>
# a=0, b=0, c=0
# x=1, y=2, z=3
# 交换前: a=10, b=20
# 交换后: a=20, b=10
# 解包: first=1, middle=[2, 3, 4], last=5
#
# ────────────────────────────────────────
# 2.2.1 整数类型（int）
# ────────────────────────────────────────
# 十进制   42 = 42
# 二进制 0b1010 = 10
# 八进制 0o12  = 10
# 十六进制 0xFF = 255
# ...
#
# 算术运算符:
#   a=17, b=5
#   a + b  = 22
#   a / b  = 3.4      ← Python 除法总返回 float
#   a // b = 3        ← 整除


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：整数除法期待但得到了浮点数
# ─────────────────────────────────────
# result = 10 / 3     # result 是 3.3333...（不是 3）
# 修正：result = 10 // 3   # 整除得到 3
#
# 错误 2：用 == 比较 None
# ──────────────────────────
# if x == None:   ❌ 不推荐
# if x is None:   ✅ 正确
#
# 错误 3：用 type() 做类型判断
# ──────────────────────────────────
# if type(x) == int:      ❌ 不考虑子类
# if isinstance(x, int):  ✅ 考虑继承
#
# 错误 4：忘记 input() 返回字符串
# ──────────────────────────────────
# age = input("年龄: ")
# if age > 18:   ❌ 字符串不能和整数比较
# 修正：
# age = int(input("年龄: "))
# if age > 18:   ✅
#
# 错误 5：浮点数精度比较
# ─────────────────────────
# 0.1 + 0.2 == 0.3   ❌ 结果是 False
# math.isclose(0.1 + 0.2, 0.3)  ✅ 结果是 True


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   写一个函数 celsius_to_fahrenheit(celsius: float) -> float
#   将摄氏温度转换为华氏温度
#   公式：F = C × 9/5 + 32
#   测试：0°C → 32°F, 100°C → 212°F, 37°C → 98.6°F
#
# 练习 2：
#   写一个函数 check_type_info(value) -> str
#   接受任意值，返回格式为：
#   "值: {value}, 类型: {type_name}, 是否为真: {bool_val}"
#   例如：check_type_info(0) → "值: 0, 类型: int, 是否为真: False"
#
# 练习 3：
#   使用位运算，写一个函数 is_power_of_two(n: int) -> bool
#   判断一个正整数是否是 2 的幂次方
#   提示：2 的幂次方在二进制中只有一个 1，例如 8 = 1000
#   可以用 n & (n - 1) == 0 来判断
#
# 练习 4：
#   写一个函数 bmi_calculator(weight_kg: float, height_m: float) -> str
#   计算 BMI = 体重(kg) / 身高(m)²
#   根据 BMI 返回：
#     < 18.5: "偏瘦"
#     18.5 - 24.9: "正常"
#     25.0 - 29.9: "超重"
#     >= 30.0: "肥胖"


# =============================================================================
# 【练习答案】
# =============================================================================

def celsius_to_fahrenheit(celsius: float) -> float:
    """练习 1：摄氏温度转华氏温度。"""
    return celsius * 9 / 5 + 32


def check_type_info(value: object) -> str:
    """练习 2：返回值的类型信息字符串。"""
    type_name = type(value).__name__  # __name__ 获取类型的名称字符串
    bool_val = bool(value)
    return f"值: {value!r}, 类型: {type_name}, 是否为真: {bool_val}"


def is_power_of_two(n: int) -> bool:
    """练习 3：判断正整数是否是 2 的幂次方。"""
    if n <= 0:
        return False
    return (n & (n - 1)) == 0  # 2的幂次方：二进制只有一个1位


def bmi_calculator(weight_kg: float, height_m: float) -> str:
    """练习 4：BMI 计算器。"""
    if height_m <= 0:
        return "身高数据无效"
    bmi = weight_kg / (height_m ** 2)
    if bmi < 18.5:
        status = "偏瘦"
    elif bmi < 25.0:
        status = "正常"
    elif bmi < 30.0:
        status = "超重"
    else:
        status = "肥胖"
    return f"BMI = {bmi:.1f}，状态：{status}"


# 取消注释以运行练习答案：
# if __name__ == "__main__":
#     # 练习 1
#     for c in [0, 100, 37]:
#         f = celsius_to_fahrenheit(c)
#         print(f"{c}°C = {f}°F")
#
#     # 练习 2
#     for v in [0, 42, "", "hello", None, [], [1, 2]]:
#         print(check_type_info(v))
#
#     # 练习 3
#     for n in [1, 2, 3, 4, 8, 16, 15, 0, -1]:
#         print(f"is_power_of_two({n}) = {is_power_of_two(n)}")
#
#     # 练习 4
#     print(bmi_calculator(70, 1.75))   # BMI = 22.9，状态：正常
#     print(bmi_calculator(90, 1.70))   # BMI = 31.1，状态：肥胖
