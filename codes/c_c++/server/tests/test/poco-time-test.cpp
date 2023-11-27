#include "gtest/gtest.h"
#include "fmt/core.h"
#include "util/types.h"
#include "util/datetime_ex.h"
#include "util/timestamp_ex.h"
#include "Poco/Thread.h"
#include "Poco/Timestamp.h"
#include "Poco/Timespan.h"
#include "Poco/Timezone.h"
#include "Poco/Clock.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeParser.h"

GTEST_TEST(PocoTimeTest, Timestamp)
{
    Poco::Timestamp t1;
    Poco::Thread::sleep(1);

    Poco::Timestamp t2;
    Poco::Timestamp t3 = t2;

    ASSERT_TRUE(t1 != t2);
    ASSERT_TRUE(!(t1 == t2));
    ASSERT_TRUE(t2 > t1);
    ASSERT_TRUE(t2 >= t1);
    ASSERT_TRUE(!(t1 > t2));
    ASSERT_TRUE(!(t1 >= t2));
    ASSERT_TRUE(t2 == t3);
    ASSERT_TRUE(!(t2 != t3));
    ASSERT_TRUE(t2 >= t3);
    ASSERT_TRUE(t2 <= t3);

    Poco::Timestamp::TimeDiff d = (t2 - t1);
    ASSERT_TRUE(d >= 1000);

    t1.swap(t2);
    ASSERT_TRUE(t1 > t2);
    t2.swap(t1);
    ASSERT_TRUE(t2 > t1);

    Poco::Timestamp::UtcTimeVal tv = t1.utcTime();
    Poco::Timestamp             t4 = Poco::Timestamp::fromUtcTime(tv);
    ASSERT_TRUE(t1 == t4);

    Poco::Timestamp epoch(0);
    tv = epoch.utcTime();
    ASSERT_TRUE(tv >> 32 == 0x01B21DD2);
    ASSERT_TRUE((tv & 0xFFFFFFFF) == 0x13814000);

    Poco::Timestamp now;
    Poco::Thread::sleep(1);
    ASSERT_TRUE(now.elapsed() >= 1000);
    ASSERT_TRUE(now.isElapsed(1000));
}

