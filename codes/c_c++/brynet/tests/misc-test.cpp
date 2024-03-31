#include "gtest/gtest.h"
#include "quill/Fmt.h"
#include "util/singleton.hpp"
#include "util/stack_trace.h"
#include "time/timespan.h"
#include "time/timestamp.h"
#include "time/datetime.h"

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

GTEST_TEST(MiscTest, Singleton)
{
    auto& st = SingletonTest::instance();
    ASSERT_TRUE(st.GetFile().empty());
    ASSERT_TRUE(st.GetLine() == 0);

    st.SetFile("hello");
    st.SetLine(10);
    ASSERT_TRUE(st.GetFile() == "hello");
    ASSERT_TRUE(st.GetLine() == 10);
}

GTEST_TEST(MiscTest, StackTrace)
{
    const std::string callstack = core::stack_trace().to_string();
    ASSERT_TRUE(!callstack.empty());

    fmt::print("-- stack trace --\n{}-----------------\n", callstack);
}

GTEST_TEST(MiscTest, Timespan)
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
    ASSERT_TRUE(span5 + span6 == 2001);
    ASSERT_TRUE(span5 + 1001  == 2001);
    ASSERT_TRUE(span5 - span6 == -1);
    ASSERT_TRUE(span5 - 1001  == -1);

    span5 += span6;
    ASSERT_TRUE(span5 == 2001);

    span5 -= span6;
    ASSERT_TRUE(span5 == 1000);

    span5 += 1001;
    ASSERT_TRUE(span5 == 2001);

    span5 -= 1001;
    ASSERT_TRUE(span5 == 1000);
}

GTEST_TEST(MiscTest, Timestamp)
{
    core::timestamp ts1(100 * core::timestamp::resolution);
    core::timestamp ts2(101 * core::timestamp::resolution);
    ASSERT_TRUE(ts1 + core::timestamp::resolution == ts2);
    ASSERT_TRUE(ts2 - core::timestamp::resolution == ts1);
    ASSERT_TRUE(ts1 + core::timespan(core::timestamp::resolution) == ts2);
    ASSERT_TRUE(ts2 - core::timespan(core::timestamp::resolution) == ts1);

    ts1 += core::timestamp::resolution;
    ASSERT_TRUE(ts1 == ts2);

    ts1 -= core::timestamp::resolution;
    ASSERT_TRUE(ts1 == ts2 - core::timestamp::resolution);

    ts1 += core::timespan(core::timestamp::resolution);
    ASSERT_TRUE(ts1 == ts2);

    ts1 -= core::timespan(core::timestamp::resolution);
    ASSERT_TRUE(ts1 == ts2 - core::timestamp::resolution);
}

