#ifndef __CORE_H__
#define __CORE_H__

// 功能：测试平台宏
void test_platform(void);

// 功能：测试内置类型固定长度
void test_fixed_length(void);

// 功能：测试环境变量
void test_environment(void);

// 功能：测试原子计数器
void test_atomic_counter(void);

// 功能：测试ASCII
void test_ascii(void);

// 功能：测试nan & inf
void test_nan_inf(void);

// 功能：测试NDC
void test_ndc(void);

// 功能：测试所有接口
void test_core_all(void);

#endif // __CORE_H__