#include "net/sw_http_server.h"
#include "net/brynet.h"
#include "net/sw_dictionary_persistence.h"
#include "net/sw_http_auth.h"
#include "net/sw_http_common.h"
#include "net/sw_http_json.h"
#include "net/sw_update_coordinator.h"
#include "net/sw_worker_registry.h"
#include "spdlog/spdlog.h"
#include <thread>
#include <utility>
#include <vector>

namespace net {
namespace {

using HttpSession         = brynet::net::http::HttpSession;
using HttpSessionHandlers = brynet::net::http::HttpSessionHandlers;
using HTTPParser          = brynet::net::http::HTTPParser;

size_t normalize_worker_count(size_t worker_count) noexcept
{
    if (worker_count != 0)
        return worker_count;

    const auto hardware_count = std::thread::hardware_concurrency();
    return hardware_count == 0 ? 1u : static_cast<size_t>(hardware_count);
}

enum class return_mode_kind
{
    contains,
    first,
    all,
};

std::string build_http_response(const http_result& result, bool keep_alive)
{
    return make_http_response(result.http_status, to_json_string(result.root), keep_alive);
}

bool parse_return_mode(
    const Json::Value& root,
    const std::string& request_id,
    return_mode_kind& mode,
    http_result& error)
{
    mode = return_mode_kind::all;

    if (!root.isMember("return_mode"))
        return true;

    if (!root["return_mode"].isString())
    {
        error = make_error_result(400, 40005, "return_mode must be string", request_id);
        return false;
    }

    const auto return_mode = root["return_mode"].asString();
    if (return_mode == "contains")
    {
        mode = return_mode_kind::contains;
        return true;
    }

    if (return_mode == "first")
    {
        mode = return_mode_kind::first;
        return true;
    }

    if (return_mode == "all")
    {
        mode = return_mode_kind::all;
        return true;
    }

    error = make_error_result(400, 40005, "return_mode must be contains, first or all", request_id);
    return false;
}

} // namespace

class sw_http_server::impl
{
public:
    explicit impl(sw_http_server_config config)
        : config_(std::move(config))
    {
    }

    ~impl()
    {
        stop();
    }

    bool start()
    {
        if (running_)
            return true;

        if (!brynet::net::base::InitSocket())
        {
            spdlog::error("failed to initialize socket library");
            return false;
        }

        word_repository repository;
        try
        {
            repository = load_word_repository(config_);
        }
        catch (const std::exception& ex)
        {
            spdlog::error("failed to load dictionary files, error: {}", ex.what());
            brynet::net::base::DestroySocket();
            return false;
        }

        const auto worker_count = normalize_worker_count(config_.worker_count);
        const auto base_engine  = build_engine_from_repository(config_.engine_config, repository);

        service_     = brynet::net::IOThreadTcpService::Create();
        event_loops_ = service_->startWorkerThread(worker_count);

        worker_registry_.initialize(event_loops_, base_engine);
        if (!worker_registry_.bind_worker_thread_locals())
        {
            spdlog::error("failed to bind worker thread locals");
            stop();
            return false;
        }

        update_coordinator_.initialize(&config_, &worker_registry_, std::move(repository));

        try
        {
            listener_.WithService(service_)
                     .WithAddr(false, config_.listen_ip, config_.listen_port)
                     .WithMaxRecvBufferSize(64 * 1024)
                     .WithEnterCallback([this](const HttpSession::Ptr& session, HttpSessionHandlers& handlers) {
                        std::ignore = session;
                         handlers.setHttpEndCallback([this](const HTTPParser& parser, const HttpSession::Ptr& session) {
                             handle_http_request(parser, session);
                         });
                     })
                     .asyncRun();
        }
        catch (const std::exception& ex)
        {
            spdlog::error("failed to start listener: {}", ex.what());
            stop();
            return false;
        }

        running_ = true;
        spdlog::info("sw http server started, listen={}:{}, workers={}",
                     config_.listen_ip,
                     config_.listen_port,
                     worker_count);
        return true;
    }

    void stop() noexcept
    {
        if (!running_ && service_ == nullptr)
            return;

        update_coordinator_.shutdown();

        if (service_ != nullptr)
        {
            try
            {
                service_->stopWorkerThread();
            }
            catch (...)
            {
            }
        }

        worker_registry_.clear();
        event_loops_.clear();
        service_.reset();
        running_ = false;

        brynet::net::base::DestroySocket();
    }

private:
    void handle_http_request(const HTTPParser& parser, const HttpSession::Ptr& session)
    {
        const auto  path       = parser.getPath();
        const bool  keep_alive = parser.isKeepAlive();
        http_result result;

        if (path == "/v1/healthz")
        {
            if (parser.method() != HTTP_GET && parser.method() != HTTP_POST)
                result = make_error_result(405, 40501, "only GET or POST is supported for /v1/healthz");
            else
                result = handle_healthz(parser);

            send_response(session, result, keep_alive);
            return;
        }

        if (parser.method() != HTTP_POST)
        {
            result = make_error_result(405, 40501, "only POST is supported");
            send_response(session, result, keep_alive);
            return;
        }

        if (!is_json_content_type(parser))
        {
            result = make_error_result(415, 41501, "content type must be application/json");
            send_response(session, result, keep_alive);
            return;
        }

        if (path == "/v1/check")
        {
            result = handle_check(parser);
            send_response(session, result, keep_alive);
        }
        else if (path == "/v1/add_words")
        {
            handle_update_async(parser, true, session, keep_alive);
        }
        else if (path == "/v1/remove_words")
        {
            handle_update_async(parser, false, session, keep_alive);
        }
        else if (path == "/v1/query_word")
        {
            result = handle_query_word(parser);
            send_response(session, result, keep_alive);
        }
        else
        {
            result = make_error_result(404, 40401, "route not found");
            send_response(session, result, keep_alive);
        }
    }

