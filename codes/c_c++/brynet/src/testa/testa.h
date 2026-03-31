#ifndef __TESTA_H__
#define __TESTA_H__

#include "core/core_port.h"
#include <string>

#if defined(CORE_PLATFORM_WINDOWS)
    #ifdef TESTA_EXPORTS
        #define TESTA_API __declspec(dllexport)
    #else
        #define TESTA_API __declspec(dllimport)
    #endif // TESTA_EXPORTS
#elif defined(CORE_PLATFORM_LINUX)
    #ifdef TESTA_EXPORTS
        #define TESTA_API __attribute__((visibility("default")))
    #else
        #define TESTA_API
    #endif // TESTA_EXPORTS
#endif // defined(CORE_PLATFORM_WINDOWS)

////////////////////////////////////////////////////////////////
// 测试hook普通函数
////////////////////////////////////////////////////////////////
TESTA_API std::string testa_func(int32_t val);

////////////////////////////////////////////////////////////////
// 测试hook成员函数
////////////////////////////////////////////////////////////////
class TESTA_API testa_base
{
public:
    virtual std::string func1(const std::string& val) const;
};

class TESTA_API testa : public testa_base
{
public:
    std::string        func1(const std::string& val) const override;
    std::string        func2(const std::string& val) const;
    static std::string func3(const std::string& val);
};

#endif // __TESTA_H__