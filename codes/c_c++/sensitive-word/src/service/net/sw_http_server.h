#pragma once

#ifndef SW_HTTP_SERVER_H
#define SW_HTTP_SERVER_H

#include "sw/sensitive_word.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>

namespace brynet {
namespace net {
class EventLoop;
class IOThreadTcpService;
} // namespace net
} // namespace brynet

namespace brynet {
namespace net {
namespace wrapper {
class ListenerBuilder;
} // namespace wrapper
} // namespace net
} // namespace brynet

namespace net {

//////////////////////////////////////////////////////////////
// 屏蔽词 HTTP 服务运行配置
//////////////////////////////////////////////////////////////
struct sw_http_server_config
{
    std::string                           listen_ip       = "0.0.0.0";                       // 监听 IP 地址
    uint16_t                              listen_port     = 9000;                            // 监听端口
    size_t                                worker_count    = 0;                               // worker 线程数，0 表示按硬件并发数自动推导
    std::string                           deny_file_path  = "res/sensitive_word_deny.txt";   // 屏蔽词文件路径
    std::string                           allow_file_path = "res/sensitive_word_allow.txt";  // 白名单文件路径
    sensitive_word::sensitive_word_config engine_config{};                                   // 敏感词引擎构建参数
};

/**
 * @brief 屏蔽词 HTTP 服务
 *
 * 该类负责：
 * 1. 启动 brynet TCP/HTTP 监听；
 * 2. 初始化多 worker 敏感词引擎；
 * 3. 对外暴露 JSON POST 接口。
 *
 * 当前对外提供的接口：
 * - /v1/check
 * - /v1/query_word
 * - /v1/healthz
 */
class sw_http_server
{
public:
    explicit sw_http_server(sw_http_server_config config);
    ~sw_http_server();

    sw_http_server(const sw_http_server&) = delete;
    sw_http_server& operator=(const sw_http_server&) = delete;

    /**
     * @brief 启动 HTTP 服务
     * @return true 表示启动成功；false 表示启动失败
     */
    bool start();

    /**
     * @brief 停止 HTTP 服务
     */
    void stop() noexcept;

private:
    class impl;
    std::unique_ptr<impl> impl_;
};

} // namespace net

#endif // SW_HTTP_SERVER_H