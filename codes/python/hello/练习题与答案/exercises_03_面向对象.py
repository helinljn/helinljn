# =============================================================================
# 综合练习 03：面向对象篇
# 覆盖：ch08 模块与包、ch09 面向对象基础、ch10 面向对象进阶、ch11 异常处理
# =============================================================================

from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import Callable, Protocol


# =============================================================================
# 练习 1：银行账户系统
# =============================================================================
# 设计一个银行账户系统，支持多种账户类型。
#
# 要求：
#   - 抽象基类 BankAccount，包含：
#       owner: str, balance: float（属性）
#       deposit(amount), withdraw(amount)（抽象方法）
#   - 子类 SavingsAccount（储蓄账户）：
#       利率 rate，withdraw 时余额不能低于 0
#   - 子类 CreditAccount（信用账户）：
#       信用额度 credit_limit，可透支但不能超过额度
#   - 使用 @property 装饰 balance，使其不能被直接设置
#   - 自定义异常 InsufficientFundsError
#   - __str__ 返回账户信息


# =============================================================================
# 练习 2：插件系统（基于抽象基类）
# =============================================================================
# 设计一个简单的插件系统。
#
# 要求：
#   - 抽象基类 Plugin，定义：
#       name: str（属性）
#       process(data) → str（抽象方法）
#   - PluginManager 类：
#       register(plugin: Plugin) → 注册插件
#       unregister(name: str) → 注销插件
#       run(data) → 依次调用所有已注册插件的 process 方法，返回结果列表
#   - 实现至少两个具体插件：UpperCasePlugin、ReversePlugin
#   - PluginManager 使用上下文管理器（__enter__ / __exit__）


# =============================================================================
# 练习 3：数据类与不可变对象
# =============================================================================
# 使用 dataclass 设计几个数据模型。
#
# 要求：
#   - @dataclass Point: x, y（支持加法运算 __add__）
#   - @dataclass(frozen=True) Color: r, g, b（0-255，__post_init__ 验证）
#   - @dataclass Rectangle: top_left: Point, width, height
#       添加 area 属性和 contains(point) 方法
#   - 使用 field(default_factory=...) 处理可变默认值


# =============================================================================
# 练习 4：自定义异常层次结构
# =============================================================================
# 设计一个 API 调用的异常层次结构。
#
# 要求：
#   - 基类 APIError(Exception)
#   - 子类：
#       NetworkError（网络错误，含 status_code）
#       AuthenticationError（认证失败）
#       RateLimitError（频率限制，含 retry_after 秒数）
#       ValidationError（数据验证失败，含 field_errors: dict）
#   - 写一个 simulate_api_call() 函数，随机抛出上述异常
#   - 写 handle_api_error() 函数，优雅地处理每种异常


# =============================================================================
# 练习 5：观察者模式（事件系统）
# =============================================================================
# 实现一个简单的发布-订阅（Pub/Sub）事件系统。
#
# 要求：
#   - EventEmitter 类：
#       on(event: str, callback) → 订阅事件
#       off(event: str, callback) → 取消订阅
#       emit(event: str, *args, **kwargs) → 触发事件
#       once(event: str, callback) → 只订阅一次
#   - 使用 Protocol 定义 Callback 类型
#   - 触发事件时如果有回调抛出异常，捕获并打印警告后继续执行其他回调


# =============================================================================
# ============================= 答案分隔线 ====================================
# =============================================================================

import random
import time
from typing import Any


# =============================================================================
# 练习 1 答案：银行账户系统
# =============================================================================

class InsufficientFundsError(Exception):
    """余额不足异常。"""

    def __init__(self, owner: str, balance: float, amount: float) -> None:
        self.owner = owner
        self.balance = balance
        self.amount = amount
        super().__init__(
            f"{owner} 余额不足！当前余额: {balance:.2f}，"
            f"试图取款: {amount:.2f}"
        )


class BankAccount(ABC):
    """银行账户抽象基类。"""

    def __init__(self, owner: str, initial_balance: float = 0.0) -> None:
        self.owner = owner
        self._balance = initial_balance  # 私有属性

    @property
    def balance(self) -> float:
        """余额属性（只读）。"""
        return self._balance

    def deposit(self, amount: float) -> None:
        """存款。"""
        if amount <= 0:
            raise ValueError("存款金额必须大于 0")
        self._balance += amount

    @abstractmethod
    def withdraw(self, amount: float) -> None:
        """取款（子类实现具体逻辑）。"""
        ...

    def __str__(self) -> str:
        return f"[{self.__class__.__name__}] {self.owner}: ¥{self.balance:.2f}"


