#include "net/sw_http_json.h"
#include "net/brynet.h"
#include <cctype>
#include <memory>

namespace net {
namespace     {

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

} // namespace

bool is_json_content_type(const HTTPParser& parser)
{
    const auto& content_type = parser.getValue("Content-Type");
    return starts_with_case_insensitive(content_type, k_content_type_json);
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

        const auto word = item.asString();
        if (!word.empty())
            result.push_back(word);
    }

    if (result.empty())
    {
        error_result = make_error_result(400, 40004, std::string(key) + " must not be empty", request_id);
        return std::nullopt;
    }

    return result;
}

} // namespace net