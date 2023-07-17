#include "gtest/gtest.h"
#include "Poco/Environment.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Ascii.h"
#include "Poco/FPEnvironment.h"
#include "Poco/NestedDiagnosticContext.h"
#include "Poco/DynamicFactory.h"
#include "Poco/MemoryPool.h"

#include <memory>

namespace dynamic_factory {

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

} // namespace dynamic_factory

TEST(CoreTest, FixedLength)
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

TEST(CoreTest, Environment)
{
    std::string env_key = "shmilyl_env_test";
    ASSERT_TRUE(!Poco::Environment::has(env_key));

    Poco::Environment::set(env_key, "milan");
    ASSERT_TRUE(Poco::Environment::has(env_key));
    ASSERT_TRUE(Poco::Environment::get(env_key) == "milan");

    ASSERT_TRUE(!Poco::Environment::osName().empty());
    ASSERT_TRUE(!Poco::Environment::osDisplayName().empty());
    ASSERT_TRUE(!Poco::Environment::osVersion().empty());
    ASSERT_TRUE(!Poco::Environment::osArchitecture().empty());
    ASSERT_TRUE(!Poco::Environment::nodeName().empty());
    ASSERT_TRUE(!Poco::Environment::nodeId().empty());
    ASSERT_TRUE(Poco::Environment::processorCount() > 0);
}

TEST(CoreTest, AtomicCounter)
{
    Poco::AtomicCounter ac;

    ASSERT_TRUE(ac.value() == 0);
    ASSERT_TRUE(ac++ == 0);
    ASSERT_TRUE(ac-- == 1);
    ASSERT_TRUE(++ac == 1);
    ASSERT_TRUE(--ac == 0);

    ac = 2;
    ASSERT_TRUE(ac.value() == 2);

    Poco::AtomicCounter ac2{100};

    ASSERT_TRUE(ac2.value() == 100);
    ASSERT_TRUE(ac2++ == 100);
    ASSERT_TRUE(ac2-- == 101);
    ASSERT_TRUE(++ac2 == 101);
    ASSERT_TRUE(--ac2 == 100);

    ac2 = 200;
    ASSERT_TRUE(ac2.value() == 200);

    ac = ac2;
    ASSERT_TRUE(ac.value() == 200);
    ASSERT_TRUE(ac.value() == ac2.value());
}

