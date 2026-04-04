# =============================================================================
# 第 4 章：字符串处理
# =============================================================================
#
# 【学习目标】
#   1. 掌握字符串的创建和基本操作
#   2. 熟练使用字符串的常用方法
#   3. 掌握字符串格式化的各种方式
#   4. 理解字符串的不可变性
#   5. 掌握字符串切片操作
#
# 【运行方式】
#   python chapter04_字符串处理.py
#
# =============================================================================


# =============================================================================
# 4.1 字符串基础
# =============================================================================

def demo_string_basics() -> None:
    """演示字符串的基本概念。"""
    print("=" * 60)
    print("4.1 字符串基础")
    print("=" * 60)

    # Python 字符串可以用单引号或双引号（效果完全相同）
    s1 = 'Hello, Python!'
    s2 = "Hello, Python!"
    print(f"s1 == s2: {s1 == s2}")   # True

    # 三引号：多行字符串
    multi_line = """第一行
第二行
第三行"""
    print(f"\n多行字符串:\n{multi_line}")

    # 原始字符串（raw string）：r 前缀，反斜杠不转义
    # 在 C/C++ 中需要写 "C:\\Users\\name"，Python 有 r"C:\Users\name"
    path_raw = r"C:\Users\name\documents"
    path_normal = "C:\\Users\\name\\documents"
    print(f"\n原始字符串: {path_raw}")
    print(f"普通字符串: {path_normal}")
    print(f"它们相等: {path_raw == path_normal}")

    # 常用转义字符
    print("\n常用转义字符:")
    print("换行 \\n:", "行1\n行2")
    print("制表 \\t:", "列1\t列2\t列3")
    print("反斜杠 \\\\:", "C:\\Windows")
    print("单引号 \\':", "It\'s OK")

    # 字符串是不可变的（immutable）
    # C++ 的 std::string 是可变的，Python 的 str 不可变
    s = "hello"
    # s[0] = 'H'   ❌ TypeError: 'str' object does not support item assignment
    # 必须创建新字符串
    s = "H" + s[1:]   # 切片 + 拼接
    print(f"\n修改首字母: {s}")

    # 字符串长度
    text = "Hello, 世界!"
    print(f"\n'{text}' 的长度: {len(text)}")  # 按字符数，不是字节数

    # 字符串与字节串的区别
    encoded = text.encode("utf-8")    # 编码为字节串（bytes）
    decoded = encoded.decode("utf-8") # 解码回字符串
    print(f"UTF-8 编码字节数: {len(encoded)}")   # 中文每字符3字节
    print(f"解码回来: {decoded}")


# =============================================================================
# 4.2 字符串索引与切片
# =============================================================================

def demo_string_slicing() -> None:
    """演示字符串索引和切片。"""
    print("\n" + "=" * 60)
    print("4.2 字符串索引与切片")
    print("=" * 60)

    s = "Hello, World!"
    #    0123456789...   正向索引（从0开始）
    #   -13...-2-1       反向索引（从-1开始）

    print(f"字符串: {s!r}")
    print(f"长度: {len(s)}")

    # ── 索引（获取单个字符）─────────────────────────────
    print("\n索引访问:")
    print(f"  s[0]  = {s[0]!r}")    # 'H'（第一个字符）
    print(f"  s[7]  = {s[7]!r}")    # 'W'
    print(f"  s[-1] = {s[-1]!r}")   # '!'（最后一个字符）
    print(f"  s[-6] = {s[-6]!r}")   # 'W'（倒数第6个）

    # ── 切片（获取子字符串）─────────────────────────────
    # 语法：s[start:stop:step]
    # start: 起始索引（包含），默认 0
    # stop:  结束索引（不包含），默认 len(s)
    # step:  步长，默认 1
    print("\n切片操作:")
    print(f"  s[0:5]   = {s[0:5]!r}")     # 'Hello'（索引 0-4）
    print(f"  s[7:12]  = {s[7:12]!r}")    # 'World'
    print(f"  s[:5]    = {s[:5]!r}")      # 'Hello'（从头开始）
    print(f"  s[7:]    = {s[7:]!r}")      # 'World!'（到末尾）
    print(f"  s[:]     = {s[:]!r}")       # 完整副本
    print(f"  s[::2]   = {s[::2]!r}")    # 每隔一个字符
    print(f"  s[::-1]  = {s[::-1]!r}")   # 反转字符串！

    # 切片不会越界（与 C/C++ 的数组访问不同）
    print(f"  s[0:100] = {s[0:100]!r}")   # 不会报错，只返回到末尾

    # 实用切片示例
    filename = "document.txt"
    name = filename[:-4]       # 去掉最后4个字符（.txt）
    ext = filename[-3:]        # 取最后3个字符（txt）
    print(f"\n文件名处理:")
    print(f"  文件名: {filename}")
    print(f"  主名称: {name}")
    print(f"  扩展名: {ext}")


