#include "sw/result_condition.h"
#include "sw/text_normalizer.h"

namespace sensitive_word {

bool always_true_result_condition::match(const word_result&, std::string_view) const
{
    return true;
}

bool english_word_match_result_condition::match(const word_result& result, std::string_view) const
{
    if (!contains_ascii_letter(result.normalized_word))
        return true;

    return is_ascii_word_boundary(result.left_normalized_code_point) &&
           is_ascii_word_boundary(result.right_normalized_code_point);
}

bool english_word_num_match_result_condition::match(const word_result& result, std::string_view) const
{
    if (!contains_ascii_letter(result.normalized_word) && !contains_ascii_digit_text(result.normalized_word))
        return true;

    return is_ascii_word_boundary(result.left_normalized_code_point) &&
           is_ascii_word_boundary(result.right_normalized_code_point);
}

bool contains_ascii_letter(std::string_view text)
{
    for (unsigned char ch : text)
    {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
            return true;
    }

    return false;
}

bool contains_ascii_digit_text(std::string_view text)
{
    for (unsigned char ch : text)
    {
        if (ch >= '0' && ch <= '9')
            return true;
    }

    return false;
}

} // namespace sensitive_word