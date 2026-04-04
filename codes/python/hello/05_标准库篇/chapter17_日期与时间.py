# =============================================================================
# 第 17 章：日期与时间（datetime、time）
# =============================================================================
#
# 【学习目标】
#   1. 掌握 datetime 模块：日期、时间、日期时间的创建与运算
#   2. 掌握 timedelta：时间差计算
#   3. 掌握时区处理（timezone）
#   4. 掌握 time 模块：时间戳、性能计时
#   5. 学会日期格式化与解析
#
# 【这些模块解决什么问题？】
#   datetime: 日期时间的表示、计算、格式化，是处理时间数据的核心
#   time:     获取系统时间戳、暂停程序执行、性能计时
#
# 【与 C/C++ 的对比】
#   C/C++:  time.h 提供基础时间函数，操作笨重，时区处理麻烦
#   Python: datetime 对象封装良好，支持直接加减运算，时区处理方便
#
# 【运行方式】
#   python chapter17_日期与时间.py
#
# =============================================================================

import datetime
import re
import time
import calendar
from typing import Optional


# =============================================================================
# 17.1 datetime.date：纯日期
# =============================================================================

def demo_date() -> None:
    """演示 datetime.date 的基本用法。"""
    print("=" * 60)
    print("17.1 datetime.date：纯日期")
    print("=" * 60)

    # ── 创建日期对象 ─────────────────────────────────────
    print("1. 创建日期对象:")

    # 指定年月日创建
    d1 = datetime.date(2024, 1, 15)
    print(f"  指定日期: {d1}")                 # 2024-01-15

    # 获取今天的日期
    today = datetime.date.today()
    print(f"  今天: {today}")

    # 从时间戳创建（Unix 时间戳：从1970-01-01 00:00:00 起的秒数）
    ts = 1705276800  # 2024-01-15 00:00:00 UTC
    d2 = datetime.date.fromtimestamp(ts)
    print(f"  从时间戳创建: {d2}")

    # 从 ISO 格式字符串创建
    d3 = datetime.date.fromisoformat('2024-03-20')
    print(f"  从 ISO 字符串创建: {d3}")
    print()

    # ── 访问日期的各个部分 ───────────────────────────────
    print("2. 访问日期属性:")
    d = datetime.date(2024, 7, 4)
    print(f"  日期: {d}")
    print(f"  年: {d.year}")           # 2024
    print(f"  月: {d.month}")          # 7
    print(f"  日: {d.day}")            # 4
    print(f"  星期几（0=周一）: {d.weekday()}")   # 3（周四）
    print(f"  星期几（1=周日）: {d.isoweekday()}")  # 4
    print(f"  ISO 格式: {d.isoformat()}")           # 2024-07-04
    # isocalendar 返回 (year, week_number, weekday)
    iso = d.isocalendar()
    print(f"  ISO 日历（年, 第几周, 星期几）: {iso}")
    print()

    # ── 日期格式化 ───────────────────────────────────────
    print("3. 日期格式化（strftime）:")
    d = datetime.date(2024, 1, 15)
    print(f"  %Y-%m-%d:           {d.strftime('%Y-%m-%d')}")   # 2024-01-15
    print(f"  %d/%m/%Y:           {d.strftime('%d/%m/%Y')}")   # 15/01/2024
    print(f"  %B %d, %Y:          {d.strftime('%B %d, %Y')}")  # January 15, 2024
    print(f"  %Y年%m月%d日:        {d.strftime('%Y年%m月%d日')}")
    print(f"  %A（英文星期名）:     {d.strftime('%A')}")         # Monday
    print()

    # ── 日期比较与运算 ───────────────────────────────────
    print("4. 日期比较:")
    d1 = datetime.date(2024, 1, 1)
    d2 = datetime.date(2024, 12, 31)
    print(f"  {d1} < {d2}: {d1 < d2}")
    print(f"  {d1} == {d2}: {d1 == d2}")

    # 日期相减得到 timedelta
    delta = d2 - d1
    print(f"  相差天数: {delta.days} 天")


# =============================================================================
# 17.2 datetime.time：纯时间
# =============================================================================

