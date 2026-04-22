#include "sw/char_ignore.h"
#include "sw/text_normalizer.h"

namespace sensitive_word {

bool none_char_ignore::ignore(char32_t, char32_t) const
{
    return false;
}

bool special_char_ignore::ignore(char32_t, char32_t normalized_code_point) const
{
    return !is_word_like_code_point(normalized_code_point);
}

} // namespace sensitive_word