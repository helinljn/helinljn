#include "XXTestSuite.h"
#include "EmptyTest.h"
#include "FmtTest.h"
#include "ProtobufTest.h"
#include "PocoCoreTest.h"
#include "PocoStringTest.h"
#include "PocoNumericStringTest.h"

CppUnit::Test* XXTestSuite::suite(void)
{
    CppUnit::TestSuite* testSuite = new CppUnit::TestSuite("XXTestSuite");

    testSuite->addTest(EmptyTest::suite());
    testSuite->addTest(FmtTest::suite());
    testSuite->addTest(ProtobufTest::suite());
    testSuite->addTest(PocoCoreTest::suite());
    testSuite->addTest(PocoStringTest::suite());
    testSuite->addTest(PocoNumericStringTest::suite());

    return testSuite;
}