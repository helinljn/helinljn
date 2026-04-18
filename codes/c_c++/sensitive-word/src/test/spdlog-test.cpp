#include "doctest.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/spdlog.h"
#include "spdlog/async.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <system_error>
#include <thread>
#include <utility>

namespace fs = std::filesystem;

class scoped_test_directory
{
public:
    explicit scoped_test_directory(std::string name)
        : m_root(fs::current_path() / "tmp" / "spdlog-test")
        , m_path(m_root / std::move(name))
    {
        std::error_code ec;
        fs::remove_all(m_path, ec);
        fs::create_directories(m_path, ec);
    }

    ~scoped_test_directory()
    {
        std::error_code ec;
        fs::remove_all(m_path, ec);

        auto current = m_root;
        while (!current.empty() && current != current.root_path())
        {
            fs::remove(current, ec);
            if (ec)
            {
                ec.clear();
                break;
            }

            current = current.parent_path();
        }
    }

    scoped_test_directory(const scoped_test_directory&) = delete;
    scoped_test_directory& operator=(const scoped_test_directory&) = delete;

    fs::path file(const fs::path& relative) const
    {
        return m_path / relative;
    }

private:
    fs::path m_root;
    fs::path m_path;
};

std::string read_text_file(const fs::path& file)
{
    std::ifstream input(file, std::ios::binary);
    if (!input.is_open())
        return {};

    std::ostringstream output;
    output << input.rdbuf();
    return output.str();
}

bool contains(const std::string& text, const std::string& expected)
{
    return text.find(expected) != std::string::npos;
}

