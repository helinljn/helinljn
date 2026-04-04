# =============================================================================
# 第 11 章：异常处理
# =============================================================================
#
# 【学习目标】
#   1. 理解异常的概念和异常层次结构
#   2. 掌握 try-except-else-finally 语法
#   3. 学会捕获多种异常
#   4. 能够自定义异常类
#   5. 掌握 raise 和 assert 的使用
#   6. 理解异常链（from）
#   7. 了解异常处理最佳实践
#
# 【与 C++ 的对比】
#   C++:  try { ... } catch (std::exception& e) { ... }
#   Python: try: ... except Exception as e: ...
#   Python 异常更灵活，支持 else 和 finally 子句
#
# 【运行方式】
#   python chapter11_异常处理.py
#
# =============================================================================


# =============================================================================
# 11.1 异常基础
# =============================================================================

def demo_exception_basics() -> None:
    """演示异常的基本概念。"""
    print("=" * 60)
    print("11.1 异常基础")
    print("=" * 60)

    # ── 常见内置异常 ─────────────────────────────────────
    print("常见内置异常类型:")

    # ZeroDivisionError：除以零
    try:
        result = 10 / 0
    except ZeroDivisionError as e:
        print(f"  ZeroDivisionError: {e}")

    # IndexError：列表索引越界
    try:
        lst = [1, 2, 3]
        val = lst[10]
    except IndexError as e:
        print(f"  IndexError: {e}")

    # KeyError：字典键不存在
    try:
        d = {"a": 1}
        val = d["b"]
    except KeyError as e:
        print(f"  KeyError: {e}")

    # TypeError：类型错误
    try:
        result = "hello" + 5
    except TypeError as e:
        print(f"  TypeError: {e}")

    # ValueError：值错误
    try:
        num = int("abc")
    except ValueError as e:
        print(f"  ValueError: {e}")

    # AttributeError：属性不存在
    try:
        x = None
        x.upper()
    except AttributeError as e:
        print(f"  AttributeError: {e}")

    # FileNotFoundError：文件不存在
    try:
        with open("不存在的文件.txt") as f:
            content = f.read()
    except FileNotFoundError as e:
        print(f"  FileNotFoundError: {e}")


# =============================================================================
# 11.2 try-except-else-finally
# =============================================================================

def read_number_from_string(s: str) -> float:
    """
    演示完整的 try-except-else-finally 结构。

    - try:     可能抛出异常的代码
    - except:  捕获并处理异常
    - else:    try 块没有异常时执行
    - finally: 无论如何都会执行（清理资源）
    """
    print(f"\n  尝试将 {s!r} 转换为数字:")
    result = None
    try:
        # 可能抛出 ValueError
        result = float(s)
        print(f"    [try] 转换成功: {result}")
    except ValueError as e:
        # 只有发生 ValueError 时才执行
        print(f"    [except] 转换失败: {e}")
    else:
        # try 块完全成功（没有任何异常）时执行
        print(f"    [else] 一切正常，结果是 {result}")
    finally:
        # 不管成功还是失败，都会执行（用于清理资源）
        print(f"    [finally] 清理完成")

    return result if result is not None else 0.0


def demo_try_except_else_finally() -> None:
    """演示 try-except-else-finally。"""
    print("\n" + "=" * 60)
    print("11.2 try-except-else-finally")
    print("=" * 60)

    read_number_from_string("3.14")
    read_number_from_string("hello")
    read_number_from_string("42")


# =============================================================================
# 11.3 捕获多种异常
# =============================================================================

def process_data(data: list, index: int, divisor: float) -> float:
    """演示捕获多种异常。"""
    try:
        value = data[index]          # 可能 IndexError
        result = int(value) / divisor  # 可能 ValueError 或 ZeroDivisionError
        return result
    except IndexError:
        print(f"    索引 {index} 超出范围（列表长度 {len(data)}）")
        return 0.0
    except ValueError:
        print(f"    值 {value!r} 无法转换为整数")
        return 0.0
    except ZeroDivisionError:
        print(f"    除数不能为零")
        return 0.0
    except (TypeError, AttributeError) as e:
        # 同时捕获多种异常类型
        print(f"    类型错误: {e}")
        return 0.0
    except Exception as e:
        # 捕获所有其他异常（兜底）
        # 注意：不要轻易使用，会隐藏真正的错误
        print(f"    未知错误: {type(e).__name__}: {e}")
        return 0.0


