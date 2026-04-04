# =============================================================================
# 第 3 章：控制流程
# =============================================================================
#
# 【学习目标】
#   1. 掌握 if-elif-else 条件语句
#   2. 掌握 while 和 for 循环
#   3. 理解 break、continue、pass 的用法
#   4. 理解循环的 else 子句（Python 特有）
#   5. 掌握条件表达式（三元运算符）
#
# 【运行方式】
#   python chapter03_控制流程.py
#
# =============================================================================


# =============================================================================
# 3.1 条件语句（if-elif-else）
# =============================================================================
#
# C/C++ 写法：
#   if (x > 0) {
#       printf("正数\n");
#   } else if (x < 0) {
#       printf("负数\n");
#   } else {
#       printf("零\n");
#   }
#
# Python 写法：
#   if x > 0:
#       print("正数")
#   elif x < 0:
#       print("负数")
#   else:
#       print("零")
#
# 关键差异：
#   1. 条件不需要括号（但加括号也不报错）
#   2. 用 elif 而不是 else if
#   3. 用冒号 : 结尾
#   4. 用缩进代替大括号

def demo_if_statement() -> None:
    """演示 if-elif-else 条件语句。"""
    print("=" * 60)
    print("3.1 条件语句（if-elif-else）")
    print("=" * 60)

    # 基本 if-elif-else
    def classify_number(n: int) -> str:
        """将数字分类为正数、负数或零。"""
        if n > 0:
            return "正数"
        elif n < 0:
            return "负数"
        else:
            return "零"

    for n in [10, -5, 0]:
        print(f"  {n} 是{classify_number(n)}")

    # 嵌套 if（多层条件）
    print("\n成绩等级判断:")
    def get_grade(score: int) -> str:
        """根据分数返回等级。"""
        if score < 0 or score > 100:
            return "分数无效"
        elif score >= 90:
            return "A（优秀）"
        elif score >= 80:
            return "B（良好）"
        elif score >= 70:
            return "C（中等）"
        elif score >= 60:
            return "D（及格）"
        else:
            return "F（不及格）"

    test_scores = [95, 83, 71, 62, 45, 101]
    for score in test_scores:
        print(f"  {score} 分 → {get_grade(score)}")

    # 条件表达式（三元运算符）
    # C/C++: (condition) ? value_if_true : value_if_false
    # Python: value_if_true if condition else value_if_false
    print("\n条件表达式（三元运算符）:")
    x = 10
    result = "偶数" if x % 2 == 0 else "奇数"
    print(f"  {x} 是{result}")

    age = 20
    status = "成年人" if age >= 18 else "未成年人"
    print(f"  年龄 {age}：{status}")

    # 条件表达式可以嵌套（不推荐，可读性差）
    n = 5
    sign = "正" if n > 0 else ("负" if n < 0 else "零")
    print(f"  {n} 是{sign}数")

    # 利用真值测试简化条件
    print("\n利用真值测试:")
    name = ""           # 空字符串是假值
    display = name or "匿名用户"  # 如果 name 为空，使用默认值
    print(f"  显示名称: {display}")

    name = "Alice"
    display = name or "匿名用户"
    print(f"  显示名称: {display}")

    # Python 没有 switch/case（Python 3.10+ 有 match-case，见下面）
    # Python 3.10 之前的替代方案：用字典模拟
    print("\n用字典模拟 switch-case:")
    def get_day_name(day: int) -> str:
        """返回星期几的名称。"""
        days = {
            1: "星期一", 2: "星期二", 3: "星期三",
            4: "星期四", 5: "星期五", 6: "星期六", 7: "星期日"
        }
        return days.get(day, "无效的星期数")  # get 方法：键不存在时返回默认值

    for d in [1, 5, 7, 8]:
        print(f"  第 {d} 天: {get_day_name(d)}")

    # Python 3.10+ 的 match-case（结构模式匹配）
    print("\nmatch-case 语句（Python 3.10+）:")
    def describe_point(point: tuple) -> str:
        """描述一个坐标点。"""
        match point:
            case (0, 0):
                return "原点"
            case (x, 0):
                return f"X 轴上的点，x={x}"
            case (0, y):
                return f"Y 轴上的点，y={y}"
            case (x, y):
                return f"普通点 ({x}, {y})"
            case _:
                return "未知"

    for p in [(0, 0), (3, 0), (0, 4), (2, 5)]:
        print(f"  {p} → {describe_point(p)}")


