#include "sensitive_word.h"

#include "num_detector.h"
#include "text_normalizer.h"
#include "trie_dictionary.h"
#include "word_detector.h"
#include "result_condition.h"
#include "replace_strategy.h"
#include "word_dictionary_loader.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace sensitive_word
{

namespace
{

class none_char_ignore final : public char_ignore
{
public:
    bool ignore(char32_t, char32_t) const override
    {
        return false;
    }
};

class special_char_ignore final : public char_ignore
{
public:
    bool ignore(char32_t, char32_t normalized_code_point) const override
    {
        return !is_word_like_code_point(normalized_code_point);
    }
};

}  // namespace

class sensitive_word_engine::impl
{
public:
    impl(
        sensitive_word_config config,
        std::vector<std::string> deny_words,
        std::vector<std::string> allow_words,
        std::shared_ptr<class char_ignore> char_ignore,
        std::shared_ptr<class result_condition> result_condition,
        std::shared_ptr<class replace_strategy> replace_strategy)
        : config_(std::move(config))
        , char_ignore_(std::move(char_ignore))
        , result_condition_(std::move(result_condition))
        , replace_strategy_(std::move(replace_strategy))
    {
        if (!char_ignore_)
        {
            char_ignore_ = char_ignores::none();
        }

        if (!result_condition_)
        {
            result_condition_ = result_conditions::english_word_match();
        }

        if (!replace_strategy_)
        {
            replace_strategy_ = replace_strategies::stars();
        }

        normalizer_ = std::make_shared<text_normalizer>(text_normalizer_options{
            config_.ignore_case,
            config_.ignore_width,
            config_.ignore_num_style,
            config_.ignore_chinese_style,
            config_.ignore_english_style,
        });

        allow_trie_ = std::make_shared<trie_dictionary>();
        deny_trie_ = std::make_shared<trie_dictionary>();

        detector_ = std::make_shared<word_detector>(
            word_detector_options{
                config_.ignore_repeat,
                config_.word_fail_fast,
            },
            char_ignore_,
            normalizer_,
            allow_trie_,
            deny_trie_);

        num_detector_ = std::make_shared<num_detector>(
            num_detector_options{
                config_.num_check_len,
            },
            char_ignore_);

        for (const auto& word : deny_words)
        {
            add_word(word);
        }

        for (const auto& word : allow_words)
        {
            add_allow_word(word);
        }
    }

    std::vector<word_result> find_all(std::string_view text, bool stop_after_first) const
    {
        std::vector<word_result> results;

        if (text.empty() || (!config_.enable_word_check && !config_.enable_num_check))
        {
            return results;
        }

        const normalized_text normalized = normalizer_->normalize_text(text);
        if (normalized.normalized_chars.empty())
        {
            return results;
        }

        for (std::size_t i = 0; i < normalized.normalized_chars.size(); ++i)
        {
            std::size_t allow_length = 0;
            std::size_t deny_length = 0;
            std::u32string normalized_deny_word;
            match_type detected_type = match_type::word;

            if (config_.enable_word_check)
            {
                const word_detector_result detect_result = detector_->detect_at(normalized, i);
                if (detect_result.allow_length > 0 || detect_result.deny_length > 0)
                {
                    allow_length = detect_result.allow_length;
                    deny_length = detect_result.deny_length;
                    normalized_deny_word = detect_result.normalized_deny_word;
                    detected_type = match_type::word;
                }
            }

            if (allow_length == 0 && deny_length == 0 && config_.enable_num_check)
            {
                const num_detector_result detect_result = num_detector_->detect_at(normalized, i);
                if (detect_result.deny_length > 0)
                {
                    deny_length = detect_result.deny_length;
                    normalized_deny_word = detect_result.normalized_deny_word;
                    detected_type = match_type::num;
                }
            }

            if (allow_length < deny_length && deny_length > 0)
            {
                word_result result;
                result.raw_begin = normalized.normalized_chars[i].raw_byte_begin;
                result.raw_end = normalized.normalized_chars[i + deny_length - 1].raw_byte_end;
                result.word = std::string(text.substr(result.raw_begin, result.raw_end - result.raw_begin));
                result.normalized_word = encode_utf8(normalized_deny_word);
                result.type = detected_type;

                if (!result_condition_ || result_condition_->match(result, text))
                {
                    results.push_back(result);
                    if (stop_after_first)
                    {
                        break;
                    }
                }

                i += deny_length - 1;
            }
            else if (allow_length > 0)
            {
                i += allow_length - 1;
            }
        }

        return results;
    }

    bool contains(std::string_view text) const
    {
        return static_cast<bool>(find_first(text));
    }

    std::optional<word_result> find_first(std::string_view text) const
    {
        std::vector<word_result> results = find_all(text, true);
        if (results.empty())
        {
            return std::nullopt;
        }

        return results.front();
    }

    std::string replace(std::string_view text) const
    {
        return replace(text, *replace_strategy_);
    }

    std::string replace(std::string_view text, const replace_strategy& strategy) const
    {
        const std::vector<word_result> results = find_all(text, false);
        if (results.empty())
        {
            return std::string(text);
        }

        std::string out;
        std::size_t cursor = 0;

        for (const auto& result : results)
        {
            if (cursor < result.raw_begin)
            {
                out.append(text.substr(cursor, result.raw_begin - cursor));
            }

            out += strategy.replacement_for(result, text);
            cursor = std::max(cursor, result.raw_end);
        }

        if (cursor < text.size())
        {
            out.append(text.substr(cursor));
        }

        return out;
    }

    void add_word(std::string_view word)
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
        {
            return;
        }

        deny_trie_->add_word(normalized);
    }

    void remove_word(std::string_view word)
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
        {
            return;
        }

        deny_trie_->remove_word(normalized);
    }

    void add_allow_word(std::string_view word)
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
        {
            return;
        }

        allow_trie_->add_word(normalized);
    }

    void remove_allow_word(std::string_view word)
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
        {
            return;
        }

        allow_trie_->remove_word(normalized);
    }

    const sensitive_word_config& config() const noexcept
    {
        return config_;
    }

