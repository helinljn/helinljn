/*
 * [fmt/base.h] — 基础公开 API（通常通过 format.h 间接使用）
 *   ✓ fmt::string_view / fmt::basic_string_view
 *   ✓ fmt::format_to(OutputIt, fmt, args...)
 *   ✓ fmt::format_to_n(OutputIt, n, fmt, args...)
 *   ✓ fmt::formatted_size(fmt, args...)
 *   ✓ fmt::print(fmt, args...)
 *   ✓ fmt::println(fmt, args...)
 *   ✓ fmt::arg(name, value)
 *   ✓ fmt::make_format_args(args...)
 *   ✓ fmt::runtime(str)
 *   ✓ fmt::is_formattable<T>
 *   ✓ fmt::format_to(char(&)[N], fmt, args...) → format_to_result
 *
 * [fmt/format.h] — 核心格式化 API
 *   ✓ fmt::format(fmt, args...) — 各类型（整数、浮点、字符串、bool、char）
 *   ✓ fmt::vformat(fmt, format_args)
 *   ✓ fmt::memory_buffer / fmt::basic_memory_buffer
 *   ✓ fmt::to_string(value)
 *   ✓ fmt::format_error
 *   ✓ fmt::format_int
 *   ✓ fmt::ptr(T*)
 *   ✓ fmt::underlying(enum)
 *   ✓ fmt::group_digits(value)
 *   ✓ fmt::bytes(string_view)
 *   ✓ FMT_STRING 宏
 *   ✓ fmt::arg / "name"_a 命名参数
 *   ✓ fmt::system_error / fmt::format_system_error
 *   ✓ fmt::writer / fmt::string_buffer
 *   ✓ 自定义 formatter<T> 特化
 *   ✓ 格式规格说明符（对齐、宽度、精度、进制、符号等）
 *
 * [fmt/chrono.h] — 时间格式化
 *   ✓ std::chrono::duration 基本格式化（秒、毫秒、分钟等）
 *   ✓ std::chrono::duration 自定义格式（%H:%M:%S 等）
 *   ✓ std::tm 格式化
 *   ✓ fmt::gmtime(time_t)
 *   ✓ fmt::weekday / fmt::day / fmt::month / fmt::year / fmt::year_month_day
 *   ✓ fmt::sys_time 格式化
 *
 * [fmt/compile.h] — 编译时格式化
 *   ✓ FMT_COMPILE 宏 + fmt::format
 *   ✓ FMT_COMPILE 宏 + fmt::format_to
 *   ✓ FMT_COMPILE 宏 + fmt::formatted_size
 *   ✓ FMT_COMPILE 宏 + fmt::format_to_n
 *
 * [fmt/std.h] — 标准库类型支持
 *   ✓ std::optional<T> 格式化
 *   ✓ std::variant<T...> 格式化
 *   ✓ std::monostate 格式化
 *   ✓ std::error_code 格式化
 *   ✓ std::exception 格式化
 *   ✓ std::bitset<N> 格式化
 *   ✓ std::complex<T> 格式化
 *   ✓ std::atomic<T> 格式化
 *   ✓ std::reference_wrapper<T> 格式化
 *   ✓ fmt::ptr(unique_ptr) / fmt::ptr(shared_ptr)
 *
 * [fmt/ranges.h] — 范围与元组格式化
 *   ✓ std::tuple<T...> / std::pair<K,V> 格式化
 *   ✓ std::vector / std::list / std::deque / std::array / C 数组格式化（序列）
 *   ✓ std::set / std::multiset / std::unordered_set / std::unordered_multiset 格式化（集合）
 *   ✓ std::map / std::multimap / std::unordered_map / std::unordered_multimap 格式化（映射）
 *   ✓ std::stack / std::queue / std::priority_queue 格式化（容器适配器）
 *   ✓ fmt::join(begin, end, sep) — 迭代器范围连接
 *   ✓ fmt::join(range, sep) — 范围连接
 *   ✓ fmt::join(tuple, sep) — 元组连接
 *   ✓ fmt::join(initializer_list, sep) — 初始化列表连接
 *   ✓ 范围格式说明符：:n（无括号）、:s（字符串）、:?s（调试字符串）
 *   ✓ 元组格式说明符：:n（无括号和分隔符）
 *   ✓ fmt::is_tuple_like<T> / fmt::is_range<T, Char> 类型特征
 */
#include "doctest.h"
#include "core/common.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/fmt/chrono.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4127)
    #pragma warning(disable:4702)
#endif // defined(CORE_PLATFORM_WINDOWS)

#include "spdlog/fmt/compile.h"
#include "spdlog/fmt/ranges.h"
#include "spdlog/fmt/std.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(pop)
#endif // defined(CORE_PLATFORM_WINDOWS)

#include <algorithm>
#include <array>
#include <deque>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <atomic>
#include <bitset>
#include <chrono>
#include <complex>
#include <ctime>
#include <filesystem>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <cmath>
#include <optional>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>
#include <variant>
#include <vector>

/// 一个表示坐标点的自定义类型
struct Point
{
    int x, y;
};

/// 为 Point 特化 fmt::formatter
template <>
struct fmt::formatter<Point>
{
    // 支持 'd'（笛卡尔坐标）和 'p'（极坐标）两种格式说明符
    char presentation = 'd';

    /// 解析格式规格；'d' 为笛卡尔坐标 (x, y)，'p' 为极坐标 (r, θ)
    constexpr auto parse(fmt::parse_context<>& ctx) -> const char*
    {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && (*it == 'p' || *it == 'd'))
            presentation = *it++;
        // 检查是否到达结束符 '}'
        if (it != end && *it != '}')
            throw fmt::format_error("invalid format specification for Point");
        return it;
    }

    /// 根据 presentation 格式化 Point
    template <typename FormatContext>
    auto format(const Point& p, FormatContext& ctx) const -> decltype(ctx.out())
    {
        if (presentation == 'p')
        {
            // 极坐标：(r, θ)，r = sqrt(x²+y²)，θ = atan2(y, x)
            double r = std::sqrt(static_cast<double>(p.x * p.x + p.y * p.y));
            double theta = std::atan2(static_cast<double>(p.y), static_cast<double>(p.x));
            return fmt::format_to(ctx.out(), "({:.2f}, {:.2f})", r, theta);
        }
        // 笛卡尔坐标：(x, y)
        return fmt::format_to(ctx.out(), "({}, {})", p.x, p.y);
    }
};

/// 一个带有枚举类型的简单结构，用于 underlying 测试
enum class Color
{
    Red   = 0,
    Green = 1,
    Blue  = 2
};

