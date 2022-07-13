//////////////////////////////////////////////////////////////////////////
// ev_shared_ptr
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_SHARED_PTR_H__
#define __EV_SHARED_PTR_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"
#include "base/ev_checked_delete.h"
#include "base/ev_atomic.h"

#include <cstddef>     // for NULL
#include <cassert>
#include <functional>  // for std::less

namespace evpp {

template<typename T> class ev_shared_ptr;
template<typename T> class ev_weak_ptr;
template<typename T> class ev_enable_shared_from_this;

namespace details {

class ev_shared_count;
class ev_weak_count;

struct ev_static_cast_tag  {};
struct ev_const_cast_tag   {};
struct ev_dynamic_cast_tag {};

//////////////////////////////////////////////////////////////////////////
// ev_counted_base
//////////////////////////////////////////////////////////////////////////
class ev_counted_base : public ev_noncopyable
{
public:
	ev_counted_base(void)
		: use_count_(1)
		, weak_count_(1)
	{
	}

	virtual ~ev_counted_base(void)
	{
	}

	virtual void dispose(void) = 0;

	void add_ref_copy(void)
	{
		ev_atomic_fetch_and_increment(&use_count_);
	}

	bool add_ref_lock(void)
	{
		return ev_atomic_test_nonzero_and_increment(&use_count_);
	}

	void release(void)
	{
		if (1 == ev_atomic_fetch_and_decrement(&use_count_))
		{
			dispose();
			weak_release();
		}
	}

	void weak_add_ref(void)
	{
		ev_atomic_fetch_and_increment(&weak_count_);
	}

	void weak_release(void)
	{
		if (1 == ev_atomic_fetch_and_decrement(&weak_count_))
		{
			delete this;
		}
	}

	int32 use_count(void) const
	{
		return const_cast<const ev_atomic_int32&>(use_count_);
	}

private:
	ev_atomic_int32 use_count_;
	ev_atomic_int32 weak_count_;
};

//////////////////////////////////////////////////////////////////////////
// ev_counted_base_impl
//////////////////////////////////////////////////////////////////////////
template<typename T, typename D>
class ev_counted_base_impl : public ev_counted_base
{
public:
	ev_counted_base_impl(T* p, D d)
		: ev_counted_base()
		, ptr_(p)
		, deleter_(d)
	{
	}

	virtual void dispose(void)
	{
		deleter_(ptr_);
	}

private:
	T* ptr_;
	D  deleter_;
};

//////////////////////////////////////////////////////////////////////////
// ev_shared_count
//////////////////////////////////////////////////////////////////////////
class ev_shared_count
{
public:
	ev_shared_count(void)
		: pi_(NULL)
	{
	}

	template<typename T>
	ev_shared_count(T* p)
		: pi_(NULL)
	{
		try
		{
			pi_ = new ev_counted_base_impl<T, ev_checked_deleter<T> >(p, ev_checked_deleter<T>());
		}
		catch (...)
		{
			ev_checked_deleter<T>()(p);
			throw; // throw exception again
		}
	}

	template<typename T, typename D>
	ev_shared_count(T* p, D d)
		: pi_(NULL)
	{
		try
		{
			pi_ = new ev_counted_base_impl<T, D>(p, d);
		}
		catch (...)
		{
			d(p);
			throw; // throw exception again
		}
	}

	ev_shared_count(const ev_shared_count& r)
		: pi_(r.pi_)
	{
		if (pi_)
		{
			pi_->add_ref_copy();
		}
	}

	explicit ev_shared_count(const ev_weak_count& r);

	~ev_shared_count(void)
	{
		if (pi_)
		{
			pi_->release();
		}
	}

	bool empty(void) const
	{
		return (NULL == pi_);
	}

	bool unique(void) const
	{
		return (1 == use_count());
	}

	int32 use_count(void) const
	{
		return (pi_ ? pi_->use_count() : 0);
	}

	void swap(ev_shared_count& r)
	{
		ev_counted_base* temp_ptr = r.pi_;
		r.pi_                     = pi_;
		pi_                       = temp_ptr;
	}

