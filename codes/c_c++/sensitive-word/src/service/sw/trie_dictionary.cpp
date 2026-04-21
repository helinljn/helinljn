#include "sw/trie_dictionary.h"

namespace sensitive_word
{

trie_dictionary::trie_dictionary() = default;

void trie_dictionary::add_word(const std::u32string& word)
{
    if (word.empty())
    {
        return;
    }

    trie_node* node = &root_;
    for (char32_t ch : word)
    {
        auto& child = node->next[ch];
        if (!child)
        {
            child = std::make_unique<trie_node>();
        }

        node = child.get();
    }

    node->terminal = true;
}

void trie_dictionary::remove_word(const std::u32string& word)
{
    if (word.empty())
    {
        return;
    }

    remove_word_recursive(root_, word, 0);
}

trie_contains_type trie_dictionary::contains(const std::u32string& word, bool ignore_repeat) const
{
    const trie_node* node = &root_;

    for (std::size_t i = 0; i < word.size(); ++i)
    {
        const char32_t ch = word[i];

        if (ignore_repeat && i > 0 && word[i - 1] == ch)
        {
            continue;
        }

        const auto it = node->next.find(ch);
        if (it == node->next.end() || !it->second)
        {
            return trie_contains_type::not_found;
        }

        node = it->second.get();
    }

    if (node->terminal)
    {
        return trie_contains_type::contains_end;
    }

    return trie_contains_type::prefix;
}

bool trie_dictionary::remove_word_recursive(trie_node& node, const std::u32string& word, std::size_t index)
{
    if (index >= word.size())
    {
        if (!node.terminal)
        {
            return false;
        }

        node.terminal = false;
        return node.next.empty();
    }

    const char32_t ch = word[index];
    const auto it = node.next.find(ch);
    if (it == node.next.end() || !it->second)
    {
        return false;
    }

    const bool should_erase_child = remove_word_recursive(*it->second, word, index + 1);
    if (should_erase_child)
    {
        node.next.erase(it);
    }

    return !node.terminal && node.next.empty();
}

}  // namespace sensitive_word
