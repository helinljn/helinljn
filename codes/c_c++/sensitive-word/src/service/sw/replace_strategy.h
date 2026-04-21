#pragma once

#ifndef SW_REPLACE_STRATEGY_H
#define SW_REPLACE_STRATEGY_H

#include "sw/sensitive_word.h"

namespace sensitive_word {

//////////////////////////////////////////////////////////////
// 字符串替换策略
// 用于将敏感词替换为指定的字符
//////////////////////////////////////////////////////////////
class chars_replace_strategy final : public replace_strategy
{
public:
    explicit chars_replace_strategy(char replacement);

    std::string replacement_for(const word_result& result, std::string_view original_text) const override;

private:
    char replacement_;
};

} // namespace sensitive_word

#endif // SW_REPLACE_STRATEGY_H