TEST_SUITE("spdlog usage")
{
    TEST_CASE("logger + ostream sink: 基础日志、格式化参数与级别过滤")
    {
        /*
         * 这是 spdlog 最常见的使用方式：
         * - 创建 logger 并绑定 sink
         * - 记录带格式化参数的日志
         * - 调整 logger level，验证 should_log / 过滤行为
         */
        auto stream = std::make_shared<std::ostringstream>();
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(*stream);
        auto logger = std::make_shared<spdlog::logger>("ostream-basic", sink);

        logger->set_pattern("[%n] [%^%l%$] %v");
        logger->set_level(spdlog::level::info);

        CHECK(logger->should_log(spdlog::level::info));
        CHECK(!logger->should_log(spdlog::level::debug));

        logger->info("hello {}", "spdlog");
        logger->warn("answer={}", 42);
        logger->debug("this should be filtered");

        const std::string output = stream->str();
        CHECK(contains(output, "[ostream-basic]"));
        CHECK(contains(output, "[info] hello spdlog"));
        CHECK(contains(output, "[warning] answer=42"));
        CHECK(!contains(output, "this should be filtered"));
    }

    TEST_CASE("default logger API: 全局日志接口与替换默认 logger")
    {
        /*
         * 很多项目会直接使用 spdlog::info / warn / error 这类全局接口。
         * 这背后依赖默认 logger，因此这里验证：
         * - 替换默认 logger
         * - 使用全局 API 输出日志
         * - 用完后恢复原默认 logger，避免污染其它测试
         */
        auto original_default = spdlog::default_logger();

        auto stream = std::make_shared<std::ostringstream>();
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(*stream);
        auto logger = std::make_shared<spdlog::logger>("default-captured", sink);

        logger->set_pattern("[%l] %v");
        logger->set_level(spdlog::level::trace);

        spdlog::set_default_logger(logger);
        spdlog::info("user={}, id={}", "alice", 7);
        spdlog::warn("quota is low");
        spdlog::default_logger()->flush();

        const std::string output = stream->str();
        CHECK(contains(output, "[info] user=alice, id=7"));
        CHECK(contains(output, "[warning] quota is low"));

        spdlog::set_default_logger(original_default);
    }

    TEST_CASE("registry: 注册、获取与删除命名 logger")
    {
        /*
         * spdlog 提供全局 registry 管理具名 logger。
         * 这在大型工程里非常常见：模块启动时注册，其他位置按名字获取。
         */
        spdlog::drop("registry-example");

        auto stream = std::make_shared<std::ostringstream>();
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(*stream);
        auto logger = std::make_shared<spdlog::logger>("registry-example", sink);

        spdlog::register_logger(logger);

        auto fetched = spdlog::get("registry-example");
        REQUIRE(fetched != nullptr);

        fetched->info("registry works");
        CHECK(contains(stream->str(), "registry works"));

        spdlog::drop("registry-example");
        CHECK(spdlog::get("registry-example") == nullptr);
    }

    TEST_CASE("pattern + source location: 自定义 pattern 与源位置输出")
    {
        /*
         * pattern 是 spdlog 最常用的定制点之一。
         * 这里同时覆盖：
         * - logger name / level / message 等 pattern 标记
         * - source_loc 记录文件、行号、函数名
         */
        auto stream = std::make_shared<std::ostringstream>();
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(*stream);
        auto logger = std::make_shared<spdlog::logger>("pattern-demo", sink);

        logger->set_pattern("[%n] [%l] [%s:%# %!] %v");
        logger->log(spdlog::source_loc{"spdlog-test.cpp", 123, "pattern_case"},
                    spdlog::level::info,
                    "line check");

        const std::string output = stream->str();
        CHECK(contains(output, "[pattern-demo]"));
        CHECK(contains(output, "[info]"));
        CHECK(contains(output, "spdlog-test.cpp:123"));
        CHECK(contains(output, "pattern_case"));
        CHECK(contains(output, "line check"));
    }

    TEST_CASE("backtrace: 启用回溯缓冲并转储最近日志")
    {
        /*
         * backtrace 适合“平时不落盘，出问题时回放最近 N 条日志”的场景。
         * 这里验证：
         * - enable_backtrace
         * - debug 日志进入回溯缓冲
         * - dump_backtrace 输出历史记录
         */
        auto stream = std::make_shared<std::ostringstream>();
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(*stream);
        auto logger = std::make_shared<spdlog::logger>("backtrace-demo", sink);

        logger->set_level(spdlog::level::trace);
        logger->enable_backtrace(4);

        logger->debug("step {}", 1);
        logger->info("step {}", 2);
        logger->warn("step {}", 3);
        logger->error("step {}", 4);
        logger->dump_backtrace();

        const std::string output = stream->str();
        CHECK(contains(output, "step 1"));
        CHECK(contains(output, "step 2"));
        CHECK(contains(output, "step 3"));
        CHECK(contains(output, "step 4"));
    }

    TEST_CASE("clone + multiple sinks: 克隆 logger 并复用配置")
    {
        /*
         * clone 适合在保持 sink / formatter / level 等配置一致的前提下，
         * 快速复制一个新名字的 logger。
         */
        auto stream = std::make_shared<std::ostringstream>();
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(*stream);
        auto logger = std::make_shared<spdlog::logger>("origin", sink);

        logger->set_pattern("[%n] %v");
        logger->set_level(spdlog::level::info);

        auto cloned = logger->clone("origin.clone");
        REQUIRE(cloned != nullptr);

        cloned->info("cloned logger");
        const std::string output = stream->str();

        CHECK(contains(output, "[origin.clone] cloned logger"));
        CHECK(cloned->level() == logger->level());
    }

    TEST_CASE("basic file sink: 文件日志写入与读取验证")
    {
        /*
         * basic_file_sink 是最常用的落盘方式。
         * 测试需要验证文件确实写入，同时保证结束后把目录清理掉。
         */
        scoped_test_directory dir("basic-file");
        const fs::path log_file = dir.file("basic.log");

        auto logger = spdlog::basic_logger_mt("basic-file-logger", log_file.string(), true);
        logger->set_pattern("[%l] %v");
        logger->set_level(spdlog::level::trace);

        logger->info("file logging works");
        logger->error("error code={}", 500);
        logger->flush();

        const std::string content = read_text_file(log_file);
        CHECK(contains(content, "[info] file logging works"));
        CHECK(contains(content, "[error] error code=500"));

        spdlog::drop("basic-file-logger");
    }

    TEST_CASE("rotating file sink: 文件滚动")
    {
        /*
         * rotating_file_sink 也是高频能力：
         * - 单文件达到上限后自动滚动
         * - 保留若干历史文件
         * 为了让测试更快触发滚动，这里使用很小的 max_size。
         */
        scoped_test_directory dir("rotating-file");
        const fs::path log_file = dir.file("rotating.log");

        auto logger = spdlog::rotating_logger_mt("rotating-file-logger",
                                                 log_file.string(),
                                                 128,
                                                 2);
        logger->set_pattern("%v");

        for (int i = 0; i < 20; ++i)
        {
            logger->info("message-{}-xxxxxxxxxxxxxxxx", i);
        }
        logger->flush();

        CHECK(fs::exists(log_file));

        const bool res = fs::exists(dir.file("rotating.1.log")) || fs::exists(dir.file("rotating.log.1"));
        CHECK(res);

        spdlog::drop("rotating-file-logger");
    }

    TEST_CASE("async logger: 异步日志输出")
    {
        /*
         * 异步 logger 是 spdlog 的常见进阶能力：
         * - 通过线程池异步写日志
         * - 降低业务线程等待 sink I/O 的时间
         */
        auto stream = std::make_shared<std::ostringstream>();
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(*stream);

        spdlog::init_thread_pool(1024, 1);

        auto logger = std::make_shared<spdlog::async_logger>("async-demo",
                                                             sink,
                                                             spdlog::thread_pool(),
                                                             spdlog::async_overflow_policy::block);
        logger->set_pattern("[%l] %v");
        logger->set_level(spdlog::level::trace);

        logger->info("async {}", 1);
        logger->warn("async {}", 2);
        logger->flush();

        for (int i = 0; i < 50 && !contains(stream->str(), "async 2"); ++i)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        const std::string output = stream->str();
        CHECK(contains(output, "[info] async 1"));
        CHECK(contains(output, "[warning] async 2"));
    }

    TEST_CASE("flush_on + manual flush: 达到级别后自动刷新")
    {
        /*
         * flush_on 常用于生产环境：
         * - 普通 info/debug 可以缓冲
         * - error 及以上自动 flush
         */
        auto stream = std::make_shared<std::ostringstream>();
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(*stream);
        auto logger = std::make_shared<spdlog::logger>("flush-demo", sink);

        logger->set_pattern("%v");
        logger->flush_on(spdlog::level::err);

        logger->info("buffered-info");
        logger->error("critical-error");
        logger->flush();

        const std::string output = stream->str();
        CHECK(contains(output, "buffered-info"));
        CHECK(contains(output, "critical-error"));
    }
}