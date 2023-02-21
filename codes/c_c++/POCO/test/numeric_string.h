#ifndef __NUMERIC_STRING_H__
#define __NUMERIC_STRING_H__

// 功能：测试POCO字符串转数值接口
void test_poco_str_to_numeric(void);

// 功能：测试std字符串转数值接口
void test_std_str_to_numeric(void);

// 功能：测试POCO数值转字符串接口
void test_poco_numeric_to_str(void);

// 功能：测试std数值转字符串接口
void test_std_numeric_to_str(void);

// 功能：测试字符串转数值性能
void test_benchmark_str_to_numeric(void);

// 功能：测试数值转字符串性能
void test_benchmark_numeric_to_str(void);

// 功能：测试所有接口
void test_numeric_string_all(void);

#endif // __NUMERIC_STRING_H__