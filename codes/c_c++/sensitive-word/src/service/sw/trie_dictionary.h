#pragma once

#ifndef SW_TRIE_DICTIONARY_H
#define SW_TRIE_DICTIONARY_H

#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

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

    bool any(void) const noexcept
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
        const void* node = nullptr;

        bool valid(void) const noexcept
        {
            return node != nullptr;
        }
    };

public:
    trie_dictionary(void) = default;
    ~trie_dictionary() = default;

    trie_dictionary(const trie_dictionary& other);
    trie_dictionary& operator=(const trie_dictionary& other);

    trie_dictionary(trie_dictionary&& other) noexcept = default;
    trie_dictionary& operator=(trie_dictionary&& other) noexcept = default;

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
    traversal_state root_state(void) const noexcept;

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

private:
    //////////////////////////////////////////////////////////////
    // 字典树节点
    // 用于保存子节点和当前节点的终端标志
    //////////////////////////////////////////////////////////////
    struct trie_node
    {
        std::unordered_map<char32_t, std::unique_ptr<trie_node>> next{};
        trie_terminal_flags                                      terminal{};
    };

    /**
     * @brief 递归删除词条
     * @param node  当前节点
     * @param word  要删除的归一化词条
     * @param index 当前处理的字符索引
     * @param kind  词条类型
     * @return 删除后当前节点是否已无子节点且不是任何词条的终点
     */
    static bool remove_word_recursive(trie_node& node, const std::u32string& word, size_t index, trie_word_kind kind);

    /**
     * @brief 深拷贝字典树节点
     * @param node 要深拷贝的节点
     * @return 深拷贝后的节点
     */
    static trie_node clone_node(const trie_node& node);

private:
    trie_node root_;
};

} // namespace sensitive_word

#endif // SW_TRIE_DICTIONARY_H