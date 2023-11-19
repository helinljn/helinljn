#ifndef __FOO_H__
#define __FOO_H__

#include "util/types.h"

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
FOO_API std::string patch_test_foo(int32_t val);

////////////////////////////////////////////////////////////////
// 测试hook成员函数
////////////////////////////////////////////////////////////////
class FOO_API foo_base
{
public:
    virtual std::string func1(const std::string& val) const;
};

class FOO_API foo : public foo_base
{
public:
    std::string func1(const std::string& val) const override;
    std::string func2(const std::string& val) const;
    static std::string func3(const std::string& val);
};

class FOO_API patch_foo : public foo
{
public:
    std::string patch_func1(const std::string& val) const;
    std::string patch_func2(const std::string& val) const;
    static std::string patch_func3(const std::string& val);
};

#endif // __FOO_H__