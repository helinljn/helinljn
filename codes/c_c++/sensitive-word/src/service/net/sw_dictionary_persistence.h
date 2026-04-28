#pragma once

#ifndef SW_DICTIONARY_PERSISTENCE_H
#define SW_DICTIONARY_PERSISTENCE_H

#include "net/sw_http_common.h"
#include "net/sw_http_server.h"
#include <set>
#include <string>

namespace net {

/**
 * @brief 从配置指定的文件中加载 deny/allow 词库
 * @param config HTTP 服务运行配置
 * @return 加载得到的词库仓储快照
 */
word_repository load_word_repository(const sw_http_server_config& config);

/**
 * @brief 根据词库快照构造一份敏感词引擎
 * @param config 敏感词引擎构建配置
 * @param repository 词库仓储快照
 * @return 基于当前词库快照构造出的敏感词引擎
 */
sensitive_word::sensitive_word_engine build_engine_from_repository(
    const sensitive_word::sensitive_word_config& config,
    const word_repository&                       repository);

/**
 * @brief 将更新命令应用到词库快照
 * @param repository 待更新的词库仓储快照
 * @param command 需要应用的更新命令
 */
void apply_update_to_word_sets(word_repository& repository,
                               const update_command& command);

/**
 * @brief 生成一条更新命令的逆操作命令
 * @param command 原始更新命令
 * @return 用于回滚的逆操作命令
 */
update_command make_inverse_command(const update_command& command);

/**
 * @brief 将词库快照持久化到配置指定的 deny/allow 文件
 * @param config HTTP 服务运行配置
 * @param repository 待持久化的词库仓储快照
 * @return true 表示持久化成功；false 表示持久化失败
 */
bool persist_word_repository(const sw_http_server_config& config,
                             const word_repository&       repository);

} // namespace net

#endif // SW_DICTIONARY_PERSISTENCE_H