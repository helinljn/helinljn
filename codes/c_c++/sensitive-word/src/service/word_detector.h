#pragma once

#include "sensitive_word.h"
#include "text_normalizer.h"
#include "trie_dictionary.h"

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>

namespace sensitive_word
{

struct word_detector_options
{
    bool ignore_repeat = false;
    bool word_fail_fast = true;
};

struct word_detector_result
{
    std::size_t allow_length = 0;
    std::size_t deny_length = 0;
    std::u32string normalized_allow_word {};
    std::u32string normalized_deny_word {};
};

class word_detector
{
public:
    word_detector(
        word_detector_options options,
        std::shared_ptr<class char_ignore> char_ignore,
        std::shared_ptr<text_normalizer> normalizer,
        std::shared_ptr<trie_dictionary> allow_trie,
        std::shared_ptr<trie_dictionary> deny_trie);

    word_detector_result detect_at(
        const normalized_text& text,
        std::size_t begin_index) const;

private:
    class impl;
    std::shared_ptr<impl> impl_ {};
};

}  // namespace sensitive_word
