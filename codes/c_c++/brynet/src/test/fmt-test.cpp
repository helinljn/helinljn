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
 * [fmt/format-inl.h] — 说明
 *   ✗ format-inl.h 是 format.h 的内联实现文件，不应直接包含（FMT_HEADER_ONLY
 *     模式除外）。其中没有用户可直接调用的独立公开 API，所有功能已通过
 *     fmt/format.h 暴露，相关测试已在 format.h 测试套件中覆盖。
 *
 * [fmt/base.h] — 说明
 *   base.h 提供基础类型和基础 API 的底层实现，大部分是 detail:: 命名空间下的
 *   内部实现，或通过 format.h 重新导出。独立的用户可调用公开 API 已在上方
 *   [fmt/base.h] 条目中列出。
 *
 * [fmt/args.h] — 动态参数存储
 *   ✓ fmt::dynamic_format_arg_store::push_back（位置参数）
 *   ✓ fmt::dynamic_format_arg_store::push_back（命名参数）
 *   ✓ fmt::dynamic_format_arg_store::push_back（引用包装）
 *   ✓ fmt::dynamic_format_arg_store::clear
 *   ✓ fmt::dynamic_format_arg_store::reserve
 *   ✓ fmt::dynamic_format_arg_store::size
 *   ✓ 隐式转换为 fmt::format_args 并传入 vformat
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
#include "core/core_port.h"
#include "core/common.h"
#include "fmt/format.h"
#include "fmt/chrono.h"
#include "fmt/args.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #pragma warning(push)
    #pragma warning(disable:4127)
    #pragma warning(disable:4702)
#endif // defined(CORE_PLATFORM_WINDOWS)

#include "fmt/compile.h"
#include "fmt/ranges.h"
#include "fmt/std.h"

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

