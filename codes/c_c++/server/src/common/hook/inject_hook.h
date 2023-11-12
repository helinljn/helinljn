#ifndef __INJECT_HOOK_H__
#define __INJECT_HOOK_H__

#include "util/common.h"

namespace common {

////////////////////////////////////////////////////////////////
// 通过在运行时注入跳转指令来达到热更新部分函数的目的
// 使用限制：
//     1. hook和被hook的函数不能太小(编译后小于5条指令)
//     2. 只支持全局函数、普通类成员函数、普通类静态成员函数
//     3. 不支持hook虚函数
////////////////////////////////////////////////////////////////
class COMMON_API inject_hook final
{
public:
    inject_hook(void);
    ~inject_hook(void);

    inject_hook(const inject_hook&) = delete;
    inject_hook& operator=(const inject_hook&) = delete;

    inject_hook(inject_hook&&) = delete;
    inject_hook& operator=(inject_hook&&) = delete;

    ////////////////////////////////////////////////////////////////
    // @brief 加载模块
    //
    // @param
    // @return 成功返回true，失败返回false
    ////////////////////////////////////////////////////////////////
    bool load(void);
    bool reload(void);

    ////////////////////////////////////////////////////////////////
    // @brief 替换函数(要保证新旧函数的签名完全一致，并且名字不能相同)
    //
    // @param oldfaddr 旧函数地址
    // @param newfaddr 新函数地址
    // @return 成功返回true，失败返回false
    ////////////////////////////////////////////////////////////////
    bool replace(void* oldfaddr, void* newfaddr);

    ////////////////////////////////////////////////////////////////
    // @brief 应用当前所有的替换函数操作
    //
    // @param
    // @return 成功返回true，失败返回false
    ////////////////////////////////////////////////////////////////
    bool install(void);

    ////////////////////////////////////////////////////////////////
    // @brief 恢复当前所有的已被替换函数
    //
    // @param
    // @return 成功返回true，失败返回false
    ////////////////////////////////////////////////////////////////
    bool uninstall(void);

private:
    void* _hook;
};

} // namespace common

#endif // __INJECT_HOOK_H__