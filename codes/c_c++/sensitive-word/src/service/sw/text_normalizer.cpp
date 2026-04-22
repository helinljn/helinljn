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
    return (ch >= 0x4E00  && ch <= 0x9FFF)  ||  // CJK Unified Ideographs
           (ch >= 0x3400  && ch <= 0x4DBF)  ||  // CJK Extension A
           (ch >= 0x20000 && ch <= 0x2A6DF) ||  // CJK Extension B
           (ch >= 0x2A700 && ch <= 0x2B73F) ||  // CJK Extension C
           (ch >= 0x2B740 && ch <= 0x2B81F) ||  // CJK Extension D
           (ch >= 0x2B820 && ch <= 0x2CEAF) ||  // CJK Extension E
           (ch >= 0x2CEB0 && ch <= 0x2EBEF) ||  // CJK Extension F
           (ch >= 0xF900  && ch <= 0xFAFF)  ||  // CJK Compatibility Ideographs
           (ch >= 0x2F800 && ch <= 0x2FA1F);    // CJK Compatibility Ideographs Supplement
}

/**
 * @brief 检查字符是否为拉丁扩展字符
 * @param ch 输入字符
 * @return true 字符是拉丁扩展字符，否则返回false
 */
bool is_latin_extended(char32_t ch)
{
    if (ch == 0x00D7 || ch == 0x00F7) // × ÷ 不是字母
        return false;

    return (ch >= 0x00C0 && ch <= 0x024F) || (ch >= 0x1E00 && ch <= 0x1EFF);
}

/**
 * @brief 折叠全角 ASCII 兼容字符
 * @param ch 输入字符
 * @return 折叠后的字符
 */
char32_t fold_width(char32_t ch)
{
    if (ch == 0x3000)
        return U' ';

    if (ch >= 0xFF01 && ch <= 0xFF5E)
        return ch - 0xFEE0;

    // 全角符号映射
    if (ch == 0xFFE5) return U'¥';  // 全角￥ → ¥
    if (ch == 0xFFE0) return U'¢';  // 全角￠ → ¢
    if (ch == 0xFFE1) return U'£';  // 全角￡ → £
    if (ch == 0xFFE4) return U'¦';  // 全角￤ → ¦

    return ch;
}

/**
 * @brief 折叠ASCII大小写字符
 *        当前仅忽略 ASCII 大小写，不处理希腊字母、德语 ß、土耳其语 I 等 Unicode 特例
 * @param ch 输入字符
 * @return 折叠后的字符
 */
char32_t fold_ascii_case(char32_t ch)
{
    if (ch >= U'A' && ch <= U'Z')
        return ch - U'A' + U'a';

    // Latin-1 Supplement 大写 → 小写（排除 U+00D7 × 和 U+00DF ß）
    // if ((ch >= 0x00C0 && ch <= 0x00D6) || (ch >= 0x00D8 && ch <= 0x00DE))
    //     return ch + 0x20;

    return ch;
}

