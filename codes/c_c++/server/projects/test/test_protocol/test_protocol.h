//////////////////////////////////////////////////////////////////////////
// protobuf测试
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_PROTOCOL_H__
#define __TEST_PROTOCOL_H__

#include "util/common.h"

// 功能：测试枚举
void test_protocol_enum(void);

// 功能：测试序列化
void test_protocol_serialization(void);

// 功能：测试序列化列表
void test_protocol_serialization_list(void);

// 功能：测试性能
void test_protocol_performance(void);

// 功能：测试所有接口
void test_protocol_all(void);

#endif // __TEST_PROTOCOL_H__