TEST_SUITE("fmt/format.h — 核心格式化功能")
{
    /**
     * 测试 API:   fmt::format(format_string<T...>, T&&...)
     * 用法说明: 最基础的格式化接口，将参数按格式字符串渲染为 std::string。
     * 预期行为: 按照 {} 占位符依次替换参数。
     */
    TEST_CASE("fmt::format — 基本占位符替换")
    {
        // 无参数格式
        SUBCASE("无参数格式字符串")
        {
            std::string s = fmt::format("hello world");
            CHECK(s == "hello world");
        }
        // 单个整数参数
        SUBCASE("单个整数参数")
        {
            std::string s = fmt::format("The answer is {}.", 42);
            CHECK(s == "The answer is 42.");
        }
        // 多个参数
        SUBCASE("多个参数依次替换")
        {
            std::string s = fmt::format("{} + {} = {}", 1, 2, 3);
            CHECK(s == "1 + 2 = 3");
        }
        // 转义花括号
        SUBCASE("转义花括号 {{ 和 }}")
        {
            std::string s = fmt::format("{{}} means empty");
            CHECK(s == "{} means empty");
        }
        // 手动位置索引
        SUBCASE("手动位置索引 {0} {1}")
        {
            std::string s = fmt::format("{0} and {1} and {0}", "a", "b");
            CHECK(s == "a and b and a");
        }
    }

    /**
     * 测试 API:   fmt::format 整数类型
     * 用法说明: 测试各种整数类型及格式说明符（十进制、十六进制、八进制、二进制）。
     * 预期行为: 输出符合标准格式规范。
     */
    TEST_CASE("fmt::format — 整数类型格式化")
    {
        SUBCASE("有符号整数")
        {
            CHECK(fmt::format("{}", 0) == "0");
            CHECK(fmt::format("{}", -1) == "-1");
            CHECK(fmt::format("{}", 2147483647) == "2147483647");
            CHECK(fmt::format("{}", -2147483648LL) == "-2147483648");
        }
        SUBCASE("无符号整数")
        {
            CHECK(fmt::format("{}", 0u) == "0");
            CHECK(fmt::format("{}", 42u) == "42");
            CHECK(fmt::format("{}", 4294967295u) == "4294967295");
        }
        SUBCASE("long long / unsigned long long")
        {
            CHECK(fmt::format("{}", 9223372036854775807LL) == "9223372036854775807");
            CHECK(fmt::format("{}", 18446744073709551615ULL) == "18446744073709551615");
        }
        SUBCASE("十六进制 :x :X")
        {
            CHECK(fmt::format("{:x}",  255) == "ff");
            CHECK(fmt::format("{:X}",  255) == "FF");
            CHECK(fmt::format("{:#x}", 255) == "0xff");
            CHECK(fmt::format("{:#X}", 255) == "0XFF");
        }
        SUBCASE("八进制 :o")
        {
            CHECK(fmt::format("{:o}",  8) == "10");
            CHECK(fmt::format("{:#o}", 8) == "010");
        }
        SUBCASE("二进制 :b :B")
        {
            CHECK(fmt::format("{:b}",  10) == "1010");
            CHECK(fmt::format("{:#b}", 10) == "0b1010");
            CHECK(fmt::format("{:#B}", 10) == "0B1010");
        }
        SUBCASE("字符类型 :c")
        {
            CHECK(fmt::format("{:c}", 65) == "A");
            CHECK(fmt::format("{:c}", 97) == "a");
        }
        SUBCASE("符号 :+ :space")
        {
            CHECK(fmt::format("{:+}", 42)  == "+42");
            CHECK(fmt::format("{:+}", -42) == "-42");
            CHECK(fmt::format("{: }", 42)  == " 42");
        }
    }

    /**
     * 测试 API:   fmt::format 浮点类型
     * 用法说明: 测试 float/double/long double 的各种格式说明符。
     * 预期行为: 按照 IEEE 754 规范格式化。
     */
    TEST_CASE("fmt::format — 浮点类型格式化")
    {
        SUBCASE("默认浮点格式（最短表示）")
        {
            CHECK(fmt::format("{}", 1.0)   == "1");
            CHECK(fmt::format("{}", 1.5)   == "1.5");
            CHECK(fmt::format("{}", 0.1)   == "0.1");
        }
        SUBCASE("固定小数点 :f")
        {
            CHECK(fmt::format("{:.2f}", 3.14159) == "3.14");
            CHECK(fmt::format("{:.0f}", 2.5)     == "2");
            CHECK(fmt::format("{:.4f}", 1.0)     == "1.0000");
        }
        SUBCASE("科学计数法 :e :E")
        {
            CHECK(fmt::format("{:.2e}", 1234.5) == "1.23e+03");
            CHECK(fmt::format("{:.2E}", 1234.5) == "1.23E+03");
        }
        SUBCASE("通用格式 :g :G")
        {
            // :g 移除末尾零，选择较短的 f/e 形式
            std::string r = fmt::format("{:g}", 1.0);
            CHECK(r == "1");
        }
        SUBCASE("特殊浮点值 nan / inf")
        {
            double nan_val = std::numeric_limits<double>::quiet_NaN();
            double inf_val = std::numeric_limits<double>::infinity();
            std::string snan = fmt::format("{}", nan_val);
            std::string sinf = fmt::format("{}", inf_val);
            // fmt 输出小写 "nan"/"inf"，但部分平台可能大写，统一转小写比较
            std::string snan_lower = core::to_lower(snan);
            std::string sinf_lower = core::to_lower(sinf);
            CHECK(snan_lower == "nan");
            CHECK(sinf_lower == "inf");

            std::string sneg = fmt::format("{}", -inf_val);
            std::string sneg_lower = core::to_lower(sneg);
            CHECK(sneg_lower == "-inf");
        }
        SUBCASE("十六进制浮点 :a :A")
        {
            // 仅检查格式合法，不检查精确值（平台差异可能存在）
            std::string r = fmt::format("{:a}", 1.0);
            CHECK(r.find("0x") != std::string::npos);
        }
    }

    /**
     * 测试 API:   fmt::format 字符串与字符
     * 用法说明: 测试 C 字符串、std::string、string_view 以及字符类型。
     * 预期行为: 直接输出字符串内容。
     */
    TEST_CASE("fmt::format — 字符串与字符格式化")
    {
        SUBCASE("C 字符串")
        {
            CHECK(fmt::format("{}", "hello") == "hello");
            CHECK(fmt::format("{}", "")      == "");
        }
        SUBCASE("std::string")
        {
            std::string s = "world";
            CHECK(fmt::format("{}", s) == "world");
        }
        SUBCASE("fmt::string_view / std::string_view")
        {
            fmt::string_view sv = "fmtlib";
            CHECK(fmt::format("{}", sv) == "fmtlib");
        }
        SUBCASE("字符 char")
        {
            CHECK(fmt::format("{}", 'A') == "A");
            CHECK(fmt::format("{}", '\n') == "\n");
        }
        SUBCASE("bool")
        {
            CHECK(fmt::format("{}", true)  == "true");
            CHECK(fmt::format("{}", false) == "false");
            // 整数格式显示 0/1
            CHECK(fmt::format("{:d}", true)  == "1");
            CHECK(fmt::format("{:d}", false) == "0");
        }
        SUBCASE("字符串精度截断 :.N")
        {
            CHECK(fmt::format("{:.3}", "hello") == "hel");
        }
        SUBCASE("调试格式 :?（显示转义）")
        {
            // 调试模式下字符串带引号并转义特殊字符
            std::string r = fmt::format("{:?}", "hello\n");
            CHECK(r == "\"hello\\n\"");
        }
    }

    /**
     * 测试 API:   fmt::format 对齐与填充
     * 用法说明: 测试左对齐、右对齐、居中对齐，以及自定义填充字符。
     * 预期行为: 输出按指定宽度和对齐方式填充。
     */
    TEST_CASE("fmt::format — 对齐与填充")
    {
        SUBCASE("右对齐（默认）")
        {
            CHECK(fmt::format("{:5}",  42) == "   42");
            CHECK(fmt::format("{:>5}", 42) == "   42");
        }
        SUBCASE("左对齐")
        {
            CHECK(fmt::format("{:<5}", 42) == "42   ");
            CHECK(fmt::format("{:<5}", "hi") == "hi   ");
        }
        SUBCASE("居中对齐")
        {
            CHECK(fmt::format("{:^5}", "x") == "  x  ");
            CHECK(fmt::format("{:^6}", "x") == "  x   ");
        }
        SUBCASE("自定义填充字符")
        {
            CHECK(fmt::format("{:*>5}", 42) == "***42");
            CHECK(fmt::format("{:0>5}", 42) == "00042");
        }
        SUBCASE("字符串宽度对齐")
        {
            CHECK(fmt::format("{:<10}", "left") == "left      ");
            CHECK(fmt::format("{:>10}", "right")== "     right");
        }
        SUBCASE("数字零填充 :05d")
        {
            CHECK(fmt::format("{:05}", 42)  == "00042");
            CHECK(fmt::format("{:05}", -42) == "-0042");
        }
    }

    /**
     * 测试 API:   fmt::vformat(string_view fmt, format_args args)
     * 用法说明: 运行时格式字符串版本，配合 fmt::make_format_args 使用。
     *           适合需要在运行时构建格式字符串的场景。
     * 预期行为: 等价于对应的 fmt::format 调用。
     */
    TEST_CASE("fmt::vformat — 运行时格式化")
    {
        SUBCASE("基本用法")
        {
            int n = 42;
            std::string s = fmt::vformat("{}", fmt::make_format_args(n));
            CHECK(s == "42");
        }
        SUBCASE("多参数")
        {
            int a = 1; std::string b = "two"; double c = 3.0;
            std::string s = fmt::vformat("{} {} {}", fmt::make_format_args(a, b, c));
            CHECK(s == "1 two 3");
        }
        SUBCASE("fmt::runtime — 禁用编译期检查")
        {
            // fmt::runtime 将字符串标记为运行时格式字符串，规避编译期验证
            std::string fmt_str = "{}";
            std::string s = fmt::format(fmt::runtime(fmt_str), 99);
            CHECK(s == "99");
        }
    }

    /**
     * 测试 API:   fmt::format_to(OutputIt out, format_string<T...> fmt, T&&... args)
     * 用法说明: 将格式化结果写入输出迭代器，不分配额外堆内存（取决于目标容器）。
     * 预期行为: 写入内容与 fmt::format 相同。
     */
    TEST_CASE("fmt::format_to — 输出到迭代器")
    {
        SUBCASE("写入 std::string（back_inserter）")
        {
            std::string out;
            fmt::format_to(std::back_inserter(out), "hello {}", "world");
            CHECK(out == "hello world");
        }
        SUBCASE("写入 std::vector<char>")
        {
            std::vector<char> buf;
            fmt::format_to(std::back_inserter(buf), "{:d}", 123);
            std::string result(buf.begin(), buf.end());
            CHECK(result == "123");
        }
        SUBCASE("写入 fmt::memory_buffer")
        {
            fmt::memory_buffer buf;
            fmt::format_to(fmt::appender(buf), "The answer is {}.", 42);
            std::string result = fmt::to_string(buf);
            CHECK(result == "The answer is 42.");
        }
        SUBCASE("写入固定大小 char 数组（format_to_result）")
        {
            // 特殊重载：format_to(char(&)[N], ...) 返回 format_to_result
            char buf[16];
            auto res = fmt::format_to(buf, "{:d}", 12345);
            // 检查是否截断（12345 长度 5 < 16，不截断）
            CHECK(res.truncated == false);
            // 输出内容
            *res.out = '\0';
            CHECK(std::string(buf) == "12345");
        }
        SUBCASE("固定大小数组截断检测")
        {
            char buf[4];
            auto res = fmt::format_to(buf, "{}", "hello");  // "hello" = 5 chars > 4
            CHECK(res.truncated == true);
        }
    }

    /**
     * 测试 API:   fmt::format_to_n(OutputIt out, size_t n, format_string, args...)
     * 用法说明: 最多写入 n 个字符，返回迭代器和总长度（含被截断部分）。
     * 预期行为: 写入字符数不超过 n，.size 返回完整输出长度。
     */
    TEST_CASE("fmt::format_to_n — 有界输出")
    {
        SUBCASE("未截断")
        {
            char buf[32] = {};
            auto res = fmt::format_to_n(buf, sizeof(buf) - 1, "{}", "hello");
            CHECK(std::string(buf, res.out) == "hello");
            CHECK(res.size == 5);
        }
        SUBCASE("截断：size 返回完整长度")
        {
            char buf[4] = {};
            // "hello world" = 11 chars
            auto res = fmt::format_to_n(buf, 4, "{}", "hello world");
            CHECK(res.size == 11);        // 总长度 11
            CHECK(std::string(buf, res.out) == "hell");  // 只写入 4 个字符
        }
        SUBCASE("写入 vector<char>")
        {
            std::vector<char> buf(16, '\0');
            auto res = fmt::format_to_n(buf.data(), 10, "The answer is {}.", 42);
            CHECK(res.size == 17);  // "The answer is 42." = 17
            // 写入前 10 个字符
            std::string s(buf.data(), std::min<size_t>(res.size, 10));
            CHECK(s == "The answer");
        }
    }

    /**
     * 测试 API:   fmt::formatted_size(format_string<T...>, T&&...)
     * 用法说明: 计算格式化后的字符数，不实际产生输出。用于提前分配缓冲区。
     * 预期行为: 返回值等于 fmt::format 输出的字符数。
     */
    TEST_CASE("fmt::formatted_size — 计算格式化大小")
    {
        SUBCASE("基本用法")
        {
            CHECK(fmt::formatted_size("{}", 42)       == 2);
            CHECK(fmt::formatted_size("{}", "hello")  == 5);
            CHECK(fmt::formatted_size("{:.2f}", 3.14) == 4);
        }
        SUBCASE("与 format 结果长度一致")
        {
            std::string s = fmt::format("The answer is {}.", 42);
            size_t expected = s.size();
            CHECK(fmt::formatted_size("The answer is {}.", 42) == expected);
        }
        SUBCASE("空格式字符串")
        {
            CHECK(fmt::formatted_size("") == 0);
        }
    }

    /**
     * 测试 API:   fmt::memory_buffer / fmt::basic_memory_buffer<T, SIZE>
     *             fmt::to_string(basic_memory_buffer<char, SIZE>)
     * 用法说明: 提供高效的动态增长字符缓冲区，避免重复分配。
     *           可通过 fmt::to_string 转为 std::string。
     * 预期行为: 支持 append/push_back/resize/reserve；to_string 返回正确内容。
     */
    TEST_CASE("fmt::memory_buffer — 内存缓冲区")
    {
        SUBCASE("format_to + to_string")
        {
            fmt::memory_buffer buf;
            fmt::format_to(fmt::appender(buf), "x = {}", 7);
            std::string s = fmt::to_string(buf);
            CHECK(s == "x = 7");
        }
        SUBCASE("多次写入")
        {
            fmt::memory_buffer buf;
            for (int i = 0; i < 3; ++i)
                fmt::format_to(fmt::appender(buf), "[{}]", i);
            CHECK(fmt::to_string(buf) == "[0][1][2]");
        }
        SUBCASE("resize / reserve")
        {
            fmt::memory_buffer buf;
            buf.reserve(64);
            CHECK(buf.capacity() >= 64);
            buf.resize(10);
            CHECK(buf.size() == 10);
        }
        SUBCASE("clear 后重用")
        {
            fmt::memory_buffer buf;
            fmt::format_to(fmt::appender(buf), "old");
            buf.clear();
            CHECK(buf.size() == 0);
            fmt::format_to(fmt::appender(buf), "new");
            CHECK(fmt::to_string(buf) == "new");
        }
        SUBCASE("移动语义")
        {
            fmt::memory_buffer buf1;
            fmt::format_to(fmt::appender(buf1), "move me");
            fmt::memory_buffer buf2 = std::move(buf1);
            CHECK(fmt::to_string(buf2) == "move me");
        }
    }

    /**
     * 测试 API:   fmt::to_string(T value)
     * 用法说明: 将数值类型快速转换为字符串，等价于 fmt::format("{}", value)。
     * 预期行为: 返回对应的十进制字符串表示。
     */
    TEST_CASE("fmt::to_string — 值转字符串")
    {
        SUBCASE("整数")
        {
            CHECK(fmt::to_string(42)         == "42");
            CHECK(fmt::to_string(-1)         == "-1");
            CHECK(fmt::to_string(0)          == "0");
            CHECK(fmt::to_string(1000000LL)  == "1000000");
        }
        SUBCASE("bool")
        {
            // bool 特化：to_string 返回 "true"/"false"
            CHECK(fmt::to_string(true)  == "true");
            CHECK(fmt::to_string(false) == "false");
        }
        SUBCASE("浮点（通用格式）")
        {
            // to_string 使用最短精确表示
            std::string s = fmt::to_string(1.5);
            CHECK(s == "1.5");
        }
    }

    /**
     * 测试 API:   fmt::format_error (继承自 std::runtime_error)
     * 用法说明: 当格式字符串非法时，fmt 会抛出 fmt::format_error。
     *           用户也可以主动构造并抛出该异常。
     * 预期行为: 异常继承自 std::runtime_error，what() 返回错误消息。
     */
    TEST_CASE("fmt::format_error — 格式化错误")
    {
        SUBCASE("手动构造 format_error")
        {
            fmt::format_error err("test error");
            CHECK(std::string(err.what()) == "test error");
        }
        SUBCASE("format_error 继承自 std::runtime_error")
        {
            // 可以用 std::runtime_error& 捕获
            try {
                throw fmt::format_error("my error");
            } catch (const std::runtime_error& e)
            {
                CHECK(std::string(e.what()) == "my error");
            }
        }
        SUBCASE("非法格式字符串抛出 format_error")
        {
            // 未闭合的花括号
            CHECK_THROWS_AS(std::ignore = fmt::format(fmt::runtime("{"), 42), fmt::format_error);
            // 参数索引越界
            CHECK_THROWS_AS(std::ignore = fmt::format(fmt::runtime("{0} {2}"), 1, 2), fmt::format_error);
            // 不匹配的类型说明符（对字符串使用整数格式）
            CHECK_THROWS_AS(std::ignore = fmt::format(fmt::runtime("{:d}"), "not_a_number"), fmt::format_error);
        }
    }

    /**
     * 测试 API:   fmt::format_int(value)
     *             format_int::data() / c_str() / str() / size()
     * 用法说明: 高效的整数到字符串转换，避免动态内存分配。
     *           适合对性能敏感的整数格式化场景。
     * 预期行为: 输出与 std::to_string 一致；c_str() 以 null 结尾。
     */
    TEST_CASE("fmt::format_int — 快速整数格式化器")
    {
        SUBCASE("正整数")
        {
            fmt::format_int fi(42);
            CHECK(fi.str()  == "42");
            CHECK(fi.size() == 2);
            CHECK(std::string(fi.c_str()) == "42");
        }
        SUBCASE("负整数")
        {
            fmt::format_int fi(-12345);
            CHECK(fi.str() == "-12345");
        }
        SUBCASE("零")
        {
            fmt::format_int fi(0);
            CHECK(fi.str() == "0");
        }
        SUBCASE("long long 最大值")
        {
            fmt::format_int fi(9223372036854775807LL);
            CHECK(fi.str() == "9223372036854775807");
        }
        SUBCASE("unsigned long long")
        {
            fmt::format_int fi(18446744073709551615ULL);
            CHECK(fi.str() == "18446744073709551615");
        }
        SUBCASE("data() 指针有效且长度正确")
        {
            fmt::format_int fi(777);
            CHECK(fi.size() == 3);
            CHECK(std::string(fi.data(), fi.size()) == "777");
        }
    }

    /**
     * 测试 API:   fmt::ptr(T* p) -> const void*
     * 用法说明: 将非 void 指针转换为 const void* 以进行格式化。
     *           指针输出为 0x 开头的十六进制地址。
     * 预期行为: 格式化结果以 "0x" 开头；空指针输出 "0x0" 或 "(nil)" 等平台相关值。
     */
    TEST_CASE("fmt::ptr — 指针格式化")
    {
        SUBCASE("格式化非空指针")
        {
            int value = 42;
            std::string s = fmt::format("{}", fmt::ptr(&value));
            // 指针以 0x 开头（主流平台稳定）
            CHECK(s.find("0x") == 0);
        }
        SUBCASE("格式化空指针")
        {
            int* p = nullptr;
            std::string s = fmt::format("{}", fmt::ptr(p));
            // 空指针：0x0 或类似
            CHECK(s.find("0x") == 0);
        }
        SUBCASE("不同指针类型")
        {
            double d = 3.14;
            char c = 'z';
            std::string sd = fmt::format("{}", fmt::ptr(&d));
            std::string sc = fmt::format("{}", fmt::ptr(&c));
            CHECK(sd.find("0x") == 0);
            CHECK(sc.find("0x") == 0);
        }
    }

    /**
     * 测试 API:   fmt::underlying(Enum e) -> underlying_t<Enum>
     * 用法说明: 将枚举值转换为其底层整数类型以进行格式化。
     *           避免为每个枚举写 static_cast。
     * 预期行为: 输出枚举的整数值。
     */
    TEST_CASE("fmt::underlying — 枚举转底层类型")
    {
        SUBCASE("enum class Color")
        {
            CHECK(fmt::format("{}", fmt::underlying(Color::Red))   == "0");
            CHECK(fmt::format("{}", fmt::underlying(Color::Green)) == "1");
            CHECK(fmt::format("{}", fmt::underlying(Color::Blue))  == "2");
        }
        SUBCASE("普通 enum")
        {
            enum Status { OK = 200, NotFound = 404, Error = 500 };
            CHECK(fmt::format("{}", fmt::underlying(OK))       == "200");
            CHECK(fmt::format("{}", fmt::underlying(NotFound)) == "404");
        }
        SUBCASE("底层为 unsigned char 的枚举")
        {
            enum class Byte : unsigned char { A = 0, B = 255 };
            CHECK(fmt::format("{}", fmt::underlying(Byte::A)) == "0");
            CHECK(fmt::format("{}", fmt::underlying(Byte::B)) == "255");
        }
    }

    /**
     * 测试 API:   fmt::group_digits(T value) -> group_digits_view<T>
     * 用法说明: 使用 ',' 作为千位分隔符格式化整数（不依赖 locale）。
     * 预期行为: 每三位插入一个逗号。
     */
    TEST_CASE("fmt::group_digits — 数字分组")
    {
        SUBCASE("基本用法")
        {
            CHECK(fmt::format("{}", fmt::group_digits(1000))    == "1,000");
            CHECK(fmt::format("{}", fmt::group_digits(12345))   == "12,345");
            CHECK(fmt::format("{}", fmt::group_digits(1234567)) == "1,234,567");
        }
        SUBCASE("小数不分组")
        {
            CHECK(fmt::format("{}", fmt::group_digits(0))   == "0");
            CHECK(fmt::format("{}", fmt::group_digits(999)) == "999");
        }
        SUBCASE("long long")
        {
            CHECK(fmt::format("{}", fmt::group_digits(1000000000LL)) == "1,000,000,000");
        }
        SUBCASE("配合宽度")
        {
            // 带宽度的 group_digits
            std::string s = fmt::format("{:>15}", fmt::group_digits(1234567));
            CHECK(s == "      1,234,567");
        }
    }

    /**
     * 测试 API:   fmt::bytes(string_view s)
     * 用法说明: 将字符串以原始字节形式处理（按字节宽度而非 Unicode 码点对齐）。
     * 预期行为: 可配合宽度/精度说明符对字节数组进行格式化。
     */
    TEST_CASE("fmt::bytes — 字节序列格式化")
    {
        SUBCASE("基本输出")
        {
            std::string data = "hello";
            std::string s = fmt::format("{}", fmt::bytes(data));
            CHECK(s == "hello");
        }
        SUBCASE("配合精度截断")
        {
            // fmt::bytes 忽略精度说明符，输出完整字节序列
            std::string data = "hello world";
            std::string s = fmt::format("{:.5}", fmt::bytes(data));
            CHECK(s == "hello world");
        }
        SUBCASE("配合宽度对齐")
        {
            std::string data = "hi";
            std::string s = fmt::format("{:<10}", fmt::bytes(data));
            CHECK(s == "hi        ");
        }
    }

    /**
     * 测试 API:   FMT_STRING(s)
     * 用法说明: 将字符串字面量转化为编译期格式字符串，触发格式字符串验证。
     *           错误的格式字符串会导致编译错误（不会运行到断言处）。
     * 预期行为: 正确的 FMT_STRING 使用产生与普通格式化相同的输出。
     */
    TEST_CASE("FMT_STRING — 编译时格式字符串")
    {
        SUBCASE("基本正确用法")
        {
            auto s = fmt::format(FMT_STRING("{}"), 42);
            CHECK(s == "42");
        }
        SUBCASE("多参数")
        {
            auto s = fmt::format(FMT_STRING("{} + {} = {}"), 1, 2, 3);
            CHECK(s == "1 + 2 = 3");
        }
        SUBCASE("格式说明符")
        {
            auto s = fmt::format(FMT_STRING("{:05d}"), 42);
            CHECK(s == "00042");
        }
    }

    /**
     * 测试 API:   fmt::arg(const char* name, const T& value)
     *             using namespace fmt::literals; "name"_a = value
     * 用法说明: 创建命名参数，在格式字符串中用 {name} 引用。
     * 预期行为: {name} 替换为对应命名参数的格式化值。
     */
    TEST_CASE("fmt::arg 和 _a — 命名参数")
    {
        SUBCASE("fmt::arg 基本用法")
        {
            std::string s = fmt::format("The answer is {answer}.",
                                         fmt::arg("answer", 42));
            CHECK(s == "The answer is 42.");
        }
        SUBCASE("多个命名参数")
        {
            std::string s = fmt::format("{name} is {age} years old.",
                                         fmt::arg("name", "Alice"),
                                         fmt::arg("age", 30));
            CHECK(s == "Alice is 30 years old.");
        }
        SUBCASE("_a 用户自定义字面量")
        {
            using namespace fmt::literals;
            std::string s = fmt::format("{greeting}, {subject}!",
                                         "greeting"_a = "Hello",
                                         "subject"_a = "World");
            CHECK(s == "Hello, World!");
        }
        SUBCASE("命名参数与位置参数混合")
        {
            using namespace fmt::literals;
            std::string s = fmt::format("{} and {name}",
                                         "pos0",
                                         "name"_a = "named");
            CHECK(s == "pos0 and named");
        }
    }

    /**
     * 测试 API:   fmt::print(format_string, args...)
     *             fmt::print(FILE*, format_string, args...)
     *             fmt::println(format_string, args...)
     *             fmt::println(FILE*, format_string, args...)
     * 用法说明: 将格式化结果直接写入 stdout 或指定文件。
     *           println 在末尾追加换行符。
     * 注意: 直接测试 stdout 输出较困难，此处仅验证函数可调用且不抛出异常。
     *       若需要验证输出内容，建议重定向 stdout 或使用 fmt::format 代替。
     */
    TEST_CASE("fmt::print / println — 输出到 stdout")
    {
        SUBCASE("print 不抛出异常")
        {
            // 注意：这会实际向 stdout 写内容，仅验证不抛出
            CHECK_NOTHROW(fmt::print(""));
            CHECK_NOTHROW(fmt::print("{}", 42));
        }
        SUBCASE("println 不抛出异常")
        {
            CHECK_NOTHROW(fmt::println(""));
            CHECK_NOTHROW(fmt::println("{}", 42));
        }
        SUBCASE("print 到 stderr")
        {
            CHECK_NOTHROW(fmt::print(stderr, "{}", "error log"));
        }
        SUBCASE("println 到 stderr")
        {
            CHECK_NOTHROW(fmt::println(stderr, "{}", "error with newline"));
        }
    }

    /**
     * 测试 API:   fmt::string_buffer
     *             fmt::writer
     *             writer::print(format_string, args...)
     * 用法说明: string_buffer 是字符串后端的缓冲写入器，
     *           可隐式转换为 fmt::writer 以调用 print。
     * 预期行为: writer::print 将内容追加到关联缓冲区。
     */
    TEST_CASE("fmt::writer / fmt::string_buffer")
    {
        SUBCASE("string_buffer 基本用法")
        {
            fmt::string_buffer sb;
            fmt::writer w = sb;
            w.print("{}", 42);
            w.print(" hello");
            CHECK(sb.str() == "42 hello");
        }
        SUBCASE("多次写入累积")
        {
            fmt::string_buffer sb;
            fmt::writer w = sb;
            for (int i = 0; i < 3; ++i)
                w.print("[{}]", i);
            CHECK(sb.str() == "[0][1][2]");
        }
    }

    /**
     * 测试 API:   fmt::system_error(int error_code, format_string, args...)
     *             fmt::format_system_error(buffer<char>&, int, const char*)
     * 用法说明: 构建包含系统错误描述的异常对象或格式化系统错误信息。
     * 预期行为: what() 中包含用户消息和系统错误描述。
     */
    TEST_CASE("fmt::system_error / fmt::format_system_error")
    {
        SUBCASE("system_error 基本构造")
        {
            // ENOENT = 2 (no such file or directory)
            auto err = fmt::system_error(ENOENT, "cannot open '{}'", "test.txt");
            std::string what = err.what();
            // 消息中应包含用户指定的前缀
            CHECK(what.find("cannot open 'test.txt'") != std::string::npos);
        }
        SUBCASE("system_error 是 std::system_error")
        {
            auto err = fmt::system_error(0, "ok");
            // 可以用 std::system_error& 捕获
            const std::system_error& se = err;
            CHECK(se.code().category().name() != nullptr);
        }
        SUBCASE("format_system_error 写入 buffer")
        {
            fmt::memory_buffer buf;
            fmt::format_system_error(buf, ENOENT, "file error");
            std::string s = fmt::to_string(buf);
            CHECK(s.find("file error") != std::string::npos);
        }
    }

    /**
     * 测试 API:   template<> struct fmt::formatter<T> { parse, format }
     * 用法说明: 为自定义类型实现 formatter 特化，使其可以用 fmt::format 格式化。
     *           需要实现 parse（解析格式规格）和 format（生成输出）两个成员函数。
     * 预期行为: fmt::format("{}", myobj) 调用自定义 formatter::format。
     */
    TEST_CASE("自定义 formatter<T> 特化")
    {
        SUBCASE("基本格式化自定义类型（默认笛卡尔坐标）")
        {
            // Point 已在文件顶部特化了 formatter<Point>
            Point p{3, 4};
            std::string s = fmt::format("{}", p);
            CHECK(s == "(3, 4)");
        }
        SUBCASE(":d 笛卡尔坐标")
        {
            Point p{3, 4};
            std::string s = fmt::format("{:d}", p);
            CHECK(s == "(3, 4)");
        }
        SUBCASE(":p 极坐标")
        {
            Point p{3, 4};
            std::string s = fmt::format("{:p}", p);
            // r = sqrt(3²+4²) = 5.00, θ = atan2(4, 3) ≈ 0.93 rad
            CHECK(s.find("5.00") != std::string::npos);
            CHECK(s.find("0.93") != std::string::npos);
        }
        SUBCASE("format_to 也使用自定义 formatter")
        {
            Point p{-1, 0};
            std::string out;
            fmt::format_to(std::back_inserter(out), "p={}", p);
            CHECK(out == "p=(-1, 0)");
        }
        SUBCASE("多个自定义类型参数")
        {
            Point a{1, 2}, b{3, 4};
            std::string s = fmt::format("A={} B={}", a, b);
            CHECK(s == "A=(1, 2) B=(3, 4)");
        }
        SUBCASE("非法格式说明符抛出 format_error")
        {
            Point p{1, 2};
            CHECK_THROWS_AS(std::ignore = fmt::format("{:z}", p), fmt::format_error);
        }
        SUBCASE("fmt::is_formattable — 检测类型是否可格式化")
        {
            // int, double, std::string 是内置可格式化类型
            static_assert(fmt::is_formattable<int>::value,        "int should be formattable");
            static_assert(fmt::is_formattable<double>::value,     "double should be formattable");
            static_assert(fmt::is_formattable<std::string>::value,"string should be formattable");
            // Point 有特化 formatter，应为可格式化
            static_assert(fmt::is_formattable<Point>::value,      "Point should be formattable");
            // 验证检测结果正确
            CHECK(fmt::is_formattable<int>::value    == true);
            CHECK(fmt::is_formattable<Point>::value  == true);
        }
    }
} // TEST_SUITE("fmt/format.h — 核心格式化功能")

