#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sensitive_word
{

enum class match_type
{
    word,
    num,
};

struct word_result
{
    std::size_t raw_begin = 0;
    std::size_t raw_end = 0;
    std::size_t raw_code_point_length = 0;
    std::string word;
    std::string normalized_word;
    match_type type = match_type::word;

    char32_t left_raw_code_point = 0;
    char32_t right_raw_code_point = 0;
    char32_t left_normalized_code_point = 0;
    char32_t right_normalized_code_point = 0;
};

struct sensitive_word_config
{
    bool ignore_case = true;
    bool ignore_width = true;
    bool ignore_num_style = true;
    bool ignore_chinese_style = true;
    bool ignore_english_style = true;
    bool ignore_repeat = false;

    bool enable_word_check = true;
    bool enable_num_check = false;

    bool word_fail_fast = true;
    std::size_t num_check_len = 8;
};

//////////////////////////////////////////////////////////////
// 字符忽略策略
// 用于判断是否忽略指定的字符
//////////////////////////////////////////////////////////////
class char_ignore
{
public:
    virtual ~char_ignore(void) = default;

    virtual bool ignore(char32_t raw_code_point, char32_t normalized_code_point) const = 0;
};

//////////////////////////////////////////////////////////////
// 敏感词匹配条件
// 用于判断敏感词是否匹配
//////////////////////////////////////////////////////////////
class result_condition
{
public:
    virtual ~result_condition(void) = default;

    virtual bool match(const word_result& result, std::string_view text) const = 0;
};

//////////////////////////////////////////////////////////////
// 字符串替换策略
// 用于将敏感词替换为指定的字符
//////////////////////////////////////////////////////////////
class replace_strategy
{
public:
    virtual ~replace_strategy(void) = default;

    virtual std::string replacement_for(const word_result& result, std::string_view original_text) const = 0;
};

class sensitive_word_engine;

class sensitive_word_builder
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

    sensitive_word_builder& num_check_len(std::size_t value);
    sensitive_word_builder& word_fail_fast(bool value);

    sensitive_word_builder& deny_words(std::vector<std::string> words);
    sensitive_word_builder& allow_words(std::vector<std::string> words);

    sensitive_word_builder& add_deny_word(std::string word);
    sensitive_word_builder& add_allow_word(std::string word);
    sensitive_word_builder& add_deny_words_from_text(std::string text);
    sensitive_word_builder& add_allow_words_from_text(std::string text);
    sensitive_word_builder& add_deny_words_from_file(const std::string& file_path);
    sensitive_word_builder& add_allow_words_from_file(const std::string& file_path);

    sensitive_word_builder& char_ignore(std::shared_ptr<class char_ignore> value);
    sensitive_word_builder& result_condition(std::shared_ptr<class result_condition> value);
    sensitive_word_builder& replace_strategy(std::shared_ptr<class replace_strategy> value);

    sensitive_word_engine build() const;

private:
    sensitive_word_config config_ {};
    std::vector<std::string> deny_words_ {};
    std::vector<std::string> allow_words_ {};
    std::shared_ptr<class char_ignore> char_ignore_ {};
    std::shared_ptr<class result_condition> result_condition_ {};
    std::shared_ptr<class replace_strategy> replace_strategy_ {};
};

class sensitive_word_engine
{
public:
    sensitive_word_engine();
    ~sensitive_word_engine();

    sensitive_word_engine(const sensitive_word_engine& other);
    sensitive_word_engine& operator=(const sensitive_word_engine& other);

    sensitive_word_engine(sensitive_word_engine&& other) noexcept;
    sensitive_word_engine& operator=(sensitive_word_engine&& other) noexcept;

    sensitive_word_engine(
        sensitive_word_config config,
        std::vector<std::string> deny_words,
        std::vector<std::string> allow_words,
        std::shared_ptr<class char_ignore> char_ignore,
        std::shared_ptr<class result_condition> result_condition,
        std::shared_ptr<class replace_strategy> replace_strategy);

    bool contains(std::string_view text) const;
    std::optional<word_result> find_first(std::string_view text) const;
    std::vector<word_result> find_all(std::string_view text) const;

    std::optional<std::string> find_first_word(std::string_view text) const;
    std::vector<std::string> find_all_words(std::string_view text) const;

    std::string replace(std::string_view text) const;
    std::string replace(std::string_view text, char replacement) const;
    std::string replace(std::string_view text, const replace_strategy& strategy) const;

    void add_word(std::string_view word);
    void remove_word(std::string_view word);

    void add_allow_word(std::string_view word);
    void remove_allow_word(std::string_view word);

    const sensitive_word_config& config() const noexcept;

private:
    class impl;
    std::unique_ptr<impl> impl_ {};
};

namespace char_ignores {

/**
 * @brief 创建一个字符忽略策略，不忽略任何字符
 * @param
 * @return
 */
std::shared_ptr<char_ignore> none(void);

/**
 * @brief 创建一个字符忽略策略，忽略所有特殊字符
 * @param
 * @return
 */
std::shared_ptr<char_ignore> special_chars(void);

} // namespace char_ignores

namespace result_conditions {

/**
 * @brief 创建一个结果条件，始终返回true
 * @param
 * @return
 */
std::shared_ptr<result_condition> always_true(void);

/**
 * @brief 创建一个结果条件，匹配英文单词
 * @param
 * @return
 */
std::shared_ptr<result_condition> english_word_match(void);

/**
 * @brief 创建一个结果条件，匹配英文单词和数字
 * @param
 * @return
 */
std::shared_ptr<result_condition> english_word_num_match(void);

} // namespace result_conditions

namespace replace_strategies {

/**
 * @brief 创建一个替换策略，用星号替换敏感词
 * @param
 * @return
 */
std::shared_ptr<replace_strategy> stars(void);

/**
 * @brief 创建一个替换策略，用指定字符替换敏感词
 * @param replacement 替换字符
 * @return
 */
std::shared_ptr<replace_strategy> chars(char replacement);

} // namespace replace_strategies
} // namespace sensitive_word