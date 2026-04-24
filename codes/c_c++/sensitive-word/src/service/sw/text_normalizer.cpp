#include "sw/text_normalizer.h"
#include "opencc.h"
#include "utf8.h"
#include <utility>
#include <iterator>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

namespace sensitive_word {
namespace                {

//////////////////////////////////////////////////////////////
// 原始代码点结构体
// 用于保存 UTF-8 解码后的代码点及其在原始文本中的字节区间
//////////////////////////////////////////////////////////////
struct raw_code_point
{
    char32_t value      = 0;  // 解码得到的代码点
    size_t   byte_begin = 0;  // 该代码点在原始文本中的起始字节位置
    size_t   byte_end   = 0;  // 该代码点在原始文本中的结束字节位置(右开区间)
};

/**
 * @brief 检查字符是否为 CJK 汉字代码点
 *        这里的“CJK”仅指汉字/兼容汉字本体，用于中文分词和敏感词匹配中的“字”判断；
 *        不包含中文标点、部首、注音符号、日文假名、韩文等其它东亚文字。
 * @param ch 输入字符
 * @return true 字符是 CJK 汉字或兼容汉字，否则返回 false
 */
constexpr bool is_cjk(char32_t ch)
{
    return (ch >= 0x4E00  && ch <= 0x9FFF)  ||  // CJK Unified Ideographs
           (ch >= 0x3400  && ch <= 0x4DBF)  ||  // CJK Unified Ideographs Extension A
           (ch >= 0x20000 && ch <= 0x2A6DF) ||  // CJK Unified Ideographs Extension B
           (ch >= 0x2A700 && ch <= 0x2B73F) ||  // CJK Unified Ideographs Extension C
           (ch >= 0x2B740 && ch <= 0x2B81F) ||  // CJK Unified Ideographs Extension D
           (ch >= 0x2B820 && ch <= 0x2CEAF) ||  // CJK Unified Ideographs Extension E
           (ch >= 0x2CEB0 && ch <= 0x2EBEF) ||  // CJK Unified Ideographs Extension F
           (ch >= 0x30000 && ch <= 0x3134F) ||  // CJK Unified Ideographs Extension G
           (ch >= 0x31350 && ch <= 0x323AF) ||  // CJK Unified Ideographs Extension H
           (ch >= 0x2EBF0 && ch <= 0x2EE5D) ||  // CJK Unified Ideographs Extension I
           (ch >= 0xF900  && ch <= 0xFAFF)  ||  // CJK Compatibility Ideographs
           (ch >= 0x2F800 && ch <= 0x2FA1F);    // CJK Compatibility Ideographs Supplement
}

/**
 * @brief 折叠全角/半角宽度差异
 *        仅处理“单字符可无损近似折叠”的宽度形式：
 *        - 全角空格、全角 ASCII 符号
 *        不处理：
 *        - 全角英文字母（由 fold_english_style 处理）
 *        - 全角数字（由 fold_num_style 处理）
 * @param ch 输入字符
 * @return 折叠后的字符
 */
constexpr char32_t fold_width(char32_t ch)
{
    if (ch == 0x3000)
        return U' ';

    // 全角符号 → 半角符号 (U+FF01..0xFF5E，排除数字和英文字母)
    if ((ch >= 0xFF01 && ch <= 0xFF0F) ||
        (ch >= 0xFF1A && ch <= 0xFF20) ||
        (ch >= 0xFF3B && ch <= 0xFF40) ||
        (ch >= 0xFF5B && ch <= 0xFF5E))
        return ch - 0xFEE0;

    return ch;
}

/**
 * @brief 折叠大小写字符
 *        仅执行基础 ASCII A-Z 到 a-z 转换
 * @param ch 输入字符
 * @return 折叠后的字符
 */
constexpr char32_t fold_ascii_case(char32_t ch)
{
    if (ch >= U'A' && ch <= U'Z')
        return ch - U'A' + U'a';

    return ch;
}

/**
 * @brief 折叠英文样式字符
 *        面向中文环境下的“常用英文样式规避”：
 *        - 全角英文字母
 *        - 带圈字母
 *        - 数学字母样式（粗体/斜体/花体/哥特体/双线体/无衬线/等宽体等）
 *        - BMP 中若干与数学字母样式等价的兼容字母符号（如 ℂ ℍ ℕ ℙ ℚ ℝ ℤ）
 *
 *        说明：
 *        1. 该函数目标是将“视觉样式不同、语义仍是英文字母”的单字符折叠回普通拉丁字母；
 *        2. 不处理全角数字（由 fold_num_style 处理）；
 *        3. 不处理全角符号（由 fold_width 处理）；
 *        4. 不追求完整 Unicode 兼容分解，只覆盖常见英文规避写法。
 * @param ch 输入字符
 * @return 折叠后的字符
 */
constexpr char32_t fold_english_style(char32_t ch)
{
    // Letterlike Symbols / 兼容字母：这些字符在视觉和语义上等价于英文字母，
    // 但没有落在连续的 SMP 数学字母区间中，需要单独映射。
    switch (ch)
    {
    case 0x2102: return U'C'; // ℂ DOUBLE-STRUCK CAPITAL C
    case 0x210A: return U'g'; // ℊ SCRIPT SMALL G
    case 0x210B: return U'H'; // ℋ SCRIPT CAPITAL H
    case 0x210C: return U'H'; // ℌ FRAKTUR CAPITAL H
    case 0x210D: return U'H'; // ℍ DOUBLE-STRUCK CAPITAL H
    case 0x2110: return U'I'; // ℐ SCRIPT CAPITAL I
    case 0x2111: return U'I'; // ℑ FRAKTUR CAPITAL I
    case 0x2112: return U'L'; // ℒ SCRIPT CAPITAL L
    case 0x2113: return U'l'; // ℓ SCRIPT SMALL L
    case 0x2115: return U'N'; // ℕ DOUBLE-STRUCK CAPITAL N
    case 0x2119: return U'P'; // ℙ DOUBLE-STRUCK CAPITAL P
    case 0x211A: return U'Q'; // ℚ DOUBLE-STRUCK CAPITAL Q
    case 0x211B: return U'R'; // ℛ SCRIPT CAPITAL R
    case 0x211C: return U'R'; // ℜ FRAKTUR CAPITAL R
    case 0x211D: return U'R'; // ℝ DOUBLE-STRUCK CAPITAL R
    case 0x2124: return U'Z'; // ℤ DOUBLE-STRUCK CAPITAL Z
    case 0x2128: return U'Z'; // ℨ FRAKTUR CAPITAL Z
    case 0x212A: return U'K'; // K KELVIN SIGN，常见兼容英文 K
    case 0x212C: return U'B'; // ℬ SCRIPT CAPITAL B
    case 0x212D: return U'C'; // ℭ FRAKTUR CAPITAL C
    case 0x212F: return U'e'; // ℯ SCRIPT SMALL E
    case 0x2130: return U'E'; // ℰ SCRIPT CAPITAL E
    case 0x2131: return U'F'; // ℱ SCRIPT CAPITAL F
    case 0x2133: return U'M'; // ℳ SCRIPT CAPITAL M
    case 0x2134: return U'o'; // ℴ SCRIPT SMALL O
    default: break;
    }
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

    // 数学斜体 (Mathematical Italic)。
    if (ch >= 0x1D434 && ch <= 0x1D44D)
    {
        if (ch == 0x1D439)
            return ch;
        return static_cast<char32_t>(U'A' + (ch - 0x1D434 - (ch > 0x1D439 ? 1 : 0)));
    }

    if (ch >= 0x1D44E && ch <= 0x1D467)
        return static_cast<char32_t>(U'a' + (ch - 0x1D44E));

    // 数学粗斜体 (Mathematical Bold Italic) — 无空洞
    if (ch >= 0x1D468 && ch <= 0x1D481)
        return static_cast<char32_t>(U'A' + (ch - 0x1D468));

    if (ch >= 0x1D482 && ch <= 0x1D49B)
        return static_cast<char32_t>(U'a' + (ch - 0x1D482));

    // 数学花体 (Mathematical Script)
    //    大写空洞对应的兼容字母在 BMP 中单独编码，已在函数开头处理：
    //    1D49D,1D4A0,1D4A1,1D4A3,1D4A4,1D4A7,1D4A8,1D4AD
    //    小写空洞: 1D4BA,1D4BC（当前保持原样，不做多余猜测）
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

    // 数学哥特体 (Mathematical Fraktur)。
    // 大写空洞对应的兼容字母在 BMP 中单独编码，已在函数开头处理。
    if (ch >= 0x1D504 && ch <= 0x1D51C)
    {
        if (ch == 0x1D506)
            return ch;
        return static_cast<char32_t>(U'A' + (ch - 0x1D504 - (ch > 0x1D506 ? 1 : 0)));
    }

    if (ch >= 0x1D51E && ch <= 0x1D537)
        return static_cast<char32_t>(U'a' + (ch - 0x1D51E));

    // 数学粗哥特体 (Mathematical Bold Fraktur) — 无空洞
    if (ch >= 0x1D56C && ch <= 0x1D585)
        return static_cast<char32_t>(U'A' + (ch - 0x1D56C));

    if (ch >= 0x1D586 && ch <= 0x1D59F)
        return static_cast<char32_t>(U'a' + (ch - 0x1D586));

    // 数学双线体 (Mathematical Double-struck)。
    // 大写空洞对应的兼容字母在 BMP 中单独编码，已在函数开头处理。
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

    if (ch >= 0x1D552 && ch <= 0x1D56B)
        return static_cast<char32_t>(U'a' + (ch - 0x1D552));

    // 数学无衬线体 (Mathematical Sans-serif) — 无空洞
    if (ch >= 0x1D5A0 && ch <= 0x1D5B9)
        return static_cast<char32_t>(U'A' + (ch - 0x1D5A0));

    if (ch >= 0x1D5BA && ch <= 0x1D5D3)
        return static_cast<char32_t>(U'a' + (ch - 0x1D5BA));

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
 *        仅将能够“无歧义映射为单个十进制数字 0-9”的单字符数字样式
 *        折叠为 ASCII 数字。
 *
 *        设计边界：
 *        - 处理：全角数字、上下标数字、数学样式数字、部分带圈/括号/装饰数字、
 *                常见中文单数字（零~九、壹~玖、两/兩 等）
 *        - 不处理：表示多位数的单字符（如 ⑩、⑪、⑳）、序号语义字符、
 *                  中文复合数词（十、百、千、万、亿、廿、卅 等）
 *        说明：将 ○ / ◯ 视为数字 0 是面向文本过滤/规避场景的策略性折叠，
 *        并非严格的通用数值语义规则。
 * @param ch 输入字符
 * @return 折叠后的字符
 */
constexpr char32_t fold_num_style(char32_t ch)
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
// 用于在归一化阶段执行繁体到简体的转换
//////////////////////////////////////////////////////////////
class opencc_t2s_converter
{
public:
    opencc_t2s_converter(void)
        : converter_(nullptr)
    {
        try
        {
            converter_ = std::make_unique<opencc::SimpleConverter>("t2s.json", opencc_search_paths());
        }
        catch (...)
        {
        }
    }

    /**
     * @brief 将整段文本按 OpenCC 规则转换为简体
     *        当整句繁简转换导致长度变化时，后续只能近似对齐原始字节位置与归一化字符
     * @param text 输入文本
     * @return 转换后的文本；如果 OpenCC 不可用或转换失败则返回原文本
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
     * @brief 将单个字符按 OpenCC 规则转换为简体
     *        单字符 fallback 不依赖上下文，因此只保证保守的一对一近似转换
     * @param ch 输入字符
     * @return 转换后的字符；如果无法转换则返回原字符
     */
    char32_t convert_char(char32_t ch) const
    {
        if (!is_cjk(ch))
            return ch;

        if (auto it = char_cache_.find(ch); it != char_cache_.end())
            return it->second;

        char32_t result = ch;
        const std::u32string converted = decode_utf8(convert_text(encode_utf8(ch)));
        if (!converted.empty())
            result = converted.front();

        char_cache_[ch] = result;

        return result;
    }

private:
    std::unique_ptr<opencc::SimpleConverter>       converter_;
    mutable std::unordered_map<char32_t, char32_t> char_cache_;
};

/**
 * @brief 解码 UTF-8 字符串并保留每个代码点对应的原始字节区间
 * @param text 输入 UTF-8 字符串
 * @return 带原始字节位置信息的代码点序列
 */
std::vector<raw_code_point> decode_utf8_with_positions(std::string_view text)
{
    std::vector<raw_code_point> result;
    // 使用字节长度预分配容量来空间换时间：
    // UTF-8 字符通常占 1~4 字节，预分配 text.size() 可能会浪费些许内存，
    // 但可完全避免 vector 在插入过程中的重分配和内存拷贝开销。
    result.reserve(text.size());

    auto it = text.begin();
    while (it != text.end())
    {
        const auto   begin_it = it;
        const size_t begin    = static_cast<size_t>(begin_it - text.begin());

        try
        {
            const char32_t cp  = utf8::next(it, text.end());
            const size_t   end = static_cast<size_t>(it - text.begin());
            result.push_back({cp, begin, end});
        }
        catch (...)
        {
            it = begin_it;
            ++it;

            const size_t end = static_cast<size_t>(it - text.begin());
            result.push_back({0xFFFD, begin, end});
        }
    }

    return result;
}

} // namespace

//////////////////////////////////////////////////////////////
// 文本归一化器实现
// 用于执行字符折叠、繁简转换以及位置映射构建
//////////////////////////////////////////////////////////////
class text_normalizer::impl
{
public:
    explicit impl(text_normalizer_options options)
        : options_(std::move(options))
        , chinese_converter_()
    {
    }

