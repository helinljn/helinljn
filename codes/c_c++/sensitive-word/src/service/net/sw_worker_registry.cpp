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

} // namespace net