GTEST_TEST(PocoTimeTest, TimestampEx)
{
    {
        // default constructor
        common::timestamp_ex ts1;

        // copy constructor from timestamp_ex
        common::timestamp_ex ts2(ts1);
        ASSERT_TRUE(ts1.year() == ts2.year());
        ASSERT_TRUE(ts1.month() == ts2.month());
        ASSERT_TRUE(ts1.day() == ts2.day());
        ASSERT_TRUE(ts1.day_of_week() == ts2.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts2.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts2.hour());
        ASSERT_TRUE(ts1.minute() == ts2.minute());
        ASSERT_TRUE(ts1.second() == ts2.second());
        ASSERT_TRUE(ts1 == ts2);
        ASSERT_TRUE(ts1.timestamp() == ts2.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts2.epoch_time());
        ASSERT_TRUE(ts1.epoch_microseconds() == ts2.epoch_microseconds());

        // copy constructor from Timestamp
        common::timestamp_ex ts3(ts1.timestamp());
        ASSERT_TRUE(ts1.year() == ts3.year());
        ASSERT_TRUE(ts1.month() == ts3.month());
        ASSERT_TRUE(ts1.day() == ts3.day());
        ASSERT_TRUE(ts1.day_of_week() == ts3.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts3.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts3.hour());
        ASSERT_TRUE(ts1.minute() == ts3.minute());
        ASSERT_TRUE(ts1.second() == ts3.second());
        ASSERT_TRUE(ts1 == ts3);
        ASSERT_TRUE(ts1.timestamp() == ts3.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts3.epoch_time());
        ASSERT_TRUE(ts1.epoch_microseconds() == ts3.epoch_microseconds());

        // copy constructor from Unix epoch
        common::timestamp_ex ts4(ts1.epoch_time());
        ASSERT_TRUE(ts1.year() == ts4.year());
        ASSERT_TRUE(ts1.month() == ts4.month());
        ASSERT_TRUE(ts1.day() == ts4.day());
        ASSERT_TRUE(ts1.day_of_week() == ts4.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts4.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts4.hour());
        ASSERT_TRUE(ts1.minute() == ts4.minute());
        ASSERT_TRUE(ts1.second() == ts4.second());
        //ASSERT_TRUE(ts1 == ts4);
        //ASSERT_TRUE(ts1.timestamp() == ts4.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts4.epoch_time());
        //ASSERT_TRUE(ts1.epoch_microseconds() == ts4.epoch_microseconds());

        // copy constructor from tm struct
        common::timestamp_ex ts5(ts1.epoch_time());
        ASSERT_TRUE(ts1.year() == ts5.year());
        ASSERT_TRUE(ts1.month() == ts5.month());
        ASSERT_TRUE(ts1.day() == ts5.day());
        ASSERT_TRUE(ts1.day_of_week() == ts5.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts5.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts5.hour());
        ASSERT_TRUE(ts1.minute() == ts5.minute());
        ASSERT_TRUE(ts1.second() == ts5.second());
        //ASSERT_TRUE(ts1 == ts5);
        //ASSERT_TRUE(ts1.timestamp() == ts5.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts5.epoch_time());
        //ASSERT_TRUE(ts1.epoch_microseconds() == ts5.epoch_microseconds());

        // copy constructor from the given Gregorian local date and time
        common::timestamp_ex ts6(ts1.year(), ts1.month(), ts1.day(), ts1.hour(), ts1.minute(), ts1.second(), ts1.millisecond(), ts1.microsecond());
        ASSERT_TRUE(ts1.year() == ts6.year());
        ASSERT_TRUE(ts1.month() == ts6.month());
        ASSERT_TRUE(ts1.day() == ts6.day());
        ASSERT_TRUE(ts1.day_of_week() == ts6.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts6.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts6.hour());
        ASSERT_TRUE(ts1.minute() == ts6.minute());
        ASSERT_TRUE(ts1.second() == ts6.second());
        ASSERT_TRUE(ts1 == ts6);
        ASSERT_TRUE(ts1.timestamp() == ts6.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts6.epoch_time());
        ASSERT_TRUE(ts1.epoch_microseconds() == ts6.epoch_microseconds());

        common::timestamp_ex ts7;

        // assigns a timestamp_ex
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7  = ts1;
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts7.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        ASSERT_TRUE(ts1 == ts7);
        ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());

        // assigns a Timestamp
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7  = ts1.timestamp();
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts7.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        ASSERT_TRUE(ts1 == ts7);
        ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());

        // assigns a Unix epoch
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7  = ts1.epoch_time();
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts7.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        //ASSERT_TRUE(ts1 == ts7);
        //ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        //ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());

        // assigns a tm struct
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7  = ts1.make_tm();
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts7.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        //ASSERT_TRUE(ts1 == ts7);
        //ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        //ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());

        // assigns a timestamp_ex
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7.assign(ts1);
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts7.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        ASSERT_TRUE(ts1 == ts7);
        ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());

        // assigns a Timestamp
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7.assign(ts1.timestamp());
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        ASSERT_TRUE(ts1 == ts7);
        ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());

        // assigns a Unix epoch
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7.assign(ts1.epoch_time());
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts7.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        //ASSERT_TRUE(ts1 == ts7);
        //ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        //ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());

        // assigns a tm struct
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7.assign(ts1.make_tm());
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts7.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        //ASSERT_TRUE(ts1 == ts7);
        //ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        //ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());

        // assigns a Gregorian local date and time
        ts7 += Poco::Timespan(std::chrono::hours(1));
        ts7.assign(ts1.year(), ts1.month(), ts1.day(), ts1.hour(), ts1.minute(), ts1.second(), ts1.millisecond(), ts1.microsecond());
        ASSERT_TRUE(ts1.year() == ts7.year());
        ASSERT_TRUE(ts1.month() == ts7.month());
        ASSERT_TRUE(ts1.day() == ts7.day());
        ASSERT_TRUE(ts1.day_of_week() == ts7.day_of_week());
        ASSERT_TRUE(ts1.day_of_year() == ts7.day_of_year());
        ASSERT_TRUE(ts1.hour() == ts7.hour());
        ASSERT_TRUE(ts1.minute() == ts7.minute());
        ASSERT_TRUE(ts1.second() == ts7.second());
        ASSERT_TRUE(ts1 == ts7);
        ASSERT_TRUE(ts1.timestamp() == ts7.timestamp());
        ASSERT_TRUE(ts1.epoch_time() == ts7.epoch_time());
        ASSERT_TRUE(ts1.epoch_microseconds() == ts7.epoch_microseconds());
    }

    // 1970-01-01 00:00:00 Thursday
    Poco::DateTime ds(Poco::Timestamp::fromEpochTime(0));
    ASSERT_TRUE(ds.year() == 1970);
    ASSERT_TRUE(ds.month() == 1);
    ASSERT_TRUE(ds.day() == 1);
    ASSERT_TRUE(ds.dayOfWeek() == 4);
    ASSERT_TRUE(ds.dayOfYear() == 1);
    ASSERT_TRUE(ds.hour() == 0);
    ASSERT_TRUE(ds.minute() == 0);
    ASSERT_TRUE(ds.second() == 0);
    ASSERT_TRUE(ds.millisecond() == 0);
    ASSERT_TRUE(ds.microsecond() == 0);
    ASSERT_TRUE(ds.timestamp().epochTime() == 0);

    // 1970-01-01 08:00:00 Thursday
    common::timestamp_ex ts(Poco::Timestamp::fromEpochTime(0));
    ASSERT_TRUE(ts.year() == 1970);
    ASSERT_TRUE(ts.month() == 1);
    ASSERT_TRUE(ts.day() == 1);
    ASSERT_TRUE(ts.day_of_week() == 4);
    ASSERT_TRUE(ts.day_of_year() == 1);
    ASSERT_TRUE(ts.hour() == 8);
    ASSERT_TRUE(ts.minute() == 0);
    ASSERT_TRUE(ts.second() == 0);
    ASSERT_TRUE(ts.millisecond() == 0);
    ASSERT_TRUE(ts.microsecond() == 0);
    ASSERT_TRUE(ts.epoch_time() == 0);
    ASSERT_TRUE(ts.epoch_microseconds() == 0);
    ASSERT_TRUE(ts.timestamp().epochTime() == 0);

    tm tmStruct = ts.make_tm();
    ASSERT_TRUE(tmStruct.tm_year == 70);
    ASSERT_TRUE(tmStruct.tm_mon == 0);
    ASSERT_TRUE(tmStruct.tm_mday == 1);
    ASSERT_TRUE(tmStruct.tm_hour == 8);
    ASSERT_TRUE(tmStruct.tm_min == 0);
    ASSERT_TRUE(tmStruct.tm_sec == 0);
    ASSERT_TRUE(tmStruct.tm_wday == 4);
    ASSERT_TRUE(tmStruct.tm_yday == 0);

    ts = tmStruct;
    ASSERT_TRUE(ts.year() == 1970);
    ASSERT_TRUE(ts.month() == 1);
    ASSERT_TRUE(ts.day() == 1);
    ASSERT_TRUE(ts.day_of_week() == 4);
    ASSERT_TRUE(ts.day_of_year() == 1);
    ASSERT_TRUE(ts.hour() == 8);
    ASSERT_TRUE(ts.minute() == 0);
    ASSERT_TRUE(ts.second() == 0);
    ASSERT_TRUE(ts.millisecond() == 0);
    ASSERT_TRUE(ts.microsecond() == 0);
    ASSERT_TRUE(ts.epoch_time() == 0);
    ASSERT_TRUE(ts.epoch_microseconds() == 0);
    ASSERT_TRUE(ts.timestamp().epochTime() == 0);

    ts = ds;
    ASSERT_TRUE(ts.year() == 1970);
    ASSERT_TRUE(ts.month() == 1);
    ASSERT_TRUE(ts.day() == 1);
    ASSERT_TRUE(ts.day_of_week() == 4);
    ASSERT_TRUE(ts.day_of_year() == 1);
    ASSERT_TRUE(ts.hour() == 8);
    ASSERT_TRUE(ts.minute() == 0);
    ASSERT_TRUE(ts.second() == 0);
    ASSERT_TRUE(ts.millisecond() == 0);
    ASSERT_TRUE(ts.microsecond() == 0);
    ASSERT_TRUE(ts.epoch_time() == 0);
    ASSERT_TRUE(ts.epoch_microseconds() == 0);
    ASSERT_TRUE(ts.timestamp().epochTime() == 0);

    // 2001-09-09 09:46:40 Sunday
    ts = Poco::Timestamp::fromEpochTime(1000000000);
    ASSERT_TRUE(ts.year() == 2001);
    ASSERT_TRUE(ts.month() == 9);
    ASSERT_TRUE(ts.day() == 9);
    ASSERT_TRUE(ts.day_of_week() == 0);
    ASSERT_TRUE(ts.day_of_year() == 252);
    ASSERT_TRUE(ts.hour() == 9);
    ASSERT_TRUE(ts.minute() == 46);
    ASSERT_TRUE(ts.second() == 40);
    ASSERT_TRUE(ts.millisecond() == 0);
    ASSERT_TRUE(ts.microsecond() == 0);
    ASSERT_TRUE(ts.epoch_time() == 1000000000);
    ASSERT_TRUE(ts.epoch_microseconds() == Poco::Timespan(1000000000, 0).totalMicroseconds());
    ASSERT_TRUE(ts.timestamp().epochTime() == 1000000000);

    // 2011-08-28 16:43:20 Sunday
    ts = Poco::Timestamp::fromEpochTime(1314521000);
    ASSERT_TRUE(ts.year() == 2011);
    ASSERT_TRUE(ts.month() == 8);
    ASSERT_TRUE(ts.day() == 28);
    ASSERT_TRUE(ts.day_of_week() == 0);
    ASSERT_TRUE(ts.day_of_year() == 240);
    ASSERT_TRUE(ts.hour() == 16);
    ASSERT_TRUE(ts.minute() == 43);
    ASSERT_TRUE(ts.second() == 20);
    ASSERT_TRUE(ts.millisecond() == 0);
    ASSERT_TRUE(ts.microsecond() == 0);
    ASSERT_TRUE(ts.epoch_time() == 1314521000);
    ASSERT_TRUE(ts.epoch_microseconds() == Poco::Timespan(1314521000, 0).totalMicroseconds());
    ASSERT_TRUE(ts.timestamp().epochTime() == 1314521000);

    common::timestamp_ex ts1 = ts + Poco::Timespan(std::chrono::hours(1));
    ASSERT_TRUE(ts1.year() == 2011);
    ASSERT_TRUE(ts1.month() == 8);
    ASSERT_TRUE(ts1.day() == 28);
    ASSERT_TRUE(ts1.day_of_week() == 0);
    ASSERT_TRUE(ts1.day_of_year() == 240);
    ASSERT_TRUE(ts1.hour() == 17);
    ASSERT_TRUE(ts1.minute() == 43);
    ASSERT_TRUE(ts1.second() == 20);
    ASSERT_TRUE(ts1.millisecond() == 0);
    ASSERT_TRUE(ts1.microsecond() == 0);
    ASSERT_TRUE(ts1.epoch_time() == 1314524600);
    ASSERT_TRUE(ts1.epoch_microseconds() == Poco::Timespan(1314524600, 0).totalMicroseconds());
    ASSERT_TRUE(ts1.timestamp().epochTime() == 1314524600);

    common::timestamp_ex ts2 = ts1 - Poco::Timespan(std::chrono::hours(1));
    ASSERT_TRUE(ts2.year() == 2011);
    ASSERT_TRUE(ts2.month() == 8);
    ASSERT_TRUE(ts2.day() == 28);
    ASSERT_TRUE(ts2.day_of_week() == 0);
    ASSERT_TRUE(ts2.day_of_year() == 240);
    ASSERT_TRUE(ts2.hour() == 16);
    ASSERT_TRUE(ts2.minute() == 43);
    ASSERT_TRUE(ts2.second() == 20);
    ASSERT_TRUE(ts2.millisecond() == 0);
    ASSERT_TRUE(ts2.microsecond() == 0);
    ASSERT_TRUE(ts2.epoch_time() == 1314521000);
    ASSERT_TRUE(ts2.epoch_microseconds() == Poco::Timespan(1314521000, 0).totalMicroseconds());
    ASSERT_TRUE(ts2.timestamp().epochTime() == 1314521000);

    Poco::Timespan span = ts1 - ts2;
    ASSERT_TRUE(span.days() == 0);
    ASSERT_TRUE(span.hours() == 1);
    ASSERT_TRUE(span.totalHours() == 1);
    ASSERT_TRUE(span.minutes() == 0);
    ASSERT_TRUE(span.totalMinutes() == 60);
    ASSERT_TRUE(span.seconds() == 0);
    ASSERT_TRUE(span.totalSeconds() == 3600);
    ASSERT_TRUE(span.milliseconds() == 0);
    ASSERT_TRUE(span.totalMilliseconds() == 3600000);
}

