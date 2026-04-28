#include "net/sw_update_coordinator.h"
#include <utility>

namespace net {

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
    next_version_.store(0);
    committed_version_.store(0);
    degraded_.store(false);
}

void sw_update_coordinator::reset()
{
    config_ = nullptr;
    worker_registry_ = nullptr;
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        repository_ = {};
    }
    next_version_.store(0);
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

http_result sw_update_coordinator::apply_update(const std::string& list_type,
                                                std::vector<std::string> words,
                                                bool                     is_add,
                                                const std::string&       request_id)
{
    bool ok = false;
    const auto op = resolve_update_op(list_type, is_add, ok);
    if (!ok)
        return make_error_result(400, 40004, "list_type must be deny or allow", request_id);

    update_command command;
    command.version = next_version_.fetch_add(1) + 1;
    command.op      = op;
    command.words   = std::move(words);

    word_repository candidate_repository;
    {
        std::lock_guard<std::mutex> lock(state_mutex_);
        candidate_repository = repository_;
    }
    apply_update_to_word_sets(candidate_repository, command);

    if (worker_registry_ == nullptr || !worker_registry_->broadcast_update(command))
        return make_error_result(500, 50002, "worker sync failed", request_id);

    if (config_ == nullptr || !persist_word_repository(*config_, candidate_repository))
    {
        const auto rollback_command = make_inverse_command(command);
        const bool rollback_ok =
            worker_registry_ != nullptr && worker_registry_->broadcast_update(rollback_command);
        degraded_.store(!rollback_ok);

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