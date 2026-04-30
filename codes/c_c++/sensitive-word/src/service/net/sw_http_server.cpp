#include "net/sw_http_server.h"
#include "net/sw_http_protocol.h"
#include "net/sw_worker_registry.h"
#include "sw/word_dictionary_loader.h"
#include "core/brynet.h"
#include "spdlog/spdlog.h"
#include <atomic>
#include <thread>
#include <utility>
#include <vector>

namespace net {
namespace {

using HttpSession         = brynet::net::http::HttpSession;
using HttpSessionHandlers = brynet::net::http::HttpSessionHandlers;
using HTTPParser          = brynet::net::http::HTTPParser;

constexpr size_t k_max_recv_buffer_size = 64 * 1024;  // 64KB HTTP 接收缓冲区

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
        if (running_.load(std::memory_order_acquire))
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

        try
        {
            listener_.WithService(service_)
                     .WithAddr(false, config_.listen_ip, config_.listen_port)
                     .WithMaxRecvBufferSize(k_max_recv_buffer_size)
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

        running_.store(true, std::memory_order_release);
        spdlog::info("sw http server started, listen={}:{}, workers={}",
                     config_.listen_ip,
                     config_.listen_port,
                     worker_count);
        return true;
    }

    void stop() noexcept
    {
        if (!running_.load(std::memory_order_acquire) && service_ == nullptr)
            return;

        running_.store(false, std::memory_order_release);

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

        brynet::net::base::DestroySocket();
    }

private:
    std::set<std::string> load_word_set_from_file(const std::string& path)
    {
        std::set<std::string> result;
        for (auto& word : sensitive_word::load_words_from_file(path))
            result.insert(std::move(word));
        return result;
    }

    word_repository load_word_repository(const sw_http_server_config& config)
    {
        word_repository repository;
        repository.deny_words  = load_word_set_from_file(config.deny_file_path);
        repository.allow_words = load_word_set_from_file(config.allow_file_path);
        return repository;
    }

    sensitive_word::sensitive_word_engine build_engine_from_repository(
        const sensitive_word::sensitive_word_config& config,
        const word_repository&                       repository)
    {
        sensitive_word::sensitive_word_builder builder;
        builder.ignore_case(config.ignore_case)
               .ignore_width(config.ignore_width)
               .ignore_num_style(config.ignore_num_style)
               .ignore_chinese_style(config.ignore_chinese_style)
               .ignore_english_style(config.ignore_english_style)
               .ignore_repeat(config.ignore_repeat)
               .enable_word_check(config.enable_word_check)
               .enable_num_check(config.enable_num_check)
               .num_check_len(config.num_check_len);

        std::vector<std::string> deny_list;
        deny_list.reserve(repository.deny_words.size());
        for (const auto& word : repository.deny_words)
            deny_list.push_back(word);

        std::vector<std::string> allow_list;
        allow_list.reserve(repository.allow_words.size());
        for (const auto& word : repository.allow_words)
            allow_list.push_back(word);

        builder.deny_words(std::move(deny_list));
        builder.allow_words(std::move(allow_list));
        return builder.build();
    }

    void handle_http_request(const HTTPParser& parser, const HttpSession::Ptr& session)
    {
        if (!running_.load(std::memory_order_acquire))
            return;

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
        data["status"]       = "up";
        data["worker_count"] = Json::UInt64(worker_registry_.size());
        return make_ok_result(std::move(data));
    }

private:
    sw_http_server_config                        config_;
    brynet::net::IOThreadTcpService::Ptr         service_;
    brynet::net::wrapper::HttpListenerBuilder    listener_;
    std::vector<EventLoopPtr>                    event_loops_;
    sw_worker_registry                           worker_registry_;
    std::atomic<bool>                            running_{false};
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