	ev_shared_count& operator =(const ev_shared_count& r)
	{
		ev_counted_base* temp_ptr = r.pi_;
		if (temp_ptr != pi_)
		{
			if (temp_ptr) temp_ptr->add_ref_copy();
			if (pi_)      pi_->release();
			pi_ = temp_ptr;
		}

		return *this;
	}

	friend inline bool operator ==(const ev_shared_count& l, const ev_shared_count& r)
	{
		return (l.pi_ == r.pi_);
	}

	friend inline bool operator <(const ev_shared_count& l, const ev_shared_count& r)
	{
		return std::less<ev_counted_base*>()(l.pi_, r.pi_);
	}

private:
	friend class ev_weak_count;

private:
	ev_counted_base* pi_;
};

//////////////////////////////////////////////////////////////////////////
// ev_weak_count
//////////////////////////////////////////////////////////////////////////
class ev_weak_count
{
public:
	ev_weak_count(void)
		: pi_(NULL)
	{
	}

	ev_weak_count(const ev_weak_count& r)
		: pi_(r.pi_)
	{
		if (pi_)
		{
			pi_->weak_add_ref();
		}
	}

	ev_weak_count(const ev_shared_count& r)
		: pi_(r.pi_)
	{
		if (pi_)
		{
			pi_->weak_add_ref();
		}
	}

	~ev_weak_count(void)
	{
		if (pi_)
		{
			pi_->weak_release();
		}
	}

	bool empty(void) const
	{
		return (NULL == pi_);
	}

	int32 use_count(void) const
	{
		return (pi_ ? pi_->use_count() : 0);
	}

	void swap(ev_weak_count& r)
	{
		ev_counted_base* temp_ptr = r.pi_;
		r.pi_                     = pi_;
		pi_                       = temp_ptr;
	}

	ev_weak_count& operator =(const ev_shared_count& r)
	{
		ev_counted_base* temp_ptr = r.pi_;
		if (temp_ptr != pi_)
		{
			if (temp_ptr) temp_ptr->weak_add_ref();
			if (pi_)      pi_->weak_release();
			pi_ = temp_ptr;
		}

		return *this;
	}

	ev_weak_count& operator =(const ev_weak_count& r)
	{
		ev_counted_base* temp_ptr = r.pi_;
		if (temp_ptr != pi_)
		{
			if (temp_ptr) temp_ptr->weak_add_ref();
			if (pi_)      pi_->weak_release();
			pi_ = temp_ptr;
		}

		return *this;
	}

	friend inline bool operator ==(const ev_weak_count& l, const ev_weak_count& r)
	{
		return (l.pi_ == r.pi_);
	}

	friend inline bool operator <(const ev_weak_count& l, const ev_weak_count& r)
	{
		return std::less<ev_counted_base*>()(l.pi_, r.pi_);
	}

private:
	friend class ev_shared_count;

private:
	ev_counted_base* pi_;
};

inline ev_shared_count::ev_shared_count(const ev_weak_count& r)
	: pi_(r.pi_)
{
	if (pi_ && !pi_->add_ref_lock())
	{
		pi_ = NULL;
	}
}

} // namespace details

// ev_enable_shared_from_this support
template<typename T1, typename T2>
void ev_sp_enable_shared_from_this(const details::ev_shared_count&, const ev_enable_shared_from_this<T1>*, const T2*);

inline void ev_sp_enable_shared_from_this(const details::ev_shared_count&, ...)
{
}

//////////////////////////////////////////////////////////////////////////
// ev_shared_ptr
//////////////////////////////////////////////////////////////////////////
template<typename T>
class ev_shared_ptr
{
public:
	typedef T                element_type;
	typedef ev_shared_ptr<T> this_type;

public:
	ev_shared_ptr(void)
		: ptr_(NULL)
		, count_()
	{
	}

	template<typename Y>
	explicit ev_shared_ptr(Y* p)
		: ptr_(p)
		, count_(p)
	{
		ev_sp_enable_shared_from_this(count_, p, p);
	}

