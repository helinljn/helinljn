#ifndef __EMPTY_TEST_H__
#define __EMPTY_TEST_H__

#include "CppUnit/TestCase.h"

class EmptyTest : public CppUnit::TestCase
{
public:
    EmptyTest(const std::string& name);

    void setUp(void) override;
    void tearDown(void) override;
    int  countTestCases(void) const override;

public:
    void testNull(void);

public:
    static CppUnit::Test* suite(void);
};

#endif // __EMPTY_TEST_H__