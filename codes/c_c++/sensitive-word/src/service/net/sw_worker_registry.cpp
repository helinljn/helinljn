#include "net/sw_worker_registry.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <utility>

namespace net {
namespace {

thread_local worker_context* tls_worker_context = nullptr;

} // namespace

void sw_worker_registry::initialize(const std::vector<EventLoopPtr>&             event_loops,
                                    const sensitive_word::sensitive_word_engine& base_engine)
{
    event_loops_ = event_loops;
    worker_contexts_.clear();
    worker_contexts_.reserve(event_loops_.size());

    for (size_t i = 0; i < event_loops_.size(); ++i)
    {
        auto context          = std::make_unique<worker_context>();
        context->worker_index = i;
        context->event_loop   = event_loops_[i];
        context->engine       = base_engine;
        worker_contexts_.push_back(std::move(context));
    }
}

void sw_worker_registry::clear()
{
    event_loops_.clear();
    worker_contexts_.clear();
}

bool sw_worker_registry::bind_worker_thread_locals()
{
    auto completed = std::make_shared<std::atomic<size_t>>(0);
    auto wg        = brynet::base::WaitGroup::Create();
    wg->add(static_cast<int>(worker_contexts_.size()));

    for (size_t i = 0; i < worker_contexts_.size(); ++i)
    {
        auto* context = worker_contexts_[i].get();
        event_loops_[i]->runAsyncFunctor([context, completed, wg]() {
            tls_worker_context = context;
            completed->fetch_add(1, std::memory_order_relaxed);
            wg->done();
        });
    }

    wg->wait(std::chrono::seconds(3));
    return completed->load(std::memory_order_relaxed) == worker_contexts_.size();
}

worker_context* sw_worker_registry::current_worker() const noexcept
{
    return tls_worker_context;
}

size_t sw_worker_registry::size() const noexcept
{
    return worker_contexts_.size();
}

bool sw_worker_registry::broadcast_update(const update_command& command)
{
    if (tls_worker_context == nullptr)
        return false;

    auto* current_context = tls_worker_context;
    auto  completed       = std::make_shared<std::atomic<size_t>>(1);
    auto  wg              = brynet::base::WaitGroup::Create();

    apply_update_to_engine(current_context->engine, command);
    current_context->applied_version = command.version;

    auto shared_command = std::make_shared<const update_command>(command);

    for (const auto& context : worker_contexts_)
    {
        if (context.get() == current_context)
            continue;

        wg->add(1);

        auto* target_context = context.get();
        target_context->event_loop->runAsyncFunctor([target_context, shared_command, completed, wg]() {
            apply_update_to_engine(target_context->engine, *shared_command);
            target_context->applied_version = shared_command->version;
            completed->fetch_add(1, std::memory_order_relaxed);
            wg->done();
        });
    }

    wg->wait(std::chrono::seconds(5));
    return completed->load(std::memory_order_relaxed) == worker_contexts_.size();
}

void sw_worker_registry::apply_update_to_engine(sensitive_word::sensitive_word_engine& engine,
                                                const update_command&                  command)
{
    switch (command.op)
    {
        case update_op::add_deny:
            for (const auto& word : command.words)
                engine.add_word(word);
            break;
        case update_op::remove_deny:
            for (const auto& word : command.words)
                engine.remove_word(word);
            break;
        case update_op::add_allow:
            for (const auto& word : command.words)
                engine.add_allow_word(word);
            break;
        case update_op::remove_allow:
            for (const auto& word : command.words)
                engine.remove_allow_word(word);
            break;
    }
}

} // namespace net