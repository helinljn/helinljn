#pragma once

#include "sw/sensitive_word.h"

#include <memory>
#include <string_view>

namespace sensitive_word
{

char32_t previous_code_point(std::string_view text, std::size_t begin);
char32_t next_code_point(std::string_view text, std::size_t end);

bool contains_ascii_letter(std::string_view text);
bool contains_ascii_digit_text(std::string_view text);
bool is_ascii_word_boundary(char32_t ch);

class always_true_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

class english_word_match_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

class english_word_num_match_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

}  // namespace sensitive_word
