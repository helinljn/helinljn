#include "PocoNumericStringTest.h"
#include "CppUnit/TestSuite.h"
#include "CppUnit/TestCaller.h"

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#include "Poco/NumericString.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#include "cpp11_numeric_string.hpp"

PocoNumericStringTest::PocoNumericStringTest(const std::string& name)
    : CppUnit::TestCase(name)
{
}

void PocoNumericStringTest::setUp(void)
{
    // body
}

void PocoNumericStringTest::tearDown(void)
{
    // body
}

int PocoNumericStringTest::countTestCases(void) const
{
    return 1;
}

void PocoNumericStringTest::testPocoStrToNumeric(void)
{
    int8_t   i8  = 0;
    uint8_t  u8  = 0;
    int16_t  i16 = 0;
    uint16_t u16 = 0;
    int32_t  i32 = 0;
    uint32_t u32 = 0;
    int64_t  i64 = 0;
    uint64_t u64 = 0;
    float    f32 = 0.f;
    double   d64 = 0.0;

    assertTrue(Poco::strToInt("0", i8, 10) && i8 == 0);
    assertTrue(Poco::strToInt("000", i8, 10) && i8 == 0);
    assertTrue(Poco::strToInt("123", i8, 10) && i8 == 123);
    assertTrue(Poco::strToInt("-123", i8, 10) && i8 == -123);
    assertTrue(!Poco::strToInt("1234", i8, 10));

    assertTrue(Poco::strToInt("0", u8, 10) && u8 == 0);
    assertTrue(Poco::strToInt("000", u8, 10) && u8 == 0);
    assertTrue(Poco::strToInt("123", u8, 10) && u8 == 123);
    assertTrue(!Poco::strToInt("-123", u8, 10));
    assertTrue(!Poco::strToInt("1234", u8, 10));

    assertTrue(Poco::strToInt("0", i16, 10) && i16 == 0);
    assertTrue(Poco::strToInt("000", i16, 10) && i16 == 0);
    assertTrue(Poco::strToInt("12345", i16, 10) && i16 == 12345);
    assertTrue(Poco::strToInt("-12345", i16, 10) && i16 == -12345);
    assertTrue(!Poco::strToInt("123456", i16, 10));

    assertTrue(Poco::strToInt("0", u16, 10) && u16 == 0);
    assertTrue(Poco::strToInt("000", u16, 10) && u16 == 0);
    assertTrue(Poco::strToInt("12345", u16, 10) && u16 == 12345);
    assertTrue(!Poco::strToInt("-12345", u16, 10));
    assertTrue(!Poco::strToInt("123456", u16, 10));

    assertTrue(Poco::strToInt("0", i32, 10) && i32 == 0);
    assertTrue(Poco::strToInt("000", i32, 10) && i32 == 0);
    assertTrue(Poco::strToInt("1234567890", i32, 10) && i32 == 1234567890);
    assertTrue(Poco::strToInt("-1234567890", i32, 10) && i32 == -1234567890);
    assertTrue(!Poco::strToInt("12345678909", i32, 10));

    assertTrue(Poco::strToInt("0", u32, 10) && u32 == 0);
    assertTrue(Poco::strToInt("000", u32, 10) && u32 == 0);
    assertTrue(Poco::strToInt("1234567890", u32, 10) && u32 == 1234567890);
    assertTrue(!Poco::strToInt("-1234567890", u32, 10));
    assertTrue(!Poco::strToInt("12345678909", u32, 10));

    assertTrue(Poco::strToInt("0", i64, 10) && i64 == 0);
    assertTrue(Poco::strToInt("000", i64, 10) && i64 == 0);
    assertTrue(Poco::strToInt("1234567890987654321", i64, 10) && i64 == 1234567890987654321);
    assertTrue(Poco::strToInt("-1234567890987654321", i64, 10) && i64 == -1234567890987654321);
    assertTrue(!Poco::strToInt("12345678909876543210123456789", i64, 10));

    assertTrue(Poco::strToInt("0", u64, 10) && u64 == 0);
    assertTrue(Poco::strToInt("000", u64, 10) && u64 == 0);
    assertTrue(Poco::strToInt("1234567890987654321", u64, 10) && u64 == 1234567890987654321);
    assertTrue(!Poco::strToInt("-1234567890987654321", u64, 10));
    assertTrue(!Poco::strToInt("12345678909876543210123456789", u64, 10));

    assertTrue(Poco::strToFloat("0", f32) && f32 == 0.f);
    assertTrue(Poco::strToFloat("0.f", f32) && f32 == 0.f);
    assertTrue(Poco::strToFloat("0.0f", f32) && f32 == 0.f);
    assertTrue(Poco::strToFloat("0.00", f32) && f32 == 0.f);
    assertTrue(Poco::strToFloat("0.000001", f32) && f32 == 0.000001f);
    assertTrue(Poco::strToFloat("3.141593", f32) && f32 == 3.141593f);
    assertTrue(Poco::strToFloat("-1234567", f32) && f32 == -1234567.f);
    assertTrue(Poco::strToFloat("1234567", f32) && f32 == 1234567.f);

    assertTrue(Poco::strToDouble("0", d64) && d64 == 0.0);
    assertTrue(Poco::strToDouble("0.00", d64) && d64 == 0.0);
    assertTrue(Poco::strToDouble("0.000000000000001", d64) && d64 == 0.000000000000001);
    assertTrue(Poco::strToDouble("3.141592653589793", d64) && d64 == 3.141592653589793);
    assertTrue(Poco::strToDouble("-1234567890123456", d64) && d64 == -1234567890123456.0);
    assertTrue(Poco::strToDouble("1234567890123456", d64) && d64 == 1234567890123456.0);
}

