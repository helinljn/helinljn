#include "PocoCoreTest.h"
#include "CppUnit/TestSuite.h"
#include "CppUnit/TestCaller.h"

#include "Poco/Environment.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Ascii.h"
#include "Poco/FPEnvironment.h"
#include "Poco/NestedDiagnosticContext.h"
#include "Poco/DynamicFactory.h"
#include "Poco/MemoryPool.h"
#include "Poco/ByteOrder.h"

#include <memory>

// Unnamed namespace for internal linkage
namespace {

class base_msg
{
public:
    base_msg(void) = default;
    virtual ~base_msg(void) = default;

    base_msg(const base_msg& msg) = delete;
    base_msg(base_msg&& msg) = delete;

    base_msg& operator=(const base_msg& msg) = delete;
    base_msg& operator=(base_msg&& msg) = delete;

    virtual std::string name(void) const = 0;
};

class player_info : public base_msg
{
public:
    player_info(void) = default;
    virtual ~player_info(void) = default;

    std::string name(void) const override
    {
        return "player_info";
    }
};

class friend_info : public base_msg
{
public:
    friend_info(void) = default;
    virtual ~friend_info(void) = default;

    std::string name(void) const override
    {
        return "friend_info";
    }
};

} // unnamed namespace

PocoCoreTest::PocoCoreTest(const std::string& name)
    : CppUnit::TestCase(name)
{
}

void PocoCoreTest::setUp(void)
{
    // body
}

void PocoCoreTest::tearDown(void)
{
    // body
}

int PocoCoreTest::countTestCases(void) const
{
    return 1;
}

void PocoCoreTest::testFixedLength(void)
{
    static_assert(sizeof(Poco::Int8) == 1);
    static_assert(sizeof(Poco::UInt8) == 1);
    static_assert(sizeof(Poco::Int16) == 2);
    static_assert(sizeof(Poco::UInt16) == 2);
    static_assert(sizeof(Poco::Int32) == 4);
    static_assert(sizeof(Poco::UInt32) == 4);
    static_assert(sizeof(Poco::Int64) == 8);
    static_assert(sizeof(Poco::UInt64) == 8);
    static_assert(sizeof(Poco::IntPtr) == 8);
    static_assert(sizeof(Poco::UIntPtr) == 8);

    static_assert(sizeof(int8_t) == 1);
    static_assert(sizeof(uint8_t) == 1);
    static_assert(sizeof(int16_t) == 2);
    static_assert(sizeof(uint16_t) == 2);
    static_assert(sizeof(int32_t) == 4);
    static_assert(sizeof(uint32_t) == 4);
    static_assert(sizeof(int64_t) == 8);
    static_assert(sizeof(uint64_t) == 8);
    static_assert(sizeof(intptr_t) == 8);
    static_assert(sizeof(uintptr_t) == 8);

    static_assert(sizeof(signed char) == 1);
    static_assert(sizeof(unsigned char) == 1);

    static_assert(sizeof(signed short) == 2);
    static_assert(sizeof(unsigned short) == 2);

    static_assert(sizeof(signed int) == 4);
    static_assert(sizeof(unsigned int) == 4);

#if POCO_OS == POCO_OS_WINDOWS_NT
    static_assert(sizeof(signed long int) == 4);
    static_assert(sizeof(unsigned long int) == 4);

    static_assert(sizeof(signed long long int) == 8);
    static_assert(sizeof(unsigned long long int) == 8);
#else
    static_assert(sizeof(signed long int) == 8);
    static_assert(sizeof(unsigned long int) == 8);

    static_assert(sizeof(signed long long int) == 8);
    static_assert(sizeof(unsigned long long int) == 8);
#endif // POCO_OS == POCO_OS_WINDOWS_NT

    static_assert(sizeof(float) == 4);
    static_assert(sizeof(double) == 8);

    static_assert(sizeof(size_t) == 8);
    static_assert(sizeof(ptrdiff_t) == 8);
}

void PocoCoreTest::testEnvironment(void)
{
    std::string env_key = "shmilyl_env_test";
    assertTrue(!Poco::Environment::has(env_key));

    Poco::Environment::set(env_key, "milan");
    assertTrue(Poco::Environment::has(env_key));
    assertTrue(Poco::Environment::get(env_key) == "milan");

    assertTrue(!Poco::Environment::osName().empty());
    assertTrue(!Poco::Environment::osDisplayName().empty());
    assertTrue(!Poco::Environment::osVersion().empty());
    assertTrue(!Poco::Environment::osArchitecture().empty());
    assertTrue(!Poco::Environment::nodeName().empty());
    assertTrue(!Poco::Environment::nodeId().empty());
    assertTrue(Poco::Environment::processorCount() > 0);
}

