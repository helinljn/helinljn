#pragma once

#ifndef TESTB_H
#define TESTB_H

#include <string>

////////////////////////////////////////////////////////////////
// 测试hook普通函数
////////////////////////////////////////////////////////////////
std::string testb_func(int32_t val);

////////////////////////////////////////////////////////////////
// 测试hook成员函数
////////////////////////////////////////////////////////////////
class testb_base
{
public:
    virtual std::string func1(const std::string& val) const;
};

class testb : public testb_base
{
public:
    std::string        func1(const std::string& val) const override;
    std::string        func2(const std::string& val) const;
    static std::string func3(const std::string& val);
};

#endif // TESTB_H