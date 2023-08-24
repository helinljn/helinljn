#include "gtest/gtest.h"
#include "util/poco.h"
#include "Poco/Thread.h"
#include "Poco/Timestamp.h"
#include "Poco/Clock.h"

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