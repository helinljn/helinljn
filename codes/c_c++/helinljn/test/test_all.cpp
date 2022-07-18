#include "test_all.h"
#include "test_size.h"
#include "test_misc.h"

void test_all(void)
{
    test_sizeof();
    test_numeric_limits();
    test_safe_time();
}