def demo_time_obj() -> None:
    """演示 datetime.time 的基本用法。"""
    print("\n" + "=" * 60)
    print("17.2 datetime.time：纯时间")
    print("=" * 60)

    # ── 创建时间对象 ─────────────────────────────────────
    print("1. 创建时间对象:")

    t1 = datetime.time(14, 30, 0)         # 14:30:00
    print(f"  基本时间: {t1}")

    t2 = datetime.time(9, 5, 30, 500000)  # 9:05:30.500000（带微秒）
    print(f"  带微秒: {t2}")

    t3 = datetime.time.fromisoformat('16:45:30')
    print(f"  从 ISO 字符串: {t3}")
    print()

    # ── 访问时间属性 ─────────────────────────────────────
    print("2. 访问时间属性:")
    t = datetime.time(14, 30, 45, 123456)
    print(f"  时间: {t}")
    print(f"  时: {t.hour}")        # 14
    print(f"  分: {t.minute}")      # 30
    print(f"  秒: {t.second}")      # 45
    print(f"  微秒: {t.microsecond}")  # 123456
    print()

    # ── 时间格式化 ───────────────────────────────────────
    print("3. 时间格式化（strftime）:")
    t = datetime.time(14, 30, 45)
    print(f"  %H:%M:%S:     {t.strftime('%H:%M:%S')}")   # 24小时制
    print(f"  %I:%M:%S %p:  {t.strftime('%I:%M:%S %p')}") # 12小时制
    print()

    # ── 时间比较 ─────────────────────────────────────────
    print("4. 时间比较:")
    t1 = datetime.time(8, 0, 0)
    t2 = datetime.time(18, 0, 0)
    print(f"  上班时间: {t1}")
    print(f"  下班时间: {t2}")
    print(f"  上班 < 下班: {t1 < t2}")


# =============================================================================
# 17.3 datetime.datetime：日期时间
# =============================================================================

def demo_datetime() -> None:
    """演示 datetime.datetime 的核心功能。"""
    print("\n" + "=" * 60)
    print("17.3 datetime.datetime：日期时间")
    print("=" * 60)

    # ── 创建 datetime 对象 ───────────────────────────────
    print("1. 创建 datetime 对象:")

    # 指定年月日时分秒
    dt1 = datetime.datetime(2024, 1, 15, 14, 30, 0)
    print(f"  指定时间: {dt1}")

    # 获取当前时间（本地时间，无时区信息）
    now = datetime.datetime.now()
    print(f"  当前时间: {now}")

    # 获取当前 UTC 时间（有时区信息）
    utc_now = datetime.datetime.now(datetime.timezone.utc)
    print(f"  当前 UTC 时间: {utc_now}")

    # 从时间戳创建
    dt2 = datetime.datetime.fromtimestamp(1705329000)
    print(f"  从时间戳: {dt2}")

    # 从 ISO 格式字符串创建
    dt3 = datetime.datetime.fromisoformat('2024-01-15T14:30:00')
    print(f"  从 ISO 字符串: {dt3}")
    print()

    # ── 访问各个部分 ─────────────────────────────────────
    print("2. 访问 datetime 属性:")
    dt = datetime.datetime(2024, 6, 15, 9, 30, 45, 500000)
    print(f"  完整时间: {dt}")
    print(f"  日期部分: {dt.date()}")
    print(f"  时间部分: {dt.time()}")
    print(f"  年={dt.year}, 月={dt.month}, 日={dt.day}")
    print(f"  时={dt.hour}, 分={dt.minute}, 秒={dt.second}")
    print(f"  微秒={dt.microsecond}")
    print()

    # ── 格式化与解析 ─────────────────────────────────────
    print("3. 格式化（strftime）与解析（strptime）:")

    dt = datetime.datetime(2024, 1, 15, 14, 30, 0)

    # 格式化输出
    fmt1 = dt.strftime('%Y-%m-%d %H:%M:%S')
    fmt2 = dt.strftime('%Y/%m/%d %I:%M %p')
    fmt3 = dt.strftime('%d %B %Y')
    print(f"  格式1: {fmt1}")   # 2024-01-15 14:30:00
    print(f"  格式2: {fmt2}")   # 2024/01/15 02:30 PM
    print(f"  格式3: {fmt3}")   # 15 January 2024
    print(f"  ISO格式: {dt.isoformat()}")  # 2024-01-15T14:30:00

    # 解析字符串（strptime = string parse time）
    time_str = '2024-03-20 09:15:30'
    parsed = datetime.datetime.strptime(time_str, '%Y-%m-%d %H:%M:%S')
    print(f"\n  解析字符串: '{time_str}'")
    print(f"  解析结果: {parsed}")
    print(f"  类型: {type(parsed)}")
    print()

    # ── 时间戳转换 ───────────────────────────────────────
    print("4. 时间戳转换:")
    dt = datetime.datetime(2024, 1, 15, 12, 0, 0)
    ts = dt.timestamp()  # 转为 Unix 时间戳（浮点数）
    print(f"  datetime → 时间戳: {dt} → {ts}")

    dt_back = datetime.datetime.fromtimestamp(ts)
    print(f"  时间戳 → datetime: {ts} → {dt_back}")

    # ── datetime 合并 ────────────────────────────────────
    print("\n5. 合并日期和时间:")
    d = datetime.date(2024, 6, 15)
    t = datetime.time(9, 30, 0)
    dt = datetime.datetime.combine(d, t)
    print(f"  date({d}) + time({t}) = datetime({dt})")


