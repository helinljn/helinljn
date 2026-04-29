#include "net/sw_http_auth.h"
#include "net/brynet.h"

namespace net {
namespace {

bool constant_time_equal(std::string_view a, std::string_view b) noexcept
{
    if (a.size() != b.size())
        return false;

    volatile unsigned char result = 0;
    for (size_t i = 0; i < a.size(); ++i)
        result |= static_cast<unsigned char>(a[i]) ^ static_cast<unsigned char>(b[i]);

    return result == 0;
}

} // namespace

bool is_authorized_request(const sw_http_server_config& config, const HTTPParser& parser)
{
    if (config.admin_api_key.empty())
        return true;

    return constant_time_equal(parser.getValue("X-API-Key"), config.admin_api_key);
}

} // namespace net