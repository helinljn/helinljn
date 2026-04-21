#pragma once

#include "sw/sensitive_word.h"

#include <string>

namespace sensitive_word
{

std::size_t count_code_points(std::string_view text);

class chars_replace_strategy final : public replace_strategy
{
public:
    explicit chars_replace_strategy(char replacement);

    std::string replacement_for(
        const word_result& result,
        std::string_view original_text) const override;

private:
    char replacement_ = '*';
};

}  // namespace sensitive_word