/**
 * @brief 折叠英文样式字符
 *        覆盖常见样式，不保证完整 Unicode 数学字母集
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

    // 带圈字母 (U+249C-24B5 小写, U+24B6-24CF 大写, U+24D0-24E9 小写)
    if (ch >= 0x249C && ch <= 0x24B5)
        return static_cast<char32_t>(U'a' + (ch - 0x249C));

    if (ch >= 0x24B6 && ch <= 0x24CF)
        return static_cast<char32_t>(U'A' + (ch - 0x24B6));

    if (ch >= 0x24D0 && ch <= 0x24E9)
        return static_cast<char32_t>(U'a' + (ch - 0x24D0));

    // 数学粗体 (Mathematical Bold) — 无空洞
    if (ch >= 0x1D400 && ch <= 0x1D419)
        return static_cast<char32_t>(U'A' + (ch - 0x1D400));

    if (ch >= 0x1D41A && ch <= 0x1D433)
        return static_cast<char32_t>(U'a' + (ch - 0x1D41A));

    // 数学斜体 (Mathematical Italic) — 大写有空洞: 1D439→ℎ
    if (ch >= 0x1D434 && ch <= 0x1D44D)
        return static_cast<char32_t>(U'A' + (ch - 0x1D434));

    if (ch >= 0x1D44E && ch <= 0x1D467)
        return static_cast<char32_t>(U'a' + (ch - 0x1D44E));

    // 数学粗斜体 (Mathematical Bold Italic) — 无空洞
    if (ch >= 0x1D468 && ch <= 0x1D481)
        return static_cast<char32_t>(U'A' + (ch - 0x1D468));

    if (ch >= 0x1D482 && ch <= 0x1D49B)
        return static_cast<char32_t>(U'a' + (ch - 0x1D482));

    // 数学花体 (Mathematical Script)
    //    大写空洞: 1D49D,1D4A0,1D4A1,1D4A3,1D4A4,1D4A7,1D4A8,1D4AD
    //    小写空洞: 1D4BA,1D4BC
    if (ch >= 0x1D49C && ch <= 0x1D4B5)
    {
        if (ch == 0x1D49D || ch == 0x1D4AD
            || (ch >= 0x1D4A0 && ch <= 0x1D4A1)
            || (ch >= 0x1D4A3 && ch <= 0x1D4A4)
            || (ch >= 0x1D4A7 && ch <= 0x1D4A8))
            return ch;

        int offset = static_cast<int>(ch - 0x1D49C);
        int holes  = 0;

        if (ch > 0x1D49D) holes++;
        if (ch > 0x1D4A0) holes++;
        if (ch > 0x1D4A1) holes++;
        if (ch > 0x1D4A3) holes++;
        if (ch > 0x1D4A4) holes++;
        if (ch > 0x1D4A7) holes++;
        if (ch > 0x1D4A8) holes++;
        if (ch > 0x1D4AD) holes++;

        return static_cast<char32_t>(U'A' + offset - holes);
    }

    if (ch >= 0x1D4B6 && ch <= 0x1D4CF)
    {
        if (ch == 0x1D4BA || ch == 0x1D4BC)
            return ch;

        int offset = static_cast<int>(ch - 0x1D4B6);
        int holes  = 0;

        if (ch > 0x1D4BA) holes++;
        if (ch > 0x1D4BC) holes++;

        return static_cast<char32_t>(U'a' + offset - holes);
    }

    // 数学粗花体 (Mathematical Bold Script) — 无空洞
    if (ch >= 0x1D4D0 && ch <= 0x1D4E9)
        return static_cast<char32_t>(U'A' + (ch - 0x1D4D0));

    if (ch >= 0x1D4EA && ch <= 0x1D503)
        return static_cast<char32_t>(U'a' + (ch - 0x1D4EA));

    // 数学花体 (Mathematical Fraktur) — 大写空洞: 1D506→ℭ
    if (ch >= 0x1D504 && ch <= 0x1D51C)
    {
        if (ch == 0x1D506)
            return ch;
        return static_cast<char32_t>(U'A' + (ch - 0x1D504 - (ch > 0x1D506 ? 1 : 0)));
    }

    if (ch >= 0x1D51E && ch <= 0x1D537)
        return static_cast<char32_t>(U'a' + (ch - 0x1D51E));

    // 数学粗花体 (Mathematical Bold Fraktur) — 无空洞
    if (ch >= 0x1D56C && ch <= 0x1D585)
        return static_cast<char32_t>(U'A' + (ch - 0x1D56C));

    if (ch >= 0x1D586 && ch <= 0x1D59F)
        return static_cast<char32_t>(U'a' + (ch - 0x1D586));

    // 数学双线体 (Mathematical Double-struck) — 大写空洞: 1D53A,1D53F,1D545,1D547-1D549
    if (ch >= 0x1D538 && ch <= 0x1D54F)
    {
        if (ch == 0x1D53A || ch == 0x1D53F || ch == 0x1D545 || (ch >= 0x1D547 && ch <= 0x1D549))
            return ch;

        int offset = static_cast<int>(ch - 0x1D538);
        int holes  = 0;

        if (ch > 0x1D53A) holes++;
        if (ch > 0x1D53F) holes++;
        if (ch > 0x1D545) holes++;
        if (ch > 0x1D547) holes++;
        if (ch > 0x1D548) holes++;
        if (ch > 0x1D549) holes++;

        return static_cast<char32_t>(U'A' + offset - holes);
    }

    if (ch >= 0x1D550 && ch <= 0x1D56B)
        return static_cast<char32_t>(U'a' + (ch - 0x1D550));

    // 数学无衬线粗体 (Mathematical Sans-serif Bold) — 无空洞
    if (ch >= 0x1D5D4 && ch <= 0x1D5ED)
        return static_cast<char32_t>(U'A' + (ch - 0x1D5D4));

    if (ch >= 0x1D5EE && ch <= 0x1D607)
        return static_cast<char32_t>(U'a' + (ch - 0x1D5EE));

    // 数学无衬线斜体 (Mathematical Sans-serif Italic) — 无空洞
    if (ch >= 0x1D608 && ch <= 0x1D621)
        return static_cast<char32_t>(U'A' + (ch - 0x1D608));

    if (ch >= 0x1D622 && ch <= 0x1D63B)
        return static_cast<char32_t>(U'a' + (ch - 0x1D622));

    // 数学无衬线粗斜体 (Mathematical Sans-serif Bold Italic) — 无空洞
    if (ch >= 0x1D63C && ch <= 0x1D655)
        return static_cast<char32_t>(U'A' + (ch - 0x1D63C));

    if (ch >= 0x1D656 && ch <= 0x1D66F)
        return static_cast<char32_t>(U'a' + (ch - 0x1D656));

    // 数学等宽体 (Mathematical Monospace) — 无空洞
    if (ch >= 0x1D670 && ch <= 0x1D689)
        return static_cast<char32_t>(U'A' + (ch - 0x1D670));

    if (ch >= 0x1D68A && ch <= 0x1D6A3)
        return static_cast<char32_t>(U'a' + (ch - 0x1D68A));

    return ch;
}

/**
 * @brief 折叠数字样式字符
 *        仅将能够无歧义映射为单个十进制数字 0-9 的单字符样式数字折叠为 ASCII 数字；不处理表示多位数、序号或复合数值的单字符
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

//////////////////////////////////////////////////////////////
// OpenCC 转换器
// 用于将文本从繁体转换为简体
//////////////////////////////////////////////////////////////
class opencc_t2s_converter
{
public:
    opencc_t2s_converter(void)
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

    /**
     * @brief 将文本从繁体转换为简体
     *        当整句繁简转换长度变化时，字节位置与归一化字符的一一映射只能做近似处理
     * @param text 输入文本
     * @return 转换后的文本
     */
    std::string convert_text(std::string_view text) const
    {
        if (!converter_)
            return std::string(text);

        try
        {
            return converter_->Convert(std::string(text));
        }
        catch (...)
        {
            return std::string(text);
        }
    }

    /**
     * @brief 将字符从繁体转换为简体
     *        单字符 fallback 是保守退化，不保证上下文语义最优
     * @param ch 输入字符
     * @return 转换后的字符
     */
    char32_t convert_char(char32_t ch) const
    {
        const std::u32string converted = decode_utf8(convert_text(encode_utf8(ch)));
        if (!converted.empty())
            return converted.front();

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

} // namespace

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

text_normalizer::~text_normalizer(void) = default;

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