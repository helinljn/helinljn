//////////////////////////////////////////////////////////////////////////
// 原子操作测试
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_ATOMIC_H__
#define __TEST_ATOMIC_H__

namespace test_atomic {

// 功能：测试++val
void test_atomic_increment_and_fetch(void);

// 功能：测试--val
void test_atomic_decrement_and_fetch(void);

// 功能：测试val++
void test_atomic_fetch_and_increment(void);

// 功能：测试val--
void test_atomic_fetch_and_decrement(void);

// 功能：测试val + n
void test_atomic_fetch_and_add(void);

// 功能：测试使用原子锁实现生产者与消费者
void test_producer_and_consumer(void);

} // namespace test_atomic

#endif // __TEST_ATOMIC_H__