#ifndef __POCO_NUMERIC_STRING_TEST_H__
#define __POCO_NUMERIC_STRING_TEST_H__

#include "CppUnit/TestCase.h"

class PocoNumericStringTest : public CppUnit::TestCase
{
public:
    PocoNumericStringTest(const std::string& name);

    void setUp(void) override;
    void tearDown(void) override;
    int  countTestCases(void) const override;

public:
    void testPocoStrToNumeric(void);
    void testStdStrToNumeric(void);
    void testPocoNumericToStr(void);
    void testStdNumericToStr(void);
    void testNumberFormatter(void);
    void testNumberParser(void);

public:
    static CppUnit::Test* suite(void);
};

#endif // __POCO_NUMERIC_STRING_TEST_H__