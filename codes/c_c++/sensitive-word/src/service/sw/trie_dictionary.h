#pragma once

#ifndef SW_TRIE_DICTIONARY_H
#define SW_TRIE_DICTIONARY_H

#include <cstddef>
#include <string>
#include <vector>
#include <utility>

namespace sensitive_word {

//////////////////////////////////////////////////////////////
// 字典树敏感词类型
// 用于区分词条在字典树中的 deny / allow 类型
//////////////////////////////////////////////////////////////
enum class trie_word_kind
{
    deny,   // 拒绝词
    allow,  // 允许词
};

//////////////////////////////////////////////////////////////
// 字典树终端标志
// 用于表示当前节点是否对应 allow / deny 词条的结束位置
//////////////////////////////////////////////////////////////
struct trie_terminal_flags
{
    bool allow = false;
    bool deny  = false;

    bool any() const noexcept
    {
        return allow || deny;
    }
};

//////////////////////////////////////////////////////////////
// 字典树
// 用于存储归一化后的 allow / deny 词条
//////////////////////////////////////////////////////////////
class trie_dictionary
{
public:
    //////////////////////////////////////////////////////////////
    // 字典树遍历状态
    // 用于表示当前遍历所处的节点状态
    //////////////////////////////////////////////////////////////
    struct traversal_state
    {
        uint32_t node_index = 0xFFFFFFFF;

        bool valid() const noexcept
        {
            return node_index != 0xFFFFFFFF;
        }
    };

public:
    trie_dictionary();
    ~trie_dictionary() = default;

    trie_dictionary(const trie_dictionary& other);
    trie_dictionary& operator=(const trie_dictionary& other);

    trie_dictionary(trie_dictionary&& other) noexcept;
    trie_dictionary& operator=(trie_dictionary&& other) noexcept;

    /**
     * @brief 添加词条
     * @param word 要添加的归一化词条
     * @param kind 词条类型
     * @return
     */
    void add_word(const std::u32string& word, trie_word_kind kind);

    /**
     * @brief 删除词条
     * @param word 要删除的归一化词条
     * @param kind 词条类型
     * @return
     */
    void remove_word(const std::u32string& word, trie_word_kind kind);

    /**
     * @brief 获取字典树根节点的遍历状态
     * @return 根节点对应的遍历状态
     */
    traversal_state root_state() const noexcept;

    /**
     * @brief 按字符推进一次字典树遍历
     * @param state 当前遍历状态
     * @param ch    要继续匹配的字符
     * @return 推进后的遍历状态；如果不存在对应分支则返回无效状态
     */
    traversal_state advance(traversal_state state, char32_t ch) const noexcept;

    /**
     * @brief 获取当前遍历状态的终端标志
     * @param state 当前遍历状态
     * @return 当前节点对应的 allow / deny 终端标志
     */
    trie_terminal_flags terminal_flags(traversal_state state) const noexcept;

    /**
     * @brief 查询完整词条的终端标志
     * @param word 要查询的归一化词条
     * @return 词条终点对应的 allow / deny 终端标志；不存在则返回空标志
     */
    trie_terminal_flags find_word(const std::u32string& word) const noexcept;

private:
    //////////////////////////////////////////////////////////////
    // 字典树节点
    // 用于保存子节点和当前节点的终端标志
    //////////////////////////////////////////////////////////////
    struct trie_node
    {
        std::vector<std::pair<char32_t, uint32_t>> next{};
        trie_terminal_flags                        terminal{};
    };

    /**
     * @brief 递归删除词条
     * @param node_idx  当前节点在内存池中的索引
     * @param word      要删除的归一化词条
     * @param index     当前处理的字符索引
     * @param kind      词条类型
     * @return 删除后当前节点是否已无子节点且不是任何词条的终点
     */
    bool remove_word_recursive(uint32_t node_idx, const std::u32string& word, size_t index, trie_word_kind kind);

    /**
     * @brief 分配一个新节点
     * @return 新节点在 node_pool_ 中的索引
     */
    uint32_t allocate_node();

private:
    std::vector<trie_node> node_pool_;
    uint32_t               root_ascii_cache_[128]; // 缓存根节点的 ASCII 子节点索引，0xFFFFFFFF 表示不存在
};

} // namespace sensitive_word

#endif // SW_TRIE_DICTIONARY_H
