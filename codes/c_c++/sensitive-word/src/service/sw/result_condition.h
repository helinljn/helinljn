#pragma once

#ifndef SW_RESULT_CONDITION_H
#define SW_RESULT_CONDITION_H

#include "sw/sensitive_word.h"

namespace sensitive_word {

//////////////////////////////////////////////////////////////
// 敏感词匹配条件
// 用于判断敏感词是否匹配(默认返回true)
//////////////////////////////////////////////////////////////
class always_true_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

//////////////////////////////////////////////////////////////
// 敏感词匹配条件
// 用于判断敏感词是否匹配(仅匹配英文单词)
//////////////////////////////////////////////////////////////
class english_word_match_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

//////////////////////////////////////////////////////////////
// 敏感词匹配条件
// 用于判断敏感词是否匹配(仅匹配英文单词和数字)
//////////////////////////////////////////////////////////////
class english_word_num_match_result_condition final : public result_condition
{
public:
    bool match(const word_result& result, std::string_view text) const override;
};

/**
 * @brief 检查字符串是否包含ASCII字母
 * @param text 输入字符串
 * @return true 如果字符串中包含ASCII字母，否则返回false
 */
bool contains_ascii_letter(std::string_view text);

/**
 * @brief 检查字符串是否包含ASCII数字
 * @param text 输入字符串
 * @return true 如果字符串中包含ASCII数字，否则返回false
 */
bool contains_ascii_digit_text(std::string_view text);

} // namespace sensitive_word

#endif // SW_RESULT_CONDITION_H