# =============================================================================
# 4.3 字符串常用方法
# =============================================================================

def demo_string_methods() -> None:
    """演示字符串的常用方法。"""
    print("\n" + "=" * 60)
    print("4.3 字符串常用方法")
    print("=" * 60)

    # ── 大小写转换 ───────────────────────────────────────
    print("大小写转换:")
    s = "hello, WORLD!"
    print(f"  原始:       {s!r}")
    print(f"  upper():    {s.upper()!r}")      # 全大写
    print(f"  lower():    {s.lower()!r}")      # 全小写
    print(f"  title():    {s.title()!r}")      # 每词首字母大写
    print(f"  capitalize:{s.capitalize()!r}")  # 首字母大写
    print(f"  swapcase(): {s.swapcase()!r}")   # 大小写互换

    # ── 去除空白 ─────────────────────────────────────────
    print("\n去除空白:")
    s = "  \t Hello, World! \n  "
    print(f"  原始:    {s!r}")
    print(f"  strip():  {s.strip()!r}")    # 去除两端空白
    print(f"  lstrip(): {s.lstrip()!r}")   # 去除左端空白
    print(f"  rstrip(): {s.rstrip()!r}")   # 去除右端空白

    # strip 也可以去除指定字符
    url = "###python.org###"
    print(f"  {url!r}.strip('#') = {url.strip('#')!r}")

    # ── 查找与替换 ───────────────────────────────────────
    print("\n查找与替换:")
    text = "Python is great. Python is easy."
    print(f"  原始: {text!r}")
    print(f"  find('Python'):    {text.find('Python')}")    # 第一次出现的索引
    print(f"  find('Java'):      {text.find('Java')}")      # -1 表示未找到
    print(f"  rfind('Python'):   {text.rfind('Python')}")   # 从右查找
    print(f"  count('Python'):   {text.count('Python')}")   # 出现次数
    print(f"  replace():         {text.replace('Python', 'Go')!r}")  # 替换
    print(f"  replace(次数限制): {text.replace('Python', 'Go', 1)!r}")  # 只替换第一个

    # ── 判断类方法 ───────────────────────────────────────
    print("\n判断类方法（返回 bool）:")
    tests = [
        ("hello123", ["isalpha", "isdigit", "isalnum", "isspace"]),
        ("12345",    ["isalpha", "isdigit", "isalnum"]),
        ("   ",      ["isspace"]),
        ("Hello",    ["istitle", "isupper", "islower"]),
    ]
    for s, methods in tests:
        results = []
        for method in methods:
            val = getattr(s, method)()
            results.append(f"{method}()={val}")
        print(f"  {s!r}: {', '.join(results)}")

    # 检查前缀和后缀
    filename = "report_2024.csv"
    print(f"\n  {filename!r}.startswith('report'): {filename.startswith('report')}")
    print(f"  {filename!r}.endswith('.csv'):    {filename.endswith('.csv')}")
    print(f"  endswith(('.csv','.xlsx')):       {filename.endswith(('.csv', '.xlsx'))}")

    # ── 分割与合并 ───────────────────────────────────────
    print("\n分割与合并:")
    csv_line = "Alice,28,Engineer,Beijing"
    parts = csv_line.split(",")           # 按逗号分割
    print(f"  split(','):   {parts}")

    # split 可以限制分割次数
    parts2 = csv_line.split(",", 2)
    print(f"  split(',',2): {parts2}")

    # 按行分割
    text = "第一行\n第二行\n第三行"
    lines = text.splitlines()             # 按换行符分割
    print(f"  splitlines(): {lines}")

    # join：用分隔符连接列表（与 split 相反）
    words = ["Hello", "World", "Python"]
    print(f"  '-'.join(): {'-'.join(words)}")
    print(f"  ' '.join(): {' '.join(words)}")
    print(f"  ''.join():  {''.join(words)}")

    # ── 对齐与填充 ───────────────────────────────────────
    print("\n对齐与填充:")
    s = "hello"
    print(f"  center(20):       {s.center(20)!r}")
    print(f"  center(20, '*'):  {s.center(20, '*')!r}")
    print(f"  ljust(20, '-'):   {s.ljust(20, '-')!r}")
    print(f"  rjust(20, '-'):   {s.rjust(20, '-')!r}")
    print(f"  zfill(10):        {'42'.zfill(10)!r}")  # 数字补零