	template<typename Y, typename D>
	explicit ev_shared_ptr(Y* p, D d)
		: ptr_(p)
		, count_(p, d)
	{
		ev_sp_enable_shared_from_this(count_, p, p);
	}

	template<typename Y>
	ev_shared_ptr(const ev_shared_ptr<Y>& r)
		: ptr_(r.ptr_)
		, count_(r.count_)
	{
	}

	template<typename Y>
	explicit ev_shared_ptr(const ev_weak_ptr<Y>& r)
		: ptr_(NULL)
		, count_(r.count_)
	{
		if (!count_.empty())
		{
			ptr_ = r.ptr_;
		}
	}

	template<typename Y>
	ev_shared_ptr(const ev_shared_ptr<Y>& r, details::ev_static_cast_tag)
		: ptr_(static_cast<element_type*>(r.ptr_))
		, count_(r.count_)
	{
	}

	template<typename Y>
	ev_shared_ptr(const ev_shared_ptr<Y>& r, details::ev_const_cast_tag)
		: ptr_(const_cast<element_type*>(r.ptr_))
		, count_(r.count_)
	{
	}

	template<typename Y>
	ev_shared_ptr(const ev_shared_ptr<Y>& r, details::ev_dynamic_cast_tag)
		: ptr_(dynamic_cast<element_type*>(r.ptr_))
		, count_(r.count_)
	{
		if (NULL == ptr_)
		{
			count_ = details::ev_shared_count();
		}
	}

	bool unique(void) const
	{
		return count_.unique();
	}

	int32 use_count(void) const
	{
		return count_.use_count();
	}

	T* get(void) const
	{
		return ptr_;
	}

	void reset(void)
	{
		this_type().swap(*this);
	}

	template<typename Y>
	void reset(Y* p)
	{
		assert(NULL == p || p != ptr_);
		this_type(p).swap(*this);
	}

	template<typename Y, typename D>
	void reset(Y* p, D d)
	{
		assert(NULL == p || p != ptr_);
		this_type(p, d).swap(*this);
	}

	void swap(ev_shared_ptr& r)
	{
		T* temp_ptr = r.ptr_;
		r.ptr_      = ptr_;
		ptr_        = temp_ptr;

		count_.swap(r.count_);
	}

	template<typename Y>
	ev_shared_ptr& operator =(const ev_shared_ptr<Y>& r)
	{
		ptr_   = r.ptr_;
		count_ = r.count_;
		return *this;
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
	template<typename Y>
	bool less(const ev_shared_ptr<Y>& r) const
	{
		return count_ < r.count_;
	}

	template<typename Y>
	friend inline bool operator ==(const ev_shared_ptr& l, const ev_shared_ptr<Y>& r)
	{
		return (l.get() == r.get());
	}

	template<typename Y>
	friend inline bool operator !=(const ev_shared_ptr& l, const ev_shared_ptr<Y>& r)
	{
		return (l.get() != r.get());
	}

	template<typename Y>
	friend inline bool operator <(const ev_shared_ptr& l, const ev_shared_ptr<Y>& r)
	{
		return l.less(r);
	}

private:
	template<typename Y> friend class ev_shared_ptr;
	template<typename Y> friend class ev_weak_ptr;

private:
	T*                       ptr_;
	details::ev_shared_count count_;
};

template<typename T>
inline void swap(ev_shared_ptr<T>& l, ev_shared_ptr<T>& r)
{
	return l.swap(r);
}

template<typename T, typename U>
inline ev_shared_ptr<T> ev_static_pointer_cast(const ev_shared_ptr<U>& r)
{
	return ev_shared_ptr<T>(r, details::ev_static_cast_tag());
}

template<typename T, typename U>
inline ev_shared_ptr<T> ev_const_pointer_cast(const ev_shared_ptr<U>& r)
{
	return ev_shared_ptr<T>(r, details::ev_const_cast_tag());
}

template<typename T, typename U>
inline ev_shared_ptr<T> ev_dynamic_pointer_cast(const ev_shared_ptr<U>& r)
{
	return ev_shared_ptr<T>(r, details::ev_dynamic_cast_tag());
}

//////////////////////////////////////////////////////////////////////////
// ev_weak_ptr
//////////////////////////////////////////////////////////////////////////
template<typename T>
class ev_weak_ptr
{
public:
	typedef T              element_type;
	typedef ev_weak_ptr<T> this_type;

public:
	ev_weak_ptr(void)
		: ptr_(NULL)
		, count_()
	{
	}