# =============================================================================
# 3.2 while 循环
# =============================================================================
#
# C/C++ 写法：
#   while (条件) {
#       循环体;
#   }
#
# Python 写法：
#   while 条件:
#       循环体

def demo_while_loop() -> None:
    """演示 while 循环。"""
    print("\n" + "=" * 60)
    print("3.2 while 循环")
    print("=" * 60)

    # 基本 while 循环：计算 1 到 10 的累加和
    total = 0
    i = 1
    while i <= 10:
        total += i    # 等同于 total = total + i
        i += 1        # 等同于 i = i + 1（Python 没有 i++ 语法！）

    print(f"1 到 10 的累加和: {total}")   # 55

    # 【注意】Python 没有 i++ 和 --i 这样的自增自减运算符
    # 必须写成 i += 1 或 i -= 1

    # while + break：找到第一个满足条件的数
    print("\n使用 break 提前退出:")
    target = 37
    n = 1
    while n <= 100:
        if n * n > target:
            break           # 退出循环
        n += 1
    print(f"  {target} 的整数平方根（向下取整）: {n - 1}")

    # while + continue：跳过某些迭代
    print("\n使用 continue 跳过奇数（只打印偶数）:")
    n = 0
    result = []
    while n < 10:
        n += 1
        if n % 2 != 0:
            continue    # 跳过奇数，直接进入下一轮循环
        result.append(n)
    print(f"  10 以内的偶数: {result}")

    # while-else（Python 特有！）
    # else 子句在循环正常结束时执行（不是通过 break 退出时）
    print("\nwhile-else（Python 特有）:")
    def find_prime(start: int) -> int:
        """从 start 开始找下一个质数。"""
        n = start
        while True:
            is_prime = True
            divisor = 2
            while divisor * divisor <= n:
                if n % divisor == 0:
                    is_prime = False
                    break
                divisor += 1
            else:
                # while 正常结束（没有 break），说明找到了质数
                if is_prime and n > 1:
                    return n
            n += 1

    print(f"  10 之后的第一个质数: {find_prime(11)}")   # 11
    print(f"  20 之后的第一个质数: {find_prime(21)}")   # 23

    # 无限循环（用于需要持续运行的程序，如服务器）
    # while True:
    #     command = input("请输入命令（输入 quit 退出）: ")
    #     if command == "quit":
    #         break
    #     print(f"执行命令: {command}")


# =============================================================================
# 3.3 for 循环
# =============================================================================
#
# Python 的 for 循环与 C/C++ 不同：
# C/C++: for (int i = 0; i < n; i++) { ... }
# Python: for i in range(n): ...
#
# Python 的 for 是"for-each"风格，遍历任何可迭代对象
# 这类似于 C++11 的范围 for 循环：for (auto& item : container)