# =============================================================================
# 17.4 datetime.timedelta：时间差
# =============================================================================

def demo_timedelta() -> None:
    """演示 timedelta 时间差计算。"""
    print("\n" + "=" * 60)
    print("17.4 datetime.timedelta：时间差计算")
    print("=" * 60)

    # ── 创建时间差 ───────────────────────────────────────
    print("1. 创建 timedelta:")

    delta1 = datetime.timedelta(days=7)          # 7天
    delta2 = datetime.timedelta(hours=2, minutes=30)  # 2小时30分
    delta3 = datetime.timedelta(days=1, hours=12, minutes=30, seconds=45)
    delta4 = datetime.timedelta(weeks=2)         # 2周

    print(f"  7天: {delta1}")
    print(f"  2小时30分: {delta2}")
    print(f"  1天12时30分45秒: {delta3}")
    print(f"  2周: {delta4}")
    print()

    # ── 时间加减运算 ─────────────────────────────────────
    print("2. 时间加减运算:")

    now = datetime.datetime(2024, 1, 15, 12, 0, 0)
    print(f"  当前时间: {now}")

    # 加法
    tomorrow = now + datetime.timedelta(days=1)
    next_week = now + datetime.timedelta(weeks=1)
    in_2h = now + datetime.timedelta(hours=2)
    print(f"  +1天: {tomorrow}")
    print(f"  +1周: {next_week}")
    print(f"  +2小时: {in_2h}")

    # 减法
    yesterday = now - datetime.timedelta(days=1)
    last_month = now - datetime.timedelta(days=30)
    print(f"  -1天: {yesterday}")
    print(f"  -30天: {last_month}")
    print()

    # ── timedelta 的属性 ─────────────────────────────────
    print("3. timedelta 属性:")
    delta = datetime.timedelta(days=2, hours=3, minutes=30, seconds=15)
    print(f"  timedelta: {delta}")
    print(f"  总天数: {delta.days}")           # 只有天数部分
    print(f"  剩余秒数: {delta.seconds}")      # 天数之外的秒数
    print(f"  总秒数: {delta.total_seconds()}")  # 全部转为秒
    print()

    # ── 实际应用 ─────────────────────────────────────────
    print("4. 实际应用：")

    # 计算距离某个日期还有多少天
    new_year = datetime.date(2025, 1, 1)
    today = datetime.date(2024, 1, 15)
    days_left = (new_year - today).days
    print(f"  距离2025年新年还有 {days_left} 天")

    # 判断是否超时（token 是否过期）
    def is_expired(created_at: datetime.datetime, ttl_hours: int) -> bool:
        """检查令牌是否过期（TTL = Time To Live）"""
        expiry = created_at + datetime.timedelta(hours=ttl_hours)
        return datetime.datetime.now() > expiry

    token_created = datetime.datetime(2024, 1, 15, 10, 0, 0)
    print(f"\n  令牌创建时间: {token_created}")
    print(f"  令牌已过期（24h TTL）: {is_expired(token_created, 24)}")

    # 生成日期范围
    print("\n  生成未来7天日期列表:")
    start = datetime.date(2024, 1, 15)
    for i in range(7):
        day = start + datetime.timedelta(days=i)
        week_name = ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
        print(f"    {day} {week_name[day.weekday()]}")


# =============================================================================
# 17.5 时区处理
# =============================================================================

