#pragma once

#ifndef SW_HTTP_AUTH_H
#define SW_HTTP_AUTH_H

#include "net/sw_http_common.h"
#include "net/sw_http_server.h"

namespace net {

/**
 * @brief 校验管理接口请求是否已授权
 * @param config HTTP 服务运行配置
 * @param parser HTTP 请求解析器
 * @return true 表示请求已授权；false 表示请求未授权
 */
bool is_authorized_request(const sw_http_server_config& config, const HTTPParser& parser);

} // namespace net

#endif // SW_HTTP_AUTH_H