TEST(CoreTest, ASCII)
{
    ASSERT_TRUE(Poco::Ascii::isAscii('0'));
    ASSERT_TRUE(Poco::Ascii::isAscii('A'));
    ASSERT_TRUE(!Poco::Ascii::isAscii(-1));
    ASSERT_TRUE(!Poco::Ascii::isAscii(128));

    ASSERT_TRUE(Poco::Ascii::isSpace(' '));
    ASSERT_TRUE(Poco::Ascii::isSpace('\t'));
    ASSERT_TRUE(Poco::Ascii::isSpace('\r'));
    ASSERT_TRUE(Poco::Ascii::isSpace('\n'));
    ASSERT_TRUE(!Poco::Ascii::isSpace('A'));
    ASSERT_TRUE(!Poco::Ascii::isSpace(-1));
    ASSERT_TRUE(!Poco::Ascii::isSpace(222));

    ASSERT_TRUE(Poco::Ascii::isDigit('0'));
    ASSERT_TRUE(Poco::Ascii::isDigit('1'));
    ASSERT_TRUE(Poco::Ascii::isDigit('2'));
    ASSERT_TRUE(Poco::Ascii::isDigit('3'));
    ASSERT_TRUE(Poco::Ascii::isDigit('4'));
    ASSERT_TRUE(Poco::Ascii::isDigit('5'));
    ASSERT_TRUE(Poco::Ascii::isDigit('6'));
    ASSERT_TRUE(Poco::Ascii::isDigit('7'));
    ASSERT_TRUE(Poco::Ascii::isDigit('8'));
    ASSERT_TRUE(Poco::Ascii::isDigit('9'));
    ASSERT_TRUE(!Poco::Ascii::isDigit('a'));

    ASSERT_TRUE(Poco::Ascii::isHexDigit('0'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('1'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('2'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('3'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('4'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('5'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('6'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('7'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('8'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('9'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('a'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('b'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('c'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('d'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('e'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('f'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('A'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('B'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('C'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('D'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('E'));
    ASSERT_TRUE(Poco::Ascii::isHexDigit('F'));
    ASSERT_TRUE(!Poco::Ascii::isHexDigit('G'));

    ASSERT_TRUE(Poco::Ascii::isPunct('.'));
    ASSERT_TRUE(Poco::Ascii::isPunct(','));
    ASSERT_TRUE(!Poco::Ascii::isPunct('A'));

    ASSERT_TRUE(Poco::Ascii::isAlpha('a'));
    ASSERT_TRUE(Poco::Ascii::isAlpha('Z'));
    ASSERT_TRUE(!Poco::Ascii::isAlpha('0'));

    ASSERT_TRUE(Poco::Ascii::isLower('a'));
    ASSERT_TRUE(!Poco::Ascii::isLower('A'));

    ASSERT_TRUE(Poco::Ascii::isUpper('A'));
    ASSERT_TRUE(!Poco::Ascii::isUpper('a'));

    ASSERT_TRUE(Poco::Ascii::toLower('A') == 'a');
    ASSERT_TRUE(Poco::Ascii::toLower('z') == 'z');
    ASSERT_TRUE(Poco::Ascii::toLower('0') == '0');

    ASSERT_TRUE(Poco::Ascii::toUpper('a') == 'A');
    ASSERT_TRUE(Poco::Ascii::toUpper('0') == '0');
    ASSERT_TRUE(Poco::Ascii::toUpper('Z') == 'Z');
}

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4723)
#endif

TEST(CoreTest, NanInf)
{
    // float
    {
        float a   = 0.f;
        float b   = 0.f;
        float nan = a / b;
        float inf = 1.f / b;

        ASSERT_TRUE(!Poco::FPEnvironment::isNaN(a));
        ASSERT_TRUE(!Poco::FPEnvironment::isInfinite(a));

        ASSERT_TRUE(!Poco::FPEnvironment::isNaN(b));
        ASSERT_TRUE(!Poco::FPEnvironment::isInfinite(b));

        ASSERT_TRUE(Poco::FPEnvironment::isNaN(nan));
        ASSERT_TRUE(!Poco::FPEnvironment::isInfinite(nan));

        ASSERT_TRUE(!Poco::FPEnvironment::isNaN(inf));
        ASSERT_TRUE(Poco::FPEnvironment::isInfinite(inf));
    }

    // double
    {
        double a   = 0.0;
        double b   = 0.0;
        double nan = a / b;
        double inf = 1.0 / b;

        ASSERT_TRUE(!Poco::FPEnvironment::isNaN(a));
        ASSERT_TRUE(!Poco::FPEnvironment::isInfinite(a));

        ASSERT_TRUE(!Poco::FPEnvironment::isNaN(b));
        ASSERT_TRUE(!Poco::FPEnvironment::isInfinite(b));

        ASSERT_TRUE(Poco::FPEnvironment::isNaN(nan));
        ASSERT_TRUE(!Poco::FPEnvironment::isInfinite(nan));

        ASSERT_TRUE(!Poco::FPEnvironment::isNaN(inf));
        ASSERT_TRUE(Poco::FPEnvironment::isInfinite(inf));
    }
}

#if defined(_MSC_VER)
    #pragma warning(pop)
#endif

TEST(CoreTest, NDC)
{
    {
        Poco::NDC ndc;
        ASSERT_TRUE(ndc.depth() == 0);
        ASSERT_TRUE(ndc.toString().empty());

        ndc.push("item1");
        ASSERT_TRUE(ndc.depth() == 1);
        ASSERT_TRUE(ndc.toString() == "item1");

        ndc.push("item2");
        ASSERT_TRUE(ndc.depth() == 2);
        ASSERT_TRUE(ndc.toString() == "item1:item2");

        ndc.pop();
        ASSERT_TRUE(ndc.depth() == 1);
        ASSERT_TRUE(ndc.toString() == "item1");

        ndc.pop();
        ASSERT_TRUE(ndc.depth() == 0);
        ASSERT_TRUE(ndc.toString().empty());
    }

    ASSERT_TRUE(Poco::NDC::current().depth() == 0);
    ASSERT_TRUE(Poco::NDC::current().toString().empty());

    {
        ASSERT_TRUE(Poco::NDC::current().depth() == 0);
        ASSERT_TRUE(Poco::NDC::current().toString().empty());

        Poco::NDC::current().push("item1");
        ASSERT_TRUE(Poco::NDC::current().depth() == 1);
        ASSERT_TRUE(Poco::NDC::current().toString() == "item1");

        Poco::NDC::current().push("item2");
        ASSERT_TRUE(Poco::NDC::current().depth() == 2);
        ASSERT_TRUE(Poco::NDC::current().toString() == "item1:item2");

        Poco::NDC::current().pop();
        ASSERT_TRUE(Poco::NDC::current().depth() == 1);
        ASSERT_TRUE(Poco::NDC::current().toString() == "item1");

        Poco::NDC::current().pop();
        ASSERT_TRUE(Poco::NDC::current().depth() == 0);
        ASSERT_TRUE(Poco::NDC::current().toString().empty());
    }

    ASSERT_TRUE(Poco::NDC::current().depth() == 0);
    ASSERT_TRUE(Poco::NDC::current().toString().empty());
}

TEST(CoreTest, DynamicFactory)
{
    using namespace dynamic_factory;

    Poco::DynamicFactory<base_msg> df;

    df.registerClass<player_info>("player_info");
    df.registerClass<friend_info>("friend_info");

    ASSERT_TRUE(df.isClass("player_info"));
    ASSERT_TRUE(df.isClass("friend_info"));

    std::unique_ptr<player_info> player_ptr(static_cast<player_info*>(df.createInstance("player_info")));
    std::unique_ptr<friend_info> friend_ptr(static_cast<friend_info*>(df.createInstance("friend_info")));

    ASSERT_TRUE(player_ptr && player_ptr.get() && player_ptr->name() == "player_info");
    ASSERT_TRUE(friend_ptr && friend_ptr.get() && friend_ptr->name() == "friend_info");

    df.unregisterClass("friend_info");

    ASSERT_TRUE(df.isClass("player_info"));
    ASSERT_TRUE(!df.isClass("friend_info"));
}

TEST(CoreTest, MemoryPool)
{
    Poco::MemoryPool pool(sizeof(int), 32);

    ASSERT_TRUE(pool.blockSize() == sizeof(int));
    ASSERT_TRUE(pool.allocated() == 32);

    std::vector<void*> ptrs;
    for (size_t idx = 0; idx != 32; ++idx)
    {
        int* pi = static_cast<int*>(pool.get());
        *pi     = static_cast<int>(idx + 1);

        ptrs.emplace_back(pi);
    }

    *static_cast<int*>(pool.get()) = 33;

    ASSERT_TRUE(pool.blockSize() == sizeof(int));
    ASSERT_TRUE(pool.allocated() == 33);

    for (size_t idx = 0; idx != ptrs.size(); ++idx)
    {
        int* pi = static_cast<int*>(ptrs[idx]);
        ASSERT_TRUE(*pi == static_cast<int>(idx + 1));
        pool.release(pi);
    }
}