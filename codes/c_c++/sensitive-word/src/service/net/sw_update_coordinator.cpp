#include "net/sw_update_coordinator.h"
#include "spdlog/spdlog.h"
#include <utility>

namespace net {

sw_update_coordinator::~sw_update_coordinator()
{
    shutdown();
}

void sw_update_coordinator::initialize(const sw_http_server_config* config,
                                       sw_worker_registry*          worker_registry,
                                       word_repository              repository)
{
    config_ = config;
    worker_registry_ = worker_registry;
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        repository_ = std::move(repository);
    }
    next_version_ = 0;
    committed_version_.store(0);
    degraded_.store(false);
    shutdown_flag_.store(false);

    worker_thread_ = std::thread([this] { worker_thread_func(); });
}

void sw_update_coordinator::shutdown()
{
    if (!worker_thread_.joinable())
        return;

    shutdown_flag_.store(true);
    queue_cv_.notify_all();

    if (worker_thread_.joinable())
        worker_thread_.join();

    config_ = nullptr;
    worker_registry_ = nullptr;
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        repository_ = {};
    }
    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        while (!task_queue_.empty())
            task_queue_.pop();
    }
    next_version_ = 0;
    committed_version_.store(0);
    degraded_.store(false);
}

word_repository sw_update_coordinator::snapshot_repository() const
{
    std::lock_guard<std::mutex> lock(state_mutex_);
    return repository_;
}

uint64_t sw_update_coordinator::committed_version() const noexcept
{
    return committed_version_.load();
}

bool sw_update_coordinator::degraded() const noexcept
{
    return degraded_.load();
}

void sw_update_coordinator::apply_update_async(const std::string&       list_type,
                                               std::vector<std::string> words,
                                               bool                     is_add,
                                               const std::string&       request_id,
                                               update_callback          callback)
{
    update_task task;
    task.list_type  = list_type;
    task.words      = std::move(words);
    task.is_add     = is_add;
    task.request_id = request_id;
    task.callback   = std::move(callback);

    {
        std::lock_guard<std::mutex> lock(queue_mutex_);
        task_queue_.push(std::move(task));
    }
    queue_cv_.notify_one();
}

void sw_update_coordinator::worker_thread_func()
{
    while (true)
    {
        update_task task;
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            queue_cv_.wait(lock, [this] {
                return shutdown_flag_.load() || !task_queue_.empty();
            });

            if (shutdown_flag_.load() && task_queue_.empty())
                break;

            if (task_queue_.empty())
                continue;

            task = std::move(task_queue_.front());
            task_queue_.pop();
        }

        auto result = apply_update_internal(task.list_type, task.words, task.is_add, task.request_id);
        if (task.callback)
            task.callback(std::move(result));
    }
}

http_result sw_update_coordinator::apply_update_internal(const std::string&              list_type,
                                                         const std::vector<std::string>& words,
                                                         bool                            is_add,
                                                         const std::string&              request_id)
{
    bool ok = false;
    const auto op = resolve_update_op(list_type, is_add, ok);
    if (!ok)
        return make_error_result(400, 40004, "list_type must be deny or allow", request_id);

    word_repository previous_repository;
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        previous_repository = repository_;
    }

    update_command command;
    command.version = next_version_ + 1;
    command.op      = op;
    command.words   = words;

    word_repository candidate_repository = previous_repository;
    apply_update_to_word_sets(candidate_repository, command);

    if (worker_registry_ == nullptr || !worker_registry_->broadcast_update(command))
    {
        const bool rollback_ok = worker_registry_ != nullptr && config_ != nullptr &&
                                 worker_registry_->broadcast_rebuild(config_->engine_config, previous_repository);
        degraded_.store(!rollback_ok);

        spdlog::error("worker broadcast failed, rollback_ok={}", rollback_ok);
        return make_error_result(
            500,
            rollback_ok ? 50002 : 50005,
            rollback_ok ? "worker sync failed" : "worker sync and rollback failed",
            request_id);
    }

    if (config_ == nullptr || !persist_word_repository(*config_, candidate_repository))
    {
        const bool rollback_ok = worker_registry_ != nullptr && config_ != nullptr &&
                                 worker_registry_->broadcast_rebuild(config_->engine_config, previous_repository);
        degraded_.store(!rollback_ok);

        spdlog::error("persistence failed, rollback_ok={}", rollback_ok);
        return make_error_result(
            500,
            rollback_ok ? 50003 : 50004,
            rollback_ok ? "persistence failed" : "rollback failed",
            request_id);
    }

    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        repository_ = std::move(candidate_repository);
    }

    committed_version_.store(command.version);
    next_version_ = command.version;
    degraded_.store(false);

    Json::Value data(Json::objectValue);
    data["list_type"]       = list_type;
    data["requested"]       = Json::UInt64(command.words.size());
    data["applied_workers"] = Json::UInt64(worker_registry_ == nullptr ? 0 : worker_registry_->size());
    data["version"]         = Json::UInt64(command.version);
    data["persisted"]       = true;
    return make_ok_result(std::move(data), request_id);
}

update_op sw_update_coordinator::resolve_update_op(const std::string& list_type,
                                                   bool               is_add,
                                                   bool&              ok) const
{
    ok = true;

    if (list_type == "deny")
        return is_add ? update_op::add_deny : update_op::remove_deny;

    if (list_type == "allow")
        return is_add ? update_op::add_allow : update_op::remove_allow;

    ok = false;
    return update_op::add_deny;
}

} // namespace net
