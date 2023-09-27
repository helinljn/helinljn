#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fmt/format.h"
#include "util/poco.h"

#include "Poco/Crypto/Crypto.h"
#include "Poco/Data/Data.h"
#include "Poco/Data/MySQL/MySQL.h"
#include "Poco/Foundation.h"
#include "Poco/JSON/JSON.h"
#include "Poco/JWT/JWT.h"
#include "Poco/Net/Net.h"
#include "Poco/Net/NetSSL.h"
#include "Poco/Redis/Redis.h"
#include "Poco/Util/Util.h"
#include "Poco/XML/XML.h"
#include "Poco/Zip/Zip.h"

#include "util/stack_trace.h"
#include "util/DateTimeEx.h"
#include "Poco/FileStream.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Mutex.h"
#include "Poco/Thread.h"
#include "Poco/Process.h"
#include <csignal>

#define REGISTER_SIGNAL(sig)                             \
    if (::signal(sig, signal_handler) != signal_handler) \
        ::signal(sig, signal_handler)

void signal_handler(int sig)
{
    // 保存当前调用栈信息
    {
        static Poco::FastMutex      mutex;
        Poco::FastMutex::ScopedLock holder(mutex);

        Poco::FileOutputStream fos;
        fos.open("crash.log", std::ios::app);
        fos << "-------------------------"
            << POCO_DEFAULT_NEWLINE_CHARS
            << "sig:  " << sig
            << POCO_DEFAULT_NEWLINE_CHARS
            << "tid:  " << Poco::Thread::currentOsTid()
            << POCO_DEFAULT_NEWLINE_CHARS
            << "pid:  " << Poco::Process::id()
            << POCO_DEFAULT_NEWLINE_CHARS
            << "time: " << Poco::DateTimeFormatter::format(Poco::DateTimeEx().utcLocal(), "%Y-%m-%d %H:%M:%S")
            << POCO_DEFAULT_NEWLINE_CHARS
            << "-------------------------"
            << POCO_DEFAULT_NEWLINE_CHARS
            << "------ stack trace ------"
            << POCO_DEFAULT_NEWLINE_CHARS
            << common::stack_trace().to_string()
            << "-------------------------"
            << POCO_DEFAULT_NEWLINE_CHARS
            << POCO_DEFAULT_NEWLINE_CHARS
            << POCO_DEFAULT_NEWLINE_CHARS;
        fos.close();
    }

    // 恢复信号默认处理，然后重新发送
    ::signal(sig, SIG_DFL);
    ::raise(sig);
}

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    common::stack_trace::initialize();
    Poco::Net::initializeNetwork();
    Poco::Net::initializeSSL();

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    // 注册信号处理
    REGISTER_SIGNAL(SIGILL);
    REGISTER_SIGNAL(SIGFPE);
    REGISTER_SIGNAL(SIGSEGV);
    REGISTER_SIGNAL(SIGABRT);

    const int ret = RUN_ALL_TESTS();

    Poco::Net::uninitializeSSL();
    Poco::Net::uninitializeNetwork();
    common::stack_trace::cleanup();

    return ret;
}