#ifndef __POCO_CORE_TEST_H__
#define __POCO_CORE_TEST_H__

#include "CppUnit/TestCase.h"

class PocoCoreTest : public CppUnit::TestCase
{
public:
    PocoCoreTest(const std::string& name);

    void setUp(void) override;
    void tearDown(void) override;
    int  countTestCases(void) const override;

public:
    void testFixedLength(void);
    void testEnvironment(void);
    void testAtomicCounter(void);
    void testASCII(void);
    void testNanInf(void);
    void testNDC(void);
    void testDynamicFactory(void);
    void testMemoryPooly(void);
    void testFlipBytes(void);
    void testBigEndian(void);
    void testLittleEndian(void);
    void testNetwork(void);

public:
    static CppUnit::Test* suite(void);
};

#endif // __POCO_CORE_TEST_H__