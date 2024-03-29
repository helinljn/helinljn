#include "gtest/gtest.h"
#include "quill/Fmt.h"
#include "util/singleton.hpp"
#include "util/stack_trace.h"
#include "time/timestamp.h"

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
    core::timestamp ts;
    ASSERT_TRUE(ts.epoch_time() > 0);

    fmt::print("Current timestamp expressed in seconds: {}\n", ts.epoch_time());
    fmt::print("Current timestamp expressed in microseconds: {}\n", ts.epoch_microseconds());
}