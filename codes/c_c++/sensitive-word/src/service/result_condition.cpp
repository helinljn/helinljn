#include "result_condition.h"

#include "text_normalizer.h"
#include "utf8.h"

#include <cstddef>
#include <string_view>

namespace sensitive_word
{

char32_t previous_code_point(std::string_view text, std::size_t begin)
{
    if (begin == 0 || begin > text.size())
    {
        return 0;
    }

    auto text_begin = text.begin();
    auto it = text.begin() + static_cast<std::ptrdiff_t>(begin);

    try
    {
        return utf8::prior(it, text_begin);
    }
    catch (...)
    {
        return 0;
    }
}

char32_t next_code_point(std::string_view text, std::size_t end)
{
    if (end >= text.size())
    {
        return 0;
    }

    auto it = text.begin() + static_cast<std::ptrdiff_t>(end);

    try
    {
        return utf8::next(it, text.end());
    }
    catch (...)
    {
        return 0;
    }
}

bool contains_ascii_letter(std::string_view text)
{
    for (unsigned char ch : text)
    {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
        {
            return true;
        }
    }

    return false;
}

bool contains_ascii_digit_text(std::string_view text)
{
    for (unsigned char ch : text)
    {
        if (ch >= '0' && ch <= '9')
        {
            return true;
        }
    }

    return false;
}

bool is_ascii_word_boundary(char32_t ch)
{
    return !is_ascii_alnum(ch);
}

bool always_true_result_condition::match(const word_result&, std::string_view) const
{
    return true;
}

bool english_word_match_result_condition::match(
    const word_result& result,
    std::string_view text) const
{
    if (!contains_ascii_letter(result.normalized_word))
    {
        return true;
    }

    const char32_t left = previous_code_point(text, result.raw_begin);
    const char32_t right = next_code_point(text, result.raw_end);

    return is_ascii_word_boundary(left) && is_ascii_word_boundary(right);
}

bool english_word_num_match_result_condition::match(
    const word_result& result,
    std::string_view text) const
{
    if (!contains_ascii_letter(result.normalized_word) &&
        !contains_ascii_digit_text(result.normalized_word))
    {
        return true;
    }

    const char32_t left = previous_code_point(text, result.raw_begin);
    const char32_t right = next_code_point(text, result.raw_end);

    return is_ascii_word_boundary(left) && is_ascii_word_boundary(right);
}

}  // namespace sensitive_word
