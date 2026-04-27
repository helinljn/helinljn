#include "sw/word_dictionary_loader.h"
#include <sstream>
#include <fstream>
#include <stdexcept>

namespace sensitive_word {
namespace                {

std::string trim_copy(const std::string& value)
{
    const char* whitespace = " \t\r\n";
    auto        begin      = value.find_first_not_of(whitespace);
    if (begin == std::string::npos)
        return "";

    auto end = value.find_last_not_of(whitespace);
    return value.substr(begin, end - begin + 1);
}

bool should_skip_line(const std::string& line)
{
    if (line.empty())
        return true;

    if (line[0] == '#')
        return true;

    if (line.size() >= 2 && line[0] == '/' && line[1] == '/')
        return true;

    return false;
}

template <typename InputStream>
std::vector<std::string> load_words_from_stream(InputStream& input)
{
    std::vector<std::string> words;
    std::string              line;

    while (std::getline(input, line))
    {
        std::string trimmed = trim_copy(line);
        if (should_skip_line(trimmed))
            continue;

        words.push_back(std::move(trimmed));
    }

    return words;
}

} // namespace

std::vector<std::string> load_words_from_text(std::string_view text)
{
    std::istringstream input{std::string{text}};
    return load_words_from_stream(input);
}

std::vector<std::string> load_words_from_file(const std::string& file_path)
{
    std::ifstream input{file_path, std::ios::binary};
    if (!input.is_open())
        throw std::runtime_error("failed to open word dictionary file: " + file_path);

    return load_words_from_stream(input);
}

} // namespace sensitive_word