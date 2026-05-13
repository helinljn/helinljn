#pragma once

#ifndef SW_RESULT_CONDITION_H
#define SW_RESULT_CONDITION_H

#include "sw/sensitive_word.h"

namespace sensitive_word {

//////////////////////////////////////////////////////////////
// 敏感词匹配条件
// 用于不过滤任何命中结果
//////////////////////////////////////////////////////////////
class always_true_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

//////////////////////////////////////////////////////////////
// 敏感词匹配条件
// 用于要求包含 ASCII 字母的命中满足英文单词边界
//////////////////////////////////////////////////////////////
class english_word_match_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

//////////////////////////////////////////////////////////////
// 敏感词匹配条件
// 用于要求包含 ASCII 字母或数字的命中满足单词边界
//////////////////////////////////////////////////////////////
class english_word_num_match_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

/**
 * @brief 检查字符串是否包含 ASCII 字母
 * @param text 输入字符串
 * @return true 字符串中包含至少一个 ASCII 字母，否则返回 false
 */
bool contains_ascii_letter(std::string_view text);

/**
 * @brief 检查字符串是否包含 ASCII 数字
 * @param text 输入字符串
 * @return true 字符串中包含至少一个 ASCII 数字，否则返回 false
 */
bool contains_ascii_digit_text(std::string_view text);

} // namespace sensitive_word

#endif // SW_RESULT_CONDITION_H
