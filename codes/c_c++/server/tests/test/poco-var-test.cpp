#include "gtest/gtest.h"
#include "util/types.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/Dynamic/Struct.h"

GTEST_TEST(PocoVarTest, VarInt8)
{
    Poco::Int8         src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(Poco::Int8));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        Poco::Int8 value = a1.extract<Poco::Int8>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarUInt8)
{
    Poco::UInt8        src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(Poco::UInt8));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        Poco::UInt8 value = a1.extract<Poco::UInt8>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarInt16)
{
    Poco::Int16        src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(Poco::Int16));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        Poco::Int16 value = a1.extract<Poco::Int16>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarUInt16)
{
    Poco::UInt16       src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(Poco::UInt16));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        Poco::UInt16 value = a1.extract<Poco::UInt16>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarInt32)
{
    Poco::Int32        src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(Poco::Int32));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        Poco::Int32 value = a1.extract<Poco::Int32>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarUInt32)
{
    Poco::UInt32       src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(Poco::UInt32));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        Poco::UInt32 value = a1.extract<Poco::UInt32>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarInt64)
{
    Poco::Int64        src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(Poco::Int64));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        Poco::Int64 value = a1.extract<Poco::Int64>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarUInt64)
{
    Poco::UInt64       src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(Poco::UInt64));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        Poco::UInt64 value = a1.extract<Poco::UInt64>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarLong)
{
    long               src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(long));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        long value = a1.extract<long>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarULong)
{
    unsigned long      src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(unsigned long));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        unsigned long value = a1.extract<unsigned long>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarLongLong)
{
    long long          src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(long long));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        long long value = a1.extract<long long>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarULongLong)
{
    unsigned long long src = 32;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(unsigned long long));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        unsigned long long value = a1.extract<unsigned long long>();
        ASSERT_TRUE(value == 32);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1;
        ASSERT_TRUE(a3 == 33);

        a3 = a1 - 1;
        ASSERT_TRUE(a3 == 31);

        a3 += 1;
        ASSERT_TRUE(a3 == 32);

        a3 -= 1;
        ASSERT_TRUE(a3 == 31);

        a3 = a1 / 2;
        ASSERT_TRUE(a3 == 16);

        a3 = a1 * 2;
        ASSERT_TRUE(a3 == 64);

        a3 /= 2;
        ASSERT_TRUE(a3 == 32);

        a3 *= 2;
        ASSERT_TRUE(a3 == 64);
    }
}

GTEST_TEST(PocoVarTest, VarBool)
{
    bool               src = true;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(bool));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "true");
    ASSERT_TRUE(s2 == 1);
    ASSERT_TRUE(s3 == 1);
    ASSERT_TRUE(s4 == 1);
    ASSERT_TRUE(s5 == 1);
    ASSERT_TRUE(s6 == 1);
    ASSERT_TRUE(s7 == 1);
    ASSERT_TRUE(s8 == 1);
    ASSERT_TRUE(s9 == 1);
    ASSERT_TRUE(s10 == 1.f);
    ASSERT_TRUE(s11 == 1.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == '\x1');
    ASSERT_TRUE(s14 == 1);
    ASSERT_TRUE(s15 == 1);
    ASSERT_TRUE(s16 == 1);
    ASSERT_TRUE(s17 == 1);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        bool value = a1.extract<bool>();
        ASSERT_TRUE(value);
    }
}

GTEST_TEST(PocoVarTest, VarChar)
{
    char               src = ' ';
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(char));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == " ");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        char value = a1.extract<char>();
        ASSERT_TRUE(value == ' ');
    }
}

GTEST_TEST(PocoVarTest, VarFloat)
{
    float              src = 32.f;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(float));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        float value = a1.extract<float>();
        ASSERT_TRUE(value == 32.f);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1.f;
        ASSERT_TRUE(a3 == 33.f);

        a3 = a1 - 1.f;
        ASSERT_TRUE(a3 == 31.f);

        a3 += 1.f;
        ASSERT_TRUE(a3 == 32.f);

        a3 -= 1.f;
        ASSERT_TRUE(a3 == 31.f);

        a3 = a1 / 2.f;
        ASSERT_TRUE(a3 == 16.f);

        a3 = a1 * 2.f;
        ASSERT_TRUE(a3 == 64.f);

        a3 /= 2.f;
        ASSERT_TRUE(a3 == 32.f);

        a3 *= 2.f;
        ASSERT_TRUE(a3 == 64.f);
    }
}

