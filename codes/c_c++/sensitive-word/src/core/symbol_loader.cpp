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
    : handle_(nullptr)
    , should_unload_(false)
{
}

symbol_loader::~symbol_loader(void)
{
    unload();
}

bool symbol_loader::load(const std::string& path)
{
    if (handle_)
        unload();

#if defined(CORE_PLATFORM_WINDOWS)
    // 如果路径为空，加载当前可执行文件
    if (path.empty())
    {
        handle_        = reinterpret_cast<HMODULE>(GetModuleHandle(nullptr));
        should_unload_ = false;
    }
    else
    {
        handle_        = reinterpret_cast<HMODULE>(LoadLibrary(path.c_str()));
        should_unload_ = handle_ != nullptr;
    }
#elif defined(CORE_PLATFORM_LINUX)
    // RTLD_LAZY:   延迟绑定
    // RTLD_NOW:    立即绑定
    // RTLD_GLOBAL: 符号全局可见
    // RTLD_LOCAL:  符号仅本地可见
    // 如果路径为空，则打开当前可执行文件
    if (path.empty())
    {
        handle_        = dlopen(nullptr, RTLD_LAZY | RTLD_LOCAL);
        should_unload_ = false;
    }
    else
    {
        handle_        = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        should_unload_ = handle_ != nullptr;
    }
#endif // defined(CORE_PLATFORM_WINDOWS)

    return handle_ != nullptr;
}

void symbol_loader::unload(void)
{
    if (handle_ && should_unload_)
    {
#if defined(CORE_PLATFORM_WINDOWS)
        FreeLibrary(reinterpret_cast<HMODULE>(handle_));
#elif defined(CORE_PLATFORM_LINUX)
        dlclose(handle_);
#endif // defined(CORE_PLATFORM_WINDOWS)

        handle_        = nullptr;
        should_unload_ = false;
    }
}

void* symbol_loader::get_symbol(const std::string& sname)
{
    if (!handle_ || sname.empty())
        return nullptr;

#if defined(CORE_PLATFORM_WINDOWS)
    return GetProcAddress(reinterpret_cast<HMODULE>(handle_), sname.c_str());
#elif defined(CORE_PLATFORM_LINUX)
    // 清除上一次调用 dlerror() 时的错误信息
    dlerror();

    // dlsym 返回值为 nullptr 时并不总表示失败，需通过 dlerror 判断
    void*       symbol_addr = dlsym(handle_, sname.c_str());
    const char* error_msg   = dlerror();
    if (error_msg != nullptr)
        return nullptr;

    return symbol_addr;
#endif // defined(CORE_PLATFORM_WINDOWS)
}

} // namespace core
