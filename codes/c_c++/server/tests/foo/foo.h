#ifndef __FOO_H__
#define __FOO_H__

#include "util/poco.h"
#include "Poco/Foundation.h"

#if POCO_OS == POCO_OS_WINDOWS_NT
    #ifdef FOO_EXPORTS
        #define FOO_API __declspec(dllexport)
    #else
        #define FOO_API __declspec(dllimport)
    #endif
#else
    #ifdef FOO_EXPORTS
        #define FOO_API __attribute__((visibility("default")))
    #else
        #define FOO_API
    #endif
#endif

////////////////////////////////////////////////////////////////
// 测试hook普通函数
////////////////////////////////////////////////////////////////
FOO_API std::string test_foo(int32_t val);
FOO_API std::string hotfix_test_foo(int32_t val);

////////////////////////////////////////////////////////////////
// 测试hook成员函数
////////////////////////////////////////////////////////////////
class FOO_API foo
{
public:
    std::string func1(const std::string& val) const;
    static std::string func2(const std::string& val);
};

class FOO_API hotfix_foo
{
public:
    std::string hotfix_func1(const std::string& val) const;
    static std::string hotfix_func2(const std::string& val);
};

#endif // __FOO_H__