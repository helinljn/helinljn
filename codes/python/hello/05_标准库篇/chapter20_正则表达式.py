# =============================================================================
# 第 20 章：正则表达式（re 模块）
# =============================================================================
#
# 【学习目标】
#   1. 掌握正则表达式的基本语法和元字符
#   2. 掌握 re 模块的核心函数：match、search、findall、sub、split
#   3. 掌握编译正则表达式和使用标志位
#   4. 掌握捕获组、命名组和非捕获组
#   5. 学会用正则表达式解决实际文本处理问题
#
# 【正则表达式解决什么问题？】
#   - 模式匹配：判断字符串是否符合某种格式（邮箱、手机号、URL）
#   - 文本搜索：在大段文本中查找符合模式的所有子串
#   - 文本替换：批量替换符合模式的内容
#   - 文本分割：按复杂规则分割字符串（比 str.split 更强大）
#   - 数据提取：从非结构化文本中提取结构化信息
#
# 【与其他语言的对比】
#   Python re 模块的正则语法基本遵循 Perl 风格（PCRE）
#   与 JavaScript、Java、C++ std::regex 语法高度相似
#   但各语言在细节上有差异（如命名组语法、标志位）
#
# 【运行方式】
#   python chapter20_正则表达式.py
#
# =============================================================================

import re
import time
from typing import Optional


# =============================================================================
# 20.1 正则表达式基础：元字符与字符类
# =============================================================================

def demo_regex_basics() -> None:
    """演示正则表达式的基本元字符和字符类。"""
    print("=" * 60)
    print("20.1 正则表达式基础：元字符与字符类")
    print("=" * 60)

    print("【核心概念】")
    print("  正则表达式（Regular Expression，简称 regex）是描述字符串模式的语言")
    print("  元字符：具有特殊含义的字符（如 . * + ? [] {} () ^ $ | \\）")
    print("  字符类：用 [] 定义的字符集合，匹配其中任意一个字符")
    print()

    # ── 基本元字符 ────────────────────────────────────────
    print("1. 基本元字符:")
    print()

    patterns = [
        (r'.',      '匹配任意单个字符（除换行符）',        'a1@',      'a', '1', '@'),
        (r'\d',     '匹配数字 [0-9]',                    '0123abc',  '0', '1', '2'),
        (r'\D',     '匹配非数字',                        '0123abc',  'a', 'b', 'c'),
        (r'\w',     '匹配单词字符 [a-zA-Z0-9_]',         'a1_@',     'a', '1', '_'),
        (r'\W',     '匹配非单词字符',                    'a1_@',     '@'),
        (r'\s',     '匹配空白字符（空格、制表符、换行）', ' \t\n',    ' ', '\t'),
        (r'\S',     '匹配非空白字符',                    ' abc',     'a', 'b', 'c'),
    ]

    for pattern, desc, test_str, *expected in patterns:
        matches = re.findall(pattern, test_str)
        print(f"  {pattern:<8} {desc}")
        print(f"           测试: '{test_str}' → {matches}")
        print()

    # ── 字符类 ────────────────────────────────────────────
    print("2. 字符类 [...]:")
    print()

    char_classes = [
        (r'[abc]',      '匹配 a、b 或 c',              'abcdef',   ['a', 'b', 'c']),
        (r'[a-z]',      '匹配小写字母',                'Hello123', ['e', 'l', 'l', 'o']),
        (r'[A-Z]',      '匹配大写字母',                'Hello123', ['H']),
        (r'[0-9]',      '匹配数字（等价于 \\d）',      'abc123',   ['1', '2', '3']),
        (r'[a-zA-Z]',   '匹配所有字母',                'Hello123', ['H', 'e', 'l', 'l', 'o']),
        (r'[^0-9]',     '匹配非数字（^ 表示取反）',    'abc123',   ['a', 'b', 'c']),
        (r'[a-z0-9]',   '匹配小写字母或数字',          'Hello123', ['e', 'l', 'l', 'o', '1', '2', '3']),
    ]

    for pattern, desc, test_str, expected in char_classes:
        matches = re.findall(pattern, test_str)
        print(f"  {pattern:<12} {desc}")
        print(f"               测试: '{test_str}' → {matches}")
        print()

    # ── 量词 ──────────────────────────────────────────────
    print("3. 量词（指定重复次数）:")
    print()

    quantifiers = [
        (r'a*',      '匹配 0 个或多个 a',           'aaabbc',    ['aaa', '', '', '', '', '']),
        (r'a+',      '匹配 1 个或多个 a',           'aaabbc',    ['aaa']),
        (r'a?',      '匹配 0 个或 1 个 a',          'aaabbc',    ['a', 'a', 'a', '', '', '', '']),
        (r'a{3}',    '匹配恰好 3 个 a',             'aaabbc',    ['aaa']),
        (r'a{2,4}',  '匹配 2-4 个 a',               'aaaaabc',   ['aaaa', 'aa']),
        (r'a{2,}',   '匹配至少 2 个 a',             'aaaaabc',   ['aaaaaa']),
        (r'\d{3}',   '匹配 3 位数字',               '12-345-67', ['345']),
    ]

    for pattern, desc, test_str, expected in quantifiers:
        matches = re.findall(pattern, test_str)
        print(f"  {pattern:<12} {desc}")
        print(f"               测试: '{test_str}' → {matches[:3]}{'...' if len(matches) > 3 else ''}")
        print()

    # ── 位置锚点 ──────────────────────────────────────────
    print("4. 位置锚点:")
    print()

    text = "hello world"
    anchors = [
        (r'^hello',     '匹配字符串开头的 hello',     True),
        (r'world$',     '匹配字符串结尾的 world',     True),
        (r'^world',     '匹配字符串开头的 world',     False),
        (r'hello$',     '匹配字符串结尾的 hello',     False),
        (r'\bhello\b',  '匹配单词边界的 hello',       True),
        (r'\bllo\b',    '匹配单词边界的 llo',         False),
    ]

    print(f"  测试字符串: '{text}'")
    print()
    for pattern, desc, expected in anchors:
        match = re.search(pattern, text)
        result = match is not None
        status = '✓' if result == expected else '✗'
        print(f"  {status} {pattern:<15} {desc:<30} → {result}")
    print()

    # ── 贪婪 vs 非贪婪 ────────────────────────────────────
    print("5. 贪婪匹配 vs 非贪婪匹配:")
    print()

    html = '<div>content1</div><div>content2</div>'
    print(f"  HTML: {html}")
    print()

    greedy = re.findall(r'<div>.*</div>', html)      # 贪婪：尽可能多匹配
    lazy   = re.findall(r'<div>.*?</div>', html)     # 非贪婪：尽可能少匹配

    print(f"  贪婪匹配   <div>.*</div>   → {greedy}")
    print(f"  非贪婪匹配 <div>.*?</div>  → {lazy}")
    print()
    print("  说明：")
    print("  - 贪婪（默认）：量词 * + ? {} 会尽可能多地匹配字符")
    print("  - 非贪婪：在量词后加 ? 变成 *? +? ?? {m,n}?，尽可能少匹配")


