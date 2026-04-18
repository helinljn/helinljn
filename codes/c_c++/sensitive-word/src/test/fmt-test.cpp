#include "doctest.h"
#include "spdlog/fmt/fmt.h"

#include <cstddef>
#include <iterator>
#include <string>
#include <utility>

enum class color_code : unsigned
{
    red = 1,
    green = 2,
    blue = 3
};

struct level
{
    int value;
};

struct point
{
    int x;
    int y;
};

auto format_as(level l) -> int
{
    return l.value;
}

std::string render_with_vformat(fmt::string_view pattern, fmt::format_args args)
{
    return fmt::vformat(pattern, args);
}

template <>
struct fmt::formatter<point>
{
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const point& value, FormatContext& ctx) const -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "({}, {})", value.x, value.y);
    }
};

TEST_SUITE("fmt usage")
{
    TEST_CASE("format: 基础格式化与常见格式说明符")
    {
        /*
         * 这是 fmt 最核心的入口：
         * - 按位置替换参数
         * - 对整数、浮点、字符串应用格式说明符
         * - 支持宽度、对齐、补零、进制、符号等常见能力
         */
        CHECK(fmt::format("name={}, score={:06d}, ratio={:.2f}", "fmt", 42, 3.14159) ==
              "name=fmt, score=000042, ratio=3.14");

        CHECK(fmt::format("|{:>6}|{:<6}|{:^6}|", "x", "y", "z") == "|     x|y     |  z   |");
        CHECK(fmt::format("{:+d} {:#x} {:#b}", 42, 42, 42) == "+42 0x2a 0b101010");
        CHECK(fmt::format("{:d}", true) == "1");
    }

    TEST_CASE("format: 动态宽度与动态精度")
    {
        /*
         * base.h 中的格式字符串解析支持动态 width / precision，
         * 这在做“列宽由运行时决定”“小数位数由配置决定”时很常见。
         */
        CHECK(fmt::format("{:>{}}", 42, 6) == "    42");
        CHECK(fmt::format("{:.{}f}", 3.1415926, 3) == "3.142");
    }

    TEST_CASE("runtime / FMT_STRING: 运行时与编译期格式串")
    {
        /*
         * - 默认 format_string 会在编译期校验格式串
         * - fmt::runtime 可切换到运行时校验
         * - FMT_STRING 可显式构造编译期格式串
         */
        const std::string runtime_fmt = "{} + {} = {}";

        CHECK(fmt::format(fmt::runtime(runtime_fmt), 1, 2, 3) == "1 + 2 = 3");
        CHECK(fmt::format(FMT_STRING("{:>5}"), 7) == "    7");
        CHECK_THROWS_AS(std::ignore = fmt::format(fmt::runtime("{:d}"), "oops"), fmt::format_error);
    }

    TEST_CASE("named arguments: 支持具名参数")
    {
        /*
         * 具名参数适合模板消息、日志字段或占位符较多时提升可读性。
         */
        CHECK(fmt::format("{name} => {value}",
                          fmt::arg("name", "answer"),
                          fmt::arg("value", 42)) == "answer => 42");

        using namespace fmt::literals;
        CHECK(fmt::format("{name} => {value}", "name"_a = "pi", "value"_a = 3.14) ==
              "pi => 3.14");
    }

    TEST_CASE("format_to / memory_buffer / to_string: 直接写入外部缓冲区")
    {
        /*
         * 除了直接返回 std::string，
         * fmt 也非常适合把结果写入已有容器或内存缓冲区，
         * 这正是很多高性能日志场景常用的方式。
         */
        std::string out;
        fmt::format_to(std::back_inserter(out), "{}-{:04d}", "id", 7);
        CHECK(out == "id-0007");

        fmt::memory_buffer buffer;
        fmt::format_to(std::back_inserter(buffer), "[{}:{}]", "fmt", 123);
        CHECK(fmt::to_string(buffer) == "[fmt:123]");
    }

    TEST_CASE("format_to_n / formatted_size: 截断写入与预估输出长度")
    {
        /*
         * - format_to_n: 写入固定容量缓冲区，但同时返回完整输出长度
         * - formatted_size: 可先计算需要多少字节，再决定如何分配内存
         */
        char buffer[5] = {};
        const auto result = fmt::format_to_n(buffer, 5, "{}", "abcdefg");

        CHECK(std::string(buffer, buffer + 5) == "abcde");
        CHECK(result.size == 7);
        CHECK(result.out == buffer + 5);

        CHECK(fmt::formatted_size("id={:04d}", 7) == 7);
    }

    TEST_CASE("vformat / make_format_args: 类型擦除格式化接口")
    {
        /*
         * 这类 API 很适合：
         * - 封装自己的日志函数
         * - 转发可变参数但延迟格式化
         * - 在接口层只接收 format_args
         */
        std::string user = "alice";
        int score = 95;

        const auto args = fmt::make_format_args(user, score);
        CHECK(render_with_vformat("{}: {}", args) == "alice: 95");

        auto named_user = fmt::arg("user", user);
        const auto named_args = fmt::make_format_args(named_user);
        CHECK(render_with_vformat("hello, {user}", named_args) == "hello, alice");
    }

    TEST_CASE("pointer / enum / format_as: 指针、枚举与自定义映射")
    {
        /*
         * - fmt::ptr: 明确按指针地址格式化
         * - fmt::underlying: 取枚举底层整数
         * - format_as: 让自定义类型复用已有 formatter
         */
        int value = 10;
        int* ptr = &value;

        CHECK(fmt::format("{}", fmt::ptr(ptr)) == fmt::format("{}", static_cast<const void*>(ptr)));
        CHECK(fmt::format("{}", fmt::underlying(color_code::green)) == "2");
        CHECK(fmt::format("{}", level{5}) == "5");
    }

    TEST_CASE("format_int / to_string / group_digits: 常用辅助工具")
    {
        /*
         * format_int 是 fmt 提供的轻量整数格式化工具；
         * to_string 和 group_digits 则是非常实用的辅助接口。
         */
        const fmt::format_int fast_int(-123456);

        CHECK(fast_int.size() == 7);
        CHECK(std::string(fast_int.data(), fast_int.size()) == "-123456");
        CHECK(std::string(fast_int.c_str()) == "-123456");
        CHECK(fast_int.str() == "-123456");

        CHECK(fmt::to_string(2026) == "2026");
        CHECK(fmt::to_string(true) == "true");
        CHECK(fmt::format("{}", fmt::group_digits(123456789)) == "123,456,789");
    }

    TEST_CASE("debug presentation: 字符串调试格式")
    {
        /*
         * {:?} 会输出更适合调试查看的转义形式，
         * 对包含换行、制表符、引号的文本尤其有用。
         */
        CHECK(fmt::format("{:?}", "A\nB\t\"C\"") == "\"A\\nB\\t\\\"C\\\"\"");
        CHECK(fmt::format("{:?}", '\n') == "'\\n'");
    }

    TEST_CASE("custom formatter: 为业务类型定制格式化行为")
    {
        /*
         * 自定义 formatter 是 fmt 扩展能力里的核心部分。
         * spdlog 里很多业务对象最终也会走类似路径。
         */
        const point pt{3, 4};

        CHECK(fmt::format("{}", pt) == "(3, 4)");
        CHECK(fmt::format("pt={}", pt) == "pt=(3, 4)");
    }
}