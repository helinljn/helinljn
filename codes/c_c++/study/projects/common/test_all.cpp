#include "test_all.h"

#include "test_cpp/test_misc.h"
#include "test_libevent/test_libevent.h"
#include "test_protocol/test_protocol.h"

void test_cpp(void)
{
    // test_misc
    test_sizeof();
    test_numeric_limits();
    test_safe_time();
    test_memory_and_hex_convert();
    test_pair_tuple_tie();
}

void test_libevent(void)
{
    test_libevent_all();
}

void test_protocol(void)
{
    test_protocol_all();
}