//////////////////////////////////////////////////////////////////////////
// protobuf����
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_PROTOCOL_H__
#define __TEST_PROTOCOL_H__

#include "util/common.h"

// ���ܣ�����ö��
void test_protocol_enum(void);

// ���ܣ��������л�
void test_protocol_serialization(void);

// ���ܣ��������л��б�
void test_protocol_serialization_list(void);

// ���ܣ���������
void test_protocol_performance(void);

// ���ܣ��������нӿ�
void test_protocol_all(void);

#endif // __TEST_PROTOCOL_H__