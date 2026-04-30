#include <chrono>
#include <cstdlib>
#include <thread>
#include <tuple>
#include "mimalloc.h"
#include "net/brynet.h"
#include "net/sw_http_server.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/spdlog.h"

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

} // namespace

int main(int argc, char** argv)
{
    static_assert(__cplusplus == 201703);

    std::ignore = argc;
    std::ignore = argv;

    if (!verify_mimalloc())
        return EXIT_FAILURE;

    net::sw_http_server_config config;
    config.listen_ip      = "0.0.0.0";
    config.listen_port    = 9000;
    config.worker_count   = 0;

    net::sw_http_server server(std::move(config));
    if (!server.start())
    {
        spdlog::error("failed to start sensitive-word http server");
        return EXIT_FAILURE;
    }

    fmt::print("service started at http://0.0.0.0:9000\n");
    fmt::print("press any key to exit...\n");

    while (!brynet::base::app_kbhit())
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    server.stop();
    return EXIT_SUCCESS;
}