class SavingsAccount(BankAccount):
    """储蓄账户：不可透支。"""

    def __init__(self, owner: str, initial_balance: float = 0.0,
                 rate: float = 0.02) -> None:
        super().__init__(owner, initial_balance)
        self.rate = rate

    def withdraw(self, amount: float) -> None:
        if amount <= 0:
            raise ValueError("取款金额必须大于 0")
        if self._balance < amount:
            raise InsufficientFundsError(self.owner, self._balance, amount)
        self._balance -= amount

    def add_interest(self) -> None:
        """添加利息。"""
        self._balance += self._balance * self.rate

    def __str__(self) -> str:
        return f"{super().__str__()} (利率: {self.rate:.1%})"


class CreditAccount(BankAccount):
    """信用账户：可在额度内透支。"""

    def __init__(self, owner: str, initial_balance: float = 0.0,
                 credit_limit: float = 1000.0) -> None:
        super().__init__(owner, initial_balance)
        self.credit_limit = credit_limit

    def withdraw(self, amount: float) -> None:
        if amount <= 0:
            raise ValueError("取款金额必须大于 0")
        if self._balance + self.credit_limit < amount:
            raise InsufficientFundsError(
                self.owner, self._balance + self.credit_limit, amount
            )
        self._balance -= amount

    def __str__(self) -> str:
        return f"{super().__str__()} (额度: ¥{self.credit_limit:.2f})"


# =============================================================================
# 练习 2 答案：插件系统
# =============================================================================

class Plugin(ABC):
    """插件抽象基类。"""

    @property
    @abstractmethod
    def name(self) -> str:
        """插件名称。"""
        ...

    @abstractmethod
    def process(self, data: str) -> str:
        """处理数据。"""
        ...


class UpperCasePlugin(Plugin):
    """将文本转为大写。"""
    name = "uppercase"

    def process(self, data: str) -> str:
        return data.upper()


class ReversePlugin(Plugin):
    """反转文本。"""
    name = "reverse"

    def process(self, data: str) -> str:
        return data[::-1]


class RepeatPlugin(Plugin):
    """重复文本。"""
    name = "repeat"

    def __init__(self, times: int = 2) -> None:
        self.times = times

    def process(self, data: str) -> str:
        return data * self.times


class PluginManager:
    """插件管理器，支持上下文管理器。"""

    def __init__(self) -> None:
        self._plugins: dict[str, Plugin] = {}
        self._results: list[str] = []

    def register(self, plugin: Plugin) -> None:
        """注册插件。"""
        if plugin.name in self._plugins:
            raise ValueError(f"插件 '{plugin.name}' 已注册")
        self._plugins[plugin.name] = plugin
        print(f"  已注册插件: {plugin.name}")

    def unregister(self, name: str) -> None:
        """注销插件。"""
        if name not in self._plugins:
            raise KeyError(f"插件 '{name}' 未找到")
        del self._plugins[name]
        print(f"  已注销插件: {name}")

    def run(self, data: str) -> list[str]:
        """依次运行所有插件。"""
        results = []
        for name, plugin in self._plugins.items():
            try:
                result = plugin.process(data)
                results.append(f"[{name}] {result}")
            except Exception as e:
                results.append(f"[{name}] ERROR: {e}")
        self._results = results
        return results

    def __enter__(self) -> "PluginManager":
        print("PluginManager 已启动")
        return self

    def __exit__(self, exc_type, exc_val, exc_tb) -> None:
        print(f"PluginManager 已关闭，共处理 {len(self._results)} 个结果")
        return False  # 不抑制异常


# =============================================================================
# 练习 3 答案：数据类
# =============================================================================

@dataclass
class Point:
    """二维坐标点（可变）。"""
    x: float
    y: float

    def __add__(self, other: "Point") -> "Point":
        return Point(self.x + other.x, self.y + other.y)

    def __sub__(self, other: "Point") -> "Point":
        return Point(self.x - other.x, self.y - other.y)