GTEST_TEST(PocoTimeTest, Clock)
{
    Poco::Clock t1;
    Poco::Thread::sleep(1);

    Poco::Clock t2;
    Poco::Clock t3 = t2;

    ASSERT_TRUE(t1 != t2);
    ASSERT_TRUE(!(t1 == t2));
    ASSERT_TRUE(t2 > t1);
    ASSERT_TRUE(t2 >= t1);
    ASSERT_TRUE(!(t1 > t2));
    ASSERT_TRUE(!(t1 >= t2));
    ASSERT_TRUE(t2 == t3);
    ASSERT_TRUE(!(t2 != t3));
    ASSERT_TRUE(t2 >= t3);
    ASSERT_TRUE(t2 <= t3);

    Poco::Clock::ClockDiff d = (t2 - t1);
    ASSERT_TRUE(d >= 1000);

    Poco::Clock::ClockDiff acc = Poco::Clock::accuracy();
    ASSERT_TRUE(acc > 0 && acc < Poco::Clock::resolution() && Poco::Clock::monotonic());

    t1.swap(t2);
    ASSERT_TRUE(t1 > t2);
    t2.swap(t1);
    ASSERT_TRUE(t2 > t1);

    Poco::Clock now;
    Poco::Thread::sleep(1);
    ASSERT_TRUE(now.elapsed() >= 1000);
    ASSERT_TRUE(now.isElapsed(1000));
}

