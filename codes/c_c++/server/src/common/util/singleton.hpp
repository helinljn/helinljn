#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

#include "util/types.h"

namespace common {

////////////////////////////////////////////////////////////////
// 线程安全的单例模式
////////////////////////////////////////////////////////////////
template <typename T>
class singleton
{
public:
    static_assert(std::is_class_v<T>, "Invalid subclass type!");

public:
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

    singleton(singleton&&) = delete;
    singleton& operator=(singleton&&) = delete;

    static T& instance(void)
    {
        static T ins;
        return ins;
    }

protected:
    singleton(void) noexcept = default;
    ~singleton(void) noexcept = default;

protected:
    using internal_subclass_type = T;
};

} // namespace common

#ifndef COMMON_SINGLETON_HELPER
    #define COMMON_SINGLETON_HELPER \
        friend class common::singleton<internal_subclass_type>
#endif // COMMON_SINGLETON_HELPER

////////////////////////////////////////////////////////////////
// 使用示例
////////////////////////////////////////////////////////////////
// class example : public common::singleton<example>
// {
// public:
//     int32 get_int(void)   const {return int_data_;}
//     float get_float(void) const {return float_data_;}
//
// private:
//     example(void)
//         : int_data_(0)
//         , float_data_(0.f)
//     {
//     }
//
// private:
//     COMMON_SINGLETON_HELPER;
//
// private:
//     int32 int_data_;
//     float float_data_;
// };
//
// example& ex = example::instance();
// ex.get_int();
// ex.get_float();

#endif // __SINGLETON_HPP__