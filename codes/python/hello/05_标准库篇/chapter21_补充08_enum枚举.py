# =============================================================================
# 第 21 章：补充 8 - enum 枚举
# =============================================================================
#
# 【学习目标】
#   1. 掌握 Enum / IntEnum 的定义和基本用法
#   2. 掌握 auto() 自动赋值
#   3. 理解 Flag / IntFlag 的位掩码用法
#   4. 理解 enum 比字符串常量好在哪
#
# 【与 C/C++ 的对比】
#   C/C++ 中 enum 本质是整数常量，默认从 0 开始递增，可以与 int 隐式转换。
#   Python 的 enum 是类型安全的：枚举成员不是整数，不能与 int 比较（除非
#   使用 IntEnum）。这种设计避免了 C 中常见的将任意整数当枚举值使用的 bug。
#
# 【运行方式】
#   python chapter21_补充08_enum枚举.py
#
# =============================================================================

from enum import Enum, IntEnum, StrEnum, Flag, IntFlag, auto, unique


# =============================================================================
# 21.补充8.1 Enum 基础
# =============================================================================

def demo_enum_basics() -> None:
    """演示 Enum 的定义和基本用法。"""
    print("=" * 60)
    print("21.补充8.1 Enum 基础")
    print("=" * 60)

    # ── 定义枚举 ──────────────────────────────────────────
    class Color(Enum):
        RED = 1
        GREEN = 2
        BLUE = 3

    # ── 访问成员 ──────────────────────────────────────────
    print(f"Color.RED        = {Color.RED!r}")
    print(f"Color.RED.name   = {Color.RED.name!r}")
    print(f"Color.RED.value  = {Color.RED.value!r}")

    # ── 通过值获取枚举成员 ────────────────────────────────
    print(f"Color(2)         = {Color(2)!r}")
    print(f"Color['RED']     = {Color['RED']!r}")

    # ── 遍历 ──────────────────────────────────────────────
    print(f"\n遍历所有成员:")
    for color in Color:
        print(f"  {color.name:8s} = {color.value}")

    # ── 相等性比较 ────────────────────────────────────────
    print(f"\n比较:")
    print(f"  Color.RED == Color.RED   → {Color.RED == Color.RED}")
    print(f"  Color.RED == Color.BLUE  → {Color.RED == Color.BLUE}")
    print(f"  Color.RED == 1           → {Color.RED == 1}  (不是整数!)")

    # ── 枚举成员是单例 ────────────────────────────────────
    print(f"  Color.RED is Color.RED   → {Color.RED is Color.RED}")

    # ── 类型安全：Enum 不能与 int 混用 ────────────────────
    print(f"\n类型安全:")
    print(f"  isinstance(Color.RED, int) = {isinstance(Color.RED, int)}")


# =============================================================================
# 21.补充8.2 IntEnum / StrEnum / auto
# =============================================================================

def demo_intenum_and_auto() -> None:
    """演示 IntEnum、StrEnum 和 auto() 的用法。"""
    print("\n" + "=" * 60)
    print("21.补充8.2 IntEnum / StrEnum / auto")
    print("=" * 60)

    # ── IntEnum：可与 int 比较，可在需要 int 的地方使用 ────
    class Priority(IntEnum):
        LOW = 1
        MEDIUM = 2
        HIGH = 3

    print(f"IntEnum 示例:")
    print(f"  Priority.HIGH == 3     → {Priority.HIGH == 3}      (可以与 int 比较)")
    print(f"  isinstance(P.HIGH, int) → {isinstance(Priority.HIGH, int)}")
    print(f"  Priority.HIGH > Priority.LOW → {Priority.HIGH > Priority.LOW}")

    # ── auto()：自动递增赋值 ──────────────────────────────
    class Status(Enum):
        PENDING = auto()     # 1
        RUNNING = auto()     # 2
        DONE = auto()        # 3
        FAILED = auto()      # 4

    print(f"\nauto() 自动赋值:")
    for s in Status:
        print(f"  {s.name:10s} = {s.value}")

    # ── StrEnum (Python 3.11+) ────────────────────────────
    class Direction(StrEnum):
        NORTH = "north"
        SOUTH = "south"
        EAST = "east"
        WEST = "west"

    print(f"\nStrEnum 示例:")
    print(f"  Direction.NORTH == 'north' → {Direction.NORTH == 'north'}")
    print(f"  f-string: {Direction.NORTH} (自动使用值)")