DOCTEST_TEST_SUITE("fmt/format.h — 核心格式化功能")
{
    /**
     * 测试 API:   fmt::format(format_string<T...>, T&&...)
     * 用法说明: 最基础的格式化接口，将参数按格式字符串渲染为 std::string。
     * 预期行为: 按照 {} 占位符依次替换参数。
     */
    DOCTEST_TEST_CASE("fmt::format — 基本占位符替换")
    {
        // 无参数格式
        DOCTEST_SUBCASE("无参数格式字符串")
        {
            std::string s = fmt::format("hello world");
            DOCTEST_CHECK(s == "hello world");
        }
        // 单个整数参数
        DOCTEST_SUBCASE("单个整数参数")
        {
            std::string s = fmt::format("The answer is {}.", 42);
            DOCTEST_CHECK(s == "The answer is 42.");
        }
        // 多个参数
        DOCTEST_SUBCASE("多个参数依次替换")
        {
            std::string s = fmt::format("{} + {} = {}", 1, 2, 3);
            DOCTEST_CHECK(s == "1 + 2 = 3");
        }
        // 转义花括号
        DOCTEST_SUBCASE("转义花括号 {{ 和 }}")
        {
            std::string s = fmt::format("{{}} means empty");
            DOCTEST_CHECK(s == "{} means empty");
        }
        // 手动位置索引
        DOCTEST_SUBCASE("手动位置索引 {0} {1}")
        {
            std::string s = fmt::format("{0} and {1} and {0}", "a", "b");
            DOCTEST_CHECK(s == "a and b and a");
        }
    }

    /**
     * 测试 API:   fmt::format 整数类型
     * 用法说明: 测试各种整数类型及格式说明符（十进制、十六进制、八进制、二进制）。
     * 预期行为: 输出符合标准格式规范。
     */
    DOCTEST_TEST_CASE("fmt::format — 整数类型格式化")
    {
        DOCTEST_SUBCASE("有符号整数")
        {
            DOCTEST_CHECK(fmt::format("{}", 0) == "0");
            DOCTEST_CHECK(fmt::format("{}", -1) == "-1");
            DOCTEST_CHECK(fmt::format("{}", 2147483647) == "2147483647");
            DOCTEST_CHECK(fmt::format("{}", -2147483648LL) == "-2147483648");
        }
        DOCTEST_SUBCASE("无符号整数")
        {
            DOCTEST_CHECK(fmt::format("{}", 0u) == "0");
            DOCTEST_CHECK(fmt::format("{}", 42u) == "42");
            DOCTEST_CHECK(fmt::format("{}", 4294967295u) == "4294967295");
        }
        DOCTEST_SUBCASE("long long / unsigned long long")
        {
            DOCTEST_CHECK(fmt::format("{}", 9223372036854775807LL) == "9223372036854775807");
            DOCTEST_CHECK(fmt::format("{}", 18446744073709551615ULL) == "18446744073709551615");
        }
        DOCTEST_SUBCASE("十六进制 :x :X")
        {
            DOCTEST_CHECK(fmt::format("{:x}",  255) == "ff");
            DOCTEST_CHECK(fmt::format("{:X}",  255) == "FF");
            DOCTEST_CHECK(fmt::format("{:#x}", 255) == "0xff");
            DOCTEST_CHECK(fmt::format("{:#X}", 255) == "0XFF");
        }
        DOCTEST_SUBCASE("八进制 :o")
        {
            DOCTEST_CHECK(fmt::format("{:o}",  8) == "10");
            DOCTEST_CHECK(fmt::format("{:#o}", 8) == "010");
        }
        DOCTEST_SUBCASE("二进制 :b :B")
        {
            DOCTEST_CHECK(fmt::format("{:b}",  10) == "1010");
            DOCTEST_CHECK(fmt::format("{:#b}", 10) == "0b1010");
            DOCTEST_CHECK(fmt::format("{:#B}", 10) == "0B1010");
        }
        DOCTEST_SUBCASE("字符类型 :c")
        {
            DOCTEST_CHECK(fmt::format("{:c}", 65) == "A");
            DOCTEST_CHECK(fmt::format("{:c}", 97) == "a");
        }
        DOCTEST_SUBCASE("符号 :+ :space")
        {
            DOCTEST_CHECK(fmt::format("{:+}", 42)  == "+42");
            DOCTEST_CHECK(fmt::format("{:+}", -42) == "-42");
            DOCTEST_CHECK(fmt::format("{: }", 42)  == " 42");
        }
    }

    /**
     * 测试 API:   fmt::format 浮点类型
     * 用法说明: 测试 float/double/long double 的各种格式说明符。
     * 预期行为: 按照 IEEE 754 规范格式化。
     */
    DOCTEST_TEST_CASE("fmt::format — 浮点类型格式化")
    {
        DOCTEST_SUBCASE("默认浮点格式（最短表示）")
        {
            DOCTEST_CHECK(fmt::format("{}", 1.0)   == "1");
            DOCTEST_CHECK(fmt::format("{}", 1.5)   == "1.5");
            DOCTEST_CHECK(fmt::format("{}", 0.1)   == "0.1");
        }
        DOCTEST_SUBCASE("固定小数点 :f")
        {
            DOCTEST_CHECK(fmt::format("{:.2f}", 3.14159) == "3.14");
            DOCTEST_CHECK(fmt::format("{:.0f}", 2.5)     == "2");
            DOCTEST_CHECK(fmt::format("{:.4f}", 1.0)     == "1.0000");
        }
        DOCTEST_SUBCASE("科学计数法 :e :E")
        {
            DOCTEST_CHECK(fmt::format("{:.2e}", 1234.5) == "1.23e+03");
            DOCTEST_CHECK(fmt::format("{:.2E}", 1234.5) == "1.23E+03");
        }
        DOCTEST_SUBCASE("通用格式 :g :G")
        {
            // :g 移除末尾零，选择较短的 f/e 形式
            std::string r = fmt::format("{:g}", 1.0);
            DOCTEST_CHECK(r == "1");
        }
        DOCTEST_SUBCASE("特殊浮点值 nan / inf")
        {
            double nan_val = std::numeric_limits<double>::quiet_NaN();
            double inf_val = std::numeric_limits<double>::infinity();
            std::string snan = fmt::format("{}", nan_val);
            std::string sinf = fmt::format("{}", inf_val);
            // fmt 输出小写 "nan"/"inf"，但部分平台可能大写，统一转小写比较
            std::string snan_lower = core::to_lower(snan);
            std::string sinf_lower = core::to_lower(sinf);
            DOCTEST_CHECK(snan_lower == "nan");
            DOCTEST_CHECK(sinf_lower == "inf");

            std::string sneg = fmt::format("{}", -inf_val);
            std::string sneg_lower = core::to_lower(sneg);
            DOCTEST_CHECK(sneg_lower == "-inf");
        }
        DOCTEST_SUBCASE("十六进制浮点 :a :A")
        {
            // 仅检查格式合法，不检查精确值（平台差异可能存在）
            std::string r = fmt::format("{:a}", 1.0);
            DOCTEST_CHECK(r.find("0x") != std::string::npos);
        }
    }

    /**
     * 测试 API:   fmt::format 字符串与字符
     * 用法说明: 测试 C 字符串、std::string、string_view 以及字符类型。
     * 预期行为: 直接输出字符串内容。
     */
    DOCTEST_TEST_CASE("fmt::format — 字符串与字符格式化")
    {
        DOCTEST_SUBCASE("C 字符串")
        {
            DOCTEST_CHECK(fmt::format("{}", "hello") == "hello");
            DOCTEST_CHECK(fmt::format("{}", "")      == "");
        }
        DOCTEST_SUBCASE("std::string")
        {
            std::string s = "world";
            DOCTEST_CHECK(fmt::format("{}", s) == "world");
        }
        DOCTEST_SUBCASE("fmt::string_view / std::string_view")
        {
            fmt::string_view sv = "fmtlib";
            DOCTEST_CHECK(fmt::format("{}", sv) == "fmtlib");
        }
        DOCTEST_SUBCASE("字符 char")
        {
            DOCTEST_CHECK(fmt::format("{}", 'A') == "A");
            DOCTEST_CHECK(fmt::format("{}", '\n') == "\n");
        }
        DOCTEST_SUBCASE("bool")
        {
            DOCTEST_CHECK(fmt::format("{}", true)  == "true");
            DOCTEST_CHECK(fmt::format("{}", false) == "false");
            // 整数格式显示 0/1
            DOCTEST_CHECK(fmt::format("{:d}", true)  == "1");
            DOCTEST_CHECK(fmt::format("{:d}", false) == "0");
        }
        DOCTEST_SUBCASE("字符串精度截断 :.N")
        {
            DOCTEST_CHECK(fmt::format("{:.3}", "hello") == "hel");
        }
        DOCTEST_SUBCASE("调试格式 :?（显示转义）")
        {
            // 调试模式下字符串带引号并转义特殊字符
            std::string r = fmt::format("{:?}", "hello\n");
            DOCTEST_CHECK(r == "\"hello\\n\"");
        }
    }

    /**
     * 测试 API:   fmt::format 对齐与填充
     * 用法说明: 测试左对齐、右对齐、居中对齐，以及自定义填充字符。
     * 预期行为: 输出按指定宽度和对齐方式填充。
     */
    DOCTEST_TEST_CASE("fmt::format — 对齐与填充")
    {
        DOCTEST_SUBCASE("右对齐（默认）")
        {
            DOCTEST_CHECK(fmt::format("{:5}",  42) == "   42");
            DOCTEST_CHECK(fmt::format("{:>5}", 42) == "   42");
        }
        DOCTEST_SUBCASE("左对齐")
        {
            DOCTEST_CHECK(fmt::format("{:<5}", 42) == "42   ");
            DOCTEST_CHECK(fmt::format("{:<5}", "hi") == "hi   ");
        }
        DOCTEST_SUBCASE("居中对齐")
        {
            DOCTEST_CHECK(fmt::format("{:^5}", "x") == "  x  ");
            DOCTEST_CHECK(fmt::format("{:^6}", "x") == "  x   ");
        }
        DOCTEST_SUBCASE("自定义填充字符")
        {
            DOCTEST_CHECK(fmt::format("{:*>5}", 42) == "***42");
            DOCTEST_CHECK(fmt::format("{:0>5}", 42) == "00042");
        }
        DOCTEST_SUBCASE("字符串宽度对齐")
        {
            DOCTEST_CHECK(fmt::format("{:<10}", "left") == "left      ");
            DOCTEST_CHECK(fmt::format("{:>10}", "right")== "     right");
        }
        DOCTEST_SUBCASE("数字零填充 :05d")
        {
            DOCTEST_CHECK(fmt::format("{:05}", 42)  == "00042");
            DOCTEST_CHECK(fmt::format("{:05}", -42) == "-0042");
        }
    }

    /**
     * 测试 API:   fmt::vformat(string_view fmt, format_args args)
     * 用法说明: 运行时格式字符串版本，配合 fmt::make_format_args 使用。
     *           适合需要在运行时构建格式字符串的场景。
     * 预期行为: 等价于对应的 fmt::format 调用。
     */
    DOCTEST_TEST_CASE("fmt::vformat — 运行时格式化")
    {
        DOCTEST_SUBCASE("基本用法")
        {
            int n = 42;
            std::string s = fmt::vformat("{}", fmt::make_format_args(n));
            DOCTEST_CHECK(s == "42");
        }
        DOCTEST_SUBCASE("多参数")
        {
            int a = 1; std::string b = "two"; double c = 3.0;
            std::string s = fmt::vformat("{} {} {}", fmt::make_format_args(a, b, c));
            DOCTEST_CHECK(s == "1 two 3");
        }
        DOCTEST_SUBCASE("fmt::runtime — 禁用编译期检查")
        {
            // fmt::runtime 将字符串标记为运行时格式字符串，规避编译期验证
            std::string fmt_str = "{}";
            std::string s = fmt::format(fmt::runtime(fmt_str), 99);
            DOCTEST_CHECK(s == "99");
        }
    }

    /**
     * 测试 API:   fmt::format_to(OutputIt out, format_string<T...> fmt, T&&... args)
     * 用法说明: 将格式化结果写入输出迭代器，不分配额外堆内存（取决于目标容器）。
     * 预期行为: 写入内容与 fmt::format 相同。
     */
    DOCTEST_TEST_CASE("fmt::format_to — 输出到迭代器")
    {
        DOCTEST_SUBCASE("写入 std::string（back_inserter）")
        {
            std::string out;
            fmt::format_to(std::back_inserter(out), "hello {}", "world");
            DOCTEST_CHECK(out == "hello world");
        }
        DOCTEST_SUBCASE("写入 std::vector<char>")
        {
            std::vector<char> buf;
            fmt::format_to(std::back_inserter(buf), "{:d}", 123);
            std::string result(buf.begin(), buf.end());
            DOCTEST_CHECK(result == "123");
        }
        DOCTEST_SUBCASE("写入 fmt::memory_buffer")
        {
            fmt::memory_buffer buf;
            fmt::format_to(fmt::appender(buf), "The answer is {}.", 42);
            std::string result = fmt::to_string(buf);
            DOCTEST_CHECK(result == "The answer is 42.");
        }
        DOCTEST_SUBCASE("写入固定大小 char 数组（format_to_result）")
        {
            // 特殊重载：format_to(char(&)[N], ...) 返回 format_to_result
            char buf[16];
            auto res = fmt::format_to(buf, "{:d}", 12345);
            // 检查是否截断（12345 长度 5 < 16，不截断）
            DOCTEST_CHECK(res.truncated == false);
            // 输出内容
            *res.out = '\0';
            DOCTEST_CHECK(std::string(buf) == "12345");
        }
        DOCTEST_SUBCASE("固定大小数组截断检测")
        {
            char buf[4];
            auto res = fmt::format_to(buf, "{}", "hello");  // "hello" = 5 chars > 4
            DOCTEST_CHECK(res.truncated == true);
        }
    }

    /**
     * 测试 API:   fmt::format_to_n(OutputIt out, size_t n, format_string, args...)
     * 用法说明: 最多写入 n 个字符，返回迭代器和总长度（含被截断部分）。
     * 预期行为: 写入字符数不超过 n，.size 返回完整输出长度。
     */
    DOCTEST_TEST_CASE("fmt::format_to_n — 有界输出")
    {
        DOCTEST_SUBCASE("未截断")
        {
            char buf[32] = {};
            auto res = fmt::format_to_n(buf, sizeof(buf) - 1, "{}", "hello");
            DOCTEST_CHECK(std::string(buf, res.out) == "hello");
            DOCTEST_CHECK(res.size == 5);
        }
        DOCTEST_SUBCASE("截断：size 返回完整长度")
        {
            char buf[4] = {};
            // "hello world" = 11 chars
            auto res = fmt::format_to_n(buf, 4, "{}", "hello world");
            DOCTEST_CHECK(res.size == 11);        // 总长度 11
            DOCTEST_CHECK(std::string(buf, res.out) == "hell");  // 只写入 4 个字符
        }
        DOCTEST_SUBCASE("写入 vector<char>")
        {
            std::vector<char> buf(16, '\0');
            auto res = fmt::format_to_n(buf.data(), 10, "The answer is {}.", 42);
            DOCTEST_CHECK(res.size == 17);  // "The answer is 42." = 17
            // 写入前 10 个字符
            std::string s(buf.data(), std::min<size_t>(res.size, 10));
            DOCTEST_CHECK(s == "The answer");
        }
    }

    /**
     * 测试 API:   fmt::formatted_size(format_string<T...>, T&&...)
     * 用法说明: 计算格式化后的字符数，不实际产生输出。用于提前分配缓冲区。
     * 预期行为: 返回值等于 fmt::format 输出的字符数。
     */
    DOCTEST_TEST_CASE("fmt::formatted_size — 计算格式化大小")
    {
        DOCTEST_SUBCASE("基本用法")
        {
            DOCTEST_CHECK(fmt::formatted_size("{}", 42)       == 2);
            DOCTEST_CHECK(fmt::formatted_size("{}", "hello")  == 5);
            DOCTEST_CHECK(fmt::formatted_size("{:.2f}", 3.14) == 4);
        }
        DOCTEST_SUBCASE("与 format 结果长度一致")
        {
            std::string s = fmt::format("The answer is {}.", 42);
            size_t expected = s.size();
            DOCTEST_CHECK(fmt::formatted_size("The answer is {}.", 42) == expected);
        }
        DOCTEST_SUBCASE("空格式字符串")
        {
            DOCTEST_CHECK(fmt::formatted_size("") == 0);
        }
    }

    /**
     * 测试 API:   fmt::memory_buffer / fmt::basic_memory_buffer<T, SIZE>
     *             fmt::to_string(basic_memory_buffer<char, SIZE>)
     * 用法说明: 提供高效的动态增长字符缓冲区，避免重复分配。
     *           可通过 fmt::to_string 转为 std::string。
     * 预期行为: 支持 append/push_back/resize/reserve；to_string 返回正确内容。
     */
    DOCTEST_TEST_CASE("fmt::memory_buffer — 内存缓冲区")
    {
        DOCTEST_SUBCASE("format_to + to_string")
        {
            fmt::memory_buffer buf;
            fmt::format_to(fmt::appender(buf), "x = {}", 7);
            std::string s = fmt::to_string(buf);
            DOCTEST_CHECK(s == "x = 7");
        }
        DOCTEST_SUBCASE("多次写入")
        {
            fmt::memory_buffer buf;
            for (int i = 0; i < 3; ++i)
                fmt::format_to(fmt::appender(buf), "[{}]", i);
            DOCTEST_CHECK(fmt::to_string(buf) == "[0][1][2]");
        }
        DOCTEST_SUBCASE("resize / reserve")
        {
            fmt::memory_buffer buf;
            buf.reserve(64);
            DOCTEST_CHECK(buf.capacity() >= 64);
            buf.resize(10);
            DOCTEST_CHECK(buf.size() == 10);
        }
        DOCTEST_SUBCASE("clear 后重用")
        {
            fmt::memory_buffer buf;
            fmt::format_to(fmt::appender(buf), "old");
            buf.clear();
            DOCTEST_CHECK(buf.size() == 0);
            fmt::format_to(fmt::appender(buf), "new");
            DOCTEST_CHECK(fmt::to_string(buf) == "new");
        }
        DOCTEST_SUBCASE("移动语义")
        {
            fmt::memory_buffer buf1;
            fmt::format_to(fmt::appender(buf1), "move me");
            fmt::memory_buffer buf2 = std::move(buf1);
            DOCTEST_CHECK(fmt::to_string(buf2) == "move me");
        }
    }

    /**
     * 测试 API:   fmt::to_string(T value)
     * 用法说明: 将数值类型快速转换为字符串，等价于 fmt::format("{}", value)。
     * 预期行为: 返回对应的十进制字符串表示。
     */
    DOCTEST_TEST_CASE("fmt::to_string — 值转字符串")
    {
        DOCTEST_SUBCASE("整数")
        {
            DOCTEST_CHECK(fmt::to_string(42)         == "42");
            DOCTEST_CHECK(fmt::to_string(-1)         == "-1");
            DOCTEST_CHECK(fmt::to_string(0)          == "0");
            DOCTEST_CHECK(fmt::to_string(1000000LL)  == "1000000");
        }
        DOCTEST_SUBCASE("bool")
        {
            // bool 特化：to_string 返回 "true"/"false"
            DOCTEST_CHECK(fmt::to_string(true)  == "true");
            DOCTEST_CHECK(fmt::to_string(false) == "false");
        }
        DOCTEST_SUBCASE("浮点（通用格式）")
        {
            // to_string 使用最短精确表示
            std::string s = fmt::to_string(1.5);
            DOCTEST_CHECK(s == "1.5");
        }
    }

    /**
     * 测试 API:   fmt::format_error (继承自 std::runtime_error)
     * 用法说明: 当格式字符串非法时，fmt 会抛出 fmt::format_error。
     *           用户也可以主动构造并抛出该异常。
     * 预期行为: 异常继承自 std::runtime_error，what() 返回错误消息。
     */
    DOCTEST_TEST_CASE("fmt::format_error — 格式化错误")
    {
        DOCTEST_SUBCASE("手动构造 format_error")
        {
            fmt::format_error err("test error");
            DOCTEST_CHECK(std::string(err.what()) == "test error");
        }
        DOCTEST_SUBCASE("format_error 继承自 std::runtime_error")
        {
            // 可以用 std::runtime_error& 捕获
            try {
                throw fmt::format_error("my error");
            } catch (const std::runtime_error& e)
            {
                DOCTEST_CHECK(std::string(e.what()) == "my error");
            }
        }
        DOCTEST_SUBCASE("非法格式字符串抛出 format_error")
        {
            // 未闭合的花括号
            DOCTEST_CHECK_THROWS_AS(std::ignore = fmt::format(fmt::runtime("{"), 42), fmt::format_error);
            // 参数索引越界
            DOCTEST_CHECK_THROWS_AS(std::ignore = fmt::format(fmt::runtime("{0} {2}"), 1, 2), fmt::format_error);
            // 不匹配的类型说明符（对字符串使用整数格式）
            DOCTEST_CHECK_THROWS_AS(std::ignore = fmt::format(fmt::runtime("{:d}"), "not_a_number"), fmt::format_error);
        }
    }

    /**
     * 测试 API:   fmt::format_int(value)
     *             format_int::data() / c_str() / str() / size()
     * 用法说明: 高效的整数到字符串转换，避免动态内存分配。
     *           适合对性能敏感的整数格式化场景。
     * 预期行为: 输出与 std::to_string 一致；c_str() 以 null 结尾。
     */
    DOCTEST_TEST_CASE("fmt::format_int — 快速整数格式化器")
    {
        DOCTEST_SUBCASE("正整数")
        {
            fmt::format_int fi(42);
            DOCTEST_CHECK(fi.str()  == "42");
            DOCTEST_CHECK(fi.size() == 2);
            DOCTEST_CHECK(std::string(fi.c_str()) == "42");
        }
        DOCTEST_SUBCASE("负整数")
        {
            fmt::format_int fi(-12345);
            DOCTEST_CHECK(fi.str() == "-12345");
        }
        DOCTEST_SUBCASE("零")
        {
            fmt::format_int fi(0);
            DOCTEST_CHECK(fi.str() == "0");
        }
        DOCTEST_SUBCASE("long long 最大值")
        {
            fmt::format_int fi(9223372036854775807LL);
            DOCTEST_CHECK(fi.str() == "9223372036854775807");
        }
        DOCTEST_SUBCASE("unsigned long long")
        {
            fmt::format_int fi(18446744073709551615ULL);
            DOCTEST_CHECK(fi.str() == "18446744073709551615");
        }
        DOCTEST_SUBCASE("data() 指针有效且长度正确")
        {
            fmt::format_int fi(777);
            DOCTEST_CHECK(fi.size() == 3);
            DOCTEST_CHECK(std::string(fi.data(), fi.size()) == "777");
        }
    }

    /**
     * 测试 API:   fmt::ptr(T* p) -> const void*
     * 用法说明: 将非 void 指针转换为 const void* 以进行格式化。
     *           指针输出为 0x 开头的十六进制地址。
     * 预期行为: 格式化结果以 "0x" 开头；空指针输出 "0x0" 或 "(nil)" 等平台相关值。
     */
    DOCTEST_TEST_CASE("fmt::ptr — 指针格式化")
    {
        DOCTEST_SUBCASE("格式化非空指针")
        {
            int value = 42;
            std::string s = fmt::format("{}", fmt::ptr(&value));
            // 指针以 0x 开头（主流平台稳定）
            DOCTEST_CHECK(s.find("0x") == 0);
        }
        DOCTEST_SUBCASE("格式化空指针")
        {
            int* p = nullptr;
            std::string s = fmt::format("{}", fmt::ptr(p));
            // 空指针：0x0 或类似
            DOCTEST_CHECK(s.find("0x") == 0);
        }
        DOCTEST_SUBCASE("不同指针类型")
        {
            double d = 3.14;
            char c = 'z';
            std::string sd = fmt::format("{}", fmt::ptr(&d));
            std::string sc = fmt::format("{}", fmt::ptr(&c));
            DOCTEST_CHECK(sd.find("0x") == 0);
            DOCTEST_CHECK(sc.find("0x") == 0);
        }
    }

    /**
     * 测试 API:   fmt::underlying(Enum e) -> underlying_t<Enum>
     * 用法说明: 将枚举值转换为其底层整数类型以进行格式化。
     *           避免为每个枚举写 static_cast。
     * 预期行为: 输出枚举的整数值。
     */
    DOCTEST_TEST_CASE("fmt::underlying — 枚举转底层类型")
    {
        DOCTEST_SUBCASE("enum class Color")
        {
            DOCTEST_CHECK(fmt::format("{}", fmt::underlying(Color::Red))   == "0");
            DOCTEST_CHECK(fmt::format("{}", fmt::underlying(Color::Green)) == "1");
            DOCTEST_CHECK(fmt::format("{}", fmt::underlying(Color::Blue))  == "2");
        }
        DOCTEST_SUBCASE("普通 enum")
        {
            enum Status { OK = 200, NotFound = 404, Error = 500 };
            DOCTEST_CHECK(fmt::format("{}", fmt::underlying(OK))       == "200");
            DOCTEST_CHECK(fmt::format("{}", fmt::underlying(NotFound)) == "404");
        }
        DOCTEST_SUBCASE("底层为 unsigned char 的枚举")
        {
            enum class Byte : unsigned char { A = 0, B = 255 };
            DOCTEST_CHECK(fmt::format("{}", fmt::underlying(Byte::A)) == "0");
            DOCTEST_CHECK(fmt::format("{}", fmt::underlying(Byte::B)) == "255");
        }
    }

    /**
     * 测试 API:   fmt::group_digits(T value) -> group_digits_view<T>
     * 用法说明: 使用 ',' 作为千位分隔符格式化整数（不依赖 locale）。
     * 预期行为: 每三位插入一个逗号。
     */
    DOCTEST_TEST_CASE("fmt::group_digits — 数字分组")
    {
        DOCTEST_SUBCASE("基本用法")
        {
            DOCTEST_CHECK(fmt::format("{}", fmt::group_digits(1000))    == "1,000");
            DOCTEST_CHECK(fmt::format("{}", fmt::group_digits(12345))   == "12,345");
            DOCTEST_CHECK(fmt::format("{}", fmt::group_digits(1234567)) == "1,234,567");
        }
        DOCTEST_SUBCASE("小数不分组")
        {
            DOCTEST_CHECK(fmt::format("{}", fmt::group_digits(0))   == "0");
            DOCTEST_CHECK(fmt::format("{}", fmt::group_digits(999)) == "999");
        }
        DOCTEST_SUBCASE("long long")
        {
            DOCTEST_CHECK(fmt::format("{}", fmt::group_digits(1000000000LL)) == "1,000,000,000");
        }
        DOCTEST_SUBCASE("配合宽度")
        {
            // 带宽度的 group_digits
            std::string s = fmt::format("{:>15}", fmt::group_digits(1234567));
            DOCTEST_CHECK(s == "      1,234,567");
        }
    }

    /**
     * 测试 API:   fmt::bytes(string_view s)
     * 用法说明: 将字符串以原始字节形式处理（按字节宽度而非 Unicode 码点对齐）。
     * 预期行为: 可配合宽度/精度说明符对字节数组进行格式化。
     */
    DOCTEST_TEST_CASE("fmt::bytes — 字节序列格式化")
    {
        DOCTEST_SUBCASE("基本输出")
        {
            std::string data = "hello";
            std::string s = fmt::format("{}", fmt::bytes(data));
            DOCTEST_CHECK(s == "hello");
        }
        DOCTEST_SUBCASE("配合精度截断")
        {
            // fmt::bytes 忽略精度说明符，输出完整字节序列
            std::string data = "hello world";
            std::string s = fmt::format("{:.5}", fmt::bytes(data));
            DOCTEST_CHECK(s == "hello world");
        }
        DOCTEST_SUBCASE("配合宽度对齐")
        {
            std::string data = "hi";
            std::string s = fmt::format("{:<10}", fmt::bytes(data));
            DOCTEST_CHECK(s == "hi        ");
        }
    }

    /**
     * 测试 API:   FMT_STRING(s)
     * 用法说明: 将字符串字面量转化为编译期格式字符串，触发格式字符串验证。
     *           错误的格式字符串会导致编译错误（不会运行到断言处）。
     * 预期行为: 正确的 FMT_STRING 使用产生与普通格式化相同的输出。
     */
    DOCTEST_TEST_CASE("FMT_STRING — 编译时格式字符串")
    {
        DOCTEST_SUBCASE("基本正确用法")
        {
            auto s = fmt::format(FMT_STRING("{}"), 42);
            DOCTEST_CHECK(s == "42");
        }
        DOCTEST_SUBCASE("多参数")
        {
            auto s = fmt::format(FMT_STRING("{} + {} = {}"), 1, 2, 3);
            DOCTEST_CHECK(s == "1 + 2 = 3");
        }
        DOCTEST_SUBCASE("格式说明符")
        {
            auto s = fmt::format(FMT_STRING("{:05d}"), 42);
            DOCTEST_CHECK(s == "00042");
        }
    }

    /**
     * 测试 API:   fmt::arg(const char* name, const T& value)
     *             using namespace fmt::literals; "name"_a = value
     * 用法说明: 创建命名参数，在格式字符串中用 {name} 引用。
     * 预期行为: {name} 替换为对应命名参数的格式化值。
     */
    DOCTEST_TEST_CASE("fmt::arg 和 _a — 命名参数")
    {
        DOCTEST_SUBCASE("fmt::arg 基本用法")
        {
            std::string s = fmt::format("The answer is {answer}.",
                                         fmt::arg("answer", 42));
            DOCTEST_CHECK(s == "The answer is 42.");
        }
        DOCTEST_SUBCASE("多个命名参数")
        {
            std::string s = fmt::format("{name} is {age} years old.",
                                         fmt::arg("name", "Alice"),
                                         fmt::arg("age", 30));
            DOCTEST_CHECK(s == "Alice is 30 years old.");
        }
        DOCTEST_SUBCASE("_a 用户自定义字面量")
        {
            using namespace fmt::literals;
            std::string s = fmt::format("{greeting}, {subject}!",
                                         "greeting"_a = "Hello",
                                         "subject"_a = "World");
            DOCTEST_CHECK(s == "Hello, World!");
        }
        DOCTEST_SUBCASE("命名参数与位置参数混合")
        {
            using namespace fmt::literals;
            std::string s = fmt::format("{} and {name}",
                                         "pos0",
                                         "name"_a = "named");
            DOCTEST_CHECK(s == "pos0 and named");
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
    DOCTEST_TEST_CASE("fmt::print / println — 输出到 stdout")
    {
        DOCTEST_SUBCASE("print 不抛出异常")
        {
            // 注意：这会实际向 stdout 写内容，仅验证不抛出
            DOCTEST_CHECK_NOTHROW(fmt::print(""));
            DOCTEST_CHECK_NOTHROW(fmt::print("{}", 42));
        }
        DOCTEST_SUBCASE("println 不抛出异常")
        {
            DOCTEST_CHECK_NOTHROW(fmt::println(""));
            DOCTEST_CHECK_NOTHROW(fmt::println("{}", 42));
        }
        DOCTEST_SUBCASE("print 到 stderr")
        {
            DOCTEST_CHECK_NOTHROW(fmt::print(stderr, "{}", "error log"));
        }
        DOCTEST_SUBCASE("println 到 stderr")
        {
            DOCTEST_CHECK_NOTHROW(fmt::println(stderr, "{}", "error with newline"));
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
    DOCTEST_TEST_CASE("fmt::writer / fmt::string_buffer")
    {
        DOCTEST_SUBCASE("string_buffer 基本用法")
        {
            fmt::string_buffer sb;
            fmt::writer w = sb;
            w.print("{}", 42);
            w.print(" hello");
            DOCTEST_CHECK(sb.str() == "42 hello");
        }
        DOCTEST_SUBCASE("多次写入累积")
        {
            fmt::string_buffer sb;
            fmt::writer w = sb;
            for (int i = 0; i < 3; ++i)
                w.print("[{}]", i);
            DOCTEST_CHECK(sb.str() == "[0][1][2]");
        }
    }

    /**
     * 测试 API:   fmt::system_error(int error_code, format_string, args...)
     *             fmt::format_system_error(buffer<char>&, int, const char*)
     * 用法说明: 构建包含系统错误描述的异常对象或格式化系统错误信息。
     * 预期行为: what() 中包含用户消息和系统错误描述。
     */
    DOCTEST_TEST_CASE("fmt::system_error / fmt::format_system_error")
    {
        DOCTEST_SUBCASE("system_error 基本构造")
        {
            // ENOENT = 2 (no such file or directory)
            auto err = fmt::system_error(ENOENT, "cannot open '{}'", "test.txt");
            std::string what = err.what();
            // 消息中应包含用户指定的前缀
            DOCTEST_CHECK(what.find("cannot open 'test.txt'") != std::string::npos);
        }
        DOCTEST_SUBCASE("system_error 是 std::system_error")
        {
            auto err = fmt::system_error(0, "ok");
            // 可以用 std::system_error& 捕获
            const std::system_error& se = err;
            DOCTEST_CHECK(se.code().category().name() != nullptr);
        }
        DOCTEST_SUBCASE("format_system_error 写入 buffer")
        {
            fmt::memory_buffer buf;
            fmt::format_system_error(buf, ENOENT, "file error");
            std::string s = fmt::to_string(buf);
            DOCTEST_CHECK(s.find("file error") != std::string::npos);
        }
    }

    /**
     * 测试 API:   template<> struct fmt::formatter<T> { parse, format }
     * 用法说明: 为自定义类型实现 formatter 特化，使其可以用 fmt::format 格式化。
     *           需要实现 parse（解析格式规格）和 format（生成输出）两个成员函数。
     * 预期行为: fmt::format("{}", myobj) 调用自定义 formatter::format。
     */
    DOCTEST_TEST_CASE("自定义 formatter<T> 特化")
    {
        DOCTEST_SUBCASE("基本格式化自定义类型（默认笛卡尔坐标）")
        {
            // Point 已在文件顶部特化了 formatter<Point>
            Point p{3, 4};
            std::string s = fmt::format("{}", p);
            DOCTEST_CHECK(s == "(3, 4)");
        }
        DOCTEST_SUBCASE(":d 笛卡尔坐标")
        {
            Point p{3, 4};
            std::string s = fmt::format("{:d}", p);
            DOCTEST_CHECK(s == "(3, 4)");
        }
        DOCTEST_SUBCASE(":p 极坐标")
        {
            Point p{3, 4};
            std::string s = fmt::format("{:p}", p);
            // r = sqrt(3²+4²) = 5.00, θ = atan2(4, 3) ≈ 0.93 rad
            DOCTEST_CHECK(s.find("5.00") != std::string::npos);
            DOCTEST_CHECK(s.find("0.93") != std::string::npos);
        }
        DOCTEST_SUBCASE("format_to 也使用自定义 formatter")
        {
            Point p{-1, 0};
            std::string out;
            fmt::format_to(std::back_inserter(out), "p={}", p);
            DOCTEST_CHECK(out == "p=(-1, 0)");
        }
        DOCTEST_SUBCASE("多个自定义类型参数")
        {
            Point a{1, 2}, b{3, 4};
            std::string s = fmt::format("A={} B={}", a, b);
            DOCTEST_CHECK(s == "A=(1, 2) B=(3, 4)");
        }
        DOCTEST_SUBCASE("非法格式说明符抛出 format_error")
        {
            Point p{1, 2};
            DOCTEST_CHECK_THROWS_AS(std::ignore = fmt::format("{:z}", p), fmt::format_error);
        }
        DOCTEST_SUBCASE("fmt::is_formattable — 检测类型是否可格式化")
        {
            // int, double, std::string 是内置可格式化类型
            static_assert(fmt::is_formattable<int>::value,        "int should be formattable");
            static_assert(fmt::is_formattable<double>::value,     "double should be formattable");
            static_assert(fmt::is_formattable<std::string>::value,"string should be formattable");
            // Point 有特化 formatter，应为可格式化
            static_assert(fmt::is_formattable<Point>::value,      "Point should be formattable");
            // 验证检测结果正确
            DOCTEST_CHECK(fmt::is_formattable<int>::value    == true);
            DOCTEST_CHECK(fmt::is_formattable<Point>::value  == true);
        }
    }
} // DOCTEST_TEST_SUITE("fmt/format.h — 核心格式化功能")

DOCTEST_TEST_SUITE("fmt/args.h — 动态参数存储")
{
    /**
     * 测试 API:   fmt::dynamic_format_arg_store<Context>
     *             push_back(const T& arg)         — 添加位置参数
     *             push_back(named_arg<Char, T>)   — 添加命名参数
     *             push_back(reference_wrapper<T>) — 添加引用参数（避免拷贝）
     *             clear()                          — 清空存储
     *             reserve(new_cap, new_cap_named) — 预分配容量
     *             size()                           — 返回参数数量
     *             隐式转换为 fmt::format_args 传入 vformat
     * 用法说明: 在运行时动态构建参数列表，用于类型擦除的格式化函数（如 vformat）。
     * 预期行为: 参数按顺序存储，vformat 能正确读取并格式化。
     */
    DOCTEST_TEST_CASE("dynamic_format_arg_store — 位置参数")
    {
        DOCTEST_SUBCASE("添加基本类型参数并格式化")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(42);
            store.push_back("hello");
            store.push_back(3.14);
            std::string s = fmt::vformat("{} {} {:.2f}", store);
            DOCTEST_CHECK(s == "42 hello 3.14");
        }
        DOCTEST_SUBCASE("单参数")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(100);
            DOCTEST_CHECK(fmt::vformat("{}", store) == "100");
        }
        DOCTEST_SUBCASE("size() 返回参数数量")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            DOCTEST_CHECK(store.size() == 0);
            store.push_back(1);
            DOCTEST_CHECK(store.size() == 1);
            store.push_back(2);
            DOCTEST_CHECK(store.size() == 2);
        }
        DOCTEST_SUBCASE("clear() 清空参数")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(1);
            store.push_back(2);
            store.clear();
            DOCTEST_CHECK(store.size() == 0);
            // 清空后可重新添加
            store.push_back(99);
            DOCTEST_CHECK(fmt::vformat("{}", store) == "99");
        }
        DOCTEST_SUBCASE("reserve() 预分配容量")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            // 预分配 10 个参数（其中 0 个命名）
            DOCTEST_CHECK_NOTHROW(store.reserve(10, 0));
            store.push_back(1);
            DOCTEST_CHECK(store.size() == 1);
        }
        DOCTEST_SUBCASE("bool 和浮点类型")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(true);
            store.push_back(false);
            store.push_back(1.5f);
            std::string s = fmt::vformat("{} {} {}", store);
            DOCTEST_CHECK(s == "true false 1.5");
        }
    }

    DOCTEST_TEST_CASE("dynamic_format_arg_store — 命名参数")
    {
        DOCTEST_SUBCASE("添加命名参数并按名引用")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(fmt::arg("name", "Alice"));
            store.push_back(fmt::arg("age", 30));
            std::string s = fmt::vformat("{name} is {age} years old.", store);
            DOCTEST_CHECK(s == "Alice is 30 years old.");
        }
        DOCTEST_SUBCASE("混合命名和位置参数")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            store.push_back(fmt::arg("x", 10));
            store.push_back(fmt::arg("y", 20));
            std::string s = fmt::vformat("({x}, {y})", store);
            DOCTEST_CHECK(s == "(10, 20)");
        }
    }

    DOCTEST_TEST_CASE("dynamic_format_arg_store — 引用包装（std::cref）")
    {
        DOCTEST_SUBCASE("push_back(std::cref) 避免字符串拷贝")
        {
            fmt::dynamic_format_arg_store<fmt::format_context> store;
            std::string str = "original";
            // cref 存储引用，后续修改会反映到格式化结果
            store.push_back(std::cref(str));
            str = "modified";
            // 此时 store 中引用的字符串已被修改
            std::string s = fmt::vformat("{}", store);
            DOCTEST_CHECK(s == "modified");
        }
    }

} // DOCTEST_TEST_SUITE("fmt/args.h — 动态参数存储")