@dataclass(frozen=True)
class Color:
    """RGB 颜色（不可变）。"""
    r: int
    g: int
    b: int

    def __post_init__(self) -> None:
        """验证 RGB 值在 0-255 范围内。"""
        for name in ("r", "g", "b"):
            value = getattr(self, name)
            if not 0 <= value <= 255:
                raise ValueError(f"{name} 必须在 0-255 之间，收到 {value}")

    def to_hex(self) -> str:
        return f"#{self.r:02X}{self.g:02X}{self.b:02X}"


@dataclass
class Rectangle:
    """矩形，使用 top-left 点和宽高定义。"""
    top_left: Point
    width: float
    height: float
    _tags: list[str] = field(default_factory=list)

    @property
    def area(self) -> float:
        return self.width * self.height

    @property
    def perimeter(self) -> float:
        return 2 * (self.width + self.height)

    def contains(self, point: Point) -> bool:
        """检查点是否在矩形内部。"""
        return (
            self.top_left.x <= point.x <= self.top_left.x + self.width
            and self.top_left.y <= point.y <= self.top_left.y + self.height
        )

    def add_tag(self, tag: str) -> None:
        self._tags.append(tag)


# =============================================================================
# 练习 4 答案：自定义异常层次结构
# =============================================================================


class APIError(Exception):
    """API 调用异常基类。"""

    def __init__(self, message: str, endpoint: str = "") -> None:
        self.endpoint = endpoint
        super().__init__(f"[{endpoint}] {message}" if endpoint else message)


class NetworkError(APIError):
    """网络错误。"""

    def __init__(self, message: str, status_code: int = 0,
                 endpoint: str = "") -> None:
        self.status_code = status_code
        super().__init__(f"HTTP {status_code}: {message}", endpoint)


class AuthenticationError(APIError):
    """认证失败。"""

    def __init__(self, message: str = "认证失败",
                 endpoint: str = "") -> None:
        super().__init__(message, endpoint)


class RateLimitError(APIError):
    """频率限制。"""

    def __init__(self, retry_after: float = 60.0,
                 endpoint: str = "") -> None:
        self.retry_after = retry_after
        super().__init__(
            f"频率限制，请在 {retry_after:.0f}s 后重试", endpoint
        )


class ValidationError(APIError):
    """数据验证失败。"""

    def __init__(self, field_errors: dict[str, str],
                 endpoint: str = "") -> None:
        self.field_errors = field_errors
        details = "; ".join(f"{k}: {v}" for k, v in field_errors.items())
        super().__init__(f"验证失败 - {details}", endpoint)


def simulate_api_call(endpoint: str = "/api/data") -> str:
    """模拟 API 调用，随机抛出各种异常。"""
    outcome = random.random()

    if outcome < 0.2:
        raise NetworkError("连接超时", status_code=503, endpoint=endpoint)
    elif outcome < 0.35:
        raise AuthenticationError(endpoint=endpoint)
    elif outcome < 0.5:
        raise RateLimitError(
            retry_after=random.randint(10, 120), endpoint=endpoint
        )
    elif outcome < 0.6:
        raise ValidationError(
            {"name": "不能为空", "email": "格式不正确"}, endpoint=endpoint
        )

    return '{"status": "ok", "data": [1, 2, 3]}'


def handle_api_error(error: APIError) -> dict:
    """处理 API 异常，返回结构化错误信息。"""
    match error:
        case NetworkError(status_code=code) if code >= 500:
            return {
                "type": "network",
                "action": "retry",
                "message": f"服务器错误，建议稍后重试: {error}",
            }
        case NetworkError():
            return {
                "type": "network",
                "action": "check",
                "message": f"网络错误，请检查连接: {error}",
            }
        case AuthenticationError():
            return {
                "type": "auth",
                "action": "relogin",
                "message": "认证已过期，请重新登录",
            }
        case RateLimitError(retry_after=retry):
            return {
                "type": "rate_limit",
                "action": "wait",
                "wait_seconds": retry,
                "message": f"请求太频繁，请 {retry:.0f}s 后重试",
            }
        case ValidationError(field_errors=errors):
            return {
                "type": "validation",
                "action": "fix",
                "fields": errors,
                "message": "请修正输入数据",
            }
        case _:
            return {"type": "unknown", "action": "report", "message": str(error)}


# =============================================================================
# 练习 5 答案：观察者模式
# =============================================================================