# =============================================================================
# 20.2 re 模块核心函数
# =============================================================================

def demo_re_functions() -> None:
    """演示 re 模块的核心函数：match、search、findall、finditer、sub、split。"""
    print("\n" + "=" * 60)
    print("20.2 re 模块核心函数")
    print("=" * 60)

    # ── re.match：从字符串开头匹配 ────────────────────────
    print("1. re.match()：从字符串开头匹配")
    print()
    print("  match() 只匹配字符串开头，如果开头不匹配则返回 None")
    print()

    text = "Python 3.11 is great"
    pattern = r'Python'

    m = re.match(pattern, text)
    if m:
        print(f"  re.match(r'{pattern}', '{text}')")
        print(f"    匹配成功: '{m.group()}'")
        print(f"    匹配位置: {m.span()}")
    print()

    # 开头不匹配的情况
    m2 = re.match(r'\d+', text)
    print(f"  re.match(r'\\d+', '{text}')")
    print(f"    匹配结果: {m2}  （开头不是数字，返回 None）")
    print()

    # ── re.search：在整个字符串中搜索 ─────────────────────
    print("2. re.search()：在整个字符串中搜索第一个匹配")
    print()
    print("  search() 扫描整个字符串，返回第一个匹配的 Match 对象")
    print()

    m3 = re.search(r'\d+\.\d+', text)
    if m3:
        print(f"  re.search(r'\\d+\\.\\d+', '{text}')")
        print(f"    找到: '{m3.group()}' at {m3.span()}")
    print()

    # ── re.findall：查找所有匹配 ──────────────────────────
    print("3. re.findall()：查找所有匹配，返回字符串列表")
    print()

    log = "Error at 10:23:45, Warning at 10:24:12, Error at 10:25:03"
    times = re.findall(r'\d{2}:\d{2}:\d{2}', log)
    print(f"  日志: {log}")
    print(f"  提取所有时间: {times}")
    print()

    # 带捕获组的 findall
    emails = "Contact: alice@example.com or bob@test.org"
    users = re.findall(r'(\w+)@\w+\.\w+', emails)
    print(f"  邮箱: {emails}")
    print(f"  提取用户名（捕获组）: {users}")
    print()

    # ── re.finditer：返回迭代器 ───────────────────────────
    print("4. re.finditer()：返回 Match 对象的迭代器")
    print()
    print("  finditer() 比 findall() 更节省内存（惰性求值）")
    print("  返回的 Match 对象包含更多信息（位置、捕获组等）")
    print()

    text2 = "Price: $19.99, Discount: $5.50, Total: $14.49"
    print(f"  文本: {text2}")
    print(f"  查找所有价格:")
    for match in re.finditer(r'\$(\d+\.\d{2})', text2):
        print(f"    位置 {match.span()}: '{match.group()}' (金额={match.group(1)})")
    print()

    # ── re.sub：替换 ─────────────────────────────────────
    print("5. re.sub()：替换匹配的内容")
    print()

    phone = "联系电话：138-1234-5678 或 186-9876-5432"
    print(f"  原文: {phone}")

    masked = re.sub(r'\d{3}-\d{4}-\d{4}', '***-****-****', phone)
    print(f"  全部隐藏: {masked}")

    def mask_phone(match: re.Match) -> str:
        """只保留前 3 位和后 4 位，中间用 * 替换。"""
        full = match.group()
        parts = full.split('-')
        return f"{parts[0]}-****-{parts[2]}"

    masked2 = re.sub(r'\d{3}-\d{4}-\d{4}', mask_phone, phone)
    print(f"  部分隐藏: {masked2}")
    print()

    date = "2024-03-15"
    us_date = re.sub(r'(\d{4})-(\d{2})-(\d{2})', r'\2/\3/\1', date)
    print(f"  日期格式转换: {date} → {us_date}")
    print()

    # ── re.split：分割 ────────────────────────────────────
    print("6. re.split()：按正则模式分割字符串")
    print()

    text3 = "apple,banana;orange|grape:melon"
    print(f"  文本: {text3}")
    print(f"  str.split(',')       → {text3.split(',')}")
    print(f"  re.split(r'[,;|:]') → {re.split(r'[,;|:]', text3)}")
    print()

    text4 = "one  two\tthree\n\nfour"
    words = re.split(r'\s+', text4.strip())
    print(f"  文本: {repr(text4)}")
    print(f"  re.split(r'\\s+')   → {words}")
    print()

    text5 = "a1b2c3"
    parts = re.split(r'(\d)', text5)
    print(f"  文本: {text5}")
    print(f"  re.split(r'(\\d)')  → {parts}  （捕获组会被保留）")


