#pragma once

#include "sw/sensitive_word.h"

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace sensitive_word
{

struct normalized_char
{
    char32_t raw_code_point = 0;
    char32_t normalized_code_point = 0;
    std::size_t raw_byte_begin = 0;
    std::size_t raw_byte_end = 0;
};

struct normalized_text
{
    std::vector<normalized_char> normalized_chars {};
};

struct text_normalizer_options
{
    bool ignore_case = true;
    bool ignore_width = true;
    bool ignore_num_style = true;
    bool ignore_chinese_style = true;
    bool ignore_english_style = true;
};

class text_normalizer
{
public:
    explicit text_normalizer(text_normalizer_options options);
    ~text_normalizer();

    text_normalizer(const text_normalizer&) = delete;
    text_normalizer& operator=(const text_normalizer&) = delete;

    text_normalizer(text_normalizer&& other) noexcept;
    text_normalizer& operator=(text_normalizer&& other) noexcept;

    char32_t normalize_code_point(char32_t code_point) const;
    std::u32string normalize_word(std::string_view word) const;
    normalized_text normalize_text(std::string_view text) const;

private:
    class impl;
    std::unique_ptr<impl> impl_ {};
};

/**
 * @brief 检查字符是否为ASCII字母
 * @param ch 输入字符
 * @return true 字符是ASCII字母，否则返回false
 */
bool is_ascii_alpha(char32_t ch);

/**
 * @brief 检查字符是否为ASCII数字
 * @param ch 输入字符
 * @return true 字符是ASCII数字，否则返回false
 */
bool is_ascii_digit(char32_t ch);

/**
 * @brief 检查字符是否为ASCII字母、数字
 * @param ch 输入字符
 * @return true 字符是ASCII字母、数字，否则返回false
 */
bool is_ascii_alnum(char32_t ch);

/**
 * @brief 检查字符是否为ASCII字母、数字、CJK、拉丁扩展
 * @param ch 输入字符
 * @return true 字符是ASCII字母、数字、CJK、拉丁扩展，否则返回false
 */
bool is_word_like_code_point(char32_t ch);

/**
 * @brief 检查字符是否不是ASCII字母或数字
 * @param ch 输入字符
 * @return true 字符不是ASCII字母或数字，否则返回false
 */
bool is_ascii_word_boundary(char32_t ch);

std::string encode_utf8(char32_t code_point);
std::string encode_utf8(const std::u32string& text);
std::u32string decode_utf8(std::string_view text);

/**
 * @brief 统计字符串中代码点的数量(实际字符个数，count_code_points("你好111") == 5)
 * @param text 输入字符串
 * @return 代码点数量
 */
std::size_t count_code_points(std::string_view text);

std::vector<std::string> opencc_search_paths();

}  // namespace sensitive_word