void PocoCoreTest::testAtomicCounter(void)
{
    Poco::AtomicCounter ac;

    assertTrue(ac.value() == 0);
    assertTrue(ac++ == 0);
    assertTrue(ac-- == 1);
    assertTrue(++ac == 1);
    assertTrue(--ac == 0);

    ac = 2;
    assertTrue(ac.value() == 2);

    Poco::AtomicCounter ac2{100};

    assertTrue(ac2.value() == 100);
    assertTrue(ac2++ == 100);
    assertTrue(ac2-- == 101);
    assertTrue(++ac2 == 101);
    assertTrue(--ac2 == 100);

    ac2 = 200;
    assertTrue(ac2.value() == 200);

    ac = ac2;
    assertTrue(ac.value() == 200);
    assertTrue(ac.value() == ac2.value());
}

void PocoCoreTest::testASCII(void)
{
    assertTrue(Poco::Ascii::isAscii('0'));
    assertTrue(Poco::Ascii::isAscii('A'));
    assertTrue(!Poco::Ascii::isAscii(-1));
    assertTrue(!Poco::Ascii::isAscii(128));

    assertTrue(Poco::Ascii::isSpace(' '));
    assertTrue(Poco::Ascii::isSpace('\t'));
    assertTrue(Poco::Ascii::isSpace('\r'));
    assertTrue(Poco::Ascii::isSpace('\n'));
    assertTrue(!Poco::Ascii::isSpace('A'));
    assertTrue(!Poco::Ascii::isSpace(-1));
    assertTrue(!Poco::Ascii::isSpace(222));

    assertTrue(Poco::Ascii::isDigit('0'));
    assertTrue(Poco::Ascii::isDigit('1'));
    assertTrue(Poco::Ascii::isDigit('2'));
    assertTrue(Poco::Ascii::isDigit('3'));
    assertTrue(Poco::Ascii::isDigit('4'));
    assertTrue(Poco::Ascii::isDigit('5'));
    assertTrue(Poco::Ascii::isDigit('6'));
    assertTrue(Poco::Ascii::isDigit('7'));
    assertTrue(Poco::Ascii::isDigit('8'));
    assertTrue(Poco::Ascii::isDigit('9'));
    assertTrue(!Poco::Ascii::isDigit('a'));

    assertTrue(Poco::Ascii::isHexDigit('0'));
    assertTrue(Poco::Ascii::isHexDigit('1'));
    assertTrue(Poco::Ascii::isHexDigit('2'));
    assertTrue(Poco::Ascii::isHexDigit('3'));
    assertTrue(Poco::Ascii::isHexDigit('4'));
    assertTrue(Poco::Ascii::isHexDigit('5'));
    assertTrue(Poco::Ascii::isHexDigit('6'));
    assertTrue(Poco::Ascii::isHexDigit('7'));
    assertTrue(Poco::Ascii::isHexDigit('8'));
    assertTrue(Poco::Ascii::isHexDigit('9'));
    assertTrue(Poco::Ascii::isHexDigit('a'));
    assertTrue(Poco::Ascii::isHexDigit('b'));
    assertTrue(Poco::Ascii::isHexDigit('c'));
    assertTrue(Poco::Ascii::isHexDigit('d'));
    assertTrue(Poco::Ascii::isHexDigit('e'));
    assertTrue(Poco::Ascii::isHexDigit('f'));
    assertTrue(Poco::Ascii::isHexDigit('A'));
    assertTrue(Poco::Ascii::isHexDigit('B'));
    assertTrue(Poco::Ascii::isHexDigit('C'));
    assertTrue(Poco::Ascii::isHexDigit('D'));
    assertTrue(Poco::Ascii::isHexDigit('E'));
    assertTrue(Poco::Ascii::isHexDigit('F'));
    assertTrue(!Poco::Ascii::isHexDigit('G'));

    assertTrue(Poco::Ascii::isPunct('.'));
    assertTrue(Poco::Ascii::isPunct(','));
    assertTrue(!Poco::Ascii::isPunct('A'));

    assertTrue(Poco::Ascii::isAlpha('a'));
    assertTrue(Poco::Ascii::isAlpha('Z'));
    assertTrue(!Poco::Ascii::isAlpha('0'));

    assertTrue(Poco::Ascii::isLower('a'));
    assertTrue(!Poco::Ascii::isLower('A'));

    assertTrue(Poco::Ascii::isUpper('A'));
    assertTrue(!Poco::Ascii::isUpper('a'));

    assertTrue(Poco::Ascii::toLower('A') == 'a');
    assertTrue(Poco::Ascii::toLower('z') == 'z');
    assertTrue(Poco::Ascii::toLower('0') == '0');

    assertTrue(Poco::Ascii::toUpper('a') == 'A');
    assertTrue(Poco::Ascii::toUpper('0') == '0');
    assertTrue(Poco::Ascii::toUpper('Z') == 'Z');
}

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4723)
#endif