GTEST_TEST(PocoVarTest, VarDouble)
{
    double             src = 32.0;
    Poco::Dynamic::Var a1  = src;

    ASSERT_TRUE(a1.type() == typeid(double));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == ' ');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        Poco::Dynamic::Var a2(a1);
        std::string        t2;

        a2.convert(t2);
        ASSERT_TRUE(s1 == t2);
    }

    {
        double value = a1.extract<double>();
        ASSERT_TRUE(value == 32.0);
    }

    {
        Poco::Dynamic::Var a3 = a1 + 1.0;
        ASSERT_TRUE(a3 == 33.0);

        a3 = a1 - 1.0;
        ASSERT_TRUE(a3 == 31.0);

        a3 += 1.0;
        ASSERT_TRUE(a3 == 32.0);

        a3 -= 1.0;
        ASSERT_TRUE(a3 == 31.0);

        a3 = a1 / 2.0;
        ASSERT_TRUE(a3 == 16.0);

        a3 = a1 * 2.0;
        ASSERT_TRUE(a3 == 64.0);

        a3 /= 2.0;
        ASSERT_TRUE(a3 == 32.0);

        a3 *= 2.0;
        ASSERT_TRUE(a3 == 64.0);
    }
}

GTEST_TEST(PocoVarTest, VarString)
{
    Poco::Dynamic::Var a1("32");

    ASSERT_TRUE(a1.type() == typeid(std::string));

    std::string        s1;
    Poco::Int8         s2;
    Poco::Int16        s3;
    Poco::Int32        s4;
    Poco::Int64        s5;
    Poco::UInt8        s6;
    Poco::UInt16       s7;
    Poco::UInt32       s8;
    Poco::UInt64       s9;
    float              s10;
    double             s11;
    bool               s12 = false;
    char               s13;
    long               s14;
    unsigned long      s15;
    long long          s16;
    unsigned long long s17;

    a1.convert(s1);
    a1.convert(s2);
    a1.convert(s3);
    a1.convert(s4);
    a1.convert(s5);
    a1.convert(s6);
    a1.convert(s7);
    a1.convert(s8);
    a1.convert(s9);
    a1.convert(s10);
    a1.convert(s11);
    a1.convert(s12);
    a1.convert(s13);
    a1.convert(s14);
    a1.convert(s15);
    a1.convert(s16);
    a1.convert(s17);

    ASSERT_TRUE(s1 == "32");
    ASSERT_TRUE(s2 == 32);
    ASSERT_TRUE(s3 == 32);
    ASSERT_TRUE(s4 == 32);
    ASSERT_TRUE(s5 == 32);
    ASSERT_TRUE(s6 == 32);
    ASSERT_TRUE(s7 == 32);
    ASSERT_TRUE(s8 == 32);
    ASSERT_TRUE(s9 == 32);
    ASSERT_TRUE(s10 == 32.0f);
    ASSERT_TRUE(s11 == 32.0);
    ASSERT_TRUE(s12);
    ASSERT_TRUE(s13 == '3');
    ASSERT_TRUE(s14 == 32);
    ASSERT_TRUE(s15 == 32);
    ASSERT_TRUE(s16 == 32);
    ASSERT_TRUE(s17 == 32);

    {
        const std::string& value = a1.extract<std::string>();
        ASSERT_TRUE(value == "32");
    }

    {
        Poco::Dynamic::Var a4(123);
        std::string        s("456");

        Poco::Dynamic::Var a5 = a4 + s;
        ASSERT_TRUE(a5 == "123456");

        a4 += s;
        ASSERT_TRUE(a4 == "123456");

        Poco::Dynamic::Var a6 = a4 + "789";
        ASSERT_TRUE(a6 == "123456789");

        a4 += "789";
        ASSERT_TRUE(a4 == "123456789");

        a4 = "";
        ASSERT_TRUE(!a4);

        a4 = "0";
        ASSERT_TRUE(!a4);

        a4 = "FaLsE";
        ASSERT_TRUE(!a4);
    }
}

