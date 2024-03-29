#include "stack_trace.h"
#include "quill/Fmt.h"
#include <stdexcept>

#if defined(CORE_PLATFORM_WINDOWS)
    #include <windows.h>
    #include <dbghelp.h>
#elif defined(CORE_PLATFORM_LINUX)
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <dlfcn.h>
#endif

namespace core {

bool       stack_trace::_initialized{false};
std::mutex stack_trace::_capture_mutex{};

void stack_trace::initialize(void)
{
    if (_initialized)
        return;

#if defined(CORE_PLATFORM_WINDOWS)
    // Provide required symbol options
    SymSetOptions(SYMOPT_PUBLICS_ONLY);

    // Get the current process handle
    bool success = false;
    if (HANDLE hProcess = GetCurrentProcess(); hProcess)
    {
        // Initializes symbol handler for the current process
        const int attempts = 10;
        const int sleep    = 100;
        for (int attempt = 0; attempt != attempts; ++attempt)
        {
            if (SymInitialize(hProcess, nullptr, TRUE))
            {
                success = true;
                break;
            }

            Sleep(sleep);
        }
    }

    if (!success)
        throw std::runtime_error("Cannot initialize symbol handler for the current process!");
#endif

    _initialized = true;
}

void stack_trace::uninitialize(void)
{
    if (!_initialized)
        return;

#if defined(CORE_PLATFORM_WINDOWS)
    // Get the current process handle
    bool success = false;
    if (HANDLE hProcess = GetCurrentProcess(); hProcess && SymCleanup(hProcess))
        success = true;

    if (!success)
        throw std::runtime_error("Cannot uninitialize symbol handler for the current process!");
#endif

    _initialized = false;
}

stack_trace::stack_trace(void)
    : _frames()
{
    const int capacity         = 64;
    void*     frames[capacity] = {0};

    // Capture stack trace snapshot under the critical section
    std::lock_guard holder(_capture_mutex);

#if defined(CORE_PLATFORM_WINDOWS)
    // Capture the current stack trace
    const USHORT captured = CaptureStackBackTrace(0, capacity, frames, nullptr);
    if (captured > 0)
        _frames.resize(captured, frame{});

    // Fill all captured frames with symbol information
    for (int idx = 0; idx != captured; ++idx)
    {
        // Get the frame address
        auto& f   = _frames[idx];
        f.address = frames[idx];

        // Get the current process handle
        HANDLE hProcess = GetCurrentProcess();

        // Get the frame module
        IMAGEHLP_MODULE64 module;
        ZeroMemory(&module, sizeof(module));
        module.SizeOfStruct = sizeof(module);
        if (SymGetModuleInfo64(hProcess, (DWORD64)f.address, &module))
        {
            const char* image = std::strrchr(module.ImageName, '\\');
            if (image)
                f.module = image + 1;
        }

        // Get the frame function
        char symbol[sizeof(SYMBOL_INFO) + MAX_SYM_NAME];
        ZeroMemory(&symbol, sizeof(symbol));
        PSYMBOL_INFO pSymbol  = (PSYMBOL_INFO)symbol;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen   = MAX_SYM_NAME;
        if (SymFromAddr(hProcess, (DWORD64)f.address, nullptr, pSymbol))
        {
            char buffer[4096];
            if (UnDecorateSymbolName(pSymbol->Name, buffer, (DWORD)sizeof(buffer), UNDNAME_NAME_ONLY) > 0)
                f.function = buffer;
        }

        // Get the frame file name and line number
        DWORD offset = 0;
        IMAGEHLP_LINE64 line;
        ZeroMemory(&line, sizeof(line));
        line.SizeOfStruct = sizeof(line);
        if (SymGetLineFromAddr64(hProcess, (DWORD64)f.address, &offset, &line))
        {
            if (line.FileName)
                f.filename = line.FileName;
            f.line = line.LineNumber;
        }
    }
#elif defined(CORE_PLATFORM_LINUX)
    // Capture the current stack trace
    const int captured   = backtrace(frames, capacity);
    char**    stacktrace = backtrace_symbols(frames, captured);
    if (captured > 0)
        _frames.resize(captured, frame{});

    // Fill all captured frames with symbol information
    for (int idx = 0; idx != captured; ++idx)
    {
        // Get the frame address
        auto& f   = _frames[idx];
        f.address = frames[idx];

        // Get the frame information
        Dl_info info;
        if (dladdr(frames[idx], &info) == 0)
            continue;

        // Get the frame module
        if (info.dli_fname)
        {
            const char* module = std::strrchr(info.dli_fname, '/');
            if (module)
                f.module = module + 1;
        }

        // Get the frame function
        if (info.dli_sname)
        {
            // Demangle symbol name if need
            int   status    = 0;
            char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
            if (status == 0 && demangled)
            {
                f.function = demangled;
                free(demangled);
            }
            else
            {
                f.function = info.dli_sname;
            }
        }

        if (f.function.empty())
        {
            f.function = stacktrace[idx];
            continue;
        }

        // Get addr fix
        const char* ps = std::strrchr(stacktrace[idx], '+');
        const char* rb = std::strrchr(stacktrace[idx], ')');
        if (ps && rb && rb - ps > 0)
            f.function.append(ps, rb - ps);
    }

    free(stacktrace);
#endif
}

std::string stack_trace::to_string(void) const
{
    std::string ret;
    if (!_frames.empty())
        ret.reserve(_frames.size() * 256);

    auto frame_to_string = [](const size_t idx, const frame& f) -> std::string
    {
        if (f.line > 0)
        {
            return fmt::format("#{:<3} 0x{:016x}: {}!{} {}({})\n", idx, reinterpret_cast<uint64_t>(f.address),
                f.module.empty() ? "<unknown>" : f.module, f.function.empty() ? "???" : f.function, f.filename, f.line);
        }
        else
        {
            return fmt::format("#{:<3} 0x{:016x}: {}!{} {}\n", idx, reinterpret_cast<uint64_t>(f.address),
                f.module.empty() ? "<unknown>" : f.module, f.function.empty() ? "???" : f.function, f.filename);
        }
    };

    for (size_t idx = 0; idx != _frames.size(); ++idx)
    {
        ret += frame_to_string(idx, _frames[idx]);
    }

    return ret;
}

} // namespace core