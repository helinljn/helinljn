//////////////////////////////////////////////////////////////////////////
// libevent测试
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_LIBEVENT_H__
#define __TEST_LIBEVENT_H__

#include "libevent-config.h"

// 功能：测试随机
void test_libevent_random(void);

// 功能：测试evbuffer
void test_libevent_evbuffer(void);

// 功能：测试所有接口
void test_libevent_all(void);

#endif // __TEST_LIBEVENT_H__