#pragma once

#ifndef LIGHT_HOOK_H
#define LIGHT_HOOK_H

#include "core.h"

/**
 * @file light_hook.h
 * @brief 轻量级 inline hook 接口
 *
 * 该模块通过改写原函数入口指令，将执行流跳转到目标函数；同时生成 trampoline，
 * 保存被覆盖的原始指令并跳回原函数剩余部分，供 hook 后继续调用原逻辑。
 *
 * 核心能力：
 * - 提供 create / enable / disable 的基础 inline hook 流程。
 * - 支持 trampoline 调回原函数逻辑。
 * - enable_hook()/disable_hook() 会短暂停放其他线程并事务化写入入口补丁，
 *   适用于多线程高频调用下允许短暂停顿的在线切换场景。
 * - trampoline 会重定位常见 x86_64 前导中的 RIP-relative addressing、相对 call/jmp/jcc。
 *
 * 支持范围与限制：
 * - 仅支持用户态 x86_64，不支持 32 位 x86、ARM、ARM64、RISC-V、内核态或 EFI。
 * - 不是完全无停顿热切换；启用和禁用期间会短暂停顿其他线程。
 * - 若目标函数前导包含当前轻量重定位器不支持的指令，enable_hook() 会失败并保持原函数不变。
 * - original_buffer 固定为 128 字节，理论上可能不足以覆盖极端复杂的函数前导。
 * - Linux 实现会安装进程级 SIGTRAP 和 SIGUSR2 处理器；宿主进程若也使用这些信号，
 *   需要确认处理器链式调用语义可以接受。
 * - disable_hook() 不会释放 trampoline。调用方在拿到 trampoline 指针后应视为进程期
 *   可用代码地址，避免在线卸载时释放仍被其它线程执行的代码页。
 *
 * 使用建议：
 * - 优先在初始化阶段启用 hook；确需在线切换时，应接受短暂停顿成本。
 * - 避免多个线程同时操作同一个 hook_information_t。
 * - 若需要覆盖更复杂的机器码模式，建议引入成熟反汇编/重定位库。
 * - 不要手动修改 hook_information_t 的字段；enable_hook()/disable_hook() 会校验入口
 *   备份和补丁长度，字段不一致会导致操作失败。
 */

/**
 * @brief 跳板 Hook 信息
 * 用于保存原函数地址、目标函数地址、原始指令备份以及生成的跳板地址。
 */
struct hook_information_t
{
    int           enabled;               // 当前 Hook 是否已启用
    int           bytes_to_copy;         // 为构建跳板而复制的原始指令字节数
    int           trampoline_size;       // trampoline 分配字节数
    unsigned char original_buffer[128];  // 原函数前若干字节的备份
    void*         original_function;     // 被 Hook 的原函数
    void*         target_function;       // Hook 后跳转到的目标函数
    void*         trampoline;            // 生成的跳板函数地址
};

/**
 * @brief 创建 Hook 信息并备份原函数指令
 * @param original_function 被 Hook 的原函数
 * @param target_function   Hook 后调用的目标函数
 * @return 初始化后的 hook_information_t 结构
 *
 * 说明：
 * - 该函数主要负责分析原函数入口、计算需要复制的指令长度并备份原始字节
 * - 该函数本身不会修改原函数机器码，也不会真正启用 hook
 * - 返回的 trampoline 地址在 enable_hook() 成功后才会被创建并生效
 */
CORE_API hook_information_t create_hook(void* original_function, void* target_function);

/**
 * @brief 启用 Hook
 * @param information 由 create_hook 创建的 Hook 信息
 * @return 非 0 表示成功，0 表示失败
 *
 * 该函数会短暂停放其他线程并事务化写入入口补丁，以支持目标函数被多线程高频调用时在线启用。
 */
CORE_API int enable_hook(hook_information_t* information);

/**
 * @brief 禁用 Hook
 * @param information 由 create_hook 创建的 Hook 信息
 * @return 非 0 表示成功，0 表示失败
 *
 * 该函数会短暂停放其他线程并事务化恢复入口补丁，以支持目标函数被多线程高频调用时在线禁用。
 */
CORE_API int disable_hook(hook_information_t* information);

#endif // LIGHT_HOOK_H