# =============================================================================
# 20.3 捕获组与命名组
# =============================================================================

def demo_groups() -> None:
    """演示捕获组、命名组和非捕获组的用法。"""
    print("\n" + "=" * 60)
    print("20.3 捕获组与命名组")
    print("=" * 60)

    # ── 基本捕获组 ────────────────────────────────────────
    print("1. 基本捕获组 (...)：提取匹配的子串")
    print()

    email = "联系方式：alice@example.com"
    pattern = r'(\w+)@(\w+)\.(\w+)'

    match = re.search(pattern, email)
    if match:
        print(f"  邮箱: {email}")
        print(f"  模式: {pattern}")
        print(f"  group(0)（完整匹配）: {match.group(0)}")
        print(f"  group(1)（用户名）:   {match.group(1)}")
        print(f"  group(2)（域名）:     {match.group(2)}")
        print(f"  group(3)（顶级域）:   {match.group(3)}")
        print(f"  groups()（所有组）:   {match.groups()}")
    print()

    # ── 命名组 ────────────────────────────────────────────
    print("2. 命名组 (?P<name>...)：给捕获组命名")
    print()

    log_line = "[2024-03-15 14:23:45] ERROR: Connection timeout"
    pattern2 = (r'\[(?P<date>\d{4}-\d{2}-\d{2}) '
                r'(?P<time>\d{2}:\d{2}:\d{2})\] '
                r'(?P<level>\w+): (?P<message>.*)')

    match2 = re.search(pattern2, log_line)
    if match2:
        print(f"  日志: {log_line}")
        print(f"  提取字段:")
        print(f"    日期:   {match2.group('date')}")
        print(f"    时间:   {match2.group('time')}")
        print(f"    级别:   {match2.group('level')}")
        print(f"    消息:   {match2.group('message')}")
        print(f"  groupdict(): {match2.groupdict()}")
    print()

    # ── 非捕获组 ──────────────────────────────────────────
    print("3. 非捕获组 (?:...)：分组但不捕获")
    print()
    print("  用途：需要用 () 分组（如应用量词），但不需要提取内容")
    print()

    url = "https://www.example.com/path"

    m1 = re.search(r'(https?)://(\w+\.\w+)', url)
    print(f"  URL: {url}")
    print(f"  捕获组:   r'(https?)://(\\w+\\.\\w+)'")
    print(f"    groups() = {m1.groups() if m1 else None}")

    m2 = re.search(r'(?:https?)://(\w+\.\w+)', url)
    print(f"  非捕获组: r'(?:https?)://(\\w+\\.\\w+)'")
    print(f"    groups() = {m2.groups() if m2 else None}")
    print()

    # ── 反向引用 ──────────────────────────────────────────
    print("4. 反向引用 \\1 \\2：引用前面的捕获组")
    print()

    text = "the the quick brown fox fox jumps"
    duplicates = re.findall(r'\b(\w+)\s+\1\b', text)
    print(f"  文本: {text}")
    print(f"  查找重复单词 r'\\b(\\w+)\\s+\\1\\b': {duplicates}")
    print()

    html = "<div>content</div><span>text</span>"
    tags = re.findall(r'<(\w+)>.*?</\1>', html)
    print(f"  HTML: {html}")
    print(f"  匹配标签对 r'<(\\w+)>.*?</\\1>': {tags}")


# =============================================================================
# 20.4 编译正则表达式与标志位
# =============================================================================