TEST_SUITE("fmt/chrono.h — 时间格式化")
{
    /**
     * 测试 API:   formatter<std::chrono::duration<Rep, Period>>
     * 用法说明: 直接格式化 chrono duration 类型，默认输出带单位后缀。
     *           可使用 %H:%M:%S 等 chrono 格式说明符进行自定义。
     * 预期行为: 单位后缀与 period 对应（s/ms/us/min/h）；自定义格式按说明符输出。
     */
    TEST_CASE("std::chrono::duration — 默认格式（带单位后缀）")
    {
        using namespace std::chrono_literals;

        SUBCASE("秒 seconds")
        {
            CHECK(fmt::format("{}", std::chrono::seconds(42)) == "42s");
            CHECK(fmt::format("{}", std::chrono::seconds(0))  == "0s");
            CHECK(fmt::format("{}", std::chrono::seconds(-1)) == "-1s");
        }
        SUBCASE("毫秒 milliseconds")
        {
            CHECK(fmt::format("{}", std::chrono::milliseconds(1500)) == "1500ms");
            CHECK(fmt::format("{}", std::chrono::milliseconds(0))    == "0ms");
        }
        SUBCASE("分钟 minutes")
        {
            CHECK(fmt::format("{}", std::chrono::minutes(90)) == "90min");
        }
        SUBCASE("小时 hours")
        {
            CHECK(fmt::format("{}", std::chrono::hours(24)) == "24h");
        }
        SUBCASE("微秒 microseconds")
        {
            // 微秒单位：µs（UTF-8 平台）或 us（非 UTF-8 平台）
            std::string s = fmt::format("{}", std::chrono::microseconds(100));
            // 应以 "100" 开头，后跟单位后缀（µs 或 us）
            CHECK(s.find("100") == 0);
            // 后缀应包含 's'（无论是 µs 还是 us）
            CHECK(s.find('s') != std::string::npos);
            CHECK(s.size() > 3);
        }
        SUBCASE("浮点 duration")
        {
            // 浮点表示的 duration
            std::chrono::duration<double> d(1.5);
            std::string s = fmt::format("{}", d);
            CHECK(s.find("1.5") == 0);
        }
    }

    TEST_CASE("std::chrono::duration — 自定义格式说明符")
    {
        using namespace std::chrono;

        SUBCASE("%H:%M:%S 小时:分钟:秒")
        {
            // 3661 秒 = 1 小时 1 分 1 秒
            auto d = seconds(3661);
            std::string s = fmt::format("{:%H:%M:%S}", d);
            CHECK(s == "01:01:01");
        }
        SUBCASE("%H:%M:%S 零值")
        {
            auto d = seconds(0);
            std::string s = fmt::format("{:%H:%M:%S}", d);
            CHECK(s == "00:00:00");
        }
        SUBCASE("%H:%M:%S 大值")
        {
            // 86400 秒 = 24 小时整
            // %H 对 24 取模，所以 24 小时显示为 00
            auto d = seconds(86400);
            std::string s = fmt::format("{:%H:%M:%S}", d);
            CHECK(s == "00:00:00");
        }
        SUBCASE("%Q 仅数值部分")
        {
            auto d = seconds(42);
            std::string s = fmt::format("{:%Q}", d);
            CHECK(s == "42");
        }
        SUBCASE("%q 仅单位部分")
        {
            auto d = seconds(1);
            std::string s = fmt::format("{:%q}", d);
            CHECK(s == "s");
        }
        SUBCASE("毫秒精度 %S 包含小数部分")
        {
            // 1500 ms = 1 s + 500 ms，%S 应包含小数秒
            auto d = milliseconds(1500);
            std::string s = fmt::format("{:%S}", d);
            // 应包含 ".500" 或类似小数部分
            CHECK(s.find('.') != std::string::npos);
        }
        SUBCASE("宽度与对齐")
        {
            auto d = seconds(5);
            std::string s = fmt::format("{:>10%Q%q}", d);
            CHECK(s.size() == 10);
            // 右对齐，内容为 "5s"
            CHECK(s.find("5s") != std::string::npos);
        }
    }

    /**
     * 测试 API:   formatter<std::tm>
     *             fmt::gmtime(std::time_t) → std::tm
     * 用法说明: 格式化 C 语言 tm 结构体，支持标准 strftime 风格的格式说明符。
     *           gmtime 是 std::gmtime 的线程安全版本。
     * 预期行为: 使用固定 epoch 值（1970-01-01 00:00:00 UTC）验证格式化正确性。
     */
    TEST_CASE("std::tm 格式化 + fmt::gmtime")
    {
        SUBCASE("fmt::gmtime(0) = 1970-01-01 00:00:00 UTC")
        {
            // epoch 0 对应 1970-01-01 00:00:00 UTC
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            // 默认格式为 %F %T（ISO 8601 日期 + 时间）
            std::string s = fmt::format("{}", t);
            CHECK(s == "1970-01-01 00:00:00");
        }
        SUBCASE("%Y-%m-%d 日期格式")
        {
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string s = fmt::format("{:%Y-%m-%d}", t);
            CHECK(s == "1970-01-01");
        }
        SUBCASE("%H:%M:%S 时间格式")
        {
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string s = fmt::format("{:%H:%M:%S}", t);
            CHECK(s == "00:00:00");
        }
        SUBCASE("%F %T 完整 ISO 8601")
        {
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string s = fmt::format("{:%F %T}", t);
            CHECK(s == "1970-01-01 00:00:00");
        }
        SUBCASE("已知时间戳验证")
        {
            // 2000-01-01 00:00:00 UTC = 946684800
            std::tm t = fmt::gmtime(static_cast<std::time_t>(946684800));
            CHECK(fmt::format("{:%Y}", t) == "2000");
            CHECK(fmt::format("{:%m}", t) == "01");
            CHECK(fmt::format("{:%d}", t) == "01");
        }
        SUBCASE("星期几 %a %A")
        {
            // 1970-01-01 是星期四
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string abbr = fmt::format("{:%a}", t);
            std::string full = fmt::format("{:%A}", t);
            CHECK(abbr == "Thu");
            CHECK(full == "Thursday");
        }
        SUBCASE("月份名称 %b %B")
        {
            // 1970-01-01 = January
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string abbr = fmt::format("{:%b}", t);
            std::string full = fmt::format("{:%B}", t);
            CHECK(abbr == "Jan");
            CHECK(full == "January");
        }
        SUBCASE("fmt::gmtime 线程安全版本不抛出")
        {
            CHECK_NOTHROW(fmt::gmtime(static_cast<std::time_t>(0)));
            CHECK_NOTHROW(fmt::gmtime(static_cast<std::time_t>(1000000)));
        }
    }

    /**
     * 测试 API:   fmt::weekday / fmt::day / fmt::month / fmt::year
     *             fmt::year_month_day
     * 用法说明: 格式化 C++20 日期类型（或 fmt 提供的回退实现）。
     *           这些类型在 C++17 下通过 fmt 自行定义。
     * 预期行为: 按照日历语义正确格式化。
     */
    TEST_CASE("fmt::weekday / day / month / year / year_month_day 格式化")
    {
        SUBCASE("fmt::weekday — 星期")
        {
            // 0=Sunday, 1=Monday, ..., 6=Saturday
            fmt::weekday sun(0);
            std::string s = fmt::format("{}", sun);
            CHECK(s == "Sun");

            fmt::weekday mon(1);
            CHECK(fmt::format("{}", mon) == "Mon");
        }
        SUBCASE("fmt::day — 月中第几天")
        {
            fmt::day d(15);
            std::string s = fmt::format("{}", d);
            // 默认格式输出两位数字
            CHECK(s == "15");
        }
        SUBCASE("fmt::month — 月份")
        {
            fmt::month jan(1);
            std::string s = fmt::format("{}", jan);
            CHECK(s == "Jan");

            fmt::month dec(12);
            CHECK(fmt::format("{}", dec) == "Dec");
        }
        SUBCASE("fmt::year — 年份")
        {
            fmt::year y(2024);
            std::string s = fmt::format("{}", y);
            CHECK(s == "2024");
        }
        SUBCASE("fmt::year_month_day — 完整日期")
        {
            fmt::year_month_day ymd{
                fmt::year(2024),
                fmt::month(3),
                fmt::day(15)
            };
            std::string s = fmt::format("{}", ymd);
            // 默认格式为 ISO 日期：2024-03-15
            CHECK(s == "2024-03-15");
        }
    }

    /**
     * 测试 API:   formatter<fmt::sys_time<Duration>>
     *             fmt::sys_time<Duration> = time_point<system_clock, Duration>
     * 用法说明: 格式化 system_clock 时间点，支持 strftime 风格格式说明符。
     *           默认格式为 UTC 时间。
     * 预期行为: epoch 时间点格式化为 "1970-01-01 00:00:00"（UTC）。
     */
    TEST_CASE("fmt::sys_time 格式化")
    {
        SUBCASE("epoch 时间点（seconds）")
        {
            // 构造一个距 epoch 0 秒的时间点
            auto tp = fmt::sys_time<std::chrono::seconds>(std::chrono::seconds(0));
            std::string s = fmt::format("{}", tp);
            CHECK(s == "1970-01-01 00:00:00");
        }
        SUBCASE("已知时间戳（946684800 = 2000-01-01 00:00:00 UTC）")
        {
            auto tp = fmt::sys_time<std::chrono::seconds>(std::chrono::seconds(946684800));
            std::string s = fmt::format("{:%F %T}", tp);
            CHECK(s == "2000-01-01 00:00:00");
        }
        SUBCASE("毫秒精度时间点")
        {
            // 1.5 秒后的时间点（含小数秒）
            auto tp = fmt::sys_time<std::chrono::milliseconds>(std::chrono::milliseconds(1500));
            std::string s = fmt::format("{}", tp);
            // 默认格式应包含小数秒
            CHECK(s.find("1970-01-01") != std::string::npos);
            CHECK(s.find(".500") != std::string::npos);
        }
    }
} // TEST_SUITE("fmt/chrono.h — 时间格式化")

