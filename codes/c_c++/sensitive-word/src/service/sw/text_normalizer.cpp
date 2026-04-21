#include "sw/text_normalizer.h"
#include "opencc.h"
#include "utf8.h"

#include <filesystem>
#include <memory>
#include <utility>

namespace sensitive_word
{

namespace
{

bool is_cjk(char32_t ch)
{
    return (ch >= 0x4E00 && ch <= 0x9FFF) ||
           (ch >= 0x3400 && ch <= 0x4DBF) ||
           (ch >= 0xF900 && ch <= 0xFAFF);
}

bool is_latin_extended(char32_t ch)
{
    return (ch >= 0x00C0 && ch <= 0x024F) ||
           (ch >= 0x1E00 && ch <= 0x1EFF);
}

char32_t fold_width(char32_t ch)
{
    if (ch == 0x3000)
    {
        return U' ';
    }

    if (ch >= 0xFF01 && ch <= 0xFF5E)
    {
        return ch - 0xFEE0;
    }

    return ch;
}

char32_t fold_ascii_case(char32_t ch)
{
    if (ch >= U'A' && ch <= U'Z')
    {
        return ch - U'A' + U'a';
    }

    return ch;
}

char32_t fold_english_style(char32_t ch)
{
    if (ch >= 0x249C && ch <= 0x24B5)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x249C));
    }

    if (ch >= 0x24B6 && ch <= 0x24CF)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x24B6));
    }

    if (ch >= 0x24D0 && ch <= 0x24E9)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x24D0));
    }

    if (ch >= 0x1D400 && ch <= 0x1D419)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D400));
    }

    if (ch >= 0x1D41A && ch <= 0x1D433)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D41A));
    }

    if (ch >= 0x1D434 && ch <= 0x1D44D)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D434));
    }

    if (ch >= 0x1D44E && ch <= 0x1D467)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D44E));
    }

    if (ch >= 0x1D468 && ch <= 0x1D481)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D468));
    }

    if (ch >= 0x1D482 && ch <= 0x1D49B)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D482));
    }

    if (ch >= 0x1D4D0 && ch <= 0x1D4E9)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D4D0));
    }

    if (ch >= 0x1D4EA && ch <= 0x1D503)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D4EA));
    }

    if (ch >= 0x1D56C && ch <= 0x1D585)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D56C));
    }

    if (ch >= 0x1D586 && ch <= 0x1D59F)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D586));
    }

    if (ch >= 0x1D5D4 && ch <= 0x1D5ED)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D5D4));
    }

    if (ch >= 0x1D5EE && ch <= 0x1D607)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D5EE));
    }

    if (ch >= 0x1D608 && ch <= 0x1D621)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D608));
    }

    if (ch >= 0x1D622 && ch <= 0x1D63B)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D622));
    }

    if (ch >= 0x1D63C && ch <= 0x1D655)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D63C));
    }

    if (ch >= 0x1D656 && ch <= 0x1D66F)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D656));
    }

    if (ch >= 0x1D670 && ch <= 0x1D689)
    {
        return static_cast<char32_t>(U'A' + (ch - 0x1D670));
    }

    if (ch >= 0x1D68A && ch <= 0x1D6A3)
    {
        return static_cast<char32_t>(U'a' + (ch - 0x1D68A));
    }

    return ch;
}

