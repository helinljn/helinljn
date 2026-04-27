#pragma once

#ifndef LIGHT_HOOK_H
#define LIGHT_HOOK_H

#include "core.h"

/**
 * @file light_hook.h
 * @brief 轻量级 inline hook 接口
 *
 * 该实现通过覆盖原函数入口的前若干字节，在入口处写入一段绝对跳转指令，
 * 使执行流跳转到用户提供的目标函数；同时将被覆盖的原始指令复制到跳板
 * （trampoline）中，并在跳板尾部跳回原函数剩余部分，从而允许目标函数
 * 继续调用“原函数逻辑”。
 *
 * 设计目标：
 * - 尽量减少依赖，不引入大型第三方 hook 框架
 * - 保持实现简单，适合作为轻量级运行时拦截组件
 * - 提供基础的启用/禁用 hook 与 trampoline 回调能力
 *
 * 当前实现的优点：
 * - 代码体积小，依赖少，便于集成到现有项目
 * - 同时提供 create / enable / disable 的最小闭环接口
 * - 支持为目标函数保留 trampoline，便于在 hook 后继续调用原逻辑
 * - 适合受控场景下的快速函数拦截，例如调试、埋点、原型验证、初始化期替换
 *
 * 当前实现更适合解决的问题：
 * - 进程初始化阶段、单线程阶段或低并发场景下的函数替换
 * - 对特定函数进行快速观测、日志插桩、统计或临时行为重定向
 * - 不希望引入额外大型依赖时的轻量级 inline hook 需求
 *
 * 当前支持范围说明：
 * - 当前实现按 x86/x64 指令解析思路编写，但入口补丁与跳转模板按 x86_64 方式组织
 * - 实际上应将其视为面向 x86_64 的轻量级 inline hook 实现
 * - 不保证支持 32 位 x86
 * - 不支持 ARM、ARM64、RISC-V 等其他指令体系结构
 *
 * 当前实现的限制与已知问题：
 * - 不保证多线程高频调用场景下的“原子替换”安全性：
 *   enable_hook()/disable_hook() 会直接改写原函数入口字节，但不会暂停其他线程，
 *   也不会保证入口补丁整体原子生效；若其他线程在补丁写入过程中执行到该函数，
 *   可能读到半写入指令并导致崩溃。
 * - enable_hook()/disable_hook() 本身不是并发安全接口：
 *   hook_information_t::enabled 只是普通整数，若多个线程同时启用/禁用同一个 hook，
 *   可能产生数据竞争和状态错乱。
 * - trampoline 当前仅复制原始指令字节，不保证对所有相对寻址指令进行重定位修正：
 *   若被复制指令包含 RIP-relative addressing、相对 call/jmp/jcc 等，复制到新地址后
 *   可能失效，进而导致行为错误或崩溃。
 * - original_buffer 固定为 32 字节：
 *   对大多数常见函数前导通常足够，但理论上仍可能不足以容纳需要完整复制的指令序列。
 * - 当前实现更偏向“轻量”和“可用”，不等同于成熟 hook 框架的热补丁能力。
 *
 * 使用建议：
 * - 尽量只在受控时机调用 enable_hook()/disable_hook()，例如程序初始化阶段
 * - 避免在目标函数被高频、多线程并发调用时在线启用/禁用 hook
 * - 若需要生产级热切换能力，应额外引入线程同步、原子 patch、异常协同或更完整的
 *   指令重定位机制
 */

/**
 * @brief 跳板 Hook 信息
 * 用于保存原函数地址、目标函数地址、原始指令备份以及生成的跳板地址。
 */
struct hook_information_t
{
    int           enabled;              // 当前 Hook 是否已启用
    int           bytes_to_copy;        // 为构建跳板而复制的原始指令字节数
    unsigned char original_buffer[32];  // 原函数前若干字节的备份
    void*         original_function;    // 被 Hook 的原函数
    void*         target_function;      // Hook 后跳转到的目标函数
    void*         trampoline;           // 生成的跳板函数地址
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
 * @warning 当前实现不保证多线程在线热切换时的原子安全性。
 * 若其他线程正在高频调用 original_function，则在入口补丁写入过程中存在
 * 观察到半写入状态的风险，可能导致崩溃。
 *
 * 建议仅在受控阶段调用，例如：
 * - 程序初始化期
 * - 相关业务线程尚未启动时
 * - 已确保不会有其他线程并发进入该函数时
 */
CORE_API int enable_hook(hook_information_t* information);

/**
 * @brief 禁用 Hook
 * @param information 由 create_hook 创建的 Hook 信息
 * @return 非 0 表示成功，0 表示失败
 *
 * @warning 与 enable_hook() 类似，当前实现恢复原函数入口时也不保证多线程
 * 原子安全性；若在高并发执行期间在线撤销 hook，同样存在半恢复状态被执行的风险。
 */
CORE_API int disable_hook(hook_information_t* information);

#endif // LIGHT_HOOK_H