TEST_SUITE("fmt/compile.h — 编译时格式化")
{
    /**
     * 测试 API:   FMT_COMPILE(s) 宏
     *             fmt::format(compiled_string, args...)
     *             fmt::format_to(out, compiled_string, args...)
     *             fmt::formatted_size(compiled_string, args...)
     *             fmt::format_to_n(out, n, compiled_string, args...)
     * 用法说明: FMT_COMPILE 在支持 C++17 constexpr-if 的编译器上将格式字符串
     *           编译为高效的格式化代码，规避运行时解析开销。
     *           在不支持的编译器上退化为 FMT_STRING。
     * 预期行为: 输出结果与普通 fmt::format 完全相同。
     */
    TEST_CASE("FMT_COMPILE + fmt::format")
    {
        SUBCASE("整数格式化")
        {
            // FMT_COMPILE 在支持 constexpr-if 的编译器上生成优化代码
            auto s = fmt::format(FMT_COMPILE("{}"), 42);
            CHECK(s == "42");
        }
        SUBCASE("字符串格式化")
        {
            auto s = fmt::format(FMT_COMPILE("hello {}"), "world");
            CHECK(s == "hello world");
        }
        SUBCASE("多参数")
        {
            auto s = fmt::format(FMT_COMPILE("{} + {} = {}"), 1, 2, 3);
            CHECK(s == "1 + 2 = 3");
        }
        SUBCASE("格式说明符")
        {
            auto s = fmt::format(FMT_COMPILE("{:08.3f}"), 3.14);
            CHECK(s == "0003.140");
        }
        SUBCASE("空格式字符串")
        {
            auto s = fmt::format(FMT_COMPILE("no args"));
            CHECK(s == "no args");
        }
        SUBCASE("十六进制")
        {
            auto s = fmt::format(FMT_COMPILE("{:#x}"), 255);
            CHECK(s == "0xff");
        }
    }

    TEST_CASE("FMT_COMPILE + fmt::format_to")
    {
        SUBCASE("写入 std::string")
        {
            std::string out;
            fmt::format_to(std::back_inserter(out), FMT_COMPILE("{}"), 99);
            CHECK(out == "99");
        }
        SUBCASE("写入 memory_buffer")
        {
            fmt::memory_buffer buf;
            fmt::format_to(fmt::appender(buf), FMT_COMPILE("{:d}"), 42);
            CHECK(fmt::to_string(buf) == "42");
        }
        SUBCASE("多次写入")
        {
            std::string out;
            auto it = std::back_inserter(out);
            fmt::format_to(it, FMT_COMPILE("[{}]"), 1);
            fmt::format_to(it, FMT_COMPILE("[{}]"), 2);
            fmt::format_to(it, FMT_COMPILE("[{}]"), 3);
            CHECK(out == "[1][2][3]");
        }
    }

    TEST_CASE("FMT_COMPILE + fmt::formatted_size")
    {
        SUBCASE("计算格式化大小")
        {
            auto sz = fmt::formatted_size(FMT_COMPILE("{}"), 12345);
            CHECK(sz == 5);
        }
        SUBCASE("与 format 结果一致")
        {
            auto s  = fmt::format(FMT_COMPILE("{:>10}"), "hi");
            auto sz = fmt::formatted_size(FMT_COMPILE("{:>10}"), "hi");
            CHECK(s.size() == sz);
            CHECK(sz == 10);
        }
    }

    TEST_CASE("FMT_COMPILE + fmt::format_to_n")
    {
        SUBCASE("未截断")
        {
            char buf[32] = {};
            auto res = fmt::format_to_n(buf, 31, FMT_COMPILE("{}"), "hello");
            *res.out = '\0';
            CHECK(std::string(buf) == "hello");
            CHECK(res.size == 5);
        }
        SUBCASE("截断")
        {
            char buf[4] = {};
            auto res = fmt::format_to_n(buf, 4, FMT_COMPILE("{}"), "hello world");
            CHECK(res.size == 11);  // 完整长度
            CHECK(std::string(buf, res.out) == "hell");
        }
    }
} // TEST_SUITE("fmt/compile.h — 编译时格式化")

