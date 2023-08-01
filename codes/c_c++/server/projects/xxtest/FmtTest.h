#ifndef __FMT_TEST_H__
#define __FMT_TEST_H__

#include "CppUnit/TestCase.h"

class FmtTest : public CppUnit::TestCase
{
public:
    FmtTest(const std::string& name);

    void setUp(void) override;
    void tearDown(void) override;
    int  countTestCases(void) const override;

public:
    void testArgs(void);

public:
    static CppUnit::Test* suite(void);
};

#endif // __FMT_TEST_H__