//////////////////////////////////////////////////////////////////////////
// ev_scoped_ptr
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_SCOPED_PTR_H__
#define __EV_SCOPED_PTR_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"
#include "base/ev_checked_delete.h"

#include <cstddef>  // for NULL
#include <cassert>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_scoped_ptr
//////////////////////////////////////////////////////////////////////////
template<typename T>
class ev_scoped_ptr : public ev_noncopyable
{
public:
	typedef T                element_type;
	typedef ev_scoped_ptr<T> this_type;

public:
	explicit ev_scoped_ptr(T* p = NULL)
		: ptr_(p)
	{
	}

	~ev_scoped_ptr(void)
	{
		ev_checked_deleter<T>()(ptr_);
	}

	T* get(void) const
	{
		return ptr_;
	}

	void reset(T* p = NULL)
	{
		assert(NULL == p || p != ptr_);
		this_type(p).swap(*this);
	}

	void swap(ev_scoped_ptr& r)
	{
		T* temp_ptr = r.ptr_;
		r.ptr_      = ptr_;
		ptr_        = temp_ptr;
	}

	typename ev_reference<T>::type operator *(void) const
	{
		assert(ptr_ != NULL);
		return *ptr_;
	}

	T* operator ->(void) const
	{
		assert(ptr_ != NULL);
		return ptr_;
	}

	bool operator !(void) const
	{
		return (NULL == ptr_);
	}

	operator bool(void) const
	{
		return (ptr_ != NULL);
	}

private:
	T* ptr_;
};

template<typename T>
inline void swap(ev_scoped_ptr<T>& l, ev_scoped_ptr<T>& r)
{
	l.swap(r);
}

} // namespace evpp

#endif // __EV_SCOPED_PTR_H__