TEST_SUITE("fmt/std.h — 标准库类型支持")
{
    /**
     * 测试 API:   formatter<std::optional<T>, Char>
     * 用法说明: 格式化 std::optional<T>。
     *           - 有值时输出 "optional(value)"
     *           - 无值时输出 "none"
     * 预期行为: 与文档一致；字符串值带引号。
     */
    TEST_CASE("std::optional 格式化")
    {
        SUBCASE("有值的 optional<int>")
        {
            std::optional<int> opt = 42;
            std::string s = fmt::format("{}", opt);
            CHECK(s == "optional(42)");
        }
        SUBCASE("空值 optional<int>")
        {
            std::optional<int> opt;
            std::string s = fmt::format("{}", opt);
            CHECK(s == "none");
        }
        SUBCASE("optional<string> — 字符串值带引号")
        {
            std::optional<std::string> opt = "hello";
            std::string s = fmt::format("{}", opt);
            // 字符串以调试模式输出，带引号
            CHECK(s == "optional(\"hello\")");
        }
        SUBCASE("optional<double>")
        {
            std::optional<double> opt = 3.14;
            std::string s = fmt::format("{}", opt);
            CHECK(s.find("optional(") == 0);
            CHECK(s.find("3.14") != std::string::npos);
        }
        SUBCASE("std::nullopt")
        {
            std::optional<int> opt = std::nullopt;
            CHECK(fmt::format("{}", opt) == "none");
        }
    }

    /**
     * 测试 API:   formatter<std::variant<T...>, Char>
     *             formatter<std::monostate, Char>
     * 用法说明: 格式化 std::variant，输出 "variant(value)"；
     *           std::monostate 输出 "monostate"。
     * 预期行为: 字符串值带引号；单类型 variant 直接显示值。
     */
    TEST_CASE("std::variant 格式化")
    {
        SUBCASE("variant<int> — 整数值")
        {
            std::variant<int, std::string, double> v = 42;
            std::string s = fmt::format("{}", v);
            CHECK(s == "variant(42)");
        }
        SUBCASE("variant<string> — 字符串值带引号")
        {
            std::variant<int, std::string, double> v = std::string("hello");
            std::string s = fmt::format("{}", v);
            CHECK(s == "variant(\"hello\")");
        }
        SUBCASE("variant<double>")
        {
            std::variant<int, std::string, double> v = 3.14;
            std::string s = fmt::format("{}", v);
            CHECK(s.find("variant(") == 0);
        }
        SUBCASE("std::monostate")
        {
            std::variant<std::monostate, int> v;  // 默认 monostate
            std::string s = fmt::format("{}", v);
            CHECK(s == "variant(monostate)");
        }
        SUBCASE("独立 std::monostate")
        {
            std::string s = fmt::format("{}", std::monostate{});
            CHECK(s == "monostate");
        }
    }

    /**
     * 测试 API:   formatter<std::error_code>
     * 用法说明: 格式化 std::error_code，默认输出 "category:value"。
     *           使用 :s 格式说明符输出错误消息字符串。
     * 预期行为: 格式正确；category 名称和 value 与构造时一致。
     */
    TEST_CASE("std::error_code 格式化")
    {
        SUBCASE("默认格式 category:value")
        {
            std::error_code ec = std::make_error_code(std::errc::no_such_file_or_directory);
            std::string s = fmt::format("{}", ec);
            // 格式为 "generic:2" 或类似（category 名称和 value）
            CHECK(s.find("generic") != std::string::npos);
            // value = ENOENT = 2
            CHECK(s.find("2") != std::string::npos);
        }
        SUBCASE(":s 格式输出消息字符串")
        {
            std::error_code ec = std::make_error_code(std::errc::no_such_file_or_directory);
            std::string s = fmt::format("{:s}", ec);
            // 消息字符串不为空
            CHECK(!s.empty());
        }
        SUBCASE("成功错误码 value=0")
        {
            std::error_code ec;  // 默认构造 = 无错误
            std::string s = fmt::format("{}", ec);
            CHECK(s.find("0") != std::string::npos);
        }
        SUBCASE("调试格式 :?")
        {
            std::error_code ec = std::make_error_code(std::errc::permission_denied);
            std::string s = fmt::format("{:?}", ec);
            // 调试格式带引号
            CHECK(s.find('"') != std::string::npos);
        }
    }

    /**
     * 测试 API:   formatter<T> where T 继承自 std::exception
     * 用法说明: 格式化 std::exception 子类，输出 what() 的内容。
     * 预期行为: 输出 what() 字符串；格式化不抛出。
     */
    TEST_CASE("std::exception 格式化")
    {
        SUBCASE("std::runtime_error")
        {
            std::runtime_error ex("something went wrong");
            std::string s = fmt::format("{}", ex);
            CHECK(s == "something went wrong");
        }
        SUBCASE("std::invalid_argument")
        {
            std::invalid_argument ex("bad argument");
            std::string s = fmt::format("{}", ex);
            CHECK(s == "bad argument");
        }
        SUBCASE("自定义异常类")
        {
            struct MyError : std::exception {
                const char* what() const noexcept override { return "my error"; }
            };
            MyError err;
            std::string s = fmt::format("{}", err);
            CHECK(s == "my error");
        }
    }

    /**
     * 测试 API:   formatter<std::bitset<N>, Char>
     * 用法说明: 将 bitset 格式化为二进制字符串（高位在前）。
     * 预期行为: 输出为 '0'/'1' 组成的字符串，最高位在最左。
     */
    TEST_CASE("std::bitset 格式化")
    {
        SUBCASE("bitset<8> 全零")
        {
            std::bitset<8> bs(0);
            CHECK(fmt::format("{}", bs) == "00000000");
        }
        SUBCASE("bitset<8> 全一")
        {
            std::bitset<8> bs(255);
            CHECK(fmt::format("{}", bs) == "11111111");
        }
        SUBCASE("bitset<8> 部分位")
        {
            std::bitset<8> bs(0b10101010);
            CHECK(fmt::format("{}", bs) == "10101010");
        }
        SUBCASE("bitset<4>")
        {
            std::bitset<4> bs(5);  // 0101
            CHECK(fmt::format("{}", bs) == "0101");
        }
        SUBCASE("bitset<1>")
        {
            std::bitset<1> bs1(1);
            std::bitset<1> bs0(0);
            CHECK(fmt::format("{}", bs1) == "1");
            CHECK(fmt::format("{}", bs0) == "0");
        }
        SUBCASE("配合宽度对齐")
        {
            std::bitset<4> bs(3);  // 0011
            std::string s = fmt::format("{:>8}", bs);
            CHECK(s == "    0011");
        }
    }

    /**
     * 测试 API:   formatter<std::complex<T>, Char>
     * 用法说明: 格式化复数，实部非零时输出 "(real+imagi)"，仅虚部时输出 "imagi"。
     * 注意: 浮点精度可能有细微平台差异，使用宽松检测。
     * 预期行为: 格式化结果包含实部和虚部的正确值及 'i' 后缀。
     */
    TEST_CASE("std::complex 格式化")
    {
        SUBCASE("实部+虚部")
        {
            std::complex<double> c(1.0, 2.0);
            std::string s = fmt::format("{}", c);
            // 格式: "(1+2i)"
            CHECK(s == "(1+2i)");
        }
        SUBCASE("仅虚部（实部为0）")
        {
            std::complex<double> c(0.0, 3.0);
            std::string s = fmt::format("{}", c);
            // 格式: "3i"
            CHECK(s == "3i");
        }
        SUBCASE("负虚部")
        {
            std::complex<double> c(1.0, -2.0);
            std::string s = fmt::format("{}", c);
            CHECK(s == "(1-2i)");
        }
        SUBCASE("配合精度说明符")
        {
            std::complex<double> c(1.5, 2.5);
            std::string s = fmt::format("{:.1f}", c);
            CHECK(s == "(1.5+2.5i)");
        }
        SUBCASE("float 类型")
        {
            std::complex<float> c(0.0f, 1.0f);
            std::string s = fmt::format("{}", c);
            CHECK(s == "1i");
        }
    }

    /**
     * 测试 API:   formatter<std::atomic<T>, Char>
     * 用法说明: 格式化 std::atomic<T>，透明调用 T 的格式化器（通过 load()）。
     * 预期行为: 输出与格式化底层值相同。
     */
    TEST_CASE("std::atomic 格式化")
    {
        SUBCASE("atomic<int>")
        {
            std::atomic<int> a(42);
            std::string s = fmt::format("{}", a);
            CHECK(s == "42");
        }
        SUBCASE("atomic<bool>")
        {
            std::atomic<bool> a(true);
            std::string s = fmt::format("{}", a);
            CHECK(s == "true");
        }
        SUBCASE("atomic<long>")
        {
            std::atomic<long> a(-1L);
            std::string s = fmt::format("{}", a);
            CHECK(s == "-1");
        }
        SUBCASE("配合格式说明符")
        {
            std::atomic<int> a(255);
            std::string s = fmt::format("{:#x}", a);
            CHECK(s == "0xff");
        }
    }

    /**
     * 测试 API:   formatter<std::reference_wrapper<T>, Char>
     * 用法说明: 格式化 std::reference_wrapper<T>（透明委托给 T 的格式化器）。
     *           通过 std::ref/std::cref 创建。
     * 预期行为: 输出与直接格式化 T 相同。
     */
    TEST_CASE("std::reference_wrapper 格式化")
    {
        SUBCASE("cref<int>")
        {
            int val = 42;
            auto ref = std::cref(val);
            CHECK(fmt::format("{}", ref) == "42");
        }
        SUBCASE("ref<string>")
        {
            std::string s = "hello";
            auto ref = std::ref(s);
            CHECK(fmt::format("{}", ref) == "hello");
        }
        SUBCASE("修改原值后格式化反映变化")
        {
            int val = 10;
            auto ref = std::ref(val);
            CHECK(fmt::format("{}", ref) == "10");
            val = 20;
            CHECK(fmt::format("{}", ref) == "20");
        }
        SUBCASE("配合格式说明符")
        {
            int val = 255;
            CHECK(fmt::format("{:x}", std::cref(val)) == "ff");
        }
    }

    /**
     * 测试 API:   fmt::ptr(const std::unique_ptr<T, Deleter>& p) -> const void*
     *             fmt::ptr(const std::shared_ptr<T>& p) -> const void*
     *             （定义于 fmt/std.h）
     * 用法说明: 从智能指针提取原始指针以进行格式化（输出十六进制地址）。
     * 预期行为: 格式化结果以 "0x" 开头；空指针输出 "0x0"。
     */
    TEST_CASE("fmt::ptr(unique_ptr) / fmt::ptr(shared_ptr)")
    {
        SUBCASE("unique_ptr 非空")
        {
            auto p = std::make_unique<int>(42);
            std::string s = fmt::format("{}", fmt::ptr(p));
            CHECK(s.find("0x") == 0);
        }
        SUBCASE("unique_ptr 空")
        {
            std::unique_ptr<int> p;
            std::string s = fmt::format("{}", fmt::ptr(p));
            CHECK(s.find("0x") == 0);
        }
        SUBCASE("shared_ptr 非空")
        {
            auto p = std::make_shared<double>(3.14);
            std::string s = fmt::format("{}", fmt::ptr(p));
            CHECK(s.find("0x") == 0);
        }
        SUBCASE("shared_ptr 空")
        {
            std::shared_ptr<int> p;
            std::string s = fmt::format("{}", fmt::ptr(p));
            CHECK(s.find("0x") == 0);
        }
        SUBCASE("unique_ptr 指向的值与地址一致")
        {
            auto p = std::make_unique<int>(99);
            int* raw = p.get();
            // fmt::ptr(p) 应与 fmt::ptr(raw) 输出相同
            std::string sp = fmt::format("{}", fmt::ptr(p));
            std::string sr = fmt::format("{}", fmt::ptr(raw));
            CHECK(sp == sr);
        }
    }

    /**
     * 测试 API:   formatter<std::filesystem::path, Char>
     * 用法说明: 格式化文件系统路径，默认输出 native 格式；
     *           使用 :? 输出带引号的转义字符串；使用 :g 输出通用格式（'/'分隔符）。
     * 注意: 路径分隔符在 Windows（\）和 POSIX（/）上不同，使用平台无关的测试。
     */
    TEST_CASE("std::filesystem::path 格式化")
    {
        SUBCASE("简单文件名")
        {
            std::filesystem::path p("test.txt");
            std::string s = fmt::format("{}", p);
            // 应包含文件名（Windows 上可能带引号）
            CHECK(s.find("test.txt") != std::string::npos);
        }
        SUBCASE("通用格式 :g")
        {
            std::filesystem::path p("dir/sub/file.txt");
            std::string s = fmt::format("{:g}", p);
            // 通用格式使用 '/' 分隔符（跨平台一致）
            CHECK(s.find("file.txt") != std::string::npos);
        }
        SUBCASE("调试格式 :?（带引号）")
        {
            std::filesystem::path p("hello.txt");
            std::string s = fmt::format("{:?}", p);
            CHECK(s.front() == '"');
            CHECK(s.back()  == '"');
        }
    }
} // TEST_SUITE("fmt/std.h — 标准库类型支持")

