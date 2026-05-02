# =============================================================================
# 第 21 章：补充 1 - 数学与数值工具
# =============================================================================
#
# 【学习目标】
#   1. 掌握 math 模块的常用数学函数
#   2. 掌握 random 模块的随机数和抽样方法
#   3. 掌握 statistics 模块的基础统计函数
#   4. 理解 Decimal 适合处理金额等精确小数
#   5. 理解 Fraction 适合处理有理数
#
# 【运行方式】
#   python chapter21_补充1_数学与数值工具.py
#
# =============================================================================

from decimal import Decimal, ROUND_HALF_UP, getcontext
from fractions import Fraction
import math
import random
import statistics


# =============================================================================
# 21.补充1.1 math 模块
# =============================================================================


def circle_area(radius: float) -> float:
    """计算圆面积。"""
    if radius < 0:
        raise ValueError("半径不能为负数")
    return math.pi * radius**2


def demo_math_module() -> None:
    """演示 math 模块常用函数。"""
    print("=" * 60)
    print("21.补充1.1 math 模块")
    print("=" * 60)

    print("pi:", round(math.pi, 6))
    print("sqrt(16):", math.sqrt(16))
    print("ceil(3.2):", math.ceil(3.2))
    print("floor(3.8):", math.floor(3.8))
    print("sin(pi / 2):", math.sin(math.pi / 2))
    print("circle_area(2):", round(circle_area(2), 4))


# =============================================================================
# 21.补充1.2 random 模块
# =============================================================================


def roll_dice(times: int, seed: int | None = None) -> list[int]:
    """掷骰子，返回每次点数。"""
    rng = random.Random(seed)
    return [rng.randint(1, 6) for _ in range(times)]


def demo_random_module() -> None:
    """演示 random 模块常用方法。"""
    print("\n" + "=" * 60)
    print("21.补充1.2 random 模块")
    print("=" * 60)

    numbers = list(range(1, 11))
    rng = random.Random(42)

    print("随机小数:", round(rng.random(), 4))
    print("随机整数:", rng.randint(1, 100))
    print("抽样 3 个:", rng.sample(numbers, 3))

    rng.shuffle(numbers)
    print("打乱列表:", numbers)
    print("掷骰子:", roll_dice(5, seed=7))


# =============================================================================
# 21.补充1.3 statistics 模块
# =============================================================================


def summarize_scores(scores: list[float]) -> dict[str, float]:
    """统计分数的均值、中位数和标准差。"""
    if not scores:
        raise ValueError("scores 不能为空")

    return {
        "mean": statistics.mean(scores),
        "median": statistics.median(scores),
        "pstdev": statistics.pstdev(scores),
    }


def demo_statistics_module() -> None:
    """演示 statistics 模块。"""
    print("\n" + "=" * 60)
    print("21.补充1.3 statistics 模块")
    print("=" * 60)

    scores = [88, 92, 75, 91, 84, 92]
    summary = summarize_scores(scores)

    print("数据:", scores)
    print("均值:", round(summary["mean"], 2))
    print("中位数:", summary["median"])
    print("总体标准差:", round(summary["pstdev"], 2))
    print("众数:", statistics.mode(scores))


# =============================================================================
# 21.补充1.4 Decimal 精确小数
# =============================================================================


def calculate_total_price(prices: list[str], tax_rate: str = "0.06") -> Decimal:
    """用 Decimal 计算含税总价，适合金额计算。"""
    subtotal = sum(Decimal(price) for price in prices)
    total = subtotal * (Decimal("1") + Decimal(tax_rate))
    return total.quantize(Decimal("0.01"), rounding=ROUND_HALF_UP)


def demo_decimal_module() -> None:
    """演示 Decimal 避免二进制浮点误差。"""
    print("\n" + "=" * 60)
    print("21.补充1.4 Decimal 精确小数")
    print("=" * 60)

    print("float: 0.1 + 0.2 =", 0.1 + 0.2)
    print("Decimal: 0.1 + 0.2 =", Decimal("0.1") + Decimal("0.2"))

    getcontext().prec = 28
    print("含税总价:", calculate_total_price(["19.90", "8.80", "3.50"]))


# =============================================================================
# 21.补充1.5 Fraction 有理数
# =============================================================================


def add_fractions(a: str, b: str) -> Fraction:
    """把两个分数字符串相加。"""
    return Fraction(a) + Fraction(b)


def demo_fraction_module() -> None:
    """演示 Fraction 精确表示有理数。"""
    print("\n" + "=" * 60)
    print("21.补充1.5 Fraction 有理数")
    print("=" * 60)

    value = Fraction(1, 3) + Fraction(1, 6)
    print("1/3 + 1/6 =", value)
    print("0.75 转 Fraction:", Fraction("0.75"))
    print("2/5 + 1/10 =", add_fractions("2/5", "1/10"))


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_math_module()
    demo_random_module()
    demo_statistics_module()
    demo_decimal_module()
    demo_fraction_module()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. math
#    - math.pi、math.sqrt、math.ceil、math.floor
#    - 三角函数、对数、幂运算等数学工具
#
# 2. random
#    - random.Random(seed)：创建独立随机数生成器
#    - randint、choice、sample、shuffle
#    - 教学和测试中使用 seed 可得到稳定结果
#
# 3. statistics
#    - mean、median、mode、pstdev、stdev
#    - 适合基础统计，不替代专业数据分析库
#
# 4. decimal.Decimal
#    - 用字符串创建 Decimal，避免 float 误差
#    - 金额计算常配合 quantize 做四舍五入
#
# 5. fractions.Fraction
#    - 精确表达有理数，例如 1/3、2/5
#    - 适合比例、分数和精确有理数运算


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个函数 distance(x1, y1, x2, y2)，返回两点之间的距离。
#
# 练习 2（基础）：
#   写一个函数 random_password(length, seed=None)，从大小写字母和数字中生成密码。
#
# 练习 3（进阶）：
#   写一个函数 split_bill(total, people)，使用 Decimal 把账单平均分摊到每个人。
#
# 练习答案提示：
#   练习1：使用 math.hypot 或平方和开根号
#   练习2：使用 random.Random(seed) 和 string.ascii_letters
#   练习3：使用 Decimal(str(total))，最后 quantize 到 0.01


# =============================================================================
# 【练习答案】
# =============================================================================


def distance(x1: float, y1: float, x2: float, y2: float) -> float:
    """练习 1 答案：计算两点距离。"""
    return math.hypot(x2 - x1, y2 - y1)


def split_bill(total: str, people: int) -> Decimal:
    """练习 3 答案：平均分摊账单。"""
    if people <= 0:
        raise ValueError("人数必须大于 0")
    amount = Decimal(total) / Decimal(people)
    return amount.quantize(Decimal("0.01"), rounding=ROUND_HALF_UP)


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 40)
#     print(distance(0, 0, 3, 4))
#
#     print("\n" + "=" * 40)
#     print(split_bill("128.00", 3))
