#include "gtest/gtest.h"
#include "fmt/format.h"
#include "util/types.h"
#include "Poco/File.h"
#include "Poco/Logger.h"
#include "Poco/LoggingFactory.h"
#include "Poco/SplitterChannel.h"
#include "Poco/FormattingChannel.h"
#include "Poco/AsyncChannel.h"

using MsgList = std::list<Poco::Message>;

class TestChannel : public Poco::Channel
{
public:
    TestChannel(void)
        : Channel()
        , _msgList()
    {
    }

    void log(const Poco::Message& msg) override
    {
        _msgList.push_back(msg);
    }

    MsgList& list(void)
    {
        return _msgList;
    }

    void clear(void)
    {
        _msgList.clear();
    }

private:
    MsgList _msgList;
};

GTEST_TEST(PocoLoggingTest, Logger)
{
    Poco::AutoPtr<TestChannel> tc = new TestChannel;
    ASSERT_TRUE(tc);

    auto& root = Poco::Logger::root();
    ASSERT_TRUE(!root.getChannel());

    root.setChannel(tc);
    ASSERT_TRUE(root.getChannel());

    ASSERT_TRUE(root.getLevel() == Poco::Message::PRIO_INFORMATION);
    ASSERT_TRUE(root.is(Poco::Message::PRIO_INFORMATION));
    ASSERT_TRUE(root.fatal());
    ASSERT_TRUE(root.critical());
    ASSERT_TRUE(root.error());
    ASSERT_TRUE(root.warning());
    ASSERT_TRUE(root.notice());
    ASSERT_TRUE(root.information());
    ASSERT_TRUE(!root.debug());
    ASSERT_TRUE(!root.trace());

    root.information("Informational message");
    ASSERT_TRUE(tc->list().size() == 1);
    root.warning("Warning message");
    ASSERT_TRUE(tc->list().size() == 2);
    root.debug("Debug message");
    ASSERT_TRUE(tc->list().size() == 2);

    Poco::Logger& logger1 = Poco::Logger::get("Logger1");
    Poco::Logger& logger2 = Poco::Logger::get("Logger2");
    Poco::Logger& logger11 = Poco::Logger::get("Logger1.Logger1");
    Poco::Logger& logger12 = Poco::Logger::get("Logger1.Logger2");
    Poco::Logger& logger21 = Poco::Logger::get("Logger2.Logger1");
    Poco::Logger& logger22 = Poco::Logger::get("Logger2.Logger2");

    std::vector<std::string> loggers;
    Poco::Logger::names(loggers);
    ASSERT_TRUE(loggers.size() == 7);
    ASSERT_TRUE(loggers[0] == "");
    ASSERT_TRUE(loggers[1] == "Logger1");
    ASSERT_TRUE(loggers[2] == "Logger1.Logger1");
    ASSERT_TRUE(loggers[3] == "Logger1.Logger2");
    ASSERT_TRUE(loggers[4] == "Logger2");
    ASSERT_TRUE(loggers[5] == "Logger2.Logger1");
    ASSERT_TRUE(loggers[6] == "Logger2.Logger2");

    Poco::Logger::setLevel("Logger1", Poco::Message::PRIO_DEBUG);
    ASSERT_TRUE(logger1.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(logger11.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(logger12.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(!logger2.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(!logger21.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(!logger22.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(logger11.is(Poco::Message::PRIO_INFORMATION));
    ASSERT_TRUE(logger12.is(Poco::Message::PRIO_INFORMATION));
    ASSERT_TRUE(logger21.is(Poco::Message::PRIO_INFORMATION));
    ASSERT_TRUE(logger22.is(Poco::Message::PRIO_INFORMATION));

    Poco::Logger::setLevel("Logger2.Logger1", Poco::Message::PRIO_ERROR);
    ASSERT_TRUE(logger1.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(logger11.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(logger12.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(!logger21.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(!logger22.is(Poco::Message::PRIO_DEBUG));
    ASSERT_TRUE(logger11.is(Poco::Message::PRIO_INFORMATION));
    ASSERT_TRUE(logger12.is(Poco::Message::PRIO_INFORMATION));
    ASSERT_TRUE(logger21.is(Poco::Message::PRIO_ERROR));
    ASSERT_TRUE(logger22.is(Poco::Message::PRIO_INFORMATION));

    Poco::Logger::setLevel("", Poco::Message::PRIO_WARNING);
    ASSERT_TRUE(root.getLevel() == Poco::Message::PRIO_WARNING);
    ASSERT_TRUE(logger1.getLevel() == Poco::Message::PRIO_WARNING);
    ASSERT_TRUE(logger11.getLevel() == Poco::Message::PRIO_WARNING);
    ASSERT_TRUE(logger12.getLevel() == Poco::Message::PRIO_WARNING);
    ASSERT_TRUE(logger1.getLevel() == Poco::Message::PRIO_WARNING);
    ASSERT_TRUE(logger21.getLevel() == Poco::Message::PRIO_WARNING);
    ASSERT_TRUE(logger22.getLevel() == Poco::Message::PRIO_WARNING);

    root.setLevel(Poco::Message::PRIO_TRACE);
    ASSERT_TRUE(root.getLevel() == Poco::Message::PRIO_TRACE);
    root.setLevel("7");
    ASSERT_TRUE(root.getLevel() == Poco::Message::PRIO_DEBUG);
    root.setLevel("information");
    ASSERT_TRUE(root.getLevel() == Poco::Message::PRIO_INFORMATION);
    root.setLevel("NoTIcE");
    ASSERT_TRUE(root.getLevel() == Poco::Message::PRIO_NOTICE);

    ASSERT_THROW(root.setLevel("null"), Poco::InvalidArgumentException);
    ASSERT_THROW(root.setLevel("9"), Poco::InvalidArgumentException);
}

GTEST_TEST(PocoLoggingTest, LoggerAsync)
{
    auto&             logger      = Poco::Logger::root();
    const std::string testLogName = "log_test.log";

    {
        auto& logFactory = Poco::LoggingFactory::defaultFactory();

        // 创建格式化器，设置日志输出格式
        auto formatter = logFactory.createFormatter("PatternFormatter");
        ASSERT_TRUE(formatter);
        if (formatter)
        {
            formatter->setProperty("pattern", "%Y-%m-%d %H:%M:%S.%F [%q]: %t [tid:%J,%O:%u]");
            formatter->setProperty("times",   "local");
        }

        // 创建文件channel，设置日志名和滚动属性
        auto fileChannel = logFactory.createChannel("FileChannel");
        ASSERT_TRUE(fileChannel);
        if (fileChannel)
        {
            fileChannel->setProperty("path",         testLogName);
            fileChannel->setProperty("rotation",     "daily");
            fileChannel->setProperty("archive",      "timestamp");
            fileChannel->setProperty("times",        "local");
            fileChannel->setProperty("compress",     "false");
            fileChannel->setProperty("purgeAge",     "none");
            fileChannel->setProperty("purgeCount",   "none");
            fileChannel->setProperty("flush",        "false");
            fileChannel->setProperty("rotateOnOpen", "false");
        }

        // 创建控制台channel，设置日志输出颜色
        auto consoleChannel = logFactory.createChannel("ColorConsoleChannel");
        ASSERT_TRUE(consoleChannel);
        if (consoleChannel)
        {
            consoleChannel->setProperty("enableColors",     "True");
            consoleChannel->setProperty("traceColor",       "LightGreen");
            consoleChannel->setProperty("debugColor",       "LightGreen");
            consoleChannel->setProperty("informationColor", "White");
            consoleChannel->setProperty("noticeColor",      "LightBlue");
            consoleChannel->setProperty("warningColor",     "Yellow");
            consoleChannel->setProperty("errorColor",       "LightRed");
            consoleChannel->setProperty("criticalColor",    "LightRed");
            consoleChannel->setProperty("fatalColor",       "LightRed");
        }

        // 格式化输出
        auto formattingChannel = logFactory.createChannel("FormattingChannel").cast<Poco::FormattingChannel>();
        ASSERT_TRUE(formattingChannel);
        if (formattingChannel)
        {
            // 合并文件channel和控制台channel
            auto splitterChannel = logFactory.createChannel("SplitterChannel").cast<Poco::SplitterChannel>();
            ASSERT_TRUE(splitterChannel);
            if (splitterChannel)
            {
                splitterChannel->addChannel(fileChannel);
                splitterChannel->addChannel(consoleChannel);
            }

            formattingChannel->setFormatter(formatter);
            formattingChannel->setChannel(splitterChannel);
        }

        // 同步输出
        {
            logger.setChannel(formattingChannel);
            logger.setLevel(Poco::Message::PRIO_TRACE);

            poco_trace(logger, "this is a trace msg!!!");
            poco_debug(logger, "this is a debug msg!!!");
            poco_information(logger, "this is an information msg!!!");
            poco_notice(logger, "this is a notice msg!!!");
            poco_warning(logger, "this is a warning msg!!!");
            poco_error(logger, "this is an error msg!!!");
            poco_critical(logger, "this is a critical msg!!!");
            poco_fatal(logger, "this is a fatal msg!!!");
        }

        // 异步输出
        {
            // 创建异步channel
            auto asyncChannel = logFactory.createChannel("AsyncChannel").cast<Poco::AsyncChannel>();
            ASSERT_TRUE(asyncChannel);
            if (asyncChannel)
                asyncChannel->setChannel(formattingChannel);

            logger.setChannel(asyncChannel);
            logger.setLevel(Poco::Message::PRIO_TRACE);

            // 移除控制台channel
            if (auto splitterChannel = formattingChannel->getChannel().cast<Poco::SplitterChannel>(); splitterChannel)
                splitterChannel->removeChannel(consoleChannel);

            for (int idx = 1; idx <= 1000; ++idx)
                poco_information(logger, fmt::format("Hello, this is an information msg! idx = {}", idx));
        }
    }

    // 清空当前异步channel
    logger.setChannel(nullptr);

    Poco::File f(testLogName);
    if (f.exists() && f.isFile())
        f.remove();
}