GTEST_TEST(PocoVarTest, VarConversionOperator)
{
    Poco::Dynamic::Var any("42");
    int i = any;
    ASSERT_TRUE(i == 42);
    ASSERT_TRUE(any == i);

    any = 123;
    std::string s1 = any.convert<std::string>();
    ASSERT_TRUE(s1 == "123");
    ASSERT_TRUE(s1 == any);
    ASSERT_TRUE(any == s1);
    ASSERT_TRUE("123" == any);

    any = 321;
    s1 = any.convert<std::string>();
    ASSERT_TRUE(s1 == "321");

    any = "456";
    ASSERT_TRUE(any == "456");
    ASSERT_TRUE("456" == any);

    any = 789;
    std::string s2 = any.convert<std::string>();
    ASSERT_TRUE(s2 == "789");
    ASSERT_TRUE(s2 == any);
    ASSERT_TRUE(any == s2);
    ASSERT_TRUE("789" == any);

    Poco::Dynamic::Var any2 = "1.5";
    double d = any2;
    ASSERT_TRUE(d == 1.5);
    ASSERT_TRUE(any2 == d);
}

GTEST_TEST(PocoVarTest, VarComparisonOperators)
{
    Poco::Dynamic::Var any1 = 1;
    Poco::Dynamic::Var any2 = "1";
    ASSERT_TRUE(any1 == any2);
    ASSERT_TRUE(any1 == 1);
    ASSERT_TRUE(1 == any1);
    ASSERT_TRUE(any1 == "1");
    ASSERT_TRUE("1" == any1);
    ASSERT_TRUE(any1 <= 1);
    ASSERT_TRUE(1 >= any1);
    ASSERT_TRUE(any1 <= 2);
    ASSERT_TRUE(2 >= any1);
    ASSERT_TRUE(any1 < 2);
    ASSERT_TRUE(2 > any1);
    ASSERT_TRUE(any1 > 0);
    ASSERT_TRUE(0 < any1);
    ASSERT_TRUE(any1 >= 1);
    ASSERT_TRUE(1 <= any1);
    ASSERT_TRUE(any1 >= 0);
    ASSERT_TRUE(0 <= any1);

    any1 = 1L;
    ASSERT_TRUE(any1 == any2);
    ASSERT_TRUE(any1 == 1L);
    ASSERT_TRUE(1L == any1);
    ASSERT_TRUE(any1 == "1");
    ASSERT_TRUE("1" == any1);
    ASSERT_TRUE(any1 != 2L);
    ASSERT_TRUE(2L != any1);
    ASSERT_TRUE(any1 != "2");
    ASSERT_TRUE("2" != any1);
    ASSERT_TRUE(any1 <= 1L);
    ASSERT_TRUE(1L >= any1);
    ASSERT_TRUE(any1 <= 2L);
    ASSERT_TRUE(2L >= any1);
    ASSERT_TRUE(any1 < 2L);
    ASSERT_TRUE(2L > any1);
    ASSERT_TRUE(any1 > 0);
    ASSERT_TRUE(0 < any1);
    ASSERT_TRUE(any1 >= 1L);
    ASSERT_TRUE(1L <= any1);
    ASSERT_TRUE(any1 >= 0);
    ASSERT_TRUE(0 <= any1);

    any1 = 0x31;
    ASSERT_TRUE(any1 == '1');
    ASSERT_TRUE('1' == any1);
    ASSERT_TRUE(any1 <= '1');
    ASSERT_TRUE('1' >= any1);
    ASSERT_TRUE(any1 <= '2');
    ASSERT_TRUE('2' >= any1);
    ASSERT_TRUE(any1 < '2');
    ASSERT_TRUE('2' > any1);
    ASSERT_TRUE(any1 > 0);
    ASSERT_TRUE(0 < any1);
    ASSERT_TRUE(any1 >= '1');
    ASSERT_TRUE('1' <= any1);
    ASSERT_TRUE(any1 >= 0);
    ASSERT_TRUE(0 <= any1);

    any1 = "2";
    ASSERT_TRUE(any1 != any2);
    ASSERT_TRUE(any1 != 1);
    ASSERT_TRUE(1 != any1);
    ASSERT_TRUE(any1 != "1");
    ASSERT_TRUE("1" != any1);

    any1 = 1.5;
    ASSERT_TRUE(any1 == 1.5);
    ASSERT_TRUE(1.5 == any1);
    ASSERT_TRUE(any1 == "1.5");
    ASSERT_TRUE("1.5" == any1);
    ASSERT_TRUE(any1 != 2.5);
    ASSERT_TRUE(2.5 != any1);
    ASSERT_TRUE(any1 != "2.5");
    ASSERT_TRUE("2.5" != any1);
    ASSERT_TRUE(any1 <= 1.5);
    ASSERT_TRUE(1.5 >= any1);
    ASSERT_TRUE(any1 <= 2.5);
    ASSERT_TRUE(2.5 >= any1);
    ASSERT_TRUE(any1 < 2.5);
    ASSERT_TRUE(2.5 > any1);
    ASSERT_TRUE(any1 > 0);
    ASSERT_TRUE(0 < any1);
    ASSERT_TRUE(any1 >= 1.5);
    ASSERT_TRUE(1.5 <= any1);
    ASSERT_TRUE(any1 >= 0);
    ASSERT_TRUE(0 <= any1);

    any1 = 1.5f;
    ASSERT_TRUE(any1 == 1.5f);
    ASSERT_TRUE(1.5f == any1);
    ASSERT_TRUE(any1 == "1.5");
    ASSERT_TRUE("1.5" == any1);
    ASSERT_TRUE(any1 != 2.5f);
    ASSERT_TRUE(2.5f != any1);
    ASSERT_TRUE(any1 != "2.5");
    ASSERT_TRUE("2.5" != any1);
    ASSERT_TRUE(any1 <= 1.5f);
    ASSERT_TRUE(1.5f >= any1);
    ASSERT_TRUE(any1 <= 2.5f);
    ASSERT_TRUE(2.5f >= any1);
    ASSERT_TRUE(any1 < 2.5f);
    ASSERT_TRUE(2.5f > any1);
    ASSERT_TRUE(any1 > 0);
    ASSERT_TRUE(0 < any1);
    ASSERT_TRUE(any1 >= 1.5f);
    ASSERT_TRUE(1.5f <= any1);
    ASSERT_TRUE(any1 >= 0);
    ASSERT_TRUE(0 <= any1);
}

