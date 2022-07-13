//////////////////////////////////////////////////////////////////////////
// 线程安全的单例模式
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_SINGLETON_H__
#define __EV_SINGLETON_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"
#include "base/ev_scoped_ptr.h"
#include "base/ev_checked_delete.h"

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_singleton
//////////////////////////////////////////////////////////////////////////
template<typename T>
class ev_singleton : public ev_noncopyable
{
public:
	/**
	 * @brief 获取实例
	 * @param
	 * @return
	 */
	static T& get_instance(void)
	{
		return *instance_ptr_;
	}

protected:
	ev_singleton(void)  {}
	~ev_singleton(void) {}

private:
	static ev_scoped_ptr<T> instance_ptr_;
};

template<typename T> ev_scoped_ptr<T> ev_singleton<T>::instance_ptr_(new T);

} // namespace evpp

#define EV_DECLARE_SINGLETON_CLASS(subclass_type)        \
    friend class evpp::ev_singleton<subclass_type>;      \
    friend class evpp::ev_checked_deleter<subclass_type>

//////////////////////////////////////////////////////////////////////////
// 使用示例
//////////////////////////////////////////////////////////////////////////
// class example : public evpp::ev_singleton<example>
// {
// public:
//     evpp::int32 get_int(void)   const {return int_data_;}
//     float       get_float(void) const {return float_data_;}
//
// private:
//     example(void)
//         : int_data_(0)
//         , float_data_(0.f)
//     {
//     }
//
// private:
//     EV_DECLARE_SINGLETON_CLASS(example);
//
// private:
//     evpp::int32 int_data_;
//     float       float_data_;
// };
//
// example& ex = example::get_instance();
// ex.get_int();
// ex.get_float();

#endif // __EV_SINGLETON_H__