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
static void signal_dump_handler(int sig);

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    common::stack_trace::initialize();
    Poco::Net::initializeNetwork();
    Poco::Net::initializeSSL();

    testing::InitGoogleMock(&argc, argv);
    testing::InitGoogleTest(&argc, argv);

    // 设置exec_name
    exec_name = std::move(Poco::Path(argv[0]).getBaseName());
    Poco::toLowerInPlace(exec_name);

    // 注册信号处理
    REGISTER_SIGNAL(SIGINT, SIG_IGN);
    REGISTER_SIGNAL(SIGTERM, SIG_IGN);

#if POCO_OS == POCO_OS_WINDOWS_NT
    REGISTER_SIGNAL(SIGILL, signal_dump_handler);
    REGISTER_SIGNAL(SIGFPE, signal_dump_handler);
    REGISTER_SIGNAL(SIGSEGV, signal_dump_handler);
    REGISTER_SIGNAL(SIGABRT, signal_dump_handler);
#else
    REGISTER_SIGNAL(SIGILL, signal_dump_handler);
    REGISTER_SIGNAL(SIGFPE, signal_dump_handler);
    REGISTER_SIGNAL(SIGSEGV, signal_dump_handler);
    REGISTER_SIGNAL(SIGABRT, signal_dump_handler);
    REGISTER_SIGNAL(SIGBUS, signal_dump_handler);
    REGISTER_SIGNAL(SIGQUIT, signal_dump_handler);
    REGISTER_SIGNAL(SIGSYS, signal_dump_handler);
    REGISTER_SIGNAL(SIGTRAP, signal_dump_handler);
    REGISTER_SIGNAL(SIGXCPU, signal_dump_handler);
    REGISTER_SIGNAL(SIGXFSZ, signal_dump_handler);
#endif

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

void signal_dump_handler(int sig)
{
    // 格式化当前堆栈信息
    std::string format_info;
    {
        std::ostringstream oss;
        oss << "---------------------------------"
            << std::endl
            << "sig:  " << sig << '(' << signal_to_string(sig) << ')'
            << std::endl
            << "tid:  " << Poco::Thread::currentOsTid()
            << std::endl;

        if (Poco::Thread* curThread = Poco::Thread::current(); curThread)
            oss << "tnm:  " << curThread->getName() << std::endl;

        oss << "pid:  " << Poco::Process::id()
            << std::endl
            << "date: " << Poco::DateTimeFormatter::format(Poco::DateTimeEx().utcLocal(), "%Y-%m-%d %H:%M:%s")
            << std::endl
            << "---------- stack trace ----------"
            << std::endl
            << common::stack_trace().to_string()
            << "---------------------------------"
            << std::endl
            << std::endl;
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

    // 恢复信号默认处理，然后重新发送
    ::signal(sig, SIG_DFL);
    ::raise(sig);
}