def demo_timezone() -> None:
    """演示时区处理。"""
    print("\n" + "=" * 60)
    print("17.5 时区处理（timezone）")
    print("=" * 60)

    # ── 创建有时区信息的 datetime ────────────────────────
    print("1. 创建带时区的 datetime:")

    # UTC 时区
    utc = datetime.timezone.utc
    dt_utc = datetime.datetime(2024, 1, 15, 12, 0, 0, tzinfo=utc)
    print(f"  UTC 时间: {dt_utc}")

    # 自定义时区偏移（东八区 = UTC+8）
    tz_cn = datetime.timezone(datetime.timedelta(hours=8), name='CST')
    dt_cn = datetime.datetime(2024, 1, 15, 20, 0, 0, tzinfo=tz_cn)
    print(f"  北京时间（CST）: {dt_cn}")

    # 纽约时区（UTC-5，冬令时）
    tz_ny = datetime.timezone(datetime.timedelta(hours=-5), name='EST')
    dt_ny = datetime.datetime(2024, 1, 15, 7, 0, 0, tzinfo=tz_ny)
    print(f"  纽约时间（EST）: {dt_ny}")
    print()

    # ── 时区转换 ─────────────────────────────────────────
    print("2. 时区转换（astimezone）:")

    # 将 UTC 时间转换为北京时间
    utc_time = datetime.datetime(2024, 1, 15, 12, 0, 0, tzinfo=datetime.timezone.utc)
    tz_beijing = datetime.timezone(datetime.timedelta(hours=8))
    beijing_time = utc_time.astimezone(tz_beijing)
    print(f"  UTC 12:00 → 北京时间: {beijing_time}")

    # 将北京时间转换为 UTC
    bj = datetime.datetime(2024, 1, 15, 20, 0, 0,
                           tzinfo=datetime.timezone(datetime.timedelta(hours=8)))
    utc = bj.astimezone(datetime.timezone.utc)
    print(f"  北京时间 20:00 → UTC: {utc}")
    print()

    # ── naive 与 aware ───────────────────────────────────
    print("3. naive（无时区）与 aware（有时区）:")

    naive_dt = datetime.datetime(2024, 1, 15, 12, 0, 0)
    aware_dt = datetime.datetime(2024, 1, 15, 12, 0, 0,
                                 tzinfo=datetime.timezone.utc)

    print(f"  naive datetime: {naive_dt}  (tzinfo={naive_dt.tzinfo})")
    print(f"  aware datetime: {aware_dt}  (tzinfo={aware_dt.tzinfo})")
    print("  ⚠️  naive 和 aware 不能直接比较或运算！")

    # 给 naive datetime 添加时区信息
    tz = datetime.timezone(datetime.timedelta(hours=8))
    aware_from_naive = naive_dt.replace(tzinfo=tz)
    print(f"\n  给 naive 添加时区: {aware_from_naive}")


# =============================================================================
# 17.6 time 模块：时间戳与计时
# =============================================================================

def demo_time_module() -> None:
    """演示 time 模块的常用功能。"""
    print("\n" + "=" * 60)
    print("17.6 time 模块：时间戳与计时")
    print("=" * 60)

    # ── 时间戳 ───────────────────────────────────────────
    print("1. 时间戳（time.time）:")
    ts = time.time()
    print(f"  当前时间戳: {ts}")
    print(f"  类型: {type(ts).__name__}（浮点数，单位：秒）")
    print(f"  对应时间: {datetime.datetime.fromtimestamp(ts)}")
    print()

    # ── 暂停执行 ─────────────────────────────────────────
    print("2. 暂停执行（time.sleep）:")
    print("  开始暂停 0.1 秒...")
    start = time.time()
    time.sleep(0.1)  # 暂停 0.1 秒
    elapsed = time.time() - start
    print(f"  暂停结束，实际耗时: {elapsed:.4f} 秒")
    print()

    # ── 高精度计时 ───────────────────────────────────────
    print("3. 高精度计时（time.perf_counter）:")
    print("  time.perf_counter() 是测量代码性能的最佳选择")

    start = time.perf_counter()

    # 模拟耗时操作
    total = sum(range(1_000_000))

    elapsed = time.perf_counter() - start
    print(f"  计算 sum(range(1_000_000)) = {total}")
    print(f"  耗时: {elapsed*1000:.2f} 毫秒")
    print()

    # ── time.struct_time ─────────────────────────────────
    print("4. struct_time（结构化时间）:")

    local_time = time.localtime()  # 当前本地时间（struct_time）
    print(f"  localtime(): {local_time}")
    print(f"  年: {local_time.tm_year}")
    print(f"  月: {local_time.tm_mon}")
    print(f"  日: {local_time.tm_mday}")
    print(f"  时: {local_time.tm_hour}")
    print(f"  分: {local_time.tm_min}")
    print(f"  秒: {local_time.tm_sec}")
    print(f"  星期几（0=周一）: {local_time.tm_wday}")
    print(f"  当年第几天: {local_time.tm_yday}")
    print()

    # ── time.strftime ────────────────────────────────────
    print("5. time.strftime（格式化时间）:")
    formatted = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
    print(f"  当前本地时间: {formatted}")


