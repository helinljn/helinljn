#ifndef __FOOEX_H__
#define __FOOEX_H__

#include "util/types.h"

////////////////////////////////////////////////////////////////
// 测试hook普通函数
////////////////////////////////////////////////////////////////
std::string test_fooex(int32_t val);
std::string patch_test_fooex(int32_t val);

////////////////////////////////////////////////////////////////
// 测试hook成员函数
////////////////////////////////////////////////////////////////
class fooex_base
{
public:
    virtual std::string func1(const std::string& val) const;
};

class fooex : public fooex_base
{
public:
    std::string func1(const std::string& val) const override;
    std::string func2(const std::string& val) const;
    static std::string func3(const std::string& val);
};

class patch_fooex : public fooex
{
public:
    std::string patch_func1(const std::string& val) const;
    std::string patch_func2(const std::string& val) const;
    static std::string patch_func3(const std::string& val);
};

#endif // __FOOEX_H__