#include "sw/text_normalizer.h"
#include "opencc.h"
#include "utf8.h"
#include <utility>
#include <filesystem>

namespace sensitive_word {
namespace                {

//////////////////////////////////////////////////////////////
// 原始代码点结构体
// 存储原始代码点的信息，包括代码点值、字节开始位置、字节结束位置，用于后续的文本处理
//////////////////////////////////////////////////////////////
struct raw_code_point
{
    char32_t value      = 0;  // 代码点值
    size_t   byte_begin = 0;  // 字节开始位置
    size_t   byte_end   = 0;  // 字节结束位置
};

/**
 * @brief 检查字符是否为CJK字符
 * @param ch 输入字符
 * @return true 字符是CJK字符，否则返回false
 */
bool is_cjk(char32_t ch)
{
    return (ch >= 0x4E00 && ch <= 0x9FFF) ||
           (ch >= 0x3400 && ch <= 0x4DBF) ||
           (ch >= 0xF900 && ch <= 0xFAFF);
}

/**
 * @brief 检查字符是否为拉丁扩展字符
 * @param ch 输入字符
 * @return true 字符是拉丁扩展字符，否则返回false
 */
bool is_latin_extended(char32_t ch)
{
    return (ch >= 0x00C0 && ch <= 0x024F) ||
           (ch >= 0x1E00 && ch <= 0x1EFF);
}

/**
 * @brief 折叠宽度字符
 * @param ch 输入字符
 * @return 折叠后的字符
 */
char32_t fold_width(char32_t ch)
{
    if (ch == 0x3000)
        return U' ';

    if (ch >= 0xFF01 && ch <= 0xFF5E)
        return ch - 0xFEE0;

    return ch;
}

/**
 * @brief 折叠ASCII大小写字符
 * @param ch 输入字符
 * @return 折叠后的字符
 */
char32_t fold_ascii_case(char32_t ch)
{
    if (ch >= U'A' && ch <= U'Z')
    {
        return ch - U'A' + U'a';
    }

    return ch;
}

/**
 * @brief 折叠英文样式字符
 * @param ch 输入字符
 * @return 折叠后的字符
 */
char32_t fold_english_style(char32_t ch)
{
    // 全角英文字母 (U+FF21-FF3A, U+FF41-FF5A)
    if (ch >= 0xFF21 && ch <= 0xFF3A)
        return static_cast<char32_t>(U'A' + (ch - 0xFF21));

    if (ch >= 0xFF41 && ch <= 0xFF5A)
        return static_cast<char32_t>(U'a' + (ch - 0xFF41));

    // 带圈字母 (U+249C-24B5, U+24B6-24CF, U+24D0-24E9)
    if (ch >= 0x249C && ch <= 0x24B5)
        return static_cast<char32_t>(U'a' + (ch - 0x249C));

    if (ch >= 0x24B6 && ch <= 0x24CF)
        return static_cast<char32_t>(U'A' + (ch - 0x24B6));

    if (ch >= 0x24D0 && ch <= 0x24E9)
        return static_cast<char32_t>(U'a' + (ch - 0x24D0));

    // 数学花式字母
    if (ch >= 0x1D400 && ch <= 0x1D419)
        return static_cast<char32_t>(U'A' + (ch - 0x1D400));

    if (ch >= 0x1D41A && ch <= 0x1D433)
        return static_cast<char32_t>(U'a' + (ch - 0x1D41A));

    if (ch >= 0x1D434 && ch <= 0x1D44D)
        return static_cast<char32_t>(U'A' + (ch - 0x1D434));

    if (ch >= 0x1D44E && ch <= 0x1D467)
        return static_cast<char32_t>(U'a' + (ch - 0x1D44E));

    if (ch >= 0x1D468 && ch <= 0x1D481)
        return static_cast<char32_t>(U'A' + (ch - 0x1D468));

    if (ch >= 0x1D482 && ch <= 0x1D49B)
        return static_cast<char32_t>(U'a' + (ch - 0x1D482));

    if (ch >= 0x1D4D0 && ch <= 0x1D4E9)
        return static_cast<char32_t>(U'A' + (ch - 0x1D4D0));

    if (ch >= 0x1D4EA && ch <= 0x1D503)
        return static_cast<char32_t>(U'a' + (ch - 0x1D4EA));

    if (ch >= 0x1D56C && ch <= 0x1D585)
        return static_cast<char32_t>(U'A' + (ch - 0x1D56C));

    if (ch >= 0x1D586 && ch <= 0x1D59F)
        return static_cast<char32_t>(U'a' + (ch - 0x1D586));

    // 无衬线字体
    if (ch >= 0x1D5D4 && ch <= 0x1D5ED)
        return static_cast<char32_t>(U'A' + (ch - 0x1D5D4));

    if (ch >= 0x1D5EE && ch <= 0x1D607)
        return static_cast<char32_t>(U'a' + (ch - 0x1D5EE));

    if (ch >= 0x1D608 && ch <= 0x1D621)
        return static_cast<char32_t>(U'A' + (ch - 0x1D608));

    if (ch >= 0x1D622 && ch <= 0x1D63B)
        return static_cast<char32_t>(U'a' + (ch - 0x1D622));

    if (ch >= 0x1D63C && ch <= 0x1D655)
        return static_cast<char32_t>(U'A' + (ch - 0x1D63C));

    if (ch >= 0x1D656 && ch <= 0x1D66F)
        return static_cast<char32_t>(U'a' + (ch - 0x1D656));

    // 等宽字体
    if (ch >= 0x1D670 && ch <= 0x1D689)
        return static_cast<char32_t>(U'A' + (ch - 0x1D670));

    if (ch >= 0x1D68A && ch <= 0x1D6A3)
        return static_cast<char32_t>(U'a' + (ch - 0x1D68A));

    return ch;
}

/**
 * @brief 折叠数字样式字符
 * @param ch 输入字符
 * @return 折叠后的字符
 */
char32_t fold_num_style(char32_t ch)
{
    // 只折叠能明确表示单个 0-9 数字的字符；
    // 像 ⑩、⑪、⑳ 这类单字符但表示多位数/大于 9 的数字，不在此处处理。
    if (ch >= 0xFF10 && ch <= 0xFF19)
        return static_cast<char32_t>(U'0' + (ch - 0xFF10));

    if (ch == 0x2070)
        return U'0';

    if (ch == 0x00B9)
        return U'1';

    if (ch == 0x00B2)
        return U'2';

    if (ch == 0x00B3)
        return U'3';

    if (ch >= 0x2074 && ch <= 0x2079)
        return static_cast<char32_t>(U'4' + (ch - 0x2074));

    if (ch >= 0x2080 && ch <= 0x2089)
        return static_cast<char32_t>(U'0' + (ch - 0x2080));

    // 数学样式数字：bold / double-struck / sans-serif / sans-serif bold / monospace
    if (ch >= 0x1D7CE && ch <= 0x1D7FF)
        return static_cast<char32_t>(U'0' + ((ch - 0x1D7CE) % 10));

    // 带圈/括号/句点/双圈/装饰数字
    if (ch == 0x24EA || ch == 0x24FF)
        return U'0';

    if (ch >= 0x2460 && ch <= 0x2468)
        return static_cast<char32_t>(U'1' + (ch - 0x2460));

    if (ch >= 0x2474 && ch <= 0x247C)
        return static_cast<char32_t>(U'1' + (ch - 0x2474));

    if (ch >= 0x2488 && ch <= 0x2490)
        return static_cast<char32_t>(U'1' + (ch - 0x2488));

    if (ch >= 0x24F5 && ch <= 0x24FD)
        return static_cast<char32_t>(U'1' + (ch - 0x24F5));

    if (ch >= 0x2780 && ch <= 0x2788)
        return static_cast<char32_t>(U'1' + (ch - 0x2780));

    if (ch >= 0x278A && ch <= 0x2792)
        return static_cast<char32_t>(U'1' + (ch - 0x278A));

    if (ch >= 0x278C && ch <= 0x2794)
        return static_cast<char32_t>(U'1' + (ch - 0x278C));

    // 中文/兼容汉字数字样式
    if (ch >= 0x3220 && ch <= 0x3228)
        return static_cast<char32_t>(U'1' + (ch - 0x3220));

    if (ch >= 0x3280 && ch <= 0x3288)
        return static_cast<char32_t>(U'1' + (ch - 0x3280));

    if (ch >= 0x3021 && ch <= 0x3029)
        return static_cast<char32_t>(U'1' + (ch - 0x3021));

    switch (ch)
    {
    case U'零':
    case U'〇':
    case U'○':
    case U'◯':
        return U'0';
    case U'一':
    case U'壹':
    case U'㈠':
    case U'㊀':
    case U'〡':
        return U'1';
    case U'二':
    case U'两':
    case U'兩':
    case U'贰':
    case U'貳':
    case U'㈡':
    case U'㊁':
    case U'〢':
        return U'2';
    case U'三':
    case U'叁':
    case U'參':
    case U'㈢':
    case U'㊂':
    case U'〣':
        return U'3';
    case U'四':
    case U'肆':
    case U'㈣':
    case U'㊃':
    case U'〤':
        return U'4';
    case U'五':
    case U'伍':
    case U'㈤':
    case U'㊄':
    case U'〥':
        return U'5';
    case U'六':
    case U'陆':
    case U'陸':
    case U'㈥':
    case U'㊅':
    case U'〦':
        return U'6';
    case U'七':
    case U'柒':
    case U'㈦':
    case U'㊆':
    case U'〧':
        return U'7';
    case U'八':
    case U'捌':
    case U'㈧':
    case U'㊇':
    case U'〨':
        return U'8';
    case U'九':
    case U'玖':
    case U'㈨':
    case U'㊈':
    case U'〩':
        return U'9';
    default:
        return ch;
    }
}

class opencc_t2s_converter
{
public:
    opencc_t2s_converter()
    {
        try
        {
            converter_ = std::make_unique<opencc::SimpleConverter>("t2s.json", opencc_search_paths());
        }
        catch (...)
        {
            converter_.reset();
        }
    }

