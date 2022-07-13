//////////////////////////////////////////////////////////////////////////
// 释放检测，避免释放不完整的类型
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_CHECKED_DELETE_H__
#define __EV_CHECKED_DELETE_H__

namespace evpp {

template<typename T>
inline void ev_checked_delete(T* p)
{
	typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
	(void)sizeof(type_must_be_complete);
	delete p;
}

template<typename T>
inline void ev_checked_array_delete(T* p)
{
	typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
	(void)sizeof(type_must_be_complete);
	delete[] p;
}

template<typename T>
class ev_checked_deleter
{
public:
	typedef void result_type;
	typedef T*   argument_type;

public:
	void operator ()(T* p) const
	{
		typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
		(void)sizeof(type_must_be_complete);
		delete p;
	}
};

template<typename T>
class ev_checked_array_deleter
{
public:
	typedef void result_type;
	typedef T*   argument_type;

public:
	void operator ()(T* p) const
	{
		typedef char type_must_be_complete[sizeof(T) ? 1 : -1];
		(void)sizeof(type_must_be_complete);
		delete[] p;
	}
};

} // namespace evpp

#endif // __EV_CHECKED_DELETE_H__