GTEST_TEST(PocoVarTest, VarArithmeticOperators)
{
    Poco::Dynamic::Var any1 = 1;
    Poco::Dynamic::Var any2 = 2;
    Poco::Dynamic::Var any3 = any1 + any2;
    ASSERT_TRUE(any3 == 3);

    int i = 1;
    i    += any1;
    ASSERT_TRUE(2 == i);

    any1 = 3;
    ASSERT_TRUE((5 - any1) == 2);

    any2 = 5;
    any3 = any2 - any1;
    ASSERT_TRUE(any3 == 2);

    any3 -= 1;
    ASSERT_TRUE(any3 == 1);

    i  = 5;
    i -= any1;
    ASSERT_TRUE(2 == i);

    any1 = 3;
    ASSERT_TRUE((5 * any1) == 15);

    any2 = 5;
    any3 = any1 * any2;
    ASSERT_TRUE(any3 == 15);

    any3 *= 3;
    ASSERT_TRUE(any3 == 45);

    i  = 5;
    i *= any1;
    ASSERT_TRUE(15 == i);

    any1 = 3;
    ASSERT_TRUE((9 / any1) == 3);

    any2 = 9;
    any3 = any2 / any1;
    ASSERT_TRUE(any3 == 3);

    any3 /= 3;
    ASSERT_TRUE(any3 == 1);

    i  = 9;
    i /= any1;
    ASSERT_TRUE(3 == i);

    any1 = 1.0f;
    any2 = .5f;
    any3 = .0f;
    any3 = any1 + any2;
    ASSERT_TRUE(any3 == 1.5f);

    any3 += .5f;
    ASSERT_TRUE(any3 == 2.0f);

    any1 = 1.0;
    any2 = .5;
    any3 = 0.0;
    any3 = any1 + any2;
    ASSERT_TRUE(any3 == 1.5);

    any3 += .5;
    ASSERT_TRUE(any3 == 2.0);

    any1 = 1;
    any2 = "2";
    any3 = any1 + any2;
    ASSERT_TRUE(any3 == 3);

    any2  = "4";
    any3 += any2;
    ASSERT_TRUE(any3 == 7);
    ASSERT_TRUE(1 + any3 == 8);

    any1 = "123";
    any2 = "456";
    any3 = any1 + any2;
    ASSERT_TRUE(any3 == "123456");

    any2  = "789";
    any3 += any2;
    ASSERT_TRUE(any3 == "123456789");
    ASSERT_TRUE(("xyz" + any3) == "xyz123456789");

    any1 = 10;

    ASSERT_TRUE(any1++ == 10);
    ASSERT_TRUE(any1 == 11);
    ASSERT_TRUE(++any1 == 12);

    ASSERT_TRUE(any1-- == 12);
    ASSERT_TRUE(any1 == 11);
    ASSERT_TRUE(--any1 == 10);
}

