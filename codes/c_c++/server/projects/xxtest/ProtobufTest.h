#ifndef __PROTOBUF_TEST_H__
#define __PROTOBUF_TEST_H__

#include "CppUnit/TestCase.h"

class ProtobufTest : public CppUnit::TestCase
{
public:
    ProtobufTest(const std::string& name);

    void setUp(void) override;
    void tearDown(void) override;
    int  countTestCases(void) const override;

public:
    void testEnumName(void);
    void testSerialization(void);
    void testSerializationList(void);

public:
    static CppUnit::Test* suite(void);
};

#endif // __PROTOBUF_TEST_H__