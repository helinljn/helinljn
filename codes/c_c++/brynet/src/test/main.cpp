#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "fmt/format.h"
#include "core/hook.h"
#include "core/brynet.h"
#include "core/common.h"
#include "core/datetime.h"
#include <cstdlib>
#include <csignal>
#include <exception>

#if defined(CORE_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <fcntl.h>
    #include <io.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(CORE_PLATFORM_LINUX)
    #include <unistd.h>
    #include <fcntl.h>
#endif // defined(CORE_PLATFORM_WINDOWS)

/**
 * @brief 信号处理函数
 * @param signo 信号编号
 * @return
 */
static void signal_handler(int signo)
{
    auto format_dump_info = [](int signo) -> std::string
    {
        const char* sigstr = "UNKNOWN";
        switch (signo)
        {
            case SIGSEGV:  sigstr = "SIGSEGV";  break;
            case SIGABRT:  sigstr = "SIGABRT";  break;
            case SIGFPE:   sigstr = "SIGFPE";   break;
            case SIGILL:   sigstr = "SIGILL";   break;
            case SIGTERM:  sigstr = "SIGTERM";  break;
            case SIGINT:   sigstr = "SIGINT";   break;
#if defined(CORE_PLATFORM_WINDOWS)
            case SIGBREAK: sigstr = "SIGBREAK"; break;
#elif defined(CORE_PLATFORM_LINUX)
            case SIGBUS:   sigstr = "SIGBUS";   break;
            case SIGQUIT:  sigstr = "SIGQUIT";  break;
            case SIGSYS:   sigstr = "SIGSYS";   break;
            case SIGTRAP:  sigstr = "SIGTRAP";  break;
            case SIGXCPU:  sigstr = "SIGXCPU";  break;
            case SIGXFSZ:  sigstr = "SIGXFSZ";  break;
#endif // defined(CORE_PLATFORM_WINDOWS)
            default:       sigstr = "UNKNOWN";  break;
        }

        const std::string timestr    = core::datetime().format("%Y-%m-%d %H:%M:%S.{ms}{us}");
        const std::string exepath    = core::get_exepath();
        const std::string stacktrace = core::current_stacktrace(true);

        return fmt::format("******************************** Crash dump begin ********************************\n"
                           "Signal: {}\n"
                           "Time  : {}\n"
                           "Path  : {}\n"
                           "{}"
                           "******************************** Crash dump  end  ********************************\n",
                           sigstr, timestr, exepath, stacktrace);
    };

    const std::string dump_info = format_dump_info(signo);

#if defined(CORE_PLATFORM_WINDOWS)
#pragma warning(push)
#pragma warning(disable:4996)
    const int fd = _open("crash_dump.log", _O_WRONLY | _O_CREAT | _O_APPEND, _S_IREAD | _S_IWRITE);
#pragma warning(pop)
#elif defined(CORE_PLATFORM_LINUX)
    const int fd = open("crash_dump.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
#endif // defined(CORE_PLATFORM_WINDOWS)

    if (fd != -1)
    {
#if defined(CORE_PLATFORM_WINDOWS)
        std::ignore = _write(fd, dump_info.c_str(), static_cast<unsigned int>(dump_info.size()));
        _close(fd);
#elif defined(CORE_PLATFORM_LINUX)
        std::ignore = write(fd, dump_info.c_str(), dump_info.size());
        close(fd);
#endif // defined(CORE_PLATFORM_WINDOWS)
    }

    _Exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

#if defined(CORE_PLATFORM_WINDOWS)
    // 控制台编码设置
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    // 致命信号处理
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGFPE,  signal_handler);
    std::signal(SIGILL,  signal_handler);
    std::signal(SIGSEGV, signal_handler);

    // 忽略的信号
    std::signal(SIGINT,   SIG_IGN);  // 忽略 Ctrl+C
    std::signal(SIGTERM,  SIG_IGN);  // 忽略终止请求
    std::signal(SIGBREAK, SIG_IGN);  // 忽略 Ctrl+Break

    // 设置abort行为
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);

    // 各种错误处理器
    std::set_terminate([]() {
        signal_handler(SIGABRT);
    });

    _set_purecall_handler([]() {
        signal_handler(SIGABRT);
    });

    _set_invalid_parameter_handler([](const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t) {
        signal_handler(SIGABRT);
    });

    _set_thread_local_invalid_parameter_handler([](const wchar_t*, const wchar_t*, const wchar_t*, unsigned int, uintptr_t) {
        signal_handler(SIGABRT);
    });

#pragma warning(push)
#pragma warning(disable:4702)
    // 未处理异常过滤器
    SetUnhandledExceptionFilter([](LPEXCEPTION_POINTERS) -> LONG {
        signal_handler(SIGABRT);
        return EXCEPTION_EXECUTE_HANDLER;
    });
#pragma warning(pop)
#elif defined(CORE_PLATFORM_LINUX)
    // 致命信号处理
    std::signal(SIGABRT, signal_handler);
    std::signal(SIGFPE,  signal_handler);
    std::signal(SIGILL,  signal_handler);
    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGBUS,  signal_handler);
    std::signal(SIGQUIT, signal_handler);
    std::signal(SIGSYS,  signal_handler);
    std::signal(SIGTRAP, signal_handler);
    std::signal(SIGXCPU, signal_handler);
    std::signal(SIGXFSZ, signal_handler);

    // 忽略的信号
    std::signal(SIGINT,  SIG_IGN);  // 忽略 Ctrl+C
    std::signal(SIGTERM, SIG_IGN);  // 忽略终止请求
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGCHLD, SIG_IGN);
    std::signal(SIGHUP,  SIG_IGN);
    std::signal(SIGTSTP, SIG_IGN);
    std::signal(SIGCONT, SIG_IGN);
    std::signal(SIGTTIN, SIG_IGN);
    std::signal(SIGTTOU, SIG_IGN);
    std::signal(SIGPOLL, SIG_IGN);
    std::signal(SIGUSR1, SIG_IGN);
    std::signal(SIGUSR2, SIG_IGN);
#endif // defined(CORE_PLATFORM_WINDOWS)

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    context.run();

    return EXIT_SUCCESS;
}