def demo_compile_and_flags() -> None:
    """演示编译正则表达式和使用标志位。"""
    print("\n" + "=" * 60)
    print("20.4 编译正则表达式与标志位")
    print("=" * 60)

    # ── 编译正则表达式 ────────────────────────────────────
    print("1. re.compile()：预编译正则表达式")
    print()
    print("  【为什么要编译？】")
    print("  - 重复使用同一个模式时，编译一次可提升性能")
    print("  - 编译后的 Pattern 对象有 match/search/findall 等方法")
    print("  - 可以在编译时指定标志位，避免每次调用时传递")
    print()

    phone_pattern = re.compile(r'\d{3}-\d{4}-\d{4}')
    phones = ["138-1234-5678", "186-9876-5432", "not a phone", "139-0000-1111"]

    print(f"  编译模式: {phone_pattern.pattern}")
    print(f"  测试数据:")
    for p in phones:
        if phone_pattern.search(p):
            print(f"    ✓ {p}")
        else:
            print(f"    ✗ {p}")
    print()

    # 性能对比
    text_list = ["test123", "abc456", "xyz789"] * 1000
    pattern_str = r'\d+'

    t1 = time.perf_counter()
    for _ in range(100):
        for t in text_list:
            re.search(pattern_str, t)
    time_no_compile = time.perf_counter() - t1

    compiled = re.compile(pattern_str)
    t2 = time.perf_counter()
    for _ in range(100):
        for t in text_list:
            compiled.search(t)
    time_compiled = time.perf_counter() - t2

    print(f"  性能对比（3000次匹配 × 100轮）:")
    print(f"    未编译: {time_no_compile * 1000:.2f} ms")
    print(f"    已编译: {time_compiled * 1000:.2f} ms")
    speedup = time_no_compile / time_compiled if time_compiled > 0 else 1
    print(f"    提速:   {speedup:.1f}x")
    print()

    # ── 标志位 ────────────────────────────────────────────
    print("2. 标志位（flags）：改变匹配行为")
    print()

    # re.IGNORECASE
    print("  re.IGNORECASE (re.I)：忽略大小写")
    text = "Hello WORLD hello world"
    print(f"    文本: '{text}'")
    print(f"    findall('hello'):          {re.findall('hello', text)}")
    print(f"    findall('hello', re.I):    {re.findall('hello', text, re.I)}")
    print()

    # re.MULTILINE
    print("  re.MULTILINE (re.M)：多行模式（^ 和 $ 匹配每行的开头/结尾）")
    text2 = "line1\nline2\nline3"
    print(f"    文本: {repr(text2)}")
    print(f"    findall('^line', text):       {re.findall('^line', text2)}")
    print(f"    findall('^line', text, re.M): {re.findall('^line', text2, re.M)}")
    print()

    # re.DOTALL
    print("  re.DOTALL (re.S)：. 匹配任意字符（包括换行符）")
    text3 = "start\nmiddle\nend"
    print(f"    文本: {repr(text3)}")
    print(f"    search('start.*end'):       {re.search('start.*end', text3)}")
    print(f"    search('start.*end', re.S): {re.search('start.*end', text3, re.S).group()}")
    print()

    # re.VERBOSE
    print("  re.VERBOSE (re.X)：详细模式（允许注释和空白，提升可读性）")
    email_pattern = re.compile(r"""
        [\w.+-]+        # 用户名：字母、数字、点、加号、连字符
        @               # @ 符号
        [\w-]+          # 域名
        \.              # 点
        [a-zA-Z]{2,}    # 顶级域（至少 2 个字母）
    """, re.VERBOSE)

    test_emails = ["alice@example.com", "bob.smith@test.org", "invalid@", "ok@a.io"]
    print(f"    测试邮箱:")
    for e in test_emails:
        result = '✓ 有效' if email_pattern.fullmatch(e) else '✗ 无效'
        print(f"      {result}: {e}")
    print()

    # 组合标志位
    print("  组合标志位（用 | 连接）:")
    text4 = "Hello\nWORLD"
    matches = re.findall(r'^hello$', text4, re.I | re.M)
    print(f"    文本: {repr(text4)}")
    print(f"    findall('^hello$', re.I | re.M): {matches}")


# =============================================================================
# 20.5 实战：常用正则表达式
# =============================================================================

