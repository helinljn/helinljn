//////////////////////////////////////////////////////////////////////////
// 线程相关测试
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_THREAD_H__
#define __TEST_THREAD_H__

namespace test_thread {

// 功能：测试ev_thread_local变量
void test_thread_local(void);

// 功能：测试ev_countdown_latch
void test_countdown_latch(void);

// 功能：测试ev_lock
void test_lock(void);

// 功能：测试ev_atomic_lock
void test_atomic_lock(void);

// 功能：测试ev_once
void test_once(void);

} // namespace test_thread

#endif // __TEST_THREAD_H__