GTEST_TEST(PocoTimeTest, TimespanConversions)
{
    Poco::Timespan ts;
    ASSERT_TRUE(ts.totalMicroseconds() == 0);

    ts = Poco::Timespan::DAYS;
    ASSERT_TRUE(ts.totalMicroseconds() == Poco::Timespan::DAYS);
    ASSERT_TRUE(ts.totalMilliseconds() == 86400000);
    ASSERT_TRUE(ts.totalSeconds() == 86400);
    ASSERT_TRUE(ts.totalMinutes() == 60 * 24);
    ASSERT_TRUE(ts.totalHours() == 24);
    ASSERT_TRUE(ts.days() == 1);

    ASSERT_TRUE(ts.microseconds() == 0);
    ASSERT_TRUE(ts.milliseconds() == 0);
    ASSERT_TRUE(ts.seconds() == 0);
    ASSERT_TRUE(ts.minutes() == 0);
    ASSERT_TRUE(ts.hours() == 0);

    ts.assign(2, 12, 30, 10, 123456);
    ASSERT_TRUE(ts.microseconds() == 456);
    ASSERT_TRUE(ts.milliseconds() == 123);
    ASSERT_TRUE(ts.seconds() == 10);
    ASSERT_TRUE(ts.minutes() == 30);
    ASSERT_TRUE(ts.hours() == 12);
    ASSERT_TRUE(ts.days() == 2);

    ts.assign(0, 36, 30, 10, 123456);
    ASSERT_TRUE(ts.microseconds() == 456);
    ASSERT_TRUE(ts.milliseconds() == 123);
    ASSERT_TRUE(ts.useconds() == 123456);
    ASSERT_TRUE(ts.seconds() == 10);
    ASSERT_TRUE(ts.minutes() == 30);
    ASSERT_TRUE(ts.hours() == 12);
    ASSERT_TRUE(ts.days() == 1);

    ts.assign(0, 0, 2190, 10, 123456);
    ASSERT_TRUE(ts.microseconds() == 456);
    ASSERT_TRUE(ts.milliseconds() == 123);
    ASSERT_TRUE(ts.useconds() == 123456);
    ASSERT_TRUE(ts.seconds() == 10);
    ASSERT_TRUE(ts.minutes() == 30);
    ASSERT_TRUE(ts.hours() == 12);
    ASSERT_TRUE(ts.days() == 1);

    ts.assign(std::chrono::minutes(62));
    ASSERT_TRUE(ts.hours() == 1);
    ASSERT_TRUE(ts.minutes() == 2);
}

GTEST_TEST(PocoTimeTest, TimespanComparisons)
{
    Poco::Timespan ts1(10000000);
    Poco::Timespan ts2(20000000);
    Poco::Timespan ts3(20000000);

    ASSERT_TRUE(ts1 != ts2);
    ASSERT_TRUE(!(ts1 == ts2));
    ASSERT_TRUE(ts1 <= ts2);
    ASSERT_TRUE(ts1 < ts2);
    ASSERT_TRUE(ts2 > ts1);
    ASSERT_TRUE(ts2 >= ts1);

    ASSERT_TRUE(ts2 == ts3);
    ASSERT_TRUE(!(ts2 != ts3));
    ASSERT_TRUE(ts2 >= ts3);
    ASSERT_TRUE(ts2 <= ts3);
    ASSERT_TRUE(!(ts2 > ts3));
    ASSERT_TRUE(!(ts2 < ts3));

    ASSERT_TRUE(ts1 == 10000000);
    ASSERT_TRUE(ts1 != 20000000);
    ASSERT_TRUE(ts1 <= 10000000);
    ASSERT_TRUE(ts1 <= 20000000);
    ASSERT_TRUE(ts1 >= 10000000);
    ASSERT_TRUE(ts1 >= 5000000);
    ASSERT_TRUE(ts1 < 20000000);
    ASSERT_TRUE(ts1 > 5000000);
}

GTEST_TEST(PocoTimeTest, TimespanArithmetics)
{
    Poco::Timespan ts1(100000000);
    Poco::Timespan ts2(50000000);
    Poco::Timespan ts3;

    ts3 = ts1 + ts2;
    ASSERT_TRUE(ts3 == 150000000);

    ts3 = ts1 + 30000000;
    ASSERT_TRUE(ts3 == 130000000);

    ts3 = ts1 - ts2;
    ASSERT_TRUE(ts3 == 50000000);

    ts3 = ts1 - 20000000;
    ASSERT_TRUE(ts3 == 80000000);

    ts3 += 20000000;
    ASSERT_TRUE(ts3 == ts1);

    ts3 -= ts2;
    ASSERT_TRUE(ts3 == ts2);
}

GTEST_TEST(PocoTimeTest, Timezone)
{
    fmt::print("Timezone::name         = {}\n", Poco::Timezone::name());
    fmt::print("Timezone::dstName      = {}\n", Poco::Timezone::dstName());
    fmt::print("Timezone::standardName = {}\n", Poco::Timezone::standardName());
    fmt::print("Timezone::utcOffset    = {}\n", Poco::Timezone::utcOffset());
    fmt::print("Timezone::dst          = {}\n", Poco::Timezone::dst());
    fmt::print("Timezone::tzd          = {}\n", Poco::Timezone::tzd());
}