def demo_practical_examples() -> None:
    """演示正则表达式的实际应用：数据验证、信息提取、文本处理。"""
    print("\n" + "=" * 60)
    print("20.5 实战：常用正则表达式")
    print("=" * 60)

    # ── 数据验证 ──────────────────────────────────────────
    print("1. 数据格式验证")
    print()

    validators = {
        '邮箱':   re.compile(r'^[\w.+-]+@[\w-]+\.[a-zA-Z]{2,}$'),
        '手机号': re.compile(r'^1[3-9]\d{9}$'),
        'IP地址': re.compile(r'^(\d{1,3}\.){3}\d{1,3}$'),
        '日期':   re.compile(r'^\d{4}-(0[1-9]|1[0-2])-(0[1-9]|[12]\d|3[01])$'),
    }

    test_data = {
        '邮箱':   ['alice@example.com', 'invalid-email', 'bob@test.org'],
        '手机号': ['13812345678', '12345678901', '18698765432'],
        'IP地址': ['192.168.1.1', '999.0.0.1', '10.0.0.255'],
        '日期':   ['2024-03-15', '2024-13-01', '2024-02-29'],
    }

    for name, pattern in validators.items():
        print(f"  {name} 验证:")
        for value in test_data[name]:
            ok = '✓' if pattern.match(value) else '✗'
            print(f"    {ok} {value}")
        print()

    # ── 信息提取 ──────────────────────────────────────────
    print("2. 从文本中提取结构化信息")
    print()

    log = """
2024-03-15 10:23:45 ERROR   [main] Database connection failed: timeout
2024-03-15 10:24:12 WARNING [api]  Response time exceeded 500ms
2024-03-15 10:25:03 INFO    [main] Retry attempt 1/3
2024-03-15 10:25:08 ERROR   [api]  HTTP 503: Service unavailable
""".strip()

    log_pattern = re.compile(
        r'(?P<date>\d{4}-\d{2}-\d{2}) '
        r'(?P<time>\d{2}:\d{2}:\d{2}) '
        r'(?P<level>\w+)\s+'
        r'\[(?P<module>\w+)\]\s+'
        r'(?P<message>.*)'
    )

    print(f"  日志文本:")
    for line in log.split('\n'):
        print(f"    {line}")
    print()
    print(f"  提取的结构化数据:")
    errors = []
    for match in log_pattern.finditer(log):
        d = match.groupdict()
        if d['level'] in ('ERROR', 'WARNING'):
            errors.append(d)
            print(f"    [{d['level']}] {d['date']} {d['time']} - {d['message']}")
    print()

    # ── 文本清洗 ──────────────────────────────────────────
    print("3. 文本清洗与格式化")
    print()

    dirty_text = "  Hello,   World!!!   This  is    a   test...  "
    print(f"  原文: '{dirty_text}'")

    # 合并多余空格
    cleaned = re.sub(r'\s+', ' ', dirty_text.strip())
    print(f"  合并空格: '{cleaned}'")

    # 去除标点重复
    cleaned2 = re.sub(r'([!?.，。])\1+', r'\1', cleaned)
    print(f"  去重标点: '{cleaned2}'")
    print()

    # 驼峰命名转下划线
    print("  驼峰命名 → 下划线命名:")
    camel_names = ['getUserName', 'MyClassName', 'parseHTMLContent', 'XMLParser']
    for name in camel_names:
        # 在大写字母前插入下划线，然后转小写
        snake = re.sub(r'(?<=[a-z0-9])(?=[A-Z])|(?<=[A-Z])(?=[A-Z][a-z])', '_', name).lower()
        print(f"    {name:<20} → {snake}")
    print()

    # ── URL 解析 ──────────────────────────────────────────
    print("4. URL 解析")
    print()

    url_pattern = re.compile(
        r'(?P<scheme>https?|ftp)://'
        r'(?P<host>[^/:]+)'
        r'(?::(?P<port>\d+))?'
        r'(?P<path>/[^?#]*)?'
        r'(?:\?(?P<query>[^#]*))?'
        r'(?:#(?P<fragment>.*))?'
    )

    urls = [
        'https://www.example.com/path/to/page?key=value&foo=bar#section',
        'http://api.test.org:8080/v1/users',
        'ftp://files.example.com/data.zip',
    ]

    for url in urls:
        m = url_pattern.match(url)
        if m:
            d = {k: v for k, v in m.groupdict().items() if v is not None}
            print(f"  URL: {url}")
            for k, v in d.items():
                print(f"    {k:<10}: {v}")
            print()


# =============================================================================
# 20.6 常见错误与陷阱
# =============================================================================

