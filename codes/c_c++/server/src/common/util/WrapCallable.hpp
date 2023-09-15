#ifndef __WRAP_CALLABLE_H__
#define __WRAP_CALLABLE_H__

#include "Poco/Runnable.h"
#include <type_traits>

namespace Poco {

template <typename Callable>
class WrapCallable final : public Poco::Runnable
{
public:
    static_assert(std::is_invocable_r_v<void, Callable>, "Callable must be a callable type (function, lambda, functor, etc.)");

public:
    WrapCallable(const Callable& functor)
        : Runnable()
        , _functor(functor)
    {
    }

    WrapCallable(Callable&& functor)
        : Runnable()
        , _functor(std::move(functor))
    {
    }

    WrapCallable(const WrapCallable& wc)
        : Runnable()
        , _functor(wc._functor)
    {
    }

    WrapCallable(WrapCallable&& wc)
        : Runnable()
        , _functor(std::move(wc._functor))
    {
    }

    WrapCallable& operator=(const WrapCallable& wc)
    {
        if (this != &wc)
            _functor = wc._functor;
        return *this;
    }

    WrapCallable& operator=(WrapCallable&& wc)
    {
        if (this != &wc)
            _functor = std::move(wc._functor);
        return *this;
    }

    void run(void) override
    {
        return _functor();
    }

private:
    Callable _functor;
};

} // namespace Poco

#endif // __WRAP_CALLABLE_H__