//////////////////////////////////////////////////////////////////////////
// 杂项功能测试
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_MISC_H__
#define __TEST_MISC_H__

#include "base/ev_types.h"

namespace test_misc {

// 功能：测试ev_common
void test_ev_common(void);

// 功能：测试MD5签名
void test_md5(void);

// 功能：测试SHA签名
void test_sha(void);

// 功能：测试AES加密
void test_aes(void);

// 功能：测试Base64转码
void test_base64(void);

// 功能：测试时间，统计时间格式化函数的执行效率
void test_time(evpp::uint32 repeat_times);

// 功能：测试定时器，统计MD5签名函数的执行效率
void test_time_counter(evpp::uint32 repeat_times);

// 功能：测试定时器，统计随机数生成函数的执行效率
void test_random(evpp::uint32 repeat_times);

// 功能：测试ev_fixed_buffer
void test_fixed_buffer(void);

// 功能：测试ev_stream_buffer
void test_stream_buffer(void);

// 功能：测试ev_file
void test_file(void);

// 功能：测试ev_create_socketpair
void test_socketpair(void);

} // namespace test_misc

#endif // __TEST_MISC_H__