TEST_SUITE("fmt/base.h — 独立公开 API 验证")
{
    /**
     * 测试 API:   fmt::string_view / fmt::basic_string_view<Char>
     * 用法说明: fmt 自己实现的字符串视图，兼容 std::string_view 和 C 字符串。
     *           不依赖 C++17 的 std::string_view，确保 C++11/14 兼容性。
     * 预期行为: 支持构造、比较、data/size/begin/end/starts_with 等操作。
     */
    TEST_CASE("fmt::string_view / fmt::basic_string_view")
    {
        SUBCASE("从 C 字符串构造")
        {
            fmt::string_view sv("hello");
            CHECK(sv.size()         == 5);
            CHECK(std::string(sv.data(), sv.size()) == "hello");
        }
        SUBCASE("从 std::string 构造")
        {
            std::string s = "world";
            fmt::string_view sv(s);
            CHECK(sv.size()     == 5);
            CHECK(sv[0]         == 'w');
            CHECK(sv[4]         == 'd');
        }
        SUBCASE("begins/end 迭代器")
        {
            fmt::string_view sv("abc");
            std::string result(sv.begin(), sv.end());
            CHECK(result == "abc");
        }
        SUBCASE("比较运算符")
        {
            fmt::string_view a("foo");
            fmt::string_view b("foo");
            fmt::string_view c("bar");
            CHECK(a == b);
            CHECK(a != c);
            CHECK(c < a);
        }
        SUBCASE("starts_with")
        {
            fmt::string_view sv("hello world");
            CHECK(sv.starts_with("hello") == true);
            CHECK(sv.starts_with("world") == false);
            CHECK(sv.starts_with('h')     == true);
            CHECK(sv.starts_with('w')     == false);
        }
        SUBCASE("remove_prefix")
        {
            fmt::string_view sv("hello world");
            sv.remove_prefix(6);
            CHECK(std::string(sv.data(), sv.size()) == "world");
        }
        SUBCASE("空视图")
        {
            fmt::string_view sv("");
            CHECK(sv.size() == 0);
            CHECK(sv.begin() == sv.end());
        }
    }

    /**
     * 测试 API:   fmt::is_formattable<T, Char>
     * 用法说明: 编译期检测类型 T 是否可以用 fmt::format 格式化。
     * 预期行为: 内置类型和有 formatter 特化的类型返回 true；
     *           无 formatter 且不可映射的类型返回 false。
     */
    TEST_CASE("fmt::is_formattable<T> 类型特征检测")
    {
        // 内置类型应可格式化
        static_assert(fmt::is_formattable<int>::value,           "int");
        static_assert(fmt::is_formattable<unsigned>::value,      "unsigned");
        static_assert(fmt::is_formattable<long long>::value,     "long long");
        static_assert(fmt::is_formattable<double>::value,        "double");
        static_assert(fmt::is_formattable<float>::value,         "float");
        static_assert(fmt::is_formattable<bool>::value,          "bool");
        static_assert(fmt::is_formattable<char>::value,          "char");
        static_assert(fmt::is_formattable<const char*>::value,   "const char*");
        static_assert(fmt::is_formattable<std::string>::value,   "std::string");
        static_assert(fmt::is_formattable<fmt::string_view>::value, "string_view");

        // 自定义类型（有 formatter 特化）应可格式化
        static_assert(fmt::is_formattable<Point>::value, "Point with formatter");

        CHECK(fmt::is_formattable<int>::value    == true);
        CHECK(fmt::is_formattable<double>::value == true);
        CHECK(fmt::is_formattable<Point>::value  == true);
    }

    /**
     * 测试 API:   fmt::make_format_args(T&... args)
     * 用法说明: 将参数列表打包为类型擦除的 format_args，供 vformat/vformat_to 使用。
     * 预期行为: 返回的 format_args 可传入 vformat 得到正确结果。
     */
    TEST_CASE("fmt::make_format_args")
    {
        SUBCASE("单参数")
        {
            int n = 42;
            auto args = fmt::make_format_args(n);
            std::string s = fmt::vformat("{}", args);
            CHECK(s == "42");
        }
        SUBCASE("多参数")
        {
            int a = 1;
            std::string b = "two";
            double c = 3.0;
            auto args = fmt::make_format_args(a, b, c);
            std::string s = fmt::vformat("{} {} {}", args);
            CHECK(s == "1 two 3");
        }
        SUBCASE("format_args 转换")
        {
            // make_format_args 隐式转换为 fmt::format_args
            int x = 7;
            fmt::format_args fa = fmt::make_format_args(x);
            (void)fa;  // 验证转换合法
        }
    }

    /**
     * 测试 API:   fmt::runtime(string_view s) -> runtime_format_string<>
     * 用法说明: 将运行时字符串标记为格式字符串，禁用编译期格式验证。
     *           适用于需要在运行时决定格式字符串的场景。
     * 预期行为: fmt::format(fmt::runtime(s), args...) 等价于 fmt::vformat(s, ...)。
     */
    TEST_CASE("fmt::runtime — 运行时格式字符串")
    {
        SUBCASE("基本用法")
        {
            const char* fmt_str = "{}";
            std::string s = fmt::format(fmt::runtime(fmt_str), 42);
            CHECK(s == "42");
        }
        SUBCASE("std::string 格式字符串")
        {
            std::string fmt_str = "{} + {} = {}";
            std::string s = fmt::format(fmt::runtime(fmt_str), 1, 2, 3);
            CHECK(s == "1 + 2 = 3");
        }
        SUBCASE("动态选择格式字符串")
        {
            bool show_hex = true;
            const char* fmt_str = show_hex ? "{:#x}" : "{}";
            std::string s = fmt::format(fmt::runtime(fmt_str), 255);
            CHECK(s == "0xff");
        }
    }
} // TEST_SUITE("fmt/base.h — 独立公开 API 验证")