GTEST_TEST(PocoVarTest, ArrayToString)
{
    std::string s1("string");
    Poco::Int8  s2(23);

    std::vector<Poco::Dynamic::Var> s16;
    s16.push_back(s1);
    s16.push_back(s2);

    Poco::Dynamic::Var a1(s16);
    ASSERT_TRUE(a1.convert<std::string>() == "[ \"string\", 23 ]");
}

GTEST_TEST(PocoVarTest, ArrayToStringEscape)
{
    std::string s1("\"quoted string\"");
    Poco::Int8  s2(23);

    std::vector<Poco::Dynamic::Var> s16;
    s16.push_back(s1);
    s16.push_back(s2);

    Poco::Dynamic::Var a1(s16);
    ASSERT_TRUE(a1.convert<std::string>() == "[ \"\\\"quoted string\\\"\", 23 ]");
}

GTEST_TEST(PocoVarTest, StructToString)
{
    Poco::Dynamic::Struct<std::string> aStruct;
    aStruct["First Name"] = "Junior";
    aStruct["Last Name"]  = "POCO";
    aStruct["Age"]        = 1;

    Poco::Dynamic::Var a1(aStruct);
    const std::string  res      = a1.convert<std::string>();
    const std::string  expected = "{ \"Age\": 1, \"First Name\": \"Junior\", \"Last Name\": \"POCO\" }";
    ASSERT_TRUE(res == expected);
    ASSERT_TRUE(res == aStruct.toString());
}

GTEST_TEST(PocoVarTest, StructToStringEscape)
{
    Poco::Dynamic::Struct<std::string> aStruct;
    aStruct["Value"] = "Value with \" and \n";

    Poco::Dynamic::Var a1(aStruct);
    const std::string  res      = a1.convert<std::string>();
    const std::string  expected = "{ \"Value\": \"Value with \\\" and \\n\" }";
    ASSERT_TRUE(res == expected);
    ASSERT_TRUE(res == aStruct.toString());
}

GTEST_TEST(PocoVarTest, JSONDeserializeString)
{
    Poco::Dynamic::Var a("test");
    std::string        tst;

    {
        tst = Poco::Dynamic::Var::toString(a);

        Poco::Dynamic::Var b = Poco::Dynamic::Var::parse(tst);
        ASSERT_TRUE(b.convert<std::string>() == "test");
    }

    {
        Poco::Dynamic::Var c('c');

        tst = Poco::Dynamic::Var::toString(c);

        Poco::Dynamic::Var b2 = Poco::Dynamic::Var::parse(tst);
        ASSERT_TRUE(b2.convert<char>() == 'c');
    }

    tst = "{ \"a\": \"1\", \"b\": \"2\" \n}";
    a   = Poco::Dynamic::Var::parse(tst);
    ASSERT_TRUE(a.toString() == "{ \"a\": \"1\", \"b\": \"2\" }");

    tst = "{ \"a\": \"1\", \"b\": \"2\"\n}";
    a   = Poco::Dynamic::Var::parse(tst);
    ASSERT_TRUE(a.toString() == "{ \"a\": \"1\", \"b\": \"2\" }");

    tst = "{ \"message\": \"escape\\b\\f\\n\\r\\t\", \"path\": \"\\/dev\\/null\", \"zero\": null }";
    a   = Poco::Dynamic::Var::parse(tst);
    ASSERT_TRUE(a.toString() == "{ \"message\": \"escape\\b\\f\\n\\r\\t\", \"path\": \"/dev/null\", \"zero\": null }");
}

