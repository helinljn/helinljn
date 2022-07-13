//////////////////////////////////////////////////////////////////////////
// 原子操作
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_ATOMIC_H__
#define __EV_ATOMIC_H__

#include "base/ev_types.h"

namespace evpp {

typedef volatile int32 ev_atomic_int32;
typedef volatile int64 ev_atomic_int64;

/**
 * @brief 自增，相当于++val
 * @param val 被增加的数
 * @return 返回val修改后的值
 */
int32 ev_atomic_increment_and_fetch(ev_atomic_int32* val);
int64 ev_atomic_increment_and_fetch(ev_atomic_int64* val);

/**
 * @brief 自增，相当于val++
 * @param val 被增加的数
 * @return 返回val修改前的值
 */
int32 ev_atomic_fetch_and_increment(ev_atomic_int32* val);
int64 ev_atomic_fetch_and_increment(ev_atomic_int64* val);

/**
 * @brief 自减，相当于--val
 * @param val 被减少的数
 * @return 返回val修改后的值
 */
int32 ev_atomic_decrement_and_fetch(ev_atomic_int32* val);
int64 ev_atomic_decrement_and_fetch(ev_atomic_int64* val);

/**
 * @brief 自减，相当于val--
 * @param val 被减少的数
 * @return 返回val修改前的值
 */
int32 ev_atomic_fetch_and_decrement(ev_atomic_int32* val);
int64 ev_atomic_fetch_and_decrement(ev_atomic_int64* val);

/**
 * @brief 先取旧值然后累加，相当于target + val
 * @param target 被增加的数
 * @param val    增加的值，如果为负数就是减
 * @return 返回target修改前的值
 */
int32 ev_atomic_fetch_and_add(ev_atomic_int32* target, int32 val);
int64 ev_atomic_fetch_and_add(ev_atomic_int64* target, int64 val);

/**
 * @brief CAS操作
 *        如果*target == old_value，就将new_value写入*target，返回old_value
 *        如果*target != old_value，不进行任何操作，然后返回*target
 * @param target    被操作的数
 * @param old_value 用以比较的旧值
 * @param new_value 设置的新值
 * @return
 */
int32 ev_atomic_compare_and_swap(ev_atomic_int32* target, int32 old_value, int32 new_value);
int64 ev_atomic_compare_and_swap(ev_atomic_int64* target, int64 old_value, int64 new_value);

/**
 * @brief CAS操作
 *        如果*target == 0，直接返回false
 *        如果*target != 0，将*target加1，然后返回true
 * @param target 被操作的数
 * @return 操作成功返回true，失败返回false
 */
bool ev_atomic_test_nonzero_and_increment(ev_atomic_int32* target);
bool ev_atomic_test_nonzero_and_increment(ev_atomic_int64* target);

/**
 * @brief CAS操作
 *        如果*target == 0，将*target加1，然后返回true
 *        如果*target != 0，直接返回false
 * @param target 被操作的数
 * @return 操作成功返回true，失败返回false
 */
bool ev_atomic_test_zero_and_increment(ev_atomic_int32* target);
bool ev_atomic_test_zero_and_increment(ev_atomic_int64* target);

/**
 * @brief 获取当前值
 * @param
 * @return
 */
inline int32 ev_atomic_get_value(ev_atomic_int32* val) {return ev_atomic_compare_and_swap(val, 0, 0);}
inline int64 ev_atomic_get_value(ev_atomic_int64* val) {return ev_atomic_compare_and_swap(val, 0, 0);}

} // namespace evpp

#endif // __EV_ATOMIC_H__