# =============================================================================
# 4.4 字符串格式化
# =============================================================================

def demo_string_formatting() -> None:
    """演示字符串格式化的各种方式。"""
    print("\n" + "=" * 60)
    print("4.4 字符串格式化")
    print("=" * 60)

    name = "Alice"
    age = 28
    score = 98.567
    pi = 3.14159265358979

    # ── 方式 1：% 格式化（旧式）─────────────────────────
    print("方式1：% 格式化（旧式，类似 C 的 printf）:")
    print("  %s = 字符串, %d = 整数, %f = 浮点数")
    print("  结果:", "姓名: %s, 年龄: %d, 分数: %.2f" % (name, age, score))

    # ── 方式 2：str.format()─────────────────────────────
    print("\n方式2：str.format():")
    # 位置参数
    print("  {}".format("位置参数"))
    print("  {} 今年 {} 岁".format(name, age))
    # 索引
    print("  {0} {1} {0}".format("hello", "world"))
    # 命名参数
    print("  {name} 今年 {age} 岁".format(name=name, age=age))
    # 格式化浮点数
    print("  {:.2f}".format(score))      # 保留2位小数
    print("  {:10.2f}".format(score))    # 宽度10，保留2位小数

    # ── 方式 3：f-string（推荐，Python 3.6+）──────────
    print("\n方式3：f-string（推荐使用）:")
    print(f"  {name} 今年 {age} 岁")

    # f-string 中可以写任意表达式
    print(f"  年龄的平方: {age ** 2}")
    print(f"  大写姓名: {name.upper()}")
    print(f"  是否成年: {'是' if age >= 18 else '否'}")

    # f-string 格式化数字
    print(f"\n数字格式化:")
    print(f"  保留2位小数:   {score:.2f}")
    print(f"  科学计数法:    {score:.2e}")
    print(f"  百分比:        {0.856:.1%}")
    print(f"  千分位:        {1234567.89:,.2f}")
    print(f"  补零(宽度8):  {42:08d}")
    print(f"  右对齐(宽10): {name:>10}")
    print(f"  左对齐(宽10): {name:<10}|")
    print(f"  居中(宽10):   {name:^10}|")
    print(f"  填充字符*:    {name:*^15}")

    # 进制格式化
    n = 255
    print(f"\n进制格式化（n={n}）:")
    print(f"  二进制:     {n:b}")
    print(f"  八进制:     {n:o}")
    print(f"  十六进制:   {n:x}")
    print(f"  十六进制大: {n:X}")
    print(f"  带前缀十六: {n:#x}")

    # Python 3.8+ 的调试格式：变量名=值
    x, y = 10, 20
    print(f"\n调试格式（Python 3.8+）:")
    print(f"  {x=}, {y=}")        # x=10, y=20
    print(f"  {x+y=}")             # x+y=30

    # ── 方式 4：Template（用于安全插值）─────────────────
    print("\n方式4：string.Template（安全模板）:")
    from string import Template
    t = Template("你好，$name！你有 $count 条消息。")
    result = t.substitute(name="Bob", count=5)
    print(f"  {result}")
    # safe_substitute：遇到缺失的变量不报错
    result2 = t.safe_substitute(name="Charlie")
    print(f"  {result2}")


