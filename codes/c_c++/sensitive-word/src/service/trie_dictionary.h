#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

namespace sensitive_word
{

enum class trie_contains_type
{
    not_found,
    prefix,
    contains_end,
};

class trie_dictionary
{
public:
    trie_dictionary();

    void add_word(const std::u32string& word);
    void remove_word(const std::u32string& word);

    trie_contains_type contains(const std::u32string& word, bool ignore_repeat) const;

private:
    struct trie_node
    {
        bool terminal = false;
        std::unordered_map<char32_t, std::unique_ptr<trie_node>> next {};
    };

    static bool remove_word_recursive(trie_node& node, const std::u32string& word, std::size_t index);

private:
    trie_node root_ {};
};

}  // namespace sensitive_word