DOCTEST_TEST_SUITE("fmt/chrono.h — 时间格式化")
{
    /**
     * 测试 API:   formatter<std::chrono::duration<Rep, Period>>
     * 用法说明: 直接格式化 chrono duration 类型，默认输出带单位后缀。
     *           可使用 %H:%M:%S 等 chrono 格式说明符进行自定义。
     * 预期行为: 单位后缀与 period 对应（s/ms/us/min/h）；自定义格式按说明符输出。
     */
    DOCTEST_TEST_CASE("std::chrono::duration — 默认格式（带单位后缀）")
    {
        using namespace std::chrono_literals;

        DOCTEST_SUBCASE("秒 seconds")
        {
            DOCTEST_CHECK(fmt::format("{}", std::chrono::seconds(42)) == "42s");
            DOCTEST_CHECK(fmt::format("{}", std::chrono::seconds(0))  == "0s");
            DOCTEST_CHECK(fmt::format("{}", std::chrono::seconds(-1)) == "-1s");
        }
        DOCTEST_SUBCASE("毫秒 milliseconds")
        {
            DOCTEST_CHECK(fmt::format("{}", std::chrono::milliseconds(1500)) == "1500ms");
            DOCTEST_CHECK(fmt::format("{}", std::chrono::milliseconds(0))    == "0ms");
        }
        DOCTEST_SUBCASE("分钟 minutes")
        {
            DOCTEST_CHECK(fmt::format("{}", std::chrono::minutes(90)) == "90min");
        }
        DOCTEST_SUBCASE("小时 hours")
        {
            DOCTEST_CHECK(fmt::format("{}", std::chrono::hours(24)) == "24h");
        }
        DOCTEST_SUBCASE("微秒 microseconds")
        {
            // 微秒单位：µs（UTF-8 平台）或 us（非 UTF-8 平台）
            std::string s = fmt::format("{}", std::chrono::microseconds(100));
            // 应以 "100" 开头，后跟单位后缀（µs 或 us）
            DOCTEST_CHECK(s.find("100") == 0);
            // 后缀应包含 's'（无论是 µs 还是 us）
            DOCTEST_CHECK(s.find('s') != std::string::npos);
            DOCTEST_CHECK(s.size() > 3);
        }
        DOCTEST_SUBCASE("浮点 duration")
        {
            // 浮点表示的 duration
            std::chrono::duration<double> d(1.5);
            std::string s = fmt::format("{}", d);
            DOCTEST_CHECK(s.find("1.5") == 0);
        }
    }

    DOCTEST_TEST_CASE("std::chrono::duration — 自定义格式说明符")
    {
        using namespace std::chrono;

        DOCTEST_SUBCASE("%H:%M:%S 小时:分钟:秒")
        {
            // 3661 秒 = 1 小时 1 分 1 秒
            auto d = seconds(3661);
            std::string s = fmt::format("{:%H:%M:%S}", d);
            DOCTEST_CHECK(s == "01:01:01");
        }
        DOCTEST_SUBCASE("%H:%M:%S 零值")
        {
            auto d = seconds(0);
            std::string s = fmt::format("{:%H:%M:%S}", d);
            DOCTEST_CHECK(s == "00:00:00");
        }
        DOCTEST_SUBCASE("%H:%M:%S 大值")
        {
            // 86400 秒 = 24 小时整
            // %H 对 24 取模，所以 24 小时显示为 00
            auto d = seconds(86400);
            std::string s = fmt::format("{:%H:%M:%S}", d);
            DOCTEST_CHECK(s == "00:00:00");
        }
        DOCTEST_SUBCASE("%Q 仅数值部分")
        {
            auto d = seconds(42);
            std::string s = fmt::format("{:%Q}", d);
            DOCTEST_CHECK(s == "42");
        }
        DOCTEST_SUBCASE("%q 仅单位部分")
        {
            auto d = seconds(1);
            std::string s = fmt::format("{:%q}", d);
            DOCTEST_CHECK(s == "s");
        }
        DOCTEST_SUBCASE("毫秒精度 %S 包含小数部分")
        {
            // 1500 ms = 1 s + 500 ms，%S 应包含小数秒
            auto d = milliseconds(1500);
            std::string s = fmt::format("{:%S}", d);
            // 应包含 ".500" 或类似小数部分
            DOCTEST_CHECK(s.find('.') != std::string::npos);
        }
        DOCTEST_SUBCASE("宽度与对齐")
        {
            auto d = seconds(5);
            std::string s = fmt::format("{:>10%Q%q}", d);
            DOCTEST_CHECK(s.size() == 10);
            // 右对齐，内容为 "5s"
            DOCTEST_CHECK(s.find("5s") != std::string::npos);
        }
    }

    /**
     * 测试 API:   formatter<std::tm>
     *             fmt::gmtime(std::time_t) → std::tm
     * 用法说明: 格式化 C 语言 tm 结构体，支持标准 strftime 风格的格式说明符。
     *           gmtime 是 std::gmtime 的线程安全版本。
     * 预期行为: 使用固定 epoch 值（1970-01-01 00:00:00 UTC）验证格式化正确性。
     */
    DOCTEST_TEST_CASE("std::tm 格式化 + fmt::gmtime")
    {
        DOCTEST_SUBCASE("fmt::gmtime(0) = 1970-01-01 00:00:00 UTC")
        {
            // epoch 0 对应 1970-01-01 00:00:00 UTC
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            // 默认格式为 %F %T（ISO 8601 日期 + 时间）
            std::string s = fmt::format("{}", t);
            DOCTEST_CHECK(s == "1970-01-01 00:00:00");
        }
        DOCTEST_SUBCASE("%Y-%m-%d 日期格式")
        {
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string s = fmt::format("{:%Y-%m-%d}", t);
            DOCTEST_CHECK(s == "1970-01-01");
        }
        DOCTEST_SUBCASE("%H:%M:%S 时间格式")
        {
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string s = fmt::format("{:%H:%M:%S}", t);
            DOCTEST_CHECK(s == "00:00:00");
        }
        DOCTEST_SUBCASE("%F %T 完整 ISO 8601")
        {
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string s = fmt::format("{:%F %T}", t);
            DOCTEST_CHECK(s == "1970-01-01 00:00:00");
        }
        DOCTEST_SUBCASE("已知时间戳验证")
        {
            // 2000-01-01 00:00:00 UTC = 946684800
            std::tm t = fmt::gmtime(static_cast<std::time_t>(946684800));
            DOCTEST_CHECK(fmt::format("{:%Y}", t) == "2000");
            DOCTEST_CHECK(fmt::format("{:%m}", t) == "01");
            DOCTEST_CHECK(fmt::format("{:%d}", t) == "01");
        }
        DOCTEST_SUBCASE("星期几 %a %A")
        {
            // 1970-01-01 是星期四
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string abbr = fmt::format("{:%a}", t);
            std::string full = fmt::format("{:%A}", t);
            DOCTEST_CHECK(abbr == "Thu");
            DOCTEST_CHECK(full == "Thursday");
        }
        DOCTEST_SUBCASE("月份名称 %b %B")
        {
            // 1970-01-01 = January
            std::tm t = fmt::gmtime(static_cast<std::time_t>(0));
            std::string abbr = fmt::format("{:%b}", t);
            std::string full = fmt::format("{:%B}", t);
            DOCTEST_CHECK(abbr == "Jan");
            DOCTEST_CHECK(full == "January");
        }
        DOCTEST_SUBCASE("fmt::gmtime 线程安全版本不抛出")
        {
            DOCTEST_CHECK_NOTHROW(fmt::gmtime(static_cast<std::time_t>(0)));
            DOCTEST_CHECK_NOTHROW(fmt::gmtime(static_cast<std::time_t>(1000000)));
        }
    }

    /**
     * 测试 API:   fmt::weekday / fmt::day / fmt::month / fmt::year
     *             fmt::year_month_day
     * 用法说明: 格式化 C++20 日期类型（或 fmt 提供的回退实现）。
     *           这些类型在 C++17 下通过 fmt 自行定义。
     * 预期行为: 按照日历语义正确格式化。
     */
    DOCTEST_TEST_CASE("fmt::weekday / day / month / year / year_month_day 格式化")
    {
        DOCTEST_SUBCASE("fmt::weekday — 星期")
        {
            // 0=Sunday, 1=Monday, ..., 6=Saturday
            fmt::weekday sun(0);
            std::string s = fmt::format("{}", sun);
            DOCTEST_CHECK(s == "Sun");

            fmt::weekday mon(1);
            DOCTEST_CHECK(fmt::format("{}", mon) == "Mon");
        }
        DOCTEST_SUBCASE("fmt::day — 月中第几天")
        {
            fmt::day d(15);
            std::string s = fmt::format("{}", d);
            // 默认格式输出两位数字
            DOCTEST_CHECK(s == "15");
        }
        DOCTEST_SUBCASE("fmt::month — 月份")
        {
            fmt::month jan(1);
            std::string s = fmt::format("{}", jan);
            DOCTEST_CHECK(s == "Jan");

            fmt::month dec(12);
            DOCTEST_CHECK(fmt::format("{}", dec) == "Dec");
        }
        DOCTEST_SUBCASE("fmt::year — 年份")
        {
            fmt::year y(2024);
            std::string s = fmt::format("{}", y);
            DOCTEST_CHECK(s == "2024");
        }
        DOCTEST_SUBCASE("fmt::year_month_day — 完整日期")
        {
            fmt::year_month_day ymd{
                fmt::year(2024),
                fmt::month(3),
                fmt::day(15)
            };
            std::string s = fmt::format("{}", ymd);
            // 默认格式为 ISO 日期：2024-03-15
            DOCTEST_CHECK(s == "2024-03-15");
        }
    }

    /**
     * 测试 API:   formatter<fmt::sys_time<Duration>>
     *             fmt::sys_time<Duration> = time_point<system_clock, Duration>
     * 用法说明: 格式化 system_clock 时间点，支持 strftime 风格格式说明符。
     *           默认格式为 UTC 时间。
     * 预期行为: epoch 时间点格式化为 "1970-01-01 00:00:00"（UTC）。
     */
    DOCTEST_TEST_CASE("fmt::sys_time 格式化")
    {
        DOCTEST_SUBCASE("epoch 时间点（seconds）")
        {
            // 构造一个距 epoch 0 秒的时间点
            auto tp = fmt::sys_time<std::chrono::seconds>(std::chrono::seconds(0));
            std::string s = fmt::format("{}", tp);
            DOCTEST_CHECK(s == "1970-01-01 00:00:00");
        }
        DOCTEST_SUBCASE("已知时间戳（946684800 = 2000-01-01 00:00:00 UTC）")
        {
            auto tp = fmt::sys_time<std::chrono::seconds>(std::chrono::seconds(946684800));
            std::string s = fmt::format("{:%F %T}", tp);
            DOCTEST_CHECK(s == "2000-01-01 00:00:00");
        }
        DOCTEST_SUBCASE("毫秒精度时间点")
        {
            // 1.5 秒后的时间点（含小数秒）
            auto tp = fmt::sys_time<std::chrono::milliseconds>(std::chrono::milliseconds(1500));
            std::string s = fmt::format("{}", tp);
            // 默认格式应包含小数秒
            DOCTEST_CHECK(s.find("1970-01-01") != std::string::npos);
            DOCTEST_CHECK(s.find(".500") != std::string::npos);
        }
    }
} // DOCTEST_TEST_SUITE("fmt/chrono.h — 时间格式化")

