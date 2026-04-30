#include "net/sw_http_protocol.h"
#include "core/brynet.h"
#include <cctype>
#include <memory>

namespace net {
namespace {

bool starts_with_case_insensitive(std::string_view text, std::string_view prefix)
{
    if (text.size() < prefix.size())
        return false;

    for (size_t i = 0; i < prefix.size(); ++i)
    {
        const auto lhs = static_cast<unsigned char>(text[i]);
        const auto rhs = static_cast<unsigned char>(prefix[i]);
        if (std::tolower(lhs) != std::tolower(rhs))
            return false;
    }

    return true;
}

bool is_ascii_space(char ch)
{
    const auto value = static_cast<unsigned char>(ch);
    return std::isspace(value) != 0;
}

} // namespace

//////////////////////////////////////////////////////////////
// HTTP 响应构建
//////////////////////////////////////////////////////////////

std::string status_reason_phrase(int status_code)
{
    switch (status_code)
    {
        case 200: return "OK";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 415: return "Unsupported Media Type";
        case 500: return "Internal Server Error";
        default:  return "Unknown";
    }
}

std::string make_http_response(int status_code,
                               const std::string& body,
                               bool keep_alive,
                               std::string_view content_type)
{
    std::string response;
    response.reserve(body.size() + 256);
    response += "HTTP/1.1 ";
    response += std::to_string(status_code);
    response += ' ';
    response += status_reason_phrase(status_code);
    response += "\r\n";
    response += "Content-Type: ";
    response += content_type;
    response += "\r\n";
    response += "Content-Length: ";
    response += std::to_string(body.size());
    response += "\r\n";
    response += keep_alive ? "Connection: keep-alive\r\n" : "Connection: close\r\n";
    response += "\r\n";
    response += body;
    return response;
}

//////////////////////////////////////////////////////////////
// JSON 序列化
//////////////////////////////////////////////////////////////

std::string to_json_string(const Json::Value& value)
{
    thread_local Json::StreamWriterBuilder builder = []() {
        Json::StreamWriterBuilder b;
        b["indentation"] = "";
        return b;
    }();

    return Json::writeString(builder, value);
}

Json::Value make_match_json(const sensitive_word::word_result& result)
{
    Json::Value item(Json::objectValue);
    item["word"]                  = result.word;
    item["normalized_word"]       = result.normalized_word;
    item["type"]                  = result.type == sensitive_word::match_type::num ? "num" : "word";
    item["raw_begin"]             = Json::UInt64(result.raw_begin);
    item["raw_end"]               = Json::UInt64(result.raw_end);
    item["raw_code_point_length"] = Json::UInt64(result.raw_code_point_length);
    return item;
}

//////////////////////////////////////////////////////////////
// 业务响应构建
//////////////////////////////////////////////////////////////

http_result make_error_result(int http_status, int code, std::string message, const std::string& request_id)
{
    http_result result;
    result.http_status     = http_status;
    result.root["code"]    = code;
    result.root["message"] = std::move(message);
    if (!request_id.empty())
        result.root["request_id"] = request_id;
    result.root["data"] = Json::objectValue;
    return result;
}

http_result make_ok_result(Json::Value data, const std::string& request_id)
{
    http_result result;
    result.http_status     = 200;
    result.root["code"]    = 0;
    result.root["message"] = "ok";
    if (!request_id.empty())
        result.root["request_id"] = request_id;
    result.root["data"] = std::move(data);
    return result;
}

//////////////////////////////////////////////////////////////
// JSON 解析与字段提取
//////////////////////////////////////////////////////////////

bool is_json_content_type(const HTTPParser& parser)
{
    const auto& content_type = parser.getValueCaseInsensitive("Content-Type");
    if (!starts_with_case_insensitive(content_type, k_content_type_json))
        return false;

    if (content_type.size() == std::char_traits<char>::length(k_content_type_json))
        return true;

    const auto next = content_type[std::char_traits<char>::length(k_content_type_json)];
    return next == ';' || is_ascii_space(next);
}

std::optional<json_request> parse_json_request_body(const HTTPParser& parser, http_result& error_result)
{
    json_request request;
    std::string  errors;

    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;

    const auto& body = parser.getBody();
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    if (!reader->parse(body.data(), body.data() + body.size(), &request.root, &errors))
    {
        error_result = make_error_result(400, 40001, "invalid json");
        return std::nullopt;
    }

    if (!request.root.isObject())
    {
        error_result = make_error_result(400, 40004, "json root must be object");
        return std::nullopt;
    }

    if (request.root.isMember("request_id"))
    {
        if (!request.root["request_id"].isString())
        {
            error_result = make_error_result(400, 40003, "request_id must be string");
            return std::nullopt;
        }

        request.request_id = request.root["request_id"].asString();
    }

    return request;
}

std::optional<std::string> require_string_field(const Json::Value& root,
                                                const char*        key,
                                                http_result&       error_result,
                                                const std::string& request_id)
{
    if (!root.isMember(key))
    {
        error_result = make_error_result(400, 40002, std::string("missing required field: ") + key, request_id);
        return std::nullopt;
    }

    const auto& value = root[key];
    if (!value.isString())
    {
        error_result = make_error_result(400, 40003, std::string(key) + " must be string", request_id);
        return std::nullopt;
    }

    return value.asString();
}

std::optional<bool> get_optional_bool_field(const Json::Value& root,
                                            const char*        key,
                                            bool               default_value,
                                            http_result&       error_result,
                                            const std::string& request_id)
{
    if (!root.isMember(key))
        return default_value;

    const auto& value = root[key];
    if (!value.isBool())
    {
        error_result = make_error_result(400, 40003, std::string(key) + " must be bool", request_id);
        return std::nullopt;
    }

    return value.asBool();
}

std::optional<int> get_optional_int_field(const Json::Value& root,
                                          const char*        key,
                                          int                default_value,
                                          http_result&       error_result,
                                          const std::string& request_id)
{
    if (!root.isMember(key))
        return default_value;

    const auto& value = root[key];
    if (!value.isInt())
    {
        error_result = make_error_result(400, 40003, std::string(key) + " must be int", request_id);
        return std::nullopt;
    }

    return value.asInt();
}

std::optional<std::vector<std::string>> require_string_array_field(
    const Json::Value& root,
    const char*        key,
    http_result&       error_result,
    const std::string& request_id)
{
    if (!root.isMember(key))
    {
        error_result = make_error_result(400, 40002, std::string("missing required field: ") + key, request_id);
        return std::nullopt;
    }

    const auto& value = root[key];
    if (!value.isArray())
    {
        error_result = make_error_result(400, 40003, std::string(key) + " must be array", request_id);
        return std::nullopt;
    }

    std::vector<std::string> result;
    result.reserve(value.size());

    for (const auto& item : value)
    {
        if (!item.isString())
        {
            error_result = make_error_result(400, 40003, std::string(key) + " elements must be string", request_id);
            return std::nullopt;
        }

        auto word = item.asString();
        if (!word.empty())
            result.push_back(std::move(word));
    }

    if (result.empty())
    {
        error_result = make_error_result(400, 40004, std::string(key) + " must not be empty", request_id);
        return std::nullopt;
    }

    return result;
}

} // namespace net
