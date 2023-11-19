#ifndef __WRAP_CALLABLE_H__
#define __WRAP_CALLABLE_H__

#include "util/types.h"
#include "Poco/Runnable.h"

namespace Poco {

template <typename Callable>
class WrapCallable final : public Poco::Runnable
{
public:
    static_assert(std::is_invocable_r_v<void, Callable>, "Callable must be a callable type (function, lambda, functor, etc.)");

public:
    WrapCallable(const Callable& call)
        : Runnable()
        , _call(call)
    {
    }

    WrapCallable(Callable&& call)
        : Runnable()
        , _call(std::move(call))
    {
    }

    WrapCallable(const WrapCallable& wc)
        : Runnable()
        , _call(wc._call)
    {
    }

    WrapCallable(WrapCallable&& wc)
        : Runnable()
        , _call(std::move(wc._call))
    {
    }

    WrapCallable& operator=(const WrapCallable& wc)
    {
        if (this != &wc)
            _call = wc._call;
        return *this;
    }

    WrapCallable& operator=(WrapCallable&& wc)
    {
        if (this != &wc)
            _call = std::move(wc._call);
        return *this;
    }

    void run(void) override
    {
        return _call();
    }

private:
    Callable _call;
};

} // namespace Poco

#endif // __WRAP_CALLABLE_H__