void PocoNumericStringTest::testStdStrToNumeric(void)
{
    // cpp11_numeric_string::from_string
    {
        assertTrue(cpp11_numeric_string::from_string<int8_t>("0") == 0);
        assertTrue(cpp11_numeric_string::from_string<int8_t>("000") == 0);
        assertTrue(cpp11_numeric_string::from_string<int8_t>("123") == 123);
        assertTrue(cpp11_numeric_string::from_string<int8_t>("-123") == -123);
        assertTrue(cpp11_numeric_string::from_string<int8_t>("1234") == -46);

        assertTrue(cpp11_numeric_string::from_string<uint8_t>("0") == 0);
        assertTrue(cpp11_numeric_string::from_string<uint8_t>("000") == 0);
        assertTrue(cpp11_numeric_string::from_string<uint8_t>("123") == 123);
        assertTrue(cpp11_numeric_string::from_string<uint8_t>("-123") == 133);
        assertTrue(cpp11_numeric_string::from_string<uint8_t>("1234") == 210);

        assertTrue(cpp11_numeric_string::from_string<int16_t>("0") == 0);
        assertTrue(cpp11_numeric_string::from_string<int16_t>("000") == 0);
        assertTrue(cpp11_numeric_string::from_string<int16_t>("12345") == 12345);
        assertTrue(cpp11_numeric_string::from_string<int16_t>("-12345") == -12345);
        assertTrue(cpp11_numeric_string::from_string<int16_t>("123456") == -7616);

        assertTrue(cpp11_numeric_string::from_string<uint16_t>("0") == 0);
        assertTrue(cpp11_numeric_string::from_string<uint16_t>("000") == 0);
        assertTrue(cpp11_numeric_string::from_string<uint16_t>("12345") == 12345);
        assertTrue(cpp11_numeric_string::from_string<uint16_t>("-12345") == 53191);
        assertTrue(cpp11_numeric_string::from_string<uint16_t>("123456") == 57920);

        assertTrue(cpp11_numeric_string::from_string<int32_t>("0") == 0);
        assertTrue(cpp11_numeric_string::from_string<int32_t>("000") == 0);
        assertTrue(cpp11_numeric_string::from_string<int32_t>("1234567890") == 1234567890);
        assertTrue(cpp11_numeric_string::from_string<int32_t>("-1234567890") == -1234567890);

        assertTrue(cpp11_numeric_string::from_string<uint32_t>("0") == 0);
        assertTrue(cpp11_numeric_string::from_string<uint32_t>("000") == 0);
        assertTrue(cpp11_numeric_string::from_string<uint32_t>("1234567890") == 1234567890);
        assertTrue(cpp11_numeric_string::from_string<uint32_t>("-1234567890") == 3060399406);

        assertTrue(cpp11_numeric_string::from_string<int64_t>("0") == 0);
        assertTrue(cpp11_numeric_string::from_string<int64_t>("000") == 0);
        assertTrue(cpp11_numeric_string::from_string<int64_t>("1234567890987654321") == 1234567890987654321ll);
        assertTrue(cpp11_numeric_string::from_string<int64_t>("-1234567890987654321") == -1234567890987654321ll);

        assertTrue(cpp11_numeric_string::from_string<uint64_t>("0") == 0);
        assertTrue(cpp11_numeric_string::from_string<uint64_t>("000") == 0);
        assertTrue(cpp11_numeric_string::from_string<uint64_t>("1234567890987654321") == 1234567890987654321ull);
        assertTrue(cpp11_numeric_string::from_string<uint64_t>("-1234567890987654321") == 17212176182721897295ull);

        assertTrue(cpp11_numeric_string::from_string<float>("0") == 0.f);
        assertTrue(cpp11_numeric_string::from_string<float>("0.f") == 0.f);
        assertTrue(cpp11_numeric_string::from_string<float>("0.0f") == 0.f);
        assertTrue(cpp11_numeric_string::from_string<float>("0.00") == 0.f);
        assertTrue(cpp11_numeric_string::from_string<float>("0.000001") == 0.000001f);
        assertTrue(cpp11_numeric_string::from_string<float>("3.141593") == 3.141593f);
        assertTrue(cpp11_numeric_string::from_string<float>("-1234567") == -1234567.f);
        assertTrue(cpp11_numeric_string::from_string<float>("1234567") == 1234567.f);

        assertTrue(cpp11_numeric_string::from_string<double>("0") == 0.0);
        assertTrue(cpp11_numeric_string::from_string<double>("0.00") == 0.0);
        assertTrue(cpp11_numeric_string::from_string<double>("0.000000000000001") == 0.000000000000001);
        assertTrue(cpp11_numeric_string::from_string<double>("3.141592653589793") == 3.141592653589793);
        assertTrue(cpp11_numeric_string::from_string<double>("-1234567890123456") == -1234567890123456.0);
        assertTrue(cpp11_numeric_string::from_string<double>("1234567890123456") == 1234567890123456.0);
    }

    // cpp11_numeric_string::str_to_*
    {
        int8_t   i8  = 0;
        uint8_t  u8  = 0;
        int16_t  i16 = 0;
        uint16_t u16 = 0;
        int32_t  i32 = 0;
        uint32_t u32 = 0;
        int64_t  i64 = 0;
        uint64_t u64 = 0;
        float    f32 = 0.f;
        double   d64 = 0.0;

        assertTrue(cpp11_numeric_string::str_to_int("0", i8) && i8 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("000", i8) && i8 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("123", i8) && i8 == 123);
        assertTrue(cpp11_numeric_string::str_to_int("-123", i8) && i8 == -123);
        assertTrue(cpp11_numeric_string::str_to_int("1234", i8) && i8 == -46);

        assertTrue(cpp11_numeric_string::str_to_int("0", u8) && u8 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("000", u8) && u8 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("123", u8) && u8 == 123);
        assertTrue(cpp11_numeric_string::str_to_int("-123", u8) && u8 == 133);
        assertTrue(cpp11_numeric_string::str_to_int("1234", u8) && u8 == 210);

        assertTrue(cpp11_numeric_string::str_to_int("0", i16) && i16 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("000", i16) && i16 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("12345", i16) && i16 == 12345);
        assertTrue(cpp11_numeric_string::str_to_int("-12345", i16) && i16 == -12345);
        assertTrue(cpp11_numeric_string::str_to_int("123456", i16) && i16 == -7616);

        assertTrue(cpp11_numeric_string::str_to_int("0", u16) && u16 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("000", u16) && u16 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("12345", u16) && u16 == 12345);
        assertTrue(cpp11_numeric_string::str_to_int("-12345", u16) && u16 == 53191);
        assertTrue(cpp11_numeric_string::str_to_int("123456", u16) && u16 == 57920);

        assertTrue(cpp11_numeric_string::str_to_int("0", i32) && i32 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("000", i32) && i32 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("1234567890", i32) && i32 == 1234567890);
        assertTrue(cpp11_numeric_string::str_to_int("-1234567890", i32) && i32 == -1234567890);

        assertTrue(cpp11_numeric_string::str_to_int("0", u32) && u32 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("000", u32) && u32 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("1234567890", u32) && u32 == 1234567890);
        assertTrue(cpp11_numeric_string::str_to_int("-1234567890", u32) && u32 == 3060399406);

        assertTrue(cpp11_numeric_string::str_to_int("0", i64) && i64 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("000", i64) && i64 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("1234567890987654321", i64) && i64 == 1234567890987654321ll);
        assertTrue(cpp11_numeric_string::str_to_int("-1234567890987654321", i64) && i64 == -1234567890987654321ll);

        assertTrue(cpp11_numeric_string::str_to_int("0", u64) && u64 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("000", u64) && u64 == 0);
        assertTrue(cpp11_numeric_string::str_to_int("1234567890987654321", u64) && u64 == 1234567890987654321ull);
        assertTrue(cpp11_numeric_string::str_to_int("-1234567890987654321", u64) && u64 == 17212176182721897295ull);

        assertTrue(cpp11_numeric_string::str_to_float("0", f32) && f32 == 0.f);
        assertTrue(cpp11_numeric_string::str_to_float("0.f", f32) && f32 == 0.f);
        assertTrue(cpp11_numeric_string::str_to_float("0.0f", f32) && f32 == 0.f);
        assertTrue(cpp11_numeric_string::str_to_float("0.00", f32) && f32 == 0.f);
        assertTrue(cpp11_numeric_string::str_to_float("0.000001", f32) && f32 == 0.000001f);
        assertTrue(cpp11_numeric_string::str_to_float("3.141593", f32) && f32 == 3.141593f);
        assertTrue(cpp11_numeric_string::str_to_float("-1234567", f32) && f32 == -1234567.f);
        assertTrue(cpp11_numeric_string::str_to_float("1234567", f32) && f32 == 1234567.f);

        assertTrue(cpp11_numeric_string::str_to_double("0", d64) && d64 == 0.0);
        assertTrue(cpp11_numeric_string::str_to_double("0.00", d64) && d64 == 0.0);
        assertTrue(cpp11_numeric_string::str_to_double("0.000000000000001", d64) && d64 == 0.000000000000001);
        assertTrue(cpp11_numeric_string::str_to_double("3.141592653589793", d64) && d64 == 3.141592653589793);
        assertTrue(cpp11_numeric_string::str_to_double("-1234567890123456", d64) && d64 == -1234567890123456.0);
        assertTrue(cpp11_numeric_string::str_to_double("1234567890123456", d64) && d64 == 1234567890123456.0);
    }
}

