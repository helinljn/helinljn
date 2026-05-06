#pragma once

#ifndef SW_WORD_DICTIONARY_LOADER_H
#define SW_WORD_DICTIONARY_LOADER_H

#include <string>
#include <string_view>
#include <vector>

namespace sensitive_word {

/**
 * @brief 从文本中按行加载词条(支持注释行：# 开头或 // 开头的行)
 * @param text 文本内容；每个非空且非注释行会被解析为一个词条
 * @return 按输入顺序解析得到的词条列表
 */
std::vector<std::string> load_words_from_text(std::string_view text);

/**
 * @brief 从文件中按行加载词条(支持注释行：# 开头或 // 开头的行)
 * @param file_path 词库文件路径；每个非空且非注释行会被解析为一个词条
 * @return 按文件顺序解析得到的词条列表
 */
std::vector<std::string> load_words_from_file(const std::string& file_path);

} // namespace sensitive_word

#endif // SW_WORD_DICTIONARY_LOADER_H