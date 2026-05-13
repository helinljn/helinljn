#pragma once

#ifndef SW_TEXT_NORMALIZER_H
#define SW_TEXT_NORMALIZER_H

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace sensitive_word {

//////////////////////////////////////////////////////////////
// 归一化字符
// 用于表示原始字符及其归一化结果，并记录其在原始文本中的字节区间
//////////////////////////////////////////////////////////////
struct normalized_char
{
    char32_t raw_code_point        = 0;  // 原始代码点
    char32_t normalized_code_point = 0;  // 归一化后的代码点
    size_t   raw_byte_begin        = 0;  // 原始字节起始位置
    size_t   raw_byte_end          = 0;  // 原始字节结束位置(右开区间)
};

//////////////////////////////////////////////////////////////
// 归一化文本
// 用于保存归一化后的字符序列及其与原始文本的字节位置映射
//////////////////////////////////////////////////////////////
struct normalized_text
{
    std::vector<normalized_char> normalized_chars{};  // 归一化字符列表
};

//////////////////////////////////////////////////////////////
// 归一化选项
// 用于配置文本归一化阶段要折叠或转换的字符差异
//////////////////////////////////////////////////////////////
struct text_normalizer_options
{
    bool ignore_case          = true;  // 是否忽略英文大小写差异
    bool ignore_width         = true;  // 是否忽略全角/半角宽度差异
    bool ignore_num_style     = true;  // 是否忽略数字样式差异
    bool ignore_chinese_style = true;  // 是否忽略中文繁简体差异(繁体转简体)
    bool ignore_english_style = true;  // 是否忽略英文字符的样式变体
};

//////////////////////////////////////////////////////////////
// 归一化器
// 用于将输入文本转换为统一的归一化表示
//////////////////////////////////////////////////////////////
class text_normalizer
{
public:
    explicit text_normalizer(text_normalizer_options options);
    ~text_normalizer();

    text_normalizer(const text_normalizer&) = delete;
    text_normalizer& operator=(const text_normalizer&) = delete;

    text_normalizer(text_normalizer&& other) noexcept = default;
    text_normalizer& operator=(text_normalizer&& other) noexcept = default;

    /**
     * @brief 归一化代码点
     * @param code_point 输入代码点
     * @return 归一化后的代码点
     */
    [[nodiscard]] char32_t normalize_code_point(char32_t code_point) const;

    /**
     * @brief 归一化单词
     * @param word 输入单词
     * @return 归一化后的单词
     */
    [[nodiscard]] std::u32string normalize_word(std::string_view word) const;

    /**
     * @brief 归一化文本
     * @param text 输入文本
     * @return 包含归一化字符及原始字节位置映射的文本结果
     */
    [[nodiscard]] normalized_text normalize_text(std::string_view text) const;

private:
    class impl;
    std::unique_ptr<impl> impl_;
};

/**
 * @brief 检查字符是否为 ASCII 字母
 * @param ch 输入字符
 * @return true 字符是 ASCII 字母，否则返回 false
 */
[[nodiscard]] constexpr inline bool is_ascii_alpha(char32_t ch)
{
    return (ch >= U'a' && ch <= U'z') || (ch >= U'A' && ch <= U'Z');
}

/**
 * @brief 检查字符是否为 ASCII 数字
 * @param ch 输入字符
 * @return true 字符是 ASCII 数字，否则返回 false
 */
[[nodiscard]] constexpr inline bool is_ascii_digit(char32_t ch)
{
    return ch >= U'0' && ch <= U'9';
}

/**
 * @brief 检查字符是否为 ASCII 字母或数字
 * @param ch 输入字符
 * @return true 字符是 ASCII 字母或数字，否则返回 false
 */
[[nodiscard]] constexpr inline bool is_ascii_alnum(char32_t ch)
{
    return is_ascii_alpha(ch) || is_ascii_digit(ch);
}

/**
 * @brief 检查字符是否为 ASCII 单词边界
 * @param ch 输入字符
 * @return true 字符不是 ASCII 字母或数字，否则返回 false
 */
[[nodiscard]] constexpr inline bool is_ascii_word_boundary(char32_t ch)
{
    return !is_ascii_alnum(ch);
}

/**
 * @brief 检查字符是否可视为词字符
 * @param ch 输入字符
 * @return true 字符属于 ASCII 字母/数字、CJK 汉字，否则返回 false
 */
[[nodiscard]] bool is_word_like_code_point(char32_t ch);

/**
 * @brief 编码 UTF-8 字符
 * @param code_point 输入代码点
 * @return 编码后的 UTF-8 字符串
 */
[[nodiscard]] std::string encode_utf8(char32_t code_point);

/**
 * @brief 编码 UTF-8 文本
 * @param text 输入文本
 * @return 编码后的 UTF-8 字符串
 */
[[nodiscard]] std::string encode_utf8(const std::u32string& text);

/**
 * @brief 解码 UTF-8 文本
 * @param text 输入 UTF-8 字符串
 * @return 解码后的 UTF-32 文本
 */
[[nodiscard]] std::u32string decode_utf8(std::string_view text);

/**
 * @brief 统计字符串中的代码点数量(实际字符个数，count_code_points("你好111") == 5)
 * @param text 输入字符串
 * @return 代码点数量
 */
[[nodiscard]] size_t count_code_points(std::string_view text);

/**
 * @brief 搜索 OpenCC 配置和词典目录(统一为当前目录：./data/config 和 ./data/dictionary)
 * @return 可用于初始化 OpenCC 的配置目录和词典目录路径列表
 */
[[nodiscard]] std::vector<std::string> opencc_search_paths();

} // namespace sensitive_word

#endif // SW_TEXT_NORMALIZER_H