void PocoNumericStringTest::testPocoNumericToStr(void)
{
    int8_t   i8  = 0;
    uint8_t  u8  = 0;
    int16_t  i16 = 0;
    uint16_t u16 = 0;
    int32_t  i32 = 0;
    uint32_t u32 = 0;
    int64_t  i64 = 0;
    uint64_t u64 = 0;
    float    f32 = 0.f;
    double   d64 = 0.0;

    std::string res;

    i8 = 0;
    assertTrue(Poco::intToStr(i8, 10, res) && res == "0");
    i8 = 123;
    assertTrue(Poco::intToStr(i8, 10, res) && res == "123");
    i8 = -123;
    assertTrue(Poco::intToStr(i8, 10, res) && res == "-123");

    u8 = 0;
    assertTrue(Poco::intToStr(u8, 10, res) && res == "0");
    u8 = 123;
    assertTrue(Poco::intToStr(u8, 10, res) && res == "123");

    i16 = 0;
    assertTrue(Poco::intToStr(i16, 10, res) && res == "0");
    i16 = 12345;
    assertTrue(Poco::intToStr(i16, 10, res) && res == "12345");
    i16 = -12345;
    assertTrue(Poco::intToStr(i16, 10, res) && res == "-12345");

    u16 = 0;
    assertTrue(Poco::intToStr(u16, 10, res) && res == "0");
    u16 = 12345;
    assertTrue(Poco::intToStr(u16, 10, res) && res == "12345");

    i32 = 0;
    assertTrue(Poco::intToStr(i32, 10, res) && res == "0");
    i32 = 1234567890;
    assertTrue(Poco::intToStr(i32, 10, res) && res == "1234567890");
    i32 = -1234567890;
    assertTrue(Poco::intToStr(i32, 10, res) && res == "-1234567890");

    u32 = 0;
    assertTrue(Poco::intToStr(u32, 10, res) && res == "0");
    u32 = 1234567890;
    assertTrue(Poco::intToStr(u32, 10, res) && res == "1234567890");

    i64 = 0;
    assertTrue(Poco::intToStr(i64, 10, res) && res == "0");
    i64 = 1234567890987654321;
    assertTrue(Poco::intToStr(i64, 10, res) && res == "1234567890987654321");
    i64 = -1234567890987654321;
    assertTrue(Poco::intToStr(i64, 10, res) && res == "-1234567890987654321");

    u64 = 0;
    assertTrue(Poco::intToStr(u64, 10, res) && res == "0");
    u64 = 1234567890987654321;
    assertTrue(Poco::intToStr(u64, 10, res) && res == "1234567890987654321");

    f32 = 0.f;
    assertTrue(Poco::floatToStr(res, f32) == "0" && res == "0");
    f32 = 0.000001f;
    assertTrue(Poco::floatToStr(res, f32) == "0.000001" && res == "0.000001");
    f32 = 3.141593f;
    assertTrue(Poco::floatToStr(res, f32) == "3.141593" && res == "3.141593");
    f32 = -1234567.f;
    assertTrue(Poco::floatToStr(res, f32) == "-1.234567e+6" && res == "-1.234567e+6");
    f32 = 1234567.f;
    assertTrue(Poco::floatToStr(res, f32) == "1.234567e+6" && res == "1.234567e+6");

    d64 = 0.0;
    assertTrue(Poco::doubleToStr(res, d64) == "0" && res == "0");
    d64 = 0.000000000000001;
    assertTrue(Poco::doubleToStr(res, d64) == "0.000000000000001" && res == "0.000000000000001");
    d64 = 3.141592653589793;
    assertTrue(Poco::doubleToStr(res, d64) == "3.141592653589793" && res == "3.141592653589793");
    d64 = -1234567890123456.0;
    assertTrue(Poco::doubleToStr(res, d64) == "-1.234567890123456e+15" && res == "-1.234567890123456e+15");
    d64 = 1234567890123456.0;
    assertTrue(Poco::doubleToStr(res, d64) == "1.234567890123456e+15" && res == "1.234567890123456e+15");
}

