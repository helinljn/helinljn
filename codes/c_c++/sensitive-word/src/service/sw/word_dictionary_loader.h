#pragma once

#ifndef SW_WORD_DICTIONARY_LOADER_H
#define SW_WORD_DICTIONARY_LOADER_H

#include <string>
#include <string_view>
#include <vector>

namespace sensitive_word {

/**
 * @brief 从文本加载敏感词字典(支持注释行：# 开头或 // 开头的行)
 * @param text 文本内容，每行一个敏感词，支持注释
 * @return 敏感词列表
 */
std::vector<std::string> load_words_from_text(std::string_view text);

/**
 * @brief 从文件加载敏感词字典(支持注释行：# 开头或 // 开头的行)
 * @param file_path 文件路径，每行一个敏感词，支持注释
 * @return 敏感词列表
 */
std::vector<std::string> load_words_from_file(const std::string& file_path);

} // namespace sensitive_word

#endif // SW_WORD_DICTIONARY_LOADER_H