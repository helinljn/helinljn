#include "gtest/gtest.h"
#include "util/poco.h"
#include "Poco/NumberFormatter.h"
#include "Poco/NumberParser.h"

GTEST_TEST(PocoNumberFormatterTest, NumberFormatter)
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
        ASSERT_TRUE(Poco::NumberFormatter::format(i8) == "0");
        i8 = 123;
        ASSERT_TRUE(Poco::NumberFormatter::format(i8) == "123");
        i8 = -123;
        ASSERT_TRUE(Poco::NumberFormatter::format(i8) == "-123");

        u8 = 0;
        ASSERT_TRUE(Poco::NumberFormatter::format(u8) == "0");
        u8 = 123;
        ASSERT_TRUE(Poco::NumberFormatter::format(u8) == "123");

        i16 = 0;
        ASSERT_TRUE(Poco::NumberFormatter::format(i16) == "0");
        i16 = 12345;
        ASSERT_TRUE(Poco::NumberFormatter::format(i16) == "12345");
        i16 = -12345;
        ASSERT_TRUE(Poco::NumberFormatter::format(i16) == "-12345");

        u16 = 0;
        ASSERT_TRUE(Poco::NumberFormatter::format(u16) == "0");
        u16 = 12345;
        ASSERT_TRUE(Poco::NumberFormatter::format(u16) == "12345");

        i32 = 0;
        ASSERT_TRUE(Poco::NumberFormatter::format(i32) == "0");
        i32 = 1234567890;
        ASSERT_TRUE(Poco::NumberFormatter::format(i32) == "1234567890");
        i32 = -1234567890;
        ASSERT_TRUE(Poco::NumberFormatter::format(i32) == "-1234567890");

        u32 = 0;
        ASSERT_TRUE(Poco::NumberFormatter::format(u32) == "0");
        u32 = 1234567890;
        ASSERT_TRUE(Poco::NumberFormatter::format(u32) == "1234567890");

        i64 = 0;
        ASSERT_TRUE(Poco::NumberFormatter::format(i64) == "0");
        i64 = 1234567890987654321;
        ASSERT_TRUE(Poco::NumberFormatter::format(i64) == "1234567890987654321");
        i64 = -1234567890987654321;
        ASSERT_TRUE(Poco::NumberFormatter::format(i64) == "-1234567890987654321");

        u64 = 0;
        ASSERT_TRUE(Poco::NumberFormatter::format(u64) == "0");
        u64 = 1234567890987654321;
        ASSERT_TRUE(Poco::NumberFormatter::format(u64) == "1234567890987654321");

        f32 = 0.f;
        ASSERT_TRUE(Poco::NumberFormatter::format(f32) == "0");
        f32 = 0.000001f;
        ASSERT_TRUE(Poco::NumberFormatter::format(f32) == "0.000001");
        f32 = 3.141593f;
        ASSERT_TRUE(Poco::NumberFormatter::format(f32) == "3.141593");
        f32 = -1234567.f;
        ASSERT_TRUE(Poco::NumberFormatter::format(f32) == "-1.234567e+6");
        f32 = 1234567.f;
        ASSERT_TRUE(Poco::NumberFormatter::format(f32) == "1.234567e+6");

        d64 = 0.0;
        ASSERT_TRUE(Poco::NumberFormatter::format(d64) == "0");
        d64 = 0.000000000000001;
        ASSERT_TRUE(Poco::NumberFormatter::format(d64) == "0.000000000000001");
        d64 = 3.141592653589793;
        ASSERT_TRUE(Poco::NumberFormatter::format(d64) == "3.141592653589793");
        d64 = -1234567890123456.0;
        ASSERT_TRUE(Poco::NumberFormatter::format(d64) == "-1.234567890123456e+15");
        d64 = 1234567890123456.0;
        ASSERT_TRUE(Poco::NumberFormatter::format(d64) == "1.234567890123456e+15");
    }

    {
        ASSERT_TRUE(Poco::NumberFormatter::format((int8_t)123) == "123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int8_t)123, 5) == "  123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int8_t)-123) == "-123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int8_t)-123, 5) == " -123");

        ASSERT_TRUE(Poco::NumberFormatter::format((uint8_t)123) == "123");
        ASSERT_TRUE(Poco::NumberFormatter::format((uint8_t)123, 5) == "  123");

        ASSERT_TRUE(Poco::NumberFormatter::format((int16_t)123) == "123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int16_t)123, 5) == "  123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int16_t)-123) == "-123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int16_t)-123, 5) == " -123");

        ASSERT_TRUE(Poco::NumberFormatter::format((uint16_t)123) == "123");
        ASSERT_TRUE(Poco::NumberFormatter::format((uint16_t)123, 5) == "  123");

        ASSERT_TRUE(Poco::NumberFormatter::format((int32_t)123) == "123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int32_t)123, 5) == "  123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int32_t)-123) == "-123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int32_t)-123, 5) == " -123");

        ASSERT_TRUE(Poco::NumberFormatter::format((uint32_t)123) == "123");
        ASSERT_TRUE(Poco::NumberFormatter::format((uint32_t)123, 5) == "  123");

        ASSERT_TRUE(Poco::NumberFormatter::format((int64_t)123) == "123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int64_t)123, 5) == "  123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int64_t)-123) == "-123");
        ASSERT_TRUE(Poco::NumberFormatter::format((int64_t)-123, 5) == " -123");

        ASSERT_TRUE(Poco::NumberFormatter::format((uint64_t)123) == "123");
        ASSERT_TRUE(Poco::NumberFormatter::format((uint64_t)123, 5) == "  123");

        ASSERT_TRUE(Poco::NumberFormatter::format(1.23f) == "1.23");
        ASSERT_TRUE(Poco::NumberFormatter::format(-1.23f) == "-1.23");
        ASSERT_TRUE(Poco::NumberFormatter::format(1.0f) == "1");
        ASSERT_TRUE(Poco::NumberFormatter::format(-1.0f) == "-1");
        ASSERT_TRUE(Poco::NumberFormatter::format(0.1f) == "0.1");
        ASSERT_TRUE(Poco::NumberFormatter::format(-0.1f) == "-0.1");

        ASSERT_TRUE(Poco::NumberFormatter::format(1.23) == "1.23");
        ASSERT_TRUE(Poco::NumberFormatter::format(-1.23) == "-1.23");
        ASSERT_TRUE(Poco::NumberFormatter::format(1.0) == "1");
        ASSERT_TRUE(Poco::NumberFormatter::format(-1.0) == "-1");
        ASSERT_TRUE(Poco::NumberFormatter::format(0.1) == "0.1");
        ASSERT_TRUE(Poco::NumberFormatter::format(-0.1) == "-0.1");

        ASSERT_TRUE(Poco::NumberFormatter::format(50.0f, 3) == "50.000");
        ASSERT_TRUE(Poco::NumberFormatter::format(50.123f, 3) == "50.123");
        ASSERT_TRUE(Poco::NumberFormatter::format(50.123f, 0) == "50");
        ASSERT_TRUE(Poco::NumberFormatter::format(50.546f, 0) == "51");
        ASSERT_TRUE(Poco::NumberFormatter::format(50.546f, 2) == "50.55");

        ASSERT_TRUE(Poco::NumberFormatter::format(50.0, 3) == "50.000");
        ASSERT_TRUE(Poco::NumberFormatter::format(50.123, 3) == "50.123");
        ASSERT_TRUE(Poco::NumberFormatter::format(50.123, 0) == "50");
        ASSERT_TRUE(Poco::NumberFormatter::format(50.546, 0) == "51");
        ASSERT_TRUE(Poco::NumberFormatter::format(50.546, 2) == "50.55");
    }

    {
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0x12) == "12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0xab) == "AB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0x12, 4) == "0012");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0xab, 4) == "00AB");

        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0x12) == "12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0xab) == "AB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0x12, 4) == "0012");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0xab, 4) == "00AB");

        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0x12) == "12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0xab) == "AB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0x12, 4) == "0012");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0xab, 4) == "00AB");

        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0x12) == "12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0xab) == "AB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0x12, 4) == "0012");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0xab, 4) == "00AB");
    }

    {
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0x12, true) == "0x12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0xab, true) == "0xAB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0x12, 4, true) == "0x12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0xab, 4, true) == "0xAB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0x12, 6, true) == "0x0012");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int32_t)0xab, 6, true) == "0x00AB");

        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0x12, true) == "0x12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0xab, true) == "0xAB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0x12, 4, true) == "0x12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0xab, 4, true) == "0xAB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0x12, 6, true) == "0x0012");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint32_t)0xab, 6, true) == "0x00AB");

        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0x12, true) == "0x12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0xab, true) == "0xAB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0x12, 4, true) == "0x12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0xab, 4, true) == "0xAB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0x12, 6, true) == "0x0012");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((int64_t)0xab, 6, true) == "0x00AB");

        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0x12, true) == "0x12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0xab, true) == "0xAB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0x12, 4, true) == "0x12");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0xab, 4, true) == "0xAB");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0x12, 6, true) == "0x0012");
        ASSERT_TRUE(Poco::NumberFormatter::formatHex((uint64_t)0xab, 6, true) == "0x00AB");
    }
}