def demo_for_loop() -> None:
    """演示 for 循环。"""
    print("\n" + "=" * 60)
    print("3.3 for 循环")
    print("=" * 60)

    # ── range() 函数 ─────────────────────────────────────────────────────────
    print("range() 函数:")
    # range(stop)          → 0, 1, ..., stop-1
    # range(start, stop)   → start, start+1, ..., stop-1
    # range(start, stop, step) → start, start+step, ...

    print("  range(5):", list(range(5)))          # [0, 1, 2, 3, 4]
    print("  range(1,6):", list(range(1, 6)))     # [1, 2, 3, 4, 5]
    print("  range(0,10,2):", list(range(0, 10, 2)))   # [0, 2, 4, 6, 8]
    print("  range(10,0,-1):", list(range(10, 0, -1)))  # [10, 9, ..., 1]（倒序）

    # 计算 1 到 100 的累加和（C/C++ 风格的循环）
    total = sum(range(1, 101))  # sum() 可以对可迭代对象求和
    print(f"\n1 到 100 的累加和: {total}")  # 5050

    # ── 遍历列表 ─────────────────────────────────────────────────────────────
    print("\n遍历列表:")
    fruits = ["apple", "banana", "cherry", "date"]

    # 基本遍历
    for fruit in fruits:
        print(f"  水果: {fruit}")

    # 带索引的遍历（使用 enumerate）
    print("\n  带索引遍历（enumerate）:")
    for index, fruit in enumerate(fruits):
        print(f"  [{index}] {fruit}")

    # enumerate 可以指定起始索引
    for index, fruit in enumerate(fruits, start=1):
        print(f"  第{index}个: {fruit}")

    # ── 遍历字符串 ────────────────────────────────────────────────────────────
    print("\n遍历字符串:")
    for char in "Hello":
        print(f"  字符: {char}", end=" ")
    print()  # 换行

    # ── 遍历字典 ─────────────────────────────────────────────────────────────
    print("\n遍历字典:")
    student = {"name": "Alice", "age": 20, "grade": "A"}

    # 遍历键
    print("  遍历键:")
    for key in student:           # 默认遍历键
        print(f"    {key}")

    # 遍历值
    print("  遍历值:")
    for value in student.values():
        print(f"    {value}")

    # 同时遍历键和值（最常用）
    print("  遍历键值对:")
    for key, value in student.items():
        print(f"    {key}: {value}")

    # ── 同时遍历多个列表（zip）────────────────────────────────────────────────
    print("\n用 zip 同时遍历多个列表:")
    names = ["Alice", "Bob", "Charlie"]
    scores = [95, 87, 92]
    grades = ["A", "B+", "A-"]

    for name, score, grade in zip(names, scores, grades):
        print(f"  {name}: {score} 分 ({grade})")

    # ── for-else（Python 特有）───────────────────────────────────────────────
    print("\nfor-else（Python 特有）:")
    # else 在循环正常结束时执行（没有被 break 中断时）

    def find_in_list(lst: list, target: object) -> None:
        """在列表中查找元素，演示 for-else。"""
        for item in lst:
            if item == target:
                print(f"  找到了 {target}！")
                break
        else:
            # 循环结束后没有 break，说明没找到
            print(f"  没有找到 {target}")

    find_in_list([1, 2, 3, 4, 5], 3)    # 找到了 3！
    find_in_list([1, 2, 3, 4, 5], 10)   # 没有找到 10

    # ── 嵌套 for 循环 ─────────────────────────────────────────────────────────
    print("\n嵌套循环（乘法表）:")
    for i in range(1, 4):
        for j in range(1, 4):
            print(f"  {i}×{j}={i*j}", end="  ")
        print()  # 换行

    # ── 列表推导式（for 循环的简洁写法）─────────────────────────────────────
    # 详细内容在第 5 章，这里先展示基本用法
    print("\n列表推导式（for 的简洁写法）:")
    squares = [x ** 2 for x in range(1, 6)]    # [1, 4, 9, 16, 25]
    print(f"  1~5 的平方: {squares}")

    evens = [x for x in range(20) if x % 2 == 0]  # 带条件的推导式
    print(f"  20 以内的偶数: {evens}")


# =============================================================================
# 3.4 break、continue、pass
# =============================================================================

def demo_break_continue_pass() -> None:
    """演示 break、continue、pass 语句。"""
    print("\n" + "=" * 60)
    print("3.4 break、continue、pass")
    print("=" * 60)

    # ── break：立即退出整个循环 ───────────────────────────────────────────────
    print("break - 退出循环:")
    print("  在 1-20 中找第一个能被 7 整除的数:")
    for n in range(1, 21):
        if n % 7 == 0:
            print(f"  找到了: {n}")
            break   # 找到后立即退出，不再继续循环

    # 嵌套循环中，break 只退出最内层循环
    print("\n  嵌套循环中 break 只退出内层:")
    for i in range(3):
        for j in range(5):
            if j == 3:
                break   # 只退出内层 for j 循环
            print(f"    ({i},{j})", end=" ")
        print()  # 外层循环继续

    # ── continue：跳过本次迭代，进入下一次 ───────────────────────────────────
    print("\ncontinue - 跳过当前迭代:")
    print("  打印 1-10 中不能被 3 整除的数:")
    result = []
    for n in range(1, 11):
        if n % 3 == 0:
            continue    # 跳过能被 3 整除的数
        result.append(n)
    print(f"  结果: {result}")

    # ── pass：占位语句（什么都不做）──────────────────────────────────────────
    # pass 常用于：
    # 1. 定义空的函数/类（占位，以后再实现）
    # 2. 循环中暂时不处理某种情况
    print("\npass - 占位语句:")

    def future_function():
        """这个函数以后再实现。"""
        pass    # 没有 pass 会语法错误（函数体不能为空）

    class EmptyClass:
        """以后再添加内容的类。"""
        pass

    for n in range(5):
        if n == 2:
            pass        # 对 2 暂时不做任何处理
        else:
            print(f"  处理: {n}")


# =============================================================================
# 3.5 综合示例：各种实用算法
# =============================================================================

