#include "sw/num_detector.h"

#include <memory>
#include <utility>

namespace sensitive_word
{

class num_detector::impl
{
public:
    impl(
        num_detector_options options,
        std::shared_ptr<class char_ignore> char_ignore)
        : options_(std::move(options))
        , char_ignore_(std::move(char_ignore))
    {
    }

    num_detector_result detect_at(
        const normalized_text& text,
        std::size_t begin_index) const
    {
        num_detector_result result {};

        if (begin_index >= text.normalized_chars.size())
        {
            return result;
        }

        std::u32string builder;
        std::size_t temp_len = 0;

        for (std::size_t i = begin_index; i < text.normalized_chars.size(); ++i)
        {
            const auto& item = text.normalized_chars[i];

            if (!builder.empty() && char_ignore_ &&
                char_ignore_->ignore(item.raw_code_point, item.normalized_code_point))
            {
                ++temp_len;
                continue;
            }

            if (!is_ascii_digit(item.normalized_code_point))
            {
                break;
            }

            builder.push_back(item.normalized_code_point);
            ++temp_len;
        }

        if (builder.size() >= options_.min_length)
        {
            result.deny_length = temp_len;
            result.normalized_deny_word = std::move(builder);
        }

        return result;
    }

private:
    num_detector_options options_ {};
    std::shared_ptr<class char_ignore> char_ignore_ {};
};

num_detector::num_detector(
    num_detector_options options,
    std::shared_ptr<class char_ignore> char_ignore)
    : impl_(std::make_shared<impl>(
          std::move(options),
          std::move(char_ignore)))
{
}

num_detector_result num_detector::detect_at(
    const normalized_text& text,
    std::size_t begin_index) const
{
    return impl_->detect_at(text, begin_index);
}

}  // namespace sensitive_word