    std::string convert_text(std::string_view text) const
    {
        if (!converter_)
        {
            return std::string(text);
        }

        try
        {
            return converter_->Convert(std::string(text));
        }
        catch (...)
        {
            return std::string(text);
        }
    }

    char32_t convert_char(char32_t ch) const
    {
        const std::u32string converted = decode_utf8(convert_text(encode_utf8(ch)));
        if (!converted.empty())
        {
            return converted.front();
        }

        return ch;
    }

private:
    std::unique_ptr<opencc::SimpleConverter> converter_ {};
};

std::vector<raw_code_point> decode_utf8_with_positions(std::string_view text)
{
    std::vector<raw_code_point> result;

    auto it = text.begin();
    while (it != text.end())
    {
        const auto begin_it = it;
        const size_t begin = static_cast<size_t>(begin_it - text.begin());

        try
        {
            const char32_t cp = utf8::next(it, text.end());
            const size_t end = static_cast<size_t>(it - text.begin());
            result.push_back(raw_code_point{cp, begin, end});
        }
        catch (...)
        {
            it = begin_it;
            ++it;
            const size_t end = static_cast<size_t>(it - text.begin());
            result.push_back(raw_code_point{0xFFFD, begin, end});
        }
    }

    return result;
}

std::vector<char32_t> decode_utf8_to_code_points(std::string_view text)
{
    const std::u32string decoded = decode_utf8(text);
    return std::vector<char32_t>(decoded.begin(), decoded.end());
}

}  // namespace

class text_normalizer::impl
{
public:
    explicit impl(text_normalizer_options options)
        : options_(std::move(options))
    {
    }

