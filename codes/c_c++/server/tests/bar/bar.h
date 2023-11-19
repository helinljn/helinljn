#ifndef __BAR_H__
#define __BAR_H__

#include "util/types.h"

#if POCO_OS == POCO_OS_WINDOWS_NT
    #ifdef BAR_EXPORTS
        #define BAR_API __declspec(dllexport)
    #else
        #define BAR_API __declspec(dllimport)
    #endif
#else
    #ifdef BAR_EXPORTS
        #define BAR_API __attribute__((visibility("default")))
    #else
        #define BAR_API
    #endif
#endif

////////////////////////////////////////////////////////////////
// 测试hook普通函数
////////////////////////////////////////////////////////////////
BAR_API std::string test_bar(int32_t val);
BAR_API std::string patch_test_bar(int32_t val);

////////////////////////////////////////////////////////////////
// 测试hook成员函数
////////////////////////////////////////////////////////////////
class BAR_API bar
{
public:
    std::string func1(const std::string& val) const;
    static std::string func2(const std::string& val);
};

class BAR_API patch_bar : public bar
{
public:
    std::string patch_func1(const std::string& val) const;
    static std::string patch_func2(const std::string& val);
};

#endif // __BAR_H__