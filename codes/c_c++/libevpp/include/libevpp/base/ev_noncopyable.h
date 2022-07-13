﻿//////////////////////////////////////////////////////////////////////////
// 禁止拷贝构造和赋值
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_NONCOPYABLE_H__
#define __EV_NONCOPYABLE_H__

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_noncopyable
//////////////////////////////////////////////////////////////////////////
class ev_noncopyable
{
protected:
	ev_noncopyable(void)  {}
	~ev_noncopyable(void) {}

private:
	ev_noncopyable(const ev_noncopyable&);
	ev_noncopyable& operator =(const ev_noncopyable&);
};

} // namespace evpp

#endif // __EV_NONCOPYABLE_H__