# =============================================================================
# 21.补充8.3 Flag / IntFlag：位掩码枚举
# =============================================================================

def demo_flags() -> None:
    """演示 Flag 和 IntFlag 的位掩码用法。"""
    print("\n" + "=" * 60)
    print("21.补充8.3 Flag / IntFlag：位掩码枚举")
    print("=" * 60)

    # ── Flag：组合枚举 ────────────────────────────────────
    class Permission(Flag):
        NONE = 0
        READ = 1      # 0b001
        WRITE = 2     # 0b010
        EXECUTE = 4   # 0b100
        ALL = READ | WRITE | EXECUTE

    print(f"单成员:")
    print(f"  Permission.READ     = {Permission.READ!r}    (value={Permission.READ.value})")
    print(f"  Permission.WRITE    = {Permission.WRITE!r}    (value={Permission.WRITE.value})")

    # 组合
    rw = Permission.READ | Permission.WRITE
    print(f"\n组合 (| 运算符):")
    print(f"  READ | WRITE        = {rw!r}   (value={rw.value:#05b})")
    print(f"  READ | WRITE | EXECUTE = {Permission.ALL!r}")
    print(f"  READ in (READ|WRITE) = {Permission.READ in rw}")

    # 成员关系检查
    print(f"\n成员检查:")
    rwx = Permission.READ | Permission.WRITE | Permission.EXECUTE
    print(f"  Permission.READ in rwx  = {Permission.READ in rwx}")
    print(f"  bool(Permission.NONE)   = {bool(Permission.NONE)}")

    # ── IntFlag：可位运算的 IntEnum ───────────────────────
    class OpenMode(IntFlag):
        READ = 1
        WRITE = 2
        BINARY = 4

    mode = OpenMode.READ | OpenMode.BINARY
    print(f"\nIntFlag (可位运算):")
    print(f"  READ | BINARY  = {mode!r}  (value={mode.value})")
    print(f"  mode & OpenMode.READ = {mode & OpenMode.READ!r}")

    # ── 实际应用：re.RegexFlag ────────────────────────────
    print(f"\n标准库中的 Flag 示例:")
    import re
    flags = re.IGNORECASE | re.MULTILINE
    print(f"  re.IGNORECASE | re.MULTILINE = {flags!r} (value={flags.value})")


# =============================================================================
# 21.补充8.4 枚举的实用模式
# =============================================================================