def demo_multiple_exceptions() -> None:
    """演示捕获多种异常。"""
    print("\n" + "=" * 60)
    print("11.3 捕获多种异常")
    print("=" * 60)

    data = ["10", "20", "abc", "40"]
    test_cases = [
        (data, 1, 2.0),   # 正常：data[1] / 2 = 10
        (data, 5, 2.0),   # IndexError
        (data, 2, 2.0),   # ValueError（"abc" 不是数字）
        (data, 0, 0.0),   # ZeroDivisionError
    ]

    for d, i, div in test_cases:
        result = process_data(d, i, div)
        print(f"    → 结果: {result}")


# =============================================================================
# 11.4 自定义异常
# =============================================================================

class AppError(Exception):
    """
    应用程序基础异常。
    自定义异常应继承自 Exception（而非直接继承 BaseException）。
    """

    def __init__(self, message: str, code: int = 0) -> None:
        super().__init__(message)
        self.message = message
        self.code = code

    def __str__(self) -> str:
        return f"[{self.code}] {self.message}"


class ValidationError(AppError):
    """数据验证异常。"""

    def __init__(self, field: str, value, reason: str) -> None:
        message = f"字段 '{field}' 的值 {value!r} 无效: {reason}"
        super().__init__(message, code=400)
        self.field = field
        self.value = value
        self.reason = reason


class DatabaseError(AppError):
    """数据库操作异常。"""

    def __init__(self, operation: str, detail: str) -> None:
        message = f"数据库操作 '{operation}' 失败: {detail}"
        super().__init__(message, code=500)
        self.operation = operation


class RecordNotFoundError(DatabaseError):
    """记录不存在异常。"""

    def __init__(self, table: str, record_id) -> None:
        super().__init__("查询", f"在 {table} 中未找到 id={record_id}")
        self.table = table
        self.record_id = record_id
        self.code = 404


def validate_age(age: int) -> None:
    """验证年龄，使用自定义异常。"""
    if not isinstance(age, int):
        raise ValidationError("age", age, "必须是整数")
    if age < 0:
        raise ValidationError("age", age, "不能为负数")
    if age > 150:
        raise ValidationError("age", age, "超过合理范围")


def find_user(user_id: int) -> dict:
    """模拟数据库查询。"""
    db = {1: {"name": "张三", "age": 25}, 2: {"name": "李四", "age": 30}}
    if user_id not in db:
        raise RecordNotFoundError("users", user_id)
    return db[user_id]


def demo_custom_exceptions() -> None:
    """演示自定义异常。"""
    print("\n" + "=" * 60)
    print("11.4 自定义异常")
    print("=" * 60)

    # 测试 ValidationError
    print("验证异常测试:")
    for age in [25, -5, 200, "old"]:
        try:
            validate_age(age)
            print(f"  年龄 {age!r}: ✓ 通过")
        except ValidationError as e:
            print(f"  年龄 {age!r}: ✗ {e}")
        except AppError as e:
            print(f"  AppError: {e}")

    # 测试 DatabaseError
    print("\n数据库查询测试:")
    for uid in [1, 99, 2]:
        try:
            user = find_user(uid)
            print(f"  用户 {uid}: 找到 → {user}")
        except RecordNotFoundError as e:
            print(f"  用户 {uid}: ✗ {e} (code={e.code})")

    # 异常层次
    print("\n异常层次结构:")
    print("  Exception")
    print("  └── AppError")
    print("      ├── ValidationError")
    print("      └── DatabaseError")
    print("          └── RecordNotFoundError")

    # 可以用父类捕获子类异常
    print("\n用父类捕获子类异常:")
    try:
        find_user(999)
    except AppError as e:
        print(f"  捕获 AppError（实际是 {type(e).__name__}）: {e}")


# =============================================================================
# 11.5 raise 和 assert
# =============================================================================

def divide(a: float, b: float) -> float:
    """演示 raise 的使用。"""
    if not isinstance(a, (int, float)):
        raise TypeError(f"参数 a 必须是数字，得到 {type(a).__name__}")
    if not isinstance(b, (int, float)):
        raise TypeError(f"参数 b 必须是数字，得到 {type(b).__name__}")
    if b == 0:
        raise ZeroDivisionError("除数不能为零")
    return a / b


