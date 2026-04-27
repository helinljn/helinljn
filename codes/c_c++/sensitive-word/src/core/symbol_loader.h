#pragma once

#ifndef SYMBOL_LOADER_H
#define SYMBOL_LOADER_H

#include "core_port.h"
#include <string>

namespace core {

////////////////////////////////////////////////////////////////
// 符号加载器，用于加载动态库或可执行文件中的符号
// 导出符号：
//     1. Windows下需要设置cmake的两个属性
//         SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
//             WINDOWS_EXPORT_ALL_SYMBOLS True
//             ENABLE_EXPORTS             True
//         )
//     2. Linux下需要添加-rdynamic编译选项
// 获取符号：
//     1. Windows下使用VS开发工具
//         dumpbin /exports libcore.dll | findstr memory_to_hex_string
//     2. Linux下使用nm命令
//         nm -D libcore.so | grep memory_to_hex_string
////////////////////////////////////////////////////////////////
class CORE_API symbol_loader final
{
public:
    explicit symbol_loader(void);
    ~symbol_loader(void);

    symbol_loader(const symbol_loader&) = delete;
    symbol_loader& operator=(const symbol_loader&) = delete;

    symbol_loader(symbol_loader&&) = delete;
    symbol_loader& operator=(symbol_loader&&) = delete;

    /**
     * @brief 加载动态库或可执行文件
     * @param path 动态库或可执行文件路径(如果为空字符串则打开当前可执行文件)
     * @return 成功返回true，失败返回false
     */
    bool load(const std::string& path);

    /**
     * @brief 卸载已加载的动态库或可执行文件
     * @param
     * @return
     */
    void unload(void);

    /**
     * @brief 获取符号地址(函数或变量)
     * @param sname 符号名(该名字是经过mangling修饰的)
     * @return 符号地址，失败返回空指针
     */
    void* get_symbol(const std::string& sname);

    /**
     * @brief 分离已加载的动态库或可执行文件，放弃所有权
     *        主要用于Hook场景：将句柄转移给其他管理机制，避免析构时意外卸载
     * @param
     * @return 原始句柄，调用者负责后续释放；若未加载则返回nullptr
     */
    void* detach(void)
    {
        void* h = _handle;

        _handle        = nullptr;
        _should_unload = false;

        return h;
    }

private:
    void* _handle;
    bool  _should_unload;
};

} // namespace core

#endif // SYMBOL_LOADER_H