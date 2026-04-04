# =============================================================================
# 第 16 章：文本处理（json、csv、re）
# =============================================================================
#
# 【学习目标】
#   1. 掌握 json 模块：JSON 数据的序列化和反序列化
#   2. 掌握 csv 模块：CSV 文件的读写操作
#   3. 掌握 re 模块：正则表达式的使用
#   4. 理解文本数据处理的常见模式
#   5. 学会处理实际项目中的数据格式转换
#
# 【这些模块解决什么问题？】
#   json:  处理 JSON 格式数据，与 Web API 交互、配置文件
#   csv:   处理表格数据，Excel 导入导出、数据分析
#   re:    文本模式匹配、数据提取、字符串验证
#
# 【与 C/C++ 的对比】
#   C/C++:  需要手动解析 JSON/CSV，正则表达式需要第三方库
#   Python: 标准库内置，使用简单，功能强大
#
# 【运行方式】
#   python chapter16_文本处理.py
#
# =============================================================================

import json
import csv
import re
from pathlib import Path
from typing import Any, List, Dict, Optional
from io import StringIO
import tempfile


# =============================================================================
# 16.1 json 模块：JSON 数据处理
# =============================================================================

def demo_json_module() -> None:
    """演示 json 模块的常用功能。"""
    print("=" * 60)
    print("16.1 json 模块：JSON 数据处理")
    print("=" * 60)

    # ── Python 对象转 JSON 字符串 ────────────────────────
    print("1. 序列化：Python → JSON（json.dumps）:")

    data = {
        "name": "张三",
        "age": 25,
        "is_student": True,
        "scores": [85, 90, 78],
        "address": {
            "city": "北京",
            "district": "朝阳区"
        },
        "tags": None,
    }

    # 基本序列化
    json_str = json.dumps(data)
    print(f"  紧凑格式: {json_str[:50]}...")

    # 格式化输出（便于阅读）
    json_pretty = json.dumps(data, indent=2, ensure_ascii=False)
    print(f"  格式化输出:\n{json_pretty}")
    print()

    # ── JSON 字符串转 Python 对象 ────────────────────────
    print("2. 反序列化：JSON → Python（json.loads）:")

    json_text = '{"name": "李四", "age": 30, "skills": ["Python", "Java"]}'
    obj = json.loads(json_text)
    print(f"  JSON: {json_text}")
    print(f"  Python 对象: {obj}")
    print(f"  类型: {type(obj)}")
    print(f"  访问数据: name={obj['name']}, skills={obj['skills']}")
    print()

    # ── 文件读写 ─────────────────────────────────────────
    print("3. JSON 文件读写:")

    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False, encoding='utf-8') as f:
        temp_file = f.name
        # 写入 JSON 文件
        json.dump(data, f, indent=2, ensure_ascii=False)
        print(f"  ✓ 写入文件: {temp_file}")

    # 读取 JSON 文件
    with open(temp_file, 'r', encoding='utf-8') as f:
        loaded_data = json.load(f)
        print(f"  ✓ 读取文件: {loaded_data['name']}, 年龄 {loaded_data['age']}")

    Path(temp_file).unlink()  # 清理临时文件
    print()

    # ── 处理特殊类型 ─────────────────────────────────────
    print("4. 处理特殊类型（自定义编码器）:")

    from datetime import datetime

    class DateTimeEncoder(json.JSONEncoder):
        """自定义 JSON 编码器：支持 datetime 类型"""
        def default(self, obj: Any) -> Any:
            if isinstance(obj, datetime):
                return obj.isoformat()
            return super().default(obj)

    data_with_date = {
        "event": "会议",
        "time": datetime(2024, 1, 15, 14, 30),
    }

    json_with_date = json.dumps(data_with_date, cls=DateTimeEncoder, indent=2)
    print(f"  带日期的 JSON:\n{json_with_date}")
    print()

    # ── JSON 数据类型映射 ────────────────────────────────
    print("5. Python ↔ JSON 类型映射:")
    print("  Python          →  JSON")
    print("  dict            →  object")
    print("  list, tuple     →  array")
    print("  str             →  string")
    print("  int, float      →  number")
    print("  True            →  true")
    print("  False           →  false")
    print("  None            →  null")
    print()

    # ── 常用参数 ─────────────────────────────────────────
    print("6. json.dumps 常用参数:")
    print("  indent=2           # 缩进空格数（格式化）")
    print("  ensure_ascii=False # 允许非 ASCII 字符（中文）")
    print("  sort_keys=True     # 按键排序")
    print("  separators=(',', ':')  # 自定义分隔符（紧凑）")

    compact = json.dumps(data, separators=(',', ':'), ensure_ascii=False)
    print(f"  紧凑格式: {compact[:60]}...")


