#pragma once

#ifndef SW_TEXT_NORMALIZER_H
#define SW_TEXT_NORMALIZER_H

#include "sw/sensitive_word.h"

namespace sensitive_word {

//////////////////////////////////////////////////////////////
// 正则化字符
// 用于表示文本中的一个字符，包含原始代码点、归一化代码点、原始字节开始和结束位置
//////////////////////////////////////////////////////////////
struct normalized_char
{
    char32_t raw_code_point        = 0;  // 原始代码点
    char32_t normalized_code_point = 0;  // 归一化代码点
    size_t   raw_byte_begin        = 0;  // 原始字节开始位置
    size_t   raw_byte_end          = 0;  // 原始字节结束位置
};

//////////////////////////////////////////////////////////////
// 正则化文本
// 用于表示文本中的一个字符列表，每个字符包含归一化代码点、原始字节开始和结束位置
//////////////////////////////////////////////////////////////
struct normalized_text
{
    std::vector<normalized_char> normalized_chars{};  // 正则化字符列表
};

//////////////////////////////////////////////////////////////
// 正则化选项
// 用于配置文本正则化器的行为
//////////////////////////////////////////////////////////////
struct text_normalizer_options
{
    bool ignore_case          = true;  // 是否忽略大小写
    bool ignore_width         = true;  // 是否忽略宽度
    bool ignore_num_style     = true;  // 是否忽略数字样式
    bool ignore_chinese_style = true;  // 是否忽略中文样式
    bool ignore_english_style = true;  // 是否忽略英文样式
};

//////////////////////////////////////////////////////////////
// 正则化器
// 用于对文本进行正则化处理
//////////////////////////////////////////////////////////////
class text_normalizer
{
public:
    explicit text_normalizer(text_normalizer_options options);
    ~text_normalizer(void);

    text_normalizer(const text_normalizer&) = delete;
    text_normalizer& operator=(const text_normalizer&) = delete;

    text_normalizer(text_normalizer&& other) noexcept = default;
    text_normalizer& operator=(text_normalizer&& other) noexcept = default;

    /**
     * @brief 归一化代码点
     * @param code_point 输入代码点
     * @return 归一化后的代码点
     */
    char32_t normalize_code_point(char32_t code_point) const;

    /**
     * @brief 归一化单词
     * @param word 输入单词
     * @return 归一化后的单词
     */
    std::u32string normalize_word(std::string_view word) const;

    /**
     * @brief 归一化文本
     * @param text 输入文本
     * @return 归一化后的文本
     */
    normalized_text normalize_text(std::string_view text) const;

private:
    class impl;
    std::unique_ptr<impl> impl_;
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

/**
 * @brief 编码UTF-8字符
 * @param code_point 输入代码点
 * @return 编码后的UTF-8字符串
 */
std::string encode_utf8(char32_t code_point);

/**
 * @brief 编码UTF-8文本
 * @param text 输入文本
 * @return 编码后的UTF-8字符串
 */
std::string encode_utf8(const std::u32string& text);

/**
 * @brief 解码UTF-8文本
 * @param text 输入UTF-8字符串
 * @return 解码后的文本
 */
std::u32string decode_utf8(std::string_view text);

/**
 * @brief 统计字符串中代码点的数量(实际字符个数，count_code_points("你好111") == 5)
 * @param text 输入字符串
 * @return 代码点数量
 */
size_t count_code_points(std::string_view text);

/**
 * @brief 搜索有OpenCC配置文件的路径(统一为当前目录：./data/config 和 ./data/dictionary)
 * @param
 * @return 所有OpenCC配置文件的路径
 */
std::vector<std::string> opencc_search_paths(void);

} // namespace sensitive_word

#endif // SW_TEXT_NORMALIZER_H