	template<typename Y>
	ev_weak_ptr(const ev_weak_ptr<Y>& r)
		: ptr_(NULL)
		, count_(r.count_)
	{
		ptr_ = r.lock().get();
	}

	template<typename Y>
	ev_weak_ptr(const ev_shared_ptr<Y>& r)
		: ptr_(r.ptr_)
		, count_(r.count_)
	{
	}

	ev_shared_ptr<T> lock(void) const
	{
		return (expired() ? ev_shared_ptr<element_type>() : ev_shared_ptr<element_type>(*this));
	}

	bool expired(void) const
	{
		return (0 == count_.use_count());
	}

	int32 use_count(void) const
	{
		return count_.use_count();
	}

	void reset(void)
	{
		this_type().swap(*this);
	}

	void swap(ev_weak_ptr& r)
	{
		T* temp_ptr = r.ptr_;
		r.ptr_      = ptr_;
		ptr_        = temp_ptr;

		count_.swap(r.count_);
	}

	template<typename Y>
	ev_weak_ptr& operator =(const ev_weak_ptr<Y>& r)
	{
		ptr_   = r.lock().get();
		count_ = r.count_;
		return *this;
	}

	template<typename Y>
	ev_weak_ptr& operator =(const ev_shared_ptr<Y>& r)
	{
		ptr_   = r.ptr_;
		count_ = r.count_;
		return *this;
	}

private:
	template<typename Y>
	bool less(const ev_weak_ptr<Y>& r) const
	{
		return count_ < r.count_;
	}

	template<typename Y>
	friend inline bool operator <(const ev_weak_ptr& l, const ev_weak_ptr<Y>& r)
	{
		return l.less(r);
	}

private:
	template<typename Y> friend class ev_shared_ptr;
	template<typename Y> friend class ev_weak_ptr;
	friend class ev_enable_shared_from_this<T>;

private:
	T*                     ptr_;
	details::ev_weak_count count_;
};

template<typename T>
inline void swap(ev_weak_ptr<T>& l, ev_weak_ptr<T>& r)
{
	l.swap(r);
}

//////////////////////////////////////////////////////////////////////////
// ev_enable_shared_from_this
//////////////////////////////////////////////////////////////////////////
template<typename T>
class ev_enable_shared_from_this
{
public:
	ev_shared_ptr<T> shared_from_this(void)
	{
		return ev_shared_ptr<T>(this->weak_this_);
	}

	ev_shared_ptr<const T> shared_from_this(void) const
	{
		return ev_shared_ptr<const T>(this->weak_this_);
	}

protected:
	ev_enable_shared_from_this(void)
	{
	}

	ev_enable_shared_from_this(const ev_enable_shared_from_this&)
	{
	}

	ev_enable_shared_from_this& operator =(const ev_enable_shared_from_this&)
	{
		return *this;
	}

	~ev_enable_shared_from_this(void)
	{
	}

private:
	template<typename X>
	friend inline void ev_sp_enable_shared_from_this(const details::ev_shared_count& pn, const ev_enable_shared_from_this<T>* pe, const X* px)
	{
		if (pe != NULL)
		{
			pe->ev_internal_accept_owner(pn, px);
		}
	}

	template<typename X>
	void ev_internal_accept_owner(const details::ev_shared_count& pn, const X* px) const
	{
		weak_this_.ptr_   = const_cast<X*>(px);
		weak_this_.count_ = pn;
	}

private:
	mutable ev_weak_ptr<T> weak_this_;
};

} // namespace evpp

#endif // __EV_SHARED_PTR_H__