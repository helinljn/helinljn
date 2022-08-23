#include "test_all.h"
#include "test_misc.h"

void test_misc(void)
{
    test_sizeof();

    test_numeric_limits();

    test_safe_time();

    test_memory_and_hex_convert();

    test_pair_tuple_tie();
}