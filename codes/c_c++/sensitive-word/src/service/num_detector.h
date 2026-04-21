#pragma once

#include "sensitive_word.h"
#include "text_normalizer.h"

#include <cstddef>
#include <memory>
#include <string>

namespace sensitive_word
{

struct num_detector_options
{
    std::size_t min_length = 8;
};

struct num_detector_result
{
    std::size_t deny_length = 0;
    std::u32string normalized_deny_word {};
};

class num_detector
{
public:
    num_detector(
        num_detector_options options,
        std::shared_ptr<class char_ignore> char_ignore);

    num_detector_result detect_at(
        const normalized_text& text,
        std::size_t begin_index) const;

private:
    class impl;
    std::shared_ptr<impl> impl_ {};
};

}  // namespace sensitive_word