class Callback(Protocol):
    """回调函数协议。"""

    def __call__(self, *args: Any, **kwargs: Any) -> None:
        ...


class EventEmitter:
    """简单的事件发布-订阅系统。"""

    def __init__(self) -> None:
        self._listeners: dict[str, list[Callable]] = {}
        self._once_listeners: dict[str, list[Callable]] = {}

    def on(self, event: str, callback: Callable) -> None:
        """订阅事件。"""
        if event not in self._listeners:
            self._listeners[event] = []
        self._listeners[event].append(callback)

    def off(self, event: str, callback: Callable) -> None:
        """取消订阅事件。"""
        if event in self._listeners:
            self._listeners[event] = [
                cb for cb in self._listeners[event] if cb is not callback
            ]

    def once(self, event: str, callback: Callable) -> None:
        """只订阅一次事件。"""
        if event not in self._once_listeners:
            self._once_listeners[event] = []
        self._once_listeners[event].append(callback)

    def emit(self, event: str, *args: Any, **kwargs: Any) -> None:
        """触发事件。"""
        # 触发普通监听器
        for callback in self._listeners.get(event, []):
            try:
                callback(*args, **kwargs)
            except Exception as e:
                print(f"  ⚠️  事件 '{event}' 回调异常: {e}")

        # 触发一次性监听器
        once_callbacks = self._once_listeners.pop(event, [])
        for callback in once_callbacks:
            try:
                callback(*args, **kwargs)
            except Exception as e:
                print(f"  ⚠️  一次性事件 '{event}' 回调异常: {e}")


# =============================================================================
# 运行所有练习
# =============================================================================

if __name__ == "__main__":
    print("=" * 60)
    print("综合练习 03：面向对象篇")
    print("=" * 60)

    # 练习 1
    print("\n【练习 1】银行账户系统")
    savings = SavingsAccount("Alice", 1000.0, rate=0.03)
    credit = CreditAccount("Bob", 500.0, credit_limit=2000.0)
    print(f"  {savings}")
    print(f"  {credit}")

    savings.deposit(500)
    print(f"  存款后: {savings}")

    try:
        savings.withdraw(2000)
    except InsufficientFundsError as e:
        print(f"  取款失败: {e}")

    credit.withdraw(1500)
    print(f"  透支后: {credit}")

    # 练习 2
    print("\n【练习 2】插件系统")
    with PluginManager() as pm:
        pm.register(UpperCasePlugin())
        pm.register(ReversePlugin())
        pm.register(RepeatPlugin(3))
        results = pm.run("Hello")
        for r in results:
            print(f"  {r}")

    # 练习 3
    print("\n【练习 3】数据类")
    p1 = Point(0, 0)
    p2 = Point(3, 4)
    print(f"  {p1} + {p2} = {p1 + p2}")

    red = Color(255, 0, 0)
    print(f"  Color to_hex: {red.to_hex()}")
    try:
        Color(300, 0, 0)
    except ValueError as e:
        print(f"  颜色验证: {e}")

    rect = Rectangle(Point(10, 20), 100, 50)
    rect.add_tag("main")
    print(f"  矩形面积: {rect.area}, 周长: {rect.perimeter}")
    print(f"  (50,40) 在矩形内? {rect.contains(Point(50, 40))}")
    print(f"  (200,100) 在矩形内? {rect.contains(Point(200, 100))}")

    # 练习 4
    print("\n【练习 4】异常层次结构")
    for _ in range(5):
        try:
            result = simulate_api_call("/api/users")
            print(f"  ✅ 成功: {result}")
        except APIError as e:
            info = handle_api_error(e)
            print(f"  ❌ {info['type']}: {info['message']}")

    # 练习 5
    print("\n【练习 5】事件系统")
    emitter = EventEmitter()

    def on_login(user: str) -> None:
        print(f"  登录处理器: 欢迎 {user}！")

    def on_logout(user: str) -> None:
        print(f"  登出处理器: 再见 {user}！")

    def once_ready() -> None:
        print(f"  一次性处理器: 系统就绪！")

    emitter.on("login", on_login)
    emitter.on("logout", on_logout)
    emitter.once("ready", once_ready)

    emitter.emit("ready")           # 只触发一次
    emitter.emit("ready")           # 不再触发
    emitter.emit("login", "Alice")
    emitter.emit("logout", "Alice")