DOCTEST_TEST_SUITE("fmt/compile.h — 编译时格式化")
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
    DOCTEST_TEST_CASE("FMT_COMPILE + fmt::format")
    {
        DOCTEST_SUBCASE("整数格式化")
        {
            // FMT_COMPILE 在支持 constexpr-if 的编译器上生成优化代码
            auto s = fmt::format(FMT_COMPILE("{}"), 42);
            DOCTEST_CHECK(s == "42");
        }
        DOCTEST_SUBCASE("字符串格式化")
        {
            auto s = fmt::format(FMT_COMPILE("hello {}"), "world");
            DOCTEST_CHECK(s == "hello world");
        }
        DOCTEST_SUBCASE("多参数")
        {
            auto s = fmt::format(FMT_COMPILE("{} + {} = {}"), 1, 2, 3);
            DOCTEST_CHECK(s == "1 + 2 = 3");
        }
        DOCTEST_SUBCASE("格式说明符")
        {
            auto s = fmt::format(FMT_COMPILE("{:08.3f}"), 3.14);
            DOCTEST_CHECK(s == "0003.140");
        }
        DOCTEST_SUBCASE("空格式字符串")
        {
            auto s = fmt::format(FMT_COMPILE("no args"));
            DOCTEST_CHECK(s == "no args");
        }
        DOCTEST_SUBCASE("十六进制")
        {
            auto s = fmt::format(FMT_COMPILE("{:#x}"), 255);
            DOCTEST_CHECK(s == "0xff");
        }
    }

    DOCTEST_TEST_CASE("FMT_COMPILE + fmt::format_to")
    {
        DOCTEST_SUBCASE("写入 std::string")
        {
            std::string out;
            fmt::format_to(std::back_inserter(out), FMT_COMPILE("{}"), 99);
            DOCTEST_CHECK(out == "99");
        }
        DOCTEST_SUBCASE("写入 memory_buffer")
        {
            fmt::memory_buffer buf;
            fmt::format_to(fmt::appender(buf), FMT_COMPILE("{:d}"), 42);
            DOCTEST_CHECK(fmt::to_string(buf) == "42");
        }
        DOCTEST_SUBCASE("多次写入")
        {
            std::string out;
            auto it = std::back_inserter(out);
            fmt::format_to(it, FMT_COMPILE("[{}]"), 1);
            fmt::format_to(it, FMT_COMPILE("[{}]"), 2);
            fmt::format_to(it, FMT_COMPILE("[{}]"), 3);
            DOCTEST_CHECK(out == "[1][2][3]");
        }
    }

    DOCTEST_TEST_CASE("FMT_COMPILE + fmt::formatted_size")
    {
        DOCTEST_SUBCASE("计算格式化大小")
        {
            auto sz = fmt::formatted_size(FMT_COMPILE("{}"), 12345);
            DOCTEST_CHECK(sz == 5);
        }
        DOCTEST_SUBCASE("与 format 结果一致")
        {
            auto s  = fmt::format(FMT_COMPILE("{:>10}"), "hi");
            auto sz = fmt::formatted_size(FMT_COMPILE("{:>10}"), "hi");
            DOCTEST_CHECK(s.size() == sz);
            DOCTEST_CHECK(sz == 10);
        }
    }

    DOCTEST_TEST_CASE("FMT_COMPILE + fmt::format_to_n")
    {
        DOCTEST_SUBCASE("未截断")
        {
            char buf[32] = {};
            auto res = fmt::format_to_n(buf, 31, FMT_COMPILE("{}"), "hello");
            *res.out = '\0';
            DOCTEST_CHECK(std::string(buf) == "hello");
            DOCTEST_CHECK(res.size == 5);
        }
        DOCTEST_SUBCASE("截断")
        {
            char buf[4] = {};
            auto res = fmt::format_to_n(buf, 4, FMT_COMPILE("{}"), "hello world");
            DOCTEST_CHECK(res.size == 11);  // 完整长度
            DOCTEST_CHECK(std::string(buf, res.out) == "hell");
        }
    }
} // DOCTEST_TEST_SUITE("fmt/compile.h — 编译时格式化")