void PocoNumericStringTest::testStdNumericToStr(void)
{
    int8_t   i8  = 0;
    uint8_t  u8  = 0;
    int16_t  i16 = 0;
    uint16_t u16 = 0;
    int32_t  i32 = 0;
    uint32_t u32 = 0;
    int64_t  i64 = 0;
    uint64_t u64 = 0;
    float    f32 = 0.f;
    double   d64 = 0.0;

    std::string res;

    // cpp11_numeric_string::to_string
    {
        i8 = 0;
        assertTrue(cpp11_numeric_string::to_string(i8) == "0");
        i8 = 123;
        assertTrue(cpp11_numeric_string::to_string(i8) == "123");
        i8 = -123;
        assertTrue(cpp11_numeric_string::to_string(i8) == "-123");

        u8 = 0;
        assertTrue(cpp11_numeric_string::to_string(u8) == "0");
        u8 = 123;
        assertTrue(cpp11_numeric_string::to_string(u8) == "123");

        i16 = 0;
        assertTrue(cpp11_numeric_string::to_string(i16) == "0");
        i16 = 12345;
        assertTrue(cpp11_numeric_string::to_string(i16) == "12345");
        i16 = -12345;
        assertTrue(cpp11_numeric_string::to_string(i16) == "-12345");

        u16 = 0;
        assertTrue(cpp11_numeric_string::to_string(u16) == "0");
        u16 = 12345;
        assertTrue(cpp11_numeric_string::to_string(u16) == "12345");

        i32 = 0;
        assertTrue(cpp11_numeric_string::to_string(i32) == "0");
        i32 = 1234567890;
        assertTrue(cpp11_numeric_string::to_string(i32) == "1234567890");
        i32 = -1234567890;
        assertTrue(cpp11_numeric_string::to_string(i32) == "-1234567890");

        u32 = 0;
        assertTrue(cpp11_numeric_string::to_string(u32) == "0");
        u32 = 1234567890;
        assertTrue(cpp11_numeric_string::to_string(u32) == "1234567890");

        i64 = 0;
        assertTrue(cpp11_numeric_string::to_string(i64) == "0");
        i64 = 1234567890987654321;
        assertTrue(cpp11_numeric_string::to_string(i64) == "1234567890987654321");
        i64 = -1234567890987654321;
        assertTrue(cpp11_numeric_string::to_string(i64) == "-1234567890987654321");

        u64 = 0;
        assertTrue(cpp11_numeric_string::to_string(u64) == "0");
        u64 = 1234567890987654321;
        assertTrue(cpp11_numeric_string::to_string(u64) == "1234567890987654321");

        f32 = 0.f;
        assertTrue(!cpp11_numeric_string::to_string(f32).empty());
        f32 = 0.000001f;
        assertTrue(!cpp11_numeric_string::to_string(f32).empty());
        f32 = 3.141593f;
        assertTrue(!cpp11_numeric_string::to_string(f32).empty());
        f32 = -1234567.f;
        assertTrue(!cpp11_numeric_string::to_string(f32).empty());
        f32 = 1234567.f;
        assertTrue(!cpp11_numeric_string::to_string(f32).empty());

        d64 = 0.0;
        assertTrue(!cpp11_numeric_string::to_string(d64).empty());
        d64 = 0.000000000000001;
        assertTrue(!cpp11_numeric_string::to_string(d64).empty());
        d64 = 3.141592653589793;
        assertTrue(!cpp11_numeric_string::to_string(d64).empty());
        d64 = -1234567890123456.0;
        assertTrue(!cpp11_numeric_string::to_string(d64).empty());
        d64 = 1234567890123456.0;
        assertTrue(!cpp11_numeric_string::to_string(d64).empty());
    }

    // cpp11_numeric_string::*_to_str
    {
        i8 = 0;
        assertTrue(cpp11_numeric_string::int_to_str(i8, res) && res == "0");
        i8 = 123;
        assertTrue(cpp11_numeric_string::int_to_str(i8, res) && res == "123");
        i8 = -123;
        assertTrue(cpp11_numeric_string::int_to_str(i8, res) && res == "-123");

        u8 = 0;
        assertTrue(cpp11_numeric_string::int_to_str(u8, res) && res == "0");
        u8 = 123;
        assertTrue(cpp11_numeric_string::int_to_str(u8, res) && res == "123");

        i16 = 0;
        assertTrue(cpp11_numeric_string::int_to_str(i16, res) && res == "0");
        i16 = 12345;
        assertTrue(cpp11_numeric_string::int_to_str(i16, res) && res == "12345");
        i16 = -12345;
        assertTrue(cpp11_numeric_string::int_to_str(i16, res) && res == "-12345");

        u16 = 0;
        assertTrue(cpp11_numeric_string::int_to_str(u16, res) && res == "0");
        u16 = 12345;
        assertTrue(cpp11_numeric_string::int_to_str(u16, res) && res == "12345");

        i32 = 0;
        assertTrue(cpp11_numeric_string::int_to_str(i32, res) && res == "0");
        i32 = 1234567890;
        assertTrue(cpp11_numeric_string::int_to_str(i32, res) && res == "1234567890");
        i32 = -1234567890;
        assertTrue(cpp11_numeric_string::int_to_str(i32, res) && res == "-1234567890");

        u32 = 0;
        assertTrue(cpp11_numeric_string::int_to_str(u32, res) && res == "0");
        u32 = 1234567890;
        assertTrue(cpp11_numeric_string::int_to_str(u32, res) && res == "1234567890");

        i64 = 0;
        assertTrue(cpp11_numeric_string::int_to_str(i64, res) && res == "0");
        i64 = 1234567890987654321;
        assertTrue(cpp11_numeric_string::int_to_str(i64, res) && res == "1234567890987654321");
        i64 = -1234567890987654321;
        assertTrue(cpp11_numeric_string::int_to_str(i64, res) && res == "-1234567890987654321");

        u64 = 0;
        assertTrue(cpp11_numeric_string::int_to_str(u64, res) && res == "0");
        u64 = 1234567890987654321;
        assertTrue(cpp11_numeric_string::int_to_str(u64, res) && res == "1234567890987654321");

        f32 = 0.f;
        assertTrue(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = 0.000001f;
        assertTrue(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = 3.141593f;
        assertTrue(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = -1234567.f;
        assertTrue(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());
        f32 = 1234567.f;
        assertTrue(cpp11_numeric_string::float_to_str(f32, res) && !res.empty());

        d64 = 0.0;
        assertTrue(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = 0.000000000000001;
        assertTrue(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = 3.141592653589793;
        assertTrue(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = -1234567890123456.0;
        assertTrue(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
        d64 = 1234567890123456.0;
        assertTrue(cpp11_numeric_string::double_to_str(d64, res) && !res.empty());
    }
}

void PocoNumericStringTest::testNumberFormatter(void)
{
    {
        int8_t   i8  = 0;
        uint8_t  u8  = 0;
        int16_t  i16 = 0;
        uint16_t u16 = 0;
        int32_t  i32 = 0;
        uint32_t u32 = 0;
        int64_t  i64 = 0;
        uint64_t u64 = 0;
        float    f32 = 0.f;
        double   d64 = 0.0;

        std::string res;

        i8 = 0;
        assertTrue(Poco::NumberFormatter::format(i8) == "0");
        i8 = 123;
        assertTrue(Poco::NumberFormatter::format(i8) == "123");
        i8 = -123;
        assertTrue(Poco::NumberFormatter::format(i8) == "-123");

        u8 = 0;
        assertTrue(Poco::NumberFormatter::format(u8) == "0");
        u8 = 123;
        assertTrue(Poco::NumberFormatter::format(u8) == "123");

        i16 = 0;
        assertTrue(Poco::NumberFormatter::format(i16) == "0");
        i16 = 12345;
        assertTrue(Poco::NumberFormatter::format(i16) == "12345");
        i16 = -12345;
        assertTrue(Poco::NumberFormatter::format(i16) == "-12345");

        u16 = 0;
        assertTrue(Poco::NumberFormatter::format(u16) == "0");
        u16 = 12345;
        assertTrue(Poco::NumberFormatter::format(u16) == "12345");

        i32 = 0;
        assertTrue(Poco::NumberFormatter::format(i32) == "0");
        i32 = 1234567890;
        assertTrue(Poco::NumberFormatter::format(i32) == "1234567890");
        i32 = -1234567890;
        assertTrue(Poco::NumberFormatter::format(i32) == "-1234567890");

        u32 = 0;
        assertTrue(Poco::NumberFormatter::format(u32) == "0");
        u32 = 1234567890;
        assertTrue(Poco::NumberFormatter::format(u32) == "1234567890");

        i64 = 0;
        assertTrue(Poco::NumberFormatter::format(i64) == "0");
        i64 = 1234567890987654321;
        assertTrue(Poco::NumberFormatter::format(i64) == "1234567890987654321");
        i64 = -1234567890987654321;
        assertTrue(Poco::NumberFormatter::format(i64) == "-1234567890987654321");

        u64 = 0;
        assertTrue(Poco::NumberFormatter::format(u64) == "0");
        u64 = 1234567890987654321;
        assertTrue(Poco::NumberFormatter::format(u64) == "1234567890987654321");

        f32 = 0.f;
        assertTrue(Poco::NumberFormatter::format(f32) == "0");
        f32 = 0.000001f;
        assertTrue(Poco::NumberFormatter::format(f32) == "0.000001");
        f32 = 3.141593f;
        assertTrue(Poco::NumberFormatter::format(f32) == "3.141593");
        f32 = -1234567.f;
        assertTrue(Poco::NumberFormatter::format(f32) == "-1.234567e+6");
        f32 = 1234567.f;
        assertTrue(Poco::NumberFormatter::format(f32) == "1.234567e+6");

        d64 = 0.0;
        assertTrue(Poco::NumberFormatter::format(d64) == "0");
        d64 = 0.000000000000001;
        assertTrue(Poco::NumberFormatter::format(d64) == "0.000000000000001");
        d64 = 3.141592653589793;
        assertTrue(Poco::NumberFormatter::format(d64) == "3.141592653589793");
        d64 = -1234567890123456.0;
        assertTrue(Poco::NumberFormatter::format(d64) == "-1.234567890123456e+15");
        d64 = 1234567890123456.0;
        assertTrue(Poco::NumberFormatter::format(d64) == "1.234567890123456e+15");
    }

    {
        assertTrue(Poco::NumberFormatter::format((int8_t)123) == "123");
        assertTrue(Poco::NumberFormatter::format((int8_t)123, 5) == "  123");
        assertTrue(Poco::NumberFormatter::format((int8_t)-123) == "-123");
        assertTrue(Poco::NumberFormatter::format((int8_t)-123, 5) == " -123");

        assertTrue(Poco::NumberFormatter::format((uint8_t)123) == "123");
        assertTrue(Poco::NumberFormatter::format((uint8_t)123, 5) == "  123");

        assertTrue(Poco::NumberFormatter::format((int16_t)123) == "123");
        assertTrue(Poco::NumberFormatter::format((int16_t)123, 5) == "  123");
        assertTrue(Poco::NumberFormatter::format((int16_t)-123) == "-123");
        assertTrue(Poco::NumberFormatter::format((int16_t)-123, 5) == " -123");

        assertTrue(Poco::NumberFormatter::format((uint16_t)123) == "123");
        assertTrue(Poco::NumberFormatter::format((uint16_t)123, 5) == "  123");

        assertTrue(Poco::NumberFormatter::format((int32_t)123) == "123");
        assertTrue(Poco::NumberFormatter::format((int32_t)123, 5) == "  123");
        assertTrue(Poco::NumberFormatter::format((int32_t)-123) == "-123");
        assertTrue(Poco::NumberFormatter::format((int32_t)-123, 5) == " -123");

        assertTrue(Poco::NumberFormatter::format((uint32_t)123) == "123");
        assertTrue(Poco::NumberFormatter::format((uint32_t)123, 5) == "  123");

        assertTrue(Poco::NumberFormatter::format((int64_t)123) == "123");
        assertTrue(Poco::NumberFormatter::format((int64_t)123, 5) == "  123");
        assertTrue(Poco::NumberFormatter::format((int64_t)-123) == "-123");
        assertTrue(Poco::NumberFormatter::format((int64_t)-123, 5) == " -123");

        assertTrue(Poco::NumberFormatter::format((uint64_t)123) == "123");
        assertTrue(Poco::NumberFormatter::format((uint64_t)123, 5) == "  123");

        assertTrue(Poco::NumberFormatter::format(1.23f) == "1.23");
        assertTrue(Poco::NumberFormatter::format(-1.23f) == "-1.23");
        assertTrue(Poco::NumberFormatter::format(1.0f) == "1");
        assertTrue(Poco::NumberFormatter::format(-1.0f) == "-1");
        assertTrue(Poco::NumberFormatter::format(0.1f) == "0.1");
        assertTrue(Poco::NumberFormatter::format(-0.1f) == "-0.1");

        assertTrue(Poco::NumberFormatter::format(1.23) == "1.23");
        assertTrue(Poco::NumberFormatter::format(-1.23) == "-1.23");
        assertTrue(Poco::NumberFormatter::format(1.0) == "1");
        assertTrue(Poco::NumberFormatter::format(-1.0) == "-1");
        assertTrue(Poco::NumberFormatter::format(0.1) == "0.1");
        assertTrue(Poco::NumberFormatter::format(-0.1) == "-0.1");

        assertTrue(Poco::NumberFormatter::format(50.0f, 3) == "50.000");
        assertTrue(Poco::NumberFormatter::format(50.123f, 3) == "50.123");
        assertTrue(Poco::NumberFormatter::format(50.123f, 0) == "50");
        assertTrue(Poco::NumberFormatter::format(50.546f, 0) == "51");
        assertTrue(Poco::NumberFormatter::format(50.546f, 2) == "50.55");

        assertTrue(Poco::NumberFormatter::format(50.0, 3) == "50.000");
        assertTrue(Poco::NumberFormatter::format(50.123, 3) == "50.123");
        assertTrue(Poco::NumberFormatter::format(50.123, 0) == "50");
        assertTrue(Poco::NumberFormatter::format(50.546, 0) == "51");
        assertTrue(Poco::NumberFormatter::format(50.546, 2) == "50.55");
    }

    {
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0x12) == "12");
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0xab) == "AB");
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0x12, 4) == "0012");
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0xab, 4) == "00AB");

        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0x12) == "12");
        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0xab) == "AB");
        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0x12, 4) == "0012");
        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0xab, 4) == "00AB");

        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0x12) == "12");
        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0xab) == "AB");
        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0x12, 4) == "0012");
        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0xab, 4) == "00AB");

        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0x12) == "12");
        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0xab) == "AB");
        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0x12, 4) == "0012");
        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0xab, 4) == "00AB");
    }

    {
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0x12, true) == "0x12");
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0xab, true) == "0xAB");
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0x12, 4, true) == "0x12");
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0xab, 4, true) == "0xAB");
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0x12, 6, true) == "0x0012");
        assertTrue(Poco::NumberFormatter::formatHex((int32_t)0xab, 6, true) == "0x00AB");

        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0x12, true) == "0x12");
        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0xab, true) == "0xAB");
        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0x12, 4, true) == "0x12");
        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0xab, 4, true) == "0xAB");
        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0x12, 6, true) == "0x0012");
        assertTrue(Poco::NumberFormatter::formatHex((uint32_t)0xab, 6, true) == "0x00AB");

        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0x12, true) == "0x12");
        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0xab, true) == "0xAB");
        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0x12, 4, true) == "0x12");
        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0xab, 4, true) == "0xAB");
        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0x12, 6, true) == "0x0012");
        assertTrue(Poco::NumberFormatter::formatHex((int64_t)0xab, 6, true) == "0x00AB");

        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0x12, true) == "0x12");
        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0xab, true) == "0xAB");
        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0x12, 4, true) == "0x12");
        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0xab, 4, true) == "0xAB");
        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0x12, 6, true) == "0x0012");
        assertTrue(Poco::NumberFormatter::formatHex((uint64_t)0xab, 6, true) == "0x00AB");
    }
}