GTEST_TEST(PocoTimeTest, DateTime)
{
    // Unix epoch 1970-01-01 00:00:00 Thursday
    Poco::Timestamp ts(0);
    Poco::DateTime  dt(ts);

    ASSERT_TRUE(dt.year() == 1970);
    ASSERT_TRUE(dt.month() == 1);
    ASSERT_TRUE(dt.day() == 1);
    ASSERT_TRUE(dt.hour() == 0);
    ASSERT_TRUE(dt.minute() == 0);
    ASSERT_TRUE(dt.second() == 0);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.dayOfWeek() == 4);
    ASSERT_TRUE(dt.timestamp().epochTime() == 0);

    // 2001-09-09 01:46:40 Sunday
    ts = Poco::Timestamp::fromEpochTime(1000000000);
    dt = ts;

    ASSERT_TRUE(dt.year() == 2001);
    ASSERT_TRUE(dt.month() == 9);
    ASSERT_TRUE(dt.day() == 9);
    ASSERT_TRUE(dt.hour() == 1);
    ASSERT_TRUE(dt.minute() == 46);
    ASSERT_TRUE(dt.second() == 40);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.dayOfWeek() == 0);
    ASSERT_TRUE(dt.timestamp().epochTime() == 1000000000);

    // Test that we can represent down to the microsecond.
    dt = Poco::DateTime(2010, 1, 31, 17, 30, 15, 800, 3);

    ASSERT_TRUE(dt.year() == 2010);
    ASSERT_TRUE(dt.month() == 1);
    ASSERT_TRUE(dt.day() == 31);
    ASSERT_TRUE(dt.hour() == 17);
    ASSERT_TRUE(dt.minute() == 30);
    ASSERT_TRUE(dt.second() == 15);
    ASSERT_TRUE(dt.millisecond() == 800);
    ASSERT_TRUE(dt.microsecond() == 3);
    ASSERT_TRUE(dt.dayOfWeek() == 0);
    ASSERT_TRUE(dt.timestamp().epochTime() == 1264959015);

    // 2001-09-09 01:46:40 Sunday
    dt.assign(2001, 9, 9, 1, 46, 40);

    ASSERT_TRUE(dt.year() == 2001);
    ASSERT_TRUE(dt.month() == 9);
    ASSERT_TRUE(dt.day() == 9);
    ASSERT_TRUE(dt.hour() == 1);
    ASSERT_TRUE(dt.minute() == 46);
    ASSERT_TRUE(dt.second() == 40);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.dayOfWeek() == 0);
    ASSERT_TRUE(dt.timestamp().epochTime() == 1000000000);

    // 2001-09-09 09:46:40 Sunday
    const int tzd = Poco::Timezone::tzd();
    dt.makeLocal(tzd);

    ASSERT_TRUE(dt.year() == 2001);
    ASSERT_TRUE(dt.month() == 9);
    ASSERT_TRUE(dt.day() == 9);
    ASSERT_TRUE(dt.hour() == 9);
    ASSERT_TRUE(dt.minute() == 46);
    ASSERT_TRUE(dt.second() == 40);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.dayOfWeek() == 0);
    ASSERT_TRUE(dt.timestamp().epochTime() == static_cast<time_t>(1000000000 + tzd));

    // 2001-09-09 01:46:40 Sunday
    dt.makeUTC(tzd);

    ASSERT_TRUE(dt.year() == 2001);
    ASSERT_TRUE(dt.month() == 9);
    ASSERT_TRUE(dt.day() == 9);
    ASSERT_TRUE(dt.hour() == 1);
    ASSERT_TRUE(dt.minute() == 46);
    ASSERT_TRUE(dt.second() == 40);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.dayOfWeek() == 0);
    ASSERT_TRUE(dt.timestamp().epochTime() == 1000000000);
}

