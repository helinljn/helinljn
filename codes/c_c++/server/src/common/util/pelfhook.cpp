#include "util/pelfhook.h"
#include "plthook.h"

#include <iomanip>
#include <sstream>

namespace common {

pelfhook::pelfhook(void)
    : _hook(nullptr)
{
}

pelfhook::~pelfhook(void)
{
    if (_hook)
        plthook_close(reinterpret_cast<plthook_t*>(_hook));
}

bool pelfhook::load(const char* mname)
{
    if (_hook)
        return true;

    return plthook_open(reinterpret_cast<plthook_t**>(&_hook), mname) == PLTHOOK_SUCCESS;
}

bool pelfhook::reload(const char* mname)
{
    if (_hook)
    {
        plthook_close(reinterpret_cast<plthook_t*>(_hook));
        _hook = nullptr;
    }

    return load(mname);
}

bool pelfhook::replace(const char* oldfname, void* newfaddr, void** oldfaddr)
{
    return plthook_replace(reinterpret_cast<plthook_t*>(_hook), oldfname, newfaddr, oldfaddr) == PLTHOOK_SUCCESS;
}

std::vector<std::string> pelfhook::all_entries(void) const
{
    std::vector<std::string> entries;
    if (!_hook)
        return entries;

    unsigned int       pos  = 0;
    const char*        name = nullptr;
    void**             addr = nullptr;
    std::ostringstream ostr;
    while (plthook_enum(reinterpret_cast<plthook_t*>(_hook), &pos, &name, &addr) == PLTHOOK_SUCCESS)
    {
        ostr.str("");
        ostr.clear();

        ostr << "0x" << std::hex << std::setw(16) << std::setfill('0') << reinterpret_cast<uint64_t>(addr)
             << "(0x" << std::hex << std::setw(16) << std::setfill('0') << reinterpret_cast<uint64_t>(*addr)
             << ") -> " << name;

        entries.emplace_back(std::move(ostr.str()));
    }

    return entries;
}

} // namespace common