def demo_enum_patterns() -> None:
    """演示枚举的常见使用模式。"""
    print("\n" + "=" * 60)
    print("21.补充8.4 枚举的实用模式")
    print("=" * 60)

    # ── 模式 1：带数据的枚举 ──────────────────────────────
    print("1. 带数据的枚举（HTTP 状态码）:")

    class HttpStatus(Enum):
        OK = (200, "请求成功")
        NOT_FOUND = (404, "资源不存在")
        INTERNAL_ERROR = (500, "服务器内部错误")

        def __init__(self, code: int, message: str):
            self.code = code
            self.message = message

        @classmethod
        def from_code(cls, code: int) -> "HttpStatus":
            for member in cls:
                if member.code == code:
                    return member
            raise ValueError(f"未知状态码: {code}")

    print(f"  HttpStatus.OK.code    = {HttpStatus.OK.code}")
    print(f"  HttpStatus.OK.message = {HttpStatus.OK.message!r}")
    print(f"  HttpStatus.from_code(404) = {HttpStatus.from_code(404)!r}")

    # ── 模式 2：match/case 联动 ───────────────────────────
    print("\n2. match/case 与枚举联动:")

    def handle_status(status: HttpStatus) -> str:
        match status:
            case HttpStatus.OK:
                return "一切正常"
            case HttpStatus.NOT_FOUND:
                return "页面不存在"
            case HttpStatus.INTERNAL_ERROR:
                return "服务器出错了"
            case _:
                return "未知状态"

    print(f"  OK          → {handle_status(HttpStatus.OK)!r}")
    print(f"  NOT_FOUND   → {handle_status(HttpStatus.NOT_FOUND)!r}")

    # ── 模式 3：@unique 防止重复值 ────────────────────────
    print("\n3. @unique 保证值唯一:")

    @unique
    class Weekday(IntEnum):
        MON = 1
        TUE = 2
        WED = 3
        THU = 4
        FRI = 5
        SAT = 6
        SUN = 7

    print(f"  len(Weekday) = {len(Weekday)} 个成员（值均唯一）")
    try:
        @unique
        class Bad(Enum):
            A = 1
            B = 1  # 重复
    except ValueError as e:
        print(f"  @unique 检测到重复: {e}")

    # ── 模式 4：用枚举替代字符串常量 ──────────────────────
    print("\n4. 字符串常量 vs 枚举:")

    # ❌ 字符串常量（教程中常见写法）
    # if event_type == "TEMPERATURE":
    #     ...
    # 问题：拼写错误不会报错，IDE 无法补全

    # ✅ 枚举写法
    class SensorType(Enum):
        TEMPERATURE = auto()
        HUMIDITY = auto()
        PRESSURE = auto()

    print(f"  SensorType['TEMPERATURE'] = {SensorType['TEMPERATURE']!r}")
    print(f"  优势: IDE 可自动补全，拼写错误会被静态检查发现")
    print(f"        match/case 中对枚举成员有穷举检查")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_enum_basics()
    demo_intenum_and_auto()
    demo_flags()
    demo_enum_patterns()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. 基本定义
#    class Color(Enum):
#        RED = 1
#        GREEN = 2
#        BLUE = 3
#
# 2. 访问
#    Color.RED             成员
#    Color.RED.name        → "RED"
#    Color.RED.value       → 1
#    Color(1)              → Color.RED（按值查找）
#    Color["RED"]          → Color.RED（按名查找）
#
# 3. 变体
#    Enum       类型安全枚举（不与 int 比较）
#    IntEnum    可与 int 比较
#    StrEnum    可与 str 比较  (Python 3.11+)
#    Flag       支持 | & 运算的枚举
#    IntFlag    支持位运算的 IntEnum
#    auto()     自动递增赋值
#
# 4. match/case 联动
#    match status:
#        case HttpStatus.OK: ...
#        case HttpStatus.NOT_FOUND: ...
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   定义一个 Weekday IntEnum（MON=1 到 SUN=7），写函数 is_weekend(day)
#   判断是否为周末。
#
# 练习 2（进阶）：
#   定义 FilePermission(Flag)，包含 NONE/READ/WRITE/EXECUTE，
#   写函数 can_access(permission, required) 检查权限是否满足。
#
# 练习提示：
#   练习1：return day in (Weekday.SAT, Weekday.SUN)
#   练习2：return required in permission


# =============================================================================
# 【练习答案】
# =============================================================================


class Weekday(IntEnum):
    MON = 1
    TUE = 2
    WED = 3
    THU = 4
    FRI = 5
    SAT = 6
    SUN = 7


def is_weekend(day: Weekday) -> bool:
    """练习 1 答案：判断是否为周末。"""
    return day in (Weekday.SAT, Weekday.SUN)


class FilePermission(Flag):
    NONE = 0
    READ = 1
    WRITE = 2
    EXECUTE = 4
    ALL = READ | WRITE | EXECUTE


def can_access(permission: FilePermission, required: FilePermission) -> bool:
    """练习 2 答案：检查权限是否满足。"""
    return required in permission


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 60)
#     for day in Weekday:
#         print(f"{day.name}: is_weekend={is_weekend(day)}")
#     print()
#     rw = FilePermission.READ | FilePermission.WRITE
#     print(f"can_access(rw, READ):    {can_access(rw, FilePermission.READ)}")
#     print(f"can_access(rw, EXECUTE): {can_access(rw, FilePermission.EXECUTE)}")
