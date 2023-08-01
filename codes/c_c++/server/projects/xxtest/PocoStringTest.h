#ifndef __POCO_STRING_TEST_H__
#define __POCO_STRING_TEST_H__

#include "CppUnit/TestCase.h"

class PocoStringTest : public CppUnit::TestCase
{
public:
    PocoStringTest(const std::string& name);

    void setUp(void) override;
    void tearDown(void) override;
    int  countTestCases(void) const override;

public:
    void testTrimLeft(void);
    void testTrimRight(void);
    void testTrim(void);
    void testToUpper(void);
    void testToLower(void);
    void testICompare(void);
    void testTranslate(void);
    void testReplace(void);
    void testStartsWith(void);
    void testSplit(void);
    void testRegularExpression(void);

public:
    static CppUnit::Test* suite(void);
};

#endif // __POCO_STRING_TEST_H__