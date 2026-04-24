#include "sw/trie_dictionary.h"
#include <algorithm>

namespace sensitive_word {

trie_dictionary::trie_dictionary()
    : node_pool_()
{
    std::fill(std::begin(root_ascii_cache_), std::end(root_ascii_cache_), 0xFFFFFFFF);
    // 预分配根节点，索引始终为 0
    allocate_node();
}

trie_dictionary::trie_dictionary(const trie_dictionary& other)
    : node_pool_(other.node_pool_)
{
    std::copy(std::begin(other.root_ascii_cache_), std::end(other.root_ascii_cache_), std::begin(root_ascii_cache_));
}

trie_dictionary& trie_dictionary::operator=(const trie_dictionary& other)
{
    if (this != &other)
    {
        node_pool_ = other.node_pool_;
        std::copy(std::begin(other.root_ascii_cache_), std::end(other.root_ascii_cache_), std::begin(root_ascii_cache_));
    }

    return *this;
}

trie_dictionary::trie_dictionary(trie_dictionary&& other) noexcept
    : node_pool_(std::move(other.node_pool_))
{
    std::copy(std::begin(other.root_ascii_cache_), std::end(other.root_ascii_cache_), std::begin(root_ascii_cache_));

    // 恢复 other 为有效且为空的状态
    std::fill(std::begin(other.root_ascii_cache_), std::end(other.root_ascii_cache_), 0xFFFFFFFF);
    if (other.node_pool_.empty())
        other.allocate_node();
}

trie_dictionary& trie_dictionary::operator=(trie_dictionary&& other) noexcept
{
    if (this != &other)
    {
        node_pool_ = std::move(other.node_pool_);
        std::copy(std::begin(other.root_ascii_cache_), std::end(other.root_ascii_cache_), std::begin(root_ascii_cache_));

        // 恢复 other 为有效且为空的状态
        std::fill(std::begin(other.root_ascii_cache_), std::end(other.root_ascii_cache_), 0xFFFFFFFF);
        if (other.node_pool_.empty())
            other.allocate_node();
    }

    return *this;
}

void trie_dictionary::add_word(const std::u32string& word, trie_word_kind kind)
{
    if (word.empty())
        return;

    // 根节点固定为 0
    uint32_t curr_idx = 0;
    for (size_t i = 0; i < word.size(); ++i)
    {
        char32_t ch = word[i];

        // 根节点的 ASCII 缓存加速
        if (curr_idx == 0 && ch < 128)
        {
            if (root_ascii_cache_[ch] == 0xFFFFFFFF)
            {
                uint32_t new_idx = allocate_node();
                root_ascii_cache_[ch] = new_idx;

                // 也要插入到 next 数组中，保持完整结构以供 remove 时使用
                auto& root_node = node_pool_[0];
                auto  it        = std::lower_bound(root_node.next.begin(), root_node.next.end(), ch,
                                           [](const auto& pair, char32_t val) { return pair.first < val; });
                root_node.next.insert(it, {ch, new_idx});
            }
            curr_idx = root_ascii_cache_[ch];
            continue;
        }

        // 普通节点二分查找
        auto& node = node_pool_[curr_idx];
        auto  it   = std::lower_bound(node.next.begin(), node.next.end(), ch,
                                     [](const auto& pair, char32_t val) { return pair.first < val; });

        if (it == node.next.end() || it->first != ch)
        {
            uint32_t new_idx = allocate_node();
            // allocate_node 可能会导致 node_pool_ 扩容和迭代器失效，需要重新获取 node 和 iterator
            auto& fresh_node = node_pool_[curr_idx];
            auto  fresh_it   = std::lower_bound(fresh_node.next.begin(), fresh_node.next.end(), ch,
                                                [](const auto& pair, char32_t val) { return pair.first < val; });
            fresh_it = fresh_node.next.insert(fresh_it, {ch, new_idx});
            curr_idx = fresh_it->second;
        }
        else
        {
            curr_idx = it->second;
        }
    }

    if (kind == trie_word_kind::allow)
        node_pool_[curr_idx].terminal.allow = true;
    else
        node_pool_[curr_idx].terminal.deny = true;
}

void trie_dictionary::remove_word(const std::u32string& word, trie_word_kind kind)
{
    if (word.empty() || node_pool_.empty())
        return;

    remove_word_recursive(0, word, 0, kind);
}

trie_dictionary::traversal_state trie_dictionary::root_state() const noexcept
{
    if (node_pool_.empty())
        return {};
    return traversal_state{0};
}

trie_dictionary::traversal_state trie_dictionary::advance(traversal_state state, char32_t ch) const noexcept
{
    if (!state.valid() || state.node_index >= node_pool_.size())
        return {};

    if (state.node_index == 0 && ch < 128)
    {
        return traversal_state{root_ascii_cache_[ch]};
    }

    const auto& node = node_pool_[state.node_index];
    const auto  it   = std::lower_bound(node.next.begin(), node.next.end(), ch,
                                        [](const auto& pair, char32_t val) { return pair.first < val; });

    if (it == node.next.end() || it->first != ch)
        return {};

    return traversal_state{it->second};
}

trie_terminal_flags trie_dictionary::terminal_flags(traversal_state state) const noexcept
{
    if (!state.valid() || state.node_index >= node_pool_.size())
        return {};

    return node_pool_[state.node_index].terminal;
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

uint32_t trie_dictionary::allocate_node()
{
    uint32_t idx = static_cast<uint32_t>(node_pool_.size());
    node_pool_.emplace_back();
    return idx;
}

bool trie_dictionary::remove_word_recursive(uint32_t node_idx, const std::u32string& word, size_t index, trie_word_kind kind)
{
    auto& node = node_pool_[node_idx];

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
    auto           it = std::lower_bound(node.next.begin(), node.next.end(), ch,
                                         [](const auto& pair, char32_t val) { return pair.first < val; });

    if (it == node.next.end() || it->first != ch)
        return false;

    uint32_t   child_idx          = it->second;
    const bool should_erase_child = remove_word_recursive(child_idx, word, index + 1, kind);

    // 递归返回后，node 引用可能因为 vector 扩容失效（虽然 remove 期间通常不会分配新节点），重新获取一次安全
    auto& safe_node = node_pool_[node_idx];
    if (should_erase_child)
    {
        auto safe_it = std::lower_bound(safe_node.next.begin(), safe_node.next.end(), ch,
                                        [](const auto& pair, char32_t val) { return pair.first < val; });
        if (safe_it != safe_node.next.end() && safe_it->first == ch)
        {
            safe_node.next.erase(safe_it);
        }

        // 如果是根节点，还需要清理 ASCII 缓存
        if (node_idx == 0 && ch < 128)
        {
            root_ascii_cache_[ch] = 0xFFFFFFFF;
        }
    }

    // 回收空闲的子节点逻辑被省去：由于是内存池 Arena 结构，
    // 删除节点并不会立刻归还 vector 内存。这在生命周期较长且偏向读的字典树中是可以接受的（可避免碎片化）。
    return !safe_node.terminal.any() && safe_node.next.empty();
}

} // namespace sensitive_word