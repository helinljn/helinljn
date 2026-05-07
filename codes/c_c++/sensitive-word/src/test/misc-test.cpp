#include "doctest.h"
#include "core/common.h"
#include "core/base64.h"
#include "core/md5.h"
#include "core/numeric_cast.hpp"
#include "core/timespan.h"
#include "core/timestamp.h"
#include "core/datetime.h"
#include "spdlog/fmt/fmt.h"
#include <thread>
#include <fstream>

TEST_SUITE("Misc")
{
    TEST_CASE("Timespan")
    {
        constexpr core::timespan span1;
        static_assert(span1.days()         == 0);
        static_assert(span1.hours()        == 0);
        static_assert(span1.minutes()      == 0);
        static_assert(span1.seconds()      == 0);
        static_assert(span1.milliseconds() == 0);
        static_assert(span1.microseconds() == 0);
        static_assert(span1.total_days()         == 0);
        static_assert(span1.total_hours()        == 0);
        static_assert(span1.total_minutes()      == 0);
        static_assert(span1.total_seconds()      == 0);
        static_assert(span1.total_milliseconds() == 0);
        static_assert(span1.total_microseconds() == 0);

        constexpr core::timespan span2(6666666666666);
        static_assert(span2.days()         == 77);
        static_assert(span2.hours()        == 3);
        static_assert(span2.minutes()      == 51);
        static_assert(span2.seconds()      == 6);
        static_assert(span2.milliseconds() == 666);
        static_assert(span2.microseconds() == 666);
        static_assert(span2.total_days()         == 77);
        static_assert(span2.total_hours()        == 1851);
        static_assert(span2.total_minutes()      == 111111);
        static_assert(span2.total_seconds()      == 6666666);
        static_assert(span2.total_milliseconds() == 6666666666);
        static_assert(span2.total_microseconds() == 6666666666666);

        constexpr core::timespan span3(77, 3, 51, 6, 666666);
        static_assert(span3.total_microseconds() == 6666666666666);

        constexpr core::timespan span4(std::chrono::microseconds{6666666666666});
        static_assert(span4.days()         == 77);
        static_assert(span4.hours()        == 3);
        static_assert(span4.minutes()      == 51);
        static_assert(span4.seconds()      == 6);
        static_assert(span4.milliseconds() == 666);
        static_assert(span4.microseconds() == 666);
        static_assert(span4.total_days()         == 77);
        static_assert(span4.total_hours()        == 1851);
        static_assert(span4.total_minutes()      == 111111);
        static_assert(span4.total_seconds()      == 6666666);
        static_assert(span4.total_milliseconds() == 6666666666);
        static_assert(span4.total_microseconds() == 6666666666666);

        static_assert(span1 == 0 && span2 == 6666666666666);
        static_assert(span2 == span3 && span3 == span4);
        static_assert(span1 + span2 == span3);
        static_assert(span1 + 6666666666666 == span3);

        core::timespan span5(0, 0, 0, 0, 1000);
        core::timespan span6(0, 0, 0, 0, 1001);
        CHECK(span5 + span6 == 2001);
        CHECK(span5 + 1001  == 2001);
        CHECK(span5 - span6 == -1);
        CHECK(span5 - 1001  == -1);

        span5 += span6;
        CHECK(span5 == 2001);

        span5 -= span6;
        CHECK(span5 == 1000);

        span5 += 1001;
        CHECK(span5 == 2001);

        span5 -= 1001;
        CHECK(span5 == 1000);
    }

    TEST_CASE("Timestamp")
    {
        core::timestamp ts1(100 * core::timestamp::resolution);
        core::timestamp ts2(101 * core::timestamp::resolution);
        CHECK(ts1 + core::timestamp::resolution == ts2);
        CHECK(ts2 - core::timestamp::resolution == ts1);
        CHECK(ts1 + core::timespan(core::timestamp::resolution) == ts2);
        CHECK(ts2 - core::timespan(core::timestamp::resolution) == ts1);

        ts1 += core::timestamp::resolution;
        CHECK(ts1 == ts2);

        ts1 -= core::timestamp::resolution;
        CHECK(ts1 == ts2 - core::timestamp::resolution);

        ts1 += core::timespan(core::timestamp::resolution);
        CHECK(ts1 == ts2);

        ts1 -= core::timespan(core::timestamp::resolution);
        CHECK(ts1 == ts2 - core::timestamp::resolution);
    }

    TEST_CASE("DateTime")
    {
        {
            // default constructor
            core::datetime dt1;

            // copy constructor from DateTimeEx
            core::datetime dt2(dt1);
            CHECK(dt1 == dt2);
            CHECK(dt1.to_timestamp() == dt2.to_timestamp());

            // copy constructor from Timestamp
            core::datetime dt3(dt1.to_timestamp());
            CHECK(dt1 == dt3);
            CHECK(dt1.to_timestamp() == dt3.to_timestamp());

            // copy constructor from tm struct
            core::datetime dt4(dt1.to_tm());
            CHECK(dt1.year() == dt4.year());
            CHECK(dt1.month() == dt4.month());
            CHECK(dt1.day() == dt4.day());
            CHECK(dt1.day_of_week() == dt4.day_of_week());
            CHECK(dt1.day_of_year() == dt4.day_of_year());
            CHECK(dt1.hour() == dt4.hour());
            CHECK(dt1.minute() == dt4.minute());
            CHECK(dt1.second() == dt4.second());

            // copy constructor from the given local date and time
            core::datetime dt5(dt1.year(), dt1.month(), dt1.day(), dt1.hour(), dt1.minute(), dt1.second(), dt1.millisecond(), dt1.microsecond());
            CHECK(dt1 == dt5);
            CHECK(dt1.to_timestamp() == dt5.to_timestamp());

            core::datetime dt6;

            // assigns a datetime
            dt6 += core::timespan(std::chrono::hours(1));
            dt6  = dt1;
            CHECK(dt1 == dt6);
            CHECK(dt1.to_timestamp() == dt6.to_timestamp());

            // assigns a timestamp
            dt6 += core::timespan(std::chrono::hours(1));
            dt6  = dt1.to_timestamp();
            CHECK(dt1 == dt6);
            CHECK(dt1.to_timestamp() == dt6.to_timestamp());

            // assigns a tm struct
            dt6 += core::timespan(std::chrono::hours(1));
            dt6  = dt1.to_tm();
            CHECK(dt1.year() == dt6.year());
            CHECK(dt1.month() == dt6.month());
            CHECK(dt1.day() == dt6.day());
            CHECK(dt1.day_of_week() == dt6.day_of_week());
            CHECK(dt1.day_of_year() == dt6.day_of_year());
            CHECK(dt1.hour() == dt6.hour());
            CHECK(dt1.minute() == dt6.minute());
            CHECK(dt1.second() == dt6.second());

            // assigns a Timestamp
            dt6 += core::timespan(std::chrono::hours(1));
            dt6.assign(dt1.to_timestamp());
            CHECK(dt1 == dt6);
            CHECK(dt1.to_timestamp() == dt6.to_timestamp());

            // assigns a tm struct
            dt6 += core::timespan(std::chrono::hours(1));
            dt6.assign(dt1.to_tm());
            CHECK(dt1.year() == dt6.year());
            CHECK(dt1.month() == dt6.month());
            CHECK(dt1.day() == dt6.day());
            CHECK(dt1.day_of_week() == dt6.day_of_week());
            CHECK(dt1.day_of_year() == dt6.day_of_year());
            CHECK(dt1.hour() == dt6.hour());
            CHECK(dt1.minute() == dt6.minute());
            CHECK(dt1.second() == dt6.second());

            // assigns a local date and time
            dt6 += core::timespan(std::chrono::hours(1));
            dt6.assign(dt1.year(), dt1.month(), dt1.day(), dt1.hour(), dt1.minute(), dt1.second(), dt1.millisecond(), dt1.microsecond());
            CHECK(dt1 == dt6);
            CHECK(dt1.to_timestamp() == dt6.to_timestamp());

            dt6 += core::timespan(std::chrono::hours(1));
            CHECK(dt1 != dt6);
            CHECK(dt1 <  dt6);
            CHECK(dt1 <= dt6);
            CHECK(dt6 >  dt1);
            CHECK(dt6 >= dt1);

            CHECK(dt6 == dt1 + core::timespan(std::chrono::hours(1)));
            CHECK(dt1 == dt6 - core::timespan(std::chrono::hours(1)));

            core::timestamp ts(1314521000 * core::timestamp::resolution);
            dt5 = ts;
            dt6 = ts + core::timespan(std::chrono::seconds(5211314));

            const auto diff = dt6 - dt5;
            CHECK(diff.total_seconds() == 5211314);
        }

        {
            core::datetime dt(2011, 8, 28, 16, 43, 20);

            CHECK(dt.format("").empty());

            CHECK(dt.format("%Y-%m-%d") == "2011-08-28");
            CHECK(dt.format("%H:%M:%S") == "16:43:20");
            CHECK(dt.format("%Y-%m-%d %H:%M:%S") == "2011-08-28 16:43:20");

            core::datetime dt_ms(2011, 8, 28, 16, 43, 20, 123);
            CHECK(dt_ms.format("%Y-%m-%d %H:%M:%S.{ms}") == "2011-08-28 16:43:20.123");

            core::datetime dt_us(2011, 8, 28, 16, 43, 20, 123, 456);
            CHECK(dt_us.format("%Y-%m-%d %H:%M:%S.{ms}{us}") == "2011-08-28 16:43:20.123456");

            core::datetime dt_ms_zero(2011, 8, 28, 16, 43, 20, 5);
            CHECK(dt_ms_zero.format("{ms}") == "005");

            core::datetime dt_us_zero(2011, 8, 28, 16, 43, 20, 0, 7);
            CHECK(dt_us_zero.format("{us}") == "007");

            CHECK(dt_us.format("{ms}") == "123");
            CHECK(dt_us.format("{us}") == "456");

            core::datetime now;
            CHECK(!now.format("%Y-%m-%d %H:%M:%S").empty());
        }

        {
            // 1970-01-01 08:00:00 Thursday
            core::datetime dt(core::timestamp(0));
            CHECK(dt.year() == 1970);
            CHECK(dt.month() == 1);
            CHECK(dt.day() == 1);
            CHECK(dt.hour() == 8);
            CHECK(dt.minute() == 0);
            CHECK(dt.second() == 0);
            CHECK(dt.millisecond() == 0);
            CHECK(dt.microsecond() == 0);
            CHECK(dt.day_of_week() == 4);
            CHECK(dt.to_timestamp().epoch_time() == 0);

            // 1970-01-01 08:00:00 Thursday
            core::datetime dtex(dt);
            CHECK(dtex.year() == 1970);
            CHECK(dtex.month() == 1);
            CHECK(dtex.day() == 1);
            CHECK(dtex.hour() == 8);
            CHECK(dtex.minute() == 0);
            CHECK(dtex.second() == 0);
            CHECK(dtex.millisecond() == 0);
            CHECK(dtex.microsecond() == 0);
            CHECK(dtex.day_of_week() == 4);
            CHECK(dtex.to_timestamp().epoch_time() == 0);

            dtex += core::timespan(std::chrono::hours(1));
            dtex  = dt;
            CHECK(dtex.year() == 1970);
            CHECK(dtex.month() == 1);
            CHECK(dtex.day() == 1);
            CHECK(dtex.hour() == 8);
            CHECK(dtex.minute() == 0);
            CHECK(dtex.second() == 0);
            CHECK(dtex.millisecond() == 0);
            CHECK(dtex.microsecond() == 0);
            CHECK(dtex.day_of_week() == 4);
            CHECK(dtex.to_timestamp().epoch_time() == 0);

            dtex += core::timespan(std::chrono::hours(1));
            dtex  = dt;
            CHECK(dtex.year() == 1970);
            CHECK(dtex.month() == 1);
            CHECK(dtex.day() == 1);
            CHECK(dtex.hour() == 8);
            CHECK(dtex.minute() == 0);
            CHECK(dtex.second() == 0);
            CHECK(dtex.millisecond() == 0);
            CHECK(dtex.microsecond() == 0);
            CHECK(dtex.day_of_week() == 4);
            CHECK(dtex.to_timestamp().epoch_time() == 0);

            CHECK(dtex - dt == 0);
        }

        // 1970-01-01 08:00:00 Thursday
        core::datetime dt(core::timestamp(0));
        CHECK(dt.year() == 1970);
        CHECK(dt.month() == 1);
        CHECK(dt.day() == 1);
        CHECK(dt.hour() == 8);
        CHECK(dt.minute() == 0);
        CHECK(dt.second() == 0);
        CHECK(dt.millisecond() == 0);
        CHECK(dt.microsecond() == 0);
        CHECK(dt.day_of_week() == 4);
        CHECK(dt.to_timestamp().epoch_time() == 0);

        tm tmStruct = dt.to_tm();
        CHECK(tmStruct.tm_year == 70);
        CHECK(tmStruct.tm_mon == 0);
        CHECK(tmStruct.tm_mday == 1);
        CHECK(tmStruct.tm_hour == 8);
        CHECK(tmStruct.tm_min == 0);
        CHECK(tmStruct.tm_sec == 0);

        dt = tmStruct;
        CHECK(dt.year() == 1970);
        CHECK(dt.month() == 1);
        CHECK(dt.day() == 1);
        CHECK(dt.hour() == 8);
        CHECK(dt.minute() == 0);
        CHECK(dt.second() == 0);
        CHECK(dt.millisecond() == 0);
        CHECK(dt.microsecond() == 0);
        CHECK(dt.day_of_week() == 4);
        CHECK(dt.to_timestamp().epoch_time() == 0);

        // 2001-09-09 09:46:40 Sunday
        dt = core::timestamp(1000000000 * core::timestamp::resolution);
        CHECK(dt.year() == 2001);
        CHECK(dt.month() == 9);
        CHECK(dt.day() == 9);
        CHECK(dt.hour() == 9);
        CHECK(dt.minute() == 46);
        CHECK(dt.second() == 40);
        CHECK(dt.millisecond() == 0);
        CHECK(dt.microsecond() == 0);
        CHECK(dt.day_of_week() == 0);
        CHECK(dt.to_timestamp().epoch_time() == 1000000000);

        // 3000-12-31 23:59:59 Friday
        dt = core::datetime(3000, 12, 31, 23, 59, 59);
        CHECK(dt.year() == 3000);
        CHECK(dt.month() == 12);
        CHECK(dt.day() == 31);
        CHECK(dt.hour() == 23);
        CHECK(dt.minute() == 59);
        CHECK(dt.second() == 59);
        CHECK(dt.millisecond() == 0);
        CHECK(dt.microsecond() == 0);
        CHECK(dt.day_of_week() == 3);
        CHECK(dt.to_timestamp().epoch_time() == time_t{32535187199});

        // 2011-08-28 16:43:20 Sunday
        dt = core::datetime(2011, 8, 28, 16, 43, 20);
        CHECK(dt.year() == 2011);
        CHECK(dt.month() == 8);
        CHECK(dt.day() == 28);
        CHECK(dt.hour() == 16);
        CHECK(dt.minute() == 43);
        CHECK(dt.second() == 20);
        CHECK(dt.millisecond() == 0);
        CHECK(dt.microsecond() == 0);
        CHECK(dt.day_of_week() == 0);
        CHECK(dt.to_timestamp().epoch_time() == 1314521000);

        core::datetime dt1 = dt + core::timespan(std::chrono::hours(1));
        CHECK(dt1.year() == 2011);
        CHECK(dt1.month() == 8);
        CHECK(dt1.day() == 28);
        CHECK(dt1.hour() == 17);
        CHECK(dt1.minute() == 43);
        CHECK(dt1.second() == 20);
        CHECK(dt1.millisecond() == 0);
        CHECK(dt1.microsecond() == 0);
        CHECK(dt1.day_of_week() == 0);
        CHECK(dt1.to_timestamp().epoch_time() == 1314524600);

        core::datetime dt2 = dt1 - core::timespan(std::chrono::hours(1));
        CHECK(dt2.year() == 2011);
        CHECK(dt2.month() == 8);
        CHECK(dt2.day() == 28);
        CHECK(dt2.hour() == 16);
        CHECK(dt2.minute() == 43);
        CHECK(dt2.second() == 20);
        CHECK(dt2.millisecond() == 0);
        CHECK(dt2.microsecond() == 0);
        CHECK(dt2.day_of_week() == 0);
        CHECK(dt2.to_timestamp().epoch_time() == 1314521000);

        const auto span = dt1 - dt2;
        CHECK(span.days() == 0);
        CHECK(span.hours() == 1);
        CHECK(span.total_hours() == 1);
        CHECK(span.minutes() == 0);
        CHECK(span.total_minutes() == 60);
        CHECK(span.seconds() == 0);
        CHECK(span.total_seconds() == 3600);
        CHECK(span.milliseconds() == 0);
        CHECK(span.total_milliseconds() == 3600000);
    }

    TEST_CASE("IsGBK")
    {
        // 测试空字符串
        CHECK(!core::is_gbk(""));
        CHECK(!core::is_gbk(std::string()));

        // 测试ASCII字符串
        CHECK(core::is_gbk(std::string_view("hello", 5)));
        CHECK(core::is_gbk("hello"));

        // 测试GBK字符串（中文）
        // 注意：这里需要确保字符串是GBK编码
        // 由于编码环境可能不同，这里使用十六进制表示
        const uint8_t gbk_chinese[] = {0xD6, 0xD0, 0xCE, 0xC4}; // "中文"的GBK编码
        CHECK(core::is_gbk(std::string_view(reinterpret_cast<const char*>(gbk_chinese), sizeof(gbk_chinese))));

        // 测试混合ASCII和GBK
        const uint8_t mixed[] = {0x68, 0x65, 0x6C, 0x6C, 0x6F, 0xD6, 0xD0, 0xCE, 0xC4}; // "hello中文"
        CHECK(core::is_gbk(std::string_view(reinterpret_cast<const char*>(mixed), sizeof(mixed))));

        // 测试无效的GBK编码
        const uint8_t invalid_gbk1[] = {0x80}; // 无效的首字节
        CHECK(!core::is_gbk(std::string_view(reinterpret_cast<const char*>(invalid_gbk1), sizeof(invalid_gbk1))));

        const uint8_t invalid_gbk2[] = {0x81, 0x3F}; // 无效的尾字节
        CHECK(!core::is_gbk(std::string_view(reinterpret_cast<const char*>(invalid_gbk2), sizeof(invalid_gbk2))));

        const uint8_t invalid_gbk3[] = {0x81}; // 不完整的GBK编码
        CHECK(!core::is_gbk(std::string_view(reinterpret_cast<const char*>(invalid_gbk3), sizeof(invalid_gbk3))));
    }

    TEST_CASE("IsUTF8")
    {
        // 测试空字符串
        CHECK(!core::is_utf8(""));
        CHECK(!core::is_utf8(std::string()));

        // 测试ASCII字符串
        CHECK(core::is_utf8(std::string_view("hello", 5)));
        CHECK(core::is_utf8("hello"));

        // 测试UTF-8字符串（中文）
        const uint8_t utf8_chinese[] = {0xE4, 0xB8, 0xAD, 0xE6, 0x96, 0x87}; // "中文"的UTF-8编码
        CHECK(core::is_utf8(std::string_view(reinterpret_cast<const char*>(utf8_chinese), sizeof(utf8_chinese))));

        // 测试混合ASCII和UTF-8
        const uint8_t mixed[] = {0x68, 0x65, 0x6C, 0x6C, 0x6F, 0xE4, 0xB8, 0xAD, 0xE6, 0x96, 0x87}; // "hello中文"
        CHECK(core::is_utf8(std::string_view(reinterpret_cast<const char*>(mixed), sizeof(mixed))));

        // 测试多字节UTF-8字符
        const uint8_t multi_byte[] = {0xF0, 0x9F, 0x98, 0x83}; // 😃 表情符号的UTF-8编码
        CHECK(core::is_utf8(std::string_view(reinterpret_cast<const char*>(multi_byte), sizeof(multi_byte))));

        // 测试无效的UTF-8编码
        const uint8_t invalid_utf8_1[] = {0xC0}; // 不完整的双字节编码
        CHECK(!core::is_utf8(std::string_view(reinterpret_cast<const char*>(invalid_utf8_1), sizeof(invalid_utf8_1))));

        const uint8_t invalid_utf8_2[] = {0xC0, 0x80}; // 过度编码的ASCII
        CHECK(!core::is_utf8(std::string_view(reinterpret_cast<const char*>(invalid_utf8_2), sizeof(invalid_utf8_2))));

        const uint8_t invalid_utf8_3[] = {0xE0, 0x80, 0x80}; // 不完整的三字节编码
        CHECK(!core::is_utf8(std::string_view(reinterpret_cast<const char*>(invalid_utf8_3), sizeof(invalid_utf8_3))));

        const uint8_t invalid_utf8_4[] = {0xF8, 0x80, 0x80, 0x80, 0x80}; // 超过4字节的编码
        CHECK(!core::is_utf8(std::string_view(reinterpret_cast<const char*>(invalid_utf8_4), sizeof(invalid_utf8_4))));
    }

    TEST_CASE("MemoryToHexString")
    {
        // 测试空数据
        char buf[10];
        CHECK(!core::memory_to_hex_string(nullptr, 0, buf, sizeof(buf)));

        // 测试基本功能
        uint8_t data[] = {0x12, 0x34, 0xAB, 0xCD};
        char hex_buf[20];
        CHECK(core::memory_to_hex_string(data, sizeof(data), hex_buf, sizeof(hex_buf)));
        CHECK(strcmp(hex_buf, "1234ABCD") == 0);

        // 测试小写
        CHECK(core::memory_to_hex_string(data, sizeof(data), hex_buf, sizeof(hex_buf), false));
        CHECK(strcmp(hex_buf, "1234abcd") == 0);

        // 测试缓冲区大小不足
        CHECK(!core::memory_to_hex_string(data, sizeof(data), hex_buf, 8));

        // 测试单字节
        uint8_t single_byte = 0x5A;
        CHECK(core::memory_to_hex_string(&single_byte, 1, hex_buf, sizeof(hex_buf)));
        CHECK(strcmp(hex_buf, "5A") == 0);
    }

    TEST_CASE("HexStringToMemory")
    {
        // 测试空字符串
        uint8_t buf[10];
        CHECK(core::hex_string_to_memory("", buf, sizeof(buf)));

        // 测试基本功能
        CHECK(core::hex_string_to_memory("1234ABCD", buf, sizeof(buf)));
        CHECK(buf[0] == 0x12);
        CHECK(buf[1] == 0x34);
        CHECK(buf[2] == 0xAB);
        CHECK(buf[3] == 0xCD);

        // 测试小写
        CHECK(core::hex_string_to_memory("1234abcd", buf, sizeof(buf)));
        CHECK(buf[0] == 0x12);
        CHECK(buf[1] == 0x34);
        CHECK(buf[2] == 0xAB);
        CHECK(buf[3] == 0xCD);

        // 测试缓冲区大小不足
        CHECK(!core::hex_string_to_memory("1234ABCD", buf, 3));

        // 测试无效的十六进制字符
        CHECK(!core::hex_string_to_memory("1234XY", buf, sizeof(buf)));

        // 测试奇数长度
        CHECK(!core::hex_string_to_memory("123", buf, sizeof(buf)));

        // 测试单字节
        CHECK(core::hex_string_to_memory("5A", buf, sizeof(buf)));
        CHECK(buf[0] == 0x5A);
    }

    TEST_CASE("HexStringConversion")
    {
        // 测试基本功能
        {
            // uint8_t
            uint8_t u8_value = 0xAB;
            std::string hex_str;
            CHECK(core::to_hex_string(u8_value, hex_str));
            CHECK(hex_str == "AB");

            uint8_t u8_result = 0;
            CHECK(core::from_hex_string(hex_str, u8_result));
            CHECK(u8_result == u8_value);
        }

        // int
        {
            int value = 0x12345678;
            std::string hex_str;
            CHECK(core::to_hex_string(value, hex_str));

            int result = 0;
            CHECK(core::from_hex_string(hex_str, result));
            CHECK(result == value);
        }

        // float
        {
            float value = 123.456f;
            std::string hex_str;
            CHECK(core::to_hex_string(value, hex_str));

            float result = 0.f;
            CHECK(core::from_hex_string(hex_str, result));
            // 测试精度误差
            CHECK(std::abs(result - value) < 0.0001f);
        }

        // double
        {
            double value = 123.456;
            std::string hex_str;
            CHECK(core::to_hex_string(value, hex_str));

            double result = 0.0;
            CHECK(core::from_hex_string(hex_str, result));
            // 测试精度误差
            CHECK(std::abs(result - value) < 0.0001);
        }

        // 测试结构体转换
        {
            struct TestStruct {
                int a;
                char b;
                float c;
            } value = {123, 'A', 45.67f};

            std::string hex_str;
            CHECK(core::to_hex_string(value, hex_str));

            TestStruct result{};
            CHECK(core::from_hex_string(hex_str, result));
            CHECK(result.a == value.a);
            CHECK(result.b == value.b);
            CHECK(std::abs(result.c - value.c) < 0.0001f);
        }

        // 测试小写转换
        {
            int value = 0x12345678;
            std::string hex_str;
            CHECK(core::to_hex_string(value, hex_str, false));

            int result = 0;
            CHECK(core::from_hex_string(hex_str, result));
            CHECK(result == value);
        }

        // 测试空指针
        {
            char buf[10] = {0};
            CHECK(!core::memory_to_hex_string(nullptr, 0, buf, sizeof(buf)));
        }

        // 测试无效的十六进制字符串
        {
            int value = 0;
            CHECK(!core::from_hex_string("123", value));
        }
    }

    TEST_CASE("SystemInfo")
    {
        // 测试 get_free_memory 函数
        uint32_t free_memory = core::get_free_memory();
        CHECK(free_memory > 0);

        // 测试 get_total_memory 函数
        uint32_t total_memory = core::get_total_memory();
        CHECK(total_memory > 0);
        CHECK(total_memory >= free_memory);

        // 测试 get_process_id 函数
        uint32_t process_id = core::get_process_id();
        CHECK(process_id > 0);

        // 测试 get_thread_id 函数
        uint32_t thread_id = core::get_thread_id();
        CHECK(thread_id > 0);

        // 测试 get_cpu_logic_count 函数
        uint32_t cpu_count = core::get_cpu_logic_count();
        CHECK(cpu_count > 0);

        fmt::print("FreeMemory: {}, TotalMemory: {}, ProcessID: {}, ThreadID: {}, CPUCount: {}\n",
            free_memory, total_memory, process_id, thread_id, cpu_count);
    }

    TEST_CASE("ProgramRunningTime")
    {
        // 测试 get_program_running_time 函数
        uint64_t start_time = core::get_program_running_time();
        CHECK(start_time >= 0);

        // 等待一段时间后再次获取
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        uint64_t end_time = core::get_program_running_time();
        CHECK(end_time >= start_time);
        CHECK(end_time - start_time >= 90); // 允许一定的误差
    }

    TEST_CASE("EncodingConversion")
    {
        // 测试空字符串
        CHECK(core::gbk_to_utf8("").empty());
        CHECK(core::utf8_to_gbk("").empty());

        // 测试 ASCII 字符串
        std::string ascii_str = "hello world";
        CHECK(core::gbk_to_utf8(ascii_str) == ascii_str);
        CHECK(core::utf8_to_gbk(ascii_str) == ascii_str);

        // 测试中文转换（注意：这里的测试依赖于系统编码环境）
        // 测试 GBK 到 UTF-8 转换
        // std::string gbk_chinese  = "中文";
        // std::string utf8_chinese = core::gbk_to_utf8(gbk_chinese);
        // CHECK(!utf8_chinese.empty());

        // 测试 UTF-8 到 GBK 转换
        std::string utf8_chinese   = "中文";
        std::string converted_back = core::utf8_to_gbk(utf8_chinese);
        CHECK(!converted_back.empty());

        // 测试转换的可逆性（在支持 GBK 的环境中应该成立）
        // 注意：由于编码环境的差异，这个测试可能在某些环境下失败
        // CHECK(converted_back == gbk_chinese);

        // 测试异常处理
        try
        {
            // 测试无效输入
            core::gbk_to_utf8("\xFF\xFF"); // 无效的 GBK 编码
            core::utf8_to_gbk("\xFF\xFF"); // 无效的 UTF-8 编码
        }
        catch (...)
        {
            // 不应该抛出异常，应该返回空字符串
            CHECK(false);
        }
    }

    TEST_CASE("Random")
    {
        // 测试 random_uint32 函数
        for (int i = 0; i < 100; ++i)
        {
            uint32_t value = core::random_uint32();
            CHECK(value <= UINT32_MAX);
        }

        // 测试 random_uint64 函数
        for (int i = 0; i < 100; ++i)
        {
            uint64_t value = core::random_uint64();
            CHECK(value <= UINT64_MAX);
        }

        // 测试 random_float 函数
        for (int i = 0; i < 100; ++i)
        {
            float value = core::random_float();
            CHECK(value >= 0.0f);
            CHECK(value <= 1.0f);
        }

        // 测试 random_double 函数
        for (int i = 0; i < 100; ++i)
        {
            double value = core::random_double();
            CHECK(value >= 0.0);
            CHECK(value <= 1.0);
        }

        // 测试 random_range 函数
        // 测试有效输入
        int32_t upper_bound = 100;
        for (int i = 0; i < 1000; ++i)
        {
            int32_t value = core::random_range(upper_bound);
            CHECK(value >= 0);
            CHECK(value < upper_bound);
        }

        // 测试无效输入
        CHECK(core::random_range(0) == 0);
        CHECK(core::random_range(-10) == 0);
    }

    TEST_CASE("EnvironmentVariables")
    {
        // 测试设置环境变量
        CHECK(core::env_set("TEST_ENV_VAR", "test_value"));

        // 测试判断环境变量是否存在
        CHECK(core::env_has("TEST_ENV_VAR"));

        // 测试获取环境变量的值
        CHECK(core::env_get("TEST_ENV_VAR") == "test_value");

        // 测试获取不存在的环境变量
        CHECK(!core::env_has("NON_EXISTENT_ENV_VAR"));
        CHECK(core::env_get("NON_EXISTENT_ENV_VAR").empty());

        // 测试空环境变量名
        CHECK(!core::env_has(""));
        CHECK(core::env_get("").empty());
        CHECK(!core::env_set("", "value"));
    }

    TEST_CASE("PathOperations")
    {
        // 测试 exepath 函数
        char exepath_buf[2048];
        uint32_t exepath_len = sizeof(exepath_buf);
        CHECK(core::exepath(exepath_buf, &exepath_len));
        CHECK(exepath_len > 0);

        std::string exepath_str = core::exepath();
        CHECK(!exepath_str.empty());

        // 测试 exedir 函数
        char exedir_buf[2048];
        uint32_t exedir_len = sizeof(exedir_buf);
        CHECK(core::exedir(exedir_buf, &exedir_len));
        CHECK(exedir_len > 0);

        std::string exedir_str = core::exedir();
        CHECK(!exedir_str.empty());

        fmt::print("exepath: {}, exedir: {}\n", exepath_str, exedir_str);
    }

    TEST_CASE("StringOperations")
    {
        // 测试 trim 函数
        CHECK(core::trim("  hello world  ") == "hello world");
        CHECK(core::trim("hello world") == "hello world");
        CHECK(core::trim("   ") == "");
        CHECK(core::trim("") == "");

        // 测试 ltrim 函数
        CHECK(core::ltrim("  hello world  ") == "hello world  ");
        CHECK(core::ltrim("hello world") == "hello world");
        CHECK(core::ltrim("   ") == "");
        CHECK(core::ltrim("") == "");

        // 测试 rtrim 函数
        CHECK(core::rtrim("  hello world  ") == "  hello world");
        CHECK(core::rtrim("hello world") == "hello world");
        CHECK(core::rtrim("   ") == "");
        CHECK(core::rtrim("") == "");

        // 测试 starts_with 函数
        CHECK(core::starts_with("hello world", "hello"));
        CHECK(!core::starts_with("hello world", "world"));
        CHECK(core::starts_with("hello", ""));
        CHECK(!core::starts_with("", "hello"));

        // 测试 ends_with 函数
        CHECK(core::ends_with("hello world", "world"));
        CHECK(!core::ends_with("hello world", "hello"));
        CHECK(core::ends_with("hello", ""));
        CHECK(!core::ends_with("", "hello"));

        // 测试 contains 函数
        CHECK(core::contains("hello world", "world"));
        CHECK(core::contains("hello world", "hello"));
        CHECK(core::contains("hello world", "lo wo"));
        CHECK(!core::contains("hello world", "test"));
        CHECK(!core::contains("hello", "hello world"));
        CHECK(core::contains("hello", ""));
        CHECK(!core::contains("", "hello"));

        // 测试 to_upper 函数
        CHECK(core::to_upper("hello world") == "HELLO WORLD");
        CHECK(core::to_upper("HELLO WORLD") == "HELLO WORLD");
        CHECK(core::to_upper("") == "");

        // 测试 to_lower 函数
        CHECK(core::to_lower("HELLO WORLD") == "hello world");
        CHECK(core::to_lower("hello world") == "hello world");
        CHECK(core::to_lower("") == "");

        // 测试 replace 函数
        CHECK(core::replace("hello world", "world", "test") == "hello test");
        CHECK(core::replace("hello hello", "hello", "hi") == "hi hi");
        CHECK(core::replace("hello", "", "test") == "hello");
        CHECK(core::replace("", "hello", "test") == "");

        // 测试 join 函数
        std::vector<std::string> parts = {"hello", "world", "test"};
        CHECK(core::join(parts, ", ") == "hello, world, test");
        CHECK(core::join(parts, "") == "helloworldtest");
        CHECK(core::join({}, ", ") == "");
        parts = {"hello"};
        CHECK(core::join(parts, ", ") == "hello");

        // 测试 concat 函数
        CHECK(core::concat("hello", "world") == "helloworld");
        CHECK(core::concat("", "world") == "world");
        CHECK(core::concat("hello", "") == "hello");
        CHECK(core::concat("", "") == "");
        CHECK(core::concat("hello ", "world") == "hello world");

        // 测试 pad_left 函数
        CHECK(core::pad_left("hello", 10, '*') == "*****hello");
        CHECK(core::pad_left("hello", 5, '*') == "hello");
        CHECK(core::pad_left("hello", 3, '*') == "hello");
        CHECK(core::pad_left("", 5, '*') == "*****");
        CHECK(core::pad_left("hello", 10, '0') == "00000hello");

        // 测试 pad_right 函数
        CHECK(core::pad_right("hello", 10, '*') == "hello*****");
        CHECK(core::pad_right("hello", 5, '*') == "hello");
        CHECK(core::pad_right("hello", 3, '*') == "hello");
        CHECK(core::pad_right("", 5, '*') == "*****");
        CHECK(core::pad_right("hello", 10, '0') == "hello00000");

        // 测试 center 函数
        CHECK(core::center("hello", 10, '*') == "**hello***");
        CHECK(core::center("hello", 11, '*') == "***hello***");
        CHECK(core::center("hello", 5, '*') == "hello");
        CHECK(core::center("hello", 3, '*') == "hello");
        CHECK(core::center("", 5, '*') == "*****");
        CHECK(core::center("hello", 10, '0') == "00hello000");

        // 测试 is_blank 函数
        CHECK(core::is_blank(""));
        CHECK(core::is_blank("   "));
        CHECK(core::is_blank("\t\n\r "));
        CHECK(!core::is_blank("hello"));
        CHECK(!core::is_blank(" hello "));

        // 测试 is_digit 函数
        CHECK(core::is_digit("123"));
        CHECK(core::is_digit("0"));
        CHECK(!core::is_digit(""));
        CHECK(!core::is_digit("12a"));
        CHECK(!core::is_digit("abc"));

        // 测试 is_alpha 函数
        CHECK(core::is_alpha("abc"));
        CHECK(core::is_alpha("ABC"));
        CHECK(core::is_alpha("abcABC"));
        CHECK(!core::is_alpha(""));
        CHECK(!core::is_alpha("abc123"));
        CHECK(!core::is_alpha("123"));

        // 测试 is_alnum 函数
        CHECK(core::is_alnum("abc123"));
        CHECK(core::is_alnum("ABC123"));
        CHECK(core::is_alnum("abcABC123"));
        CHECK(!core::is_alnum(""));
        CHECK(!core::is_alnum("abc 123"));
        CHECK(!core::is_alnum("abc!@#"));

        // 测试 is_hexdigit 函数
        CHECK(core::is_hexdigit("1aF"));
        CHECK(core::is_hexdigit("ABCDEF0123456789"));
        CHECK(core::is_hexdigit("abcdef"));
        CHECK(!core::is_hexdigit(""));
        CHECK(!core::is_hexdigit("1g"));
        CHECK(!core::is_hexdigit("xyz"));

        // 测试 is_number 函数
        CHECK(core::is_number("123"));
        CHECK(core::is_number("-123"));
        CHECK(core::is_number("+456"));
        CHECK(core::is_number("3.14"));
        CHECK(core::is_number("-0.5"));
        CHECK(core::is_number("0.0"));
        CHECK(!core::is_number(""));
        CHECK(!core::is_number("."));
        CHECK(!core::is_number("+."));
        CHECK(!core::is_number("-."));
        CHECK(!core::is_number("+"));
        CHECK(!core::is_number("-"));
        CHECK(!core::is_number("12.34.56"));
        CHECK(!core::is_number("12a"));
        CHECK(!core::is_number("abc"));

        // 测试 stringcmp 函数
        CHECK(core::stringcmp("abc", "abc") == 0);
        CHECK(core::stringcmp("abc", "abd") == -1);
        CHECK(core::stringcmp("Abc", "aBd") == -1);
        CHECK(core::stringcmp("abd", "abc") == 1);
        CHECK(core::stringcmp("ab", "ABC") == 1);
        CHECK(core::stringcmp("ABC", "ab") == -1);
        CHECK(core::stringcmp("", "") == 0);

        // 测试 stringicmp 函数
        CHECK(core::stringicmp("ABC", "abc") == 0);
        CHECK(core::stringicmp("abc", "abd") == -1);
        CHECK(core::stringicmp("Abc", "aBd") == -1);
        CHECK(core::stringicmp("abd", "abc") == 1);
        CHECK(core::stringicmp("ab", "ABC") == -1);
        CHECK(core::stringicmp("ABC", "ab") == 1);
        CHECK(core::stringicmp("", "") == 0);

        // 测试 count_str 函数
        CHECK(core::count_str("hello world", "l") == 3);
        CHECK(core::count_str("ababab", "ab") == 3);
        CHECK(core::count_str("aaaaa", "aa") == 2);
        CHECK(core::count_str("hello", "") == 0);
        CHECK(core::count_str("", "hello") == 0);
        CHECK(core::count_str("", "") == 0);
        CHECK(core::count_str("hello", "x") == 0);

        // 测试 count_char 函数
        CHECK(core::count_char("hello world", 'l') == 3);
        CHECK(core::count_char("aaaaa", 'a') == 5);
        CHECK(core::count_char("hello", 'x') == 0);
        CHECK(core::count_char("", 'a') == 0);
    }

    TEST_CASE("SplitString")
    {
        std::vector<std::string> result;

        // 测试基本功能
        result.clear();
        core::split("a,b,c,d", ",", result);
        CHECK(result.size() == 4);
        CHECK(result[0] == "a");
        CHECK(result[1] == "b");
        CHECK(result[2] == "c");
        CHECK(result[3] == "d");

        // 测试多个分隔符
        result.clear();
        core::split("a,b;c,d", ",;", result);
        CHECK(result.size() == 4);
        CHECK(result[0] == "a");
        CHECK(result[1] == "b");
        CHECK(result[2] == "c");
        CHECK(result[3] == "d");

        // 测试连续分隔符
        result.clear();
        core::split("a,,b;;c", ",;", result);
        CHECK(result.size() == 3);
        CHECK(result[0] == "a");
        CHECK(result[1] == "b");
        CHECK(result[2] == "c");

        // 测试开头和结尾的分隔符
        result.clear();
        core::split(",,a,b,c,,", ",", result);
        CHECK(result.size() == 3);
        CHECK(result[0] == "a");
        CHECK(result[1] == "b");
        CHECK(result[2] == "c");

        // 测试空字符串
        result.clear();
        core::split("", ",", result);
        CHECK(result.empty());

        // 测试只有分隔符的字符串
        result.clear();
        core::split(",,,", ",", result);
        CHECK(result.empty());

        // 测试没有分隔符的字符串
        result.clear();
        core::split("hello", ",", result);
        CHECK(result.size() == 1);
        CHECK(result[0] == "hello");

        // 测试单个字符
        result.clear();
        core::split("a", ",", result);
        CHECK(result.size() == 1);
        CHECK(result[0] == "a");

        // 测试长字符串
        result.clear();
        std::string long_str;
        for (int i = 0; i < 1000; ++i)
        {
            long_str += core::to_string(i);
            if (i < 999) long_str += ",";
        }
        core::split(long_str.c_str(), ",", result);
        CHECK(result.size() == 1000);
        for (int i = 0; i < 1000; ++i)
        {
            CHECK(result[i] == core::to_string(i));
        }
    }

    TEST_CASE("Base64")
    {
        // 测试基本编码和解码
        std::string original = "Hello, Base64!";
        std::string encoded = core::base64_encode(original);
        CHECK(!encoded.empty());

        std::string decoded = core::base64_decode(encoded);
        CHECK(decoded == original);

        // 测试空字符串
        CHECK(core::base64_encode("").empty());
        CHECK(core::base64_decode("").empty());

        // 测试特殊字符
        std::string special_chars = "!@#$%^&*()_+";
        encoded = core::base64_encode(special_chars);
        decoded = core::base64_decode(encoded);
        CHECK(decoded == special_chars);

        // 测试二进制数据
        std::vector<uint8_t> binary_data = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
        std::string binary_str(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
        encoded = core::base64_encode(binary_str);
        decoded = core::base64_decode(encoded);
        CHECK(decoded.size() == binary_data.size());
        for (size_t i = 0; i < binary_data.size(); ++i)
        {
            CHECK(static_cast<uint8_t>(decoded[i]) == binary_data[i]);
        }
    }

    TEST_CASE("Base64URL")
    {
        // 测试基本编码和解码
        std::string original = "Hello, Base64URL!";
        std::string encoded = core::base64_url_encode(original);
        CHECK(!encoded.empty());

        std::string decoded = core::base64_url_decode(encoded);
        CHECK(decoded == original);

        // 测试空字符串
        CHECK(core::base64_url_encode("").empty());
        CHECK(core::base64_url_decode("").empty());

        // 测试 URL 安全字符替换：+ -> - , / -> _
        // "any carnal pleasure." 的标准 base64 为 "YW55IGNhcm5hbCBwbGVhc3VyZS4="
        // URL 安全版本应为 "YW55IGNhcm5hbCBwbGVhc3VyZS4"（无填充，+变-，/变_）
        std::string data_with_plus_slash = std::string(186, '\0') + std::string(1, '\3');
        std::string url_encoded = core::base64_url_encode(data_with_plus_slash);
        CHECK(url_encoded.find('+') == std::string::npos);
        CHECK(url_encoded.find('/') == std::string::npos);
        CHECK(url_encoded.find('=') == std::string::npos);

        std::string url_decoded = core::base64_url_decode(url_encoded);
        CHECK(url_decoded == data_with_plus_slash);

        // 测试不含填充符
        // "Hello" 标准 base64 = "SGVsbG8="，URL 安全 = "SGVsbG8"
        std::string hello = "Hello";
        std::string hello_encoded = core::base64_url_encode(hello);
        CHECK(hello_encoded == "SGVsbG8");
        CHECK(hello_encoded.find('=') == std::string::npos);

        std::string hello_decoded = core::base64_url_decode(hello_encoded);
        CHECK(hello_decoded == hello);

        // 测试标准 base64 与 URL base64 的对照
        // "Hello, Base64!" 标准 base64 = "SGVsbG8sIEJhc2U2NCE="
        std::string test_data = "Hello, Base64!";
        std::string std_encoded = core::base64_encode(test_data);
        std::string url_enc = core::base64_url_encode(test_data);

        // URL 编码应与标准编码前缀相同，但不含填充
        CHECK(url_enc.find('=') == std::string::npos);
        CHECK(std_encoded.substr(0, url_enc.size()) == url_enc);

        // 测试特殊字符
        std::string special_chars = "!@#$%^&*()_+";
        encoded = core::base64_url_encode(special_chars);
        decoded = core::base64_url_decode(encoded);
        CHECK(decoded == special_chars);

        // 测试二进制数据
        std::vector<uint8_t> binary_data = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
        std::string binary_str(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
        encoded = core::base64_url_encode(binary_str);
        decoded = core::base64_url_decode(encoded);
        CHECK(decoded.size() == binary_data.size());
        for (size_t i = 0; i < binary_data.size(); ++i)
        {
            CHECK(static_cast<uint8_t>(decoded[i]) == binary_data[i]);
        }

        // 测试解码带填充的 URL safe 字符串（兼容处理）
        // 虽然 URL safe 编码不包含填充，但解码时应能正确处理带填充的输入
        std::string with_padding = "SGVsbG8=";
        CHECK(core::base64_url_decode(with_padding) == "Hello");

        // 测试长度 % 4 == 1 的无效输入
        CHECK(core::base64_url_decode("A").empty());

        // 测试长度 % 4 == 2 的合法输入（需补2个=）
        // "A" 编码后标准 base64 为 "QQ=="，URL safe 为 "QQ"
        CHECK(core::base64_url_decode("QQ") == "A");

        // 测试长度 % 4 == 3 的合法输入（需补1个=）
        // "AB" 编码后标准 base64 为 "QUI="，URL safe 为 "QUI"
        CHECK(core::base64_url_decode("QUI") == "AB");

        // 测试长度 % 4 == 0 的合法输入（无需补=）
        // "ABC" 编码后标准 base64 为 "QUJD"，URL safe 也是 "QUJD"
        CHECK(core::base64_url_decode("QUJD") == "ABC");

        // 测试 URL safe 字符的解码
        // 确认 '-' 还原为 '+'，'_' 还原为 '/'
        CHECK(core::base64_url_decode("A-B_") == core::base64_decode("A+B/"));
    }

    TEST_CASE("MD5")
    {
        // 测试基本 MD5 计算
        std::string test_string = "Hello, MD5!";
        std::string md5_lower = core::md5_string(test_string, false);
        std::string md5_upper = core::md5_string(test_string, true);

        CHECK(!md5_lower.empty());
        CHECK(!md5_upper.empty());
        CHECK(md5_lower.size() == 32);
        CHECK(md5_upper.size() == 32);

        // 测试空字符串（应返回有效 MD5）
        CHECK(core::md5_string("", false) == "d41d8cd98f00b204e9800998ecf8427e");
        CHECK(core::md5_string("", true)  == "D41D8CD98F00B204E9800998ECF8427E");

        // 测试文件 MD5（创建临时文件）
        std::string temp_filename = "temp_md5_test.txt";
        {
            std::ofstream temp_file(temp_filename);
            temp_file << test_string;
        }

        std::string file_md5_lower = core::md5_file(temp_filename, false);
        std::string file_md5_upper = core::md5_file(temp_filename, true);

        CHECK(!file_md5_lower.empty());
        CHECK(!file_md5_upper.empty());
        CHECK(file_md5_lower.size() == 32);
        CHECK(file_md5_upper.size() == 32);
        CHECK(core::to_upper(file_md5_lower) == file_md5_upper);

        // 清理临时文件
        std::remove(temp_filename.c_str());

        // 测试不存在的文件
        CHECK(core::md5_file("non_existent_file.txt", false).empty());
    }
}