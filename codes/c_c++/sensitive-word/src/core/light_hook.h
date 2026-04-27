#pragma once

#ifndef LIGHT_HOOK_H
#define LIGHT_HOOK_H

#include "core.h"

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
 */
CORE_API hook_information_t create_hook(void* original_function, void* target_function);

/**
 * @brief 启用 Hook
 * @param information 由 create_hook 创建的 Hook 信息
 * @return 非 0 表示成功，0 表示失败
 */
CORE_API int enable_hook(hook_information_t* information);

/**
 * @brief 禁用 Hook
 * @param information 由 create_hook 创建的 Hook 信息
 * @return 非 0 表示成功，0 表示失败
 */
CORE_API int disable_hook(hook_information_t* information);

#endif // LIGHT_HOOK_H