private:
    sensitive_word_config config_ {};
    std::shared_ptr<class char_ignore> char_ignore_ {};
    std::shared_ptr<class result_condition> result_condition_ {};
    std::shared_ptr<class replace_strategy> replace_strategy_ {};
    std::shared_ptr<text_normalizer> normalizer_ {};
    std::shared_ptr<trie_dictionary> allow_trie_ {};
    std::shared_ptr<trie_dictionary> deny_trie_ {};
    std::shared_ptr<word_detector> detector_ {};
    std::shared_ptr<num_detector> num_detector_ {};
};

sensitive_word_builder& sensitive_word_builder::ignore_case(bool value)
{
    config_.ignore_case = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::ignore_width(bool value)
{
    config_.ignore_width = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::ignore_num_style(bool value)
{
    config_.ignore_num_style = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::ignore_chinese_style(bool value)
{
    config_.ignore_chinese_style = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::ignore_english_style(bool value)
{
    config_.ignore_english_style = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::ignore_repeat(bool value)
{
    config_.ignore_repeat = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::enable_word_check(bool value)
{
    config_.enable_word_check = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::enable_num_check(bool value)
{
    config_.enable_num_check = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::num_check_len(std::size_t value)
{
    config_.num_check_len = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::word_fail_fast(bool value)
{
    config_.word_fail_fast = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::deny_words(std::vector<std::string> words)
{
    deny_words_ = std::move(words);
    return *this;
}

sensitive_word_builder& sensitive_word_builder::allow_words(std::vector<std::string> words)
{
    allow_words_ = std::move(words);
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_deny_word(std::string word)
{
    deny_words_.push_back(std::move(word));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_allow_word(std::string word)
{
    allow_words_.push_back(std::move(word));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_deny_words_from_text(std::string text)
{
    auto words = load_words_from_text(text);
    deny_words_.insert(
        deny_words_.end(),
        std::make_move_iterator(words.begin()),
        std::make_move_iterator(words.end()));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_allow_words_from_text(std::string text)
{
    auto words = load_words_from_text(text);
    allow_words_.insert(
        allow_words_.end(),
        std::make_move_iterator(words.begin()),
        std::make_move_iterator(words.end()));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_deny_words_from_file(const std::string& file_path)
{
    auto words = load_words_from_file(file_path);
    deny_words_.insert(
        deny_words_.end(),
        std::make_move_iterator(words.begin()),
        std::make_move_iterator(words.end()));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_allow_words_from_file(const std::string& file_path)
{
    auto words = load_words_from_file(file_path);
    allow_words_.insert(
        allow_words_.end(),
        std::make_move_iterator(words.begin()),
        std::make_move_iterator(words.end()));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::char_ignore(std::shared_ptr<class char_ignore> value)
{
    char_ignore_ = std::move(value);
    return *this;
}

sensitive_word_builder& sensitive_word_builder::result_condition(std::shared_ptr<class result_condition> value)
{
    result_condition_ = std::move(value);
    return *this;
}

sensitive_word_builder& sensitive_word_builder::replace_strategy(std::shared_ptr<class replace_strategy> value)
{
    replace_strategy_ = std::move(value);
    return *this;
}

sensitive_word_engine sensitive_word_builder::build() const
{
    return sensitive_word_engine(
        config_,
        deny_words_,
        allow_words_,
        char_ignore_,
        result_condition_,
        replace_strategy_);
}

sensitive_word_engine::sensitive_word_engine()
    : sensitive_word_engine(
          sensitive_word_config {},
          {},
          {},
          char_ignores::none(),
          result_conditions::english_word_match(),
          replace_strategies::stars())
{
}

sensitive_word_engine::sensitive_word_engine(
    sensitive_word_config config,
    std::vector<std::string> deny_words,
    std::vector<std::string> allow_words,
    std::shared_ptr<class char_ignore> char_ignore,
    std::shared_ptr<class result_condition> result_condition,
    std::shared_ptr<class replace_strategy> replace_strategy)
    : impl_(std::make_shared<impl>(
          std::move(config),
          std::move(deny_words),
          std::move(allow_words),
          std::move(char_ignore),
          std::move(result_condition),
          std::move(replace_strategy)))
{
}

bool sensitive_word_engine::contains(std::string_view text) const
{
    return impl_->contains(text);
}

std::optional<word_result> sensitive_word_engine::find_first(std::string_view text) const
{
    return impl_->find_first(text);
}

std::vector<word_result> sensitive_word_engine::find_all(std::string_view text) const
{
    return impl_->find_all(text, false);
}

std::optional<std::string> sensitive_word_engine::find_first_word(std::string_view text) const
{
    const auto result = find_first(text);
    if (!result)
    {
        return std::nullopt;
    }

    return result->word;
}

std::vector<std::string> sensitive_word_engine::find_all_words(std::string_view text) const
{
    const auto results = find_all(text);
    std::vector<std::string> words;
    words.reserve(results.size());

    for (const auto& result : results)
    {
        words.push_back(result.word);
    }

    return words;
}

std::string sensitive_word_engine::replace(std::string_view text) const
{
    return impl_->replace(text);
}

std::string sensitive_word_engine::replace(std::string_view text, char replacement) const
{
    return impl_->replace(text, *replace_strategies::chars(replacement));
}

std::string sensitive_word_engine::replace(std::string_view text, const replace_strategy& strategy) const
{
    return impl_->replace(text, strategy);
}

void sensitive_word_engine::add_word(std::string_view word)
{
    impl_->add_word(word);
}

void sensitive_word_engine::remove_word(std::string_view word)
{
    impl_->remove_word(word);
}

void sensitive_word_engine::add_allow_word(std::string_view word)
{
    impl_->add_allow_word(word);
}

void sensitive_word_engine::remove_allow_word(std::string_view word)
{
    impl_->remove_allow_word(word);
}

const sensitive_word_config& sensitive_word_engine::config() const noexcept
{
    return impl_->config();
}

namespace char_ignores
{

std::shared_ptr<char_ignore> none()
{
    static auto instance = std::make_shared<none_char_ignore>();
    return instance;
}

std::shared_ptr<char_ignore> special_chars()
{
    static auto instance = std::make_shared<special_char_ignore>();
    return instance;
}

}  // namespace char_ignores

namespace result_conditions
{

std::shared_ptr<result_condition> always_true()
{
    static auto instance = std::make_shared<always_true_result_condition>();
    return instance;
}

std::shared_ptr<result_condition> english_word_match()
{
    static auto instance = std::make_shared<english_word_match_result_condition>();
    return instance;
}

std::shared_ptr<result_condition> english_word_num_match()
{
    static auto instance = std::make_shared<english_word_num_match_result_condition>();
    return instance;
}

}  // namespace result_conditions

namespace replace_strategies
{

std::shared_ptr<replace_strategy> stars()
{
    static auto instance = std::make_shared<chars_replace_strategy>('*');
    return instance;
}

std::shared_ptr<replace_strategy> chars(char replacement)
{
    return std::make_shared<chars_replace_strategy>(replacement);
}

}  // namespace replace_strategies

}  // namespace sensitive_word
