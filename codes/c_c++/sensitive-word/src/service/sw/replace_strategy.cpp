#include "sw/replace_strategy.h"
#include "sw/text_normalizer.h"

namespace sensitive_word
{

std::size_t count_code_points(std::string_view text)
{
    return decode_utf8_with_positions(text).size();
}

chars_replace_strategy::chars_replace_strategy(char replacement)
    : replacement_(replacement)
{
}

std::string chars_replace_strategy::replacement_for(
    const word_result& result,
    std::string_view original_text) const
{
    const std::size_t count = count_code_points(
        original_text.substr(result.raw_begin, result.raw_end - result.raw_begin));
    return std::string(count, replacement_);
}

}  // namespace sensitive_word
