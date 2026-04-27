#include "doctest.h"
#include "utf8.h"
#include <string>
#include <vector>
#include <iterator>

TEST_SUITE("utfcpp usage")
{
    TEST_CASE("is_valid: 合法 UTF-8 字符串应返回 true")
    {
        /*
         * utfcpp 最基础的能力之一，就是校验一段字节串
         * 是否是合法的 UTF-8 编码。
         */
        const std::string text = "汉字ABC";

        CHECK(utf8::is_valid(text.begin(), text.end()));
        CHECK(utf8::is_valid(text));
    }

    TEST_CASE("find_invalid: 能够定位非法 UTF-8 序列的起始位置")
    {
        /*
         * 这里构造一个包含非法字节 0xFF 的字符串：
         *   'A' + 0xFF + 'B'
         *
         * 对 UTF-8 来说，0xFF 不是合法的起始字节。
         */
        std::string text = "A";
        text.push_back(static_cast<char>(0xFF));
        text += "B";

        const auto invalid_it = utf8::find_invalid(text.begin(), text.end());

        CHECK(invalid_it != text.end());
        CHECK(std::distance(text.begin(), invalid_it) == 1);
        CHECK(utf8::find_invalid(text) == 1);
        CHECK_FALSE(utf8::is_valid(text.begin(), text.end()));
        CHECK_FALSE(utf8::is_valid(text));
    }

    TEST_CASE("replace_invalid: 可以把非法 UTF-8 替换为指定字符")
    {
        /*
         * 对接收外部输入的系统来说，
         * 遇到非法 UTF-8 时有时不希望直接失败，
         * 而是希望做“容错修复”。
         *
         * 这里把非法字节替换成 '?'。
         */
        std::string text = "A";
        text.push_back(static_cast<char>(0xFF));
        text += "B";

        std::string repaired;
        utf8::replace_invalid(text.begin(), text.end(), std::back_inserter(repaired), '?');

        CHECK(repaired == "A?B");
        CHECK(utf8::is_valid(repaired.begin(), repaired.end()));
    }

    TEST_CASE("replace_invalid overload: 可直接返回修复后的字符串")
    {
        std::string text = "A";
        text.push_back(static_cast<char>(0xFF));
        text += "B";

        const std::string repaired = utf8::replace_invalid(text, '?');

        CHECK(repaired == "A?B");
        CHECK(utf8::is_valid(repaired));
    }

    TEST_CASE("distance: 统计 UTF-8 文本中的 Unicode code point 数量")
    {
        /*
         * distance 统计的是“字符个数（更准确地说是 code point 数量）”，
         * 而不是字节数。
         *
         * "汉A字"：
         *   - 汉
         *   - A
         *   - 字
         *
         * 一共 3 个 code point，但字节数会大于 3。
         */
        const std::string text = "汉A字";

        CHECK(text.size() > 3);
        CHECK(utf8::distance(text.begin(), text.end()) == 3);
    }

    TEST_CASE("utf8to16 / utf16to8: 演示 UTF-8 与 UTF-16 的双向转换")
    {
        /*
         * 这类 API 很适合：
         * - Windows 平台字符串处理
         * - 需要与 UTF-16 接口交互的业务逻辑
         */
        const std::string original = "汉字UTF-8";
        const std::u16string utf16 = utf8::utf8to16(original);
        const std::string roundtrip = utf8::utf16to8(utf16);

        CHECK(utf16 == u"汉字UTF-8");
        CHECK(roundtrip == original);
    }

    TEST_CASE("utf8to32 / utf32to8: 演示 UTF-8 与 UTF-32 的双向转换")
    {
        const std::string original = "汉A😀";
        const std::u32string utf32 = utf8::utf8to32(original);
        const std::string roundtrip = utf8::utf32to8(utf32);

        REQUIRE(utf32.size() == 3);
        CHECK(utf32[0] == U'汉');
        CHECK(utf32[1] == U'A');
        CHECK(utf32[2] == 0x1F600);
        CHECK(roundtrip == original);
    }

    TEST_CASE("append / append16: 从 code point 构造 UTF-8 与 UTF-16 文本")
    {
        std::string utf8_text;
        utf8::append(U'汉', utf8_text);
        utf8::append(U'A', utf8_text);
        utf8::append(0x1F600, utf8_text);

        CHECK(utf8_text == "汉A😀");

        std::u16string utf16_text;
        utf8::append16(U'汉', std::back_inserter(utf16_text));
        utf8::append16(U'A', std::back_inserter(utf16_text));
        utf8::append16(0x1F600, std::back_inserter(utf16_text));

        CHECK(utf16_text == u"汉A😀");
        CHECK(utf8::utf16to8(utf16_text) == utf8_text);
    }

    TEST_CASE("next / peek_next / prior: 按 Unicode code point 正向和反向遍历")
    {
        /*
         * utfcpp 的 next / prior 适合在“按字符而不是按字节遍历”时使用。
         * 这在做文本分析、敏感词扫描、截断显示时都很常见。
         */
        const std::string text = "汉A字";

        SUBCASE("peek_next: 读取下一个 code point 但不移动迭代器")
        {
            auto it = text.begin();
            const auto end = text.end();

            const auto cp = utf8::peek_next(it, end);

            CHECK(cp == 0x6C49);
            CHECK(it == text.begin());
        }

        SUBCASE("next: 从前向后读取 code point")
        {
            auto it = text.begin();
            const auto end = text.end();

            const auto cp1 = utf8::next(it, end);
            const auto cp2 = utf8::next(it, end);
            const auto cp3 = utf8::next(it, end);

            CHECK(cp1 == 0x6C49);
            CHECK(cp2 == 0x41);
            CHECK(cp3 == 0x5B57);
            CHECK(it == end);
        }

        SUBCASE("prior: 从后向前读取 code point")
        {
            auto it = text.end();
            const auto begin = text.begin();

            const auto cp1 = utf8::prior(it, begin);
            const auto cp2 = utf8::prior(it, begin);
            const auto cp3 = utf8::prior(it, begin);

            CHECK(cp1 == 0x5B57);
            CHECK(cp2 == 0x41);
            CHECK(cp3 == 0x6C49);
            CHECK(it == begin);
        }
    }

    TEST_CASE("advance: 按 code point 数量移动迭代器")
    {
        const std::string text = "汉A😀字";
        auto it = text.begin();

        utf8::advance(it, 2, text.end());
        CHECK(utf8::peek_next(it, text.end()) == 0x1F600);

        utf8::advance(it, -1, text.begin());
        CHECK(utf8::peek_next(it, text.end()) == 0x41);
    }

    TEST_CASE("next16: 可以按 code point 遍历 UTF-16 序列")
    {
        const std::u16string text = u"汉A😀";
        auto it = text.begin();

        const auto cp1 = utf8::next16(it, text.end());
        const auto cp2 = utf8::next16(it, text.end());
        const auto cp3 = utf8::next16(it, text.end());

        CHECK(cp1 == U'汉');
        CHECK(cp2 == U'A');
        CHECK(cp3 == 0x1F600);
        CHECK(it == text.end());
    }

    TEST_CASE("starts_with_bom: 能识别 UTF-8 BOM")
    {
        const std::string with_bom =
            std::string("\xEF\xBB\xBF", 3) + "汉字";
        const std::string without_bom = "汉字";

        CHECK(utf8::starts_with_bom(with_bom.begin(), with_bom.end()));
        CHECK(utf8::starts_with_bom(with_bom));
        CHECK_FALSE(utf8::starts_with_bom(without_bom.begin(), without_bom.end()));
        CHECK_FALSE(utf8::starts_with_bom(without_bom));
    }

    TEST_CASE("iterator: 适配为按 code point 的 STL 风格迭代器")
    {
        const std::string text = "汉A😀";

        utf8::iterator<std::string::const_iterator> it(text.begin(), text.begin(), text.end());
        utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

        std::vector<utf8::utfchar32_t> code_points;
        for (; it != end; ++it)
        {
            code_points.push_back(*it);
        }

        REQUIRE(code_points.size() == 3);
        CHECK(code_points[0] == U'汉');
        CHECK(code_points[1] == U'A');
        CHECK(code_points[2] == 0x1F600);
    }

    TEST_CASE("unchecked namespace: 在已知输入合法时可走无校验快速路径")
    {
        const std::string text = "汉A字";
        REQUIRE(utf8::is_valid(text));

        auto it = text.begin();
        const auto cp1 = utf8::unchecked::next(it);
        const auto cp2 = utf8::unchecked::next(it);
        const auto cp3 = utf8::unchecked::next(it);

        CHECK(cp1 == U'汉');
        CHECK(cp2 == U'A');
        CHECK(cp3 == U'字');
    }

    TEST_CASE("exception: next 遇到非法 UTF-8 会抛出 invalid_utf8")
    {
        std::string text(1, static_cast<char>(0xFF));
        auto it = text.begin();

        CHECK_THROWS_AS(utf8::next(it, text.end()), utf8::invalid_utf8);
    }

    TEST_CASE("exception: next 遇到截断序列会抛出 not_enough_room")
    {
        std::string text;
        text.push_back(static_cast<char>(0xE6));
        text.push_back(static_cast<char>(0xB1));

        auto it = text.begin();

        CHECK_THROWS_AS(utf8::next(it, text.end()), utf8::not_enough_room);
    }

    TEST_CASE("exception: append 遇到非法 code point 会抛出 invalid_code_point")
    {
        std::string text;

        CHECK_THROWS_AS(utf8::append(0x110000, text), utf8::invalid_code_point);
    }

    TEST_CASE("exception: utf16to8 遇到非法 UTF-16 会抛出 invalid_utf16")
    {
        const std::u16string broken = {0xD800};

        CHECK_THROWS_AS(utf8::utf16to8(broken), utf8::invalid_utf16);
    }
}