def safe_sqrt(x: float) -> float:
    """安全的平方根（演示 raise ... from ... 异常链）。"""
    import math
    try:
        result = math.sqrt(x)
    except ValueError as e:
        # 用 raise ... from 创建异常链，保留原始异常信息
        raise ValueError(f"无法计算 {x} 的平方根（必须为非负数）") from e
    return result


def demo_raise_assert() -> None:
    """演示 raise 和 assert。"""
    print("\n" + "=" * 60)
    print("11.5 raise 和 assert")
    print("=" * 60)

    # ── raise ────────────────────────────────────────────
    print("raise 示例:")
    try:
        result = divide(10, 2)
        print(f"  10 / 2 = {result}")
    except (TypeError, ZeroDivisionError) as e:
        print(f"  错误: {e}")

    try:
        divide(10, 0)
    except ZeroDivisionError as e:
        print(f"  ✓ 捕获: {e}")

    try:
        divide("10", 2)
    except TypeError as e:
        print(f"  ✓ 捕获: {e}")

    # ── raise ... from（异常链）─────────────────────────
    print("\n异常链（raise ... from）:")
    try:
        safe_sqrt(-4)
    except ValueError as e:
        print(f"  ✓ 捕获: {e}")
        if e.__cause__:
            print(f"    原始原因: {e.__cause__}")

    # ── assert ───────────────────────────────────────────
    print("\nassert 示例:")
    print("  assert 用于调试阶段验证前提条件")
    print("  运行时加 -O 参数可禁用所有 assert")

    def compute_average(numbers: list) -> float:
        assert len(numbers) > 0, "列表不能为空！"
        return sum(numbers) / len(numbers)

    try:
        avg = compute_average([1, 2, 3, 4, 5])
        print(f"  平均值: {avg}")
        compute_average([])
    except AssertionError as e:
        print(f"  ✓ AssertionError: {e}")

    # ── re-raise（重新抛出）─────────────────────────────
    print("\n重新抛出异常（re-raise）:")

    def risky_operation():
        raise ValueError("原始错误")

    def outer():
        try:
            risky_operation()
        except ValueError as e:
            print(f"  outer 捕获到: {e}")
            print(f"  outer 记录日志后重新抛出...")
            raise  # 重新抛出同一个异常，保留堆栈信息

    try:
        outer()
    except ValueError as e:
        print(f"  最终捕获: {e}")


# =============================================================================
# 11.6 综合示例：健壮的配置文件加载器
# =============================================================================

class ConfigError(Exception):
    """配置相关异常基类。"""
    pass


class ConfigFileNotFoundError(ConfigError):
    """配置文件不存在。"""
    def __init__(self, path: str) -> None:
        super().__init__(f"配置文件不存在: {path}")
        self.path = path


class ConfigParseError(ConfigError):
    """配置文件解析错误。"""
    def __init__(self, path: str, line: int, detail: str) -> None:
        super().__init__(f"配置文件 {path} 第{line}行解析失败: {detail}")
        self.path = path
        self.line = line


class ConfigValueError(ConfigError):
    """配置值错误。"""
    def __init__(self, key: str, expected: str, got: str) -> None:
        super().__init__(f"配置项 '{key}' 类型错误: 期望 {expected}，得到 {got}")
        self.key = key


class Config:
    """
    简单的配置管理器。
    演示实际项目中的异常处理模式。
    """

    def __init__(self) -> None:
        self._data: dict[str, str] = {}

    def load_from_text(self, text: str, source: str = "<string>") -> None:
        """
        从文本加载配置（KEY=VALUE 格式）。
        演示分行解析和精确的错误报告。
        """
        for lineno, line in enumerate(text.strip().splitlines(), 1):
            line = line.strip()
            # 跳过注释和空行
            if not line or line.startswith("#"):
                continue
            # 解析 KEY=VALUE
            if "=" not in line:
                raise ConfigParseError(
                    source, lineno,
                    f"格式错误（缺少 '='），内容: {line!r}"
                )
            key, _, value = line.partition("=")
            key = key.strip()
            value = value.strip()
            if not key:
                raise ConfigParseError(source, lineno, "键名不能为空")
            self._data[key] = value

    def get(self, key: str, default: str = "") -> str:
        """获取配置值（字符串）。"""
        return self._data.get(key, default)

    def get_int(self, key: str, default: int | None = None) -> int:
        """获取整数配置值。"""
        value = self._data.get(key)
        if value is None:
            if default is not None:
                return default
            raise KeyError(f"配置项 '{key}' 不存在")
        try:
            return int(value)
        except ValueError:
            raise ConfigValueError(key, "整数", f"'{value}'") from None

    def get_bool(self, key: str, default: bool = False) -> bool:
        """获取布尔配置值。"""
        value = self._data.get(key)
        if value is None:
            return default
        if value.lower() in ("true", "1", "yes", "on"):
            return True
        if value.lower() in ("false", "0", "no", "off"):
            return False
        raise ConfigValueError(key, "布尔值", f"'{value}'")


