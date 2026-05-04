# =============================================================================
# 综合练习 01：Python 基础篇
# 覆盖：ch01 入门与环境、ch02 基础语法与数据类型、ch03 控制流程、
#       ch04 字符串处理、ch05 容器数据类型
# =============================================================================

# =============================================================================
# 练习 1：数字黑洞（Kaprekar 常数）
# =============================================================================
# 任取一个四位数（4个数字不完全相同），按以下步骤操作：
#   1. 将数字从大到小排列得到最大数
#   2. 将数字从小到大排列得到最小数
#   3. 用最大数减去最小数得到新数
#   4. 重复以上步骤，最终会到达 6174（Kaprekar 常数）
#
# 要求：
#   - 写函数 kaprekar_steps(n: int) -> tuple[int, list[int]]
#     返回 (步数, 中间值列表)
#   - 如果输入不是有效的四位数（如 1111），返回 (-1, [])
#   - 示例：kaprekar_steps(3524) → (3, [4087, 8172, 7443, 6174])


# =============================================================================
# 练习 2：文本中的数字提取器
# =============================================================================
# 给定一个字符串，提取其中所有的数字（包括整数和小数），
# 然后计算它们的总和、平均值、最大值和最小值。
#
# 要求：
#   - 写函数 extract_and_analyze(text: str) -> dict
#     返回 {"numbers": [提取的数字], "sum": 总和, "avg": 平均值,
#            "max": 最大值, "min": 最小值, "count": 个数}
#   - 如果没有数字，返回空结果
#   - 示例：extract_and_analyze("价格12.5元，数量3个，折扣0.8")
#     → {"numbers": [12.5, 3.0, 0.8], "sum": 16.3, "avg": 5.433..., ...}


# =============================================================================
# 练习 3：杨辉三角生成器
# =============================================================================
# 生成杨辉三角的前 n 行。
# 杨辉三角的每一行第一个和最后一个元素都是 1，中间每个元素等于上一行相邻两元素之和。
#
# 要求：
#   - 写函数 pascal_triangle(n: int) -> list[list[int]]
#   - 输入验证：n 必须 >= 0
#   - 示例：pascal_triangle(5)
#     → [[1], [1,1], [1,2,1], [1,3,3,1], [1,4,6,4,1]]


# =============================================================================
# 练习 4：字符频率统计与分析
# =============================================================================
# 统计一段文本中每个字符（字母、数字、标点等）的出现频率，
# 并按频率从高到低排序输出。
#
# 要求：
#   - 写函数 char_frequency_report(text: str) -> str
#   - 返回一个格式化的表格字符串，包含：排名、字符、次数、频率百分比
#   - 忽略大小写差异（将字母统一转为小写统计）
#   - 只统计字母和数字（a-z, 0-9）
#   - 用集合（set）记录出现过哪些不同的字符类型


# =============================================================================
# 练习 5：简易购物系统
# =============================================================================
# 实现一个简单的购物系统：
#
# 要求：
#   - 使用字典存储商品：{"商品名": {"price": 价格, "stock": 库存}}
#   - 写函数 add_to_cart(cart, product_name, quantity, inventory)
#     → 从库存中扣减，加入购物车，库存不足时抛出异常
#   - 写函数 checkout(cart, inventory)
#     → 返回总价和购物清单
#   - 写函数 show_inventory(inventory) -> str
#     → 返回格式化的库存表格
#   - 使用集合（set）存储所有曾加入购物车的商品类型


# =============================================================================
# ============================= 答案分隔线 ====================================
# =============================================================================


# =============================================================================
# 练习 1 答案：数字黑洞
# =============================================================================