# =============================================================================
# 4.5 字符串操作实用技巧
# =============================================================================

def demo_string_tips() -> None:
    """演示字符串操作的实用技巧。"""
    print("\n" + "=" * 60)
    print("4.5 实用技巧与综合示例")
    print("=" * 60)

    # ── 字符串拼接的正确方式 ─────────────────────────────
    print("字符串拼接:")

    # ❌ 低效：在循环中用 + 拼接（每次都创建新字符串）
    # result = ""
    # for word in words:
    #     result += word   # 不推荐！

    # ✅ 高效：收集到列表，最后用 join
    words = ["Python", "is", "awesome"]
    result = " ".join(words)
    print(f"  join 拼接: {result!r}")

    # ✅ 字符串乘法
    separator = "-" * 40
    print(f"  重复字符: {separator!r}")

    # ── 字符串检测 ───────────────────────────────────────
    print("\n字符串内容检测:")
    text = "  \t  "
    print(f"  空白字符串: {text!r}")
    print(f"  bool({text!r}): {bool(text)}")   # True（有字符，即使是空白）
    print(f"  strip 后: {bool(text.strip())}")  # False（去掉空白后为空）

    # ── 密码强度检查器（综合应用）───────────────────────
    print("\n密码强度检查器:")
    def check_password_strength(password: str) -> dict:
        """
        检查密码强度。

        Returns:
            包含各项检查结果的字典
        """
        result = {
            "length_ok":    len(password) >= 8,
            "has_upper":    any(c.isupper() for c in password),
            "has_lower":    any(c.islower() for c in password),
            "has_digit":    any(c.isdigit() for c in password),
            "has_special":  any(c in "!@#$%^&*()-_=+" for c in password),
        }
        score = sum(result.values())
        if score <= 2:
            strength = "弱"
        elif score <= 3:
            strength = "中等"
        elif score <= 4:
            strength = "强"
        else:
            strength = "非常强"
        result["strength"] = strength
        return result

    passwords = ["abc", "password", "Password1", "P@ssw0rd!"]
    for pwd in passwords:
        info = check_password_strength(pwd)
        print(f"  {pwd!r:15} → 强度: {info['strength']}")

    # ── 文本统计（综合应用）─────────────────────────────
    print("\n文本统计分析:")
    text = """Python is an interpreted, high-level programming language.
Python's design philosophy emphasizes code readability.
It was created by Guido van Rossum and released in 1991."""

    lines = text.strip().splitlines()
    words_list = text.split()
    chars_no_space = text.replace(" ", "").replace("\n", "")

    print(f"  行数: {len(lines)}")
    print(f"  单词数: {len(words_list)}")
    print(f"  字符数（含空格）: {len(text)}")
    print(f"  字符数（不含空格）: {len(chars_no_space)}")

    # 词频统计
    word_freq: dict[str, int] = {}
    for word in text.lower().split():
        # 去掉标点符号
        clean = word.strip(".,!?\"';:()")
        if clean:
            word_freq[clean] = word_freq.get(clean, 0) + 1

    # 找出出现最多的5个词
    top5 = sorted(word_freq.items(), key=lambda x: x[1], reverse=True)[:5]
    print(f"  出现最多的词: {top5}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示。"""
    demo_string_basics()
    demo_string_slicing()
    demo_string_methods()
    demo_string_formatting()
    demo_string_tips()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 字符串创建 ──
# s = 'hello'  /  s = "hello"   # 单双引号等价
# s = """多行"""                 # 三引号多行字符串
# s = r"C:\path"                 # 原始字符串（不转义反斜杠）
# s = f"值={x}"                  # f-string（推荐格式化方式）
#
# ── 索引与切片 ──
# s[0]       → 第一个字符
# s[-1]      → 最后一个字符
# s[a:b]     → 索引 a 到 b-1（不含 b）
# s[a:b:step] → 带步长的切片
# s[::-1]    → 反转字符串
#
# ── 常用方法 ──
# s.upper() / s.lower() / s.title()  大小写
# s.strip() / s.lstrip() / s.rstrip()  去空白
# s.find(sub) / s.count(sub)           查找
# s.replace(old, new)                  替换
# s.split(sep) / sep.join(list)        分割/合并
# s.startswith(prefix) / s.endswith(suffix)
# s.isalpha() / s.isdigit() / s.isalnum() / s.isspace()
#
# ── 格式化（推荐 f-string）──
# f"{value:.2f}"      保留2位小数
# f"{value:>10}"      右对齐，宽度10
# f"{value:08d}"      补零
# f"{value:.1%}"      百分比
# f"{value:,}"        千分位


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：修改字符串（字符串不可变）
# s = "hello"
# s[0] = 'H'   ❌ TypeError
# 修正：s = 'H' + s[1:]   ✅
#
# 错误 2：在循环中用 + 拼接字符串（性能差）
# result = ""
# for s in strings:
#     result += s    ❌ O(n²) 时间复杂度
# 修正：result = "".join(strings)   ✅
#
# 错误 3：split 和 join 混淆分隔符
# parts = text.split(",")
# result = " ".join(parts)  # 合并时用的是空格，不是逗号
#
# 错误 4：f-string 中的引号冲突
# f"{"hello"}"   ❌ SyntaxError（引号冲突）
# 修正（Python 3.12+ 已修复，之前版本）：
# name = "hello"; f"{name}"  ✅


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1：
#   写一个函数 is_palindrome(s: str) -> bool
#   判断字符串是否是回文（忽略大小写和空格）
#   例如："racecar" → True, "A man a plan a canal Panama" → True
#
# 练习 2：
#   写一个函数 title_case(s: str) -> str
#   将字符串转换为标题格式，但不大写介词（of, the, a, an, in, on, at）
#   例如："the lord of the rings" → "The Lord of the Rings"
#
# 练习 3：
#   写一个函数 format_table(data: list[list]) -> str
#   将二维数据格式化为对齐的文本表格
#   例如：[["姓名", "年龄", "城市"], ["Alice", "28", "北京"]]
#   输出：
#   姓名   | 年龄 | 城市
#   Alice  | 28   | 北京


# =============================================================================
# 【练习答案】
# =============================================================================

def is_palindrome(s: str) -> bool:
    """练习 1：判断字符串是否是回文。"""
    # 只保留字母和数字，转小写
    cleaned = "".join(c.lower() for c in s if c.isalnum())
    return cleaned == cleaned[::-1]


def title_case(s: str) -> str:
    """练习 2：自定义标题格式化。"""
    minor_words = {"of", "the", "a", "an", "in", "on", "at", "and", "or", "but"}
    words = s.split()
    result = []
    for i, word in enumerate(words):
        if i == 0 or word.lower() not in minor_words:
            result.append(word.capitalize())
        else:
            result.append(word.lower())
    return " ".join(result)


def format_table(data: list[list]) -> str:
    """练习 3：格式化文本表格。"""
    if not data:
        return ""
    # 计算每列的最大宽度
    col_widths = []
    for col_idx in range(len(data[0])):
        max_width = max(len(str(row[col_idx])) for row in data)
        col_widths.append(max_width)

    rows = []
    for row in data:
        cells = [str(cell).ljust(col_widths[i]) for i, cell in enumerate(row)]
        rows.append(" | ".join(cells))

    # 分隔线
    separator = "-+-".join("-" * w for w in col_widths)
    result = [rows[0], separator] + rows[1:]
    return "\n".join(result)


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print(is_palindrome("racecar"))               # True
#     print(is_palindrome("A man a plan a canal Panama"))  # True
#     print(is_palindrome("hello"))                 # False
#
#     print(title_case("the lord of the rings"))    # The Lord of the Rings
#     print(title_case("a tale of two cities"))     # A Tale of Two Cities
#
#     table_data = [
#         ["姓名", "年龄", "城市"],
#         ["Alice", "28", "北京"],
#         ["Bob", "32", "上海"],
#         ["Charlie", "25", "广州"],
#     ]
#     print(format_table(table_data))