# =============================================================================
# 16.2 csv 模块：CSV 文件处理
# =============================================================================

def demo_csv_module() -> None:
    """演示 csv 模块的常用功能。"""
    print("\n" + "=" * 60)
    print("16.2 csv 模块：CSV 文件处理")
    print("=" * 60)

    # ── 写入 CSV 文件 ────────────────────────────────────
    print("1. 写入 CSV 文件（csv.writer）:")

    with tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False,
                                     encoding='utf-8', newline='') as f:
        temp_csv = f.name
        writer = csv.writer(f)

        # 写入表头
        writer.writerow(['姓名', '年龄', '城市', '分数'])

        # 写入数据行
        writer.writerow(['张三', 25, '北京', 85])
        writer.writerow(['李四', 30, '上海', 90])
        writer.writerow(['王五', 28, '广州', 78])

        print(f"  ✓ 写入 CSV: {temp_csv}")

    # 读取并显示
    with open(temp_csv, 'r', encoding='utf-8') as f:
        print(f"  文件内容:\n{f.read()}")

    # ── 读取 CSV 文件 ────────────────────────────────────
    print("2. 读取 CSV 文件（csv.reader）:")

    with open(temp_csv, 'r', encoding='utf-8', newline='') as f:
        reader = csv.reader(f)
        for i, row in enumerate(reader):
            if i == 0:
                print(f"  表头: {row}")
            else:
                print(f"  数据行 {i}: {row}")
    print()

    # ── 字典方式读写 ─────────────────────────────────────
    print("3. 字典方式读写（DictReader/DictWriter）:")

    # 使用 DictWriter 写入
    with tempfile.NamedTemporaryFile(mode='w', suffix='.csv', delete=False,
                                     encoding='utf-8', newline='') as f:
        temp_dict_csv = f.name
        fieldnames = ['name', 'age', 'city']
        writer = csv.DictWriter(f, fieldnames=fieldnames)

        writer.writeheader()  # 写入表头
        writer.writerow({'name': '赵六', 'age': 35, 'city': '深圳'})
        writer.writerow({'name': '孙七', 'age': 22, 'city': '杭州'})

        print(f"  ✓ 使用 DictWriter 写入")

    # 使用 DictReader 读取
    with open(temp_dict_csv, 'r', encoding='utf-8', newline='') as f:
        reader = csv.DictReader(f)
        print(f"  使用 DictReader 读取:")
        for row in reader:
            print(f"    {row['name']}, {row['age']}岁, 来自{row['city']}")
    print()

    # ── 处理不同分隔符 ───────────────────────────────────
    print("4. 处理不同分隔符（delimiter）:")

    # TSV（Tab 分隔）
    tsv_data = StringIO("姓名\t年龄\t城市\n张三\t25\t北京\n李四\t30\t上海")
    reader = csv.reader(tsv_data, delimiter='\t')
    print("  TSV 数据（Tab 分隔）:")
    for row in reader:
        print(f"    {row}")
    print()

    # ── 处理引号和转义 ───────────────────────────────────
    print("5. 处理特殊字符（引号、逗号、换行）:")

    special_data = [
        ['姓名', '描述'],
        ['张三', '他说："你好"'],
        ['李四', '地址：北京市,朝阳区'],
        ['王五', '备注：第一行\n第二行'],
    ]

    output = StringIO()
    writer = csv.writer(output)
    writer.writerows(special_data)
    csv_content = output.getvalue()
    print(f"  CSV 内容:\n{csv_content}")

    # 读取回来
    input_stream = StringIO(csv_content)
    reader = csv.reader(input_stream)
    print("  解析结果:")
    for row in reader:
        print(f"    {row}")

    # 清理临时文件
    Path(temp_csv).unlink()
    Path(temp_dict_csv).unlink()


