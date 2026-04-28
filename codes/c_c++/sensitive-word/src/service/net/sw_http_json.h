#pragma once

#ifndef SW_HTTP_JSON_H
#define SW_HTTP_JSON_H

#include "net/sw_http_common.h"
#include <optional>
#include <string>
#include <vector>

namespace net {

/**
 * @brief 判断请求的 Content-Type 是否为 JSON
 * @param parser HTTP 请求解析器
 * @return true 表示请求 Content-Type 为 JSON；false 表示不是 JSON
 */
bool is_json_content_type(const HTTPParser& parser);

/**
 * @brief 解析 HTTP 请求体中的 JSON
 * @param parser HTTP 请求解析器
 * @param error_result 解析失败时填充的错误结果
 * @return 解析成功时返回标准 JSON 请求对象；失败时返回 std::nullopt
 */
std::optional<json_request> parse_json_request_body(const HTTPParser& parser,
                                                    http_result&       error_result);

/**
 * @brief 提取必填字符串字段
 * @param root JSON 根对象
 * @param key 字段名
 * @param error_result 提取失败时填充的错误结果
 * @param request_id 请求链路标识
 * @return 提取成功时返回字段值；字段缺失或类型不匹配时返回 std::nullopt
 */
std::optional<std::string> require_string_field(const Json::Value& root,
                                                const char*        key,
                                                http_result&       error_result,
                                                const std::string& request_id);

/**
 * @brief 提取可选布尔字段
 * @param root JSON 根对象
 * @param key 字段名
 * @param default_value 字段不存在时使用的默认值
 * @param error_result 提取失败时填充的错误结果
 * @param request_id 请求链路标识
 * @return 提取成功时返回布尔值；字段不存在时返回 default_value；类型不匹配时返回 std::nullopt
 */
std::optional<bool> get_optional_bool_field(const Json::Value& root,
                                            const char*        key,
                                            bool               default_value,
                                            http_result&       error_result,
                                            const std::string& request_id);

/**
 * @brief 提取可选整数字段
 * @param root JSON 根对象
 * @param key 字段名
 * @param default_value 字段不存在时使用的默认值
 * @param error_result 提取失败时填充的错误结果
 * @param request_id 请求链路标识
 * @return 提取成功时返回整数值；字段不存在时返回 default_value；类型不匹配时返回 std::nullopt
 */
std::optional<int> get_optional_int_field(const Json::Value& root,
                                          const char*        key,
                                          int                default_value,
                                          http_result&       error_result,
                                          const std::string& request_id);

/**
 * @brief 提取必填字符串数组字段
 * @param root JSON 根对象
 * @param key 字段名
 * @param error_result 提取失败时填充的错误结果
 * @param request_id 请求链路标识
 * @return 提取成功时返回字符串数组；字段缺失、类型不匹配或过滤空串后为空时返回 std::nullopt
 */
std::optional<std::vector<std::string>> require_string_array_field(
    const Json::Value& root,
    const char*        key,
    http_result&       error_result,
    const std::string& request_id);

} // namespace net

#endif // SW_HTTP_JSON_H