#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

namespace sensitive_word
{

enum class trie_word_kind
{
    deny,
    allow,
};

struct trie_terminal_flags
{
    bool allow = false;
    bool deny = false;

    bool any() const noexcept
    {
        return allow || deny;
    }
};

class trie_dictionary
{
private:
    struct trie_node;

public:
    struct traversal_state
    {
        const void* node = nullptr;

        bool valid() const noexcept
        {
            return node != nullptr;
        }
    };

    trie_dictionary();
    trie_dictionary(const trie_dictionary& other);
    trie_dictionary& operator=(const trie_dictionary& other);
    trie_dictionary(trie_dictionary&& other) noexcept = default;
    trie_dictionary& operator=(trie_dictionary&& other) noexcept = default;
    ~trie_dictionary() = default;

    void add_word(const std::u32string& word, trie_word_kind kind);
    void remove_word(const std::u32string& word, trie_word_kind kind);

    traversal_state root_state() const noexcept;
    traversal_state advance(traversal_state state, char32_t ch) const noexcept;
    trie_terminal_flags terminal_flags(traversal_state state) const noexcept;

private:
    struct trie_node
    {
        trie_terminal_flags terminal {};
        std::unordered_map<char32_t, std::unique_ptr<trie_node>> next {};
    };

    static bool remove_word_recursive(
        trie_node& node,
        const std::u32string& word,
        std::size_t index,
        trie_word_kind kind);
    static trie_node clone_node(const trie_node& node);

private:
    trie_node root_ {};
};

}  // namespace sensitive_word
