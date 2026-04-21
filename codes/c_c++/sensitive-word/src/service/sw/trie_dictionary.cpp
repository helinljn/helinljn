#include "sw/trie_dictionary.h"

namespace sensitive_word {

trie_dictionary::trie_dictionary(const trie_dictionary& other)
    : root_(clone_node(other.root_))
{
}

trie_dictionary& trie_dictionary::operator=(const trie_dictionary& other)
{
    if (this != &other)
        root_ = clone_node(other.root_);

    return *this;
}

void trie_dictionary::add_word(const std::u32string& word, trie_word_kind kind)
{
    if (word.empty())
        return;

    trie_node* node = &root_;
    for (char32_t ch : word)
    {
        auto& child = node->next[ch];
        if (!child)
            child = std::make_unique<trie_node>();

        node = child.get();
    }

    if (kind == trie_word_kind::allow)
        node->terminal.allow = true;
    else
        node->terminal.deny = true;
}

void trie_dictionary::remove_word(const std::u32string& word, trie_word_kind kind)
{
    if (word.empty())
        return;

    remove_word_recursive(root_, word, 0, kind);
}

trie_dictionary::traversal_state trie_dictionary::root_state(void) const noexcept
{
    return traversal_state{&root_};
}

trie_dictionary::traversal_state trie_dictionary::advance(traversal_state state, char32_t ch) const noexcept
{
    if (!state.valid())
        return {};

    const auto* node = static_cast<const trie_node*>(state.node);
    const auto it    = node->next.find(ch);
    if (it == node->next.end() || !it->second)
        return {};

    return traversal_state{it->second.get()};
}

trie_terminal_flags trie_dictionary::terminal_flags(traversal_state state) const noexcept
{
    if (!state.valid())
        return {};

    const auto* node = static_cast<const trie_node*>(state.node);
    return node->terminal;
}

bool trie_dictionary::remove_word_recursive(trie_node& node, const std::u32string& word, size_t index, trie_word_kind kind)
{
    if (index >= word.size())
    {
        if (kind == trie_word_kind::allow)
        {
            if (!node.terminal.allow)
                return false;

            node.terminal.allow = false;
        }
        else
        {
            if (!node.terminal.deny)
                return false;

            node.terminal.deny = false;
        }

        return !node.terminal.any() && node.next.empty();
    }

    const char32_t ch = word[index];
    const auto     it = node.next.find(ch);
    if (it == node.next.end() || !it->second)
        return false;

    const bool should_erase_child = remove_word_recursive(*it->second, word, index + 1, kind);
    if (should_erase_child)
        node.next.erase(it);

    return !node.terminal.any() && node.next.empty();
}

trie_dictionary::trie_node trie_dictionary::clone_node(const trie_node& node)
{
    trie_node copy;
    copy.terminal = node.terminal;

    for (const auto& [ch, child] : node.next)
    {
        if (child)
            copy.next.emplace(ch, std::make_unique<trie_node>(clone_node(*child)));
    }

    return copy;
}

} // namespace sensitive_word