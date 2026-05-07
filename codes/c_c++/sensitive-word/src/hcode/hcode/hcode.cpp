#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include "mimalloc.h"
#include "core/stack_trace.h"
#include <cstdlib>
#include <new>

#define TRY_BEGIN try {
#define TRY_END   } catch(...) {}

namespace {

/**
 * @brief 验证 mimalloc 库是否被正确加载
 * @return true 如果 mimalloc 库被正确加载，否则返回 false
 */
bool verify_mimalloc()
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

////////////////////////////////////////////////////////////////
// 初始化器
////////////////////////////////////////////////////////////////
class initializer final
{
public:
    initializer()
    {
        TRY_BEGIN
            core::stack_trace::initialize();
        TRY_END
    }

    ~initializer()
    {
        TRY_BEGIN
            core::stack_trace::uninitialize();
        TRY_END
    }
};

} // namespace

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    if (!verify_mimalloc())
        return EXIT_FAILURE;

    initializer init;

    doctest::Context context;
    context.applyCommandLine(argc, argv);

    const int result = context.run();
    if (context.shouldExit())
        return result;

    return result;
}