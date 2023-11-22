#ifndef __EXE_SYMBOL_H__
#define __EXE_SYMBOL_H__

#include "util/types.h"

namespace common {

////////////////////////////////////////////////////////////////
// 读取可执行文件中的导出符号
// 导出可执行文件中的符号：
//     1. Windows下需要设置cmake的两个属性
//         SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
//             WINDOWS_EXPORT_ALL_SYMBOLS True
//             ENABLE_EXPORTS             True
//         )
//     2. Linux下需要添加-rdynamic编译选项
////////////////////////////////////////////////////////////////
class COMMON_API exe_symbol final
{
public:
    exe_symbol(void);
    ~exe_symbol(void);

    exe_symbol(const exe_symbol&) = delete;
    exe_symbol& operator=(const exe_symbol&) = delete;

    exe_symbol(exe_symbol&&) = delete;
    exe_symbol& operator=(exe_symbol&&) = delete;

    ////////////////////////////////////////////////////////////////
    // @brief 加载可执行文件
    //
    // @param
    // @return 成功返回true，失败返回false
    ////////////////////////////////////////////////////////////////
    bool load(void);

    ////////////////////////////////////////////////////////////////
    // @brief 获取给定符号名在可执行文件中的运行时地址
    //
    // @param name 符号名(该名字是经过mangling修饰的)
    // @return 成功返回有效的地址，失败返回空指针
    ////////////////////////////////////////////////////////////////
    void* get_symbol(const std::string_view& name) const;

    ////////////////////////////////////////////////////////////////
    // @brief 卸载可执行文件
    //
    // @param
    // @return 成功返回true，失败返回false
    ////////////////////////////////////////////////////////////////
    bool unload(void);

private:
    void* _handle;
};

} // namespace common

#endif // __EXE_SYMBOL_H__