# =============================================================================
# 16.3 re 模块：正则表达式
# =============================================================================

def demo_re_module() -> None:
    """演示 re 模块的常用功能。"""
    print("\n" + "=" * 60)
    print("16.3 re 模块：正则表达式")
    print("=" * 60)

    # ── 基本匹配 ─────────────────────────────────────────
    print("1. 基本匹配（re.match, re.search, re.findall）:")

    text = "我的手机号是 13812345678，邮箱是 user@example.com"

    # match：从字符串开头匹配
    match = re.match(r'我的', text)
    print(f"  match（开头匹配）: {match.group() if match else '未匹配'}")

    # search：在整个字符串中搜索第一个匹配
    search = re.search(r'\d{11}', text)  # 11位数字
    print(f"  search（搜索）: {search.group() if search else '未匹配'}")

    # findall：找出所有匹配
    numbers = re.findall(r'\d+', text)
    print(f"  findall（所有数字）: {numbers}")
    print()

    # ── 常用正则模式 ─────────────────────────────────────
    print("2. 常用正则表达式模式:")
    patterns = {
        r'\d+':     '一个或多个数字',
        r'\w+':     '一个或多个字母/数字/下划线',
        r'\s+':     '一个或多个空白字符',
        r'[a-z]+':  '一个或多个小写字母',
        r'[A-Z]+':  '一个或多个大写字母',
        r'\d{3}':   '恰好3个数字',
        r'\d{3,5}': '3到5个数字',
        r'^开头':   '以"开头"开始',
        r'结尾$':   '以"结尾"结束',
        r'(abc)+':  '一个或多个"abc"',
        r'a|b':     '"a"或"b"',
        r'[^0-9]':  '非数字字符',
    }
    for pattern, desc in patterns.items():
        print(f"  {pattern:<20} {desc}")
    print()

    # ── 分组捕获 ─────────────────────────────────────────
    print("3. 分组捕获（括号）:")

    email_text = "联系方式：user@example.com 和 admin@test.org"
    email_pattern = r'(\w+)@(\w+\.\w+)'

    matches = re.findall(email_pattern, email_text)
    print(f"  文本: {email_text}")
    print(f"  模式: {email_pattern}")
    print(f"  匹配结果:")
    for username, domain in matches:
        print(f"    用户名={username}, 域名={domain}")
    print()

    # ── 命名分组 ─────────────────────────────────────────
    print("4. 命名分组（?P<name>）:")

    phone_text = "手机：138-1234-5678"
    phone_pattern = r'(?P<area>\d{3})-(?P<prefix>\d{4})-(?P<suffix>\d{4})'

    match = re.search(phone_pattern, phone_text)
    if match:
        print(f"  文本: {phone_text}")
        print(f"  区号: {match.group('area')}")
        print(f"  前缀: {match.group('prefix')}")
        print(f"  后缀: {match.group('suffix')}")
        print(f"  完整匹配: {match.group(0)}")
        print(f"  字典形式: {match.groupdict()}")
    print()

    # ── 替换 ─────────────────────────────────────────────
    print("5. 替换（re.sub）:")

    text = "价格：100元，折扣：50元"
    # 将所有数字替换为 ***
    masked = re.sub(r'\d+', '***', text)
    print(f"  原文: {text}")
    print(f"  替换后: {masked}")

    # 使用函数进行替换
    def double_number(m: re.Match) -> str:
        return str(int(m.group()) * 2)

    doubled = re.sub(r'\d+', double_number, text)
    print(f"  数字翻倍: {doubled}")
    print()

    # ── 分割 ─────────────────────────────────────────────
    print("6. 分割（re.split）:")

    text = "apple,banana;orange|grape"
    parts = re.split(r'[,;|]', text)  # 按多种分隔符分割
    print(f"  原文: {text}")
    print(f"  分割结果: {parts}")
    print()

    # ── 编译正则表达式 ───────────────────────────────────
    print("7. 编译正则表达式（re.compile）:")
    print("  （提高性能，重复使用）")

    email_regex = re.compile(r'\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}\b')
    text = "联系我们：info@company.com 或 support@service.org"

    emails = email_regex.findall(text)
    print(f"  找到的邮箱: {emails}")
    print()

    # ── 标志位 ───────────────────────────────────────────
    print("8. 正则表达式标志:")
    print("  re.IGNORECASE (re.I)  忽略大小写")
    print("  re.MULTILINE (re.M)   多行模式（^和$匹配每行）")
    print("  re.DOTALL (re.S)      .匹配包括换行符")
    print("  re.VERBOSE (re.X)     允许写注释和空格")

    text = "Hello WORLD"
    match = re.search(r'hello', text, re.IGNORECASE)
    print(f"  忽略大小写匹配: {match.group() if match else '未匹配'}")