# =============================================================================
# 17.7 calendar 模块：日历
# =============================================================================

def demo_calendar() -> None:
    """演示 calendar 模块。"""
    print("\n" + "=" * 60)
    print("17.7 calendar 模块：日历工具")
    print("=" * 60)

    print("1. 判断闰年（calendar.isleap）:")
    years = [2000, 1900, 2024, 2023]
    for y in years:
        print(f"  {y} 年是闰年: {calendar.isleap(y)}")
    print()

    print("2. 获取某月天数（calendar.monthrange）:")
    # monthrange 返回 (该月第一天是星期几, 该月总天数)
    year, month = 2024, 2
    first_weekday, num_days = calendar.monthrange(year, month)
    week_names = ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
    print(f"  {year}年{month}月:")
    print(f"    第一天是: {week_names[first_weekday]}")
    print(f"    共 {num_days} 天")
    print()

    print("3. 某月日历（文本形式）:")
    cal_text = calendar.month(2024, 2)
    print(cal_text)


# =============================================================================
# 17.8 综合实战：工作日计算器
# =============================================================================

def demo_workday_calculator() -> None:
    """演示综合实战：工作日计算器。"""
    print("\n" + "=" * 60)
    print("17.8 综合实战：工作日计算器")
    print("=" * 60)

    def is_workday(d: datetime.date) -> bool:
        """判断是否为工作日（简化版，不考虑法定节假日）"""
        return d.weekday() < 5  # 0-4 = 周一至周五

    def add_workdays(start: datetime.date, workdays: int) -> datetime.date:
        """从 start 开始，加上 workdays 个工作日，返回目标日期。"""
        current = start
        added = 0
        while added < workdays:
            current += datetime.timedelta(days=1)
            if is_workday(current):
                added += 1
        return current

    def count_workdays(start: datetime.date, end: datetime.date) -> int:
        """计算两个日期之间（含两端）的工作日数量。"""
        count = 0
        current = start
        while current <= end:
            if is_workday(current):
                count += 1
            current += datetime.timedelta(days=1)
        return count

    def format_date_cn(d: datetime.date) -> str:
        """格式化日期为中文形式。"""
        week_names = ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
        return f"{d.strftime('%Y年%m月%d日')} ({week_names[d.weekday()]})"

    # ── 演示 ─────────────────────────────────────────────
    start_date = datetime.date(2024, 1, 15)  # 周一
    print(f"起始日期: {format_date_cn(start_date)}")
    print()

    # 加工作日
    for n in [1, 5, 10, 20]:
        result = add_workdays(start_date, n)
        print(f"  加 {n:2d} 个工作日: {format_date_cn(result)}")
    print()

    # 统计工作日
    period_start = datetime.date(2024, 1, 1)
    period_end = datetime.date(2024, 1, 31)
    workdays = count_workdays(period_start, period_end)
    print(f"2024年1月共有 {workdays} 个工作日")
    print()

    # ── 项目截止日期计算器 ───────────────────────────────
    print("项目截止日期计算器:")
    projects = [
        ("用户界面模块", datetime.date(2024, 1, 15), 10),
        ("后端 API",    datetime.date(2024, 1, 20), 15),
        ("数据库优化",  datetime.date(2024, 2, 1),   5),
    ]

    print(f"  {'项目名称':<15} {'开始日期':<12} {'工期(工作日)':<12} {'截止日期'}")
    print("  " + "-" * 65)
    for name, start, duration in projects:
        deadline = add_workdays(start, duration)
        week_names = ['周一', '周二', '周三', '周四', '周五', '周六', '周日']
        print(f"  {name:<15} {start.isoformat():<12} {duration:<12} "
              f"{deadline.isoformat()} ({week_names[deadline.weekday()]})")


# =============================================================================
# 17.9 综合实战：计时装饰器
# =============================================================================

