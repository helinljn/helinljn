#pragma once

#ifndef SENSITIVE_WORD_H
#define SENSITIVE_WORD_H

#include "../core.h"
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sensitive_word {

class CORE_API sensitive_word_engine;

//////////////////////////////////////////////////////////////
// 敏感词匹配类型
//////////////////////////////////////////////////////////////
enum class match_type
{
    word,
    num,
};

//////////////////////////////////////////////////////////////
// 敏感词匹配结果
//////////////////////////////////////////////////////////////
struct word_result
{
    size_t      raw_begin             = 0;  // 命中片段在原始文本中的起始字节位置
    size_t      raw_end               = 0;  // 命中片段在原始文本中的结束字节位置(右开区间)
    size_t      raw_code_point_length = 0;  // 命中片段覆盖的原始代码点数量
    std::string word;                       // 原始文本中的命中片段
    std::string normalized_word;            // 命中词的归一化结果
    match_type  type = match_type::word;    // 命中类型

    char32_t left_raw_code_point         = 0;  // 命中片段左侧相邻的原始代码点
    char32_t right_raw_code_point        = 0;  // 命中片段右侧相邻的原始代码点
    char32_t left_normalized_code_point  = 0;  // 命中片段左侧相邻代码点的归一化结果
    char32_t right_normalized_code_point = 0;  // 命中片段右侧相邻代码点的归一化结果
};

//////////////////////////////////////////////////////////////
// 词条状态
//////////////////////////////////////////////////////////////
struct word_entry_status
{
    bool exists   = false;  // 词条是否存在于字典中
    bool in_deny  = false;  // 词条是否存在于黑名单
    bool in_allow = false;  // 词条是否存在于白名单
};

//////////////////////////////////////////////////////////////
// 敏感词匹配配置
//////////////////////////////////////////////////////////////
struct sensitive_word_config
{
    bool ignore_case          = true;   // 是否忽略英文大小写差异
    bool ignore_width         = true;   // 是否忽略全角/半角宽度差异
    bool ignore_num_style     = true;   // 是否忽略数字样式差异
    bool ignore_chinese_style = true;   // 是否忽略中文繁简体差异(繁体转简体)
    bool ignore_english_style = true;   // 是否忽略英文字符的样式变体
    bool ignore_repeat        = false;  // 是否忽略连续重复字符对词匹配的影响

    bool enable_word_check    = true;   // 是否启用敏感词字典匹配
    bool enable_num_check     = false;  // 是否启用纯数字片段匹配

    size_t num_check_len      = 2;      // 触发数字匹配所需的最小数字长度
};

//////////////////////////////////////////////////////////////
// 敏感词匹配执行选项
//////////////////////////////////////////////////////////////
struct match_options
{
    // 是否启用最长匹配(贪婪匹配)
    // false: 最短匹配，命中即返回(性能最高)
    // true:  最长匹配，寻找包含起点的最长词(适合高亮、替换)
    bool longest_match = false;
};

//////////////////////////////////////////////////////////////
// 字符忽略策略
// 用于判断扫描过程中是否跳过当前字符
//////////////////////////////////////////////////////////////
class CORE_API char_ignore
{
public:
    virtual ~char_ignore() = default;

    [[nodiscard]] virtual bool ignore(char32_t raw_code_point, char32_t normalized_code_point) const = 0;
};

//////////////////////////////////////////////////////////////
// 敏感词匹配条件
// 用于对候选命中结果做进一步过滤
//////////////////////////////////////////////////////////////
class CORE_API result_condition
{
public:
    virtual ~result_condition() = default;

    [[nodiscard]] virtual bool match(const word_result& result, std::string_view text) const = 0;
};

//////////////////////////////////////////////////////////////
// 字符串替换策略
// 用于为命中结果生成替换文本
//////////////////////////////////////////////////////////////
class CORE_API replace_strategy
{
public:
    virtual ~replace_strategy() = default;

    [[nodiscard]] virtual std::string replacement_for(const word_result& result, std::string_view original_text) const = 0;
};

//////////////////////////////////////////////////////////////
// 敏感词匹配构建器
// 用于组装配置、词库和策略并构建匹配引擎
//////////////////////////////////////////////////////////////
class CORE_API sensitive_word_builder
{
public:
    sensitive_word_builder& ignore_case(bool value);
    sensitive_word_builder& ignore_width(bool value);
    sensitive_word_builder& ignore_num_style(bool value);
    sensitive_word_builder& ignore_chinese_style(bool value);
    sensitive_word_builder& ignore_english_style(bool value);
    sensitive_word_builder& ignore_repeat(bool value);

    sensitive_word_builder& enable_word_check(bool value);
    sensitive_word_builder& enable_num_check(bool value);

    sensitive_word_builder& num_check_len(size_t value);

    sensitive_word_builder& deny_words(std::vector<std::string> words);
    sensitive_word_builder& allow_words(std::vector<std::string> words);

    sensitive_word_builder& add_deny_word(std::string word);
    sensitive_word_builder& add_allow_word(std::string word);
    sensitive_word_builder& add_deny_words_from_text(std::string_view text);
    sensitive_word_builder& add_allow_words_from_text(std::string_view text);
    sensitive_word_builder& add_deny_words_from_file(const std::string& file_path);
    sensitive_word_builder& add_allow_words_from_file(const std::string& file_path);

    sensitive_word_builder& char_ignore(std::shared_ptr<class char_ignore> value);
    sensitive_word_builder& result_condition(std::shared_ptr<class result_condition> value);
    sensitive_word_builder& replace_strategy(std::shared_ptr<class replace_strategy> value);

    sensitive_word_engine build();

private:
    sensitive_word_config                   config_;
    std::vector<std::string>                deny_words_;
    std::vector<std::string>                allow_words_;
    std::shared_ptr<class char_ignore>      char_ignore_;
    std::shared_ptr<class result_condition> result_condition_;
    std::shared_ptr<class replace_strategy> replace_strategy_;
};

//////////////////////////////////////////////////////////////
// 敏感词匹配引擎
// 用于执行命中检测、结果提取和替换操作
//////////////////////////////////////////////////////////////
class CORE_API sensitive_word_engine
{
public:
    sensitive_word_engine();
    ~sensitive_word_engine();

    sensitive_word_engine(const sensitive_word_engine& other);
    sensitive_word_engine& operator=(const sensitive_word_engine& other);

    sensitive_word_engine(sensitive_word_engine&& other) noexcept;
    sensitive_word_engine& operator=(sensitive_word_engine&& other) noexcept;

    sensitive_word_engine(
        sensitive_word_config                   config,
        std::vector<std::string>                deny_words,
        std::vector<std::string>                allow_words,
        std::shared_ptr<class char_ignore>      char_ignore,
        std::shared_ptr<class result_condition> result_condition,
        std::shared_ptr<class replace_strategy> replace_strategy);

    [[nodiscard]] bool contains(std::string_view text) const;
    [[nodiscard]] std::optional<word_result> find_first(std::string_view text, const match_options& options = {}) const;
    [[nodiscard]] std::vector<word_result> find_all(std::string_view text, const match_options& options = {}) const;

    [[nodiscard]] std::optional<std::string> find_first_word(std::string_view text, const match_options& options = {}) const;
    [[nodiscard]] std::vector<std::string> find_all_words(std::string_view text, const match_options& options = {}) const;

    [[nodiscard]] std::string replace(std::string_view text, const match_options& options = {}) const;
    [[nodiscard]] std::string replace(std::string_view text, char replacement, const match_options& options = {}) const;
    [[nodiscard]] std::string replace(std::string_view text, const replace_strategy& strategy, const match_options& options = {}) const;

    [[nodiscard]] std::string replace(std::string_view text, const std::vector<word_result>& results) const;
    [[nodiscard]] std::string replace(std::string_view text, const std::vector<word_result>& results, char replacement) const;
    [[nodiscard]] std::string replace(std::string_view text, const std::vector<word_result>& results, const replace_strategy& strategy) const;

    void add_word(std::string_view word);
    void remove_word(std::string_view word);

    void add_allow_word(std::string_view word);
    void remove_allow_word(std::string_view word);

    [[nodiscard]] word_entry_status query_word_status(std::string_view word) const;
    [[nodiscard]] const sensitive_word_config& config() const noexcept;

private:
    class impl;
    std::unique_ptr<impl> impl_;
};

namespace char_ignores {

/**
 * @brief 创建一个字符忽略策略，不忽略任何字符
 * @return 不会跳过任何字符的忽略策略
 */
CORE_API std::shared_ptr<char_ignore> none();

/**
 * @brief 创建一个字符忽略策略，忽略归一化后不属于词字符的字符
 * @return 会跳过非词字符的忽略策略
 */
CORE_API std::shared_ptr<char_ignore> special_chars();

} // namespace char_ignores

namespace result_conditions {

/**
 * @brief 创建一个结果条件，不过滤任何命中结果
 * @return 始终返回 true 的结果条件
 */
CORE_API std::shared_ptr<result_condition> always_true();

/**
 * @brief 创建一个结果条件，仅要求包含 ASCII 字母的命中满足英文单词边界
 * @return 英文单词边界匹配条件
 */
CORE_API std::shared_ptr<result_condition> english_word_match();

/**
 * @brief 创建一个结果条件，仅要求包含 ASCII 字母或数字的命中满足单词边界
 * @return 英文单词和数字边界匹配条件
 */
CORE_API std::shared_ptr<result_condition> english_word_num_match();

} // namespace result_conditions

namespace replace_strategies {

/**
 * @brief 创建一个替换策略，用星号按命中长度生成替换文本
 * @return 星号替换策略
 */
CORE_API std::shared_ptr<replace_strategy> stars();

/**
 * @brief 创建一个替换策略，用指定字符按命中长度生成替换文本
 * @param replacement 用于替换命中片段的字符
 * @return 指定字符替换策略
 */
CORE_API std::shared_ptr<replace_strategy> chars(char replacement);

} // namespace replace_strategies
} // namespace sensitive_word

#endif // SENSITIVE_WORD_H