    /**
     * @brief 对代码点进行归一化处理
     * @param code_point 输入代码点
     * @return 归一化后的代码点
     */
    char32_t normalize_code_point(char32_t code_point) const
    {
        char32_t result = code_point;

        if (options_.ignore_width)
            result = fold_width(result);

        if (options_.ignore_english_style)
            result = fold_english_style(result);

        if (options_.ignore_num_style)
            result = fold_num_style(result);

        if (options_.ignore_chinese_style)
            result = chinese_converter_.convert_char(result);

        if (options_.ignore_case)
            result = fold_ascii_case(result);

        return result;
    }

    /**
     * @brief 对单词进行归一化处理
     * @param word 输入单词
     * @return 归一化后的单词
     */
    std::u32string normalize_word(std::string_view word) const
    {
        // 词条路径只关心最终归一化后的字符序列，
        // 不需要保留原始字节位置，因此可以直接先做整段繁转简，再继续后续字符折叠。
        const std::u32string decoded = options_.ignore_chinese_style
                                        ? decode_utf8(chinese_converter_.convert_text(word))
                                        : decode_utf8(word);

        std::u32string normalized;
        normalized.reserve(decoded.size());

        std::transform(decoded.begin(), decoded.end(), std::back_inserter(normalized),
                        [this](char32_t ch) {
                            return normalize_non_chinese_code_point(ch);
                        });

        return normalized;
    }

