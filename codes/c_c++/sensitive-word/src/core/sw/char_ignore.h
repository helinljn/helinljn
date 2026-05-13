#pragma once

#ifndef SW_CHAR_IGNORE_H
#define SW_CHAR_IGNORE_H

#include "sw/sensitive_word.h"

namespace sensitive_word {

//////////////////////////////////////////////////////////////
// 用于不忽略任何字符
//////////////////////////////////////////////////////////////
class none_char_ignore final : public char_ignore
{
public:
    bool ignore(char32_t, char32_t) const override;
};

//////////////////////////////////////////////////////////////
// 用于忽略归一化后不属于词字符的字符
//////////////////////////////////////////////////////////////
class special_char_ignore final : public char_ignore
{
public:
    bool ignore(char32_t, char32_t normalized_code_point) const override;
};

} // namespace sensitive_word

#endif // SW_CHAR_IGNORE_H
