#include "net/sw_http_auth.h"
#include "net/brynet.h"

namespace net {

bool is_authorized_request(const sw_http_server_config& config,
                           const HTTPParser&           parser)
{
    if (config.admin_api_key.empty())
        return true;

    return parser.getValue("X-API-Key") == config.admin_api_key;
}

} // namespace net