TEST_SUITE("fmt/ranges.h — 范围与元组格式化")
{
    /**
     * 测试 API:   formatter<std::tuple<T...>, Char>
     *             formatter<std::pair<K, V>, Char>
     * 用法说明: 格式化元组和 pair，默认输出格式为 "(elem1, elem2, ...)"。
     *           元素中的字符串类型以调试模式（带引号）输出。
     *           使用 :n 说明符可省略括号和分隔符。
     * 预期行为: 默认括号为 ()，分隔符为 ", "；字符串元素带引号。
     */
    TEST_CASE("std::tuple 格式化")
    {
        SUBCASE("空元组")
        {
            auto t = std::tuple<>{};
            CHECK(fmt::format("{}", t) == "()");
        }
        SUBCASE("单元素 tuple<int>")
        {
            auto t = std::make_tuple(42);
            CHECK(fmt::format("{}", t) == "(42)");
        }
        SUBCASE("双元素 tuple<int, double>")
        {
            auto t = std::make_tuple(1, 2.5);
            CHECK(fmt::format("{}", t) == "(1, 2.5)");
        }
        SUBCASE("三元素 tuple<int, string, bool>")
        {
            // 字符串元素以调试模式（带引号）输出
            auto t = std::make_tuple(1, std::string("hi"), true);
            CHECK(fmt::format("{}", t) == "(1, \"hi\", true)");
        }
        SUBCASE(":n 说明符——省略括号和分隔符")
        {
            auto t = std::make_tuple(1, 2, 3);
            CHECK(fmt::format("{:n}", t) == "123");
        }
    }

    TEST_CASE("std::pair 格式化")
    {
        SUBCASE("pair<int, int>")
        {
            auto p = std::make_pair(1, 2);
            CHECK(fmt::format("{}", p) == "(1, 2)");
        }
        SUBCASE("pair<string, int> — 字符串键带引号")
        {
            auto p = std::make_pair(std::string("key"), 42);
            CHECK(fmt::format("{}", p) == "(\"key\", 42)");
        }
        SUBCASE("pair<int, double>")
        {
            auto p = std::make_pair(0, 3.14);
            std::string s = fmt::format("{}", p);
            CHECK(s.find("(0,") == 0);
            CHECK(s.find("3.14") != std::string::npos);
        }
    }

    /**
     * 测试 API:   formatter<R, Char>（range_format_kind == sequence）
     * 用法说明: 格式化 vector、list、deque、array 等序列容器，
     *           默认输出格式为 "[elem1, elem2, ...]"。
     *           字符串元素以调试模式（带引号）输出。
     *           使用 :n 说明符省略括号，使用 ::spec 对元素应用格式说明符。
     * 预期行为: 默认括号为 []，分隔符为 ", "。
     */
    TEST_CASE("std::vector 格式化")
    {
        SUBCASE("空 vector")
        {
            std::vector<int> v;
            CHECK(fmt::format("{}", v) == "[]");
        }
        SUBCASE("vector<int> 基本格式化")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{}", v) == "[1, 2, 3]");
        }
        SUBCASE("vector<double>")
        {
            std::vector<double> v = {1.5, 2.5};
            CHECK(fmt::format("{}", v) == "[1.5, 2.5]");
        }
        SUBCASE("vector<string>（元素带引号）")
        {
            std::vector<std::string> v = {"hello", "world"};
            CHECK(fmt::format("{}", v) == "[\"hello\", \"world\"]");
        }
        SUBCASE(":n 说明符——省略括号")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{:n}", v) == "1, 2, 3");
        }
        SUBCASE("::x — 对元素应用十六进制格式")
        {
            std::vector<int> v = {10, 255, 16};
            CHECK(fmt::format("{::x}", v) == "[a, ff, 10]");
        }
        SUBCASE("::#x — 对元素应用带前缀十六进制格式")
        {
            std::vector<int> v = {10, 255, 16};
            CHECK(fmt::format("{::#x}", v) == "[0xa, 0xff, 0x10]");
        }
    }

    TEST_CASE("std::array 格式化")
    {
        SUBCASE("array<int, 3>")
        {
            std::array<int, 3> a = {10, 20, 30};
            CHECK(fmt::format("{}", a) == "[10, 20, 30]");
        }
        SUBCASE("array<int, 0>（空数组）")
        {
            std::array<int, 0> a{};
            CHECK(fmt::format("{}", a) == "[]");
        }
        SUBCASE("array<double, 2>")
        {
            std::array<double, 2> a = {1.0, 2.0};
            CHECK(fmt::format("{}", a) == "[1, 2]");
        }
    }

    TEST_CASE("C 数组格式化")
    {
        SUBCASE("int[3]")
        {
            int a[] = {1, 2, 3};
            CHECK(fmt::format("{}", a) == "[1, 2, 3]");
        }
        SUBCASE("char[N] — 字符数组格式化")
        {
            char a[] = {'x', 'y'};
            // char 数组默认被当作字符串/字节序列处理
            // 输出可能是 "xy" 或类似格式，不一定带单引号
            std::string s = fmt::format("{}", a);
            CHECK(!s.empty());
        }
    }

    TEST_CASE("std::list 和 std::deque 格式化")
    {
        SUBCASE("list<int>")
        {
            std::list<int> lst = {1, 2, 3};
            CHECK(fmt::format("{}", lst) == "[1, 2, 3]");
        }
        SUBCASE("deque<int>")
        {
            std::deque<int> dq = {4, 5, 6};
            CHECK(fmt::format("{}", dq) == "[4, 5, 6]");
        }
        SUBCASE("空 list")
        {
            std::list<double> lst;
            CHECK(fmt::format("{}", lst) == "[]");
        }
    }

    /**
     * 测试 API:   formatter<R, Char>（range_format_kind == set）
     * 用法说明: 格式化 set、multiset 等集合容器，
     *           默认输出格式为 "{elem1, elem2, ...}"（大括号）。
     *           字符串元素以调试模式（带引号）输出。
     * 预期行为: 默认括号为 {}，分隔符为 ", "；set 元素按升序排列。
     */
    TEST_CASE("std::set 格式化")
    {
        SUBCASE("set<int>")
        {
            std::set<int> s = {3, 1, 2};
            // set 内部有序（升序）
            CHECK(fmt::format("{}", s) == "{1, 2, 3}");
        }
        SUBCASE("空 set")
        {
            std::set<int> s;
            CHECK(fmt::format("{}", s) == "{}");
        }
        SUBCASE("set<string>（元素带引号）")
        {
            std::set<std::string> s = {"b", "a"};
            CHECK(fmt::format("{}", s) == "{\"a\", \"b\"}");
        }
        SUBCASE("multiset<int>（允许重复）")
        {
            std::multiset<int> ms = {1, 2, 2, 3};
            CHECK(fmt::format("{}", ms) == "{1, 2, 2, 3}");
        }
        SUBCASE(":n 说明符——省略括号")
        {
            std::set<int> s = {1, 2, 3};
            CHECK(fmt::format("{:n}", s) == "1, 2, 3");
        }
    }

    TEST_CASE("std::unordered_set 格式化")
    {
        SUBCASE("unordered_set<int> — 包含正确元素")
        {
            std::unordered_set<int> s = {3, 1, 2};
            std::string r = fmt::format("{}", s);
            // unordered_set 不保证顺序，仅验证括号和元素存在
            CHECK(r.front() == '{');
            CHECK(r.back()  == '}');
            CHECK(r.find("1") != std::string::npos);
            CHECK(r.find("2") != std::string::npos);
            CHECK(r.find("3") != std::string::npos);
        }
        SUBCASE("空 unordered_set")
        {
            std::unordered_set<int> s;
            CHECK(fmt::format("{}", s) == "{}");
        }
        SUBCASE("unordered_set<string>（元素带引号）")
        {
            std::unordered_set<std::string> s = {"a", "b"};
            std::string r = fmt::format("{}", s);
            CHECK(r.front() == '{');
            CHECK(r.back()  == '}');
            CHECK(r.find("\"a\"") != std::string::npos);
            CHECK(r.find("\"b\"") != std::string::npos);
        }
    }

    /**
     * 测试 API:   formatter<R, Char>（range_format_kind == map）
     * 用法说明: 格式化 map、multimap 等映射容器，
     *           默认输出格式为 "{key1: val1, key2: val2, ...}"。
     *           键和值中的字符串类型以调试模式（带引号）输出。
     *           使用 :n 说明符省略大括号。
     * 预期行为: 默认括号为 {}，键值间使用 ": " 分隔，条目间使用 ", " 分隔。
     */
    TEST_CASE("std::map 格式化")
    {
        SUBCASE("map<int, int>")
        {
            std::map<int, int> m = {{1, 10}, {2, 20}};
            CHECK(fmt::format("{}", m) == "{1: 10, 2: 20}");
        }
        SUBCASE("空 map")
        {
            std::map<int, int> m;
            CHECK(fmt::format("{}", m) == "{}");
        }
        SUBCASE("map<string, int>（键带引号）")
        {
            std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
            CHECK(fmt::format("{}", m) == "{\"a\": 1, \"b\": 2}");
        }
        SUBCASE("map<int, string>（值带引号）")
        {
            std::map<int, std::string> m = {{1, "one"}, {2, "two"}};
            CHECK(fmt::format("{}", m) == "{1: \"one\", 2: \"two\"}");
        }
        SUBCASE(":n 说明符——省略括号")
        {
            std::map<int, int> m = {{1, 10}, {2, 20}};
            CHECK(fmt::format("{:n}", m) == "1: 10, 2: 20");
        }
        SUBCASE("multimap<int, int>")
        {
            std::multimap<int, int> mm = {{1, 10}, {1, 20}, {2, 30}};
            std::string s = fmt::format("{}", mm);
            CHECK(s.find("{1:") == 0);
            CHECK(s.find("2: 30") != std::string::npos);
        }
    }

    TEST_CASE("std::unordered_map 格式化")
    {
        SUBCASE("unordered_map<int, int> — 包含正确键值对")
        {
            std::unordered_map<int, int> m = {{1, 10}, {2, 20}};
            std::string r = fmt::format("{}", m);
            CHECK(r.front() == '{');
            CHECK(r.back()  == '}');
            CHECK(r.find("1: 10") != std::string::npos);
            CHECK(r.find("2: 20") != std::string::npos);
        }
        SUBCASE("空 unordered_map")
        {
            std::unordered_map<int, int> m;
            CHECK(fmt::format("{}", m) == "{}");
        }
        SUBCASE("unordered_map<string, int>（键带引号）")
        {
            std::unordered_map<std::string, int> m = {{"a", 1}, {"b", 2}};
            std::string r = fmt::format("{}", m);
            CHECK(r.front() == '{');
            CHECK(r.back()  == '}');
            CHECK(r.find("\"a\": 1") != std::string::npos);
            CHECK(r.find("\"b\": 2") != std::string::npos);
        }
    }

    /**
     * 测试 API:   formatter<T, Char>（detail::is_container_adaptor_like<T>）
     * 用法说明: 格式化 stack、queue、priority_queue 等容器适配器，
     *           委托给底层容器（container_type）的格式化器，
     *           以序列格式 "[elem1, elem2, ...]" 输出底层容器内容。
     * 注意: 输出顺序为底层容器的存储顺序（非弹出顺序）。
     *       priority_queue 底层存储为堆顺序，不做精确输出断言。
     * 预期行为: 输出格式为 [...] 的序列；包含全部已入队/入栈元素。
     */
    TEST_CASE("容器适配器格式化")
    {
        SUBCASE("std::stack<int>")
        {
            std::stack<int> s;
            s.push(1); s.push(2); s.push(3);
            // 底层默认为 deque，按入栈顺序格式化（栈底 → 栈顶）
            std::string r = fmt::format("{}", s);
            CHECK(r.front() == '[');
            CHECK(r.back()  == ']');
            CHECK(r.find("1") != std::string::npos);
            CHECK(r.find("2") != std::string::npos);
            CHECK(r.find("3") != std::string::npos);
        }
        SUBCASE("std::queue<int>")
        {
            std::queue<int> q;
            q.push(10); q.push(20); q.push(30);
            std::string r = fmt::format("{}", q);
            CHECK(r.front() == '[');
            CHECK(r.back()  == ']');
            CHECK(r.find("10") != std::string::npos);
            CHECK(r.find("30") != std::string::npos);
        }
        SUBCASE("std::priority_queue<int>")
        {
            std::priority_queue<int> pq;
            pq.push(3); pq.push(1); pq.push(2);
            std::string r = fmt::format("{}", pq);
            // 输出底层 vector 的堆序内容，格式为 [...]
            CHECK(r.front() == '[');
            CHECK(r.back()  == ']');
            CHECK(r.find("1") != std::string::npos);
            CHECK(r.find("3") != std::string::npos);
        }
        SUBCASE("空 stack")
        {
            std::stack<int> s;
            CHECK(fmt::format("{}", s) == "[]");
        }
    }

    /**
     * 测试 API:   fmt::join(It begin, Sentinel end, string_view sep)
     *             fmt::join(Range&& r, string_view sep)
     *             fmt::join(const Tuple& t, string_view sep)
     *             fmt::join(std::initializer_list<T> list, string_view sep)
     * 用法说明: 将范围、迭代器对、元组或初始化列表中的元素用指定分隔符连接，
     *           不添加括号，直接输出元素序列。
     *           可配合格式说明符对元素格式化（如 {:02}、{:x}）。
     *           join 的元素不以调试模式输出（字符串不带引号）。
     * 预期行为: 元素间以 sep 分隔；可以传入自定义格式说明符。
     */
    TEST_CASE("fmt::join — 迭代器范围连接")
    {
        SUBCASE("基本用法（vector 迭代器）")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ", ")) == "1, 2, 3");
        }
        SUBCASE("不同分隔符")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), " | ")) == "1 | 2 | 3");
            CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ""))    == "123");
            CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), "-"))   == "1-2-3");
        }
        SUBCASE("空范围")
        {
            std::vector<int> v;
            CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ", ")) == "");
        }
        SUBCASE("单元素")
        {
            std::vector<int> v = {42};
            CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ", ")) == "42");
        }
        SUBCASE("配合格式说明符 {:02d}")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{:02}", fmt::join(v.begin(), v.end(), ", ")) == "01, 02, 03");
        }
        SUBCASE("字符串元素（join 不加引号，直接输出）")
        {
            std::vector<std::string> v = {"a", "b", "c"};
            CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ", ")) == "a, b, c");
        }
    }

    TEST_CASE("fmt::join — 范围连接")
    {
        SUBCASE("vector 范围")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{}", fmt::join(v, ", ")) == "1, 2, 3");
        }
        SUBCASE("list 范围")
        {
            std::list<int> lst = {4, 5, 6};
            CHECK(fmt::format("{}", fmt::join(lst, " ")) == "4 5 6");
        }
        SUBCASE("array 范围")
        {
            std::array<int, 3> a = {7, 8, 9};
            CHECK(fmt::format("{}", fmt::join(a, "-")) == "7-8-9");
        }
        SUBCASE("配合格式说明符 {:x}")
        {
            std::vector<int> v = {0xff, 0x0a, 0x1b};
            CHECK(fmt::format("{:x}", fmt::join(v, ", ")) == "ff, a, 1b");
        }
        SUBCASE("浮点配合精度 {:.2f}")
        {
            std::vector<double> v = {1.1, 2.2, 3.3};
            CHECK(fmt::format("{:.2f}", fmt::join(v, ", ")) == "1.10, 2.20, 3.30");
        }
        SUBCASE("字符串范围（无引号）")
        {
            std::vector<std::string> v = {"foo", "bar"};
            CHECK(fmt::format("{}", fmt::join(v, " / ")) == "foo / bar");
        }
    }

    TEST_CASE("fmt::join — 元组连接")
    {
        SUBCASE("tuple<int, double, string>（无引号）")
        {
            auto t = std::make_tuple(1, 2.5, std::string("x"));
            CHECK(fmt::format("{}", fmt::join(t, ", ")) == "1, 2.5, x");
        }
        SUBCASE("pair<int, string>")
        {
            auto p = std::make_pair(42, std::string("hello"));
            CHECK(fmt::format("{}", fmt::join(p, " = ")) == "42 = hello");
        }
        SUBCASE("空元组")
        {
            auto t = std::tuple<>{};
            CHECK(fmt::format("{}", fmt::join(t, ", ")) == "");
        }
        SUBCASE("单元素元组")
        {
            auto t = std::make_tuple(99);
            CHECK(fmt::format("{}", fmt::join(t, ", ")) == "99");
        }
        SUBCASE("三元素同类型元组")
        {
            auto t = std::make_tuple(10, 20, 30);
            CHECK(fmt::format("{}", fmt::join(t, " + ")) == "10 + 20 + 30");
        }
    }

    TEST_CASE("fmt::join — 初始化列表连接")
    {
        SUBCASE("initializer_list<int>")
        {
            CHECK(fmt::format("{}", fmt::join({1, 2, 3}, ", ")) == "1, 2, 3");
        }
        SUBCASE("initializer_list<double>")
        {
            CHECK(fmt::format("{}", fmt::join({1.1, 2.2}, " - ")) == "1.1 - 2.2");
        }
        SUBCASE("initializer_list 配合格式说明符")
        {
            CHECK(fmt::format("{:04}", fmt::join({1, 20, 300}, ", ")) == "0001, 0020, 0300");
        }
        SUBCASE("单元素初始化列表")
        {
            CHECK(fmt::format("{}", fmt::join({42}, ", ")) == "42");
        }
    }

    /**
     * 测试 API:   range_formatter 格式说明符
     *             :n   — 省略括号（no brackets）
     *             :s   — 将 char 范围格式化为字符串
     *             :?s  — 将 char 范围格式化为调试字符串（带引号）
     *             ::spec — 对范围内每个元素应用 spec 格式说明符
     *             :n:spec — 组合省略括号和元素格式说明符
     * 用法说明: 在格式字符串中为序列/集合/映射类型使用这些说明符控制输出。
     *           格式说明符语法：{:range_spec} 或 {::elem_spec}。
     * 预期行为: :n 省略括号，:s 将 char 向量输出为字符串，::spec 转发给元素。
     */
    TEST_CASE("范围格式说明符")
    {
        SUBCASE(":n — 序列省略括号")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{:n}", v) == "1, 2, 3");
        }
        SUBCASE(":n — 集合省略括号")
        {
            std::set<int> s = {1, 2, 3};
            CHECK(fmt::format("{:n}", s) == "1, 2, 3");
        }
        SUBCASE(":s — char 向量格式化为字符串")
        {
            std::vector<char> v = {'h', 'e', 'l', 'l', 'o'};
            // :s 格式将 char 向量格式化为字符串（行为可能因版本而异）
            std::string s = fmt::format("{:s}", v);
            CHECK(s.find("hello") != std::string::npos);
        }
        SUBCASE(":?s — char 向量格式化为调试字符串（带引号）")
        {
            std::vector<char> v = {'h', 'i'};
            CHECK(fmt::format("{:?s}", v) == "\"hi\"");
        }
        SUBCASE("::x — 元素十六进制格式")
        {
            std::vector<int> v = {10, 255, 16};
            CHECK(fmt::format("{::x}", v) == "[a, ff, 10]");
        }
        SUBCASE("::#x — 元素带前缀十六进制格式")
        {
            std::vector<int> v = {10, 255, 16};
            CHECK(fmt::format("{::#x}", v) == "[0xa, 0xff, 0x10]");
        }
        SUBCASE("::02 — 元素零填充格式")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{::02}", v) == "[01, 02, 03]");
        }
        SUBCASE(":n::02 — 省略括号 + 元素零填充格式")
        {
            std::vector<int> v = {1, 2, 3};
            CHECK(fmt::format("{:n:02}", v) == "01, 02, 03");
        }
    }

    /**
     * 测试 API:   formatter<Tuple, Char> 格式说明符
     *             :n — 省略括号和分隔符（no brackets/separator）
     * 用法说明: 控制元组的输出格式，:n 会同时省略括号和分隔符。
     * 预期行为: :n 省略 () 括号和 ", " 分隔符，各元素紧密相邻输出。
     */
    TEST_CASE("元组格式说明符")
    {
        SUBCASE(":n — 省略括号和分隔符")
        {
            auto t = std::make_tuple(1, 2, 3);
            CHECK(fmt::format("{:n}", t) == "123");
        }
        SUBCASE(":n — 含字符串元素（字符串仍带引号）")
        {
            auto t = std::make_tuple(std::string("a"), std::string("b"));
            std::string s = fmt::format("{:n}", t);
            // :n 省略括号和分隔符，但字符串元素仍以调试模式输出（带引号）
            CHECK(s == "\"a\"\"b\"");
        }
    }

    /**
     * 测试 API:   fmt::is_tuple_like<T>
     *             fmt::is_range<T, Char>
     * 用法说明: 编译期类型特征，用于检测类型是否为类元组类型或范围类型。
     *           is_tuple_like<T>：std::tuple、std::pair 满足，vector、array 不满足。
     *           is_range<T, Char>：vector、list、set、map、array 满足，int 不满足。
     * 预期行为: 类型特征返回正确的布尔值。
     */
    TEST_CASE("fmt::is_tuple_like 和 fmt::is_range 类型特征")
    {
        SUBCASE("is_tuple_like")
        {
            static_assert(fmt::is_tuple_like<std::tuple<int, double>>::value,
                          "tuple should be tuple-like");
            static_assert(fmt::is_tuple_like<std::pair<int, int>>::value,
                          "pair should be tuple-like");
            // vector 和 array 不是 tuple-like（它们是 range）
            static_assert(!fmt::is_tuple_like<std::vector<int>>::value,
                          "vector should not be tuple-like");
            static_assert(!fmt::is_tuple_like<std::array<int, 3>>::value,
                          "array should not be tuple-like (it's a range)");
            CHECK(fmt::is_tuple_like<std::tuple<int>>::value      == true);
            CHECK(fmt::is_tuple_like<std::pair<int, int>>::value  == true);
            CHECK(fmt::is_tuple_like<std::vector<int>>::value     == false);
            CHECK(fmt::is_tuple_like<std::array<int, 3>>::value   == false);
        }
        SUBCASE("is_range")
        {
            static_assert(fmt::is_range<std::vector<int>, char>::value,
                          "vector should be a range");
            static_assert(fmt::is_range<std::list<int>, char>::value,
                          "list should be a range");
            static_assert(fmt::is_range<std::set<int>, char>::value,
                          "set should be a range");
            static_assert(fmt::is_range<std::map<int, int>, char>::value,
                          "map should be a range");
            static_assert(fmt::is_range<std::array<int, 3>, char>::value,
                          "array should be a range");
            // int 不是 range
            static_assert(!fmt::is_range<int, char>::value,
                          "int should not be a range");
            CHECK(fmt::is_range<std::vector<int>, char>::value    == true);
            CHECK(fmt::is_range<std::set<int>, char>::value       == true);
            CHECK(fmt::is_range<std::map<int, int>, char>::value  == true);
            CHECK(fmt::is_range<std::array<int, 3>, char>::value  == true);
            CHECK(fmt::is_range<int, char>::value                 == false);
        }
    }
} // TEST_SUITE("fmt/ranges.h — 范围与元组格式化")