def demo_algorithms() -> None:
    """演示控制流程在算法中的应用。"""
    print("\n" + "=" * 60)
    print("3.5 综合示例：实用算法")
    print("=" * 60)

    # ── 示例 1：FizzBuzz（经典编程题）────────────────────────────────────────
    print("FizzBuzz（1-20）:")
    result = []
    for n in range(1, 21):
        if n % 15 == 0:
            result.append("FizzBuzz")
        elif n % 3 == 0:
            result.append("Fizz")
        elif n % 5 == 0:
            result.append("Buzz")
        else:
            result.append(str(n))
    print("  " + " ".join(result))

    # ── 示例 2：质数筛（埃拉托斯特尼筛法）───────────────────────────────────
    print("\n埃拉托斯特尼筛法（100 以内的质数）:")
    def sieve_of_eratosthenes(limit: int) -> list[int]:
        """用筛法找出 limit 以内的所有质数。"""
        # 创建一个布尔列表，True 表示"可能是质数"
        is_prime = [True] * (limit + 1)
        is_prime[0] = False     # 0 不是质数
        is_prime[1] = False     # 1 不是质数

        # 从 2 开始，将每个质数的倍数标记为非质数
        for i in range(2, int(limit ** 0.5) + 1):
            if is_prime[i]:
                # i 是质数，将 i² 以上的 i 的倍数全部标记为非质数
                for multiple in range(i * i, limit + 1, i):
                    is_prime[multiple] = False

        # 收集所有质数
        return [n for n in range(2, limit + 1) if is_prime[n]]

    primes = sieve_of_eratosthenes(100)
    print(f"  共找到 {len(primes)} 个质数")
    print(f"  质数列表: {primes}")

    # ── 示例 3：猜数字游戏（演示版，不需要用户输入）─────────────────────────
    print("\n猜数字游戏（模拟）:")
    import random
    random.seed(42)         # 固定随机种子，保证每次运行结果相同

    secret = random.randint(1, 100)    # 生成 1-100 的随机整数
    guesses = [50, 75, 62, 56, 59, 60]  # 模拟猜测序列

    attempts = 0
    for guess in guesses:
        attempts += 1
        if guess < secret:
            print(f"  猜测 {guess}：太小了！")
        elif guess > secret:
            print(f"  猜测 {guess}：太大了！")
        else:
            print(f"  猜测 {guess}：恭喜！用了 {attempts} 次猜对了！")
            break
    else:
        print(f"  很遗憾，没猜到，答案是 {secret}")

    # ── 示例 4：用 while 循环读取用户输入（带验证）──────────────────────────
    # 实际使用时需要键盘输入，这里只展示逻辑结构：
    print("\n带验证的输入循环（逻辑示例）:")
    def get_valid_age(simulated_inputs: list[str]) -> int:
        """
        模拟带验证的用户输入循环。

        Args:
            simulated_inputs: 模拟的用户输入序列

        Returns:
            有效的年龄值
        """
        input_iter = iter(simulated_inputs)
        while True:
            user_input = next(input_iter, None)
            if user_input is None:
                break
            print(f"  用户输入: '{user_input}'", end=" → ")
            try:
                age = int(user_input)
                if 0 <= age <= 150:
                    print(f"有效年龄: {age}")
                    return age
                else:
                    print("年龄必须在 0-150 之间，请重新输入")
            except ValueError:
                print("请输入有效的数字")
        return -1

    valid_age = get_valid_age(["abc", "-5", "200", "25"])
    print(f"  最终获得的有效年龄: {valid_age}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_if_statement()
    demo_while_loop()
    demo_for_loop()
    demo_break_continue_pass()
    demo_algorithms()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 条件语句 ──
# if 条件:           # 条件不需要括号，以冒号结尾
#     ...
# elif 条件:         # 注意：是 elif，不是 else if
#     ...
# else:
#     ...
#
# 条件表达式（三元）：值1 if 条件 else 值2
# match-case（3.10+）：用于结构模式匹配
#
# ── while 循环 ──
# while 条件:
#     ...
# else:              # 可选，循环正常结束时执行
#     ...
#
# ── for 循环 ──
# for 变量 in 可迭代对象:
#     ...
# else:              # 可选，循环正常结束时执行
#     ...
#
# ── 常用内置函数 ──
# range(stop)            → 生成 0 到 stop-1 的整数序列
# range(start, stop, step) → 指定起止和步长
# enumerate(iterable)    → 同时获取索引和值
# zip(iter1, iter2, ...)  → 同时遍历多个可迭代对象
# sum(iterable)          → 求和
#
# ── 控制语句 ──
# break     退出当前循环（只退出最内层）
# continue  跳过本次迭代，继续下一次
# pass      占位，什么都不做
#
# ── Python vs C/C++ 差异总结 ──
# Python 没有 i++、i--（用 i += 1 代替）
# Python 的 / 是浮点除法，// 才是整除
# Python 的 for 是 for-each 风格
# Python 的循环有 else 子句（C/C++ 没有）
# Python 用 elif（C/C++ 用 else if）
# Python 用 and/or/not（C/C++ 用 &&/||/!）


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：elif 写成 else if
# ──────────────────────────────────────
# if x > 0:
#     print("正")
# else if x < 0:    ❌ SyntaxError！
#     print("负")
# 修正：改成 elif x < 0:
#
# 错误 2：用了 i++
# ──────────────────────────────────────
# i = 0
# i++    ❌ SyntaxError！Python 没有 ++ 运算符
# 修正：i += 1
#
# 错误 3：for 循环中修改正在遍历的列表
# ──────────────────────────────────────
# nums = [1, 2, 3, 4, 5]
# for n in nums:
#     if n % 2 == 0:
#         nums.remove(n)   ❌ 行为不可预期！
# 修正：遍历列表的副本，或用列表推导式
# nums = [n for n in nums if n % 2 != 0]   ✅
#
# 错误 4：缩进不一致
# ──────────────────────────────────────
# if True:
#     print("正确")
#   print("错误")    ❌ IndentationError（缩进必须一致）
#
# 错误 5：忘记冒号
# ──────────────────────────────────────
# if x > 0     ❌ SyntaxError（if 后面需要冒号）
#     print("正数")
# 修正：if x > 0:


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   写一个函数 print_pattern(n: int)，打印如下的三角形图案：
#   （n=5 时）
#   *
#   **
#   ***
#   ****
#   *****
#
# 练习 2：
#   写一个函数 count_digits(n: int) -> dict，
#   统计整数 n 中每个数字出现的次数
#   例如：count_digits(112233) → {'1': 2, '2': 2, '3': 2}
#
# 练习 3：
#   写一个函数 collatz(n: int) -> list[int]，
#   实现 Collatz 猜想：
#   - 如果 n 是偶数，下一个数是 n/2
#   - 如果 n 是奇数，下一个数是 3n+1
#   - 直到 n == 1 为止
#   返回从 n 到 1 的完整序列
#   例如：collatz(6) → [6, 3, 10, 5, 16, 8, 4, 2, 1]
#
# 练习 4：
#   写一个函数 flatten(matrix: list[list[int]]) -> list[int]，
#   将二维列表展平为一维列表
#   例如：flatten([[1,2,3],[4,5,6],[7,8,9]]) → [1,2,3,4,5,6,7,8,9]


# =============================================================================
# 【练习答案】
# =============================================================================

def print_pattern(n: int) -> None:
    """练习 1：打印三角形图案。"""
    for i in range(1, n + 1):
        print("*" * i)


def count_digits(n: int) -> dict[str, int]:
    """练习 2：统计整数中每个数字的出现次数。"""
    counts: dict[str, int] = {}
    for digit in str(abs(n)):   # abs 处理负数，str 转为字符串
        counts[digit] = counts.get(digit, 0) + 1  # get 方法带默认值
    return counts


def collatz(n: int) -> list[int]:
    """练习 3：Collatz 猜想序列。"""
    if n <= 0:
        return []
    sequence = [n]
    while n != 1:
        if n % 2 == 0:
            n = n // 2
        else:
            n = 3 * n + 1
        sequence.append(n)
    return sequence


def flatten(matrix: list[list[int]]) -> list[int]:
    """练习 4：将二维列表展平为一维列表。"""
    result = []
    for row in matrix:
        for item in row:
            result.append(item)
    return result
    # 更简洁的写法（列表推导式）：
    # return [item for row in matrix for item in row]


# 取消注释以运行练习答案：
# if __name__ == "__main__":
#     print("练习 1：")
#     print_pattern(5)
#
#     print("\n练习 2：")
#     print(count_digits(112233))   # {'1': 2, '2': 2, '3': 2}
#     print(count_digits(1000))     # {'1': 1, '0': 3}
#
#     print("\n练习 3：")
#     print(collatz(6))    # [6, 3, 10, 5, 16, 8, 4, 2, 1]
#     print(collatz(27))   # 很长的序列
#
#     print("\n练习 4：")
#     matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
#     print(flatten(matrix))   # [1, 2, 3, 4, 5, 6, 7, 8, 9]