GTEST_TEST(PocoTimeTest, DateTimeEx)
{
    {
        // default constructor
        common::datetime_ex dt1;

        // copy constructor from DateTimeEx
        common::datetime_ex dt2(dt1);
        ASSERT_TRUE(dt1 == dt2);
        ASSERT_TRUE(dt1.timestamp() == dt2.timestamp());

        // copy constructor from Timestamp
        common::datetime_ex dt3(dt1.timestamp());
        ASSERT_TRUE(dt1 == dt3);
        ASSERT_TRUE(dt1.timestamp() == dt3.timestamp());

        // copy constructor from tm struct
        common::datetime_ex dt4(dt1.make_tm());
        ASSERT_TRUE(dt1.year() == dt4.year());
        ASSERT_TRUE(dt1.month() == dt4.month());
        ASSERT_TRUE(dt1.week() == dt4.week());
        ASSERT_TRUE(dt1.day() == dt4.day());
        ASSERT_TRUE(dt1.day_of_week() == dt4.day_of_week());
        ASSERT_TRUE(dt1.day_of_year() == dt4.day_of_year());
        ASSERT_TRUE(dt1.hour() == dt4.hour());
        ASSERT_TRUE(dt1.hour_am_pm() == dt4.hour_am_pm());
        ASSERT_TRUE(dt1.is_am() == dt4.is_am());
        ASSERT_TRUE(dt1.is_pm() == dt4.is_pm());
        ASSERT_TRUE(dt1.minute() == dt4.minute());
        ASSERT_TRUE(dt1.second() == dt4.second());

        // copy constructor from the given Gregorian local date and time
        common::datetime_ex dt5(dt1.year(), dt1.month(), dt1.day(), dt1.hour(), dt1.minute(), dt1.second(), dt1.millisecond(), dt1.microsecond());
        ASSERT_TRUE(dt1 == dt5);
        ASSERT_TRUE(dt1.timestamp() == dt5.timestamp());

        common::datetime_ex dt6;

        // assigns a DateTimeEx
        dt6 += Poco::Timespan(std::chrono::hours(1));
        dt6  = dt1;
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.timestamp() == dt6.timestamp());

        // assigns a Timestamp
        dt6 += Poco::Timespan(std::chrono::hours(1));
        dt6  = dt1.timestamp();
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.timestamp() == dt6.timestamp());

        // assigns a tm struct
        dt6 += Poco::Timespan(std::chrono::hours(1));
        dt6  = dt1.make_tm();
        ASSERT_TRUE(dt1.year() == dt6.year());
        ASSERT_TRUE(dt1.month() == dt6.month());
        ASSERT_TRUE(dt1.week() == dt6.week());
        ASSERT_TRUE(dt1.day() == dt6.day());
        ASSERT_TRUE(dt1.day_of_week() == dt6.day_of_week());
        ASSERT_TRUE(dt1.day_of_year() == dt6.day_of_year());
        ASSERT_TRUE(dt1.hour() == dt6.hour());
        ASSERT_TRUE(dt1.hour_am_pm() == dt6.hour_am_pm());
        ASSERT_TRUE(dt1.is_am() == dt6.is_am());
        ASSERT_TRUE(dt1.is_pm() == dt6.is_pm());
        ASSERT_TRUE(dt1.minute() == dt6.minute());
        ASSERT_TRUE(dt1.second() == dt6.second());

        // assigns a DateTimeEx
        dt6 += Poco::Timespan(std::chrono::hours(1));
        dt6.assign(dt1);
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.timestamp() == dt6.timestamp());

        // assigns a Timestamp
        dt6 += Poco::Timespan(std::chrono::hours(1));
        dt6.assign(dt1.timestamp());
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.timestamp() == dt6.timestamp());

        // assigns a tm struct
        dt6 += Poco::Timespan(std::chrono::hours(1));
        dt6.assign(dt1.make_tm());
        ASSERT_TRUE(dt1.year() == dt6.year());
        ASSERT_TRUE(dt1.month() == dt6.month());
        ASSERT_TRUE(dt1.week() == dt6.week());
        ASSERT_TRUE(dt1.day() == dt6.day());
        ASSERT_TRUE(dt1.day_of_week() == dt6.day_of_week());
        ASSERT_TRUE(dt1.day_of_year() == dt6.day_of_year());
        ASSERT_TRUE(dt1.hour() == dt6.hour());
        ASSERT_TRUE(dt1.hour_am_pm() == dt6.hour_am_pm());
        ASSERT_TRUE(dt1.is_am() == dt6.is_am());
        ASSERT_TRUE(dt1.is_pm() == dt6.is_pm());
        ASSERT_TRUE(dt1.minute() == dt6.minute());
        ASSERT_TRUE(dt1.second() == dt6.second());

        // assigns a Gregorian local date and time
        dt6 += Poco::Timespan(std::chrono::hours(1));
        dt6.assign(dt1.year(), dt1.month(), dt1.day(), dt1.hour(), dt1.minute(), dt1.second(), dt1.millisecond(), dt1.microsecond());
        ASSERT_TRUE(dt1 == dt6);
        ASSERT_TRUE(dt1.timestamp() == dt6.timestamp());

        dt6 += Poco::Timespan(std::chrono::hours(1));
        ASSERT_TRUE(dt1 != dt6);
        ASSERT_TRUE(dt1 <  dt6);
        ASSERT_TRUE(dt1 <= dt6);
        ASSERT_TRUE(dt6 >  dt1);
        ASSERT_TRUE(dt6 >= dt1);

        ASSERT_TRUE(dt6 == dt1 + Poco::Timespan(std::chrono::hours(1)));
        ASSERT_TRUE(dt1 == dt6 - Poco::Timespan(std::chrono::hours(1)));

        Poco::Timestamp ts = Poco::Timestamp::fromEpochTime(1314521000);
        dt5                = ts;
        dt6                = ts + Poco::Timespan(std::chrono::seconds(5211314));

        Poco::Timespan diff = dt6 - dt5;
        ASSERT_TRUE(diff.totalSeconds() == 5211314);
    }

    {
        // 1970-01-01 00:00:00 Thursday
        Poco::DateTime dt(Poco::Timestamp::fromEpochTime(0));
        ASSERT_TRUE(dt.year() == 1970);
        ASSERT_TRUE(dt.month() == 1);
        ASSERT_TRUE(dt.day() == 1);
        ASSERT_TRUE(dt.hour() == 0);
        ASSERT_TRUE(dt.minute() == 0);
        ASSERT_TRUE(dt.second() == 0);
        ASSERT_TRUE(dt.millisecond() == 0);
        ASSERT_TRUE(dt.microsecond() == 0);
        ASSERT_TRUE(dt.dayOfWeek() == 4);
        ASSERT_TRUE(dt.timestamp().epochTime() == 0);

        // 1970-01-01 08:00:00 Thursday
        common::datetime_ex dtex(dt);
        ASSERT_TRUE(dtex.year() == 1970);
        ASSERT_TRUE(dtex.month() == 1);
        ASSERT_TRUE(dtex.day() == 1);
        ASSERT_TRUE(dtex.hour() == 8);
        ASSERT_TRUE(dtex.minute() == 0);
        ASSERT_TRUE(dtex.second() == 0);
        ASSERT_TRUE(dtex.millisecond() == 0);
        ASSERT_TRUE(dtex.microsecond() == 0);
        ASSERT_TRUE(dtex.day_of_week() == 4);
        ASSERT_TRUE(dtex.timestamp().epochTime() == 0);

        dtex += Poco::Timespan(std::chrono::hours(1));
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
        ASSERT_TRUE(dtex.timestamp().epochTime() == 0);

        dtex += Poco::Timespan(std::chrono::hours(1));
        dtex.assign(dt);
        ASSERT_TRUE(dtex.year() == 1970);
        ASSERT_TRUE(dtex.month() == 1);
        ASSERT_TRUE(dtex.day() == 1);
        ASSERT_TRUE(dtex.hour() == 8);
        ASSERT_TRUE(dtex.minute() == 0);
        ASSERT_TRUE(dtex.second() == 0);
        ASSERT_TRUE(dtex.millisecond() == 0);
        ASSERT_TRUE(dtex.microsecond() == 0);
        ASSERT_TRUE(dtex.day_of_week() == 4);
        ASSERT_TRUE(dtex.timestamp().epochTime() == 0);

        ASSERT_TRUE(dtex.utc() == dt);
        ASSERT_TRUE(dtex.utc_local() == dt + Poco::Timespan(dtex.tzd(), 0));
        ASSERT_TRUE(dtex.utc_time() == dt.utcTime());
        ASSERT_TRUE(dtex.utc_local_time() == dt.utcTime() + Poco::Timespan(dtex.tzd(), 0).totalMicroseconds() * 10);
        ASSERT_TRUE(dtex.operator-(dt).totalMicroseconds() == 0);
    }

    // 1970-01-01 08:00:00 Thursday
    common::datetime_ex dt(Poco::Timestamp::fromEpochTime(0));
    ASSERT_TRUE(dt.year() == 1970);
    ASSERT_TRUE(dt.month() == 1);
    ASSERT_TRUE(dt.day() == 1);
    ASSERT_TRUE(dt.hour() == 8);
    ASSERT_TRUE(dt.minute() == 0);
    ASSERT_TRUE(dt.second() == 0);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.day_of_week() == 4);
    ASSERT_TRUE(dt.timestamp().epochTime() == 0);

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
    ASSERT_TRUE(dt.timestamp().epochTime() == 0);

    // 2001-09-09 09:46:40 Sunday
    dt = Poco::Timestamp::fromEpochTime(1000000000);
    ASSERT_TRUE(dt.year() == 2001);
    ASSERT_TRUE(dt.month() == 9);
    ASSERT_TRUE(dt.day() == 9);
    ASSERT_TRUE(dt.hour() == 9);
    ASSERT_TRUE(dt.minute() == 46);
    ASSERT_TRUE(dt.second() == 40);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.day_of_week() == 0);
    ASSERT_TRUE(dt.timestamp().epochTime() == 1000000000);

    // 2011-08-28 16:43:20 Sunday
    dt = common::datetime_ex(2011, 8, 28, 16, 43, 20);
    ASSERT_TRUE(dt.year() == 2011);
    ASSERT_TRUE(dt.month() == 8);
    ASSERT_TRUE(dt.day() == 28);
    ASSERT_TRUE(dt.hour() == 16);
    ASSERT_TRUE(dt.minute() == 43);
    ASSERT_TRUE(dt.second() == 20);
    ASSERT_TRUE(dt.millisecond() == 0);
    ASSERT_TRUE(dt.microsecond() == 0);
    ASSERT_TRUE(dt.day_of_week() == 0);
    ASSERT_TRUE(dt.timestamp().epochTime() == 1314521000);

    common::datetime_ex dt1 = dt + Poco::Timespan(std::chrono::hours(1));
    ASSERT_TRUE(dt1.year() == 2011);
    ASSERT_TRUE(dt1.month() == 8);
    ASSERT_TRUE(dt1.day() == 28);
    ASSERT_TRUE(dt1.hour() == 17);
    ASSERT_TRUE(dt1.minute() == 43);
    ASSERT_TRUE(dt1.second() == 20);
    ASSERT_TRUE(dt1.millisecond() == 0);
    ASSERT_TRUE(dt1.microsecond() == 0);
    ASSERT_TRUE(dt1.day_of_week() == 0);
    ASSERT_TRUE(dt1.timestamp().epochTime() == 1314524600);

    common::datetime_ex dt2 = dt1 - Poco::Timespan(std::chrono::hours(1));
    ASSERT_TRUE(dt2.year() == 2011);
    ASSERT_TRUE(dt2.month() == 8);
    ASSERT_TRUE(dt2.day() == 28);
    ASSERT_TRUE(dt2.hour() == 16);
    ASSERT_TRUE(dt2.minute() == 43);
    ASSERT_TRUE(dt2.second() == 20);
    ASSERT_TRUE(dt2.millisecond() == 0);
    ASSERT_TRUE(dt2.microsecond() == 0);
    ASSERT_TRUE(dt2.day_of_week() == 0);
    ASSERT_TRUE(dt2.timestamp().epochTime() == 1314521000);

    Poco::Timespan span = dt1 - dt2;
    ASSERT_TRUE(span.days() == 0);
    ASSERT_TRUE(span.hours() == 1);
    ASSERT_TRUE(span.totalHours() == 1);
    ASSERT_TRUE(span.minutes() == 0);
    ASSERT_TRUE(span.totalMinutes() == 60);
    ASSERT_TRUE(span.seconds() == 0);
    ASSERT_TRUE(span.totalSeconds() == 3600);
    ASSERT_TRUE(span.milliseconds() == 0);
    ASSERT_TRUE(span.totalMilliseconds() == 3600000);
}