    /**
     * @brief 对文本进行归一化处理
     * @param text 输入文本
     * @return 包含归一化字符及原始字节位置映射的文本结果
     */
    normalized_text normalize_text(std::string_view text) const
    {
        const auto raw_code_points = decode_utf8_with_positions(text);

        normalized_text result;
        result.normalized_chars.reserve(raw_code_points.size());

        std::u32string chinese_normalized_code_points;
        if (options_.ignore_chinese_style)
            chinese_normalized_code_points = decode_utf8(chinese_converter_.convert_text(text));

        // 正文路径优先尝试“整句繁转简后按索引对齐”，尽量利用 OpenCC 的上下文能力；
        // 只有转换前后长度变化时，才退化为逐字符转换。
        const bool can_align_by_index =!options_.ignore_chinese_style || chinese_normalized_code_points.size() == raw_code_points.size();
        for (size_t idx = 0; idx < raw_code_points.size(); ++idx)
        {
            const auto& raw                   = raw_code_points[idx];
            char32_t    normalized_code_point = raw.value;
            if (options_.ignore_chinese_style)
            {
                if (can_align_by_index)
                    normalized_code_point = chinese_normalized_code_points[idx];
                else
                    normalized_code_point = chinese_converter_.convert_char(raw.value);
            }

            // 这里始终保留原始字节区间，只替换 normalized_code_point，
            // 后续匹配结果才能同时兼顾“归一化匹配”和“原文定位”。
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
    /**
     * @brief 对已完成中文繁简处理的代码点继续执行其余归一化步骤
     * @param code_point 输入代码点
     * @return 归一化后的代码点
     */
    char32_t normalize_non_chinese_code_point(char32_t code_point) const
    {
        // 这里故意不再处理中文繁简，
        // 因为中文相关转换要么已经在整句路径完成，要么已经在单字符 fallback 中完成。
        char32_t result = code_point;

        if (options_.ignore_width)
            result = fold_width(result);

        if (options_.ignore_english_style)
            result = fold_english_style(result);

        if (options_.ignore_num_style)
            result = fold_num_style(result);

        if (options_.ignore_case)
            result = fold_ascii_case(result);

        return result;
    }

private:
    text_normalizer_options options_;
    opencc_t2s_converter    chinese_converter_;
};

text_normalizer::text_normalizer(text_normalizer_options options)
    : impl_(std::make_unique<impl>(std::move(options)))
{
}

text_normalizer::~text_normalizer() = default;

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

bool is_word_like_code_point(char32_t ch)
{
    return is_ascii_alnum(ch) || is_cjk(ch);
}

std::string encode_utf8(char32_t cp)
{
    std::string out;
    out.reserve(4);
    utf8::append(cp, out);
    return out;
}

std::string encode_utf8(const std::u32string& text)
{
    return utf8::utf32to8(text);
}

std::u32string decode_utf8(std::string_view text)
{
    std::u32string result;
    // 使用字节长度预分配容量来空间换时间：
    // UTF-8 字符通常占 1~4 字节，预分配 text.size() 可能会浪费些许内存，
    // 但可完全避免 vector 在插入过程中的重分配和内存拷贝开销。
    result.reserve(text.size());

    try
    {
        utf8::utf8to32(text.begin(), text.end(), std::back_inserter(result));
    }
    catch (...)
    {
        result.clear();
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
    }

    return result;
}

size_t count_code_points(std::string_view text)
{
    return decode_utf8(text).size();
}

std::vector<std::string> opencc_search_paths()
{
    const auto root = std::filesystem::path(".");
    return std::vector<std::string>{
        (root / "data" / "config").string(),
        (root / "data" / "dictionary").string(),
    };
}

} // namespace sensitive_word