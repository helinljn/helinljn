#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace sensitive_word
{

std::vector<std::string> load_words_from_text(std::string_view text);
std::vector<std::string> load_words_from_file(const std::string& file_path);

}  // namespace sensitive_word