void PocoCoreTest::testNanInf(void)
{
    // float
    {
        float a   = 0.f;
        float b   = 0.f;
        float nan = a / b;
        float inf = 1.f / b;

        assertTrue(!Poco::FPEnvironment::isNaN(a));
        assertTrue(!Poco::FPEnvironment::isInfinite(a));

        assertTrue(!Poco::FPEnvironment::isNaN(b));
        assertTrue(!Poco::FPEnvironment::isInfinite(b));

        assertTrue(Poco::FPEnvironment::isNaN(nan));
        assertTrue(!Poco::FPEnvironment::isInfinite(nan));

        assertTrue(!Poco::FPEnvironment::isNaN(inf));
        assertTrue(Poco::FPEnvironment::isInfinite(inf));
    }

    // double
    {
        double a   = 0.0;
        double b   = 0.0;
        double nan = a / b;
        double inf = 1.0 / b;

        assertTrue(!Poco::FPEnvironment::isNaN(a));
        assertTrue(!Poco::FPEnvironment::isInfinite(a));

        assertTrue(!Poco::FPEnvironment::isNaN(b));
        assertTrue(!Poco::FPEnvironment::isInfinite(b));

        assertTrue(Poco::FPEnvironment::isNaN(nan));
        assertTrue(!Poco::FPEnvironment::isInfinite(nan));

        assertTrue(!Poco::FPEnvironment::isNaN(inf));
        assertTrue(Poco::FPEnvironment::isInfinite(inf));
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

void PocoCoreTest::testNDC(void)
{
    {
        Poco::NDC ndc;
        assertTrue(ndc.depth() == 0);
        assertTrue(ndc.toString().empty());

        ndc.push("item1");
        assertTrue(ndc.depth() == 1);
        assertTrue(ndc.toString() == "item1");

        ndc.push("item2");
        assertTrue(ndc.depth() == 2);
        assertTrue(ndc.toString() == "item1:item2");

        ndc.pop();
        assertTrue(ndc.depth() == 1);
        assertTrue(ndc.toString() == "item1");

        ndc.pop();
        assertTrue(ndc.depth() == 0);
        assertTrue(ndc.toString().empty());
    }

    assertTrue(Poco::NDC::current().depth() == 0);
    assertTrue(Poco::NDC::current().toString().empty());

    {
        assertTrue(Poco::NDC::current().depth() == 0);
        assertTrue(Poco::NDC::current().toString().empty());

        Poco::NDC::current().push("item1");
        assertTrue(Poco::NDC::current().depth() == 1);
        assertTrue(Poco::NDC::current().toString() == "item1");

        Poco::NDC::current().push("item2");
        assertTrue(Poco::NDC::current().depth() == 2);
        assertTrue(Poco::NDC::current().toString() == "item1:item2");

        Poco::NDC::current().pop();
        assertTrue(Poco::NDC::current().depth() == 1);
        assertTrue(Poco::NDC::current().toString() == "item1");

        Poco::NDC::current().pop();
        assertTrue(Poco::NDC::current().depth() == 0);
        assertTrue(Poco::NDC::current().toString().empty());
    }

    assertTrue(Poco::NDC::current().depth() == 0);
    assertTrue(Poco::NDC::current().toString().empty());
}

void PocoCoreTest::testDynamicFactory(void)
{
    Poco::DynamicFactory<base_msg> df;

    df.registerClass<player_info>("player_info");
    df.registerClass<friend_info>("friend_info");

    assertTrue(df.isClass("player_info"));
    assertTrue(df.isClass("friend_info"));

    std::unique_ptr<player_info> player_ptr(static_cast<player_info*>(df.createInstance("player_info")));
    std::unique_ptr<friend_info> friend_ptr(static_cast<friend_info*>(df.createInstance("friend_info")));

    assertTrue(player_ptr && player_ptr.get() && player_ptr->name() == "player_info");
    assertTrue(friend_ptr && friend_ptr.get() && friend_ptr->name() == "friend_info");

    df.unregisterClass("friend_info");

    assertTrue(df.isClass("player_info"));
    assertTrue(!df.isClass("friend_info"));
}

void PocoCoreTest::testMemoryPooly(void)
{
    Poco::MemoryPool pool(sizeof(int), 32);

    assertTrue(pool.blockSize() == sizeof(int));
    assertTrue(pool.allocated() == 32);

    std::vector<void*> ptrs;
    for (size_t idx = 0; idx != 32; ++idx)
    {
        int* pi = static_cast<int*>(pool.get());
        *pi     = static_cast<int>(idx + 1);

        ptrs.emplace_back(pi);
    }

    *static_cast<int*>(pool.get()) = 33;

    assertTrue(pool.blockSize() == sizeof(int));
    assertTrue(pool.allocated() == 33);

    for (size_t idx = 0; idx != ptrs.size(); ++idx)
    {
        int* pi = static_cast<int*>(ptrs[idx]);
        assertTrue(*pi == static_cast<int>(idx + 1));
        pool.release(pi);
    }
}

void PocoCoreTest::testFlipBytes(void)
{
    // int16_t | uint16_t
    {
        int16_t norm = static_cast<int16_t>(0xAABB);
        int16_t flip = Poco::ByteOrder::flipBytes(norm);
        assertTrue(static_cast<uint16_t>(flip) == 0xBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(flip == norm);
    }
    {
        uint16_t norm = static_cast<uint16_t>(0xAABB);
        uint16_t flip = Poco::ByteOrder::flipBytes(norm);
        assertTrue(flip == 0xBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(flip == norm);
    }

    // int32_t | uint32_t
    {
        int32_t norm = static_cast<int32_t>(0xAABBCCDD);
        int32_t flip = Poco::ByteOrder::flipBytes(norm);
        assertTrue(static_cast<uint32_t>(flip)  == 0xDDCCBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(flip == norm);
    }
    {
        uint32_t norm = static_cast<uint32_t>(0xAABBCCDD);
        uint32_t flip = Poco::ByteOrder::flipBytes(norm);
        assertTrue(flip == 0xDDCCBBAA);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(flip == norm);
    }

    // int64_t | uint64_t
    {
        int64_t norm = static_cast<int64_t>(0x8899AABBCCDDEEFF);
        int64_t flip = Poco::ByteOrder::flipBytes(norm);
        assertTrue(static_cast<uint64_t>(flip)  == 0xFFEEDDCCBBAA9988);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(flip == norm);
    }
    {
        uint64_t norm = static_cast<uint64_t>(0x8899AABBCCDDEEFF);
        uint64_t flip = Poco::ByteOrder::flipBytes(norm);
        assertTrue(flip == 0xFFEEDDCCBBAA9988);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(flip == norm);
    }
}

void PocoCoreTest::testBigEndian(void)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromBigEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}

void PocoCoreTest::testLittleEndian(void)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toLittleEndian(norm);
        assertTrue(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromLittleEndian(norm);
        assertTrue(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}

void PocoCoreTest::testNetwork(void)
{
#if defined(POCO_ARCH_BIG_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm == flip);
    }
#elif defined(POCO_ARCH_LITTLE_ENDIAN)
    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::toNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }

    {
        int16_t norm = 4;
        int16_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint16_t norm = 4;
        uint16_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int32_t norm = 4;
        int32_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint32_t norm = 4;
        uint32_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        int64_t norm = 4;
        int64_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
    {
        uint64_t norm = 4;
        uint64_t flip = Poco::ByteOrder::fromNetwork(norm);
        assertTrue(norm != flip);

        flip = Poco::ByteOrder::flipBytes(flip);
        assertTrue(norm == flip);
    }
#else
    #error "Unknown byte endian."
#endif // defined(POCO_ARCH_BIG_ENDIAN)
}

CppUnit::Test* PocoCoreTest::suite(void)
{
    CppUnit::TestSuite* testSuite = new CppUnit::TestSuite("PocoCoreTest");

    CppUnit_addTest(testSuite, PocoCoreTest, testFixedLength);
    CppUnit_addTest(testSuite, PocoCoreTest, testEnvironment);
    CppUnit_addTest(testSuite, PocoCoreTest, testAtomicCounter);
    CppUnit_addTest(testSuite, PocoCoreTest, testASCII);
    CppUnit_addTest(testSuite, PocoCoreTest, testNanInf);
    CppUnit_addTest(testSuite, PocoCoreTest, testNDC);
    CppUnit_addTest(testSuite, PocoCoreTest, testDynamicFactory);
    CppUnit_addTest(testSuite, PocoCoreTest, testMemoryPooly);
    CppUnit_addTest(testSuite, PocoCoreTest, testFlipBytes);
    CppUnit_addTest(testSuite, PocoCoreTest, testBigEndian);
    CppUnit_addTest(testSuite, PocoCoreTest, testLittleEndian);
    CppUnit_addTest(testSuite, PocoCoreTest, testNetwork);

    return testSuite;
}