"""
mymodule.py - 自定义数学工具模块

这是一个示例模块，演示模块的基本结构。
运行方式：
    python mymodule.py          # 直接运行（会执行测试代码）
    python -c "import mymodule" # 作为模块导入
"""

# 模块级变量
PI = 3.14159
VERSION = "1.0.0"

# 私有变量（约定以 _ 开头，外部不应直接访问）
_internal_cache: dict = {}


def add(a: float, b: float) -> float:
    """两数相加。"""
    return a + b


def multiply(a: float, b: float) -> float:
    """两数相乘。"""
    return a * b


def _helper_function() -> None:
    """私有辅助函数（约定以 _ 开头，不应被外部使用）。"""
    pass


class Calculator:
    """简单的计算器类。"""

    def __init__(self, name: str = "默认计算器") -> None:
        self.name = name
        self.history: list[str] = []

    def calculate(self, a: float, b: float, op: str) -> float:
        """执行计算并记录历史。"""
        if op == "+":
            result = add(a, b)
        elif op == "*":
            result = multiply(a, b)
        elif op == "-":
            result = a - b
        elif op == "/":
            if b == 0:
                raise ValueError("除数不能为零")
            result = a / b
        else:
            raise ValueError(f"不支持的操作: {op}")

        self.history.append(f"{a} {op} {b} = {result}")
        return result

    def show_history(self) -> None:
        """打印计算历史。"""
        if not self.history:
            print("  暂无计算历史")
        for entry in self.history:
            print(f"  {entry}")


# 模块初始化代码（导入时执行一次）
print(f"[mymodule] 模块已加载，版本 {VERSION}")


# 测试代码（只在直接运行时执行）
if __name__ == "__main__":
    print("\n直接运行 mymodule.py 的测试代码：")
    print(f"  PI = {PI}")
    print(f"  add(3, 5) = {add(3, 5)}")
    print(f"  multiply(4, 7) = {multiply(4, 7)}")

    calc = Calculator("测试计算器")
    print(f"  计算器名称: {calc.name}")
    print(f"  3 + 4 = {calc.calculate(3, 4, '+')}")
    print(f"  10 - 2 = {calc.calculate(10, 2, '-')}")
    print(f"  6 * 7 = {calc.calculate(6, 7, '*')}")
    print(f"  15 / 3 = {calc.calculate(15, 3, '/')}")
    print("\n  计算历史:")
    calc.show_history()
