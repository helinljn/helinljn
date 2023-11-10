#ifndef __BAR_H__
#define __BAR_H__

#include "util/poco.h"
#include "Poco/Foundation.h"

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
BAR_API std::string hotfix_test_bar(int32_t val);

////////////////////////////////////////////////////////////////
// 测试hook成员函数
////////////////////////////////////////////////////////////////
class BAR_API bar
{
public:
    std::string func1(const std::string& val) const;
    static std::string func2(const std::string& val);
};

class BAR_API hotfix_bar
{
public:
    std::string hotfix_func1(const std::string& val) const;
    static std::string hotfix_func2(const std::string& val);
};

#endif // __BAR_H__