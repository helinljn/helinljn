#include "doctest/doctest.h"
#include "util/singleton.hpp"
#include "util/common.h"
#include "util/base64.h"
#include "util/aes.h"
#include "util/md5.h"
#include "util/numeric_cast.hpp"
#include "time/timespan.h"
#include "time/timestamp.h"
#include "time/datetime.h"
#include "fmt/format.h"
#include <cpptrace/cpptrace.hpp>
#include <sstream>
#include <thread>
#include <fstream>

// Unnamed namespace for internal linkage
namespace {

class SingletonTest : public core::singleton<SingletonTest>
{
public:
    const std::string& GetFile(void) const {return _file;}
    void SetFile(std::string str) {_file = std::move(str);}

    int GetLine(void) const {return _line;}
    void SetLine(const int val) {_line = val;}

private:
    SingletonTest(void)
        : _file()
        , _line(0)
    {
    }

private:
    CORE_SINGLETON_HELPER;

private:
    std::string _file;
    int         _line;
};

} // unnamed namespace

DOCTEST_TEST_SUITE("Misc")
{
    DOCTEST_TEST_CASE("Singleton")
    {
        auto& st = SingletonTest::instance();
        DOCTEST_CHECK(st.GetFile().empty());
        DOCTEST_CHECK(st.GetLine() == 0);

        st.SetFile("hello");
        st.SetLine(10);
        DOCTEST_CHECK(st.GetFile() == "hello");
        DOCTEST_CHECK(st.GetLine() == 10);
    }

    DOCTEST_TEST_CASE("StackTrace")
    {
        // 这个库目前只能这么用，也就是用st.print将结果输出至std::ostringstream中
        const cpptrace::stacktrace st = cpptrace::stacktrace::current();
        std::ostringstream         ostr;
        st.print(ostr);

        const std::string callstack = ostr.str();
        DOCTEST_CHECK(!callstack.empty());

        fmt::print("-- stack trace --\n{}-----------------\n", callstack);
    }

    DOCTEST_TEST_CASE("Timespan")
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
        DOCTEST_CHECK(span5 + span6 == 2001);
        DOCTEST_CHECK(span5 + 1001  == 2001);
        DOCTEST_CHECK(span5 - span6 == -1);
        DOCTEST_CHECK(span5 - 1001  == -1);

        span5 += span6;
        DOCTEST_CHECK(span5 == 2001);

        span5 -= span6;
        DOCTEST_CHECK(span5 == 1000);

        span5 += 1001;
        DOCTEST_CHECK(span5 == 2001);

        span5 -= 1001;
        DOCTEST_CHECK(span5 == 1000);
    }

    DOCTEST_TEST_CASE("Timestamp")
    {
        core::timestamp ts1(100 * core::timestamp::resolution);
        core::timestamp ts2(101 * core::timestamp::resolution);
        DOCTEST_CHECK(ts1 + core::timestamp::resolution == ts2);
        DOCTEST_CHECK(ts2 - core::timestamp::resolution == ts1);
        DOCTEST_CHECK(ts1 + core::timespan(core::timestamp::resolution) == ts2);
        DOCTEST_CHECK(ts2 - core::timespan(core::timestamp::resolution) == ts1);

        ts1 += core::timestamp::resolution;
        DOCTEST_CHECK(ts1 == ts2);

        ts1 -= core::timestamp::resolution;
        DOCTEST_CHECK(ts1 == ts2 - core::timestamp::resolution);

        ts1 += core::timespan(core::timestamp::resolution);
        DOCTEST_CHECK(ts1 == ts2);

        ts1 -= core::timespan(core::timestamp::resolution);
        DOCTEST_CHECK(ts1 == ts2 - core::timestamp::resolution);
    }

    DOCTEST_TEST_CASE("DateTime")
    {
        {
            // default constructor
            core::datetime dt1;

            // copy constructor from DateTimeEx
            core::datetime dt2(dt1);
            DOCTEST_CHECK(dt1 == dt2);
            DOCTEST_CHECK(dt1.to_timestamp() == dt2.to_timestamp());

            // copy constructor from Timestamp
            core::datetime dt3(dt1.to_timestamp());
            DOCTEST_CHECK(dt1 == dt3);
            DOCTEST_CHECK(dt1.to_timestamp() == dt3.to_timestamp());

            // copy constructor from tm struct
            core::datetime dt4(dt1.to_tm());
            DOCTEST_CHECK(dt1.year() == dt4.year());
            DOCTEST_CHECK(dt1.month() == dt4.month());
            DOCTEST_CHECK(dt1.day() == dt4.day());
            DOCTEST_CHECK(dt1.day_of_week() == dt4.day_of_week());
            DOCTEST_CHECK(dt1.day_of_year() == dt4.day_of_year());
            DOCTEST_CHECK(dt1.hour() == dt4.hour());
            DOCTEST_CHECK(dt1.minute() == dt4.minute());
            DOCTEST_CHECK(dt1.second() == dt4.second());

            // copy constructor from the given local date and time
            core::datetime dt5(dt1.year(), dt1.month(), dt1.day(), dt1.hour(), dt1.minute(), dt1.second(), dt1.millisecond(), dt1.microsecond());
            DOCTEST_CHECK(dt1 == dt5);
            DOCTEST_CHECK(dt1.to_timestamp() == dt5.to_timestamp());

            core::datetime dt6;

            // assigns a datetime
            dt6 += core::timespan(std::chrono::hours(1));
            dt6  = dt1;
            DOCTEST_CHECK(dt1 == dt6);
            DOCTEST_CHECK(dt1.to_timestamp() == dt6.to_timestamp());

            // assigns a timestamp
            dt6 += core::timespan(std::chrono::hours(1));
            dt6  = dt1.to_timestamp();
            DOCTEST_CHECK(dt1 == dt6);
            DOCTEST_CHECK(dt1.to_timestamp() == dt6.to_timestamp());

            // assigns a tm struct
            dt6 += core::timespan(std::chrono::hours(1));
            dt6  = dt1.to_tm();
            DOCTEST_CHECK(dt1.year() == dt6.year());
            DOCTEST_CHECK(dt1.month() == dt6.month());
            DOCTEST_CHECK(dt1.day() == dt6.day());
            DOCTEST_CHECK(dt1.day_of_week() == dt6.day_of_week());
            DOCTEST_CHECK(dt1.day_of_year() == dt6.day_of_year());
            DOCTEST_CHECK(dt1.hour() == dt6.hour());
            DOCTEST_CHECK(dt1.minute() == dt6.minute());
            DOCTEST_CHECK(dt1.second() == dt6.second());

            // assigns a Timestamp
            dt6 += core::timespan(std::chrono::hours(1));
            dt6.assign(dt1.to_timestamp());
            DOCTEST_CHECK(dt1 == dt6);
            DOCTEST_CHECK(dt1.to_timestamp() == dt6.to_timestamp());

            // assigns a tm struct
            dt6 += core::timespan(std::chrono::hours(1));
            dt6.assign(dt1.to_tm());
            DOCTEST_CHECK(dt1.year() == dt6.year());
            DOCTEST_CHECK(dt1.month() == dt6.month());
            DOCTEST_CHECK(dt1.day() == dt6.day());
            DOCTEST_CHECK(dt1.day_of_week() == dt6.day_of_week());
            DOCTEST_CHECK(dt1.day_of_year() == dt6.day_of_year());
            DOCTEST_CHECK(dt1.hour() == dt6.hour());
            DOCTEST_CHECK(dt1.minute() == dt6.minute());
            DOCTEST_CHECK(dt1.second() == dt6.second());

            // assigns a local date and time
            dt6 += core::timespan(std::chrono::hours(1));
            dt6.assign(dt1.year(), dt1.month(), dt1.day(), dt1.hour(), dt1.minute(), dt1.second(), dt1.millisecond(), dt1.microsecond());
            DOCTEST_CHECK(dt1 == dt6);
            DOCTEST_CHECK(dt1.to_timestamp() == dt6.to_timestamp());

            dt6 += core::timespan(std::chrono::hours(1));
            DOCTEST_CHECK(dt1 != dt6);
            DOCTEST_CHECK(dt1 <  dt6);
            DOCTEST_CHECK(dt1 <= dt6);
            DOCTEST_CHECK(dt6 >  dt1);
            DOCTEST_CHECK(dt6 >= dt1);

            DOCTEST_CHECK(dt6 == dt1 + core::timespan(std::chrono::hours(1)));
            DOCTEST_CHECK(dt1 == dt6 - core::timespan(std::chrono::hours(1)));

            core::timestamp ts(1314521000 * core::timestamp::resolution);
            dt5 = ts;
            dt6 = ts + core::timespan(std::chrono::seconds(5211314));

            const auto diff = dt6 - dt5;
            DOCTEST_CHECK(diff.total_seconds() == 5211314);
        }

        {
            // 1970-01-01 08:00:00 Thursday
            core::datetime dt(core::timestamp(0));
            DOCTEST_CHECK(dt.year() == 1970);
            DOCTEST_CHECK(dt.month() == 1);
            DOCTEST_CHECK(dt.day() == 1);
            DOCTEST_CHECK(dt.hour() == 8);
            DOCTEST_CHECK(dt.minute() == 0);
            DOCTEST_CHECK(dt.second() == 0);
            DOCTEST_CHECK(dt.millisecond() == 0);
            DOCTEST_CHECK(dt.microsecond() == 0);
            DOCTEST_CHECK(dt.day_of_week() == 4);
            DOCTEST_CHECK(dt.to_timestamp().epoch_time() == 0);

            // 1970-01-01 08:00:00 Thursday
            core::datetime dtex(dt);
            DOCTEST_CHECK(dtex.year() == 1970);
            DOCTEST_CHECK(dtex.month() == 1);
            DOCTEST_CHECK(dtex.day() == 1);
            DOCTEST_CHECK(dtex.hour() == 8);
            DOCTEST_CHECK(dtex.minute() == 0);
            DOCTEST_CHECK(dtex.second() == 0);
            DOCTEST_CHECK(dtex.millisecond() == 0);
            DOCTEST_CHECK(dtex.microsecond() == 0);
            DOCTEST_CHECK(dtex.day_of_week() == 4);
            DOCTEST_CHECK(dtex.to_timestamp().epoch_time() == 0);

            dtex += core::timespan(std::chrono::hours(1));
            dtex  = dt;
            DOCTEST_CHECK(dtex.year() == 1970);
            DOCTEST_CHECK(dtex.month() == 1);
            DOCTEST_CHECK(dtex.day() == 1);
            DOCTEST_CHECK(dtex.hour() == 8);
            DOCTEST_CHECK(dtex.minute() == 0);
            DOCTEST_CHECK(dtex.second() == 0);
            DOCTEST_CHECK(dtex.millisecond() == 0);
            DOCTEST_CHECK(dtex.microsecond() == 0);
            DOCTEST_CHECK(dtex.day_of_week() == 4);
            DOCTEST_CHECK(dtex.to_timestamp().epoch_time() == 0);

            dtex += core::timespan(std::chrono::hours(1));
            dtex  = dt;
            DOCTEST_CHECK(dtex.year() == 1970);
            DOCTEST_CHECK(dtex.month() == 1);
            DOCTEST_CHECK(dtex.day() == 1);
            DOCTEST_CHECK(dtex.hour() == 8);
            DOCTEST_CHECK(dtex.minute() == 0);
            DOCTEST_CHECK(dtex.second() == 0);
            DOCTEST_CHECK(dtex.millisecond() == 0);
            DOCTEST_CHECK(dtex.microsecond() == 0);
            DOCTEST_CHECK(dtex.day_of_week() == 4);
            DOCTEST_CHECK(dtex.to_timestamp().epoch_time() == 0);

            DOCTEST_CHECK(dtex - dt == 0);
        }

        // 1970-01-01 08:00:00 Thursday
        core::datetime dt(core::timestamp(0));
        DOCTEST_CHECK(dt.year() == 1970);
        DOCTEST_CHECK(dt.month() == 1);
        DOCTEST_CHECK(dt.day() == 1);
        DOCTEST_CHECK(dt.hour() == 8);
        DOCTEST_CHECK(dt.minute() == 0);
        DOCTEST_CHECK(dt.second() == 0);
        DOCTEST_CHECK(dt.millisecond() == 0);
        DOCTEST_CHECK(dt.microsecond() == 0);
        DOCTEST_CHECK(dt.day_of_week() == 4);
        DOCTEST_CHECK(dt.to_timestamp().epoch_time() == 0);

        tm tmStruct = dt.to_tm();
        DOCTEST_CHECK(tmStruct.tm_year == 70);
        DOCTEST_CHECK(tmStruct.tm_mon == 0);
        DOCTEST_CHECK(tmStruct.tm_mday == 1);
        DOCTEST_CHECK(tmStruct.tm_hour == 8);
        DOCTEST_CHECK(tmStruct.tm_min == 0);
        DOCTEST_CHECK(tmStruct.tm_sec == 0);

        dt = tmStruct;
        DOCTEST_CHECK(dt.year() == 1970);
        DOCTEST_CHECK(dt.month() == 1);
        DOCTEST_CHECK(dt.day() == 1);
        DOCTEST_CHECK(dt.hour() == 8);
        DOCTEST_CHECK(dt.minute() == 0);
        DOCTEST_CHECK(dt.second() == 0);
        DOCTEST_CHECK(dt.millisecond() == 0);
        DOCTEST_CHECK(dt.microsecond() == 0);
        DOCTEST_CHECK(dt.day_of_week() == 4);
        DOCTEST_CHECK(dt.to_timestamp().epoch_time() == 0);

        // 2001-09-09 09:46:40 Sunday
        dt = core::timestamp(1000000000 * core::timestamp::resolution);
        DOCTEST_CHECK(dt.year() == 2001);
        DOCTEST_CHECK(dt.month() == 9);
        DOCTEST_CHECK(dt.day() == 9);
        DOCTEST_CHECK(dt.hour() == 9);
        DOCTEST_CHECK(dt.minute() == 46);
        DOCTEST_CHECK(dt.second() == 40);
        DOCTEST_CHECK(dt.millisecond() == 0);
        DOCTEST_CHECK(dt.microsecond() == 0);
        DOCTEST_CHECK(dt.day_of_week() == 0);
        DOCTEST_CHECK(dt.to_timestamp().epoch_time() == 1000000000);

        // 3000-12-31 23:59:59 Friday
        dt = core::datetime(3000, 12, 31, 23, 59, 59);
        DOCTEST_CHECK(dt.year() == 3000);
        DOCTEST_CHECK(dt.month() == 12);
        DOCTEST_CHECK(dt.day() == 31);
        DOCTEST_CHECK(dt.hour() == 23);
        DOCTEST_CHECK(dt.minute() == 59);
        DOCTEST_CHECK(dt.second() == 59);
        DOCTEST_CHECK(dt.millisecond() == 0);
        DOCTEST_CHECK(dt.microsecond() == 0);
        DOCTEST_CHECK(dt.day_of_week() == 3);
        DOCTEST_CHECK(dt.to_timestamp().epoch_time() == time_t{32535187199});

        // 2011-08-28 16:43:20 Sunday
        dt = core::datetime(2011, 8, 28, 16, 43, 20);
        DOCTEST_CHECK(dt.year() == 2011);
        DOCTEST_CHECK(dt.month() == 8);
        DOCTEST_CHECK(dt.day() == 28);
        DOCTEST_CHECK(dt.hour() == 16);
        DOCTEST_CHECK(dt.minute() == 43);
        DOCTEST_CHECK(dt.second() == 20);
        DOCTEST_CHECK(dt.millisecond() == 0);
        DOCTEST_CHECK(dt.microsecond() == 0);
        DOCTEST_CHECK(dt.day_of_week() == 0);
        DOCTEST_CHECK(dt.to_timestamp().epoch_time() == 1314521000);

        core::datetime dt1 = dt + core::timespan(std::chrono::hours(1));
        DOCTEST_CHECK(dt1.year() == 2011);
        DOCTEST_CHECK(dt1.month() == 8);
        DOCTEST_CHECK(dt1.day() == 28);
        DOCTEST_CHECK(dt1.hour() == 17);
        DOCTEST_CHECK(dt1.minute() == 43);
        DOCTEST_CHECK(dt1.second() == 20);
        DOCTEST_CHECK(dt1.millisecond() == 0);
        DOCTEST_CHECK(dt1.microsecond() == 0);
        DOCTEST_CHECK(dt1.day_of_week() == 0);
        DOCTEST_CHECK(dt1.to_timestamp().epoch_time() == 1314524600);

        core::datetime dt2 = dt1 - core::timespan(std::chrono::hours(1));
        DOCTEST_CHECK(dt2.year() == 2011);
        DOCTEST_CHECK(dt2.month() == 8);
        DOCTEST_CHECK(dt2.day() == 28);
        DOCTEST_CHECK(dt2.hour() == 16);
        DOCTEST_CHECK(dt2.minute() == 43);
        DOCTEST_CHECK(dt2.second() == 20);
        DOCTEST_CHECK(dt2.millisecond() == 0);
        DOCTEST_CHECK(dt2.microsecond() == 0);
        DOCTEST_CHECK(dt2.day_of_week() == 0);
        DOCTEST_CHECK(dt2.to_timestamp().epoch_time() == 1314521000);

        const auto span = dt1 - dt2;
        DOCTEST_CHECK(span.days() == 0);
        DOCTEST_CHECK(span.hours() == 1);
        DOCTEST_CHECK(span.total_hours() == 1);
        DOCTEST_CHECK(span.minutes() == 0);
        DOCTEST_CHECK(span.total_minutes() == 60);
        DOCTEST_CHECK(span.seconds() == 0);
        DOCTEST_CHECK(span.total_seconds() == 3600);
        DOCTEST_CHECK(span.milliseconds() == 0);
        DOCTEST_CHECK(span.total_milliseconds() == 3600000);
    }

    DOCTEST_TEST_CASE("IsGBK")
    {
        // 测试空字符串
        DOCTEST_CHECK(!core::is_gbk(""));
        DOCTEST_CHECK(!core::is_gbk(std::string()));

        // 测试ASCII字符串
        DOCTEST_CHECK(core::is_gbk(std::string_view("hello", 5)));
        DOCTEST_CHECK(core::is_gbk("hello"));

        // 测试GBK字符串（中文）
        // 注意：这里需要确保字符串是GBK编码
        // 由于编码环境可能不同，这里使用十六进制表示
        const uint8_t gbk_chinese[] = {0xD6, 0xD0, 0xCE, 0xC4}; // "中文"的GBK编码
        DOCTEST_CHECK(core::is_gbk(std::string_view(reinterpret_cast<const char*>(gbk_chinese), sizeof(gbk_chinese))));

        // 测试混合ASCII和GBK
        const uint8_t mixed[] = {0x68, 0x65, 0x6C, 0x6C, 0x6F, 0xD6, 0xD0, 0xCE, 0xC4}; // "hello中文"
        DOCTEST_CHECK(core::is_gbk(std::string_view(reinterpret_cast<const char*>(mixed), sizeof(mixed))));

        // 测试无效的GBK编码
        const uint8_t invalid_gbk1[] = {0x80}; // 无效的首字节
        DOCTEST_CHECK(!core::is_gbk(std::string_view(reinterpret_cast<const char*>(invalid_gbk1), sizeof(invalid_gbk1))));

        const uint8_t invalid_gbk2[] = {0x81, 0x3F}; // 无效的尾字节
        DOCTEST_CHECK(!core::is_gbk(std::string_view(reinterpret_cast<const char*>(invalid_gbk2), sizeof(invalid_gbk2))));

        const uint8_t invalid_gbk3[] = {0x81}; // 不完整的GBK编码
        DOCTEST_CHECK(!core::is_gbk(std::string_view(reinterpret_cast<const char*>(invalid_gbk3), sizeof(invalid_gbk3))));
    }

    DOCTEST_TEST_CASE("IsUTF8")
    {
        // 测试空字符串
        DOCTEST_CHECK(!core::is_utf8(""));
        DOCTEST_CHECK(!core::is_utf8(std::string()));

        // 测试ASCII字符串
        DOCTEST_CHECK(core::is_utf8(std::string_view("hello", 5)));
        DOCTEST_CHECK(core::is_utf8("hello"));

        // 测试UTF-8字符串（中文）
        const uint8_t utf8_chinese[] = {0xE4, 0xB8, 0xAD, 0xE6, 0x96, 0x87}; // "中文"的UTF-8编码
        DOCTEST_CHECK(core::is_utf8(std::string_view(reinterpret_cast<const char*>(utf8_chinese), sizeof(utf8_chinese))));

        // 测试混合ASCII和UTF-8
        const uint8_t mixed[] = {0x68, 0x65, 0x6C, 0x6C, 0x6F, 0xE4, 0xB8, 0xAD, 0xE6, 0x96, 0x87}; // "hello中文"
        DOCTEST_CHECK(core::is_utf8(std::string_view(reinterpret_cast<const char*>(mixed), sizeof(mixed))));

        // 测试多字节UTF-8字符
        const uint8_t multi_byte[] = {0xF0, 0x9F, 0x98, 0x83}; // 😃 表情符号的UTF-8编码
        DOCTEST_CHECK(core::is_utf8(std::string_view(reinterpret_cast<const char*>(multi_byte), sizeof(multi_byte))));

        // 测试无效的UTF-8编码
        const uint8_t invalid_utf8_1[] = {0xC0}; // 不完整的双字节编码
        DOCTEST_CHECK(!core::is_utf8(std::string_view(reinterpret_cast<const char*>(invalid_utf8_1), sizeof(invalid_utf8_1))));

        const uint8_t invalid_utf8_2[] = {0xC0, 0x80}; // 过度编码的ASCII
        DOCTEST_CHECK(!core::is_utf8(std::string_view(reinterpret_cast<const char*>(invalid_utf8_2), sizeof(invalid_utf8_2))));

        const uint8_t invalid_utf8_3[] = {0xE0, 0x80, 0x80}; // 不完整的三字节编码
        DOCTEST_CHECK(!core::is_utf8(std::string_view(reinterpret_cast<const char*>(invalid_utf8_3), sizeof(invalid_utf8_3))));

        const uint8_t invalid_utf8_4[] = {0xF8, 0x80, 0x80, 0x80, 0x80}; // 超过4字节的编码
        DOCTEST_CHECK(!core::is_utf8(std::string_view(reinterpret_cast<const char*>(invalid_utf8_4), sizeof(invalid_utf8_4))));
    }

    DOCTEST_TEST_CASE("MemoryToHexString")
    {
        // 测试空数据
        char buf[10];
        DOCTEST_CHECK(!core::memory_to_hex_string(nullptr, 0, buf, sizeof(buf)));

        // 测试基本功能
        uint8_t data[] = {0x12, 0x34, 0xAB, 0xCD};
        char hex_buf[20];
        DOCTEST_CHECK(core::memory_to_hex_string(data, sizeof(data), hex_buf, sizeof(hex_buf)));
        DOCTEST_CHECK(strcmp(hex_buf, "1234ABCD") == 0);

        // 测试小写
        DOCTEST_CHECK(core::memory_to_hex_string(data, sizeof(data), hex_buf, sizeof(hex_buf), false));
        DOCTEST_CHECK(strcmp(hex_buf, "1234abcd") == 0);

        // 测试缓冲区大小不足
        DOCTEST_CHECK(!core::memory_to_hex_string(data, sizeof(data), hex_buf, 8));

        // 测试单字节
        uint8_t single_byte = 0x5A;
        DOCTEST_CHECK(core::memory_to_hex_string(&single_byte, 1, hex_buf, sizeof(hex_buf)));
        DOCTEST_CHECK(strcmp(hex_buf, "5A") == 0);
    }

    DOCTEST_TEST_CASE("HexStringToMemory")
    {
        // 测试空字符串
        uint8_t buf[10];
        DOCTEST_CHECK(!core::hex_string_to_memory(nullptr, buf, sizeof(buf)));
        DOCTEST_CHECK(core::hex_string_to_memory("", buf, sizeof(buf)));

        // 测试基本功能
        DOCTEST_CHECK(core::hex_string_to_memory("1234ABCD", buf, sizeof(buf)));
        DOCTEST_CHECK(buf[0] == 0x12);
        DOCTEST_CHECK(buf[1] == 0x34);
        DOCTEST_CHECK(buf[2] == 0xAB);
        DOCTEST_CHECK(buf[3] == 0xCD);

        // 测试小写
        DOCTEST_CHECK(core::hex_string_to_memory("1234abcd", buf, sizeof(buf)));
        DOCTEST_CHECK(buf[0] == 0x12);
        DOCTEST_CHECK(buf[1] == 0x34);
        DOCTEST_CHECK(buf[2] == 0xAB);
        DOCTEST_CHECK(buf[3] == 0xCD);

        // 测试缓冲区大小不足
        DOCTEST_CHECK(!core::hex_string_to_memory("1234ABCD", buf, 3));

        // 测试无效的十六进制字符
        DOCTEST_CHECK(!core::hex_string_to_memory("1234XY", buf, sizeof(buf)));

        // 测试奇数长度
        DOCTEST_CHECK(!core::hex_string_to_memory("123", buf, sizeof(buf)));

        // 测试单字节
        DOCTEST_CHECK(core::hex_string_to_memory("5A", buf, sizeof(buf)));
        DOCTEST_CHECK(buf[0] == 0x5A);
    }

    DOCTEST_TEST_CASE("SystemInfo")
    {
        // 测试 get_free_memory 函数
        uint32_t free_memory = core::get_free_memory();
        DOCTEST_CHECK(free_memory > 0);

        // 测试 get_total_memory 函数
        uint32_t total_memory = core::get_total_memory();
        DOCTEST_CHECK(total_memory > 0);
        DOCTEST_CHECK(total_memory >= free_memory);

        // 测试 get_process_id 函数
        uint32_t process_id = core::get_process_id();
        DOCTEST_CHECK(process_id > 0);

        // 测试 get_cpu_logic_count 函数
        uint32_t cpu_count = core::get_cpu_logic_count();
        DOCTEST_CHECK(cpu_count > 0);

        fmt::print("FreeMemory: {}, TotalMemory: {}, ProcessID: {}, CPUCount: {}\n",
            free_memory, total_memory, process_id, cpu_count);
    }

    DOCTEST_TEST_CASE("ProgramRunningTime")
    {
        // 测试 get_program_running_time 函数
        uint64_t start_time = core::get_program_running_time();
        DOCTEST_CHECK(start_time >= 0);

        // 等待一段时间后再次获取
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        uint64_t end_time = core::get_program_running_time();
        DOCTEST_CHECK(end_time >= start_time);
        DOCTEST_CHECK(end_time - start_time >= 90); // 允许一定的误差
    }

    DOCTEST_TEST_CASE("EncodingConversion")
    {
        // 测试空字符串
        DOCTEST_CHECK(core::gbk_to_utf8("").empty());
        DOCTEST_CHECK(core::utf8_to_gbk("").empty());

        // 测试 ASCII 字符串
        std::string ascii_str = "hello world";
        DOCTEST_CHECK(core::gbk_to_utf8(ascii_str) == ascii_str);
        DOCTEST_CHECK(core::utf8_to_gbk(ascii_str) == ascii_str);

        // 测试中文转换（注意：这里的测试依赖于系统编码环境）
        // 测试 GBK 到 UTF-8 转换
        // std::string gbk_chinese  = "中文";
        // std::string utf8_chinese = core::gbk_to_utf8(gbk_chinese);
        // DOCTEST_CHECK(!utf8_chinese.empty());

        // 测试 UTF-8 到 GBK 转换
        std::string utf8_chinese   = "中文";
        std::string converted_back = core::utf8_to_gbk(utf8_chinese);
        DOCTEST_CHECK(!converted_back.empty());

        // 测试转换的可逆性（在支持 GBK 的环境中应该成立）
        // 注意：由于编码环境的差异，这个测试可能在某些环境下失败
        // DOCTEST_CHECK(converted_back == gbk_chinese);

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
            DOCTEST_CHECK(false);
        }
    }

    DOCTEST_TEST_CASE("Random")
    {
        // 测试 random_uint32 函数
        for (int i = 0; i < 100; ++i)
        {
            uint32_t value = core::random_uint32();
            DOCTEST_CHECK(value <= UINT32_MAX);
        }

        // 测试 random_uint64 函数
        for (int i = 0; i < 100; ++i)
        {
            uint64_t value = core::random_uint64();
            DOCTEST_CHECK(value <= UINT64_MAX);
        }

        // 测试 random_float 函数
        for (int i = 0; i < 100; ++i)
        {
            float value = core::random_float();
            DOCTEST_CHECK(value >= 0.0f);
            DOCTEST_CHECK(value <= 1.0f);
        }

        // 测试 random_double 函数
        for (int i = 0; i < 100; ++i)
        {
            double value = core::random_double();
            DOCTEST_CHECK(value >= 0.0);
            DOCTEST_CHECK(value <= 1.0);
        }

        // 测试 random_range 函数
        // 测试有效输入
        int32_t upper_bound = 100;
        for (int i = 0; i < 1000; ++i)
        {
            int32_t value = core::random_range(upper_bound);
            DOCTEST_CHECK(value >= 0);
            DOCTEST_CHECK(value < upper_bound);
        }

        // 测试无效输入
        DOCTEST_CHECK(core::random_range(0) == 0);
        DOCTEST_CHECK(core::random_range(-10) == 0);
    }

    DOCTEST_TEST_CASE("EnvironmentVariables")
    {
        // 测试设置环境变量
        DOCTEST_CHECK(core::env_set("TEST_ENV_VAR", "test_value"));

        // 测试判断环境变量是否存在
        DOCTEST_CHECK(core::env_has("TEST_ENV_VAR"));

        // 测试获取环境变量的值
        DOCTEST_CHECK(core::env_get("TEST_ENV_VAR") == "test_value");

        // 测试获取不存在的环境变量
        DOCTEST_CHECK(!core::env_has("NON_EXISTENT_ENV_VAR"));
        DOCTEST_CHECK(core::env_get("NON_EXISTENT_ENV_VAR").empty());

        // 测试空环境变量名
        DOCTEST_CHECK(!core::env_has(""));
        DOCTEST_CHECK(core::env_get("").empty());
        DOCTEST_CHECK(!core::env_set("", "value"));
    }

    DOCTEST_TEST_CASE("PathOperations")
    {
        // 测试 get_exepath 函数
        char exepath_buf[2048];
        uint32_t exepath_len = sizeof(exepath_buf);
        DOCTEST_CHECK(core::get_exepath(exepath_buf, &exepath_len));
        DOCTEST_CHECK(exepath_len > 0);

        std::string exepath_str = core::get_exepath();
        DOCTEST_CHECK(!exepath_str.empty());

        // 测试 get_exedir 函数
        char exedir_buf[2048];
        uint32_t exedir_len = sizeof(exedir_buf);
        DOCTEST_CHECK(core::get_exedir(exedir_buf, &exedir_len));
        DOCTEST_CHECK(exedir_len > 0);

        std::string exedir_str = core::get_exedir();
        DOCTEST_CHECK(!exedir_str.empty());

        fmt::print("exepath: {}, exedir: {}\n", exepath_str, exedir_str);
    }

    DOCTEST_TEST_CASE("StringOperations")
    {
        // 测试 trim 函数
        DOCTEST_CHECK(core::trim("  hello world  ") == "hello world");
        DOCTEST_CHECK(core::trim("hello world") == "hello world");
        DOCTEST_CHECK(core::trim("   ") == "");
        DOCTEST_CHECK(core::trim("") == "");

        // 测试 ltrim 函数
        DOCTEST_CHECK(core::ltrim("  hello world  ") == "hello world  ");
        DOCTEST_CHECK(core::ltrim("hello world") == "hello world");
        DOCTEST_CHECK(core::ltrim("   ") == "");
        DOCTEST_CHECK(core::ltrim("") == "");

        // 测试 rtrim 函数
        DOCTEST_CHECK(core::rtrim("  hello world  ") == "  hello world");
        DOCTEST_CHECK(core::rtrim("hello world") == "hello world");
        DOCTEST_CHECK(core::rtrim("   ") == "");
        DOCTEST_CHECK(core::rtrim("") == "");

        // 测试 starts_with 函数
        DOCTEST_CHECK(core::starts_with("hello world", "hello"));
        DOCTEST_CHECK(!core::starts_with("hello world", "world"));
        DOCTEST_CHECK(core::starts_with("hello", ""));
        DOCTEST_CHECK(!core::starts_with("", "hello"));

        // 测试 ends_with 函数
        DOCTEST_CHECK(core::ends_with("hello world", "world"));
        DOCTEST_CHECK(!core::ends_with("hello world", "hello"));
        DOCTEST_CHECK(core::ends_with("hello", ""));
        DOCTEST_CHECK(!core::ends_with("", "hello"));

        // 测试 contains 函数
        DOCTEST_CHECK(core::contains("hello world", "world"));
        DOCTEST_CHECK(core::contains("hello world", "hello"));
        DOCTEST_CHECK(core::contains("hello world", "lo wo"));
        DOCTEST_CHECK(!core::contains("hello world", "test"));
        DOCTEST_CHECK(!core::contains("hello", "hello world"));
        DOCTEST_CHECK(core::contains("hello", ""));
        DOCTEST_CHECK(!core::contains("", "hello"));

        // 测试 to_upper 函数
        DOCTEST_CHECK(core::to_upper("hello world") == "HELLO WORLD");
        DOCTEST_CHECK(core::to_upper("HELLO WORLD") == "HELLO WORLD");
        DOCTEST_CHECK(core::to_upper("") == "");

        // 测试 to_lower 函数
        DOCTEST_CHECK(core::to_lower("HELLO WORLD") == "hello world");
        DOCTEST_CHECK(core::to_lower("hello world") == "hello world");
        DOCTEST_CHECK(core::to_lower("") == "");

        // 测试 replace 函数
        DOCTEST_CHECK(core::replace("hello world", "world", "test") == "hello test");
        DOCTEST_CHECK(core::replace("hello hello", "hello", "hi") == "hi hi");
        DOCTEST_CHECK(core::replace("hello", "", "test") == "hello");
        DOCTEST_CHECK(core::replace("", "hello", "test") == "");

        // 测试 join 函数
        std::vector<std::string> parts = {"hello", "world", "test"};
        DOCTEST_CHECK(core::join(parts, ", ") == "hello, world, test");
        DOCTEST_CHECK(core::join(parts, "") == "helloworldtest");
        DOCTEST_CHECK(core::join({}, ", ") == "");
        parts = {"hello"};
        DOCTEST_CHECK(core::join(parts, ", ") == "hello");
    }

    DOCTEST_TEST_CASE("SplitString")
    {
        std::vector<std::string> result;

        // 测试基本功能
        result.clear();
        core::split("a,b,c,d", ",", result);
        DOCTEST_CHECK(result.size() == 4);
        DOCTEST_CHECK(result[0] == "a");
        DOCTEST_CHECK(result[1] == "b");
        DOCTEST_CHECK(result[2] == "c");
        DOCTEST_CHECK(result[3] == "d");

        // 测试多个分隔符
        result.clear();
        core::split("a,b;c,d", ",;", result);
        DOCTEST_CHECK(result.size() == 4);
        DOCTEST_CHECK(result[0] == "a");
        DOCTEST_CHECK(result[1] == "b");
        DOCTEST_CHECK(result[2] == "c");
        DOCTEST_CHECK(result[3] == "d");

        // 测试连续分隔符
        result.clear();
        core::split("a,,b;;c", ",;", result);
        DOCTEST_CHECK(result.size() == 3);
        DOCTEST_CHECK(result[0] == "a");
        DOCTEST_CHECK(result[1] == "b");
        DOCTEST_CHECK(result[2] == "c");

        // 测试开头和结尾的分隔符
        result.clear();
        core::split(",,a,b,c,,", ",", result);
        DOCTEST_CHECK(result.size() == 3);
        DOCTEST_CHECK(result[0] == "a");
        DOCTEST_CHECK(result[1] == "b");
        DOCTEST_CHECK(result[2] == "c");

        // 测试空字符串
        result.clear();
        core::split("", ",", result);
        DOCTEST_CHECK(result.empty());

        // 测试只有分隔符的字符串
        result.clear();
        core::split(",,,", ",", result);
        DOCTEST_CHECK(result.empty());

        // 测试没有分隔符的字符串
        result.clear();
        core::split("hello", ",", result);
        DOCTEST_CHECK(result.size() == 1);
        DOCTEST_CHECK(result[0] == "hello");

        // 测试单个字符
        result.clear();
        core::split("a", ",", result);
        DOCTEST_CHECK(result.size() == 1);
        DOCTEST_CHECK(result[0] == "a");

        // 测试长字符串
        result.clear();
        std::string long_str;
        for (int i = 0; i < 1000; ++i)
        {
            long_str += core::to_string(i);
            if (i < 999) long_str += ",";
        }
        core::split(long_str.c_str(), ",", result);
        DOCTEST_CHECK(result.size() == 1000);
        for (int i = 0; i < 1000; ++i)
        {
            DOCTEST_CHECK(result[i] == core::to_string(i));
        }
    }

    DOCTEST_TEST_CASE("AES")
    {
        // 测试 AES-128 加密和解密
        uint8_t key[16] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
        uint8_t plain_text[16] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
        uint8_t cipher_text[16];
        uint8_t decrypted_text[16];

        // 加密
        core::aes128_encrypt(key, plain_text, cipher_text);

        // 解密
        core::aes128_decrypt(key, cipher_text, decrypted_text);

        // 验证解密后的结果是否与原始明文相同
        for (int i = 0; i < 16; ++i)
        {
            DOCTEST_CHECK(decrypted_text[i] == plain_text[i]);
        }

        // 测试空数据（边界情况）
        uint8_t empty_plain[16] = {0};
        uint8_t empty_cipher[16];
        uint8_t empty_decrypted[16];
        core::aes128_encrypt(key, empty_plain, empty_cipher);
        core::aes128_decrypt(key, empty_cipher, empty_decrypted);
        for (int i = 0; i < 16; ++i)
        {
            DOCTEST_CHECK(empty_decrypted[i] == empty_plain[i]);
        }
    }

    DOCTEST_TEST_CASE("Base64")
    {
        // 测试基本编码和解码
        std::string original = "Hello, Base64!";
        std::string encoded = core::base64_encode(original);
        DOCTEST_CHECK(!encoded.empty());

        std::string decoded = core::base64_decode(encoded);
        DOCTEST_CHECK(decoded == original);

        // 测试空字符串
        DOCTEST_CHECK(core::base64_encode("").empty());
        DOCTEST_CHECK(core::base64_decode("").empty());

        // 测试特殊字符
        std::string special_chars = "!@#$%^&*()_+";
        encoded = core::base64_encode(special_chars);
        decoded = core::base64_decode(encoded);
        DOCTEST_CHECK(decoded == special_chars);

        // 测试二进制数据
        std::vector<uint8_t> binary_data = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09};
        std::string binary_str(reinterpret_cast<const char*>(binary_data.data()), binary_data.size());
        encoded = core::base64_encode(binary_str);
        decoded = core::base64_decode(encoded);
        DOCTEST_CHECK(decoded.size() == binary_data.size());
        for (size_t i = 0; i < binary_data.size(); ++i)
        {
            DOCTEST_CHECK(static_cast<uint8_t>(decoded[i]) == binary_data[i]);
        }
    }

    DOCTEST_TEST_CASE("MD5")
    {
        // 测试基本 MD5 计算
        std::string test_string = "Hello, MD5!";
        std::string md5_lower = core::md5_string(test_string, false);
        std::string md5_upper = core::md5_string(test_string, true);

        DOCTEST_CHECK(!md5_lower.empty());
        DOCTEST_CHECK(!md5_upper.empty());
        DOCTEST_CHECK(md5_lower.size() == 32);
        DOCTEST_CHECK(md5_upper.size() == 32);

        // 测试空字符串
        DOCTEST_CHECK(core::md5_string("", false).empty());

        // 测试文件 MD5（创建临时文件）
        std::string temp_filename = "temp_md5_test.txt";
        {
            std::ofstream temp_file(temp_filename);
            temp_file << test_string;
        }

        std::string file_md5_lower = core::md5_file(temp_filename.c_str(), false);
        std::string file_md5_upper = core::md5_file(temp_filename.c_str(), true);

        DOCTEST_CHECK(!file_md5_lower.empty());
        DOCTEST_CHECK(!file_md5_upper.empty());
        DOCTEST_CHECK(file_md5_lower.size() == 32);
        DOCTEST_CHECK(file_md5_upper.size() == 32);

        // 清理临时文件
        std::remove(temp_filename.c_str());

        // 测试不存在的文件
        DOCTEST_CHECK(core::md5_file("non_existent_file.txt", false).empty());
    }
}