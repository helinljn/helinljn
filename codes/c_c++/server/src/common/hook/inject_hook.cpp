#include "inject_hook.h"
#include "funchook.h"

namespace common {

inject_hook::inject_hook(void)
    : _hook(nullptr)
{
}

inject_hook::~inject_hook(void)
{
    if (_hook)
        funchook_destroy(reinterpret_cast<funchook_t*>(_hook));
}

bool inject_hook::load(void)
{
    if (_hook)
        return true;

    return (_hook = reinterpret_cast<void*>(funchook_create())) != nullptr;
}

bool inject_hook::reload(void)
{
    if (_hook)
    {
        funchook_destroy(reinterpret_cast<funchook_t*>(_hook));
        _hook = nullptr;
    }

    return load();
}

bool inject_hook::replace(void* oldfaddr, void* newfaddr)
{
    if (!_hook || !oldfaddr || !newfaddr)
        return false;

    void** origin = &oldfaddr;
    return funchook_prepare(reinterpret_cast<funchook_t*>(_hook), origin, newfaddr) == FUNCHOOK_ERROR_SUCCESS;
}

bool inject_hook::install(void)
{
    if (!_hook)
        return false;

    return funchook_install(reinterpret_cast<funchook_t*>(_hook), 0) == FUNCHOOK_ERROR_SUCCESS;
}

bool inject_hook::uninstall(void)
{
    if (!_hook)
        return false;

    return funchook_uninstall(reinterpret_cast<funchook_t*>(_hook), 0) == FUNCHOOK_ERROR_SUCCESS;
}

} // namespace common