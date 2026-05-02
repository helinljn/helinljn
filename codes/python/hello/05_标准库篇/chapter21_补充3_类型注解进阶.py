# =============================================================================
# 第 21 章：补充 3 - 类型注解进阶
# =============================================================================
#
# 【学习目标】
#   1. 掌握 Optional、Union 和 Literal 的使用场景
#   2. 掌握 TypeAlias 提高复杂类型可读性
#   3. 掌握 TypedDict 描述字典结构
#   4. 理解 Protocol 表达“只关心行为”的接口
#   5. 理解泛型 Generic 和 TypeVar 的基本写法
#
# 【运行方式】
#   python chapter21_补充3_类型注解进阶.py
#
# =============================================================================

from typing import Any, Generic, Literal, NotRequired, Protocol, TypeAlias, TypedDict, TypeVar


# =============================================================================
# 21.补充3.1 Optional、Union、Literal
# =============================================================================

UserId: TypeAlias = int
JsonValue: TypeAlias = str | int | float | bool | None | list["JsonValue"] | dict[str, "JsonValue"]
LogLevel: TypeAlias = Literal["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]


def normalize_user_id(value: int | str | None) -> UserId | None:
    """把用户 ID 转成 int；None 表示没有提供。"""
    if value is None:
        return None
    return int(value)


def should_log(message_level: LogLevel, enabled_level: LogLevel = "INFO") -> bool:
    """根据日志级别判断是否应该输出。"""
    order = ["DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"]
    return order.index(message_level) >= order.index(enabled_level)


def demo_union_literal() -> None:
    """演示联合类型、可选类型和 Literal。"""
    print("=" * 60)
    print("21.补充3.1 Optional、Union、Literal")
    print("=" * 60)

    print("normalize_user_id('42'):", normalize_user_id("42"))
    print("normalize_user_id(None):", normalize_user_id(None))
    print("should_log('ERROR', 'WARNING'):", should_log("ERROR", "WARNING"))


# =============================================================================
# 21.补充3.2 TypedDict
# =============================================================================


class UserRecord(TypedDict):
    """用户数据字典结构。"""

    id: int
    name: str
    email: str
    age: NotRequired[int]


def format_user(user: UserRecord) -> str:
    """格式化用户记录。"""
    age = user.get("age", "unknown")
    return f"{user['id']} - {user['name']} <{user['email']}> age={age}"


def demo_typed_dict() -> None:
    """演示 TypedDict 描述固定结构的字典。"""
    print("\n" + "=" * 60)
    print("21.补充3.2 TypedDict")
    print("=" * 60)

    user: UserRecord = {"id": 1, "name": "Alice", "email": "alice@example.com"}
    print(format_user(user))


# =============================================================================
# 21.补充3.3 Protocol
# =============================================================================


class Serializer(Protocol):
    """只要求对象提供 dumps 方法。"""

    def dumps(self, value: JsonValue) -> str:
        """把值序列化为字符串。"""


class SimpleJsonSerializer:
    """一个符合 Serializer 协议的类。"""

    def dumps(self, value: JsonValue) -> str:
        if isinstance(value, str):
            return f'"{value}"'
        return str(value).lower() if isinstance(value, bool) else str(value)


def save_payload(serializer: Serializer, payload: JsonValue) -> str:
    """使用任意符合 Serializer 协议的对象序列化数据。"""
    return serializer.dumps(payload)


def demo_protocol() -> None:
    """演示 Protocol 的结构化类型思想。"""
    print("\n" + "=" * 60)
    print("21.补充3.3 Protocol")
    print("=" * 60)

    serializer = SimpleJsonSerializer()
    print("序列化字符串:", save_payload(serializer, "hello"))
    print("序列化布尔值:", save_payload(serializer, True))


# =============================================================================
# 21.补充3.4 Generic 和 TypeVar
# =============================================================================


T = TypeVar("T")


class Repository(Generic[T]):
    """一个简单的泛型仓库。"""

    def __init__(self) -> None:
        self._items: list[T] = []

    def add(self, item: T) -> None:
        self._items.append(item)

    def all(self) -> list[T]:
        return list(self._items)


def first_or_none(items: list[T]) -> T | None:
    """返回列表第一个元素；空列表返回 None。"""
    return items[0] if items else None


def demo_generic() -> None:
    """演示泛型类和泛型函数。"""
    print("\n" + "=" * 60)
    print("21.补充3.4 Generic 和 TypeVar")
    print("=" * 60)

    repo: Repository[str] = Repository()
    repo.add("python")
    repo.add("typing")

    print("仓库内容:", repo.all())
    print("第一个:", first_or_none(repo.all()))


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_union_literal()
    demo_typed_dict()
    demo_protocol()
    demo_generic()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. Optional / Union
#    - Python 3.10+ 推荐写成 T | None、A | B
#    - 适合表达参数可能为空或支持多种输入类型
#
# 2. Literal
#    - 限定字符串或数字只能取某几个固定值
#    - 常用于模式、状态、级别等枚举型参数
#
# 3. TypeAlias
#    - 给复杂类型起名字，提高可读性
#    - 例如 JsonValue、UserId
#
# 4. TypedDict
#    - 描述字典应该有哪些 key 和 value 类型
#    - 适合 JSON、配置、接口返回值等字典结构
#
# 5. Protocol
#    - 表达“只要有某些方法即可”的结构化接口
#    - 更贴近 Python 鸭子类型
#
# 6. Generic / TypeVar
#    - 编写保留类型信息的容器、仓库、工具函数
#    - list[T]、Repository[T] 都属于泛型思想


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   定义一个 Literal 类型 SortOrder，只允许 "asc" 和 "desc"。
#
# 练习 2（基础）：
#   定义一个 Product TypedDict，包含 id、name、price 三个字段。
#
# 练习 3（进阶）：
#   写一个泛型函数 last_or_none(items)，返回最后一个元素或 None。
#
# 练习答案提示：
#   练习1：SortOrder: TypeAlias = Literal["asc", "desc"]
#   练习2：class Product(TypedDict): ...
#   练习3：使用 TypeVar，并让返回类型写成 T | None


# =============================================================================
# 【练习答案】
# =============================================================================


SortOrder: TypeAlias = Literal["asc", "desc"]


class Product(TypedDict):
    """练习 2 答案：商品字典结构。"""

    id: int
    name: str
    price: float


def last_or_none(items: list[T]) -> T | None:
    """练习 3 答案：返回最后一个元素或 None。"""
    return items[-1] if items else None


# 取消注释以运行练习：
# if __name__ == "__main__":
#     product: Product = {"id": 1, "name": "Python Book", "price": 68.0}
#
#     print("=" * 40)
#     print(product)
#
#     print("\n" + "=" * 40)
#     print(last_or_none([1, 2, 3]))