GTEST_TEST(PocoTimeTest, DateTimeFormatter)
{
    common::datetime_ex dt(2020, 2, 2, 13, 14, 52);

    Poco::DateTime utc      = dt.utc();
    Poco::DateTime utcLocal = dt.utc_local();

    // ASCTIME_FORMAT
    std::string str = Poco::DateTimeFormatter::format(utc, Poco::DateTimeFormat::ASCTIME_FORMAT);
    ASSERT_TRUE(str == "Sun Feb  2 05:14:52 2020");

    str = Poco::DateTimeFormatter::format(utcLocal, Poco::DateTimeFormat::ASCTIME_FORMAT);
    ASSERT_TRUE(str == "Sun Feb  2 13:14:52 2020");

    // SORTABLE_FORMAT
    str = Poco::DateTimeFormatter::format(utc, Poco::DateTimeFormat::SORTABLE_FORMAT);
    ASSERT_TRUE(str == "2020-02-02 05:14:52");

    str = Poco::DateTimeFormatter::format(utcLocal, Poco::DateTimeFormat::SORTABLE_FORMAT);
    ASSERT_TRUE(str == "2020-02-02 13:14:52");
}

GTEST_TEST(PocoTimeTest, DateTimeParser)
{
    Poco::DateTime      dt;
    common::datetime_ex dtex;
    int                 tzd;

    // ASCTIME_FORMAT
    dt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ASCTIME_FORMAT, "Sun Feb  2 05:14:52 2020", tzd);
    ASSERT_TRUE(dt.year() == 2020);
    ASSERT_TRUE(dt.month() == 2);
    ASSERT_TRUE(dt.day() == 2);
    ASSERT_TRUE(dt.hour() == 5);
    ASSERT_TRUE(dt.minute() == 14);
    ASSERT_TRUE(dt.second() == 52);
    ASSERT_TRUE(tzd == 0);

    dtex = dt;
    ASSERT_TRUE(dtex.year() == 2020);
    ASSERT_TRUE(dtex.month() == 2);
    ASSERT_TRUE(dtex.day() == 2);
    ASSERT_TRUE(dtex.hour() == 13);
    ASSERT_TRUE(dtex.minute() == 14);
    ASSERT_TRUE(dtex.second() == 52);
    ASSERT_TRUE(tzd == 0);
    ASSERT_TRUE(dtex.operator-(dt).microseconds() == 0);

    dt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::ASCTIME_FORMAT, "Sun Feb  2 13:14:52 2020", tzd);
    ASSERT_TRUE(dt.year() == 2020);
    ASSERT_TRUE(dt.month() == 2);
    ASSERT_TRUE(dt.day() == 2);
    ASSERT_TRUE(dt.hour() == 13);
    ASSERT_TRUE(dt.minute() == 14);
    ASSERT_TRUE(dt.second() == 52);
    ASSERT_TRUE(tzd == 0);

    dtex = dt.makeTM();
    ASSERT_TRUE(dtex.year() == 2020);
    ASSERT_TRUE(dtex.month() == 2);
    ASSERT_TRUE(dtex.day() == 2);
    ASSERT_TRUE(dtex.hour() == 13);
    ASSERT_TRUE(dtex.minute() == 14);
    ASSERT_TRUE(dtex.second() == 52);
    ASSERT_TRUE(tzd == 0);

    // SORTABLE_FORMAT
    dt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::SORTABLE_FORMAT, "2020-02-02 05:14:52", tzd);
    ASSERT_TRUE(dt.year() == 2020);
    ASSERT_TRUE(dt.month() == 2);
    ASSERT_TRUE(dt.day() == 2);
    ASSERT_TRUE(dt.hour() == 5);
    ASSERT_TRUE(dt.minute() == 14);
    ASSERT_TRUE(dt.second() == 52);
    ASSERT_TRUE(tzd == 0);

    dtex = dt;
    ASSERT_TRUE(dtex.year() == 2020);
    ASSERT_TRUE(dtex.month() == 2);
    ASSERT_TRUE(dtex.day() == 2);
    ASSERT_TRUE(dtex.hour() == 13);
    ASSERT_TRUE(dtex.minute() == 14);
    ASSERT_TRUE(dtex.second() == 52);
    ASSERT_TRUE(tzd == 0);
    ASSERT_TRUE(dtex.operator-(dt).microseconds() == 0);

    dt = Poco::DateTimeParser::parse(Poco::DateTimeFormat::SORTABLE_FORMAT, "2020-02-02 13:14:52", tzd);
    ASSERT_TRUE(dt.year() == 2020);
    ASSERT_TRUE(dt.month() == 2);
    ASSERT_TRUE(dt.day() == 2);
    ASSERT_TRUE(dt.hour() == 13);
    ASSERT_TRUE(dt.minute() == 14);
    ASSERT_TRUE(dt.second() == 52);
    ASSERT_TRUE(tzd == 0);

    dtex = dt.makeTM();
    ASSERT_TRUE(dtex.year() == 2020);
    ASSERT_TRUE(dtex.month() == 2);
    ASSERT_TRUE(dtex.day() == 2);
    ASSERT_TRUE(dtex.hour() == 13);
    ASSERT_TRUE(dtex.minute() == 14);
    ASSERT_TRUE(dtex.second() == 52);
    ASSERT_TRUE(tzd == 0);
}

