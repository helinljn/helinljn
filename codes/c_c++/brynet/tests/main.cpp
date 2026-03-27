#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"
#include "util/brynet.h"
#include "util/common.h"
#include <cstdlib>
#include <csignal>
#include <cstring>

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
 * @brief 写入日志文件的异步信号安全函数
 * @param sig_name   信号名称
 * @param stacktrace 栈跟踪信息
 * @return
 */
static void write_crash_log(const char* sig_name, const std::string& stacktrace)
{
    int fd = -1;

#if defined(CORE_PLATFORM_WINDOWS)
#pragma warning(push)
#pragma warning(disable:4996)
    fd = _open("crash_dump.log", _O_WRONLY | _O_CREAT | _O_APPEND, _S_IREAD | _S_IWRITE);
#pragma warning(pop)
#elif defined(CORE_PLATFORM_LINUX)
    fd = open("crash_dump.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
#endif // defined(CORE_PLATFORM_WINDOWS)

    if (fd != -1)
    {
        const char header[] = "\n=== CRASH DUMP ===\nSignal: ";
        const char middle[] = "\nStack Trace:\n";
        const char footer[] = "\n=== END DUMP ===\n\n";

#if defined(CORE_PLATFORM_WINDOWS)
        std::ignore = _write(fd, header, sizeof(header) - 1);
        std::ignore = _write(fd, sig_name, static_cast<unsigned int>(strlen(sig_name)));
        std::ignore = _write(fd, middle, sizeof(middle) - 1);
        std::ignore = _write(fd, stacktrace.c_str(), static_cast<unsigned int>(stacktrace.size()));
        std::ignore = _write(fd, footer, sizeof(footer) - 1);
        _close(fd);
#elif defined(CORE_PLATFORM_LINUX)
        std::ignore = write(fd, header, sizeof(header) - 1);
        std::ignore = write(fd, sig_name, strlen(sig_name));
        std::ignore = write(fd, middle, sizeof(middle) - 1);
        std::ignore = write(fd, stacktrace.c_str(), stacktrace.size());
        std::ignore = write(fd, footer, sizeof(footer) - 1);
        close(fd);
#endif // defined(CORE_PLATFORM_WINDOWS)
    }
}

/**
 * @brief 信号处理函数
 */
static void signal_handler(int signo)
{
    const char* sig_name = "UNKNOWN";
    switch (signo)
    {
        case SIGSEGV: sig_name = "SIGSEGV"; break;
        case SIGABRT: sig_name = "SIGABRT"; break;
        case SIGFPE:  sig_name = "SIGFPE";  break;
        case SIGILL:  sig_name = "SIGILL";  break;
        case SIGTERM: sig_name = "SIGTERM"; break;
        case SIGINT:  sig_name = "SIGINT";  break;
#if defined(CORE_PLATFORM_WINDOWS)
        case SIGBREAK: sig_name = "SIGBREAK"; break;
#elif defined(CORE_PLATFORM_LINUX)
        case SIGBUS:  sig_name = "SIGBUS";  break;
        case SIGQUIT: sig_name = "SIGQUIT"; break;
        case SIGSYS:  sig_name = "SIGSYS";  break;
        case SIGTRAP: sig_name = "SIGTRAP"; break;
        case SIGXCPU: sig_name = "SIGXCPU"; break;
        case SIGXFSZ: sig_name = "SIGXFSZ"; break;
#endif // defined(CORE_PLATFORM_WINDOWS)
        default:      sig_name = "UNKNOWN"; break;
    }

    const std::string stacktrace = core::current_stacktrace(true, 2);
    write_crash_log(sig_name, stacktrace);

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
    std::signal(SIGPIPE, SIG_IGN);
    std::signal(SIGCHLD, SIG_IGN);
    std::signal(SIGHUP,  SIG_IGN);
    std::signal(SIGTSTP, SIG_IGN);
#endif // defined(CORE_PLATFORM_WINDOWS)

    doctest::Context context;
    context.applyCommandLine(argc, argv);
    context.run();

    return EXIT_SUCCESS;
}