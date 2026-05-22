#include "doctest.h"
#include "Poco/AtomicCounter.h"
#include "Poco/Nullable.h"
#include "Poco/Ascii.h"
#include "Poco/DynamicFactory.h"

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

TEST_SUITE("Poco core")
{
    TEST_CASE("FixedLength")
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
        static_assert(sizeof(time_t) == 8);
        static_assert(sizeof(ptrdiff_t) == 8);
    }

    TEST_CASE("AtomicCounter")
    {
        Poco::AtomicCounter ac;

        CHECK(ac.value() == 0);
        CHECK(ac++ == 0);
        CHECK(ac-- == 1);
        CHECK(++ac == 1);
        CHECK(--ac == 0);

        ac = 2;
        CHECK(ac.value() == 2);

        Poco::AtomicCounter ac2{100};

        CHECK(ac2.value() == 100);
        CHECK(ac2++ == 100);
        CHECK(ac2-- == 101);
        CHECK(++ac2 == 101);
        CHECK(--ac2 == 100);

        ac2 = 200;
        CHECK(ac2.value() == 200);

        ac = ac2;
        CHECK(ac.value() == 200);
        CHECK(ac.value() == ac2.value());
    }

    TEST_CASE("Nullable")
    {
        Poco::Nullable<int>         i;
        Poco::Nullable<double>      f;
        Poco::Nullable<std::string> s;

        CHECK(i.isNull());
        CHECK(f.isNull());
        CHECK(s.isNull());

        i = 1;
        f = 1.5;
        s = "abc";

        CHECK(!i.isNull());
        CHECK(!f.isNull());
        CHECK(!s.isNull());

        CHECK(i == 1);
        CHECK(f == 1.5);
        CHECK(s == "abc");

        i.clear();
        f.clear();
        s.clear();

        CHECK(i.isNull());
        CHECK(f.isNull());
        CHECK(s.isNull());

        Poco::Nullable<int> n1;
        CHECK(n1.isNull());
        CHECK(n1.value(42) == 42);
        CHECK(n1.isNull());

        CHECK(!(0 == n1));
        CHECK(0 != n1);
        CHECK(!(n1 == 0));
        CHECK(n1 != 0);

        n1 = 1;
        CHECK(!n1.isNull());
        CHECK(n1.value() == 1);

        Poco::Nullable<int> n2(42);
        CHECK(!n2.isNull());
        CHECK(n2.value() == 42);
        CHECK(n2.value(99) == 42);

        CHECK(!(0 == n2));
        CHECK(0 != n2);
        CHECK(!(n2 == 0));
        CHECK(n2 != 0);

        n1 = n2;
        CHECK(!n1.isNull());
        CHECK(n1.value() == 42);

        n1.clear();
        n2.clear();
        CHECK(n1 == n2);

        n1 = 1; n2 = 1;
        CHECK(n1 == n2);

        n1.clear();
        CHECK(n1 < n2);
        CHECK(n2 > n1);

        n2 = -1; n1 = 0;
        CHECK(n2 < n1);
        CHECK(n2 != n1);
        CHECK(n1 > n2);
    }

    TEST_CASE("ASCII")
    {
        CHECK(Poco::Ascii::isAscii('0'));
        CHECK(Poco::Ascii::isAscii('A'));
        CHECK(!Poco::Ascii::isAscii(-1));
        CHECK(!Poco::Ascii::isAscii(128));

        CHECK(Poco::Ascii::isSpace(' '));
        CHECK(Poco::Ascii::isSpace('\t'));
        CHECK(Poco::Ascii::isSpace('\r'));
        CHECK(Poco::Ascii::isSpace('\n'));
        CHECK(!Poco::Ascii::isSpace('A'));
        CHECK(!Poco::Ascii::isSpace(-1));
        CHECK(!Poco::Ascii::isSpace(222));

        CHECK(Poco::Ascii::isDigit('0'));
        CHECK(Poco::Ascii::isDigit('1'));
        CHECK(Poco::Ascii::isDigit('2'));
        CHECK(Poco::Ascii::isDigit('3'));
        CHECK(Poco::Ascii::isDigit('4'));
        CHECK(Poco::Ascii::isDigit('5'));
        CHECK(Poco::Ascii::isDigit('6'));
        CHECK(Poco::Ascii::isDigit('7'));
        CHECK(Poco::Ascii::isDigit('8'));
        CHECK(Poco::Ascii::isDigit('9'));
        CHECK(!Poco::Ascii::isDigit('a'));

        CHECK(Poco::Ascii::isHexDigit('0'));
        CHECK(Poco::Ascii::isHexDigit('1'));
        CHECK(Poco::Ascii::isHexDigit('2'));
        CHECK(Poco::Ascii::isHexDigit('3'));
        CHECK(Poco::Ascii::isHexDigit('4'));
        CHECK(Poco::Ascii::isHexDigit('5'));
        CHECK(Poco::Ascii::isHexDigit('6'));
        CHECK(Poco::Ascii::isHexDigit('7'));
        CHECK(Poco::Ascii::isHexDigit('8'));
        CHECK(Poco::Ascii::isHexDigit('9'));
        CHECK(Poco::Ascii::isHexDigit('a'));
        CHECK(Poco::Ascii::isHexDigit('b'));
        CHECK(Poco::Ascii::isHexDigit('c'));
        CHECK(Poco::Ascii::isHexDigit('d'));
        CHECK(Poco::Ascii::isHexDigit('e'));
        CHECK(Poco::Ascii::isHexDigit('f'));
        CHECK(Poco::Ascii::isHexDigit('A'));
        CHECK(Poco::Ascii::isHexDigit('B'));
        CHECK(Poco::Ascii::isHexDigit('C'));
        CHECK(Poco::Ascii::isHexDigit('D'));
        CHECK(Poco::Ascii::isHexDigit('E'));
        CHECK(Poco::Ascii::isHexDigit('F'));
        CHECK(!Poco::Ascii::isHexDigit('G'));

        CHECK(Poco::Ascii::isPunct('.'));
        CHECK(Poco::Ascii::isPunct(','));
        CHECK(!Poco::Ascii::isPunct('A'));

        CHECK(Poco::Ascii::isAlpha('a'));
        CHECK(Poco::Ascii::isAlpha('Z'));
        CHECK(!Poco::Ascii::isAlpha('0'));

        CHECK(Poco::Ascii::isLower('a'));
        CHECK(!Poco::Ascii::isLower('A'));

        CHECK(Poco::Ascii::isUpper('A'));
        CHECK(!Poco::Ascii::isUpper('a'));

        CHECK(Poco::Ascii::toLower('A') == 'a');
        CHECK(Poco::Ascii::toLower('z') == 'z');
        CHECK(Poco::Ascii::toLower('0') == '0');

        CHECK(Poco::Ascii::toUpper('a') == 'A');
        CHECK(Poco::Ascii::toUpper('0') == '0');
        CHECK(Poco::Ascii::toUpper('Z') == 'Z');
    }

    TEST_CASE("DynamicFactory")
    {
        Poco::DynamicFactory<BaseInfo> df;

        df.registerClass<PlayerInfo>("PlayerInfo");
        df.registerClass<FriendInfo>("FriendInfo");

        CHECK(df.isClass("PlayerInfo"));
        CHECK(df.isClass("FriendInfo"));

        std::unique_ptr<PlayerInfo> player_ptr(static_cast<PlayerInfo*>(df.createInstance("PlayerInfo")));
        std::unique_ptr<FriendInfo> friend_ptr(static_cast<FriendInfo*>(df.createInstance("FriendInfo")));

        CHECK(player_ptr);
        CHECK(player_ptr.get());
        CHECK(player_ptr->name() == "PlayerInfo");
        CHECK(friend_ptr);
        CHECK(friend_ptr.get());
        CHECK(friend_ptr->name() == "FriendInfo");

        df.unregisterClass("FriendInfo");

        CHECK(df.isClass("PlayerInfo"));
        CHECK(!df.isClass("FriendInfo"));
    }
}
