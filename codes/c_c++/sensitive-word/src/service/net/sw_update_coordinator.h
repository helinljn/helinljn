#pragma once

#ifndef SW_UPDATE_COORDINATOR_H
#define SW_UPDATE_COORDINATOR_H

#include "net/sw_dictionary_persistence.h"
#include "net/sw_worker_registry.h"
#include <atomic>
#include <mutex>

namespace net {

/**
 * @brief 词库更新协调器
 *
 * 负责维护更新事务的一致性流程：
 * 1. 生成版本号；
 * 2. 广播更新到所有 worker；
 * 3. 持久化词库文件；
 * 4. 失败时执行回滚；
 * 5. 维护 committed_version 和 degraded 状态。
 */
class sw_update_coordinator
{
public:
    sw_update_coordinator() = default;

    /**
     * @brief 初始化协调器依赖和初始词库快照
     * @param config HTTP 服务运行配置
     * @param worker_registry worker 上下文注册表
     * @param repository 初始词库仓储快照
     */
    void initialize(const sw_http_server_config* config,
                    sw_worker_registry*          worker_registry,
                    word_repository              repository);

    /**
     * @brief 重置协调器内部状态
     */
    void reset();

    /**
     * @brief 获取当前词库快照的拷贝
     * @return 当前词库仓储快照
     */
    word_repository snapshot_repository() const;

    /**
     * @brief 获取最近一次成功提交的版本号
     * @return 最近一次成功提交的词库版本号
     */
    uint64_t committed_version() const noexcept;

    /**
     * @brief 查询当前是否处于降级状态
     * @return true 表示当前处于降级状态；false 表示当前状态正常
     */
    bool degraded() const noexcept;

    /**
     * @brief 执行一次 add/remove 更新事务
     * @param list_type 词库类型，通常为 deny 或 allow
     * @param words 本次需要处理的词列表
     * @param is_add true 表示新增；false 表示删除
     * @param request_id 请求链路标识
     * @return 本次更新事务的处理结果
     */
    http_result apply_update(const std::string& list_type,
                             std::vector<std::string> words,
                             bool                     is_add,
                             const std::string&       request_id);

private:
    /**
     * @brief 根据 list_type 和动作方向解析更新操作类型
     * @param list_type 词库类型
     * @param is_add true 表示新增；false 表示删除
     * @param ok 输出参数，解析成功时写入 true，否则写入 false
     * @return 解析得到的更新操作类型
     */
    update_op resolve_update_op(const std::string& list_type,
                                bool               is_add,
                                bool&              ok) const;

private:
    const sw_http_server_config* config_           = nullptr;
    sw_worker_registry*          worker_registry_  = nullptr;

    std::mutex                   apply_mutex_;
    mutable std::mutex           state_mutex_;
    word_repository              repository_;
    std::atomic<uint64_t>        next_version_{0};
    std::atomic<uint64_t>        committed_version_{0};
    std::atomic<bool>            degraded_{false};
};

} // namespace net

#endif // SW_UPDATE_COORDINATOR_H