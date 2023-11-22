#include "exe_symbol.h"

#if POCO_OS == POCO_OS_WINDOWS_NT
    #include <windows.h>
#else
    #include <dlfcn.h>
#endif

namespace common {

exe_symbol::exe_symbol(void)
    : _handle(nullptr)
{
}

exe_symbol::~exe_symbol(void)
{
    unload();
}

bool exe_symbol::load(void)
{
    if (_handle)
        return true;

#if POCO_OS == POCO_OS_WINDOWS_NT
    if (HMODULE* hMod = reinterpret_cast<HMODULE*>(&_handle); !GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, nullptr, hMod))
        return false;
#else
    if (_handle = dlopen(nullptr, RTLD_LAZY); !_handle)
        return false;
#endif

    return true;
}

void* exe_symbol::get_symbol(const std::string_view& name) const
{
    if (!_handle)
        return nullptr;

#if POCO_OS == POCO_OS_WINDOWS_NT
    return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(_handle), name.data()));
#else
    return dlsym(_handle, name.data());
#endif
}

bool exe_symbol::unload(void)
{
    if (!_handle)
        return true;

#if POCO_OS == POCO_OS_LINUX
    if (dlclose(_handle) != 0)
        return false;
#endif

    _handle = nullptr;
    return true;
}

} // namespace common