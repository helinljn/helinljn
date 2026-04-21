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
    std::string word;
    std::string normalized_word;
    match_type type = match_type::word;
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

class char_ignore
{
public:
    virtual ~char_ignore() = default;

    virtual bool ignore(char32_t raw_code_point, char32_t normalized_code_point) const = 0;
};

class result_condition
{
public:
    virtual ~result_condition() = default;

    virtual bool match(
        const word_result& result,
        std::string_view text) const = 0;
};

class replace_strategy
{
public:
    virtual ~replace_strategy() = default;

    virtual std::string replacement_for(
        const word_result& result,
        std::string_view original_text) const = 0;
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
    std::shared_ptr<impl> impl_ {};
};

namespace char_ignores
{
std::shared_ptr<char_ignore> none();
std::shared_ptr<char_ignore> special_chars();
}

namespace result_conditions
{
std::shared_ptr<result_condition> always_true();
std::shared_ptr<result_condition> english_word_match();
std::shared_ptr<result_condition> english_word_num_match();
}

namespace replace_strategies
{
std::shared_ptr<replace_strategy> stars();
std::shared_ptr<replace_strategy> chars(char replacement);
}

}  // namespace sensitive_word