GTEST_TEST(MiscTest, DateTime)
{
    {
        // default constructor
        core::datetime dt1;

        // copy constructor from DateTimeEx
        core::datetime dt2(dt1);
        ASSERT_TRUE(dt1 == dt2);
        ASSERT_TRUE(dt1.make_timestamp() == dt2.make_timestamp());

        // copy constructor from Timestamp
        core::datetime dt3(dt1.make_timestamp());
        ASSERT_TRUE(dt1 == dt3);
        ASSERT_TRUE(dt1.make_timestamp() == dt3.make_timestamp());

        // copy constructor from tm struct
        core::datetime dt4(dt1.make_tm());
        ASSERT_TRUE(dt1.year() == dt4.year());
        ASSERT_TRUE(dt1.month() == dt4.month());
        ASSERT_TRUE(dt1.day() == dt4.day());
        ASSERT_TRUE(dt1.day_of_week() == dt4.day_of_week());
        ASSERT_TRUE(dt1.day_of_year() == dt4.day_of_year());
        ASSERT_TRUE(dt1.hour() == dt4.hour());
        ASSERT_TRUE(dt1.minute() == dt4.minute());
        ASSERT_TRUE(dt1.second() == dt4.second());

        // copy constructor from the given local date and time
        core::datetime dt5(dt1.year(), dt1.month(), dt1.day(), dt1.hour(), dt1.minute(), dt1.second(), dt1.millisecond(), dt1.microsecond());
        ASSERT_TRUE(dt1 == dt5);
        ASSERT_TRUE(dt1.make_timestamp() == dt5.make_timestamp());

        core::datetime dt6;

        // assigns a datetime
        dt6 += core::timespan(std::chrono::hours(1));
        dt6  = dt1;
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.make_timestamp() == dt6.make_timestamp());

        // assigns a timestamp
        dt6 += core::timespan(std::chrono::hours(1));
        dt6  = dt1.make_timestamp();
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.make_timestamp() == dt6.make_timestamp());

        // assigns a tm struct
        dt6 += core::timespan(std::chrono::hours(1));
        dt6  = dt1.make_tm();
        ASSERT_TRUE(dt1.year() == dt6.year());
        ASSERT_TRUE(dt1.month() == dt6.month());
        ASSERT_TRUE(dt1.day() == dt6.day());
        ASSERT_TRUE(dt1.day_of_week() == dt6.day_of_week());
        ASSERT_TRUE(dt1.day_of_year() == dt6.day_of_year());
        ASSERT_TRUE(dt1.hour() == dt6.hour());
        ASSERT_TRUE(dt1.minute() == dt6.minute());
        ASSERT_TRUE(dt1.second() == dt6.second());

        // assigns a Timestamp
        dt6 += core::timespan(std::chrono::hours(1));
        dt6.assign(dt1.make_timestamp());
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.make_timestamp() == dt6.make_timestamp());

        // assigns a tm struct
        dt6 += core::timespan(std::chrono::hours(1));
        dt6.assign(dt1.make_tm());
        ASSERT_TRUE(dt1.year() == dt6.year());
        ASSERT_TRUE(dt1.month() == dt6.month());
        ASSERT_TRUE(dt1.day() == dt6.day());
        ASSERT_TRUE(dt1.day_of_week() == dt6.day_of_week());
        ASSERT_TRUE(dt1.day_of_year() == dt6.day_of_year());
        ASSERT_TRUE(dt1.hour() == dt6.hour());
        ASSERT_TRUE(dt1.minute() == dt6.minute());
        ASSERT_TRUE(dt1.second() == dt6.second());

        // assigns a local date and time
        dt6 += core::timespan(std::chrono::hours(1));
        dt6.assign(dt1.year(), dt1.month(), dt1.day(), dt1.hour(), dt1.minute(), dt1.second(), dt1.millisecond(), dt1.microsecond());
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.make_timestamp() == dt6.make_timestamp());

        dt6 += core::timespan(std::chrono::hours(1));
        ASSERT_TRUE(dt1 != dt6);
        ASSERT_TRUE(dt1 <  dt6);
        ASSERT_TRUE(dt1 <= dt6);
        ASSERT_TRUE(dt6 >  dt1);
        ASSERT_TRUE(dt6 >= dt1);

        ASSERT_TRUE(dt6 == dt1 + core::timespan(std::chrono::hours(1)));
        ASSERT_TRUE(dt1 == dt6 - core::timespan(std::chrono::hours(1)));

        core::timestamp ts(1314521000 * core::timestamp::resolution);
        dt5 = ts;
        dt6 = ts + core::timespan(std::chrono::seconds(5211314));

        const auto diff = dt6 - dt5;
        ASSERT_TRUE(diff.total_seconds() == 5211314);
    }

    {
        // 1970-01-01 08:00:00 Thursday
        core::datetime dt(core::timestamp(0));
        ASSERT_TRUE(dt.year() == 1970);
        ASSERT_TRUE(dt.month() == 1);
        ASSERT_TRUE(dt.day() == 1);
        ASSERT_TRUE(dt.hour() == 8);
        ASSERT_TRUE(dt.minute() == 0);
        ASSERT_TRUE(dt.second() == 0);
        ASSERT_TRUE(dt.millisecond() == 0);
        ASSERT_TRUE(dt.microsecond() == 0);
        ASSERT_TRUE(dt.day_of_week() == 4);
        ASSERT_TRUE(dt.make_timestamp().epoch_time() == 0);

        // 1970-01-01 08:00:00 Thursday
        core::datetime dtex(dt);
        ASSERT_TRUE(dtex.year() == 1970);
        ASSERT_TRUE(dtex.month() == 1);
        ASSERT_TRUE(dtex.day() == 1);
        ASSERT_TRUE(dtex.hour() == 8);
        ASSERT_TRUE(dtex.minute() == 0);
        ASSERT_TRUE(dtex.second() == 0);
        ASSERT_TRUE(dtex.millisecond() == 0);
        ASSERT_TRUE(dtex.microsecond() == 0);
        ASSERT_TRUE(dtex.day_of_week() == 4);
        ASSERT_TRUE(dtex.make_timestamp().epoch_time() == 0);

        dtex += core::timespan(std::chrono::hours(1));
        dtex  = dt;
        ASSERT_TRUE(dtex.year() == 1970);
        ASSERT_TRUE(dtex.month() == 1);
        ASSERT_TRUE(dtex.day() == 1);
        ASSERT_TRUE(dtex.hour() == 8);
        ASSERT_TRUE(dtex.minute() == 0);
        ASSERT_TRUE(dtex.second() == 0);
        ASSERT_TRUE(dtex.millisecond() == 0);
        ASSERT_TRUE(dtex.microsecond() == 0);
        ASSERT_TRUE(dtex.day_of_week() == 4);
        ASSERT_TRUE(dtex.make_timestamp().epoch_time() == 0);

        dtex += core::timespan(std::chrono::hours(1));
        dtex  = dt;
        ASSERT_TRUE(dtex.year() == 1970);
        ASSERT_TRUE(dtex.month() == 1);
        ASSERT_TRUE(dtex.day() == 1);
        ASSERT_TRUE(dtex.hour() == 8);
        ASSERT_TRUE(dtex.minute() == 0);
        ASSERT_TRUE(dtex.second() == 0);
        ASSERT_TRUE(dtex.millisecond() == 0);
        ASSERT_TRUE(dtex.microsecond() == 0);
        ASSERT_TRUE(dtex.day_of_week() == 4);
        ASSERT_TRUE(dtex.make_timestamp().epoch_time() == 0);

        ASSERT_TRUE(dtex - dt == 0);
    }

    // 1970-01-01 08:00:00 Thursday
    core::datetime dt(core::timestamp(0));
    ASSERT_TRUE(dt.year() == 1970);
    ASSERT_TRUE(dt.month() == 1);
    ASSERT_TRUE(dt.day() == 1);
    ASSERT_TRUE(dt.hour() == 8);
    ASSERT_TRUE(dt.minute() == 0);
    ASSERT_TRUE(dt.second() == 0);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.day_of_week() == 4);
    ASSERT_TRUE(dt.make_timestamp().epoch_time() == 0);

    tm tmStruct = dt.make_tm();
    ASSERT_TRUE(tmStruct.tm_year == 70);
    ASSERT_TRUE(tmStruct.tm_mon == 0);
    ASSERT_TRUE(tmStruct.tm_mday == 1);
    ASSERT_TRUE(tmStruct.tm_hour == 8);
    ASSERT_TRUE(tmStruct.tm_min == 0);
    ASSERT_TRUE(tmStruct.tm_sec == 0);

    dt = tmStruct;
    ASSERT_TRUE(dt.year() == 1970);
    ASSERT_TRUE(dt.month() == 1);
    ASSERT_TRUE(dt.day() == 1);
    ASSERT_TRUE(dt.hour() == 8);
    ASSERT_TRUE(dt.minute() == 0);
    ASSERT_TRUE(dt.second() == 0);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.day_of_week() == 4);
    ASSERT_TRUE(dt.make_timestamp().epoch_time() == 0);

    // 2001-09-09 09:46:40 Sunday
    dt = core::timestamp(1000000000 * core::timestamp::resolution);
    ASSERT_TRUE(dt.year() == 2001);
    ASSERT_TRUE(dt.month() == 9);
    ASSERT_TRUE(dt.day() == 9);
    ASSERT_TRUE(dt.hour() == 9);
    ASSERT_TRUE(dt.minute() == 46);
    ASSERT_TRUE(dt.second() == 40);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.day_of_week() == 0);
    ASSERT_TRUE(dt.make_timestamp().epoch_time() == 1000000000);

    // 3000-12-31 23:59:59 Friday
    dt = core::datetime(3000, 12, 31, 23, 59, 59);
    ASSERT_TRUE(dt.year() == 3000);
    ASSERT_TRUE(dt.month() == 12);
    ASSERT_TRUE(dt.day() == 31);
    ASSERT_TRUE(dt.hour() == 23);
    ASSERT_TRUE(dt.minute() == 59);
    ASSERT_TRUE(dt.second() == 59);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.day_of_week() == 3);
    ASSERT_TRUE(dt.make_timestamp().epoch_time() == time_t{32535187199});

    // 2011-08-28 16:43:20 Sunday
    dt = core::datetime(2011, 8, 28, 16, 43, 20);
    ASSERT_TRUE(dt.year() == 2011);
    ASSERT_TRUE(dt.month() == 8);
    ASSERT_TRUE(dt.day() == 28);
    ASSERT_TRUE(dt.hour() == 16);
    ASSERT_TRUE(dt.minute() == 43);
    ASSERT_TRUE(dt.second() == 20);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.day_of_week() == 0);
    ASSERT_TRUE(dt.make_timestamp().epoch_time() == 1314521000);

    core::datetime dt1 = dt + core::timespan(std::chrono::hours(1));
    ASSERT_TRUE(dt1.year() == 2011);
    ASSERT_TRUE(dt1.month() == 8);
    ASSERT_TRUE(dt1.day() == 28);
    ASSERT_TRUE(dt1.hour() == 17);
    ASSERT_TRUE(dt1.minute() == 43);
    ASSERT_TRUE(dt1.second() == 20);
    ASSERT_TRUE(dt1.millisecond() == 0);
    ASSERT_TRUE(dt1.microsecond() == 0);
    ASSERT_TRUE(dt1.day_of_week() == 0);
    ASSERT_TRUE(dt1.make_timestamp().epoch_time() == 1314524600);

    core::datetime dt2 = dt1 - core::timespan(std::chrono::hours(1));
    ASSERT_TRUE(dt2.year() == 2011);
    ASSERT_TRUE(dt2.month() == 8);
    ASSERT_TRUE(dt2.day() == 28);
    ASSERT_TRUE(dt2.hour() == 16);
    ASSERT_TRUE(dt2.minute() == 43);
    ASSERT_TRUE(dt2.second() == 20);
    ASSERT_TRUE(dt2.millisecond() == 0);
    ASSERT_TRUE(dt2.microsecond() == 0);
    ASSERT_TRUE(dt2.day_of_week() == 0);
    ASSERT_TRUE(dt2.make_timestamp().epoch_time() == 1314521000);

    const auto span = dt1 - dt2;
    ASSERT_TRUE(span.days() == 0);
    ASSERT_TRUE(span.hours() == 1);
    ASSERT_TRUE(span.total_hours() == 1);
    ASSERT_TRUE(span.minutes() == 0);
    ASSERT_TRUE(span.total_minutes() == 60);
    ASSERT_TRUE(span.seconds() == 0);
    ASSERT_TRUE(span.total_seconds() == 3600);
    ASSERT_TRUE(span.milliseconds() == 0);
    ASSERT_TRUE(span.total_milliseconds() == 3600000);
}