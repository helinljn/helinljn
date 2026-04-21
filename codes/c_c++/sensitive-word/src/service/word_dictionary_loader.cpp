#include "word_dictionary_loader.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace sensitive_word
{

namespace
{

std::string trim_copy(const std::string& value)
{
    std::size_t begin = 0;
    while (begin < value.size() &&
           (value[begin] == ' ' || value[begin] == '\t' || value[begin] == '\r' || value[begin] == '\n'))
    {
        ++begin;
    }

    std::size_t end = value.size();
    while (end > begin &&
           (value[end - 1] == ' ' || value[end - 1] == '\t' || value[end - 1] == '\r' || value[end - 1] == '\n'))
    {
        --end;
    }

    return value.substr(begin, end - begin);
}

bool should_skip_line(const std::string& line)
{
    if (line.empty())
    {
        return true;
    }

    if (line[0] == '#')
    {
        return true;
    }

    if (line.size() >= 2 && line[0] == '/' && line[1] == '/')
    {
        return true;
    }

    return false;
}

}  // namespace

std::vector<std::string> load_words_from_text(std::string_view text)
{
    std::vector<std::string> words;
    std::istringstream input{std::string(text)};
    std::string line;

    while (std::getline(input, line))
    {
        std::string trimmed = trim_copy(line);
        if (should_skip_line(trimmed))
        {
            continue;
        }

        words.push_back(std::move(trimmed));
    }

    return words;
}

std::vector<std::string> load_words_from_file(const std::string& file_path)
{
    std::ifstream input(file_path, std::ios::binary);
    if (!input.is_open())
    {
        throw std::runtime_error("failed to open word dictionary file: " + file_path);
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return load_words_from_text(buffer.str());
}

}  // namespace sensitive_word
