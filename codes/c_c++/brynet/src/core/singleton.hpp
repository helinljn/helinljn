#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

#include <type_traits>

namespace core {

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

} // namespace core

#ifndef CORE_SINGLETON_HELPER
    #define CORE_SINGLETON_HELPER \
        friend class core::singleton<internal_subclass_type>
#endif // CORE_SINGLETON_HELPER

////////////////////////////////////////////////////////////////
// 使用示例
////////////////////////////////////////////////////////////////
// class example : public core::singleton<example>
// {
// public:
//     int32 get_int(void)   const {return _int_data;}
//     float get_float(void) const {return _float_data;}
//
// private:
//     example(void)
//         : _int_data(0)
//         , _float_data(0.f)
//     {
//     }
//
// private:
//     CORE_SINGLETON_HELPER;
//
// private:
//     int32 _int_data;
//     float _float_data;
// };
//
// example& ex = example::instance();
// ex.get_int();
// ex.get_float();

#endif // __SINGLETON_HPP__