GTEST_TEST(PocoNumberFormatterTest, NumberParser)
{
    ASSERT_TRUE(Poco::NumberParser::parseBool("1") == true);
    ASSERT_TRUE(Poco::NumberParser::parseBool("0") == false);
    ASSERT_TRUE(Poco::NumberParser::parseBool("YeS") == true);
    ASSERT_TRUE(Poco::NumberParser::parseBool("No") == false);
    ASSERT_TRUE(Poco::NumberParser::parseBool("TRue") == true);
    ASSERT_TRUE(Poco::NumberParser::parseBool("FAlse") == false);

    ASSERT_TRUE(Poco::NumberParser::parse("+123") == 123);
    ASSERT_TRUE(Poco::NumberParser::parse("-123") == -123);
    ASSERT_TRUE(Poco::NumberParser::parse("0") == 0);
    ASSERT_TRUE(Poco::NumberParser::parse("000") == 0);
    ASSERT_TRUE(Poco::NumberParser::parse("0123") == 123);
    ASSERT_TRUE(Poco::NumberParser::parse("+0123") == 123);
    ASSERT_TRUE(Poco::NumberParser::parse("-0123") == -123);

    ASSERT_TRUE(Poco::NumberParser::parseUnsigned("123") == 123);

    ASSERT_TRUE(Poco::NumberParser::parseHex("12AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex("0x12AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex("0X12AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex("0x000012AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex("0X000012AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex("00") == 0);

    ASSERT_TRUE(Poco::NumberParser::parse64("+123") == 123);
    ASSERT_TRUE(Poco::NumberParser::parse64("-123") == -123);
    ASSERT_TRUE(Poco::NumberParser::parse64("0") == 0);
    ASSERT_TRUE(Poco::NumberParser::parse64("000") == 0);
    ASSERT_TRUE(Poco::NumberParser::parse64("0123") == 123);
    ASSERT_TRUE(Poco::NumberParser::parse64("+0123") == 123);
    ASSERT_TRUE(Poco::NumberParser::parse64("-0123") == -123);

    ASSERT_TRUE(Poco::NumberParser::parseUnsigned64("123") == 123);

    ASSERT_TRUE(Poco::NumberParser::parseHex64("12AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex64("0x12AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex64("0X12AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex64("0x000012AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex64("0X000012AB") == 0x12ab);
    ASSERT_TRUE(Poco::NumberParser::parseHex64("00") == 0);

    ASSERT_TRUE(Poco::NumberParser::parseFloat("50.000") == 50.0);
    ASSERT_TRUE(Poco::NumberParser::parseFloat("+50.000") == 50.0);
    ASSERT_TRUE(Poco::NumberParser::parseFloat("-50.000") == -50.0);
    ASSERT_TRUE(Poco::NumberParser::parseFloat("50.123") == 50.123);
    ASSERT_TRUE(Poco::NumberParser::parseFloat("+50.123") == 50.123);
    ASSERT_TRUE(Poco::NumberParser::parseFloat("-50.123") == -50.123);
}