    char32_t normalize_code_point(char32_t code_point) const
    {
        char32_t result = code_point;

        if (options_.ignore_width)
        {
            result = fold_width(result);
        }

        if (options_.ignore_english_style)
        {
            result = fold_english_style(result);
        }

        if (options_.ignore_num_style)
        {
            result = fold_num_style(result);
        }

        if (options_.ignore_chinese_style)
        {
            result = chinese_converter_.convert_char(result);
        }

        if (options_.ignore_case)
        {
            result = fold_ascii_case(result);
        }

        return result;
    }

    std::u32string normalize_word(std::string_view word) const
    {
        const std::string chinese_normalized_word =
            options_.ignore_chinese_style ? chinese_converter_.convert_text(word) : std::string(word);

        std::u32string normalized;
        const std::u32string decoded = decode_utf8(chinese_normalized_word);
        normalized.reserve(decoded.size());

        for (char32_t ch : decoded)
        {
            normalized.push_back(normalize_non_chinese_code_point(ch));
        }

        return normalized;
    }

    normalized_text normalize_text(std::string_view text) const
    {
        normalized_text result;
        const auto raw_code_points = decode_utf8_with_positions(text);
        result.normalized_chars.reserve(raw_code_points.size());

        std::vector<char32_t> chinese_normalized_code_points;
        if (options_.ignore_chinese_style)
        {
            chinese_normalized_code_points = decode_utf8_to_code_points(chinese_converter_.convert_text(text));
        }

        const bool can_align_by_index =
            !options_.ignore_chinese_style ||
            chinese_normalized_code_points.size() == raw_code_points.size();

        for (size_t i = 0; i < raw_code_points.size(); ++i)
        {
            const auto& raw = raw_code_points[i];
            char32_t normalized_code_point = raw.value;

            if (options_.ignore_chinese_style)
            {
                if (can_align_by_index)
                {
                    normalized_code_point = chinese_normalized_code_points[i];
                }
                else
                {
                    normalized_code_point = chinese_converter_.convert_char(raw.value);
                }
            }

            result.normalized_chars.push_back(normalized_char{
                raw.value,
                normalize_non_chinese_code_point(normalized_code_point),
                raw.byte_begin,
                raw.byte_end,
            });
        }

        return result;
    }

private:
    char32_t normalize_non_chinese_code_point(char32_t code_point) const
    {
        char32_t result = code_point;

        if (options_.ignore_width)
        {
            result = fold_width(result);
        }

        if (options_.ignore_english_style)
        {
            result = fold_english_style(result);
        }

        if (options_.ignore_num_style)
        {
            result = fold_num_style(result);
        }

        if (options_.ignore_case)
        {
            result = fold_ascii_case(result);
        }

        return result;
    }

private:
    text_normalizer_options options_ {};
    opencc_t2s_converter chinese_converter_ {};
};

text_normalizer::text_normalizer(text_normalizer_options options)
    : impl_(std::make_unique<impl>(std::move(options)))
{
}

char32_t text_normalizer::normalize_code_point(char32_t code_point) const
{
    return impl_->normalize_code_point(code_point);
}

std::u32string text_normalizer::normalize_word(std::string_view word) const
{
    return impl_->normalize_word(word);
}

normalized_text text_normalizer::normalize_text(std::string_view text) const
{
    return impl_->normalize_text(text);
}

bool is_ascii_alpha(char32_t ch)
{
    return (ch >= U'a' && ch <= U'z') || (ch >= U'A' && ch <= U'Z');
}

bool is_ascii_digit(char32_t ch)
{
    return ch >= U'0' && ch <= U'9';
}

bool is_ascii_alnum(char32_t ch)
{
    return is_ascii_alpha(ch) || is_ascii_digit(ch);
}

bool is_word_like_code_point(char32_t ch)
{
    return is_ascii_alnum(ch) || is_cjk(ch) || is_latin_extended(ch);
}

bool is_ascii_word_boundary(char32_t ch)
{
    return !is_ascii_alnum(ch);
}

std::string encode_utf8(char32_t cp)
{
    std::string out;
    utf8::append(cp, out);
    return out;
}

std::string encode_utf8(const std::u32string& text)
{
    return utf8::utf32to8(text);
}

std::u32string decode_utf8(std::string_view text)
{
    try
    {
        return utf8::utf8to32(std::string(text));
    }
    catch (...)
    {
        std::u32string result;
        auto it = text.begin();
        while (it != text.end())
        {
            const auto begin = it;
            try
            {
                result.push_back(utf8::next(it, text.end()));
            }
            catch (...)
            {
                result.push_back(0xFFFD);
                it = begin;
                ++it;
            }
        }
        return result;
    }
}

size_t count_code_points(std::string_view text)
{
    return decode_utf8(text).size();
}

std::vector<std::string> opencc_search_paths(void)
{
    const auto root = std::filesystem::path(".");
    return std::vector<std::string>{
        (root / "data" / "config").string(),
        (root / "data" / "dictionary").string(),
    };
}

} // namespace sensitive_word