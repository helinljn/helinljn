#pragma once

#ifndef SW_WORKER_REGISTRY_H
#define SW_WORKER_REGISTRY_H

#include "net/sw_http_common.h"
#include "net/sw_http_server.h"
#include "net/brynet.h"
#include <memory>
#include <vector>

namespace net {

/**
 * @brief worker 上下文注册表
 *
 * 负责管理：
 * 1. 每个业务线程对应的 worker_context；
 * 2. 当前线程 thread_local 上下文绑定；
 * 3. 词库更新命令向全部 worker 的广播同步。
 */
class sw_worker_registry
{
public:
    sw_worker_registry() = default;

    /**
     * @brief 基于 event loop 和基准引擎初始化全部 worker 上下文
     * @param event_loops worker 对应的事件循环列表
     * @param base_engine 初始化每个 worker 时使用的基准敏感词引擎
     */
    void initialize(const std::vector<EventLoopPtr>&             event_loops,
                    const sensitive_word::sensitive_word_engine& base_engine);

    /**
     * @brief 清空全部 worker 状态
     */
    void clear();

    /**
     * @brief 在每个 worker 线程中绑定 thread_local 上下文
     * @return true 表示全部绑定成功；false 表示存在绑定失败
     */
    bool bind_worker_thread_locals();

    /**
     * @brief 获取当前线程所属的 worker 上下文
     * @return 当前线程绑定的 worker 上下文；若未绑定则返回 nullptr
     */
    worker_context* current_worker() const noexcept;

    /**
     * @brief 获取 worker 数量
     * @return 当前已注册的 worker 数量
     */
    size_t size() const noexcept;

    /**
     * @brief 向全部 worker 广播一条更新命令
     * @param command 待广播的更新命令
     * @return true 表示广播成功；false 表示存在 worker 更新失败
     */
    bool broadcast_update(const update_command& command);

private:
    /**
     * @brief 将更新命令应用到单个 worker 的本地引擎
     * @param engine 目标 worker 的本地敏感词引擎
     * @param command 需要应用的更新命令
     */
    static void apply_update_to_engine(sensitive_word::sensitive_word_engine& engine,
                                       const update_command&                  command);

private:
    std::vector<EventLoopPtr>                    event_loops_;
    std::vector<std::unique_ptr<worker_context>> worker_contexts_;
};

} // namespace net

#endif // SW_WORKER_REGISTRY_H