def kaprekar_steps(n: int) -> tuple[int, list[int]]:
    """计算到达 Kaprekar 常数 6174 所需的步数和中间值。"""
    # 验证输入
    s = str(n).zfill(4)
    if len(set(s)) == 1 or len(s) != 4 or n <= 0:
        return (-1, [])

    steps = 0
    sequence = []

    while n != 6174:
        # 补零到4位
        digits = str(n).zfill(4)
        # 从大到小排列
        big = int("".join(sorted(digits, reverse=True)))
        # 从小到大排列
        small = int("".join(sorted(digits)))
        n = big - small
        steps += 1
        sequence.append(n)

        # 防止无限循环（安全退出）
        if steps > 20:
            break

    return (steps, sequence)


# =============================================================================
# 练习 2 答案：数字提取器
# =============================================================================

import re


def extract_and_analyze(text: str) -> dict:
    """提取文本中的数字并进行分析。"""
    # 匹配整数和小数（包括负数）
    pattern = r"-?\d+\.?\d*"
    matches = re.findall(pattern, text)

    if not matches:
        return {"numbers": [], "sum": 0, "avg": 0, "max": None, "min": None, "count": 0}

    numbers = [float(m) for m in matches]
    return {
        "numbers": numbers,
        "sum": sum(numbers),
        "avg": round(sum(numbers) / len(numbers), 4),
        "max": max(numbers),
        "min": min(numbers),
        "count": len(numbers),
    }


# =============================================================================
# 练习 3 答案：杨辉三角
# =============================================================================

def pascal_triangle(n: int) -> list[list[int]]:
    """生成杨辉三角的前 n 行。"""
    if n < 0:
        raise ValueError("n 必须 >= 0")
    if n == 0:
        return []

    triangle = [[1]]

    for i in range(1, n):
        prev = triangle[-1]
        # 首尾为 1，中间为相邻两元素之和
        current = [1] + [prev[j] + prev[j + 1] for j in range(len(prev) - 1)] + [1]
        triangle.append(current)

    return triangle


# =============================================================================
# 练习 4 答案：字符频率统计
# =============================================================================

def char_frequency_report(text: str) -> str:
    """统计字符频率并生成格式化报告。"""
    # 统一转为小写，只保留字母和数字
    filtered = [c.lower() for c in text if c.isalnum()]
    total = len(filtered)

    if total == 0:
        return "无有效字符"

    # 用字典统计频率
    freq: dict[str, int] = {}
    for c in filtered:
        freq[c] = freq.get(c, 0) + 1

    # 按频率降序排序
    sorted_freq = sorted(freq.items(), key=lambda x: (-x[1], x[0]))

    # 字符类型统计（用集合）
    letters = {c for c in freq if c.isalpha()}
    digits = {c for c in freq if c.isdigit()}

    # 生成表格
    lines = [f"{'排名':<6} {'字符':<8} {'次数':<8} {'频率':<10}"]
    lines.append("-" * 35)
    for rank, (char, count) in enumerate(sorted_freq, 1):
        percentage = count / total * 100
        lines.append(f"{rank:<6} {char:<8} {count:<8} {percentage:>6.2f}%")

    lines.append(f"\n字母种类: {len(letters)}, 数字种类: {len(digits)}")
    return "\n".join(lines)


# =============================================================================
# 练习 5 答案：简易购物系统
# =============================================================================

