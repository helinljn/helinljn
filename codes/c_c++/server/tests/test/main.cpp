#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "fmt/format.h"
#include "util/types.h"

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
#include "util/datetime.h"
#include "Poco/DateTimeFormat.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Thread.h"
#include "Poco/Process.h"
#include "Poco/String.h"
#include "Poco/Path.h"

#include <csignal>
#include <cstdlib>
#include <sstream>
#include <fstream>

static std::string exec_name;
static std::mutex  file_mutex;
static void signal_handler(int sig)
{
    // 将信号转换为其对应的字符串形式
    auto sig2str = [](const int sig) -> const char*
    {
        switch (sig)
        {
        case SIGINT:    return "SIGINT";
        case SIGILL:    return "SIGILL";
        case SIGABRT:   return "SIGABRT";
        case SIGFPE:    return "SIGFPE";
        case SIGSEGV:   return "SIGSEGV";
        case SIGTERM:   return "SIGTERM";
#if POCO_OS == POCO_OS_WINDOWS_NT
        case SIGBREAK:  return "SIGBREAK";
#else
        case SIGHUP:    return "SIGHUP";
        case SIGQUIT:   return "SIGQUIT";
        case SIGTRAP:   return "SIGTRAP";
        case SIGKILL:   return "SIGKILL";
        case SIGBUS:    return "SIGBUS";
        case SIGSYS:    return "SIGSYS";
        case SIGPIPE:   return "SIGPIPE";
        case SIGALRM:   return "SIGALRM";
        case SIGURG:    return "SIGURG";
        case SIGSTOP:   return "SIGSTOP";
        case SIGTSTP:   return "SIGTSTP";
        case SIGCONT:   return "SIGCONT";
        case SIGCHLD:   return "SIGCHLD";
        case SIGTTIN:   return "SIGTTIN";
        case SIGTTOU:   return "SIGTTOU";
        case SIGPOLL:   return "SIGPOLL";
        case SIGXCPU:   return "SIGXCPU";
        case SIGXFSZ:   return "SIGXFSZ";
        case SIGVTALRM: return "SIGVTALRM";
        case SIGPROF:   return "SIGPROF";
        case SIGUSR1:   return "SIGUSR1";
        case SIGUSR2:   return "SIGUSR2";
#endif
        default:        return "UNKNOWN";
        }
    };

    // 格式化当前调用栈信息
    std::string info;
    {
        std::ostringstream oss;
        oss << "---------------------------------"
            << std::endl
            << "sig:  " << sig << '(' << sig2str(sig) << ')'
            << std::endl
            << "tid:  " << Poco::Thread::currentOsTid()
            << std::endl;

        if (Poco::Thread* th = Poco::Thread::current(); th)
            oss << "tnm:  " << th->getName() << std::endl;

        oss << "pid:  " << Poco::Process::id()
            << std::endl
            << "date: " << Poco::DateTimeFormatter::format(common::datetime().utc_local(), Poco::DateTimeFormat::SORTABLE_FORMAT)
            << std::endl
            << "---------- stack trace ----------"
            << std::endl
            << common::stack_trace().to_string()
            << "---------------------------------"
            << std::endl
            << std::endl;
        info = std::move(oss.str());
    }

    // 保存信息
    {
        std::lock_guard holder(file_mutex);
        std::ofstream   ofs(fmt::format("dump_{}_{}.log", Poco::toLower(exec_name), Poco::Process::id()), std::ios::app);
        ofs << info;
        ofs.close();
    }

    // 输出信息
    fmt::print("{}", info);

    // 恢复信号默认处理
    std::signal(sig, SIG_DFL);
    std::raise(sig);
}

#if POCO_OS == POCO_OS_WINDOWS_NT
static void win_terminate_handler(void)
{
    std::abort();
}

static void win_invalid_parameter_handler(const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t)
{
    std::abort();
}

static LONG win_unhandled_exception_handler(EXCEPTION_POINTERS*)
{
    std::abort();
}
#endif

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
    poco_assert(!exec_name.empty());

    // 注册信号处理
#if POCO_OS == POCO_OS_WINDOWS_NT
    std::signal(SIGINT, SIG_IGN);
    std::signal(SIGTERM, SIG_IGN);
    std::signal(SIGBREAK, SIG_IGN);
    std::signal(SIGABRT, signal_handler);
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

    std::set_terminate(win_terminate_handler);
    _set_purecall_handler(win_terminate_handler);

    _set_invalid_parameter_handler(win_invalid_parameter_handler);
    _set_thread_local_invalid_parameter_handler(win_invalid_parameter_handler);

    SetUnhandledExceptionFilter(win_unhandled_exception_handler);
#else
    std::signal(SIGINT, SIG_IGN);
    std::signal(SIGTERM, SIG_IGN);
    std::signal(SIGHUP, SIG_IGN);
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGILL, signal_handler);
    std::signal(SIGFPE, signal_handler);
    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGBUS, signal_handler);
    std::signal(SIGQUIT, signal_handler);
    std::signal(SIGSYS, signal_handler);
    std::signal(SIGTRAP, signal_handler);
    std::signal(SIGXCPU, signal_handler);
    std::signal(SIGXFSZ, signal_handler);
#endif

    const int ret = RUN_ALL_TESTS();

    Poco::Net::uninitializeSSL();
    Poco::Net::uninitializeNetwork();
    common::stack_trace::uninitialize();

    return ret;
}