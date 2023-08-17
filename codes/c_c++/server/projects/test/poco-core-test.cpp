#include "gtest/gtest.h"
#include "Poco/Environment.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Nullable.h"
#include "Poco/Ascii.h"
#include "Poco/NestedDiagnosticContext.h"
#include "Poco/DynamicFactory.h"
#include "Poco/MemoryPool.h"

// Unnamed namespace for internal linkage
namespace {

class BaseInfo
{
public:
    BaseInfo(void) = default;
    virtual ~BaseInfo(void) = default;

    BaseInfo(const BaseInfo& msg) = delete;
    BaseInfo(BaseInfo&& msg) = delete;

    BaseInfo& operator=(const BaseInfo& msg) = delete;
    BaseInfo& operator=(BaseInfo&& msg) = delete;

    virtual std::string name(void) const = 0;
};

class PlayerInfo : public BaseInfo
{
public:
    PlayerInfo(void) = default;
    virtual ~PlayerInfo(void) = default;

    std::string name(void) const override
    {
        return "PlayerInfo";
    }
};

class FriendInfo : public BaseInfo
{
public:
    FriendInfo(void) = default;
    virtual ~FriendInfo(void) = default;

    std::string name(void) const override
    {
        return "FriendInfo";
    }
};

} // unnamed namespace

GTEST_TEST(PocoCoreTest, FixedLength)
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

GTEST_TEST(PocoCoreTest, Environment)
{
    std::string envKey = "shmilyl_env_test";
    if (Poco::Environment::has(envKey))
    {
        ASSERT_TRUE(Poco::Environment::get(envKey) == "milan");
    }
    else
    {
        Poco::Environment::set(envKey, "milan");
        ASSERT_TRUE(Poco::Environment::get(envKey) == "milan");
    }

    ASSERT_TRUE(!Poco::Environment::osName().empty());
    ASSERT_TRUE(!Poco::Environment::osDisplayName().empty());
    ASSERT_TRUE(!Poco::Environment::osVersion().empty());
    ASSERT_TRUE(!Poco::Environment::osArchitecture().empty());
    ASSERT_TRUE(!Poco::Environment::nodeName().empty());
    ASSERT_TRUE(!Poco::Environment::nodeId().empty());
    ASSERT_TRUE(Poco::Environment::processorCount() > 0);
}

GTEST_TEST(PocoCoreTest, AtomicCounter)
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

GTEST_TEST(PocoCoreTest, Nullable)
{
    Poco::Nullable<int>         i;
    Poco::Nullable<double>      f;
    Poco::Nullable<std::string> s;

    ASSERT_TRUE(i.isNull());
    ASSERT_TRUE(f.isNull());
    ASSERT_TRUE(s.isNull());

    i = 1;
    f = 1.5;
    s = "abc";

    ASSERT_TRUE(!i.isNull());
    ASSERT_TRUE(!f.isNull());
    ASSERT_TRUE(!s.isNull());

    ASSERT_TRUE(i == 1);
    ASSERT_TRUE(f == 1.5);
    ASSERT_TRUE(s == "abc");

    i.clear();
    f.clear();
    s.clear();

    ASSERT_TRUE(i.isNull());
    ASSERT_TRUE(f.isNull());
    ASSERT_TRUE(s.isNull());

    Poco::Nullable<int> n1;
    ASSERT_TRUE(n1.isNull());
    ASSERT_TRUE(n1.value(42) == 42);
    ASSERT_TRUE(n1.isNull());

    ASSERT_TRUE(!(0 == n1));
    ASSERT_TRUE(0 != n1);
    ASSERT_TRUE(!(n1 == 0));
    ASSERT_TRUE(n1 != 0);

    n1 = 1;
    ASSERT_TRUE(!n1.isNull());
    ASSERT_TRUE(n1.value() == 1);

    Poco::Nullable<int> n2(42);
    ASSERT_TRUE(!n2.isNull());
    ASSERT_TRUE(n2.value() == 42);
    ASSERT_TRUE(n2.value(99) == 42);

    ASSERT_TRUE(!(0 == n2));
    ASSERT_TRUE(0 != n2);
    ASSERT_TRUE(!(n2 == 0));
    ASSERT_TRUE(n2 != 0);

    n1 = n2;
    ASSERT_TRUE(!n1.isNull());
    ASSERT_TRUE(n1.value() == 42);

    n1.clear();
    n2.clear();
    ASSERT_TRUE(n1 == n2);

    n1 = 1; n2 = 1;
    ASSERT_TRUE(n1 == n2);

    n1.clear();
    ASSERT_TRUE(n1 < n2);
    ASSERT_TRUE(n2 > n1);

    n2 = -1; n1 = 0;
    ASSERT_TRUE(n2 < n1);
    ASSERT_TRUE(n2 != n1);
    ASSERT_TRUE(n1 > n2);
}

GTEST_TEST(PocoCoreTest, ASCII)
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

GTEST_TEST(PocoCoreTest, NDC)
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

GTEST_TEST(PocoCoreTest, DynamicFactory)
{
    Poco::DynamicFactory<BaseInfo> df;

    df.registerClass<PlayerInfo>("PlayerInfo");
    df.registerClass<FriendInfo>("FriendInfo");

    ASSERT_TRUE(df.isClass("PlayerInfo"));
    ASSERT_TRUE(df.isClass("FriendInfo"));

    std::unique_ptr<PlayerInfo> player_ptr(static_cast<PlayerInfo*>(df.createInstance("PlayerInfo")));
    std::unique_ptr<FriendInfo> friend_ptr(static_cast<FriendInfo*>(df.createInstance("FriendInfo")));

    ASSERT_TRUE(player_ptr && player_ptr.get() && player_ptr->name() == "PlayerInfo");
    ASSERT_TRUE(friend_ptr && friend_ptr.get() && friend_ptr->name() == "FriendInfo");

    df.unregisterClass("FriendInfo");

    ASSERT_TRUE(df.isClass("PlayerInfo"));
    ASSERT_TRUE(!df.isClass("FriendInfo"));
}

GTEST_TEST(PocoCoreTest, MemoryPool)
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