//////////////////////////////////////////////////////////////////////////
// ev_shared_ptr测试
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_SHARED_PTR_H__
#define __TEST_SHARED_PTR_H__

namespace test_shared_ptr {

// 功能：测试ev_shared_ptr
void test_ev_shared_ptr(void);

// 功能：测试ev_weak_ptr
void test_ev_weak_ptr(void);

// 功能：测试多线程下共同访问ev_shared_ptr
void test_multi_thread_shared_ptr(void);

} // namespace test_shared_ptr

#endif // __TEST_SHARED_PTR_H__