def demo_common_mistakes() -> None:
    """演示使用正则表达式时的常见错误。"""
    print("\n" + "=" * 60)
    print("20.6 常见错误与陷阱")
    print("=" * 60)

    # ── 错误 1：忘记转义特殊字符 ──────────────────────────
    print("1. 忘记转义特殊字符")
    print()

    text = "price: $19.99"
    print(f"  文本: '{text}'")

    # 错误：. 匹配任意字符
    wrong = re.findall(r'$19.99', text)
    print(f"  ❌ r'$19.99'   → {wrong}  （$ 是锚点，. 匹配任意字符）")

    # 正确：转义特殊字符
    correct = re.findall(r'\$19\.99', text)
    print(f"  ✓  r'\\$19\\.99' → {correct}")
    print()

    # ── 错误 2：贪婪匹配导致过度匹配 ─────────────────────
    print("2. 贪婪匹配导致过度匹配")
    print()

    html = "<b>bold</b> and <b>another bold</b>"
    print(f"  HTML: {html}")

    greedy = re.findall(r'<b>.*</b>', html)
    lazy   = re.findall(r'<b>.*?</b>', html)
    print(f"  ❌ 贪婪 r'<b>.*</b>'   → {greedy}")
    print(f"  ✓  非贪婪 r'<b>.*?</b>' → {lazy}")
    print()

    # ── 错误 3：忘记单词边界 ──────────────────────────────
    print("3. 忘记单词边界导致误匹配")
    print()

    text2 = "cat concatenate category"
    print(f"  文本: '{text2}'")

    without_boundary = re.findall(r'cat', text2)
    with_boundary    = re.findall(r'\bcat\b', text2)
    print(f"  ❌ r'cat'       → {without_boundary}  （匹配所有含 cat 的子串）")
    print(f"  ✓  r'\\bcat\\b'  → {with_boundary}   （只匹配独立单词）")
    print()

    # ── 错误 4：re.match vs re.search 混淆 ───────────────
    print("4. re.match() 与 re.search() 混淆")
    print()

    text3 = "hello world 123"
    digit_pattern = r'\d+'
    print(f"  文本: '{text3}'")
    match_result  = re.match(digit_pattern, text3)
    search_result = re.search(digit_pattern, text3).group()
    print(f"  re.match(r'\\d+', text):   {match_result}   （match 只从开头匹配）")
    print(f"  re.search(r'\\d+', text):  {search_result}  （search 扫描全文）")
    print()

    # ── 错误 5：忘记使用原始字符串 ───────────────────────
    print("5. 忘记使用原始字符串 r'...'")
    print()
    print("  Python 字符串中 \\ 是转义符，正则中 \\ 也是转义符")
    print("  不用 r'' 时需要双重转义，容易出错：")
    print()
    print("  ❌ '\\d+'   实际模式: \\d+  （可能被误解）")
    print("  ✓  r'\\d+'  实际模式: \\d+  （推荐写法）")
    print()
    # 对比
    import re as _re
    p1 = _re.compile('\\d+')   # 双反斜杠
    p2 = _re.compile(r'\d+')   # 原始字符串
    print(f"  '\\\\d+' 和 r'\\d+' 的模式相同: {p1.pattern == p2.pattern}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示函数。"""
    demo_regex_basics()
    demo_re_functions()
    demo_groups()
    demo_compile_and_flags()
    demo_practical_examples()
    demo_common_mistakes()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── 元字符速查 ────────────────────────────────────────────
#   .          匹配任意字符（除换行符）
#   ^          匹配字符串开头（re.M 模式下匹配行首）
#   $          匹配字符串结尾（re.M 模式下匹配行尾）
#   *          匹配 0 次或多次（贪婪）
#   +          匹配 1 次或多次（贪婪）
#   ?          匹配 0 次或 1 次（贪婪）
#   {m,n}      匹配 m 到 n 次（贪婪）
#   *? +? ??   非贪婪版本
#   []         字符类
#   |          或运算
#   ()         捕获组
#   \          转义字符
#
# ── 字符类速查 ────────────────────────────────────────────
#   \d         数字 [0-9]
#   \D         非数字
#   \w         单词字符 [a-zA-Z0-9_]
#   \W         非单词字符
#   \s         空白字符（空格、\t、\n、\r 等）
#   \S         非空白字符
#   \b         单词边界
#   \B         非单词边界
#
# ── 分组语法 ──────────────────────────────────────────────
#   (...)              捕获组，用 group(1) 或 \1 引用
#   (?P<name>...)      命名捕获组，用 group('name') 引用
#   (?:...)            非捕获组（分组但不捕获）
#   \1 \2              反向引用（引用第 N 个捕获组）
#
# ── re 模块函数速查 ───────────────────────────────────────
#   re.match(p, s)         从字符串开头匹配，返回 Match 或 None
#   re.search(p, s)        搜索整个字符串，返回第一个 Match 或 None
#   re.findall(p, s)       返回所有匹配的列表
#   re.finditer(p, s)      返回所有 Match 对象的迭代器
#   re.sub(p, repl, s)     替换所有匹配，repl 可以是字符串或函数
#   re.split(p, s)         按模式分割字符串
#   re.compile(p, flags)   预编译模式，返回 Pattern 对象
#   re.fullmatch(p, s)     要求整个字符串完全匹配
#
# ── 标志位速查 ────────────────────────────────────────────
#   re.IGNORECASE  re.I   忽略大小写
#   re.MULTILINE   re.M   ^ $ 匹配每行首尾
#   re.DOTALL      re.S   . 匹配包括换行在内的所有字符
#   re.VERBOSE     re.X   详细模式（允许注释和空白）
#   re.ASCII       re.A   \w \d \s 等只匹配 ASCII 字符
#
# ── Match 对象方法 ────────────────────────────────────────
#   m.group()      完整匹配的字符串（等同 m.group(0)）
#   m.group(n)     第 n 个捕获组的内容
#   m.group('name')  命名组的内容
#   m.groups()     所有捕获组的元组
#   m.groupdict()  命名组的字典
#   m.span()       匹配的 (start, end) 位置
#   m.start()      匹配开始位置
#   m.end()        匹配结束位置


# =============================================================================
# 【常见错误】
# =============================================================================
#
# ❌ 错误 1：忘记转义特殊字符
#   特殊字符 . * + ? ^ $ {} [] () | \ 在正则中有特殊含义
#   需要匹配字面量时必须用 \ 转义
#   错误：r'$19.99'    正确：r'\$19\.99'
#
# ❌ 错误 2：贪婪匹配导致过度匹配
#   默认贪婪匹配会尽可能多地匹配字符
#   提取 HTML 标签等场景应使用非贪婪：r'<b>.*?</b>'
#
# ❌ 错误 3：忘记单词边界
#   r'cat' 会匹配 "concatenate" 中的 cat
#   使用 r'\bcat\b' 只匹配独立的单词
#
# ❌ 错误 4：混淆 re.match() 与 re.search()
#   match() 只从字符串开头匹配（类似加了隐式 ^）
#   search() 扫描整个字符串查找第一个匹配
#
# ❌ 错误 5：忘记使用原始字符串 r'...'
#   正则表达式应始终使用 r'' 原始字符串，避免双重转义混乱
#   推荐：r'\d+\.\d+'   避免：'\\d+\\.\\d+'
#
# ❌ 错误 6：对复杂正则不使用 re.compile()
#   在循环中重复使用同一正则时，应提前编译
#   pattern = re.compile(r'\d+')
#   for s in data: pattern.search(s)


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   编写函数 check_password(pwd)，验证密码是否满足以下要求：
#   - 长度 8-20 位
#   - 包含至少一个大写字母
#   - 包含至少一个小写字母
#   - 包含至少一个数字
#   - 只包含字母、数字和 @#$%^&+=
#   返回格式：True/False
#   例如：
#     check_password('Abc12345')
#     # True
#     check_password('abc12345')
#     # False（无大写字母）
#
# 练习 2（进阶）：
#   从 Markdown 文本中提取所有链接，格式为 [文字](URL)。
#   返回格式：[(文字, URL), ...] 元组列表
#   例如：
#     text = '点击[这里](https://example.com)和[那里](http://test.org)'
#     extract_md_links(text)
#     # [('这里', 'https://example.com'), ('那里', 'http://test.org')]
#
# 练习 3（综合）：
#   将各种格式的手机号统一格式化为 +86-138-1234-5678。
#   返回格式：格式化后的字符串，无效输入返回 None
#   例如：
#     format_phone('13812345678')
#     # '+86-138-1234-5678'
#     format_phone('138 1234 5678')
#     # '+86-138-1234-5678'
#     format_phone('12345')
#     # None（无效手机号）
#
# 练习 4（高级）：
#   提取文本中的所有英文单词（忽略大小写），统计每个单词出现的次数。
#   要求：只统计纯字母单词，忽略数字和标点
#   返回格式：{单词: 次数} 字典，按词频降序排列
#   例如：
#     text = "To be or not to be, that is the question."
#     word_frequency(text)
#     # {'to': 2, 'be': 2, 'or': 1, 'not': 1, ...}
#
# 练习答案提示：
#   练习1：用 re.match 检查整体格式，用 re.search 检查各项要求
#   练习2：r'\[([^\]]+)\]\(([^)]+)\)' 匹配 [文字](URL) 格式
#   练习3：先用 re.sub(r'\D', '', phone) 提取纯数字，再格式化
#   练习4：re.findall(r'\b[a-zA-Z]+\b', text.lower()) 提取单词，用字典统计


# =============================================================================
# 【练习答案】
# =============================================================================


def check_password(pwd: str) -> bool:
    """
    练习 1：验证密码强度。

    Args:
        pwd: 待验证的密码字符串

    Returns:
        True 表示密码符合要求，False 表示不符合

    Example:
        check_password('Abc12345')
        # True
        check_password('abc12345')
        # False（无大写字母）
    """
    # 检查整体格式：长度 8-20，只包含字母、数字和特殊字符
    if not re.match(r'^[a-zA-Z0-9@#$%^&+=]{8,20}$', pwd):
        return False
    # 检查至少一个大写字母
    if not re.search(r'[A-Z]', pwd):
        return False
    # 检查至少一个小写字母
    if not re.search(r'[a-z]', pwd):
        return False
    # 检查至少一个数字
    if not re.search(r'\d', pwd):
        return False
    return True


def extract_md_links(text: str) -> list[tuple[str, str]]:
    """
    练习 2：从 Markdown 文本中提取所有链接。

    Args:
        text: Markdown 格式的文本

    Returns:
        [(文字, URL), ...] 元组列表

    Example:
        text = '点击[这里](https://example.com)和[那里](http://test.org)'
        extract_md_links(text)
        # [('这里', 'https://example.com'), ('那里', 'http://test.org')]
    """
    # [文字](URL) 格式：[非]字符]+(非)字符)
    return re.findall(r'\[([^\]]+)\]\(([^)]+)\)', text)


def format_phone(phone: str) -> Optional[str]:
    """
    练习 3：统一格式化手机号为 +86-138-1234-5678。

    Args:
        phone: 各种格式的手机号字符串

    Returns:
        格式化后的字符串，无效输入返回 None

    Example:
        format_phone('13812345678')
        # '+86-138-1234-5678'
        format_phone('138 1234 5678')
        # '+86-138-1234-5678'
        format_phone('12345')
        # None（无效手机号）
    """
    # 去除所有非数字字符，提取纯数字
    digits = re.sub(r'\D', '', phone)
    # 去除中国国际区号 86
    if digits.startswith('86') and len(digits) == 13:
        digits = digits[2:]
    # 验证是否为有效的 11 位手机号（以 1 开头）
    if len(digits) == 11 and digits.startswith('1'):
        return f'+86-{digits[0:3]}-{digits[3:7]}-{digits[7:11]}'
    return None


def word_frequency(text: str) -> dict[str, int]:
    """
    练习 4：统计文本中英文单词的词频（忽略大小写）。

    Args:
        text: 待统计的文本

    Returns:
        {单词: 次数} 字典，按词频降序排列

    Example:
        text = "To be or not to be, that is the question."
        word_frequency(text)
        # {'to': 2, 'be': 2, 'or': 1, 'not': 1, ...}
    """
    # 提取所有纯字母单词（忽略大小写）
    words = re.findall(r'\b[a-zA-Z]+\b', text.lower())
    # 统计词频
    freq: dict[str, int] = {}
    for word in words:
        freq[word] = freq.get(word, 0) + 1
    # 按词频降序排列
    return dict(sorted(freq.items(), key=lambda x: x[1], reverse=True))


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 check_password 函数。"""
    print("check_password 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    test_passwords = [
        ('Abc12345',        True,   '符合所有要求'),
        ('abc12345',        False,  '无大写字母'),
        ('ABC12345',        False,  '无小写字母'),
        ('Abcdefgh',        False,  '无数字'),
        ('Ab1',             False,  '太短（少于8位）'),
        ('Abc@123#456',     True,   '包含特殊字符'),
        ('Abc123456789012', False,  '太长（超过20位）'),
        ('Abc123!',         False,  '包含非法字符 !'),
    ]

    print(f"  {'密码':<20} {'结果':<8} {'说明'}")
    print("  " + "-" * 50)
    for pwd, expected, desc in test_passwords:
        result = check_password(pwd)
        status = '✓' if result == expected else '✗'
        result_str = '通过' if result else '失败'
        print(f"  {status} {pwd:<18} {result_str:<8} {desc}")


def exercise2_answer() -> None:
    """练习 2：演示 extract_md_links 函数。"""
    print("extract_md_links 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    md_text = '点击[这里](https://example.com)和[那里](http://test.org)了解详情'
    links = extract_md_links(md_text)
    print(f"  输入: {md_text}")
    print(f"  输出: {links}")
    print()

    # ── 多个链接 ──────────────────────────────────────────
    md_text2 = """
查看[官方文档](https://docs.python.org)获取更多信息。
访问[GitHub](https://github.com)和[Stack Overflow](https://stackoverflow.com)。
    """.strip()
    links2 = extract_md_links(md_text2)
    print(f"  多链接文本:")
    for text, url in links2:
        print(f"    文字: {text:<20} URL: {url}")


def exercise3_answer() -> None:
    """练习 3：演示 format_phone 函数。"""
    print("format_phone 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    test_phones = [
        ('13812345678',      '+86-138-1234-5678'),
        ('138 1234 5678',    '+86-138-1234-5678'),
        ('138-1234-5678',    '+86-138-1234-5678'),
        ('+8613812345678',   '+86-138-1234-5678'),
        ('86-138-1234-5678', '+86-138-1234-5678'),
        ('12345',            None),
        ('23812345678',      None),  # 不以1开头
    ]

    print(f"  {'输入':<20} {'输出'}")
    print("  " + "-" * 50)
    for phone, expected in test_phones:
        result = format_phone(phone)
        status = '✓' if result == expected else '✗'
        print(f"  {status} {phone:<18} → {result}")


def exercise4_answer() -> None:
    """练习 4：演示 word_frequency 函数。"""
    print("word_frequency 演示:")

    # ── 基本示例 ─────────────────────────────────────────
    sample = "To be or not to be, that is the question. To be is to exist."
    freq = word_frequency(sample)
    print(f"  文本: {sample}")
    print()
    print(f"  词频统计（前 10）:")
    print(f"  {'单词':<15} {'次数'}")
    print("  " + "-" * 25)
    for word, count in list(freq.items())[:10]:
        print(f"  {word:<15} {count}")
    print()

    # ── 验证忽略标点和数字 ────────────────────────────────
    sample2 = "Hello123 world! Python3.11 is great. Hello, Python!"
    freq2 = word_frequency(sample2)
    print(f"  文本: {sample2}")
    print(f"  提取的单词: {list(freq2.keys())}")
    print(f"  （注意：数字被忽略，只保留纯字母单词）")


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 40)
#     exercise1_answer()
#
#     print("\n" + "=" * 40)
#     exercise2_answer()
#
#     print("\n" + "=" * 40)
#     exercise3_answer()
#
#     print("\n" + "=" * 40)
#     exercise4_answer()