def demo_timer_decorator() -> None:
    """演示计时装饰器：记录函数执行时间。"""
    print("\n" + "=" * 60)
    print("17.9 综合实战：计时装饰器")
    print("=" * 60)

    import functools

    def timer(func):
        """装饰器：记录函数的执行时间。"""
        @functools.wraps(func)
        def wrapper(*args, **kwargs):
            start = time.perf_counter()
            start_time = datetime.datetime.now().strftime('%H:%M:%S.%f')[:-3]
            print(f"  [{start_time}] 开始执行 {func.__name__}()")

            result = func(*args, **kwargs)

            elapsed = time.perf_counter() - start
            end_time = datetime.datetime.now().strftime('%H:%M:%S.%f')[:-3]
            print(f"  [{end_time}] 执行完毕 {func.__name__}()，"
                  f"耗时 {elapsed*1000:.2f} ms")
            return result
        return wrapper

    @timer
    def compute_primes(limit: int) -> list[int]:
        """计算 limit 以内的所有质数（埃拉托色尼筛法）。"""
        sieve = [True] * (limit + 1)
        sieve[0] = sieve[1] = False
        for i in range(2, int(limit**0.5) + 1):
            if sieve[i]:
                for j in range(i*i, limit + 1, i):
                    sieve[j] = False
        return [i for i, is_prime in enumerate(sieve) if is_prime]

    @timer
    def sort_data(n: int) -> list[int]:
        """生成随机数据并排序。"""
        import random
        data = [random.randint(0, 10000) for _ in range(n)]
        data.sort()
        return data

    print("测试计时装饰器:")
    primes = compute_primes(100_000)
    print(f"  10万以内共有 {len(primes)} 个质数\n")

    sorted_data = sort_data(50_000)
    print(f"  排序 5万个数完成，最小={sorted_data[0]}，最大={sorted_data[-1]}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """主程序：运行所有演示。"""
    demo_date()
    demo_time_obj()
    demo_datetime()
    demo_timedelta()
    demo_timezone()
    demo_time_module()
    demo_calendar()
    demo_workday_calculator()
    demo_timer_decorator()


if __name__ == "__main__":
    main()


# =============================================================================
# 【strftime / strptime 格式码速查表】
# =============================================================================
#
# %Y  4位年份（2024）         %y  2位年份（24）
# %m  2位月份（01-12）        %B  月份全称（January）
# %b  月份缩写（Jan）         %d  2位日期（01-31）
# %H  24小时制（00-23）       %I  12小时制（01-12）
# %M  分钟（00-59）           %S  秒（00-59）
# %f  微秒（000000-999999）   %p  AM 或 PM
# %A  星期全称（Monday）      %a  星期缩写（Mon）
# %j  当年第几天（001-366）   %W  当年第几周
# %Z  时区名称               %z  UTC 偏移（+0800）
# %%  字面 % 符号


# =============================================================================
# 【datetime 类体系】
# =============================================================================
#
# datetime.date         只有日期（年月日）
# datetime.time         只有时间（时分秒微秒）
# datetime.datetime     日期 + 时间（继承自 date）
# datetime.timedelta    时间差（天、秒、微秒）
# datetime.timezone     时区信息（UTC 偏移）
# datetime.tzinfo       时区抽象基类


# =============================================================================
# 【time 模块常用函数】
# =============================================================================
#
# time.time()            返回当前 Unix 时间戳（float，秒）
# time.sleep(secs)       暂停执行 secs 秒
# time.perf_counter()    高精度计时器（用于性能测量）
# time.monotonic()       单调时钟（不受系统时间调整影响）
# time.localtime([t])    时间戳转为本地 struct_time
# time.gmtime([t])       时间戳转为 UTC struct_time
# time.mktime(t)         struct_time 转为时间戳
# time.strftime(fmt, t)  struct_time 格式化为字符串


# =============================================================================
# 【常见错误】
# =============================================================================
#
# 错误 1：strptime 格式与字符串不匹配
# datetime.datetime.strptime('2024/01/15', '%Y-%m-%d')  # ❌ / 与 - 不匹配
# datetime.datetime.strptime('2024/01/15', '%Y/%m/%d')  # ✓
#
# 错误 2：naive 与 aware datetime 混合运算
# naive = datetime.datetime.now()
# aware = datetime.datetime.now(datetime.timezone.utc)
# diff = aware - naive  # ❌ TypeError: can't subtract offset-naive...
#
# 错误 3：timedelta 不支持月份和年份
# delta = datetime.timedelta(months=1)  # ❌ 没有 months 参数！
# 解决：用 dateutil 库（第三方）或手动计算
#
# 错误 4：time.sleep 参数单位是秒（不是毫秒）
# time.sleep(1000)   # ❌ 暂停1000秒！
# time.sleep(1)      # ✓ 暂停1秒
# time.sleep(0.001)  # ✓ 暂停1毫秒
#
# 错误 5：timestamp() 受本地时区影响
# dt_naive = datetime.datetime(2024, 1, 15, 12, 0, 0)
# ts = dt_naive.timestamp()  # 结果取决于系统时区！
# 解决：使用 aware datetime（带 tzinfo）确保时区明确


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   实现函数 days_since_birthday(birth_date)：
#   接受一个 datetime.date 对象，计算从生日到今天已经过了多少天，返回整数
#   例如：
#     # 若今天是 2026-04-04
#     days_since_birthday(datetime.date(2000, 4, 4))   # 9497
#     days_since_birthday(datetime.date(1990, 1, 15))  # 13229（取决于今天的日期）
#
# 练习 2（进阶）：
#   实现函数 get_quarter(d)：
#   接受一个 datetime.date 对象，返回该日期所在的季度（整数 1-4）
#   例如：
#     get_quarter(datetime.date(2024, 1, 31))   # 1（第一季度：1-3月）
#     get_quarter(datetime.date(2024, 5, 15))   # 2（第二季度：4-6月）
#     get_quarter(datetime.date(2024, 8, 20))   # 3（第三季度：7-9月）
#     get_quarter(datetime.date(2024, 11, 30))  # 4（第四季度：10-12月）
#
# 练习 3（综合）：
#   实现函数 parse_log_timestamps(log_lines)：
#   接受日志行列表，每行格式为 "[2024-01-15 14:30:00] 消息内容"
#   返回按时间升序排序的 (datetime, 消息) 元组列表，无法解析的行跳过
#   例如：
#     logs = [
#         "[2024-01-15 14:30:00] 用户登录",
#         "[2024-01-15 09:15:00] 系统启动",
#         "无效行，跳过",
#     ]
#     parse_log_timestamps(logs)
#     # [(datetime(2024,1,15,9,15,0), '系统启动'),
#     #  (datetime(2024,1,15,14,30,0), '用户登录')]
#
# 练习 4（性能计时）：
#   实现函数 compare_sum_methods(n)：
#   用 time.perf_counter 分别计时两种求 1 到 n 之和的方法：
#     方法 A：sum(range(n))
#     方法 B：n * (n - 1) // 2
#   返回字典 {'time_a': float, 'time_b': float, 'speedup': float}
#   speedup = time_a / time_b（方法 B 比方法 A 快多少倍）
#   例如：
#     result = compare_sum_methods(10_000_000)
#     result['speedup']  # 远大于 1（数值因机器而异）
#
# 练习答案提示：
#   练习1：用 datetime.date.today() 获取今天；两个 date 相减得 timedelta；取 .days 属性
#   练习2：用 (d.month - 1) // 3 + 1 整除计算季度
#   练习3：用 re.match 匹配 r'\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\] (.+)'；
#          用 datetime.datetime.strptime 解析时间字符串；用 sorted 按元组第一个元素排序
#   练习4：调用 time.perf_counter() 在操作前后各取一次，相减得耗时；
#          time_a / time_b 即为加速比


# =============================================================================
# 【练习答案】
# =============================================================================


def days_since_birthday(birth_date: datetime.date) -> int:
    """
    练习 1：计算从生日到今天已经过了多少天。

    Args:
        birth_date: 生日日期

    Returns:
        从生日到今天经过的天数（整数）

    Example:
        # 若今天是 2026-04-04
        days_since_birthday(datetime.date(2000, 4, 4))   # 9497
        days_since_birthday(datetime.date(1990, 1, 15))  # 13229
    """
    return (datetime.date.today() - birth_date).days


def get_quarter(d: datetime.date) -> int:
    """
    练习 2：返回日期所在的季度（1-4）。

    Args:
        d: 要判断季度的日期

    Returns:
        季度数字（1=第一季度，2=第二季度，3=第三季度，4=第四季度）

    Example:
        get_quarter(datetime.date(2024, 1, 31))   # 1
        get_quarter(datetime.date(2024, 5, 15))   # 2
        get_quarter(datetime.date(2024, 8, 20))   # 3
        get_quarter(datetime.date(2024, 11, 30))  # 4
    """
    return (d.month - 1) // 3 + 1


def parse_log_timestamps(
    log_lines: list[str],
) -> list[tuple[datetime.datetime, str]]:
    """
    练习 3：解析日志行，返回按时间升序排序的 (datetime, 消息) 元组列表。

    Args:
        log_lines: 日志行列表，每行格式为 "[2024-01-15 14:30:00] 消息内容"

    Returns:
        按时间升序排序的 (datetime, 消息) 元组列表，无法解析的行跳过

    Example:
        logs = [
            "[2024-01-15 14:30:00] 用户登录",
            "[2024-01-15 09:15:00] 系统启动",
        ]
        parse_log_timestamps(logs)
        # [(datetime(2024,1,15,9,15,0), '系统启动'),
        #  (datetime(2024,1,15,14,30,0), '用户登录')]
    """
    results = []
    pattern = r'\[(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})\] (.+)'
    for line in log_lines:
        match = re.match(pattern, line)
        if match:
            dt = datetime.datetime.strptime(match.group(1), '%Y-%m-%d %H:%M:%S')
            results.append((dt, match.group(2)))
    return sorted(results, key=lambda x: x[0])


def compare_sum_methods(n: int) -> dict:
    """
    练习 4：比较两种求和方法的耗时，返回各自耗时及加速比。

    Args:
        n: 求和上限（计算 0 到 n-1 的累加之和，即 sum(range(n))）

    Returns:
        字典 {'time_a': float, 'time_b': float, 'speedup': float}
        time_a:  方法 A（sum(range(n))）的耗时（秒）
        time_b:  方法 B（n*(n-1)//2）的耗时（秒）
        speedup: 方法 B 比方法 A 快的倍数

    Example:
        result = compare_sum_methods(10_000_000)
        result['speedup']  # 远大于 1（数值因机器而异）
    """
    start = time.perf_counter()
    sum(range(n))
    time_a = time.perf_counter() - start

    start = time.perf_counter()
    n * (n - 1) // 2
    time_b = time.perf_counter() - start

    return {
        'time_a': time_a,
        'time_b': time_b,
        'speedup': time_a / time_b if time_b > 0 else float('inf'),
    }


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 days_since_birthday 函数。"""
    print("days_since_birthday 演示:")

    # ── 多个典型生日 ─────────────────────────────────────
    test_dates = [
        datetime.date(2000, 1, 1),
        datetime.date(1990, 6, 15),
        datetime.date(2010, 12, 31),
    ]

    for birth in test_dates:
        days = days_since_birthday(birth)
        years = days // 365
        print(f"  生日 {birth} → 已过 {days} 天（约 {years} 岁）")

    # ── 今天自身 ─────────────────────────────────────────
    today = datetime.date.today()
    print(f"\n  今天的日期: {today}")
    print(f"  距今天自身: {days_since_birthday(today)} 天")


def exercise2_answer() -> None:
    """练习 2：演示 get_quarter 函数。"""
    print("get_quarter 演示:")

    # ── 覆盖全年12个月 ───────────────────────────────────
    test_dates = [
        datetime.date(2024, 1, 1),    # Q1
        datetime.date(2024, 3, 31),   # Q1
        datetime.date(2024, 4, 1),    # Q2
        datetime.date(2024, 5, 15),   # Q2
        datetime.date(2024, 7, 4),    # Q3
        datetime.date(2024, 8, 20),   # Q3
        datetime.date(2024, 10, 1),   # Q4
        datetime.date(2024, 11, 30),  # Q4
        datetime.date(2024, 12, 31),  # Q4
    ]

    for d in test_dates:
        q = get_quarter(d)
        print(f"  {d} → 第 {q} 季度")


def exercise3_answer() -> None:
    """练习 3：演示 parse_log_timestamps 函数。"""
    print("parse_log_timestamps 演示:")

    # ── 混合有效行与无效行 ───────────────────────────────
    logs = [
        "[2024-01-15 14:30:00] 用户登录",
        "[2024-01-15 09:15:00] 系统启动",
        "[2024-01-15 11:45:30] 数据库备份完成",
        "[2024-01-15 14:30:05] 订单创建",
        "这是无效行，应该被跳过",
        "[格式不对] 也跳过",
    ]

    print(f"  输入 {len(logs)} 行日志（含 2 行无效）")
    parsed = parse_log_timestamps(logs)
    print(f"  成功解析 {len(parsed)} 行，已按时间升序排序:")
    for dt, msg in parsed:
        print(f"    {dt.strftime('%H:%M:%S')} - {msg}")


def exercise4_answer() -> None:
    """练习 4：演示 compare_sum_methods 函数。"""
    print("compare_sum_methods 演示:")

    # ── 性能对比 ─────────────────────────────────────────
    N = 10_000_000
    result = compare_sum_methods(N)
    print(f"  求 sum(range({N:,}))：")
    print(f"  方法 A（sum(range(n))）: {result['time_a']*1000:.2f} ms")
    print(f"  方法 B（n*(n-1)//2）:   {result['time_b']*1000:.4f} ms")
    print(f"  方法 B 比方法 A 快约 {result['speedup']:.0f} 倍")


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