GTEST_TEST(PocoVarTest, JSONDeserializePrimitives)
{
    int8_t   i8{-12};
    uint16_t u16{2345};
    int32_t  i32{-24343};
    uint64_t u64{1234567890};
    bool     b{false};
    float    f{3.1415f};
    double   d{3.1415};

    std::string s8  = Poco::Dynamic::Var::toString(i8);
    std::string s16 = Poco::Dynamic::Var::toString(u16);
    std::string s32 = Poco::Dynamic::Var::toString(i32);
    std::string s64 = Poco::Dynamic::Var::toString(u64);
    std::string sb  = Poco::Dynamic::Var::toString(b);
    std::string sf  = Poco::Dynamic::Var::toString(f);
    std::string sd  = Poco::Dynamic::Var::toString(d);

    Poco::Dynamic::Var a8  = Poco::Dynamic::Var::parse(s8);
    Poco::Dynamic::Var a16 = Poco::Dynamic::Var::parse(s16);
    Poco::Dynamic::Var a32 = Poco::Dynamic::Var::parse(s32);
    Poco::Dynamic::Var a64 = Poco::Dynamic::Var::parse(s64);
    Poco::Dynamic::Var ab  = Poco::Dynamic::Var::parse(sb);
    Poco::Dynamic::Var af  = Poco::Dynamic::Var::parse(sf);
    Poco::Dynamic::Var ad  = Poco::Dynamic::Var::parse(sd);

    ASSERT_TRUE(a8 == i8);
    ASSERT_TRUE(a16 == u16);
    ASSERT_TRUE(a32 == i32);
    ASSERT_TRUE(a64 == u64);
    ASSERT_TRUE(ab == b);
    ASSERT_TRUE(af == f);
    ASSERT_TRUE(ad == d);
}

GTEST_TEST(PocoVarTest, JSONDeserializeArray)
{
    int8_t      i8{-12};
    uint16_t    u16{2345};
    int32_t     i32{-24343};
    uint64_t    u64{1234567890};
    bool        b{false};
    float       f{3.1415f};
    double      d{3.1415};
    std::string s{"test string"};
    char        c{'x'};

    std::vector<Poco::Dynamic::Var> aVec;
    aVec.push_back(i8);
    aVec.push_back(u16);
    aVec.push_back(i32);
    aVec.push_back(u64);
    aVec.push_back(b);
    aVec.push_back(f);
    aVec.push_back(d);
    aVec.push_back(s);
    aVec.push_back(c);

    std::string        sVec = Poco::Dynamic::Var::toString(aVec);
    Poco::Dynamic::Var a    = Poco::Dynamic::Var::parse(sVec);

    ASSERT_TRUE(a[0] == i8);
    ASSERT_TRUE(a[1] == u16);
    ASSERT_TRUE(a[2] == i32);
    ASSERT_TRUE(a[3] == u64);
    ASSERT_TRUE(a[4] == b);
    ASSERT_TRUE(a[5] == f);
    ASSERT_TRUE(a[6] == d);
    ASSERT_TRUE(a[7] == s);
    ASSERT_TRUE(a[8] == c);
}

GTEST_TEST(PocoVarTest, JSONRoundtripStruct)
{
    int64_t     i64{-1234567890};
    uint64_t    u64{1234567890};
    bool        b{false};
    double      d{3.1415};
    std::string s{"test string"};

    Poco::Dynamic::Struct<std::string> aStr;
    aStr["i64"] = i64;
    aStr["u64"] = u64;
    aStr["b"]   = b;
    aStr["d"]   = d;
    aStr["s"]   = s;

    std::string        sStr = Poco::Dynamic::Var::toString(aStr);
    Poco::Dynamic::Var a    = Poco::Dynamic::Var::parse(sStr);

    ASSERT_TRUE(a["i64"].isInteger());
    ASSERT_TRUE(!a["u64"].isSigned());
    ASSERT_TRUE(a["b"].isBoolean());
    ASSERT_TRUE(a["d"].isNumeric());
    ASSERT_TRUE(a["s"].isString());

    ASSERT_TRUE(Poco::Dynamic::Var::toString(a) == sStr);
}