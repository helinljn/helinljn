#include "util/stack_trace.h"
#include "util/poco.h"
#include "Poco/Exception.h"
#include "Poco/Mutex.h"

#include <cstring>
#include <sstream>
#include <array>
#include <string_view>

#if POCO_OS == POCO_OS_WINDOWS_NT
    #include <windows.h>
    #include <dbghelp.h>
#else
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <dlfcn.h>
#endif

namespace common {

std::atomic_bool stack_trace::_initialized;

void stack_trace::initialize(void)
{
    if (_initialized.load())
        return;

#if POCO_OS == POCO_OS_WINDOWS_NT
    // Provide required symbol options
    SymSetOptions(SYMOPT_PUBLICS_ONLY);

    // Get the current process handle
    bool success = false;
    if (HANDLE hProcess = GetCurrentProcess(); hProcess)
    {
        // Initializes symbol handler for the current process
        const int attempts = 10;
        const int sleep    = 100;
        for (int attempt = 0; attempt < attempts; ++attempt)
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
        throw Poco::SystemException("Cannot initialize symbol handler for the current process!");
#endif

    _initialized.store(true);
}

void stack_trace::cleanup(void)
{
    if (!_initialized.load())
        return;

#if POCO_OS == POCO_OS_WINDOWS_NT
    // Get the current process handle
    bool success = false;
    if (HANDLE hProcess = GetCurrentProcess(); hProcess && SymCleanup(hProcess))
        success = true;

    if (!success)
        throw Poco::SystemException("Cannot cleanup symbol handler for the current process!");
#endif

    _initialized.store(false);
}

stack_trace::stack_trace(void)
    : _frames()
{
    const int capacity         = 1024;
    void*     frames[capacity] = {0};

#if POCO_OS == POCO_OS_WINDOWS_NT
    // Capture the current stack trace
    const USHORT captured = CaptureStackBackTrace(1, capacity, frames, nullptr);

    // Resize stack trace frames vector
    _frames.resize(captured, frame{});

    // Capture stack trace snapshot under the critical section
    static Poco::FastMutex      mutex;
    Poco::FastMutex::ScopedLock holder(mutex);

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
            if (image != nullptr)
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
            if (line.FileName != nullptr)
                f.filename = line.FileName;
            f.line = line.LineNumber;
        }
    }
#else
    // Capture the current stack trace
    int captured = backtrace(frames, capacity);

    // Resize stack trace frames vector
    _frames.resize(captured, frame{});

    // Capture stack trace snapshot under the critical section
    static Poco::FastMutex      mutex;
    Poco::FastMutex::ScopedLock holder(mutex);

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
        if (info.dli_fname != nullptr)
        {
            const char* module = std::strrchr(info.dli_fname, '/');
            if (module != nullptr)
                f.module = module + 1;
        }

        // Get the frame function
        if (info.dli_sname != nullptr)
        {
            // Demangle symbol name if need
            int   status    = 0;
            char* demangled = abi::__cxa_demangle(info.dli_sname, nullptr, 0, &status);
            if ((status == 0) && (demangled != nullptr))
            {
                f.function = demangled;
                free(demangled);
            }
            else
            {
                f.function = info.dli_sname;
            }
        }
    }
#endif
}

std::string stack_trace::to_string(void) const
{
    auto frame_to_string = [](const size_t idx, const frame& f) -> std::string
    {
        std::string hexAddress;
        Poco::uIntToStr(reinterpret_cast<uint64_t>(f.address), 16, hexAddress, true, 18, '0');

        std::ostringstream ostr;
        ostr << '[' << idx << "] ";
        ostr << hexAddress << ": ";
        ostr << (f.module.empty() ? "<unknown>" : f.module) << '!';
        ostr << (f.function.empty() ? "???" : f.function) << ' ';
        ostr << f.filename;
        if (f.line > 0)
            ostr << '(' << f.line << ')';

        ostr << std::ends;

        return ostr.str();
    };

    std::string ret;
    if (!_frames.empty())
        ret.reserve(_frames.size() * 256);

    for (size_t idx = 0; idx != _frames.size(); ++idx)
    {
        ret += frame_to_string(idx, _frames[idx]);
        ret += POCO_DEFAULT_NEWLINE_CHARS;
    }

    return ret;
}

} // namespace common