    void send_response(const HttpSession::Ptr& session, const http_result& result, bool keep_alive)
    {
        auto response = build_http_response(result, keep_alive);
        if (keep_alive)
        {
            session->send(std::move(response));
        }
        else
        {
            session->send(std::move(response), [session]() {
                session->postShutdown();
            });
        }
    }

    http_result handle_check(const HTTPParser& parser)
    {
        http_result parse_error;
        auto        request = parse_json_request_body(parser, parse_error);
        if (!request)
            return parse_error;

        auto text = require_string_field(request->root, "text", parse_error, request->request_id);
        if (!text)
            return parse_error;

        auto longest_match = get_optional_bool_field(
            request->root, "longest_match", false, parse_error, request->request_id);
        if (!longest_match)
            return parse_error;

        return_mode_kind return_mode;
        if (!parse_return_mode(request->root, request->request_id, return_mode, parse_error))
            return parse_error;

        auto max_results = get_optional_int_field(
            request->root, "max_results", 100, parse_error, request->request_id);
        if (!max_results)
            return parse_error;

        if (*max_results <= 0)
            return make_error_result(400, 40004, "max_results must be positive", request->request_id);

        auto* current_worker = worker_registry_.current_worker();
        if (current_worker == nullptr)
            return make_error_result(500, 50001, "worker context unavailable", request->request_id);

        sensitive_word::match_options options;
        options.longest_match = *longest_match;

        Json::Value data(Json::objectValue);

        if (return_mode == return_mode_kind::contains)
        {
            const auto hit = current_worker->engine.contains(*text);
            data["hit"]     = hit;
            data["matches"] = Json::arrayValue;
            return make_ok_result(std::move(data), request->request_id);
        }

        if (return_mode == return_mode_kind::first)
        {
            const auto result   = current_worker->engine.find_first(*text, options);
            data["hit"]         = static_cast<bool>(result);
            data["match_count"] = result ? 1 : 0;

            Json::Value matches(Json::arrayValue);
            if (result)
                matches.append(make_match_json(*result));

            data["matches"] = std::move(matches);
            return make_ok_result(std::move(data), request->request_id);
        }

        auto results = current_worker->engine.find_all(*text, options);
        const auto limit = static_cast<size_t>(*max_results);
        if (results.size() > limit)
            results.resize(limit);

        Json::Value matches(Json::arrayValue);
        for (const auto& item : results)
            matches.append(make_match_json(item));

        data["hit"]         = !results.empty();
        data["match_count"] = Json::UInt64(results.size());
        data["matches"]     = std::move(matches);
        return make_ok_result(std::move(data), request->request_id);
    }

    http_result handle_query_word(const HTTPParser& parser)
    {
        http_result parse_error;
        auto        request = parse_json_request_body(parser, parse_error);
        if (!request)
            return parse_error;

        auto word = require_string_field(request->root, "word", parse_error, request->request_id);
        if (!word)
            return parse_error;

        auto* current_worker = worker_registry_.current_worker();
        if (current_worker == nullptr)
            return make_error_result(500, 50001, "worker context unavailable", request->request_id);

        const auto status = current_worker->engine.query_word_status(*word);

        Json::Value data(Json::objectValue);
        data["word"]     = *word;
        data["exists"]   = status.exists;
        data["in_deny"]  = status.in_deny;
        data["in_allow"] = status.in_allow;
        return make_ok_result(std::move(data), request->request_id);
    }

    http_result handle_healthz(const HTTPParser& /*parser*/)
    {
        Json::Value data(Json::objectValue);
        data["status"]       = update_coordinator_.degraded() ? "degraded" : "up";
        data["worker_count"] = Json::UInt64(worker_registry_.size());
        data["version"]      = Json::UInt64(update_coordinator_.committed_version());
        return make_ok_result(std::move(data));
    }

    void handle_update_async(const HTTPParser& parser, bool is_add, const HttpSession::Ptr& session, bool keep_alive)
    {
        if (!is_authorized_request(config_, parser))
        {
            auto result = make_error_result(401, 40101, "unauthorized");
            send_response(session, result, keep_alive);
            return;
        }

        http_result parse_error;
        auto        request = parse_json_request_body(parser, parse_error);
        if (!request)
        {
            send_response(session, parse_error, keep_alive);
            return;
        }

        auto list_type = require_string_field(request->root, "list_type", parse_error, request->request_id);
        if (!list_type)
        {
            send_response(session, parse_error, keep_alive);
            return;
        }

        auto words = require_string_array_field(request->root, "word_list", parse_error, request->request_id);
        if (!words)
        {
            send_response(session, parse_error, keep_alive);
            return;
        }

        update_coordinator_.apply_update_async(
            *list_type,
            std::move(*words),
            is_add,
            request->request_id,
            [session, keep_alive, this](http_result result) {
                send_response(session, result, keep_alive);
            });
    }

private:
    sw_http_server_config                        config_;
    brynet::net::IOThreadTcpService::Ptr         service_;
    brynet::net::wrapper::HttpListenerBuilder    listener_;
    std::vector<EventLoopPtr>                    event_loops_;
    sw_worker_registry                           worker_registry_;
    sw_update_coordinator                        update_coordinator_;
    bool                                         running_ = false;
};

sw_http_server::sw_http_server(sw_http_server_config config)
    : impl_(std::make_unique<impl>(std::move(config)))
{
}

sw_http_server::~sw_http_server() = default;

bool sw_http_server::start()
{
    return impl_->start();
}

void sw_http_server::stop() noexcept
{
    impl_->stop();
}

} // namespace net
