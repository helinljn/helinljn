#ifndef __POCO_STRING_H__
#define __POCO_STRING_H__

// 功能：测试去掉字符串左边空格
void test_trim_left(void);

// 功能：测试去掉字符串右边空格
void test_trim_right(void);

// 功能：测试去掉字符串两边空格
void test_trim(void);

// 功能：测试转换大写
void test_to_upper(void);

// 功能：测试转换小写
void test_to_lower(void);

// 功能：测试忽略大小写比较
void test_icompare(void);

// 功能：测试转换
void test_translate(void);

// 功能：测试替换
void test_replace(void);

// 功能：测试starts & ends
void test_starts_ends(void);

// 功能：拆分字符串
void test_split(void);

// 功能：测试正则表达式
void test_regular_expression(void);

// 功能：测试所有接口
void test_poco_string_all(void);

#endif // __POCO_STRING_H__