# =============================================================================
# 16.4 实战案例：数据验证
# =============================================================================

def demo_validation() -> None:
    """演示常见的数据验证场景。"""
    print("\n" + "=" * 60)
    print("16.4 实战案例：数据验证")
    print("=" * 60)

    # ── 手机号验证 ───────────────────────────────────────
    def validate_phone(phone: str) -> bool:
        """验证中国大陆手机号（11位，1开头）"""
        pattern = r'^1[3-9]\d{9}$'
        return re.match(pattern, phone) is not None

    print("1. 手机号验证:")
    test_phones = ['13812345678', '12345678901', '138123456789']
    for phone in test_phones:
        result = '✓' if validate_phone(phone) else '✗'
        print(f"  {result} {phone}")
    print()

    # ── 邮箱验证 ─────────────────────────────────────────
    def validate_email(email: str) -> bool:
        """验证邮箱格式"""
        pattern = r'^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$'
        return re.match(pattern, email) is not None

    print("2. 邮箱验证:")
    test_emails = ['user@example.com', 'invalid.email', 'test@domain.co.uk']
    for email in test_emails:
        result = '✓' if validate_email(email) else '✗'
        print(f"  {result} {email}")
    print()

    # ── 身份证号验证 ─────────────────────────────────────
    def validate_id_card(id_card: str) -> bool:
        """验证18位身份证号（简化版）"""
        pattern = r'^\d{17}[\dXx]$'
        return re.match(pattern, id_card) is not None

    print("3. 身份证号验证（简化）:")
    test_ids = ['110101199001011234', '12345678901234567X', '123456']
    for id_card in test_ids:
        result = '✓' if validate_id_card(id_card) else '✗'
        print(f"  {result} {id_card}")
    print()

    # ── URL 验证 ─────────────────────────────────────────
    def validate_url(url: str) -> bool:
        """验证 URL 格式"""
        pattern = r'^https?://[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}(/.*)?$'
        return re.match(pattern, url) is not None

    print("4. URL 验证:")
    test_urls = [
        'https://www.example.com',
        'http://test.org/path',
        'ftp://invalid.com',
    ]
    for url in test_urls:
        result = '✓' if validate_url(url) else '✗'
        print(f"  {result} {url}")


# =============================================================================
# 16.5 实战案例：日志解析
# =============================================================================

def demo_log_parsing() -> None:
    """演示日志文件解析。"""
    print("\n" + "=" * 60)
    print("16.5 实战案例：日志解析")
    print("=" * 60)

    # 模拟日志内容
    log_content = (
        "\n"
        "2024-01-15 10:30:45 [INFO] 用户登录成功 user_id=1001\n"
        "2024-01-15 10:31:20 [ERROR] 数据库连接失败 error_code=500\n"
        "2024-01-15 10:32:10 [WARNING] 内存使用率过高 usage=85%\n"
        "2024-01-15 10:33:05 [INFO] 订单创建成功 order_id=20240115001\n"
        "2024-01-15 10:34:30 [ERROR] 支付失败 user_id=1002 amount=99.99\n"
    )

    print("原始日志:")
    print(log_content)

    # ── 解析日志行 ───────────────────────────────────────
    log_pattern = r'(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) \[(\w+)\] (.+)'

    print("解析结果:")
    for line in log_content.strip().split('\n'):
        m = re.match(log_pattern, line)
        if m:
            timestamp, level, message = m.groups()
            print(f"  时间={timestamp}, 级别={level}, 消息={message}")
    print()

    # ── 统计错误日志 ─────────────────────────────────────
    error_logs = re.findall(r'\[ERROR\] (.+)', log_content)
    print(f"错误日志统计（共 {len(error_logs)} 条）:")
    for err in error_logs:
        print(f"  ✗ {err}")
    print()

    # ── 提取键值对 ───────────────────────────────────────
    print("提取日志中的键值对:")
    kv_pattern = r'(\w+)=(\S+)'
    for line in log_content.strip().split('\n'):
        kvs = re.findall(kv_pattern, line)
        if kvs:
            kv_str = ', '.join(f"{k}={v}" for k, v in kvs)
            print(f"  {kv_str}")