DOCTEST_TEST_SUITE("fmt/std.h — 标准库类型支持")
{
    /**
     * 测试 API:   formatter<std::optional<T>, Char>
     * 用法说明: 格式化 std::optional<T>。
     *           - 有值时输出 "optional(value)"
     *           - 无值时输出 "none"
     * 预期行为: 与文档一致；字符串值带引号。
     */
    DOCTEST_TEST_CASE("std::optional 格式化")
    {
        DOCTEST_SUBCASE("有值的 optional<int>")
        {
            std::optional<int> opt = 42;
            std::string s = fmt::format("{}", opt);
            DOCTEST_CHECK(s == "optional(42)");
        }
        DOCTEST_SUBCASE("空值 optional<int>")
        {
            std::optional<int> opt;
            std::string s = fmt::format("{}", opt);
            DOCTEST_CHECK(s == "none");
        }
        DOCTEST_SUBCASE("optional<string> — 字符串值带引号")
        {
            std::optional<std::string> opt = "hello";
            std::string s = fmt::format("{}", opt);
            // 字符串以调试模式输出，带引号
            DOCTEST_CHECK(s == "optional(\"hello\")");
        }
        DOCTEST_SUBCASE("optional<double>")
        {
            std::optional<double> opt = 3.14;
            std::string s = fmt::format("{}", opt);
            DOCTEST_CHECK(s.find("optional(") == 0);
            DOCTEST_CHECK(s.find("3.14") != std::string::npos);
        }
        DOCTEST_SUBCASE("std::nullopt")
        {
            std::optional<int> opt = std::nullopt;
            DOCTEST_CHECK(fmt::format("{}", opt) == "none");
        }
    }

    /**
     * 测试 API:   formatter<std::variant<T...>, Char>
     *             formatter<std::monostate, Char>
     * 用法说明: 格式化 std::variant，输出 "variant(value)"；
     *           std::monostate 输出 "monostate"。
     * 预期行为: 字符串值带引号；单类型 variant 直接显示值。
     */
    DOCTEST_TEST_CASE("std::variant 格式化")
    {
        DOCTEST_SUBCASE("variant<int> — 整数值")
        {
            std::variant<int, std::string, double> v = 42;
            std::string s = fmt::format("{}", v);
            DOCTEST_CHECK(s == "variant(42)");
        }
        DOCTEST_SUBCASE("variant<string> — 字符串值带引号")
        {
            std::variant<int, std::string, double> v = std::string("hello");
            std::string s = fmt::format("{}", v);
            DOCTEST_CHECK(s == "variant(\"hello\")");
        }
        DOCTEST_SUBCASE("variant<double>")
        {
            std::variant<int, std::string, double> v = 3.14;
            std::string s = fmt::format("{}", v);
            DOCTEST_CHECK(s.find("variant(") == 0);
        }
        DOCTEST_SUBCASE("std::monostate")
        {
            std::variant<std::monostate, int> v;  // 默认 monostate
            std::string s = fmt::format("{}", v);
            DOCTEST_CHECK(s == "variant(monostate)");
        }
        DOCTEST_SUBCASE("独立 std::monostate")
        {
            std::string s = fmt::format("{}", std::monostate{});
            DOCTEST_CHECK(s == "monostate");
        }
    }

    /**
     * 测试 API:   formatter<std::error_code>
     * 用法说明: 格式化 std::error_code，默认输出 "category:value"。
     *           使用 :s 格式说明符输出错误消息字符串。
     * 预期行为: 格式正确；category 名称和 value 与构造时一致。
     */
    DOCTEST_TEST_CASE("std::error_code 格式化")
    {
        DOCTEST_SUBCASE("默认格式 category:value")
        {
            std::error_code ec = std::make_error_code(std::errc::no_such_file_or_directory);
            std::string s = fmt::format("{}", ec);
            // 格式为 "generic:2" 或类似（category 名称和 value）
            DOCTEST_CHECK(s.find("generic") != std::string::npos);
            // value = ENOENT = 2
            DOCTEST_CHECK(s.find("2") != std::string::npos);
        }
        DOCTEST_SUBCASE(":s 格式输出消息字符串")
        {
            std::error_code ec = std::make_error_code(std::errc::no_such_file_or_directory);
            std::string s = fmt::format("{:s}", ec);
            // 消息字符串不为空
            DOCTEST_CHECK(!s.empty());
        }
        DOCTEST_SUBCASE("成功错误码 value=0")
        {
            std::error_code ec;  // 默认构造 = 无错误
            std::string s = fmt::format("{}", ec);
            DOCTEST_CHECK(s.find("0") != std::string::npos);
        }
        DOCTEST_SUBCASE("调试格式 :?")
        {
            std::error_code ec = std::make_error_code(std::errc::permission_denied);
            std::string s = fmt::format("{:?}", ec);
            // 调试格式带引号
            DOCTEST_CHECK(s.find('"') != std::string::npos);
        }
    }

    /**
     * 测试 API:   formatter<T> where T 继承自 std::exception
     * 用法说明: 格式化 std::exception 子类，输出 what() 的内容。
     * 预期行为: 输出 what() 字符串；格式化不抛出。
     */
    DOCTEST_TEST_CASE("std::exception 格式化")
    {
        DOCTEST_SUBCASE("std::runtime_error")
        {
            std::runtime_error ex("something went wrong");
            std::string s = fmt::format("{}", ex);
            DOCTEST_CHECK(s == "something went wrong");
        }
        DOCTEST_SUBCASE("std::invalid_argument")
        {
            std::invalid_argument ex("bad argument");
            std::string s = fmt::format("{}", ex);
            DOCTEST_CHECK(s == "bad argument");
        }
        DOCTEST_SUBCASE("自定义异常类")
        {
            struct MyError : std::exception {
                const char* what() const noexcept override { return "my error"; }
            };
            MyError err;
            std::string s = fmt::format("{}", err);
            DOCTEST_CHECK(s == "my error");
        }
    }

    /**
     * 测试 API:   formatter<std::bitset<N>, Char>
     * 用法说明: 将 bitset 格式化为二进制字符串（高位在前）。
     * 预期行为: 输出为 '0'/'1' 组成的字符串，最高位在最左。
     */
    DOCTEST_TEST_CASE("std::bitset 格式化")
    {
        DOCTEST_SUBCASE("bitset<8> 全零")
        {
            std::bitset<8> bs(0);
            DOCTEST_CHECK(fmt::format("{}", bs) == "00000000");
        }
        DOCTEST_SUBCASE("bitset<8> 全一")
        {
            std::bitset<8> bs(255);
            DOCTEST_CHECK(fmt::format("{}", bs) == "11111111");
        }
        DOCTEST_SUBCASE("bitset<8> 部分位")
        {
            std::bitset<8> bs(0b10101010);
            DOCTEST_CHECK(fmt::format("{}", bs) == "10101010");
        }
        DOCTEST_SUBCASE("bitset<4>")
        {
            std::bitset<4> bs(5);  // 0101
            DOCTEST_CHECK(fmt::format("{}", bs) == "0101");
        }
        DOCTEST_SUBCASE("bitset<1>")
        {
            std::bitset<1> bs1(1);
            std::bitset<1> bs0(0);
            DOCTEST_CHECK(fmt::format("{}", bs1) == "1");
            DOCTEST_CHECK(fmt::format("{}", bs0) == "0");
        }
        DOCTEST_SUBCASE("配合宽度对齐")
        {
            std::bitset<4> bs(3);  // 0011
            std::string s = fmt::format("{:>8}", bs);
            DOCTEST_CHECK(s == "    0011");
        }
    }

    /**
     * 测试 API:   formatter<std::complex<T>, Char>
     * 用法说明: 格式化复数，实部非零时输出 "(real+imagi)"，仅虚部时输出 "imagi"。
     * 注意: 浮点精度可能有细微平台差异，使用宽松检测。
     * 预期行为: 格式化结果包含实部和虚部的正确值及 'i' 后缀。
     */
    DOCTEST_TEST_CASE("std::complex 格式化")
    {
        DOCTEST_SUBCASE("实部+虚部")
        {
            std::complex<double> c(1.0, 2.0);
            std::string s = fmt::format("{}", c);
            // 格式: "(1+2i)"
            DOCTEST_CHECK(s == "(1+2i)");
        }
        DOCTEST_SUBCASE("仅虚部（实部为0）")
        {
            std::complex<double> c(0.0, 3.0);
            std::string s = fmt::format("{}", c);
            // 格式: "3i"
            DOCTEST_CHECK(s == "3i");
        }
        DOCTEST_SUBCASE("负虚部")
        {
            std::complex<double> c(1.0, -2.0);
            std::string s = fmt::format("{}", c);
            DOCTEST_CHECK(s == "(1-2i)");
        }
        DOCTEST_SUBCASE("配合精度说明符")
        {
            std::complex<double> c(1.5, 2.5);
            std::string s = fmt::format("{:.1f}", c);
            DOCTEST_CHECK(s == "(1.5+2.5i)");
        }
        DOCTEST_SUBCASE("float 类型")
        {
            std::complex<float> c(0.0f, 1.0f);
            std::string s = fmt::format("{}", c);
            DOCTEST_CHECK(s == "1i");
        }
    }

    /**
     * 测试 API:   formatter<std::atomic<T>, Char>
     * 用法说明: 格式化 std::atomic<T>，透明调用 T 的格式化器（通过 load()）。
     * 预期行为: 输出与格式化底层值相同。
     */
    DOCTEST_TEST_CASE("std::atomic 格式化")
    {
        DOCTEST_SUBCASE("atomic<int>")
        {
            std::atomic<int> a(42);
            std::string s = fmt::format("{}", a);
            DOCTEST_CHECK(s == "42");
        }
        DOCTEST_SUBCASE("atomic<bool>")
        {
            std::atomic<bool> a(true);
            std::string s = fmt::format("{}", a);
            DOCTEST_CHECK(s == "true");
        }
        DOCTEST_SUBCASE("atomic<long>")
        {
            std::atomic<long> a(-1L);
            std::string s = fmt::format("{}", a);
            DOCTEST_CHECK(s == "-1");
        }
        DOCTEST_SUBCASE("配合格式说明符")
        {
            std::atomic<int> a(255);
            std::string s = fmt::format("{:#x}", a);
            DOCTEST_CHECK(s == "0xff");
        }
    }

    /**
     * 测试 API:   formatter<std::reference_wrapper<T>, Char>
     * 用法说明: 格式化 std::reference_wrapper<T>（透明委托给 T 的格式化器）。
     *           通过 std::ref/std::cref 创建。
     * 预期行为: 输出与直接格式化 T 相同。
     */
    DOCTEST_TEST_CASE("std::reference_wrapper 格式化")
    {
        DOCTEST_SUBCASE("cref<int>")
        {
            int val = 42;
            auto ref = std::cref(val);
            DOCTEST_CHECK(fmt::format("{}", ref) == "42");
        }
        DOCTEST_SUBCASE("ref<string>")
        {
            std::string s = "hello";
            auto ref = std::ref(s);
            DOCTEST_CHECK(fmt::format("{}", ref) == "hello");
        }
        DOCTEST_SUBCASE("修改原值后格式化反映变化")
        {
            int val = 10;
            auto ref = std::ref(val);
            DOCTEST_CHECK(fmt::format("{}", ref) == "10");
            val = 20;
            DOCTEST_CHECK(fmt::format("{}", ref) == "20");
        }
        DOCTEST_SUBCASE("配合格式说明符")
        {
            int val = 255;
            DOCTEST_CHECK(fmt::format("{:x}", std::cref(val)) == "ff");
        }
    }

    /**
     * 测试 API:   fmt::ptr(const std::unique_ptr<T, Deleter>& p) -> const void*
     *             fmt::ptr(const std::shared_ptr<T>& p) -> const void*
     *             （定义于 fmt/std.h）
     * 用法说明: 从智能指针提取原始指针以进行格式化（输出十六进制地址）。
     * 预期行为: 格式化结果以 "0x" 开头；空指针输出 "0x0"。
     */
    DOCTEST_TEST_CASE("fmt::ptr(unique_ptr) / fmt::ptr(shared_ptr)")
    {
        DOCTEST_SUBCASE("unique_ptr 非空")
        {
            auto p = std::make_unique<int>(42);
            std::string s = fmt::format("{}", fmt::ptr(p));
            DOCTEST_CHECK(s.find("0x") == 0);
        }
        DOCTEST_SUBCASE("unique_ptr 空")
        {
            std::unique_ptr<int> p;
            std::string s = fmt::format("{}", fmt::ptr(p));
            DOCTEST_CHECK(s.find("0x") == 0);
        }
        DOCTEST_SUBCASE("shared_ptr 非空")
        {
            auto p = std::make_shared<double>(3.14);
            std::string s = fmt::format("{}", fmt::ptr(p));
            DOCTEST_CHECK(s.find("0x") == 0);
        }
        DOCTEST_SUBCASE("shared_ptr 空")
        {
            std::shared_ptr<int> p;
            std::string s = fmt::format("{}", fmt::ptr(p));
            DOCTEST_CHECK(s.find("0x") == 0);
        }
        DOCTEST_SUBCASE("unique_ptr 指向的值与地址一致")
        {
            auto p = std::make_unique<int>(99);
            int* raw = p.get();
            // fmt::ptr(p) 应与 fmt::ptr(raw) 输出相同
            std::string sp = fmt::format("{}", fmt::ptr(p));
            std::string sr = fmt::format("{}", fmt::ptr(raw));
            DOCTEST_CHECK(sp == sr);
        }
    }

    /**
     * 测试 API:   formatter<std::filesystem::path, Char>
     * 用法说明: 格式化文件系统路径，默认输出 native 格式；
     *           使用 :? 输出带引号的转义字符串；使用 :g 输出通用格式（'/'分隔符）。
     * 注意: 路径分隔符在 Windows（\）和 POSIX（/）上不同，使用平台无关的测试。
     */
    DOCTEST_TEST_CASE("std::filesystem::path 格式化")
    {
        DOCTEST_SUBCASE("简单文件名")
        {
            std::filesystem::path p("test.txt");
            std::string s = fmt::format("{}", p);
            // 应包含文件名（Windows 上可能带引号）
            DOCTEST_CHECK(s.find("test.txt") != std::string::npos);
        }
        DOCTEST_SUBCASE("通用格式 :g")
        {
            std::filesystem::path p("dir/sub/file.txt");
            std::string s = fmt::format("{:g}", p);
            // 通用格式使用 '/' 分隔符（跨平台一致）
            DOCTEST_CHECK(s.find("file.txt") != std::string::npos);
        }
        DOCTEST_SUBCASE("调试格式 :?（带引号）")
        {
            std::filesystem::path p("hello.txt");
            std::string s = fmt::format("{:?}", p);
            DOCTEST_CHECK(s.front() == '"');
            DOCTEST_CHECK(s.back()  == '"');
        }
    }
} // DOCTEST_TEST_SUITE("fmt/std.h — 标准库类型支持")