def load_config_safely(config_text: str) -> Config | None:
    """安全地加载配置，演示完整的异常处理。"""
    config = Config()
    try:
        config.load_from_text(config_text)
    except ConfigParseError as e:
        print(f"  ❌ 解析错误: {e}")
        return None
    except ConfigError as e:
        print(f"  ❌ 配置错误: {e}")
        return None
    else:
        print(f"  ✓ 配置加载成功（{len(config._data)} 项）")
        return config
    finally:
        print(f"  [finally] load_config_safely 执行完毕")


def demo_config_loader() -> None:
    """演示配置加载器。"""
    print("\n" + "=" * 60)
    print("11.6 综合示例：配置文件加载器")
    print("=" * 60)

    # 正确的配置
    good_config_text = """
# 数据库配置
DB_HOST = localhost
DB_PORT = 5432
DB_NAME = myapp

# 应用配置
DEBUG = true
MAX_WORKERS = 4
APP_NAME = MyApplication
"""

    # 错误的配置（格式错误）
    bad_config_text = """
DB_HOST = localhost
INVALID LINE WITHOUT EQUALS
DEBUG = true
"""

    print("1. 加载正确的配置:")
    config = load_config_safely(good_config_text)
    if config:
        print(f"     DB_HOST     = {config.get('DB_HOST')!r}")
        print(f"     DB_PORT     = {config.get_int('DB_PORT')}")
        print(f"     DEBUG       = {config.get_bool('DEBUG')}")
        print(f"     MAX_WORKERS = {config.get_int('MAX_WORKERS')}")

        # 测试错误的访问
        try:
            config.get_int("TIMEOUT")  # 不存在的键
        except KeyError as e:
            print(f"     ✓ 缺失键: {e}")

        try:
            # 假装 APP_NAME 应该是整数
            config.get_int("APP_NAME")
        except ConfigValueError as e:
            print(f"     ✓ 类型错误: {e}")

    print("\n2. 加载错误的配置:")
    config2 = load_config_safely(bad_config_text)


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_exception_basics()
    demo_try_except_else_finally()
    demo_multiple_exceptions()
    demo_custom_exceptions()
    demo_raise_assert()
    demo_config_loader()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 基本结构 ──
