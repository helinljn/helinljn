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
// 用于表示字典树中的敏感词类型
//////////////////////////////////////////////////////////////
enum class trie_word_kind
{
    deny,   // 拒绝敏感词
    allow,  // 允许敏感词
};

//////////////////////////////////////////////////////////////
// 字典树终端标志
// 用于表示字典树节点是否为敏感词的终端节点
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
// 用于存储敏感词
//////////////////////////////////////////////////////////////
class trie_dictionary
{
public:
    //////////////////////////////////////////////////////////////
    // 字典树遍历状态
    // 用于表示字典树遍历的状态
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
     * @brief 添加敏感词
     * @param word 要添加的敏感词
     * @param kind 敏感词类型
     * @return
     */
    void add_word(const std::u32string& word, trie_word_kind kind);

    /**
     * @brief 删除敏感词
     * @param word 要删除的敏感词
     * @param kind 敏感词类型
     * @return
     */
    void remove_word(const std::u32string& word, trie_word_kind kind);

    /**
     * @brief 获取字典树根节点的遍历状态
     * @param
     * @return 字典树根节点的遍历状态
     */
    traversal_state root_state(void) const noexcept;

    /**
     * @brief 进行字典树遍历
     * @param state 当前遍历状态
     * @param ch    要遍历的字符
     * @return 下一个遍历状态
     */
    traversal_state advance(traversal_state state, char32_t ch) const noexcept;

    /**
     * @brief 获取当前遍历状态的终端标志
     * @param state 当前遍历状态
     * @return 当前遍历状态的终端标志
     */
    trie_terminal_flags terminal_flags(traversal_state state) const noexcept;

private:
    //////////////////////////////////////////////////////////////
    // 字典树节点
    // 用于表示字典树中的节点
    //////////////////////////////////////////////////////////////
    struct trie_node
    {
        std::unordered_map<char32_t, std::unique_ptr<trie_node>> next{};
        trie_terminal_flags                                      terminal{};
    };

    /**
     * @brief 递归删除敏感词
     * @param node  当前节点
     * @param word  要删除的敏感词
     * @param index 当前处理的字符索引
     * @param kind  敏感词类型
     * @return 是否成功删除
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