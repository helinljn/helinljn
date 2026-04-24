#include "sw/sensitive_word.h"
#include "sw/char_ignore.h"
#include "sw/replace_strategy.h"
#include "sw/result_condition.h"
#include "sw/text_normalizer.h"
#include "sw/trie_dictionary.h"
#include "sw/word_dictionary_loader.h"
#include <utility>
#include <algorithm>

namespace sensitive_word {
namespace                {

void append_words(std::vector<std::string>& target, std::vector<std::string> words)
{
    target.reserve(target.size() + words.size());
    target.insert(target.end(), std::make_move_iterator(words.begin()), std::make_move_iterator(words.end()));
}

} // namespace

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

sensitive_word_builder& sensitive_word_builder::word_fail_fast(bool value)
{
    config_.word_fail_fast = value;
    return *this;
}

sensitive_word_builder& sensitive_word_builder::num_check_len(size_t value)
{
    config_.num_check_len = value;
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

sensitive_word_builder& sensitive_word_builder::add_deny_words_from_text(std::string_view text)
{
    append_words(deny_words_, load_words_from_text(text));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_allow_words_from_text(std::string_view text)
{
    append_words(allow_words_, load_words_from_text(text));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_deny_words_from_file(const std::string& file_path)
{
    append_words(deny_words_, load_words_from_file(file_path));
    return *this;
}

sensitive_word_builder& sensitive_word_builder::add_allow_words_from_file(const std::string& file_path)
{
    append_words(allow_words_, load_words_from_file(file_path));
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

sensitive_word_engine sensitive_word_builder::build()
{
    return sensitive_word_engine(
                std::move(config_),
                std::move(deny_words_),
                std::move(allow_words_),
                std::move(char_ignore_),
                std::move(result_condition_),
                std::move(replace_strategy_));
}

////////////////////////////////////////////////////////////
// 敏感词匹配引擎内部实现
////////////////////////////////////////////////////////////
class sensitive_word_engine::impl
{
public:
    impl(
        sensitive_word_config                   config,
        std::vector<std::string>                deny_words,
        std::vector<std::string>                allow_words,
        std::shared_ptr<class char_ignore>      char_ignore,
        std::shared_ptr<class result_condition> result_condition,
        std::shared_ptr<class replace_strategy> replace_strategy)
        : config_(std::move(config))
        , char_ignore_(std::move(char_ignore))
        , result_condition_(std::move(result_condition))
        , replace_strategy_(std::move(replace_strategy))
    {
        if (!char_ignore_)
            char_ignore_ = char_ignores::none();

        if (!result_condition_)
            result_condition_ = result_conditions::english_word_match();

        if (!replace_strategy_)
            replace_strategy_ = replace_strategies::stars();

        rebuild_runtime();

        for (const auto& word : deny_words)
        {
            add_word(word);
        }

        for (const auto& word : allow_words)
        {
            add_allow_word(word);
        }
    }

    impl(const impl& other)
        : config_(other.config_)
        , char_ignore_(other.char_ignore_)
        , result_condition_(other.result_condition_)
        , replace_strategy_(other.replace_strategy_)
        , dictionary_(other.dictionary_)
    {
        rebuild_runtime();
    }

    impl& operator=(const impl& other)
    {
        if (this != &other)
        {
            config_           = other.config_;
            char_ignore_      = other.char_ignore_;
            result_condition_ = other.result_condition_;
            replace_strategy_ = other.replace_strategy_;
            dictionary_       = other.dictionary_;

            rebuild_runtime();
        }

        return *this;
    }

    bool contains(std::string_view text) const
    {
        return static_cast<bool>(find_first(text));
    }

    std::optional<word_result> find_first(std::string_view text) const
    {
        std::optional<word_result> first;
        scan_matches(text, [&](word_result&& result) {
            first = std::move(result);
            return false;
        });
        return first;
    }

    std::vector<word_result> find_all(std::string_view text) const
    {
        std::vector<word_result> results;
        scan_matches(text, [&](word_result&& result) {
            results.push_back(std::move(result));
            return true;
        });
        return results;
    }

    std::string replace(std::string_view text) const
    {
        return replace(text, *replace_strategy_);
    }

    std::string replace(std::string_view text, const replace_strategy& strategy) const
    {
        return replace(text, find_all(text), strategy);
    }

    std::string replace(std::string_view text, const std::vector<word_result>& results) const
    {
        return replace(text, results, *replace_strategy_);
    }

    std::string replace(std::string_view text, const std::vector<word_result>& results, const replace_strategy& strategy) const
    {
        if (results.empty())
            return std::string(text);

        std::string out;
        out.reserve(text.size());

        size_t cursor = 0;
        for (const auto& result : results)
        {
            if (result.raw_begin > result.raw_end || result.raw_end > text.size() || result.raw_begin < cursor)
                continue;

            if (cursor < result.raw_begin)
                out.append(text.substr(cursor, result.raw_begin - cursor));

            out   += strategy.replacement_for(result, text);
            cursor = result.raw_end;
        }

        if (cursor < text.size())
            out.append(text.substr(cursor));

        return out;
    }

    void add_word(std::string_view word)
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
            return;

        dictionary_.add_word(normalized, trie_word_kind::deny);
    }

    void remove_word(std::string_view word)
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
            return;

        dictionary_.remove_word(normalized, trie_word_kind::deny);
    }

    void add_allow_word(std::string_view word)
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
            return;

        dictionary_.add_word(normalized, trie_word_kind::allow);
    }

    void remove_allow_word(std::string_view word)
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
            return;

        dictionary_.remove_word(normalized, trie_word_kind::allow);
    }

    word_entry_status query_word_status(std::string_view word) const
    {
        const std::u32string normalized = normalizer_->normalize_word(word);
        if (normalized.empty())
            return {};

        const trie_terminal_flags flags = dictionary_.find_word(normalized);
        return word_entry_status{
            flags.any(),
            flags.deny,
            flags.allow,
        };
    }

    const sensitive_word_config& config() const noexcept
    {
        return config_;
    }

private:
    struct match_span
    {
        size_t raw_length       = 0;
        size_t effective_length = 0;
    };

    struct word_scan_result
    {
        match_span allow{};
        match_span deny{};
    };

    struct num_scan_result
    {
        size_t         deny_length = 0;
        std::u32string normalized_deny_word{};
    };

    void rebuild_runtime()
    {
        normalizer_ = std::make_unique<text_normalizer>(text_normalizer_options{
            config_.ignore_case,
            config_.ignore_width,
            config_.ignore_num_style,
            config_.ignore_chinese_style,
            config_.ignore_english_style,
        });
    }

    template <typename OnMatch>
    void scan_matches(std::string_view text, OnMatch&& on_match) const
    {
        // 统一扫描入口：
        // 1. 优先尝试词库匹配；
        // 2. 只有当前位置既没有 allow 也没有 deny 词命中时，才退化到数字匹配。
        if (text.empty() || (!config_.enable_word_check && !config_.enable_num_check))
            return;

        const normalized_text normalized = normalizer_->normalize_text(text);
        if (normalized.normalized_chars.empty())
            return;

        for (size_t idx = 0; idx < normalized.normalized_chars.size(); ++idx)
        {
            match_span     allow_match{};
            match_span     deny_match{};
            match_type     detected_type{match_type::word};
            std::u32string normalized_deny_word;

            if (config_.enable_word_check)
            {
                const word_scan_result detect_result = detect_word_at(normalized, idx);
                allow_match = detect_result.allow;
                deny_match  = detect_result.deny;
            }

            if (allow_match.raw_length == 0 && deny_match.raw_length == 0 && config_.enable_num_check)
            {
                const num_scan_result detect_result = detect_num_at(normalized, idx);
                if (detect_result.deny_length > 0)
                {
                    deny_match.raw_length       = detect_result.deny_length;
                    deny_match.effective_length = detect_result.normalized_deny_word.size();
                    normalized_deny_word        = detect_result.normalized_deny_word;
                    detected_type               = match_type::num;
                }
            }

            // allow / deny 同起点同时命中时，采用“更长者优先”。
            // 只有 deny 明确长于 allow 时，当前位置才会被判定为 deny 命中；
            // 否则由 allow 覆盖，并直接跳过 allow 的跨度。
            if (allow_match.raw_length < deny_match.raw_length && deny_match.raw_length > 0)
            {
                auto result = make_word_result(text, normalized, idx, deny_match, std::move(normalized_deny_word), detected_type);
                if ((!result_condition_ || result_condition_->match(result, text)) && !on_match(std::move(result)))
                    return;

                idx += deny_match.raw_length - 1;
            }
            else if (allow_match.raw_length > 0)
            {
                idx += allow_match.raw_length - 1;
            }
        }
    }

    word_scan_result detect_word_at(const normalized_text& text, size_t begin_index) const
    {
        word_scan_result result{};
        if (begin_index >= text.normalized_chars.size())
            return result;

        auto state        = dictionary_.root_state();
        bool allow_active = true;
        bool deny_active  = true;

        size_t   raw_length         = 0;
        size_t   effective_length   = 0;
        char32_t last_effective     = 0;
        bool     has_last_effective = false;

        for (size_t idx = begin_index; idx < text.normalized_chars.size(); ++idx)
        {
            const auto& item = text.normalized_chars[idx];
            ++raw_length;

            if (bool ignored = raw_length != 1 && char_ignore_ && char_ignore_->ignore(item.raw_code_point, item.normalized_code_point); ignored)
                continue;

            ++effective_length;

            if (bool ignored = config_.ignore_repeat && has_last_effective && last_effective == item.normalized_code_point; !ignored)
            {
                state = dictionary_.advance(state, item.normalized_code_point);
                if (!state.valid())
                    break;
            }

            const trie_terminal_flags flags = dictionary_.terminal_flags(state);
            if (allow_active && flags.allow)
            {
                result.allow.raw_length       = raw_length;
                result.allow.effective_length = effective_length;

                if (config_.word_fail_fast)
                    allow_active = false;
            }

            if (deny_active && flags.deny)
            {
                result.deny.raw_length       = raw_length;
                result.deny.effective_length = effective_length;

                if (config_.word_fail_fast)
                    deny_active = false;
            }

            if (!allow_active && !deny_active)
                break;

            last_effective     = item.normalized_code_point;
            has_last_effective = true;
        }

        if (config_.ignore_repeat)
        {
            auto extend_repeat_tail = [&](match_span& match)
            {
                if (match.raw_length == 0)
                    return;

                size_t tail_index = begin_index + match.raw_length - 1;
                while (tail_index > begin_index)
                {
                    const auto& tail_item = text.normalized_chars[tail_index];
                    if (!(char_ignore_ && char_ignore_->ignore(tail_item.raw_code_point, tail_item.normalized_code_point)))
                        break;

                    --tail_index;
                }

                const char32_t tail               = text.normalized_chars[tail_index].normalized_code_point;
                size_t         pending_raw_length = 0;
                for (size_t idx = begin_index + match.raw_length; idx < text.normalized_chars.size(); ++idx)
                {
                    const auto& item = text.normalized_chars[idx];
                    if (char_ignore_ && char_ignore_->ignore(item.raw_code_point, item.normalized_code_point))
                    {
                        ++pending_raw_length;
                        continue;
                    }

                    if (item.normalized_code_point != tail)
                        break;

                    match.raw_length += pending_raw_length + 1;
                    ++match.effective_length;
                    pending_raw_length = 0;
                }
            };

            extend_repeat_tail(result.deny);
            extend_repeat_tail(result.allow);
        }

        return result;
    }

    num_scan_result detect_num_at(const normalized_text& text, size_t begin_index) const
    {
        num_scan_result result{};
        if (begin_index >= text.normalized_chars.size())
            return result;

        // 数字匹配允许在已经开始构造数字串之后跳过可忽略字符，
        // 例如忽略连接符时，“1-2-3”仍然可以视为一个连续数字片段。
        std::u32string builder;
        builder.reserve(config_.num_check_len);

        size_t temp_len = 0;
        for (size_t idx = begin_index; idx < text.normalized_chars.size(); ++idx)
        {
            const auto& item = text.normalized_chars[idx];
            if (!builder.empty() && char_ignore_ && char_ignore_->ignore(item.raw_code_point, item.normalized_code_point))
            {
                ++temp_len;
                continue;
            }

            if (!is_ascii_digit(item.normalized_code_point))
                break;

            builder.push_back(item.normalized_code_point);
            ++temp_len;
        }

        if (builder.size() >= config_.num_check_len)
        {
            result.deny_length          = temp_len;
            result.normalized_deny_word = std::move(builder);
        }

        return result;
    }

    std::u32string build_normalized_word(const normalized_text& normalized, size_t begin_index, match_span deny_match) const
    {
        std::u32string normalized_word;
        normalized_word.reserve(deny_match.effective_length);

        for (size_t idx = begin_index; idx < begin_index + deny_match.raw_length && idx < normalized.normalized_chars.size(); ++idx)
        {
            const auto& item = normalized.normalized_chars[idx];
            if (!normalized_word.empty() && char_ignore_ && char_ignore_->ignore(item.raw_code_point, item.normalized_code_point))
                continue;

            normalized_word.push_back(item.normalized_code_point);
            if (normalized_word.size() == deny_match.effective_length)
                break;
        }

        return normalized_word;
    }

    word_result make_word_result(
        std::string_view       text,
        const normalized_text& normalized,
        size_t                 begin_index,
        match_span             deny_match,
        std::u32string         normalized_deny_word,
        match_type             detected_type) const
    {
        word_result result;
        result.raw_begin             = normalized.normalized_chars[begin_index].raw_byte_begin;
        result.raw_end               = normalized.normalized_chars[begin_index + deny_match.raw_length - 1].raw_byte_end;
        result.raw_code_point_length = deny_match.raw_length;
        result.word                  = std::string(text.substr(result.raw_begin, result.raw_end - result.raw_begin));

        if (normalized_deny_word.empty() && deny_match.effective_length > 0)
            normalized_deny_word = build_normalized_word(normalized, begin_index, deny_match);

        result.normalized_word = encode_utf8(normalized_deny_word);
        result.type            = detected_type;

        if (begin_index > 0)
        {
            const auto& left = normalized.normalized_chars[begin_index - 1];
            result.left_raw_code_point        = left.raw_code_point;
            result.left_normalized_code_point = left.normalized_code_point;
        }

        const size_t right_index = begin_index + deny_match.raw_length;
        if (right_index < normalized.normalized_chars.size())
        {
            const auto& right = normalized.normalized_chars[right_index];
            result.right_raw_code_point        = right.raw_code_point;
            result.right_normalized_code_point = right.normalized_code_point;
        }

        return result;
    }

private:
    sensitive_word_config                   config_;
    std::shared_ptr<class char_ignore>      char_ignore_;
    std::shared_ptr<class result_condition> result_condition_;
    std::shared_ptr<class replace_strategy> replace_strategy_;
    std::unique_ptr<text_normalizer>        normalizer_;
    trie_dictionary                         dictionary_;
};

sensitive_word_engine::sensitive_word_engine()
    : sensitive_word_engine(
        sensitive_word_config{},
        {},
        {},
        char_ignores::none(),
        result_conditions::english_word_match(),
        replace_strategies::stars())
{
}

sensitive_word_engine::~sensitive_word_engine() = default;

sensitive_word_engine::sensitive_word_engine(const sensitive_word_engine& other)
    : impl_(other.impl_ ? std::make_unique<impl>(*other.impl_) : nullptr)
{
}

sensitive_word_engine& sensitive_word_engine::operator=(const sensitive_word_engine& other)
{
    if (this != &other)
        impl_ = other.impl_ ? std::make_unique<impl>(*other.impl_) : nullptr;

    return *this;
}

sensitive_word_engine::sensitive_word_engine(sensitive_word_engine&& other) noexcept = default;

sensitive_word_engine& sensitive_word_engine::operator=(sensitive_word_engine&& other) noexcept = default;

sensitive_word_engine::sensitive_word_engine(
    sensitive_word_config                   config,
    std::vector<std::string>                deny_words,
    std::vector<std::string>                allow_words,
    std::shared_ptr<class char_ignore>      char_ignore,
    std::shared_ptr<class result_condition> result_condition,
    std::shared_ptr<class replace_strategy> replace_strategy)
    : impl_(std::make_unique<impl>(
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
    return impl_->find_all(text);
}

std::optional<std::string> sensitive_word_engine::find_first_word(std::string_view text) const
{
    auto result = find_first(text);
    if (!result)
        return std::nullopt;

    return std::move(result->word);
}

std::vector<std::string> sensitive_word_engine::find_all_words(std::string_view text) const
{
    std::vector<std::string> words;

    auto results = find_all(text);
    words.reserve(results.size());

    for (auto& result : results)
    {
        words.push_back(std::move(result.word));
    }

    return words;
}

std::string sensitive_word_engine::replace(std::string_view text) const
{
    return impl_->replace(text);
}

std::string sensitive_word_engine::replace(std::string_view text, char replacement) const
{
    chars_replace_strategy strategy(replacement);
    return impl_->replace(text, strategy);
}

std::string sensitive_word_engine::replace(std::string_view text, const replace_strategy& strategy) const
{
    return impl_->replace(text, strategy);
}

std::string sensitive_word_engine::replace(std::string_view text, const std::vector<word_result>& results) const
{
    return impl_->replace(text, results);
}

std::string sensitive_word_engine::replace(std::string_view text, const std::vector<word_result>& results, char replacement) const
{
    chars_replace_strategy strategy(replacement);
    return impl_->replace(text, results, strategy);
}

std::string sensitive_word_engine::replace(std::string_view text, const std::vector<word_result>& results, const replace_strategy& strategy) const
{
    return impl_->replace(text, results, strategy);
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

word_entry_status sensitive_word_engine::query_word_status(std::string_view word) const
{
    return impl_->query_word_status(word);
}

const sensitive_word_config& sensitive_word_engine::config() const noexcept
{
    return impl_->config();
}

namespace char_ignores {

std::shared_ptr<char_ignore> none()
{
    return std::make_shared<none_char_ignore>();
}

std::shared_ptr<char_ignore> special_chars()
{
    return std::make_shared<special_char_ignore>();
}

} // namespace char_ignores

namespace result_conditions {

std::shared_ptr<result_condition> always_true()
{
    return std::make_shared<always_true_result_condition>();
}

std::shared_ptr<result_condition> english_word_match()
{
    return std::make_shared<english_word_match_result_condition>();
}

std::shared_ptr<result_condition> english_word_num_match()
{
    return std::make_shared<english_word_num_match_result_condition>();
}

} // namespace result_conditions

namespace replace_strategies {

std::shared_ptr<replace_strategy> stars()
{
    return std::make_shared<chars_replace_strategy>('*');
}

std::shared_ptr<replace_strategy> chars(char replacement)
{
    return std::make_shared<chars_replace_strategy>(replacement);
}

} // namespace replace_strategies
} // namespace sensitive_word