DOCTEST_TEST_SUITE("fmt/base.h — 独立公开 API 验证")
{
    /**
     * 测试 API:   fmt::string_view / fmt::basic_string_view<Char>
     * 用法说明: fmt 自己实现的字符串视图，兼容 std::string_view 和 C 字符串。
     *           不依赖 C++17 的 std::string_view，确保 C++11/14 兼容性。
     * 预期行为: 支持构造、比较、data/size/begin/end/starts_with 等操作。
     */
    DOCTEST_TEST_CASE("fmt::string_view / fmt::basic_string_view")
    {
        DOCTEST_SUBCASE("从 C 字符串构造")
        {
            fmt::string_view sv("hello");
            DOCTEST_CHECK(sv.size()         == 5);
            DOCTEST_CHECK(std::string(sv.data(), sv.size()) == "hello");
        }
        DOCTEST_SUBCASE("从 std::string 构造")
        {
            std::string s = "world";
            fmt::string_view sv(s);
            DOCTEST_CHECK(sv.size()     == 5);
            DOCTEST_CHECK(sv[0]         == 'w');
            DOCTEST_CHECK(sv[4]         == 'd');
        }
        DOCTEST_SUBCASE("begins/end 迭代器")
        {
            fmt::string_view sv("abc");
            std::string result(sv.begin(), sv.end());
            DOCTEST_CHECK(result == "abc");
        }
        DOCTEST_SUBCASE("比较运算符")
        {
            fmt::string_view a("foo");
            fmt::string_view b("foo");
            fmt::string_view c("bar");
            DOCTEST_CHECK(a == b);
            DOCTEST_CHECK(a != c);
            DOCTEST_CHECK(c < a);
        }
        DOCTEST_SUBCASE("starts_with")
        {
            fmt::string_view sv("hello world");
            DOCTEST_CHECK(sv.starts_with("hello") == true);
            DOCTEST_CHECK(sv.starts_with("world") == false);
            DOCTEST_CHECK(sv.starts_with('h')     == true);
            DOCTEST_CHECK(sv.starts_with('w')     == false);
        }
        DOCTEST_SUBCASE("remove_prefix")
        {
            fmt::string_view sv("hello world");
            sv.remove_prefix(6);
            DOCTEST_CHECK(std::string(sv.data(), sv.size()) == "world");
        }
        DOCTEST_SUBCASE("空视图")
        {
            fmt::string_view sv("");
            DOCTEST_CHECK(sv.size() == 0);
            DOCTEST_CHECK(sv.begin() == sv.end());
        }
    }

    /**
     * 测试 API:   fmt::is_formattable<T, Char>
     * 用法说明: 编译期检测类型 T 是否可以用 fmt::format 格式化。
     * 预期行为: 内置类型和有 formatter 特化的类型返回 true；
     *           无 formatter 且不可映射的类型返回 false。
     */
    DOCTEST_TEST_CASE("fmt::is_formattable<T> 类型特征检测")
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

        DOCTEST_CHECK(fmt::is_formattable<int>::value    == true);
        DOCTEST_CHECK(fmt::is_formattable<double>::value == true);
        DOCTEST_CHECK(fmt::is_formattable<Point>::value  == true);
    }

    /**
     * 测试 API:   fmt::make_format_args(T&... args)
     * 用法说明: 将参数列表打包为类型擦除的 format_args，供 vformat/vformat_to 使用。
     * 预期行为: 返回的 format_args 可传入 vformat 得到正确结果。
     */
    DOCTEST_TEST_CASE("fmt::make_format_args")
    {
        DOCTEST_SUBCASE("单参数")
        {
            int n = 42;
            auto args = fmt::make_format_args(n);
            std::string s = fmt::vformat("{}", args);
            DOCTEST_CHECK(s == "42");
        }
        DOCTEST_SUBCASE("多参数")
        {
            int a = 1;
            std::string b = "two";
            double c = 3.0;
            auto args = fmt::make_format_args(a, b, c);
            std::string s = fmt::vformat("{} {} {}", args);
            DOCTEST_CHECK(s == "1 two 3");
        }
        DOCTEST_SUBCASE("format_args 转换")
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
    DOCTEST_TEST_CASE("fmt::runtime — 运行时格式字符串")
    {
        DOCTEST_SUBCASE("基本用法")
        {
            const char* fmt_str = "{}";
            std::string s = fmt::format(fmt::runtime(fmt_str), 42);
            DOCTEST_CHECK(s == "42");
        }
        DOCTEST_SUBCASE("std::string 格式字符串")
        {
            std::string fmt_str = "{} + {} = {}";
            std::string s = fmt::format(fmt::runtime(fmt_str), 1, 2, 3);
            DOCTEST_CHECK(s == "1 + 2 = 3");
        }
        DOCTEST_SUBCASE("动态选择格式字符串")
        {
            bool show_hex = true;
            const char* fmt_str = show_hex ? "{:#x}" : "{}";
            std::string s = fmt::format(fmt::runtime(fmt_str), 255);
            DOCTEST_CHECK(s == "0xff");
        }
    }
} // DOCTEST_TEST_SUITE("fmt/base.h — 独立公开 API 验证")

