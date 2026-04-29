#include "net/sw_http_common.h"
#include <cctype>

namespace net {

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

std::string to_json_string(const Json::Value& value)
{
    thread_local Json::StreamWriterBuilder builder = []() {
        Json::StreamWriterBuilder b;
        b["indentation"] = "";
        return b;
    }();

    return Json::writeString(builder, value);
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

} // namespace net