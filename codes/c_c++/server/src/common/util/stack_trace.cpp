#include "util/stack_trace.h"
#include "util/poco.h"
#include "Poco/String.h"
#include "Poco/Exception.h"

#include <iomanip>
#include <cstring>
#include <sstream>

#if POCO_OS == POCO_OS_WINDOWS_NT
    #include <windows.h>
    #include <dbghelp.h>
#else
    #include <execinfo.h>
    #include <cxxabi.h>
    #include <dlfcn.h>
    #if LIBBFD_SUPPORT
        #include <bfd.h>
    #endif
#endif

namespace common {

std::atomic_bool stack_trace::_initialized;
std::mutex       stack_trace::_capture_mutex;

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

void stack_trace::uninitialize(void)
{
    if (!_initialized.load())
        return;

#if POCO_OS == POCO_OS_WINDOWS_NT
    // Get the current process handle
    bool success = false;
    if (HANDLE hProcess = GetCurrentProcess(); hProcess && SymCleanup(hProcess))
        success = true;

    if (!success)
        throw Poco::SystemException("Cannot uninitialize symbol handler for the current process!");
#endif

    _initialized.store(false);
}

stack_trace::stack_trace(void)
    : _frames()
{
    const int capacity         = 32;
    void*     frames[capacity] = {0};

    // Capture stack trace snapshot under the critical section
    std::lock_guard holder(_capture_mutex);

#if POCO_OS == POCO_OS_WINDOWS_NT
    // Capture the current stack trace
    const USHORT captured = CaptureStackBackTrace(1, capacity, frames, nullptr);
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
            if (line.FileName)
                f.filename = line.FileName;
            f.line = line.LineNumber;
        }
    }
#else
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
            f.function = stacktrace[idx];

        if (!f.address || !info.dli_fname)
            continue;

#if LIBBFD_SUPPORT
        ////////////////////////////////
        // libbfd support begin
        bfd*      abfd     = nullptr;
        char**    matching = nullptr;
        void*     symsptr  = nullptr;
        asymbol** syms     = nullptr;

        unsigned int symsize  = 0;
        long         symcount = 0;
        bfd_boolean  found    = false;
        bfd_vma      pc       = 0;

        const char*  filename     = nullptr;
        const char*  functionname = nullptr;
        unsigned int line         = 0;

        abfd = bfd_openr(info.dli_fname, nullptr);
        if (!abfd)
            continue;

        if (bfd_check_format(abfd, bfd_archive))
            goto cleanup;

        if (!bfd_check_format_matches(abfd, bfd_object, &matching))
            goto cleanup;

        if ((bfd_get_file_flags(abfd) & HAS_SYMS) == 0)
            goto cleanup;

        symcount = bfd_read_minisymbols(abfd, FALSE, &symsptr, &symsize);
        if (symcount == 0)
            symcount = bfd_read_minisymbols(abfd, TRUE, &symsptr, &symsize);
        if (symcount < 0)
            goto cleanup;

        syms = (asymbol**)symsptr;
        pc   = (bfd_vma)f.address;
        for (asection* section = abfd->sections; section != nullptr; section = section->next)
        {
            if (found)
                break;

            if ((bfd_section_flags(section) & SEC_ALLOC) == 0)
                continue;

            bfd_vma vma = bfd_section_vma(section);
            if (pc < vma)
                continue;

            bfd_size_type secsize = bfd_section_size(section);
            if (pc >= vma + secsize)
                continue;

            found = bfd_find_nearest_line(abfd, section, syms, pc - vma, &filename, &functionname, &line);
        }

        if (!found)
            goto cleanup;

        if (filename)
            f.filename = filename;
        f.line = line;

cleanup:
        if (symsptr)
            free(symsptr);

        if (abfd)
            bfd_close(abfd);
        // libbfd support end
        ////////////////////////////////
#endif
    }

    free(stacktrace);
#endif
}

std::string stack_trace::to_string(void) const
{
    std::string ret;
    if (!_frames.empty())
        ret.reserve(_frames.size() * 256);

    std::ostringstream ostr;
    std::string        hexAddress;
    auto frame_to_string = [this, &ostr, &hexAddress](const size_t idx, const frame& f) -> std::string
    {
        ostr.str("");
        ostr.clear();
        hexAddress.clear();

        Poco::uIntToStr(reinterpret_cast<uint64_t>(f.address), 16, hexAddress, true, 18, '0');
        Poco::toLowerInPlace(hexAddress);

        if (_frames.size() > 10)
            ostr << '[' << std::setw(2) << std::setfill('0') << idx << "] ";
        else
            ostr << '[' << idx << "] ";

        ostr << hexAddress << ": "
             << (f.module.empty() ? "<unknown>" : f.module) << '!'
             << (f.function.empty() ? "???" : f.function) << ' '
             << f.filename;

        if (f.line > 0)
            ostr << '(' << f.line << ')';

        ostr << std::endl;

        return ostr.str();
    };

    for (size_t idx = 0; idx != _frames.size(); ++idx)
    {
        ret += frame_to_string(idx, _frames[idx]);
    }

    return ret;
}

} // namespace common