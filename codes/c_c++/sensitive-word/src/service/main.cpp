#include <cstdlib>
#include <tuple>
#include "mimalloc.h"
#include "net/brynet.h"

bool veriry_mimalloc(void)
{
    // 1. 验证 mimalloc 库版本
    if (mi_version() != MI_MALLOC_VERSION)
        return false;

    // 2. 验证 std::malloc 是否被 mimalloc 接管
    void* p1 = std::malloc(1024);
    if (p1 == nullptr)
        return false;

    const bool malloc_from_mimalloc = mi_is_in_heap_region(p1);
    std::free(p1);

    if (!malloc_from_mimalloc)
        return false;

    // 3. 验证全局 new/delete 是否被 mimalloc 接管
    void* p2 = static_cast<void*>(new char[1024]);

    const bool new_from_mimalloc = mi_is_in_heap_region(p2);
    delete[] static_cast<char*>(p2);

    if (!new_from_mimalloc)
        return false;

    return true;
}

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    std::ignore = argc;
    std::ignore = argv;

    if (!veriry_mimalloc())
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}