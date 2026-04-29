#pragma once

#ifndef SW_HTTP_COMMON_H
#define SW_HTTP_COMMON_H

#include "sw/sensitive_word.h"
#include "json/json.h"
#include <cstddef>
#include <cstdint>
#include <set>
#include <vector>
#include <string>
#include <string_view>
#include <memory>
#include <optional>

namespace brynet::net {
    class EventLoop;
} // namespace brynet::net

namespace brynet::net::http {
    class HTTPParser;
} // namespace brynet::net::http

namespace net {

using HTTPParser   = brynet::net::http::HTTPParser;
using EventLoopPtr = std::shared_ptr<brynet::net::EventLoop>;

//////////////////////////////////////////////////////////////
// 词库更新操作类型
//////////////////////////////////////////////////////////////
enum class update_op
{
    add_deny,      // 添加敏感词
    remove_deny,   // 删除敏感词
    add_allow,     // 添加允许词
    remove_allow,  // 删除允许词
};

//////////////////////////////////////////////////////////////
// 词库更新命令
//////////////////////////////////////////////////////////////
struct update_command
{
    uint64_t                 version = 0;                    // 更新事务版本号
    update_op                op      = update_op::add_deny;  // 更新操作类型
    std::vector<std::string> words;                          // 本次要处理的词列表
};

//////////////////////////////////////////////////////////////
// 单个 worker 线程上的运行上下文
//////////////////////////////////////////////////////////////
struct worker_context
{
    size_t                                worker_index    = 0;  // worker 下标
    EventLoopPtr                          event_loop;           // 所属事件循环
    sensitive_word::sensitive_word_engine engine;               // 当前 worker 的本地敏感词引擎
    uint64_t                              applied_version = 0;  // 已应用的最新词库版本
};

//////////////////////////////////////////////////////////////
// 标准 JSON 请求对象
//////////////////////////////////////////////////////////////
struct json_request
{
    Json::Value root;        // 完整 JSON 请求树
    std::string request_id;  // 请求链路标识，未提供时为空
};

//////////////////////////////////////////////////////////////
// HTTP 处理结果
//////////////////////////////////////////////////////////////
struct http_result
{
    int         http_status = 200;  // 最终返回的 HTTP 状态码
    Json::Value root;               // 要序列化成 JSON 的响应体
};

//////////////////////////////////////////////////////////////
// 词库仓储快照
//////////////////////////////////////////////////////////////
struct word_repository
{
    std::set<std::string> deny_words;   // 屏蔽词集合
    std::set<std::string> allow_words;  // 白名单集合
};

//////////////////////////////////////////////////////////////
// JSON 请求/响应默认 Content-Type
//////////////////////////////////////////////////////////////
constexpr const char* k_content_type_json = "application/json";

/**
 * @brief 将 HTTP 状态码映射为响应短语
 * @param status_code HTTP 状态码
 * @return 状态码对应的标准响应短语
 */
std::string status_reason_phrase(int status_code);

/**
 * @brief 将 Json::Value 序列化为紧凑 JSON 字符串
 * @param value 待序列化的 JSON 对象
 * @return 压缩后的 JSON 字符串
 */
std::string to_json_string(const Json::Value& value);

/**
 * @brief 构造完整 HTTP 响应报文
 * @param status_code HTTP 状态码
 * @param body HTTP 响应体
 * @param content_type 响应 Content-Type
 * @return 可直接发送的 HTTP/1.1 响应报文
 */
std::string make_http_response(int status_code, const std::string& body, std::string_view content_type = k_content_type_json);

/**
 * @brief 构造统一格式的错误响应
 * @param http_status HTTP 状态码
 * @param code 业务错误码
 * @param message 错误信息
 * @param request_id 请求链路标识
 * @return 标准错误响应结果
 */
http_result make_error_result(int http_status, int code, std::string message, const std::string& request_id = {});

/**
 * @brief 构造统一格式的成功响应
 * @param data 成功响应中的 data 字段
 * @param request_id 请求链路标识
 * @return 标准成功响应结果
 */
http_result make_ok_result(Json::Value data, const std::string& request_id = {});

/**
 * @brief 将命中结果转换为 JSON 对象
 * @param result 敏感词命中结果
 * @return 对应的 JSON 对象表示
 */
Json::Value make_match_json(const sensitive_word::word_result& result);

} // namespace net

#endif // SW_HTTP_COMMON_H