class ShoppingCart:
    """简易购物车类。"""

    def __init__(self) -> None:
        self.items: dict[str, int] = {}       # {商品名: 数量}
        self.product_types: set[str] = set()  # 用集合记录所有曾加入的商品类型

    def add(self, product_name: str, quantity: int, inventory: dict) -> None:
        """添加商品到购物车。"""
        if product_name not in inventory:
            raise ValueError(f"商品 '{product_name}' 不存在")

        item = inventory[product_name]
        if item["stock"] < quantity:
            raise ValueError(
                f"库存不足！{product_name} 剩余库存: {item['stock']}，"
                f"需要: {quantity}"
            )

        # 扣减库存
        item["stock"] -= quantity

        # 加入购物车
        self.items[product_name] = self.items.get(product_name, 0) + quantity
        self.product_types.add(product_name)

    def checkout(self, inventory: dict) -> tuple[float, list[str]]:
        """结账：根据库存中的价格返回总价和购物清单。"""
        if not self.items:
            return (0.0, [])

        total = 0.0
        receipt = []
        receipt.append("=" * 40)
        receipt.append(f"{'商品':<12} {'单价':>8} {'数量':>6} {'小计':>10}")
        receipt.append("-" * 40)

        for name, qty in self.items.items():
            price = inventory[name]["price"]
            subtotal = price * qty
            total += subtotal
            receipt.append(f"{name:<12} {price:>8.2f} {qty:>6} {subtotal:>10.2f}")

        receipt.append("-" * 40)
        receipt.append(f"{'合计':<12} {'':>8} {'':>6} {total:>10.2f}")
        receipt.append("=" * 40)
        return (total, receipt)


def add_to_cart(cart: ShoppingCart, product_name: str,
                quantity: int, inventory: dict) -> str:
    """将商品加入购物车（函数式接口）。"""
    try:
        cart.add(product_name, quantity, inventory)
        return f"✅ 已添加 {quantity} 个 {product_name} 到购物车"
    except ValueError as e:
        return f"❌ {e}"


def checkout(cart: ShoppingCart, inventory: dict) -> tuple[float, list[str]]:
    """结账并返回总价和清单。"""
    return cart.checkout(inventory)


def show_inventory(inventory: dict) -> str:
    """显示库存信息表格。"""
    lines = [f"{'商品':<12} {'单价':>8} {'库存':>6}"]
    lines.append("-" * 28)
    for name, info in inventory.items():
        lines.append(f"{name:<12} {info['price']:>8.2f} {info['stock']:>6}")
    return "\n".join(lines)


# =============================================================================
# 运行所有练习
# =============================================================================

if __name__ == "__main__":
    print("=" * 60)
    print("综合练习 01：Python 基础篇")
    print("=" * 60)

    # 练习 1
    print("\n【练习 1】Kaprekar 常数")
    steps, seq = kaprekar_steps(3524)
    print(f"  3524 → 步数: {steps}, 序列: {seq}")
    steps, seq = kaprekar_steps(6174)
    print(f"  6174 → 步数: {steps}")
    steps, seq = kaprekar_steps(1111)
    print(f"  1111 → (无效输入) 步数: {steps}")

    # 练习 2
    print("\n【练习 2】数字提取器")
    result = extract_and_analyze("价格12.5元，数量3个，折扣0.8，温度-5度")
    print(f"  提取的数字: {result['numbers']}")
    print(f"  总和: {result['sum']}, 平均: {result['avg']}")
    print(f"  最大: {result['max']}, 最小: {result['min']}")

    # 练习 3
    print("\n【练习 3】杨辉三角")
    triangle = pascal_triangle(5)
    for row in triangle:
        print(f"  {row}")

    # 练习 4
    print("\n【练习 4】字符频率统计")
    text = "Hello, World! Python 3.11 is awesome."
    print(char_frequency_report(text))

    # 练习 5
    print("\n【练习 5】简易购物系统")
    inventory = {
        "苹果":   {"price": 5.0, "stock": 100},
        "香蕉":   {"price": 3.0, "stock": 50},
        "橙子":   {"price": 4.0, "stock": 30},
        "笔记本": {"price": 25.0, "stock": 10},
    }
    cart = ShoppingCart()
    print(show_inventory(inventory))
    print(f"\n  {add_to_cart(cart, '苹果', 3, inventory)}")
    print(f"  {add_to_cart(cart, '笔记本', 2, inventory)}")
    print(f"  {add_to_cart(cart, '西瓜', 1, inventory)}")  # 不存在的商品
    total, receipt = checkout(cart, inventory)
    for line in receipt:
        print(f"  {line}")