# try:
#     可能出错的代码
# except SomeError as e:
#     处理特定异常
# except (Error1, Error2) as e:
#     同时处理多种异常
# except Exception as e:
#     捕获所有异常（兜底，谨慎使用）
# else:
#     try 块没有异常时执行
# finally:
#     无论如何都执行（清理资源）
#
# ── 主动抛出 ──
# raise ValueError("错误消息")
# raise ValueError("新错误") from original_error  # 异常链
# raise  # 重新抛出当前异常
#
# ── 断言 ──
# assert 条件, "错误消息"
#
# ── 自定义异常 ──
# class MyError(Exception):
#     def __init__(self, message, code=0):
#         super().__init__(message)
#         self.code = code
#
# ── 常见内置异常 ──
# Exception           所有非系统异常的基类
# ValueError          值不合适
# TypeError           类型不对
# KeyError            字典键不存在
# IndexError          列表索引越界
# AttributeError      属性不存在
# FileNotFoundError   文件不存在
# ZeroDivisionError   除以零
# ImportError         导入失败
# PermissionError     权限不足
# RuntimeError        运行时错误（通用）
# StopIteration       迭代结束


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：捕获太宽泛（隐藏真正的 bug）
# try:
#     ...
# except:           # ❌ 捕获所有，包括 KeyboardInterrupt
#     pass
# except Exception: # ⚠️  也很宽泛，但至少不捕获系统异常
#     pass
#
# 错误 2：忽略异常而不记录
# try:
#     risky()
# except Exception:
#     pass  # ❌ 静默忽略，调试困难
#
# 错误 3：在 finally 中 return 会覆盖异常
# def func():
#     try:
#         raise ValueError("error")
#     finally:
#         return 42  # ❌ 异常被吞掉了！
#
# 错误 4：捕获异常后重新抛出时丢失上下文
# except ValueError as e:
#     raise RuntimeError("失败")       # ⚠️ 丢失原始异常
#     raise RuntimeError("失败") from e # ✅ 保留异常链


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   写一个函数 safe_divide(a, b)，满足：
#   - 如果 b == 0，返回 None 并打印警告
#   - 如果 a 或 b 不是数字，抛出 TypeError
#   - 正常情况下返回 a / b
#   - 用 finally 打印 "操作完成"
#
# 练习 2：
#   创建一个自定义异常体系，用于一个用户注册场景：
#   - UserError（基类）
#   - UsernameError（用户名问题）：过短、含非法字符
#   - PasswordError（密码问题）：过短、不包含数字
#   - EmailError（邮箱问题）：格式不正确
#   写一个 register(username, password, email) 函数，进行验证
#
# 练习 3：
#   写一个 retry(func, max_attempts=3, exceptions=(Exception,)) 函数：
#   - 调用 func()，如果抛出 exceptions 中的异常，自动重试
#   - 最多重试 max_attempts 次
#   - 如果全部失败，抛出最后一次的异常
#
# 练习答案提示：
#   练习1：注意 try/except/finally 的执行顺序
#   练习2：每个验证函数只抛出对应的异常类型
#   练习3：使用 for 循环，用 raise 在最后一次失败时重新抛出


# =============================================================================
# 【练习答案】
# =============================================================================

def safe_divide(a: float, b: float) -> float | None:
    """
    练习 1：安全除法函数。
    - 验证参数类型
    - 处理除零错误（返回 None 并打印警告）
    - 使用 finally 打印操作完成
    """
    try:
        # 类型验证
        if not isinstance(a, (int, float)):
            raise TypeError(f"参数 a 必须是数字，得到 {type(a).__name__}")
        if not isinstance(b, (int, float)):
            raise TypeError(f"参数 b 必须是数字，得到 {type(b).__name__}")

        # 执行除法
        result = a / b

    except ZeroDivisionError:
        print(f"  ⚠️  警告：除数为零，返回 None")
        return None

    else:
        # 没有异常时执行
        return result

    finally:
        # 无论如何都执行
        print(f"  操作完成")


# ── 练习 2：用户注册异常体系 ─────────────────────────────────────────────────

class UserError(Exception):
    """用户相关异常基类。"""
    pass


class UsernameError(UserError):
    """用户名异常。"""

    def __init__(self, reason: str, username: str) -> None:
        super().__init__(f"用户名 {username!r} 无效: {reason}")
        self.username = username
        self.reason = reason


class PasswordError(UserError):
    """密码异常。"""

    def __init__(self, reason: str) -> None:
        super().__init__(f"密码无效: {reason}")
        self.reason = reason


class EmailError(UserError):
    """邮箱异常。"""

    def __init__(self, email: str) -> None:
        super().__init__(f"邮箱格式不正确: {email!r}")
        self.email = email


def register(username: str, password: str, email: str) -> dict:
    """
    练习 2：用户注册函数，带完整验证。

    验证规则：
    - 用户名：至少3位，只能包含字母、数字和下划线
    - 密码：至少8位，必须包含至少一个数字
    - 邮箱：基本格式验证

    Returns:
        注册成功时返回用户信息字典

    Raises:
        UsernameError: 用户名不符合要求
        PasswordError: 密码不符合要求
        EmailError: 邮箱格式不正确
    """
    import re

    # 验证用户名
    if len(username) < 3:
        raise UsernameError("长度不能少于3位", username)
    if not re.match(r'^[a-zA-Z0-9_]+$', username):
        raise UsernameError("只能包含字母、数字和下划线", username)

    # 验证密码
    if len(password) < 8:
        raise PasswordError("长度不能少于8位")
    if not any(c.isdigit() for c in password):
        raise PasswordError("必须包含至少一个数字")

    # 验证邮箱（简单的正则验证）
    if not re.match(r'^[\w.+-]+@[\w-]+\.[a-zA-Z]{2,}$', email):
        raise EmailError(email)

    # 验证通过，返回用户信息
    return {
        "username": username,
        "email": email,
        "status": "registered",
    }


