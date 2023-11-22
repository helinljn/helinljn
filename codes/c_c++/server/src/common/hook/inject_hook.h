#ifndef __INJECT_HOOK_H__
#define __INJECT_HOOK_H__

#include "util/types.h"

namespace common {

////////////////////////////////////////////////////////////////
// 通过在运行时注入跳转指令，来达到热更新部分函数的目的
// 使用限制：
//     1. 支持hook动态库中的调用(如果要hook静态库中的调用，可以获取
//        "特征码"的地址，然后通过偏移算出运行时的函数地址)
//     2. 支持hook可执行文件中的调用(Windows下需要导出所有用于构建
//        可执行文件的符号，Linux下需要添加-rdynamic编译选项)
//     3. 被hook和hook的函数不能太小，编译后的函数指令数量应不小于5条
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