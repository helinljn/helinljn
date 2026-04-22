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

    // allow / deny 共用同一棵 trie，只在终点标志上区分词条类型。
    // 这样可以复用公共前缀，避免维护两套独立结构。
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

    // 删除逻辑分成两步：
    // 1. 清掉目标终点上的 allow / deny 标志；
    // 2. 自底向上回收已经没有终点标志、也没有子节点的空分支。
    remove_word_recursive(root_, word, 0, kind);
}

trie_dictionary::traversal_state trie_dictionary::root_state(void) const noexcept
{
    return traversal_state{&root_};
}

trie_dictionary::traversal_state trie_dictionary::advance(traversal_state state, char32_t ch) const noexcept
{
    // traversal_state 本质上是“当前 trie 节点句柄”，
    // 供上层扫描逻辑在不暴露 trie_node 实现细节的前提下持续推进匹配。
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

trie_terminal_flags trie_dictionary::find_word(const std::u32string& word) const noexcept
{
    if (word.empty())
        return {};

    auto state = root_state();
    for (char32_t ch : word)
    {
        state = advance(state, ch);
        if (!state.valid())
            return {};
    }

    return terminal_flags(state);
}

bool trie_dictionary::remove_word_recursive(trie_node& node, const std::u32string& word, size_t index, trie_word_kind kind)
{
    // 返回值表示“当前节点是否已经可以被父节点安全擦除”，
    // 而不是“目标词条是否删除成功”。
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
    // 深拷贝用于值语义复制 trie_dictionary，
    // 每个子节点都需要重新分配，不能直接共享 unique_ptr 持有的树结构。
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