# =============================================================================
# 16.6 综合实战：配置文件转换器
# =============================================================================

def demo_config_converter() -> None:
    """演示综合实战：将简单格式配置转换为 JSON。"""
    print("\n" + "=" * 60)
    print("16.6 综合实战：配置文件转换器")
    print("=" * 60)

    # 模拟 .ini 风格的配置文件内容
    ini_content = (
        "\n"
        "# 数据库配置\n"
        "[database]\n"
        "host = localhost\n"
        "port = 5432\n"
        "name = myapp_db\n"
        "user = admin\n"
        "password = secret123\n"
        "\n"
        "# 应用配置\n"
        "[app]\n"
        "debug = true\n"
        "max_connections = 100\n"
        "timeout = 30.5\n"
        "allowed_hosts = localhost,127.0.0.1,192.168.1.0\n"
    )

    print("输入（INI 格式）:")
    print(ini_content)

    def parse_ini(content: str) -> Dict[str, Dict[str, Any]]:
        """解析简单的 INI 格式配置文件。"""
        result: Dict[str, Dict[str, Any]] = {}
        current_section: Optional[str] = None

        # 段落标题：[section_name]
        section_pattern = re.compile(r'^\[(\w+)\]$')
        # 键值对：key = value
        kv_pattern = re.compile(r'^(\w+)\s*=\s*(.+)$')
        # 注释行：# 开头
        comment_pattern = re.compile(r'^\s*#')

        for line in content.split('\n'):
            line = line.strip()

            if not line or comment_pattern.match(line):
                continue  # 跳过空行和注释

            section_match = section_pattern.match(line)
            if section_match:
                current_section = section_match.group(1)
                result[current_section] = {}
                continue

            kv_match = kv_pattern.match(line)
            if kv_match and current_section:
                key, value = kv_match.group(1), kv_match.group(2).strip()
                # 智能类型转换
                result[current_section][key] = _convert_value(value)

        return result

    def _convert_value(value: str) -> Any:
        """将字符串值转换为合适的 Python 类型。"""
        # 布尔值
        if value.lower() == 'true':
            return True
        if value.lower() == 'false':
            return False
        # 整数
        try:
            return int(value)
        except ValueError:
            pass
        # 浮点数
        try:
            return float(value)
        except ValueError:
            pass
        # 列表（逗号分隔）
        if ',' in value:
            return [v.strip() for v in value.split(',')]
        # 字符串
        return value

    # 解析 INI 配置
    config = parse_ini(ini_content)

    # 输出为 JSON 格式
    json_output = json.dumps(config, indent=2, ensure_ascii=False)
    print("输出（JSON 格式）:")
    print(json_output)

    # 验证结果
    print("\n验证解析结果:")
    print(f"  数据库主机: {config['database']['host']}")
    print(f"  端口（int）: {config['database']['port']} ({type(config['database']['port']).__name__})")
    print(f"  调试模式（bool）: {config['app']['debug']} ({type(config['app']['debug']).__name__})")
    print(f"  超时（float）: {config['app']['timeout']} ({type(config['app']['timeout']).__name__})")
    print(f"  允许主机（list）: {config['app']['allowed_hosts']}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """主程序：运行所有演示。"""
    demo_json_module()
    demo_csv_module()
    demo_re_module()
    demo_validation()
    demo_log_parsing()
    demo_config_converter()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── json 常用函数 ──
# json.dumps(obj, indent=2, ensure_ascii=False)  Python → JSON 字符串
# json.loads(json_str)                           JSON 字符串 → Python
# json.dump(obj, file_obj)                       Python → JSON 文件
# json.load(file_obj)                            JSON 文件 → Python
# json.JSONEncoder                               自定义编码器基类
#
# ── csv 常用类 ──
# csv.reader(file)                 读取 CSV（返回列表的迭代器）
# csv.writer(file)                 写入 CSV
# writer.writerow(row)             写入一行
# writer.writerows(rows)           写入多行
# csv.DictReader(file)             读取为字典
# csv.DictWriter(file, fieldnames) 写入字典
# writer.writeheader()             写入表头
# csv.reader(file, delimiter='\t') 自定义分隔符
#
# ── re 常用函数 ──
# re.match(pattern, string)     从开头匹配
# re.search(pattern, string)    搜索第一个匹配
# re.findall(pattern, string)   返回所有匹配列表
# re.finditer(pattern, string)  返回匹配迭代器
# re.sub(pattern, repl, string) 替换匹配项
# re.split(pattern, string)     按模式分割
# re.compile(pattern)           编译为正则对象
#
# ── 正则元字符速查 ──
# .      任意字符（除换行）    \d     数字 [0-9]
# *      0个或多个             \D     非数字
# +      1个或多个             \w     字母/数字/下划线
# ?      0个或1个              \W     非\w
# {n}    恰好n个               \s     空白字符
# {n,m}  n到m个               \S     非空白
# ^      字符串开头            \b     单词边界
# $      字符串结尾            []     字符集合
# |      或                    ()     分组捕获
# (?:)   非捕获分组            (?P<name>) 命名分组


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：json.dumps 中文乱码
# json.dumps({"名": "张三"})           # ❌ 输出 \u5f20\u4e09
# json.dumps({"名": "张三"}, ensure_ascii=False)  # ✓ 输出中文
#
# 错误 2：csv 文件出现空行
# with open('file.csv', 'w') as f:     # ❌ 没有 newline=''
# with open('file.csv', 'w', newline='') as f:  # ✓ Windows 必须加
#
# 错误 3：正则表达式漏用 r 字符串
# re.search('\d+', text)   # ❌ \d 被当作转义序列
# re.search(r'\d+', text)  # ✓ 使用原始字符串
#
# 错误 4：re.match 和 re.search 混淆
# re.match(r'\d+', 'abc123')   # None！match 只从开头匹配
# re.search(r'\d+', 'abc123')  # 找到 '123'
#
# 错误 5：csv.DictReader 修改行
# row['name'] = 'new'  # ❌ DictReader 返回的是 dict，修改不影响文件
# 解决：需要重新写入文件
#
# 错误 6：json.load 和 json.loads 混淆
# json.load('{"a": 1}')    # ❌ load 接受文件对象
# json.loads('{"a": 1}')   # ✓ loads 接受字符串（s = string）


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   实现一个函数 increment_ages(json_str)：
#   接受一个 JSON 字符串，将其中顶层所有 "age" 字段的值加 1，
#   返回修改后的 JSON 字符串（保留中文、格式化缩进为 2）
#   若输入不是合法 JSON，抛出 ValueError
#   例如：
#     increment_ages('{"name": "张三", "age": 25}')
#     # '{\n  "name": "张三",\n  "age": 26\n}'
#     increment_ages('{"items": [{"age": 10}, {"age": 20}]}')
#     # items 里的 age 不变（只处理顶层），结果 age 字段不存在则不报错
#     increment_ages('not json')
#     # ValueError: 无效的 JSON 字符串: ...
#
# 练习 2（进阶）：
#   实现函数 csv_column_avg(csv_text, column)：
#   接受 CSV 文本字符串和列名，计算该列所有数值的平均值并返回（float）
#   若列名不存在，抛出 KeyError；若该列无有效数值，返回 0.0
#   例如：
#     csv_text = "name,age,score\n张三,25,85\n李四,30,90\n王五,28,78"
#     csv_column_avg(csv_text, "score")   # 84.33...
#     csv_column_avg(csv_text, "age")     # 27.666...
#     csv_column_avg(csv_text, "name")    # 0.0（无有效数值）
#     csv_column_avg(csv_text, "height")  # KeyError: 'height'
#
# 练习 3（正则）：
#   实现函数 extract_links(html)：
#   从 HTML 文本中提取所有超链接地址（href="..." 的值），返回去重后的有序列表
#   例如：
#     html = '<a href="https://python.org">官网</a><a href="http://example.com">示例</a>'
#     extract_links(html)
#     # ['http://example.com', 'https://python.org']
#
# 练习 4（综合）：
#   实现函数 extract_contacts(text)：
#   从文本中提取所有手机号（1[3-9]\d{9}）和邮箱，去重后以字典返回
#   例如：
#     text = "联系张三：13812345678，邮箱：a@b.com\n备用：13812345678，support@b.com"
#     extract_contacts(text)
#     # {'phones': ['13812345678'], 'emails': ['a@b.com', 'support@b.com']}
#
# 练习答案提示：
#   练习1：用 json.loads 解析，捕获 json.JSONDecodeError 转为 ValueError；
#          遍历顶层 dict，若键为 "age" 且值为 int/float，则加 1；最后 json.dumps
#   练习2：用 StringIO + csv.DictReader 读取；检查列名是否在 fieldnames 中；
#          遍历行，用 try/except 将值转为 float，累加后求平均
#   练习3：用 re.compile(r'href="([^"]+)"') 匹配所有链接；用 list(sorted(set(...))) 去重排序
#   练习4：用 re.findall 分别提取手机号和邮箱；用 sorted(set(...)) 去重


# =============================================================================
# 【练习答案】
# =============================================================================


def increment_ages(json_str: str) -> str:
    """
    练习 1：将 JSON 字符串中顶层所有 "age" 字段的值加 1。

    Args:
        json_str: 合法的 JSON 字符串

    Returns:
        修改后的 JSON 字符串（ensure_ascii=False，indent=2）

    Raises:
        ValueError: 若输入不是合法的 JSON 字符串

    Example:
        increment_ages('{"name": "张三", "age": 25}')
        # '{\\n  "name": "张三",\\n  "age": 26\\n}'

        increment_ages('{"items": [{"age": 10}]}')
        # items 内的 age 不受影响（只处理顶层）
    """
    try:
        data = json.loads(json_str)
    except json.JSONDecodeError as e:
        raise ValueError(f"无效的 JSON 字符串: {e}") from e

    if isinstance(data, dict):
        for key, value in data.items():
            if key == "age" and isinstance(value, (int, float)):
                data[key] = value + 1

    return json.dumps(data, indent=2, ensure_ascii=False)


def csv_column_avg(csv_text: str, column: str) -> float:
    """
    练习 2：计算 CSV 文本中指定列所有数值的平均值。

    Args:
        csv_text: CSV 格式的文本字符串（第一行为表头）
        column:   要计算平均值的列名

    Returns:
        该列所有有效数值的平均值（float），无有效数值时返回 0.0

    Raises:
        KeyError: 若列名不在表头中

    Example:
        csv_text = "name,age,score\\n张三,25,85\\n李四,30,90\\n王五,28,78"
        csv_column_avg(csv_text, "score")  # 84.333...
        csv_column_avg(csv_text, "name")   # 0.0
        csv_column_avg(csv_text, "height") # KeyError: 'height'
    """
    reader = csv.DictReader(StringIO(csv_text))

    # 检查列名是否存在
    if reader.fieldnames is None or column not in reader.fieldnames:
        raise KeyError(column)

    values: List[float] = []
    for row in reader:
        try:
            values.append(float(row[column]))
        except (ValueError, TypeError):
            pass  # 跳过无法转换的值

    return sum(values) / len(values) if values else 0.0


def extract_links(html: str) -> List[str]:
    """
    练习 3：从 HTML 文本中提取所有超链接地址，返回去重后的有序列表。

    Args:
        html: 包含 <a href="..."> 标签的 HTML 字符串

    Returns:
        所有 href 属性值组成的去重有序列表

    Example:
        html = '<a href="https://python.org">官网</a><a href="http://example.com">示例</a>'
        extract_links(html)
        # ['http://example.com', 'https://python.org']
    """
    href_pattern = re.compile(r'href="([^"]+)"')
    links = href_pattern.findall(html)
    return list(sorted(set(links)))


def extract_contacts(text: str) -> Dict[str, List[str]]:
    """
    练习 4：从文本中提取所有手机号和邮箱，去重后以字典返回。

    Args:
        text: 包含联系方式的文本字符串

    Returns:
        字典 {'phones': [...], 'emails': [...]}，两个列表均已去重并排序

    Example:
        text = "联系张三：13812345678，邮箱：a@b.com\\n备用：13812345678，support@b.com"
        extract_contacts(text)
        # {'phones': ['13812345678'], 'emails': ['a@b.com', 'support@b.com']}
    """
    phone_pattern = r'1[3-9]\d{9}'
    email_pattern = r'[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}'

    phones = sorted(set(re.findall(phone_pattern, text)))
    emails = sorted(set(re.findall(email_pattern, text)))

    return {'phones': phones, 'emails': emails}


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 increment_ages 函数。"""
    print("increment_ages 演示:")

    # ── 包含 age 字段 ────────────────────────────────────
    json_in = '{"name": "张三", "age": 25, "city": "北京"}'
    result = increment_ages(json_in)
    print(f"  输入: {json_in}")
    print(f"  输出:\n{result}")

    # ── age 在嵌套结构中（不处理）────────────────────────
    json_in2 = '{"items": [{"age": 10}, {"age": 20}], "age": 5}'
    result2 = increment_ages(json_in2)
    print(f"\n  输入: {json_in2}")
    print(f"  输出（只处理顶层 age=5→6，嵌套不变）:\n{result2}")

    # ── 无 age 字段 ──────────────────────────────────────
    json_in3 = '{"x": 1, "y": 2}'
    result3 = increment_ages(json_in3)
    print(f"\n  输入（无 age）: {json_in3}")
    print(f"  输出: {result3.strip()}")

    # ── 非法 JSON ────────────────────────────────────────
    try:
        increment_ages("not json")
    except ValueError as e:
        print(f"\n  ✓ 非法 JSON 捕获: {e}")


def exercise2_answer() -> None:
    """练习 2：演示 csv_column_avg 函数。"""
    print("csv_column_avg 演示:")

    csv_text = "name,age,score\n张三,25,85\n李四,30,90\n王五,28,78"
    print(f"  CSV 数据:\n    {csv_text.replace(chr(10), chr(10) + '    ')}")

    # ── 数值列 ───────────────────────────────────────────
    avg_score = csv_column_avg(csv_text, "score")
    avg_age   = csv_column_avg(csv_text, "age")
    print(f"\n  score 列平均值: {avg_score:.2f}")
    print(f"  age   列平均值: {avg_age:.2f}")

    # ── 非数值列 ─────────────────────────────────────────
    avg_name = csv_column_avg(csv_text, "name")
    print(f"  name  列平均值: {avg_name}（无有效数值）")

    # ── 列名不存在 ────────────────────────────────────────
    try:
        csv_column_avg(csv_text, "height")
    except KeyError as e:
        print(f"\n  ✓ 列名不存在捕获: KeyError {e}")


def exercise3_answer() -> None:
    """练习 3：演示 extract_links 函数。"""
    print("extract_links 演示:")

    html = '''
    <a href="https://python.org">Python 官网</a>
    <a href="http://example.com/page">示例页面</a>
    <img src="photo.jpg" />
    <a href="https://docs.python.org/3/">文档</a>
    <a href="https://python.org">重复链接</a>
    '''

    links = extract_links(html)
    print(f"  找到 {len(links)} 个不重复链接（已排序）:")
    for link in links:
        print(f"    {link}")


def exercise4_answer() -> None:
    """练习 4：演示 extract_contacts 函数。"""
    print("extract_contacts 演示:")

    sample_text = (
        "联系张三：13812345678，邮箱：zhangsan@company.com\n"
        "联系李四：13987654321，邮箱：lisi@example.org\n"
        "备用联系：13812345678（同上），support@company.com\n"
    )

    print(f"  文本:\n    {sample_text.rstrip().replace(chr(10), chr(10) + '    ')}")
    contacts = extract_contacts(sample_text)
    print(f"\n  手机号（去重）: {contacts['phones']}")
    print(f"  邮箱（去重）:   {contacts['emails']}")


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
#
#     print("=" * 40)
#     exercise4_answer()
