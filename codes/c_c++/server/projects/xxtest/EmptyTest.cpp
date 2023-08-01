#include "EmptyTest.h"
#include "CppUnit/TestSuite.h"
#include "CppUnit/TestCaller.h"

EmptyTest::EmptyTest(const std::string& name)
    : CppUnit::TestCase(name)
{
}

void EmptyTest::setUp(void)
{
    // body
}

void EmptyTest::tearDown(void)
{
    // body
}

int EmptyTest::countTestCases(void) const
{
    return 1;
}

void EmptyTest::testNull(void)
{
    // body
}

CppUnit::Test* EmptyTest::suite(void)
{
    CppUnit::TestSuite* testSuite = new CppUnit::TestSuite("EmptyTest");

    CppUnit_addTest(testSuite, EmptyTest, testNull);

    return testSuite;
}