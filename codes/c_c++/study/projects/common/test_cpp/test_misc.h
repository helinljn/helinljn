#ifndef __TEST_MISC_H__
#define __TEST_MISC_H__

#include "util/common.h"

// 功能：测试各种类型size
void test_sizeof(void);

// 功能：测试数值极限
void test_numeric_limits(void);

// 功能：测试safe_*
void test_safe_time(void);

// 功能：测试memory & hex_string互转
void test_memory_and_hex_convert(void);

// 功能：测试str_println
void test_str_println(void);

// 功能：测试pair & tuple
void test_pair_tuple_tie(void);

// 功能：测试所有接口
void test_misc_all(void);

#endif // __TEST_MISC_H__