void PocoNumericStringTest::testNumberParser(void)
{
    assertTrue(Poco::NumberParser::parseBool("1") == true);
    assertTrue(Poco::NumberParser::parseBool("0") == false);
    assertTrue(Poco::NumberParser::parseBool("YeS") == true);
    assertTrue(Poco::NumberParser::parseBool("No") == false);
    assertTrue(Poco::NumberParser::parseBool("TRue") == true);
    assertTrue(Poco::NumberParser::parseBool("FAlse") == false);

    assertTrue(Poco::NumberParser::parse("+123") == 123);
    assertTrue(Poco::NumberParser::parse("-123") == -123);
    assertTrue(Poco::NumberParser::parse("0") == 0);
    assertTrue(Poco::NumberParser::parse("000") == 0);
    assertTrue(Poco::NumberParser::parse("0123") == 123);
    assertTrue(Poco::NumberParser::parse("+0123") == 123);
    assertTrue(Poco::NumberParser::parse("-0123") == -123);

    assertTrue(Poco::NumberParser::parseUnsigned("123") == 123);

    assertTrue(Poco::NumberParser::parseHex("12AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex("0x12AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex("0X12AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex("0x000012AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex("0X000012AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex("00") == 0);

    assertTrue(Poco::NumberParser::parse64("+123") == 123);
    assertTrue(Poco::NumberParser::parse64("-123") == -123);
    assertTrue(Poco::NumberParser::parse64("0") == 0);
    assertTrue(Poco::NumberParser::parse64("000") == 0);
    assertTrue(Poco::NumberParser::parse64("0123") == 123);
    assertTrue(Poco::NumberParser::parse64("+0123") == 123);
    assertTrue(Poco::NumberParser::parse64("-0123") == -123);

    assertTrue(Poco::NumberParser::parseUnsigned64("123") == 123);

    assertTrue(Poco::NumberParser::parseHex64("12AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex64("0x12AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex64("0X12AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex64("0x000012AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex64("0X000012AB") == 0x12ab);
    assertTrue(Poco::NumberParser::parseHex64("00") == 0);

    assertTrue(Poco::NumberParser::parseFloat("50.000") == 50.0);
    assertTrue(Poco::NumberParser::parseFloat("+50.000") == 50.0);
    assertTrue(Poco::NumberParser::parseFloat("-50.000") == -50.0);
    assertTrue(Poco::NumberParser::parseFloat("50.123") == 50.123);
    assertTrue(Poco::NumberParser::parseFloat("+50.123") == 50.123);
    assertTrue(Poco::NumberParser::parseFloat("-50.123") == -50.123);
}

CppUnit::Test* PocoNumericStringTest::suite(void)
{
    CppUnit::TestSuite* testSuite = new CppUnit::TestSuite("PocoNumericStringTest");

    CppUnit_addTest(testSuite, PocoNumericStringTest, testPocoStrToNumeric);
    CppUnit_addTest(testSuite, PocoNumericStringTest, testStdStrToNumeric);
    CppUnit_addTest(testSuite, PocoNumericStringTest, testPocoNumericToStr);
    CppUnit_addTest(testSuite, PocoNumericStringTest, testStdNumericToStr);
    CppUnit_addTest(testSuite, PocoNumericStringTest, testNumberFormatter);
    CppUnit_addTest(testSuite, PocoNumericStringTest, testNumberParser);

    return testSuite;
}