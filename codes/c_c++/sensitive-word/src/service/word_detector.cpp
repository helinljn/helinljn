#include "word_detector.h"

#include <algorithm>
#include <utility>

namespace sensitive_word
{

class word_detector::impl
{
public:
    impl(
        word_detector_options options,
        std::shared_ptr<class char_ignore> char_ignore,
        std::shared_ptr<text_normalizer> normalizer,
        std::shared_ptr<trie_dictionary> allow_trie,
        std::shared_ptr<trie_dictionary> deny_trie)
        : options_(std::move(options))
        , char_ignore_(std::move(char_ignore))
        , normalizer_(std::move(normalizer))
        , allow_trie_(std::move(allow_trie))
        , deny_trie_(std::move(deny_trie))
    {
    }

    word_detector_result detect_at(
        const normalized_text& text,
        std::size_t begin_index) const
    {
        word_detector_result result {};

        if (begin_index >= text.normalized_chars.size())
        {
            return result;
        }

        std::u32string builder;
        std::size_t temp_len = 0;
        std::size_t best_allow_builder_size = 0;
        std::size_t best_deny_builder_size = 0;

        for (std::size_t i = begin_index; i < text.normalized_chars.size(); ++i)
        {
            const auto& item = text.normalized_chars[i];

            if (temp_len != 0 && char_ignore_ &&
                char_ignore_->ignore(item.raw_code_point, item.normalized_code_point))
            {
                ++temp_len;
                continue;
            }

            builder.push_back(item.normalized_code_point);
            ++temp_len;

            trie_contains_type allow_type = trie_contains_type::not_found;
            trie_contains_type deny_type = trie_contains_type::not_found;

            if (allow_trie_)
            {
                allow_type = allow_trie_->contains(builder, options_.ignore_repeat);
            }

            if (deny_trie_)
            {
                deny_type = deny_trie_->contains(builder, options_.ignore_repeat);
            }

            if (allow_type == trie_contains_type::contains_end)
            {
                result.allow_length = temp_len;
                best_allow_builder_size = builder.size();

                if (options_.word_fail_fast)
                {
                    allow_type = trie_contains_type::not_found;
                }
            }

            if (deny_type == trie_contains_type::contains_end)
            {
                result.deny_length = temp_len;
                best_deny_builder_size = builder.size();

                if (options_.word_fail_fast)
                {
                    deny_type = trie_contains_type::not_found;
                }
            }

            if (allow_type == trie_contains_type::not_found &&
                deny_type == trie_contains_type::not_found)
            {
                break;
            }
        }

        result.normalized_allow_word = builder.substr(
            0, std::min(best_allow_builder_size, builder.size()));
        result.normalized_deny_word = builder.substr(
            0, std::min(best_deny_builder_size, builder.size()));

        return result;
    }

private:
    word_detector_options options_ {};
    std::shared_ptr<class char_ignore> char_ignore_ {};
    std::shared_ptr<text_normalizer> normalizer_ {};
    std::shared_ptr<trie_dictionary> allow_trie_ {};
    std::shared_ptr<trie_dictionary> deny_trie_ {};
};

word_detector::word_detector(
    word_detector_options options,
    std::shared_ptr<class char_ignore> char_ignore,
    std::shared_ptr<text_normalizer> normalizer,
    std::shared_ptr<trie_dictionary> allow_trie,
    std::shared_ptr<trie_dictionary> deny_trie)
    : impl_(std::make_shared<impl>(
          std::move(options),
          std::move(char_ignore),
          std::move(normalizer),
          std::move(allow_trie),
          std::move(deny_trie)))
{
}

word_detector_result word_detector::detect_at(
    const normalized_text& text,
    std::size_t begin_index) const
{
    return impl_->detect_at(text, begin_index);
}

}  // namespace sensitive_word
