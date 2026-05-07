#include "stack_trace.h"
#include <array>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#if defined(CORE_PLATFORM_WINDOWS)
    #ifndef NOMINMAX
        #define NOMINMAX
        #define STACK_TRACE_UNDEF_NOMINMAX
    #endif

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
        #define STACK_TRACE_UNDEF_WIN32_LEAN_AND_MEAN
    #endif

    #include <Windows.h>
    #include <DbgHelp.h>

    #if defined(STACK_TRACE_UNDEF_WIN32_LEAN_AND_MEAN)
        #undef WIN32_LEAN_AND_MEAN
    #endif

    #if defined(STACK_TRACE_UNDEF_NOMINMAX)
        #undef NOMINMAX
    #endif
#elif defined(CORE_PLATFORM_LINUX)
    #include <cxxabi.h>
    #include <dlfcn.h>
    #include <execinfo.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)

namespace core {

std::mutex       stack_trace::capture_mutex_;
std::atomic_bool stack_trace::initialized_{false};

namespace {

constexpr int stack_trace_capacity = 64;

#if defined(CORE_PLATFORM_WINDOWS)
DWORD saved_dbghelp_options       = 0;
bool  saved_dbghelp_options_valid = false;
#endif // defined(CORE_PLATFORM_WINDOWS)

const char* basename_of(const char* path, const char windows_separator, const char unix_separator)
{
    if (path == nullptr || *path == '\0')
        return nullptr;

    const char* windows_name = std::strrchr(path, windows_separator);
    const char* unix_name    = std::strrchr(path, unix_separator);
    const char* name         = windows_name;
    if (unix_name != nullptr && (name == nullptr || unix_name > name))
        name = unix_name;

    return name != nullptr ? name + 1 : path;
}

#if defined(CORE_PLATFORM_WINDOWS)
std::string undecorate_symbol_name(const char* symbol_name)
{
    if (symbol_name == nullptr || *symbol_name == '\0')
        return {};

    std::array<char, 4096> buffer{};
    const DWORD length = UnDecorateSymbolName(symbol_name, buffer.data(), static_cast<DWORD>(buffer.size()), UNDNAME_NAME_ONLY);
    if (length == 0)
        return symbol_name;

    return buffer.data();
}
#elif defined(CORE_PLATFORM_LINUX)
std::string demangle_symbol_name(const char* symbol_name)
{
    if (symbol_name == nullptr || *symbol_name == '\0')
        return {};

    int   status    = 0;
    char* demangled = abi::__cxa_demangle(symbol_name, nullptr, nullptr, &status);
    if (status == 0 && demangled != nullptr)
    {
        std::string result = demangled;
        std::free(demangled);
        return result;
    }

    std::free(demangled);

    return symbol_name;
}
#endif // defined(CORE_PLATFORM_WINDOWS)

} // namespace

void stack_trace::initialize()
{
    std::lock_guard<std::mutex> lock(capture_mutex_);
    initialize_locked();
}

void stack_trace::uninitialize()
{
    std::lock_guard<std::mutex> lock(capture_mutex_);
    uninitialize_locked();
}

stack_trace::stack_trace()
    : frames_()
{
    std::lock_guard<std::mutex> lock(capture_mutex_);
    initialize_locked();

    std::array<void*, stack_trace_capacity> captured_frames{};

#if defined(CORE_PLATFORM_WINDOWS)
    const USHORT captured = CaptureStackBackTrace(0, static_cast<DWORD>(captured_frames.size()), captured_frames.data(), nullptr);
    frames_.resize(captured);

    HANDLE process = GetCurrentProcess();
    for (USHORT idx = 0; idx != captured; ++idx)
    {
        frame& current  = frames_[idx];
        current.address = captured_frames[idx];

        IMAGEHLP_MODULE64 module{};
        module.SizeOfStruct = sizeof(module);
        if (SymGetModuleInfo64(process, reinterpret_cast<DWORD64>(current.address), &module))
        {
            if (const char* module_name = basename_of(module.ImageName, '\\', '/'))
                current.module = module_name;
        }

        alignas(SYMBOL_INFO) std::array<unsigned char, sizeof(SYMBOL_INFO) + MAX_SYM_NAME> symbol_buffer{};
        auto* symbol         = reinterpret_cast<PSYMBOL_INFO>(symbol_buffer.data());
        symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol->MaxNameLen   = MAX_SYM_NAME;
        if (SymFromAddr(process, reinterpret_cast<DWORD64>(current.address), nullptr, symbol))
            current.function = undecorate_symbol_name(symbol->Name);

        DWORD          offset = 0;
        IMAGEHLP_LINE64 line{};
        line.SizeOfStruct = sizeof(line);
        if (SymGetLineFromAddr64(process, reinterpret_cast<DWORD64>(current.address), &offset, &line))
        {
            if (line.FileName != nullptr)
                current.filename = line.FileName;

            current.line = static_cast<int>(line.LineNumber);
        }
    }
#elif defined(CORE_PLATFORM_LINUX)
    const int captured = backtrace(captured_frames.data(), static_cast<int>(captured_frames.size()));
    if (captured <= 0)
        return;

    frames_.resize(static_cast<size_t>(captured));
    char** symbols = backtrace_symbols(captured_frames.data(), captured);

    for (int idx = 0; idx != captured; ++idx)
    {
        frame& current  = frames_[static_cast<size_t>(idx)];
        current.address = captured_frames[static_cast<size_t>(idx)];

        Dl_info info{};
        if (dladdr(current.address, &info) != 0)
        {
            if (const char* module_name = basename_of(info.dli_fname, '\\', '/'))
                current.module = module_name;

            current.function = demangle_symbol_name(info.dli_sname);
        }

        if (current.function.empty() && symbols != nullptr && symbols[idx] != nullptr)
            current.function = symbols[idx];

        // backtrace_symbols 通常携带 "+offset" 信息，保留它便于定位无行号符号。
        if (!current.function.empty() && symbols != nullptr && symbols[idx] != nullptr)
        {
            const char* plus        = std::strrchr(symbols[idx], '+');
            const char* right_brace = std::strrchr(symbols[idx], ')');
            if (plus != nullptr && right_brace != nullptr && plus < right_brace)
                current.function.append(plus, static_cast<size_t>(right_brace - plus));
        }
    }

    std::free(symbols);
#endif // defined(CORE_PLATFORM_WINDOWS)
}

std::string stack_trace::to_string() const
{
    if (frames_.empty())
        return {};

    std::ostringstream stream;
    for (size_t idx = 0; idx != frames_.size(); ++idx)
    {
        const frame& current = frames_[idx];

        stream << '#' << std::setw(4) << std::left << idx << std::right
               << "0x" << std::hex << std::setw(static_cast<int>(sizeof(std::uintptr_t) * 2))
               << std::setfill('0') << reinterpret_cast<std::uintptr_t>(current.address)
               << std::dec << std::setfill(' ') << ": "
               << (current.module.empty() ? "<unknown>" : current.module) << '!'
               << (current.function.empty() ? "???" : current.function);

        if (!current.filename.empty())
            stream << ' ' << current.filename;

        if (current.line > 0)
            stream << '(' << current.line << ')';

        stream << '\n';
    }

    return stream.str();
}

void stack_trace::initialize_locked()
{
    if (initialized_.load(std::memory_order_acquire))
        return;

#if defined(CORE_PLATFORM_WINDOWS)
    HANDLE process = GetCurrentProcess();
    if (process == nullptr)
        throw std::runtime_error("Cannot get current process handle for stack trace.");

    if (!saved_dbghelp_options_valid)
    {
        saved_dbghelp_options = SymGetOptions();
        saved_dbghelp_options_valid = true;
    }

    // 保留进程既有选项，仅补充 stack_trace 所需的行号与解码能力。
    SymSetOptions(saved_dbghelp_options | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);
    if (!SymInitialize(process, nullptr, TRUE))
    {
        SymSetOptions(saved_dbghelp_options);
        saved_dbghelp_options_valid = false;
        throw std::runtime_error("Cannot initialize symbol handler for stack trace.");
    }
#endif // defined(CORE_PLATFORM_WINDOWS)

    initialized_.store(true, std::memory_order_release);
}

void stack_trace::uninitialize_locked()
{
    if (!initialized_.load(std::memory_order_acquire))
        return;

#if defined(CORE_PLATFORM_WINDOWS)
    HANDLE process = GetCurrentProcess();
    if (process == nullptr || !SymCleanup(process))
        throw std::runtime_error("Cannot cleanup symbol handler for stack trace.");

    if (saved_dbghelp_options_valid)
    {
        SymSetOptions(saved_dbghelp_options);
        saved_dbghelp_options_valid = false;
    }
#endif // defined(CORE_PLATFORM_WINDOWS)

    initialized_.store(false, std::memory_order_release);
}

} // namespace core