GTEST_TEST(PocoTimeTest, TimestampExBenckmark)
{
    const int total = 100000;
    for (int idx = 0; idx != total; ++idx)
    {
        // 1970-01-01 08:00:00 Thursday
        common::timestamp_ex ts(Poco::Timestamp::fromEpochTime(0));
        ASSERT_TRUE(ts.year() == 1970);
        ASSERT_TRUE(ts.month() == 1);
        ASSERT_TRUE(ts.day() == 1);
        ASSERT_TRUE(ts.day_of_week() == 4);
        ASSERT_TRUE(ts.day_of_year() == 1);
        ASSERT_TRUE(ts.hour() == 8);
        ASSERT_TRUE(ts.minute() == 0);
        ASSERT_TRUE(ts.second() == 0);
        ASSERT_TRUE(ts.millisecond() == 0);
        ASSERT_TRUE(ts.microsecond() == 0);
        ASSERT_TRUE(ts.timestamp().epochTime() == 0);
    }
}

GTEST_TEST(PocoTimeTest, DateTimeExBenckmark)
{
    const int total = 100000;
    for (int idx = 0; idx != total; ++idx)
    {
        // 1970-01-01 08:00:00 Thursday
        common::datetime_ex dt(Poco::Timestamp::fromEpochTime(0));
        ASSERT_TRUE(dt.year() == 1970);
        ASSERT_TRUE(dt.month() == 1);
        ASSERT_TRUE(dt.day() == 1);
        ASSERT_TRUE(dt.day_of_week() == 4);
        ASSERT_TRUE(dt.day_of_year() == 1);
        ASSERT_TRUE(dt.hour() == 8);
        ASSERT_TRUE(dt.minute() == 0);
        ASSERT_TRUE(dt.second() == 0);
        ASSERT_TRUE(dt.millisecond() == 0);
        ASSERT_TRUE(dt.microsecond() == 0);
        ASSERT_TRUE(dt.timestamp().epochTime() == 0);
    }
}