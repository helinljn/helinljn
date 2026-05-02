"""
mypackage.module2 - 数学工具模块

演示包中另一个子模块的编写方式。
"""

import math
from typing import Sequence


class Calculator:
    """多功能计算器类。"""

    def __init__(self, name: str = "计算器") -> None:
        self.name = name

    @staticmethod
    def mean(numbers: Sequence[float]) -> float:
        """计算平均值。"""
        if not numbers:
            raise ValueError("列表不能为空")
        return sum(numbers) / len(numbers)

    @staticmethod
    def variance(numbers: Sequence[float]) -> float:
        """计算方差。"""
        if len(numbers) < 2:
            return 0.0
        avg = sum(numbers) / len(numbers)
        return sum((x - avg) ** 2 for x in numbers) / (len(numbers) - 1)

    @staticmethod
    def std_dev(numbers: Sequence[float]) -> float:
        """计算标准差。"""
        return math.sqrt(Calculator.variance(numbers))

    @staticmethod
    def factorial(n: int) -> int:
        """计算阶乘（使用 math.factorial）。"""
        if n < 0:
            raise ValueError("阶乘只对非负整数定义")
        return math.factorial(n)


def is_prime(n: int) -> bool:
    """判断一个数是否为质数。"""
    if n < 2:
        return False
    for i in range(2, int(n ** 0.5) + 1):
        if n % i == 0:
            return False
    return True


def fibonacci(n: int) -> list[int]:
    """生成前 n 个斐波那契数。"""
    if n <= 0:
        return []
    if n == 1:
        return [0]
    seq = [0, 1]
    for _ in range(2, n):
        seq.append(seq[-1] + seq[-2])
    return seq


# 测试代码
if __name__ == "__main__":
    print("module2 直接运行测试：")
    calc = Calculator("测试")
    data = [2, 4, 6, 8, 10]
    print(f"  数据: {data}")
    print(f"  平均值: {calc.mean(data)}")
    print(f"  标准差: {calc.std_dev(data):.4f}")
    print(f"  5! = {calc.factorial(5)}")
    print(f"  前10个斐波那契数: {fibonacci(10)}")
    print(f"  100以内的质数: {[n for n in range(101) if is_prime(n)]}")
