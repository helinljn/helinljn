#ifndef __TEST_MISC_H__
#define __TEST_MISC_H__

#include "util/common.h"

// ���ܣ����Ը�������size
void test_sizeof(void);

// ���ܣ�������ֵ����
void test_numeric_limits(void);

// ���ܣ�����safe_*
void test_safe_time(void);

// ���ܣ�����memory & hex_string��ת
void test_memory_and_hex_convert(void);

// ���ܣ�����str_println
void test_str_println(void);

// ���ܣ�����pair & tuple
void test_pair_tuple_tie(void);

// ���ܣ��������нӿ�
void test_misc_all(void);

#endif // __TEST_MISC_H__