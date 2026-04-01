#include "symbol_loader.h"

#if defined(CORE_PLATFORM_WINDOWS)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #undef WIN32_LEAN_AND_MEAN
#elif defined(CORE_PLATFORM_LINUX)
    #include <dlfcn.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(CORE_PLATFORM_WINDOWS)

namespace core {

symbol_loader::symbol_loader(void)
    : _handle(nullptr)
{
}

symbol_loader::~symbol_loader(void)
{
    unload();
}

bool symbol_loader::load(std::string_view path)
{
    if (_handle)
        unload();

#if defined(CORE_PLATFORM_WINDOWS)
    // Windows不支持空路径
    if (path.empty())
        return false;

    _handle = reinterpret_cast<HMODULE>(LoadLibrary(path.data()));
    return _handle != nullptr;
#elif defined(CORE_PLATFORM_LINUX)
    // RTLD_LAZY:   延迟绑定
    // RTLD_NOW:    立即绑定
    // RTLD_GLOBAL: 符号全局可见
    // RTLD_LOCAL:  符号仅本地可见
    // 如果路径为空，则打开当前可执行文件
    if (path.empty())
        _handle = dlopen(nullptr, RTLD_LAZY | RTLD_LOCAL);
    else
        _handle = dlopen(path.data(), RTLD_LAZY | RTLD_LOCAL);
    return _handle != nullptr;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

void symbol_loader::unload(void)
{
    if (_handle)
    {
#if defined(CORE_PLATFORM_WINDOWS)
        FreeLibrary(reinterpret_cast<HMODULE>(_handle));
#elif defined(CORE_PLATFORM_LINUX)
        dlclose(_handle);
#endif // defined(CORE_PLATFORM_WINDOWS)
        _handle = nullptr;
    }
}

void* symbol_loader::get_symbol(std::string_view sname)
{
    if (!_handle)
        return nullptr;

#if defined(CORE_PLATFORM_WINDOWS)
    return GetProcAddress(reinterpret_cast<HMODULE>(_handle), sname.data());
#elif defined(CORE_PLATFORM_LINUX)
    return dlsym(_handle, sname.data());
#endif // defined(CORE_PLATFORM_WINDOWS)
}

} // namespace core