char32_t fold_num_style(char32_t ch)
{
    if (ch >= 0x2460 && ch <= 0x2468)
    {
        return static_cast<char32_t>(U'1' + (ch - 0x2460));
    }

    if (ch == 0x2469 || ch == 0x24EA || ch == 0x24FF)
    {
        return U'0';
    }

    if (ch >= 0x2474 && ch <= 0x247C)
    {
        return static_cast<char32_t>(U'1' + (ch - 0x2474));
    }

    if (ch >= 0x2488 && ch <= 0x2490)
    {
        return static_cast<char32_t>(U'1' + (ch - 0x2488));
    }

    if (ch >= 0x2780 && ch <= 0x2788)
    {
        return static_cast<char32_t>(U'1' + (ch - 0x2780));
    }

    if (ch >= 0xFF10 && ch <= 0xFF19)
    {
        return static_cast<char32_t>(U'0' + (ch - 0xFF10));
    }

    if (ch == 0x2070)
    {
        return U'0';
    }

    if (ch == 0x00B9)
    {
        return U'1';
    }

    if (ch == 0x00B2)
    {
        return U'2';
    }

    if (ch == 0x00B3)
    {
        return U'3';
    }

    if (ch >= 0x2074 && ch <= 0x2079)
    {
        return static_cast<char32_t>(U'4' + (ch - 0x2074));
    }

    if (ch >= 0x2080 && ch <= 0x2089)
    {
        return static_cast<char32_t>(U'0' + (ch - 0x2080));
    }

    switch (ch)
    {
    case U'零':
    case U'〇':
        return U'0';
    case U'一':
    case U'壹':
        return U'1';
    case U'二':
    case U'两':
    case U'兩':
    case U'贰':
    case U'貳':
        return U'2';
    case U'三':
    case U'叁':
    case U'參':
        return U'3';
    case U'四':
    case U'肆':
        return U'4';
    case U'五':
    case U'伍':
    case U'㈤':
    case U'㊄':
        return U'5';
    case U'六':
    case U'陆':
    case U'陸':
        return U'6';
    case U'七':
    case U'柒':
        return U'7';
    case U'八':
    case U'捌':
        return U'8';
    case U'九':
    case U'玖':
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

    char32_t convert_char(char32_t ch) const
    {
        if (!converter_)
        {
            return ch;
        }

        try
        {
            const std::string input = encode_utf8(ch);
            const std::string output = converter_->Convert(input);
            const std::u32string converted = decode_utf8(output);
            if (!converted.empty())
            {
                return converted.front();
            }
        }
        catch (...)
        {
        }

        return ch;
    }

private:
    std::unique_ptr<opencc::SimpleConverter> converter_ {};
};

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
        std::u32string normalized;
        const std::u32string decoded = decode_utf8(word);
        normalized.reserve(decoded.size());

        for (char32_t ch : decoded)
        {
            normalized.push_back(normalize_code_point(ch));
        }

        return normalized;
    }

    normalized_text normalize_text(std::string_view text) const
    {
        normalized_text result;
        result.raw_text = std::string(text);
        result.raw_code_points = decode_utf8_with_positions(text);
        result.normalized_chars.reserve(result.raw_code_points.size());

        for (const auto& raw : result.raw_code_points)
        {
            result.normalized_chars.push_back(normalized_char{
                raw.value,
                normalize_code_point(raw.value),
                raw.byte_begin,
                raw.byte_end,
            });
        }

        return result;
    }

private:
    text_normalizer_options options_ {};
    opencc_t2s_converter chinese_converter_ {};
};

text_normalizer::text_normalizer(text_normalizer_options options)
    : impl_(std::make_shared<impl>(std::move(options)))
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

std::vector<raw_code_point> decode_utf8_with_positions(std::string_view text)
{
    std::vector<raw_code_point> result;

    auto it = text.begin();
    while (it != text.end())
    {
        const auto begin_it = it;
        const std::size_t begin = static_cast<std::size_t>(begin_it - text.begin());

        try
        {
            const char32_t cp = utf8::next(it, text.end());
            const std::size_t end = static_cast<std::size_t>(it - text.begin());
            result.push_back(raw_code_point{cp, begin, end});
        }
        catch (...)
        {
            it = begin_it;
            ++it;
            const std::size_t end = static_cast<std::size_t>(it - text.begin());
            result.push_back(raw_code_point{0xFFFD, begin, end});
        }
    }

    return result;
}

std::vector<std::string> opencc_search_paths()
{
    const auto root = std::filesystem::path(".");
    return {
        (root / "data" / "config").string(),
        (root / "data" / "dictionary").string(),
    };
}

}  // namespace sensitive_word