# ── 练习 3：retry 重试函数 ───────────────────────────────────────────────────

def retry(func, max_attempts: int = 3, exceptions: tuple = (Exception,)):
    """
    练习 3：重试函数装饰器（函数版本）。

    自动重试失败的函数调用，最多重试 max_attempts 次。
    如果全部失败，抛出最后一次的异常。

    Args:
        func: 要执行的函数（无参数）
        max_attempts: 最大尝试次数
        exceptions: 要捕获的异常类型元组

    Returns:
        func 的返回值

    Raises:
        最后一次失败时的异常
    """
    last_exception = None

    for attempt in range(1, max_attempts + 1):
        try:
            return func()  # 尝试执行函数
        except exceptions as e:
            last_exception = e
            print(f"  第 {attempt} 次尝试失败: {type(e).__name__}: {e}")

            # 如果还有重试机会，继续循环
            if attempt < max_attempts:
                continue
            # 最后一次失败，重新抛出异常
            raise


def exercise1_answer() -> None:
    """练习 1：演示 safe_divide 函数。"""
    print("safe_divide 演示:")

    # 正常情况
    result = safe_divide(10, 2)
    print(f"  10 / 2 = {result}")

    # 除零情况
    result = safe_divide(10, 0)
    print(f"  10 / 0 = {result}")

    # 类型错误
    try:
        safe_divide("10", 2)
    except TypeError as e:
        print(f"  ✓ 类型错误被捕获: {e}")

    try:
        safe_divide(10, "2")
    except TypeError as e:
        print(f"  ✓ 类型错误被捕获: {e}")


def exercise2_answer() -> None:
    """练习 2：演示用户注册异常体系。"""
    print("用户注册验证演示:")

    test_cases = [
        ("alice123", "password123", "alice@example.com"),  # 正常
        ("ab", "password123", "alice@example.com"),        # 用户名太短
        ("alice@123", "password123", "alice@example.com"), # 用户名含非法字符
        ("alice123", "short", "alice@example.com"),        # 密码太短
        ("alice123", "password", "alice@example.com"),     # 密码无数字
        ("alice123", "password123", "invalid-email"),      # 邮箱格式错误
    ]

    for username, password, email in test_cases:
        try:
            user = register(username, password, email)
            print(f"  ✅ 注册成功: {user['username']} ({user['email']})")
        except UsernameError as e:
            print(f"  ❌ {e}")
        except PasswordError as e:
            print(f"  ❌ {e}")
        except EmailError as e:
            print(f"  ❌ {e}")
        except UserError as e:
            # 兜底：捕获所有 UserError 子类
            print(f"  ❌ 用户错误: {e}")


def exercise3_answer() -> None:
    """练习 3：演示 retry 重试函数。"""
    print("retry 重试函数演示:")

    # 模拟不稳定的函数
    attempt_count = 0

    def unstable_function():
        nonlocal attempt_count
        attempt_count += 1
        if attempt_count < 3:
            raise ConnectionError(f"连接失败（第 {attempt_count} 次）")
        return "成功！"

    print("\n1. 最终成功的场景:")
    attempt_count = 0
    try:
        result = retry(unstable_function, max_attempts=3, exceptions=(ConnectionError,))
        print(f"  ✅ 最终结果: {result}")
    except ConnectionError as e:
        print(f"  ❌ 全部失败: {e}")

    # 全部失败的场景
    def always_fail():
        raise ValueError("总是失败")

    print("\n2. 全部失败的场景:")
    try:
        retry(always_fail, max_attempts=3, exceptions=(ValueError,))
    except ValueError as e:
        print(f"  ✅ 捕获最终异常: {e}")

    # 只重试特定异常
    def mixed_errors():
        import random
        if random.random() < 0.5:
            raise ConnectionError("网络错误")
        else:
            raise ValueError("数据错误")

    print("\n3. 只重试特定异常（ConnectionError）:")
    import random
    random.seed(42)  # 固定随机种子，使结果可预测
    try:
        retry(mixed_errors, max_attempts=5, exceptions=(ConnectionError,))
    except (ConnectionError, ValueError) as e:
        print(f"  ✅ 捕获异常: {type(e).__name__}: {e}")


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
