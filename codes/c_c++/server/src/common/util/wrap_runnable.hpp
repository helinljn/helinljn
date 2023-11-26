#ifndef __WRAP_RUNNABLE_H__
#define __WRAP_RUNNABLE_H__

#include "util/types.h"
#include "Poco/Runnable.h"

namespace common {

template <typename CallableType>
class wrap_runnable final : public Poco::Runnable
{
public:
    static_assert(std::is_invocable_r_v<void, CallableType>, "Must be a callable type here(function, lambda, functor, etc.)");

public:
    wrap_runnable(const CallableType& call)
        : Runnable()
        , _call(call)
    {
    }

    wrap_runnable(CallableType&& call)
        : Runnable()
        , _call(std::move(call))
    {
    }

    wrap_runnable(const wrap_runnable& wr)
        : Runnable()
        , _call(wr._call)
    {
    }

    wrap_runnable(wrap_runnable&& wr)
        : Runnable()
        , _call(std::move(wr._call))
    {
    }

    wrap_runnable& operator=(const wrap_runnable& wr)
    {
        if (this != &wr)
            _call = wr._call;
        return *this;
    }

    wrap_runnable& operator=(wrap_runnable&& wr)
    {
        if (this != &wr)
            _call = std::move(wr._call);
        return *this;
    }

    void run(void) override
    {
        return _call();
    }

private:
    CallableType _call;
};

} // namespace common

#endif // __WRAP_RUNNABLE_H__