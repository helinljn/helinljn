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
#include "Poco/DateTimeFormatter.h"
#include "Poco/Thread.h"
#include "Poco/Process.h"
#include "Poco/String.h"
#include "Poco/Path.h"

#include <csignal>
#include <sstream>
#include <fstream>

#define REGISTER_SIGNAL(sig, handler)      \
    if (::signal(sig, handler) != handler) \
        ::signal(sig, handler)

static std::string exec_name;
static std::mutex  file_mutex;
static const char* signal_to_string(int sig);
static void signal_common_handler(int sig);
static void signal_dump_handler(int sig);

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    common::stack_trace::initialize();
    Poco::Net::initializeNetwork();
    Poco::Net::initializeSSL();

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    // 注册信号处理
    REGISTER_SIGNAL(SIGINT, signal_common_handler);
    REGISTER_SIGNAL(SIGTERM, signal_common_handler);
    REGISTER_SIGNAL(SIGILL, signal_dump_handler);
    REGISTER_SIGNAL(SIGFPE, signal_dump_handler);
    REGISTER_SIGNAL(SIGSEGV, signal_dump_handler);
    REGISTER_SIGNAL(SIGABRT, signal_dump_handler);

    // 设置exec_name
    [argv]() -> void
    {
        exec_name = Poco::Path(argv[0]).getBaseName();
        Poco::toLowerInPlace(exec_name);
        ASSERT_TRUE(!exec_name.empty());
    }();

    const int ret = RUN_ALL_TESTS();

    Poco::Net::uninitializeSSL();
    Poco::Net::uninitializeNetwork();
    common::stack_trace::uninitialize();

    return ret;
}

const char* signal_to_string(int sig)
{
    switch (sig)
    {
    case SIGINT:
        return "SIGINT";
    case SIGILL:
        return "SIGILL";
    case SIGABRT:
        return "SIGABRT";
    case SIGFPE:
        return "SIGFPE";
    case SIGSEGV:
        return "SIGSEGV";
    case SIGTERM:
        return "SIGTERM";
#if POCO_OS == POCO_OS_WINDOWS_NT
    case SIGBREAK:
        return "SIGBREAK";
#else
    case SIGHUP:
        return "SIGHUP";
    case SIGQUIT:
        return "SIGQUIT";
    case SIGTRAP:
        return "SIGTRAP";
    case SIGKILL:
        return "SIGKILL";
    case SIGBUS:
        return "SIGBUS";
    case SIGSYS:
        return "SIGSYS";
    case SIGPIPE:
        return "SIGPIPE";
    case SIGALRM:
        return "SIGALRM";
    case SIGURG:
        return "SIGURG";
    case SIGSTOP:
        return "SIGSTOP";
    case SIGTSTP:
        return "SIGTSTP";
    case SIGCONT:
        return "SIGCONT";
    case SIGCHLD:
        return "SIGCHLD";
    case SIGTTIN:
        return "SIGTTIN";
    case SIGTTOU:
        return "SIGTTOU";
    case SIGPOLL:
        return "SIGPOLL";
    case SIGXCPU:
        return "SIGXCPU";
    case SIGXFSZ:
        return "SIGXFSZ";
    case SIGVTALRM:
        return "SIGVTALRM";
    case SIGPROF:
        return "SIGPROF";
    case SIGUSR1:
        return "SIGUSR1";
    case SIGUSR2:
        return "SIGUSR2";
#endif
    default:
        return "UNKNOWN";
    }
}

void signal_common_handler(int sig)
{
    // 格式化当前堆栈信息
    std::string format_info;
    {
        std::ostringstream oss;
        oss << "-------------------------------"
            << POCO_DEFAULT_NEWLINE_CHARS
            << "sig:  " << sig << '(' << signal_to_string(sig) << ')'
            << POCO_DEFAULT_NEWLINE_CHARS
            << "tid:  " << Poco::Thread::currentOsTid()
            << POCO_DEFAULT_NEWLINE_CHARS
            << "pid:  " << Poco::Process::id()
            << POCO_DEFAULT_NEWLINE_CHARS
            << "date: " << Poco::DateTimeFormatter::format(Poco::DateTimeEx().utcLocal(), "%Y-%m-%d %H:%M:%s")
            << POCO_DEFAULT_NEWLINE_CHARS
            << "--------- stack trace ---------"
            << POCO_DEFAULT_NEWLINE_CHARS
            << common::stack_trace().to_string()
            << "-------------------------------"
            << POCO_DEFAULT_NEWLINE_CHARS
            << POCO_DEFAULT_NEWLINE_CHARS;
        format_info = std::move(oss.str());
    }

    // 保存信息至文件
    {
        std::lock_guard holder(file_mutex);
        std::ofstream   ofs(fmt::format("dump_{}_{}.log", exec_name, Poco::Process::id()), std::ios::app);
        ofs << format_info;
        ofs.close();
    }

    // 输出信息
    fmt::print("{}", format_info);
}

void signal_dump_handler(int sig)
{
    // 获取当前堆栈信息并保存至文件，然后输出
    signal_common_handler(sig);

    // 恢复信号默认处理，然后重新发送
    ::signal(sig, SIG_DFL);
    ::raise(sig);
}