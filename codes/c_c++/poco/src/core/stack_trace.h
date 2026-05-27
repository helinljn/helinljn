#pragma once

#ifndef STACK_TRACE_H
#define STACK_TRACE_H

#include "core.h"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace core {

////////////////////////////////////////////////////////////////
// 调用栈快照
//
// 说明：
// - 构造 stack_trace 时会捕获当前线程的调用栈快照。
// - Windows 下依赖 DbgHelp 符号处理器，initialize()/uninitialize()
//   用于显式管理其生命周期；构造函数也会按需延迟初始化。
// - DbgHelp 的符号接口不是线程安全的，内部使用互斥锁串行化捕获与清理。
////////////////////////////////////////////////////////////////
class CORE_API stack_trace final
{
public:
    struct frame
    {
        void*       address{nullptr};
        std::string module;
        std::string function;
        std::string filename;
        int         line{0};
    };

public:
    /**
     * @brief 初始化调用栈符号处理器
     *
     * Windows 下初始化当前进程的 DbgHelp 符号处理器；其它平台保留为空操作。
     * 该函数可重复调用，已经初始化时直接返回。
     *
     * @throw std::runtime_error Windows 符号处理器初始化失败时抛出
     */
    static void initialize();

    /**
     * @brief 反初始化调用栈符号处理器
     *
     * Windows 下清理当前进程的 DbgHelp 符号处理器；其它平台保留为空操作。
     * 未初始化时直接返回。
     *
     * @throw std::runtime_error Windows 符号处理器清理失败时抛出
     */
    static void uninitialize();

public:
    stack_trace();

    stack_trace(const stack_trace&) = default;
    stack_trace& operator=(const stack_trace&) = default;

    stack_trace(stack_trace&&) = default;
    stack_trace& operator=(stack_trace&&) = default;

    /**
     * @brief 将当前调用栈快照格式化为字符串
     * @return 调用栈文本；没有捕获到栈帧时返回空字符串
     */
    std::string to_string() const;

private:
    static void initialize_locked();
    static void uninitialize_locked();

private:
    std::vector<frame>      frames_;
    static std::mutex       capture_mutex_;
    static std::atomic_bool initialized_;
};

} // namespace core

#endif // STACK_TRACE_H
