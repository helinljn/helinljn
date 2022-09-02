#include "test_libevent.h"

#include "fmt/core.h"
#include "fmt/format.h"

#include "event2/util.h"
#include "event2/buffer.h"

void test_libevent_random(void)
{
    ev_uint32_t random_num = 0;
    int         test_times = 10;
    for (int idx = 0; idx != test_times; ++idx)
    {
        evutil_secure_rng_get_bytes(&random_num, sizeof(random_num));
        fmt::print("Random ev_uint32_t={}\n", random_num);
    }

    fmt::print("{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __FUNCTION__)));
}

void test_libevent_evbuffer(void)
{
    evbuffer* buf = evbuffer_new();
    abort_assert(buf != nullptr && "Error in evbuffer_new!!!");

    const char*  content_str = "01234567890123456789012345678901234567890123456789";
    const size_t content_len = strlen(content_str);
    const size_t add_times   = 20;

    for (size_t idx = 0; idx != add_times; ++idx)
    {
        abort_assert(evbuffer_add(buf, content_str, content_len) != -1 && "Error in evbuffer_add!!!");
    }

    size_t buflen = evbuffer_get_length(buf);
    abort_assert(buflen == content_len * add_times && "Error in evbuffer_get_length!!!");

    char content_buf[128];
    for (size_t idx = 0; idx != add_times; ++idx)
    {
        abort_assert(50 == evbuffer_copyout(buf, content_buf, 50) && "Error in evbuffer_copyout!!!");
        abort_assert(-1 != evbuffer_drain(buf, 50) && "Error in evbuffer_drain!!!");
    }

    evbuffer_free(buf);

    fmt::print("{}\n", fmt::format("{:-^60}", fmt::format(" !!!{}() success!!! ", __FUNCTION__)));
}

void test_libevent_all(void)
{
    libevent_init();

    test_libevent_random();
    test_libevent_evbuffer();

    libevent_destroy();
}