DOCTEST_TEST_SUITE("fmt/ranges.h — 范围与元组格式化")
{
    /**
     * 测试 API:   formatter<std::tuple<T...>, Char>
     *             formatter<std::pair<K, V>, Char>
     * 用法说明: 格式化元组和 pair，默认输出格式为 "(elem1, elem2, ...)"。
     *           元素中的字符串类型以调试模式（带引号）输出。
     *           使用 :n 说明符可省略括号和分隔符。
     * 预期行为: 默认括号为 ()，分隔符为 ", "；字符串元素带引号。
     */
    DOCTEST_TEST_CASE("std::tuple 格式化")
    {
        DOCTEST_SUBCASE("空元组")
        {
            auto t = std::tuple<>{};
            DOCTEST_CHECK(fmt::format("{}", t) == "()");
        }
        DOCTEST_SUBCASE("单元素 tuple<int>")
        {
            auto t = std::make_tuple(42);
            DOCTEST_CHECK(fmt::format("{}", t) == "(42)");
        }
        DOCTEST_SUBCASE("双元素 tuple<int, double>")
        {
            auto t = std::make_tuple(1, 2.5);
            DOCTEST_CHECK(fmt::format("{}", t) == "(1, 2.5)");
        }
        DOCTEST_SUBCASE("三元素 tuple<int, string, bool>")
        {
            // 字符串元素以调试模式（带引号）输出
            auto t = std::make_tuple(1, std::string("hi"), true);
            DOCTEST_CHECK(fmt::format("{}", t) == "(1, \"hi\", true)");
        }
        DOCTEST_SUBCASE(":n 说明符——省略括号和分隔符")
        {
            auto t = std::make_tuple(1, 2, 3);
            DOCTEST_CHECK(fmt::format("{:n}", t) == "123");
        }
    }

    DOCTEST_TEST_CASE("std::pair 格式化")
    {
        DOCTEST_SUBCASE("pair<int, int>")
        {
            auto p = std::make_pair(1, 2);
            DOCTEST_CHECK(fmt::format("{}", p) == "(1, 2)");
        }
        DOCTEST_SUBCASE("pair<string, int> — 字符串键带引号")
        {
            auto p = std::make_pair(std::string("key"), 42);
            DOCTEST_CHECK(fmt::format("{}", p) == "(\"key\", 42)");
        }
        DOCTEST_SUBCASE("pair<int, double>")
        {
            auto p = std::make_pair(0, 3.14);
            std::string s = fmt::format("{}", p);
            DOCTEST_CHECK(s.find("(0,") == 0);
            DOCTEST_CHECK(s.find("3.14") != std::string::npos);
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
    DOCTEST_TEST_CASE("std::vector 格式化")
    {
        DOCTEST_SUBCASE("空 vector")
        {
            std::vector<int> v;
            DOCTEST_CHECK(fmt::format("{}", v) == "[]");
        }
        DOCTEST_SUBCASE("vector<int> 基本格式化")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{}", v) == "[1, 2, 3]");
        }
        DOCTEST_SUBCASE("vector<double>")
        {
            std::vector<double> v = {1.5, 2.5};
            DOCTEST_CHECK(fmt::format("{}", v) == "[1.5, 2.5]");
        }
        DOCTEST_SUBCASE("vector<string>（元素带引号）")
        {
            std::vector<std::string> v = {"hello", "world"};
            DOCTEST_CHECK(fmt::format("{}", v) == "[\"hello\", \"world\"]");
        }
        DOCTEST_SUBCASE(":n 说明符——省略括号")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{:n}", v) == "1, 2, 3");
        }
        DOCTEST_SUBCASE("::x — 对元素应用十六进制格式")
        {
            std::vector<int> v = {10, 255, 16};
            DOCTEST_CHECK(fmt::format("{::x}", v) == "[a, ff, 10]");
        }
        DOCTEST_SUBCASE("::#x — 对元素应用带前缀十六进制格式")
        {
            std::vector<int> v = {10, 255, 16};
            DOCTEST_CHECK(fmt::format("{::#x}", v) == "[0xa, 0xff, 0x10]");
        }
    }

    DOCTEST_TEST_CASE("std::array 格式化")
    {
        DOCTEST_SUBCASE("array<int, 3>")
        {
            std::array<int, 3> a = {10, 20, 30};
            DOCTEST_CHECK(fmt::format("{}", a) == "[10, 20, 30]");
        }
        DOCTEST_SUBCASE("array<int, 0>（空数组）")
        {
            std::array<int, 0> a{};
            DOCTEST_CHECK(fmt::format("{}", a) == "[]");
        }
        DOCTEST_SUBCASE("array<double, 2>")
        {
            std::array<double, 2> a = {1.0, 2.0};
            DOCTEST_CHECK(fmt::format("{}", a) == "[1, 2]");
        }
    }

    DOCTEST_TEST_CASE("C 数组格式化")
    {
        DOCTEST_SUBCASE("int[3]")
        {
            int a[] = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{}", a) == "[1, 2, 3]");
        }
        DOCTEST_SUBCASE("char[N] — 字符数组格式化")
        {
            char a[] = {'x', 'y'};
            // char 数组默认被当作字符串/字节序列处理
            // 输出可能是 "xy" 或类似格式，不一定带单引号
            std::string s = fmt::format("{}", a);
            DOCTEST_CHECK(!s.empty());
        }
    }

    DOCTEST_TEST_CASE("std::list 和 std::deque 格式化")
    {
        DOCTEST_SUBCASE("list<int>")
        {
            std::list<int> lst = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{}", lst) == "[1, 2, 3]");
        }
        DOCTEST_SUBCASE("deque<int>")
        {
            std::deque<int> dq = {4, 5, 6};
            DOCTEST_CHECK(fmt::format("{}", dq) == "[4, 5, 6]");
        }
        DOCTEST_SUBCASE("空 list")
        {
            std::list<double> lst;
            DOCTEST_CHECK(fmt::format("{}", lst) == "[]");
        }
    }

    /**
     * 测试 API:   formatter<R, Char>（range_format_kind == set）
     * 用法说明: 格式化 set、multiset 等集合容器，
     *           默认输出格式为 "{elem1, elem2, ...}"（大括号）。
     *           字符串元素以调试模式（带引号）输出。
     * 预期行为: 默认括号为 {}，分隔符为 ", "；set 元素按升序排列。
     */
    DOCTEST_TEST_CASE("std::set 格式化")
    {
        DOCTEST_SUBCASE("set<int>")
        {
            std::set<int> s = {3, 1, 2};
            // set 内部有序（升序）
            DOCTEST_CHECK(fmt::format("{}", s) == "{1, 2, 3}");
        }
        DOCTEST_SUBCASE("空 set")
        {
            std::set<int> s;
            DOCTEST_CHECK(fmt::format("{}", s) == "{}");
        }
        DOCTEST_SUBCASE("set<string>（元素带引号）")
        {
            std::set<std::string> s = {"b", "a"};
            DOCTEST_CHECK(fmt::format("{}", s) == "{\"a\", \"b\"}");
        }
        DOCTEST_SUBCASE("multiset<int>（允许重复）")
        {
            std::multiset<int> ms = {1, 2, 2, 3};
            DOCTEST_CHECK(fmt::format("{}", ms) == "{1, 2, 2, 3}");
        }
        DOCTEST_SUBCASE(":n 说明符——省略括号")
        {
            std::set<int> s = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{:n}", s) == "1, 2, 3");
        }
    }

    DOCTEST_TEST_CASE("std::unordered_set 格式化")
    {
        DOCTEST_SUBCASE("unordered_set<int> — 包含正确元素")
        {
            std::unordered_set<int> s = {3, 1, 2};
            std::string r = fmt::format("{}", s);
            // unordered_set 不保证顺序，仅验证括号和元素存在
            DOCTEST_CHECK(r.front() == '{');
            DOCTEST_CHECK(r.back()  == '}');
            DOCTEST_CHECK(r.find("1") != std::string::npos);
            DOCTEST_CHECK(r.find("2") != std::string::npos);
            DOCTEST_CHECK(r.find("3") != std::string::npos);
        }
        DOCTEST_SUBCASE("空 unordered_set")
        {
            std::unordered_set<int> s;
            DOCTEST_CHECK(fmt::format("{}", s) == "{}");
        }
        DOCTEST_SUBCASE("unordered_set<string>（元素带引号）")
        {
            std::unordered_set<std::string> s = {"a", "b"};
            std::string r = fmt::format("{}", s);
            DOCTEST_CHECK(r.front() == '{');
            DOCTEST_CHECK(r.back()  == '}');
            DOCTEST_CHECK(r.find("\"a\"") != std::string::npos);
            DOCTEST_CHECK(r.find("\"b\"") != std::string::npos);
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
    DOCTEST_TEST_CASE("std::map 格式化")
    {
        DOCTEST_SUBCASE("map<int, int>")
        {
            std::map<int, int> m = {{1, 10}, {2, 20}};
            DOCTEST_CHECK(fmt::format("{}", m) == "{1: 10, 2: 20}");
        }
        DOCTEST_SUBCASE("空 map")
        {
            std::map<int, int> m;
            DOCTEST_CHECK(fmt::format("{}", m) == "{}");
        }
        DOCTEST_SUBCASE("map<string, int>（键带引号）")
        {
            std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
            DOCTEST_CHECK(fmt::format("{}", m) == "{\"a\": 1, \"b\": 2}");
        }
        DOCTEST_SUBCASE("map<int, string>（值带引号）")
        {
            std::map<int, std::string> m = {{1, "one"}, {2, "two"}};
            DOCTEST_CHECK(fmt::format("{}", m) == "{1: \"one\", 2: \"two\"}");
        }
        DOCTEST_SUBCASE(":n 说明符——省略括号")
        {
            std::map<int, int> m = {{1, 10}, {2, 20}};
            DOCTEST_CHECK(fmt::format("{:n}", m) == "1: 10, 2: 20");
        }
        DOCTEST_SUBCASE("multimap<int, int>")
        {
            std::multimap<int, int> mm = {{1, 10}, {1, 20}, {2, 30}};
            std::string s = fmt::format("{}", mm);
            DOCTEST_CHECK(s.find("{1:") == 0);
            DOCTEST_CHECK(s.find("2: 30") != std::string::npos);
        }
    }

    DOCTEST_TEST_CASE("std::unordered_map 格式化")
    {
        DOCTEST_SUBCASE("unordered_map<int, int> — 包含正确键值对")
        {
            std::unordered_map<int, int> m = {{1, 10}, {2, 20}};
            std::string r = fmt::format("{}", m);
            DOCTEST_CHECK(r.front() == '{');
            DOCTEST_CHECK(r.back()  == '}');
            DOCTEST_CHECK(r.find("1: 10") != std::string::npos);
            DOCTEST_CHECK(r.find("2: 20") != std::string::npos);
        }
        DOCTEST_SUBCASE("空 unordered_map")
        {
            std::unordered_map<int, int> m;
            DOCTEST_CHECK(fmt::format("{}", m) == "{}");
        }
        DOCTEST_SUBCASE("unordered_map<string, int>（键带引号）")
        {
            std::unordered_map<std::string, int> m = {{"a", 1}, {"b", 2}};
            std::string r = fmt::format("{}", m);
            DOCTEST_CHECK(r.front() == '{');
            DOCTEST_CHECK(r.back()  == '}');
            DOCTEST_CHECK(r.find("\"a\": 1") != std::string::npos);
            DOCTEST_CHECK(r.find("\"b\": 2") != std::string::npos);
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
    DOCTEST_TEST_CASE("容器适配器格式化")
    {
        DOCTEST_SUBCASE("std::stack<int>")
        {
            std::stack<int> s;
            s.push(1); s.push(2); s.push(3);
            // 底层默认为 deque，按入栈顺序格式化（栈底 → 栈顶）
            std::string r = fmt::format("{}", s);
            DOCTEST_CHECK(r.front() == '[');
            DOCTEST_CHECK(r.back()  == ']');
            DOCTEST_CHECK(r.find("1") != std::string::npos);
            DOCTEST_CHECK(r.find("2") != std::string::npos);
            DOCTEST_CHECK(r.find("3") != std::string::npos);
        }
        DOCTEST_SUBCASE("std::queue<int>")
        {
            std::queue<int> q;
            q.push(10); q.push(20); q.push(30);
            std::string r = fmt::format("{}", q);
            DOCTEST_CHECK(r.front() == '[');
            DOCTEST_CHECK(r.back()  == ']');
            DOCTEST_CHECK(r.find("10") != std::string::npos);
            DOCTEST_CHECK(r.find("30") != std::string::npos);
        }
        DOCTEST_SUBCASE("std::priority_queue<int>")
        {
            std::priority_queue<int> pq;
            pq.push(3); pq.push(1); pq.push(2);
            std::string r = fmt::format("{}", pq);
            // 输出底层 vector 的堆序内容，格式为 [...]
            DOCTEST_CHECK(r.front() == '[');
            DOCTEST_CHECK(r.back()  == ']');
            DOCTEST_CHECK(r.find("1") != std::string::npos);
            DOCTEST_CHECK(r.find("3") != std::string::npos);
        }
        DOCTEST_SUBCASE("空 stack")
        {
            std::stack<int> s;
            DOCTEST_CHECK(fmt::format("{}", s) == "[]");
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
    DOCTEST_TEST_CASE("fmt::join — 迭代器范围连接")
    {
        DOCTEST_SUBCASE("基本用法（vector 迭代器）")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ", ")) == "1, 2, 3");
        }
        DOCTEST_SUBCASE("不同分隔符")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), " | ")) == "1 | 2 | 3");
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ""))    == "123");
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), "-"))   == "1-2-3");
        }
        DOCTEST_SUBCASE("空范围")
        {
            std::vector<int> v;
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ", ")) == "");
        }
        DOCTEST_SUBCASE("单元素")
        {
            std::vector<int> v = {42};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ", ")) == "42");
        }
        DOCTEST_SUBCASE("配合格式说明符 {:02d}")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{:02}", fmt::join(v.begin(), v.end(), ", ")) == "01, 02, 03");
        }
        DOCTEST_SUBCASE("字符串元素（join 不加引号，直接输出）")
        {
            std::vector<std::string> v = {"a", "b", "c"};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v.begin(), v.end(), ", ")) == "a, b, c");
        }
    }

    DOCTEST_TEST_CASE("fmt::join — 范围连接")
    {
        DOCTEST_SUBCASE("vector 范围")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v, ", ")) == "1, 2, 3");
        }
        DOCTEST_SUBCASE("list 范围")
        {
            std::list<int> lst = {4, 5, 6};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(lst, " ")) == "4 5 6");
        }
        DOCTEST_SUBCASE("array 范围")
        {
            std::array<int, 3> a = {7, 8, 9};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(a, "-")) == "7-8-9");
        }
        DOCTEST_SUBCASE("配合格式说明符 {:x}")
        {
            std::vector<int> v = {0xff, 0x0a, 0x1b};
            DOCTEST_CHECK(fmt::format("{:x}", fmt::join(v, ", ")) == "ff, a, 1b");
        }
        DOCTEST_SUBCASE("浮点配合精度 {:.2f}")
        {
            std::vector<double> v = {1.1, 2.2, 3.3};
            DOCTEST_CHECK(fmt::format("{:.2f}", fmt::join(v, ", ")) == "1.10, 2.20, 3.30");
        }
        DOCTEST_SUBCASE("字符串范围（无引号）")
        {
            std::vector<std::string> v = {"foo", "bar"};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(v, " / ")) == "foo / bar");
        }
    }

    DOCTEST_TEST_CASE("fmt::join — 元组连接")
    {
        DOCTEST_SUBCASE("tuple<int, double, string>（无引号）")
        {
            auto t = std::make_tuple(1, 2.5, std::string("x"));
            DOCTEST_CHECK(fmt::format("{}", fmt::join(t, ", ")) == "1, 2.5, x");
        }
        DOCTEST_SUBCASE("pair<int, string>")
        {
            auto p = std::make_pair(42, std::string("hello"));
            DOCTEST_CHECK(fmt::format("{}", fmt::join(p, " = ")) == "42 = hello");
        }
        DOCTEST_SUBCASE("空元组")
        {
            auto t = std::tuple<>{};
            DOCTEST_CHECK(fmt::format("{}", fmt::join(t, ", ")) == "");
        }
        DOCTEST_SUBCASE("单元素元组")
        {
            auto t = std::make_tuple(99);
            DOCTEST_CHECK(fmt::format("{}", fmt::join(t, ", ")) == "99");
        }
        DOCTEST_SUBCASE("三元素同类型元组")
        {
            auto t = std::make_tuple(10, 20, 30);
            DOCTEST_CHECK(fmt::format("{}", fmt::join(t, " + ")) == "10 + 20 + 30");
        }
    }

    DOCTEST_TEST_CASE("fmt::join — 初始化列表连接")
    {
        DOCTEST_SUBCASE("initializer_list<int>")
        {
            DOCTEST_CHECK(fmt::format("{}", fmt::join({1, 2, 3}, ", ")) == "1, 2, 3");
        }
        DOCTEST_SUBCASE("initializer_list<double>")
        {
            DOCTEST_CHECK(fmt::format("{}", fmt::join({1.1, 2.2}, " - ")) == "1.1 - 2.2");
        }
        DOCTEST_SUBCASE("initializer_list 配合格式说明符")
        {
            DOCTEST_CHECK(fmt::format("{:04}", fmt::join({1, 20, 300}, ", ")) == "0001, 0020, 0300");
        }
        DOCTEST_SUBCASE("单元素初始化列表")
        {
            DOCTEST_CHECK(fmt::format("{}", fmt::join({42}, ", ")) == "42");
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
    DOCTEST_TEST_CASE("范围格式说明符")
    {
        DOCTEST_SUBCASE(":n — 序列省略括号")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{:n}", v) == "1, 2, 3");
        }
        DOCTEST_SUBCASE(":n — 集合省略括号")
        {
            std::set<int> s = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{:n}", s) == "1, 2, 3");
        }
        DOCTEST_SUBCASE(":s — char 向量格式化为字符串")
        {
            std::vector<char> v = {'h', 'e', 'l', 'l', 'o'};
            // :s 格式将 char 向量格式化为字符串（行为可能因版本而异）
            std::string s = fmt::format("{:s}", v);
            DOCTEST_CHECK(s.find("hello") != std::string::npos);
        }
        DOCTEST_SUBCASE(":?s — char 向量格式化为调试字符串（带引号）")
        {
            std::vector<char> v = {'h', 'i'};
            DOCTEST_CHECK(fmt::format("{:?s}", v) == "\"hi\"");
        }
        DOCTEST_SUBCASE("::x — 元素十六进制格式")
        {
            std::vector<int> v = {10, 255, 16};
            DOCTEST_CHECK(fmt::format("{::x}", v) == "[a, ff, 10]");
        }
        DOCTEST_SUBCASE("::#x — 元素带前缀十六进制格式")
        {
            std::vector<int> v = {10, 255, 16};
            DOCTEST_CHECK(fmt::format("{::#x}", v) == "[0xa, 0xff, 0x10]");
        }
        DOCTEST_SUBCASE("::02 — 元素零填充格式")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{::02}", v) == "[01, 02, 03]");
        }
        DOCTEST_SUBCASE(":n::02 — 省略括号 + 元素零填充格式")
        {
            std::vector<int> v = {1, 2, 3};
            DOCTEST_CHECK(fmt::format("{:n:02}", v) == "01, 02, 03");
        }
    }

    /**
     * 测试 API:   formatter<Tuple, Char> 格式说明符
     *             :n — 省略括号和分隔符（no brackets/separator）
     * 用法说明: 控制元组的输出格式，:n 会同时省略括号和分隔符。
     * 预期行为: :n 省略 () 括号和 ", " 分隔符，各元素紧密相邻输出。
     */
    DOCTEST_TEST_CASE("元组格式说明符")
    {
        DOCTEST_SUBCASE(":n — 省略括号和分隔符")
        {
            auto t = std::make_tuple(1, 2, 3);
            DOCTEST_CHECK(fmt::format("{:n}", t) == "123");
        }
        DOCTEST_SUBCASE(":n — 含字符串元素（字符串仍带引号）")
        {
            auto t = std::make_tuple(std::string("a"), std::string("b"));
            std::string s = fmt::format("{:n}", t);
            // :n 省略括号和分隔符，但字符串元素仍以调试模式输出（带引号）
            DOCTEST_CHECK(s == "\"a\"\"b\"");
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
    DOCTEST_TEST_CASE("fmt::is_tuple_like 和 fmt::is_range 类型特征")
    {
        DOCTEST_SUBCASE("is_tuple_like")
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
            DOCTEST_CHECK(fmt::is_tuple_like<std::tuple<int>>::value      == true);
            DOCTEST_CHECK(fmt::is_tuple_like<std::pair<int, int>>::value  == true);
            DOCTEST_CHECK(fmt::is_tuple_like<std::vector<int>>::value     == false);
            DOCTEST_CHECK(fmt::is_tuple_like<std::array<int, 3>>::value   == false);
        }
        DOCTEST_SUBCASE("is_range")
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
            DOCTEST_CHECK(fmt::is_range<std::vector<int>, char>::value    == true);
            DOCTEST_CHECK(fmt::is_range<std::set<int>, char>::value       == true);
            DOCTEST_CHECK(fmt::is_range<std::map<int, int>, char>::value  == true);
            DOCTEST_CHECK(fmt::is_range<std::array<int, 3>, char>::value  == true);
            DOCTEST_CHECK(fmt::is_range<int, char>::value                 == false);
        }
    }
} // DOCTEST_TEST_SUITE("fmt/ranges.h — 范围与元组格式化")