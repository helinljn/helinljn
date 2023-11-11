#ifndef __PELF_HOOK_H__
#define __PELF_HOOK_H__

#include "util/common.h"
#include <string>
#include <vector>

namespace common {

////////////////////////////////////////////////////////////////
// 通过hook掉以PLT(Linux)或者IAT(Windows)进行的动态库调用，来达到
// 动态热更新部分函数的目的
// 使用限制：
//     1. 只可以hook掉当前load模块的调用，假如多个模块都存在对func的
//        调用，则需要分别进行hook
//     2. 同个源文件内的函数互相调用多使用相对地址进行跳转，此种情况下
//        无法hook这些调用
//     3. 同个模块内的函数互相调用时，一般在编译、链接时就已经确定了
//        符号，此种情况下无法hook这些调用
//     4. 只支持全局函数、普通类成员函数、普通类静态成员函数
//     5. 不支持hook虚函数
//     6. hook之后无法还原，只能使用replace返回的旧函数地址
////////////////////////////////////////////////////////////////
class COMMON_API pelfhook final
{
public:
    pelfhook(void);
    ~pelfhook(void);

    pelfhook(const pelfhook&) = delete;
    pelfhook& operator=(const pelfhook&) = delete;

    pelfhook(pelfhook&&) = delete;
    pelfhook& operator=(pelfhook&&) = delete;

    ////////////////////////////////////////////////////////////////
    // @brief 加载模块(*.dll, *.so, *executable)
    //
    // @param mname 模块名(如果是Windows或Linux的可执行文件，则填nullptr)
    // @return 成功返回true，失败返回false
    ////////////////////////////////////////////////////////////////
    bool load(const char* mname);
    bool reload(const char* mname);

    ////////////////////////////////////////////////////////////////
    // @brief 替换函数(要保证新旧函数的签名完全一致，并且名字不能相同)
    //
    // @param oldfname 被替换的旧函数名字(该名字是经过mangling修饰的)
    // @param newfaddr 新函数地址
    // @param oldfaddr 旧函数地址
    // @return 成功返回true，失败返回false
    ////////////////////////////////////////////////////////////////
    bool replace(const char* oldfname, void* newfaddr, void** oldfaddr = nullptr);

    ////////////////////////////////////////////////////////////////
    // @brief 获取当前load模块的所有PLT或者IAT入口
    //
    // @param
    // @return
    ////////////////////////////////////////////////////////////////
    std::vector<std::string> all_entries(void) const;

private:
    void* _hook;
};

} // namespace common

#endif // __PELF_HOOK_H__