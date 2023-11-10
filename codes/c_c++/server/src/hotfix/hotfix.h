#ifndef __HOTFIX_H__
#define __HOTFIX_H__

#include "util/poco.h"
#include "Poco/Foundation.h"

#if POCO_OS == POCO_OS_WINDOWS_NT
    #ifdef HOTFIX_EXPORTS
        #define HOTFIX_API __declspec(dllexport)
    #else
        #define HOTFIX_API __declspec(dllimport)
    #endif
#else
    #ifdef HOTFIX_EXPORTS
        #define HOTFIX_API __attribute__((visibility("default")))
    #else
        #define HOTFIX_API
    #endif
#endif

////////////////////////////////////////////////////////////////
// 测试hook普通函数
////////////////////////////////////////////////////////////////
HOTFIX_API std::string test_func(int32_t val);
HOTFIX_API std::string hotfix_test_func(int32_t val);

////////////////////////////////////////////////////////////////
// 测试hook成员函数
////////////////////////////////////////////////////////////////
class HOTFIX_API foo
{
public:
    std::string func1(const std::string& val) const;
    static std::string func2(const std::string& val);
};

class HOTFIX_API hotfix_foo
{
public:
    std::string hotfix_func1(const std::string& val) const;
    static std::string hotfix_func2(const std::string& val);
};

#endif // __HOTFIX_H__