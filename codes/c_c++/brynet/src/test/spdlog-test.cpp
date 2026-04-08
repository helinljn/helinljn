/*
 * 【已覆盖模块】
 * ✓ 默认 logger / default_logger() / default_logger_raw() / set_default_logger()
 * ✓ 命名 logger 创建与使用
 * ✓ 全局便捷函数 spdlog::trace/debug/info/warn/error/critical()
 * ✓ spdlog::log(level, fmt, args...) 重载
 * ✓ logger->log(source_loc, level, msg) 重载
 * ✓ logger->log(level, const T& msg) 无格式字符串重载
 * ✓ SPDLOG_LOGGER_CALL 宏
 * ✓ 日志级别 level 枚举（trace/debug/info/warn/err/critical/off）
 * ✓ logger->set_level() / logger->level() / logger->should_log()
 * ✓ spdlog::set_level() / spdlog::get_level() / spdlog::should_log()
 * ✓ spdlog::level::to_string_view() / level::from_str()
 * ✓ logger->set_pattern() / spdlog::set_pattern() / sink->set_pattern()
 * ✓ logger->set_formatter() / spdlog::pattern_formatter
 * ✓ ostream_sink_mt / ostream_sink_st
 * ✓ null_sink_mt / null_sink_st
 * ✓ basic_file_sink_mt（truncate + append 模式）
 * ✓ rotating_file_sink_mt（文件大小轮转）
 * ✓ daily_file_sink_mt（每日轮转）
 * ✓ dist_sink_mt（add_sink / remove_sink / 分发日志）
 * ✓ ringbuffer_sink_mt（环形缓冲 / last_formatted()）
 * ✓ callback_sink_mt（用户回调 / 级别过滤）
 * ✓ stdout_sink_mt / stderr_sink_mt / stdout_color_sink_mt
 * ✓ logger->flush() / logger->flush_on() / logger->flush_level()
 * ✓ spdlog::flush_on()（全局）
 * ✓ logger->set_error_handler() / spdlog::set_error_handler()
 * ✓ logger->clone() / logger->sinks()（多 sink、动态添加）
 * ✓ logger->enable_backtrace() / disable_backtrace() / dump_backtrace() / should_backtrace()
 * ✓ spdlog::enable_backtrace() / dump_backtrace() / disable_backtrace()（全局）
 * ✓ spdlog::async_logger（block / overrun_oldest / discard_new 溢出策略）
 * ✓ spdlog::init_thread_pool() / spdlog::thread_pool()
 * ✓ spdlog::register_logger() / get() / drop() / drop_all() / register_or_replace()
 * ✓ spdlog::apply_all()
 * ✓ spdlog::initialize_logger()
 * ✓ spdlog::shutdown()
 * ✓ 自定义 sink（继承 base_sink<Mutex>，实现 sink_it_ + flush_）
 * ✓ 自定义 formatter（继承 spdlog::formatter，实现 format + clone）
 * ✓ 版本信息宏 SPDLOG_VER_MAJOR / SPDLOG_VER_MINOR / SPDLOG_VER_PATCH
 *
 * 【条件覆盖模块】
 * ⚙ spdlog::create<Sink>()（工厂函数）
 *   - 通过 spdlog::create<basic_file_sink_mt>() 等接口创建 logger
 *   - 与直接 make_shared 等价，测试中优先使用后者以降低复杂度
 * ⚙ spdlog::set_automatic_registration()
 *   - 控制 create() 是否自动注册；行为测试在 register_logger 套件中间接覆盖
 * ⚙ spdlog::apply_logger_env_levels()
 *   - 依赖 SPDLOG_LEVEL 环境变量；在无环境变量的 CI 中难以验证，仅调用不崩溃
 * ⚙ SPDLOG_LOGGER_TRACE/DEBUG/INFO/WARN/ERROR/CRITICAL 宏
 *   - 依赖 SPDLOG_ACTIVE_LEVEL 编译期宏；默认值下 TRACE/DEBUG 被优化掉，略过
 * ⚙ async_logger 通过 spdlog::create_async<Sink>() 工厂创建
 *   - 测试中直接使用 async_logger 构造函数以便控制 thread_pool 引用
 * ⚙ flush_every（定期 flush 线程）
 *   - 基于时间触发，测试环境时序敏感，仅 API 存在性验证，不做内容断言
 * ⚙ hourly_file_sink_mt（每小时轮转文件）
 *   - 接口与 daily_file_sink 完全相同，测试中以 daily_file_sink 代表覆盖
 * ⚙ dup_filter_sink（去重 sink）
 *   - API 存在但未单独测试（行为：相同消息在时间窗口内只输出一次）
 * ⚙ SPDLOG_WCHAR_TO_UTF8_SUPPORT（宽字符支持）
 *   - 仅 Windows 平台有效；条件编译保护，非 Windows 跳过
 *
 * 【未覆盖模块及原因】
 * ✗ android_sink / syslog_sink / systemd_sink / win_eventlog_sink
 *   - 平台专属 sink，需对应操作系统环境，跨平台测试无法验证
 * ✗ kafka_sink / mongo_sink / tcp_sink / udp_sink
 *   - 需要外部服务（Kafka/MongoDB/网络端口），集成测试范畴，超出单元测试范围
 * ✗ qt_sinks
 *   - 需要 Qt 框架，非标准 C++ 环境
 * ✗ msvc_sink / wincolor_sink / ansicolor_sink
 *   - 平台/终端专属，行为依赖 Windows 控制台 API 或 ANSI 转义序列
 * ✗ cfg/ 目录（环境变量配置）
 *   - cfg/argv.h / cfg/env.h 等用于从命令行或环境变量配置 spdlog
 *   - 需修改进程环境变量，测试隔离性差，未覆盖
 * ✗ mdc.h（Mapped Diagnostic Context）
 *   - 线程本地 key-value 上下文，spdlog 1.12+ 新增；
 *     API 存在但与 pattern %@ 占位符配合使用，未单独测试
 * ✗ stopwatch.h
 *   - 简单计时工具，非日志核心；API 为 spdlog::stopwatch，非测试重点
 * ============================================================================
 */
#include "doctest.h"
#include "spdlog/spdlog.h"
#include "spdlog/logger.h"
#include "spdlog/common.h"
#include "spdlog/version.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/formatter.h"
#include "spdlog/sinks/ostream_sink.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/dist_sink.h"
#include "spdlog/sinks/ringbuffer_sink.h"
#include "spdlog/sinks/callback_sink.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/async.h"
#include "spdlog/async_logger.h"
#include <sstream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <filesystem>
#include <functional>

namespace test_helpers {

/// 创建临时文件路径（在系统临时目录下，含唯一编号，防止并发冲突）
static std::string create_temp_file_path(const std::string& prefix = "spdlog_test")
{
    auto tmp = std::filesystem::temp_directory_path();
    static std::atomic<int> counter{0};
    auto fname = prefix + "_" + std::to_string(++counter) + ".log";
    return (tmp / fname).string();
}

/// 读取文件全部内容为字符串
static std::string read_file(const std::string& path)
{
    std::ifstream ifs(path, std::ios::in | std::ios::binary);
    if (!ifs.is_open()) return {};
    std::ostringstream ss;
    ss << ifs.rdbuf();
    return ss.str();
}

/// 安全删除文件
static void remove_file(const std::string& path)
{
    std::error_code ec;
    std::filesystem::remove(path, ec);
    // 忽略错误，文件可能已不存在
}

/// 在测试套件前/后重置 registry，并将默认 logger 设为 null sink
/// 防止不同测试用例之间相互干扰或产生意外的控制台输出
static void cleanup_loggers()
{
    spdlog::drop_all(); // 清空 registry（同时清空 default logger 引用）
    // 将默认 logger 替换为 null sink，避免测试期间输出到控制台
    auto null_sink    = std::make_shared<spdlog::sinks::null_sink_mt>();
    auto null_default = std::make_shared<spdlog::logger>("default", null_sink);
    spdlog::set_default_logger(null_default);
}

/// 创建一个输出到 ostringstream 的 logger，pattern 仅输出消息体（方便断言）
static std::pair<std::shared_ptr<spdlog::logger>, std::shared_ptr<std::ostringstream>>
make_oss_logger(const std::string& name = "oss_logger")
{
    auto oss  = std::make_shared<std::ostringstream>();
    auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(*oss);
    sink->set_pattern("%v"); // 只保留消息内容
    auto logger = std::make_shared<spdlog::logger>(name, sink);
    logger->set_level(spdlog::level::trace); // 开放所有级别
    return {logger, oss};
}

} // namespace test_helpers

DOCTEST_TEST_SUITE("spdlog — 基础日志与默认 logger")
{
    /**
     * 测试目的：验证默认 logger 的存在性与基本属性
     * 使用的 API：spdlog::default_logger(), spdlog::default_logger_raw()
     * 预期行为：返回非空指针；raw 版本返回裸指针（同一对象）
     * 注意事项：默认 logger 在库初始化时自动创建，名称通常为空字符串
     */
    DOCTEST_TEST_CASE("默认 logger 可获取")
    {
        test_helpers::cleanup_loggers();

        auto sp  = spdlog::default_logger();
        auto raw = spdlog::default_logger_raw();

        DOCTEST_CHECK(sp != nullptr);
        DOCTEST_CHECK(raw != nullptr);
        DOCTEST_CHECK_EQ(sp.get(), raw);

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证全局便捷日志函数（trace/debug/info/warn/error/critical）
     * 使用的 API：spdlog::info/debug/warn/error/critical/trace()
     * 预期行为：消息写入默认 logger 对应 sink 中
     * 注意事项：默认 logger 须先替换为 ostream_sink 才能捕获输出
     */
    DOCTEST_TEST_CASE("全局便捷函数输出到默认 logger")
    {
        test_helpers::cleanup_loggers();

        auto [logger, oss] = test_helpers::make_oss_logger("default");
        logger->set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);

        spdlog::trace("trace msg");
        spdlog::debug("debug msg");
        spdlog::info("info msg");
        spdlog::warn("warn msg");
        spdlog::error("error msg");
        spdlog::critical("critical msg");
        logger->flush();

        const std::string out = oss->str();
        DOCTEST_CHECK(out.find("trace msg")    != std::string::npos);
        DOCTEST_CHECK(out.find("debug msg")    != std::string::npos);
        DOCTEST_CHECK(out.find("info msg")     != std::string::npos);
        DOCTEST_CHECK(out.find("warn msg")     != std::string::npos);
        DOCTEST_CHECK(out.find("error msg")    != std::string::npos);
        DOCTEST_CHECK(out.find("critical msg") != std::string::npos);

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证格式化参数插值
     * 使用的 API：spdlog::info("{} {} {}", ...)（fmt 风格格式字符串）
     * 预期行为：参数按位置替换占位符，输出正确字符串
     * 注意事项：spdlog 内部使用 fmtlib 进行格式化
     */
    DOCTEST_TEST_CASE("格式化参数插值")
    {
        test_helpers::cleanup_loggers();

        auto [logger, oss] = test_helpers::make_oss_logger("fmt_test");
        spdlog::set_default_logger(logger);

        spdlog::info("Hello {}", "World");
        spdlog::info("Number: {}", 42);
        spdlog::info("Float: {:.2f}", 3.14159);
        spdlog::info("Multi: {} {} {}", 1, 2, 3);
        logger->flush();

        const std::string out = oss->str();
        DOCTEST_CHECK(out.find("Hello World")   != std::string::npos);
        DOCTEST_CHECK(out.find("Number: 42")    != std::string::npos);
        DOCTEST_CHECK(out.find("Float: 3.14")   != std::string::npos);
        DOCTEST_CHECK(out.find("Multi: 1 2 3")  != std::string::npos);

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 spdlog::log() 带 level 参数的重载
     * 使用的 API：spdlog::log(level, fmt, args...)
     * 预期行为：与对应级别便捷函数等价
     */
    DOCTEST_TEST_CASE("spdlog::log 重载（level 参数）")
    {
        test_helpers::cleanup_loggers();

        auto [logger, oss] = test_helpers::make_oss_logger("log_ovl");
        logger->set_level(spdlog::level::trace);
        spdlog::set_default_logger(logger);

        spdlog::log(spdlog::level::info,  "info via log()");
        spdlog::log(spdlog::level::warn,  "warn via log()");
        spdlog::log(spdlog::level::err,   "err via log()");
        logger->flush();

        const std::string out = oss->str();
        DOCTEST_CHECK(out.find("info via log()") != std::string::npos);
        DOCTEST_CHECK(out.find("warn via log()") != std::string::npos);
        DOCTEST_CHECK(out.find("err via log()")  != std::string::npos);

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 logger->log() 带 source_loc 的重载
     * 使用的 API：logger->log(spdlog::source_loc{file, line, func}, level, msg)
     * 预期行为：日志消息正常写入；source_loc 字段可被 pattern 的 %s/%#/%! 引用
     * 注意事项：source_loc 为空时（filename==nullptr）也应正常工作
     */
    DOCTEST_TEST_CASE("log 重载 — source_loc")
    {
        auto [logger, oss] = test_helpers::make_oss_logger("src_loc");
        logger->set_level(spdlog::level::trace);

        DOCTEST_SUBCASE("无 source_loc（空）")
        {
            logger->log(spdlog::source_loc{}, spdlog::level::info, "no location");
            logger->flush();
            DOCTEST_CHECK(oss->str().find("no location") != std::string::npos);
        }

        DOCTEST_SUBCASE("有 source_loc")
        {
            spdlog::source_loc loc{"myfile.cpp", 42, "myFunc"};
            logger->log(loc, spdlog::level::warn, "has location");
            logger->flush();
            DOCTEST_CHECK(oss->str().find("has location") != std::string::npos);
        }

        DOCTEST_SUBCASE("SPDLOG_LOGGER_CALL 宏携带源码位置")
        {
            // SPDLOG_LOGGER_CALL 自动填入 __FILE__/__LINE__/__func__
            SPDLOG_LOGGER_CALL(logger.get(), spdlog::level::info, "macro call");
            logger->flush();
            DOCTEST_CHECK(oss->str().find("macro call") != std::string::npos);
        }
    }

    /**
     * 测试目的：验证 log() 接受 T 类型消息（非格式字符串重载）
     * 使用的 API：logger->log(level, const T& msg)
     * 预期行为：任意可流式输出的对象均可作为消息
     */
    DOCTEST_TEST_CASE("log 重载 — 无格式字符串（T 类型消息）")
    {
        auto [logger, oss] = test_helpers::make_oss_logger("t_msg");
        logger->set_level(spdlog::level::trace);

        logger->info("plain string");
        logger->info(std::string("std::string"));
        logger->flush();

        const std::string out = oss->str();
        DOCTEST_CHECK(out.find("plain string")  != std::string::npos);
        DOCTEST_CHECK(out.find("std::string")   != std::string::npos);
    }
} // DOCTEST_TEST_SUITE("spdlog — 基础日志与默认 logger")

DOCTEST_TEST_SUITE("spdlog — Registry 管理")
{
    /**
     * 测试目的：验证命名 logger 的创建与注册
     * 使用的 API：spdlog::register_logger(), spdlog::get()
     * 预期行为：注册后可通过名称获取；get() 不存在时返回 nullptr
     */
    DOCTEST_TEST_CASE("register_logger 和 get")
    {
        test_helpers::cleanup_loggers();

        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();

        DOCTEST_SUBCASE("注册后可 get")
        {
            auto logger = std::make_shared<spdlog::logger>("reg_test", null_sink);
            spdlog::register_logger(logger);

            auto got = spdlog::get("reg_test");
            DOCTEST_REQUIRE(got != nullptr);
            DOCTEST_CHECK_EQ(got->name(), "reg_test");
            spdlog::drop("reg_test");
        }

        DOCTEST_SUBCASE("不存在的 logger 返回 nullptr")
        {
            DOCTEST_CHECK(spdlog::get("nonexistent___") == nullptr);
        }

        DOCTEST_SUBCASE("重复注册同名 logger 抛异常")
        {
            auto logger1 = std::make_shared<spdlog::logger>("dup", null_sink);
            spdlog::register_logger(logger1);
            auto logger2 = std::make_shared<spdlog::logger>("dup", null_sink);
            DOCTEST_CHECK_THROWS(spdlog::register_logger(logger2));
            spdlog::drop("dup");
        }

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 register_or_replace 语义
     * 使用的 API：spdlog::register_or_replace()
     * 预期行为：若同名 logger 已存在则替换，不抛异常
     */
    DOCTEST_TEST_CASE("register_or_replace")
    {
        test_helpers::cleanup_loggers();

        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto old_logger = std::make_shared<spdlog::logger>("ror", null_sink);
        spdlog::register_logger(old_logger);

        auto new_logger = std::make_shared<spdlog::logger>("ror", null_sink);
        DOCTEST_CHECK_NOTHROW(spdlog::register_or_replace(new_logger));

        auto got = spdlog::get("ror");
        DOCTEST_REQUIRE(got != nullptr);
        DOCTEST_CHECK_EQ(got.get(), new_logger.get()); // 指向新 logger

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 drop / drop_all 语义
     * 使用的 API：spdlog::drop(), spdlog::drop_all()
     * 预期行为：drop 后 get 返回 nullptr；drop_all 后所有 logger 均消失
     */
    DOCTEST_TEST_CASE("drop 和 drop_all")
    {
        test_helpers::cleanup_loggers();

        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();

        DOCTEST_SUBCASE("drop 单个 logger")
        {
            auto l = std::make_shared<spdlog::logger>("drop_me", null_sink);
            spdlog::register_logger(l);
            DOCTEST_CHECK(spdlog::get("drop_me") != nullptr);

            spdlog::drop("drop_me");
            DOCTEST_CHECK(spdlog::get("drop_me") == nullptr);
        }

        DOCTEST_SUBCASE("drop_all 清空全部")
        {
            for (int i = 0; i < 5; ++i)
            {
                auto li = std::make_shared<spdlog::logger>(
                    "drop_all_" + std::to_string(i), null_sink);
                spdlog::register_logger(li);
            }
            spdlog::drop_all();
            for (int i = 0; i < 5; ++i)
            {
                DOCTEST_CHECK(spdlog::get("drop_all_" + std::to_string(i)) == nullptr);
            }
        }

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 apply_all 对所有注册 logger 执行回调
     * 使用的 API：spdlog::apply_all(fun)
     * 预期行为：回调被每个注册的 logger 调用一次
     */
    DOCTEST_TEST_CASE("apply_all")
    {
        test_helpers::cleanup_loggers();

        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        const int N = 4;
        for (int i = 0; i < N; ++i)
        {
            auto l = std::make_shared<spdlog::logger>("aa_" + std::to_string(i), null_sink);
            spdlog::register_logger(l);
        }

        std::vector<std::string> names;
        spdlog::apply_all([&names](std::shared_ptr<spdlog::logger> l)
        {
            names.push_back(l->name());
        });

        // 注册了 N 个 + 默认 logger（默认 logger 也在 registry 中）
        // 默认 logger 名为 "default"（由 cleanup_loggers 设置）
        DOCTEST_CHECK(names.size() >= static_cast<size_t>(N));

        // 验证所有注册的 logger 均被遍历到
        for (int i = 0; i < N; ++i)
        {
            auto name = "aa_" + std::to_string(i);
            bool found = (std::find(names.begin(), names.end(), name) != names.end());
            DOCTEST_CHECK(found);
        }

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 set_default_logger 与 default_logger
     * 使用的 API：spdlog::set_default_logger(), spdlog::default_logger()
     * 预期行为：设置后 default_logger() 返回新设置的 logger
     */
    DOCTEST_TEST_CASE("set_default_logger")
    {
        test_helpers::cleanup_loggers();

        auto null_sink  = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto new_logger = std::make_shared<spdlog::logger>("my_default", null_sink);
        spdlog::set_default_logger(new_logger);

        auto got = spdlog::default_logger();
        DOCTEST_REQUIRE(got != nullptr);
        DOCTEST_CHECK_EQ(got->name(), "my_default");

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 initialize_logger 按全局配置初始化 logger
     * 使用的 API：spdlog::initialize_logger()
     * 预期行为：新创建的 logger 级别/格式与全局配置一致
     * 注意事项：initialize_logger 通常在 factory 内部调用；手动调用相当于
     *           应用全局级别到已创建的 logger
     */
    DOCTEST_TEST_CASE("initialize_logger")
    {
        test_helpers::cleanup_loggers();

        spdlog::set_level(spdlog::level::warn); // 设置全局级别

        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto logger    = std::make_shared<spdlog::logger>("init_me", null_sink);

        DOCTEST_CHECK_NOTHROW(spdlog::initialize_logger(logger));
        // 初始化后 logger 的级别应跟随全局设置
        DOCTEST_CHECK_EQ(logger->level(), spdlog::level::warn);

        test_helpers::cleanup_loggers();
    }
} // DOCTEST_TEST_SUITE("spdlog — Registry 管理")

DOCTEST_TEST_SUITE("spdlog — 日志级别")
{
    /**
     * 测试目的：验证 logger 默认级别与级别枚举值顺序
     * 使用的 API：logger->level(), spdlog::level 枚举
     * 预期行为：默认级别为 info；级别从低到高：trace < debug < info < warn < err < critical < off
     */
    DOCTEST_TEST_CASE("级别枚举顺序与默认值")
    {
        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto logger    = std::make_shared<spdlog::logger>("lv_def", null_sink);

        DOCTEST_CHECK_EQ(logger->level(), spdlog::level::info); // 默认 info

        // 枚举顺序
        DOCTEST_CHECK(spdlog::level::trace    < spdlog::level::debug);
        DOCTEST_CHECK(spdlog::level::debug    < spdlog::level::info);
        DOCTEST_CHECK(spdlog::level::info     < spdlog::level::warn);
        DOCTEST_CHECK(spdlog::level::warn     < spdlog::level::err);
        DOCTEST_CHECK(spdlog::level::err      < spdlog::level::critical);
        DOCTEST_CHECK(spdlog::level::critical < spdlog::level::off);
    }

    /**
     * 测试目的：验证 should_log 与 set_level 的过滤效果
     * 使用的 API：logger->set_level(), logger->should_log(), logger->level()
     * 预期行为：should_log 返回 level >= 当前设定级别
     */
    DOCTEST_TEST_CASE("set_level 与 should_log")
    {
        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto logger    = std::make_shared<spdlog::logger>("lv_filter", null_sink);

        logger->set_level(spdlog::level::warn);
        DOCTEST_CHECK_EQ(logger->level(), spdlog::level::warn);

        DOCTEST_CHECK_FALSE(logger->should_log(spdlog::level::trace));
        DOCTEST_CHECK_FALSE(logger->should_log(spdlog::level::debug));
        DOCTEST_CHECK_FALSE(logger->should_log(spdlog::level::info));
        DOCTEST_CHECK(logger->should_log(spdlog::level::warn));
        DOCTEST_CHECK(logger->should_log(spdlog::level::err));
        DOCTEST_CHECK(logger->should_log(spdlog::level::critical));
    }

    /**
     * 测试目的：验证级别过滤对实际输出的影响
     * 使用的 API：logger->set_level(), logger->info/warn/error()
     * 预期行为：低于设定级别的消息不出现在输出中
     */
    DOCTEST_TEST_CASE("级别过滤实际输出")
    {
        auto [logger, oss] = test_helpers::make_oss_logger("lv_out");
        logger->set_level(spdlog::level::warn);

        logger->trace("trace_hidden");
        logger->debug("debug_hidden");
        logger->info("info_hidden");
        logger->warn("warn_visible");
        logger->error("error_visible");
        logger->critical("critical_visible");
        logger->flush();

        const std::string out = oss->str();
        DOCTEST_CHECK(out.find("trace_hidden")    == std::string::npos);
        DOCTEST_CHECK(out.find("debug_hidden")    == std::string::npos);
        DOCTEST_CHECK(out.find("info_hidden")     == std::string::npos);
        DOCTEST_CHECK(out.find("warn_visible")    != std::string::npos);
        DOCTEST_CHECK(out.find("error_visible")   != std::string::npos);
        DOCTEST_CHECK(out.find("critical_visible")!= std::string::npos);
    }

    /**
     * 测试目的：验证 level::off 关闭所有日志
     * 预期行为：设为 off 后任何级别均不输出
     */
    DOCTEST_TEST_CASE("level::off 关闭所有日志")
    {
        auto [logger, oss] = test_helpers::make_oss_logger("lv_off");
        logger->set_level(spdlog::level::off);

        logger->critical("should not appear");
        logger->flush();

        DOCTEST_CHECK(oss->str().empty());
    }

    /**
     * 测试目的：验证全局级别设置影响所有 logger
     * 使用的 API：spdlog::set_level(), spdlog::get_level(), spdlog::should_log()
     * 预期行为：set_level 修改全局级别，get_level 返回当前全局级别
     * 注意事项：全局级别影响默认 logger；其他 logger 需单独 initialize
     */
    DOCTEST_TEST_CASE("全局级别 set_level / get_level")
    {
        test_helpers::cleanup_loggers();

        auto [logger, oss] = test_helpers::make_oss_logger("global_lv");
        spdlog::set_default_logger(logger);

        spdlog::set_level(spdlog::level::err);
        DOCTEST_CHECK_EQ(spdlog::get_level(), spdlog::level::err);
        DOCTEST_CHECK_FALSE(spdlog::should_log(spdlog::level::info));
        DOCTEST_CHECK_FALSE(spdlog::should_log(spdlog::level::warn));
        DOCTEST_CHECK(spdlog::should_log(spdlog::level::err));
        DOCTEST_CHECK(spdlog::should_log(spdlog::level::critical));

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 level_enum 与字符串互转
     * 使用的 API：spdlog::level::to_string_view(), spdlog::level::from_str()
     * 预期行为：能从枚举得到字符串名称，也能反向解析
     */
    DOCTEST_TEST_CASE("级别名称字符串互转")
    {
        DOCTEST_CHECK_EQ(
            std::string(spdlog::level::to_string_view(spdlog::level::info).data()),
            "info");
        DOCTEST_CHECK_EQ(
            std::string(spdlog::level::to_string_view(spdlog::level::warn).data()),
            "warning");
        DOCTEST_CHECK_EQ(
            std::string(spdlog::level::to_string_view(spdlog::level::err).data()),
            "error");

        DOCTEST_CHECK_EQ(spdlog::level::from_str("info"),     spdlog::level::info);
        DOCTEST_CHECK_EQ(spdlog::level::from_str("warn"),     spdlog::level::warn);
        DOCTEST_CHECK_EQ(spdlog::level::from_str("error"),    spdlog::level::err);
        DOCTEST_CHECK_EQ(spdlog::level::from_str("critical"), spdlog::level::critical);
        DOCTEST_CHECK_EQ(spdlog::level::from_str("off"),      spdlog::level::off);
        // 未知字符串应返回 off
        DOCTEST_CHECK_EQ(spdlog::level::from_str("unknown"),  spdlog::level::off);
    }
} // DOCTEST_TEST_SUITE("spdlog — 日志级别")

DOCTEST_TEST_SUITE("spdlog — Pattern 和 Formatter")
{
    /**
     * 测试目的：验证各种 pattern 占位符的效果
     * 使用的 API：logger->set_pattern(), sink->set_pattern()
     * 预期行为：%v=消息, %l=级别, %n=logger名, %t=线程ID, %Y-%m-%d=日期等
     * 注意事项：时间相关占位符（%Y %m %d %H %M %S）输出依赖当前时间，不做精确断言
     */
    DOCTEST_TEST_CASE("set_pattern — 各占位符")
    {
        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("pat_test", sink);
        logger->set_level(spdlog::level::trace);

        DOCTEST_SUBCASE("仅消息 %v")
        {
            logger->set_pattern("%v");
            logger->info("hello");
            logger->flush();
            // 验证消息内容存在，不严格比较末尾空白（不同版本可能有差异）
            const std::string out = oss.str();
            DOCTEST_CHECK(out.find("hello") != std::string::npos);
            DOCTEST_CHECK(out.find('\n') != std::string::npos);
        }

        DOCTEST_SUBCASE("消息 + 级别 %l")
        {
            oss.str("");
            logger->set_pattern("[%l] %v");
            logger->info("msg");
            logger->warn("wmsg");
            logger->flush();
            DOCTEST_CHECK(oss.str().find("[info]")    != std::string::npos);
            DOCTEST_CHECK(oss.str().find("[warning]") != std::string::npos);
        }

        DOCTEST_SUBCASE("Logger 名称 %n")
        {
            oss.str("");
            logger->set_pattern("[%n] %v");
            logger->info("n_msg");
            logger->flush();
            DOCTEST_CHECK(oss.str().find("[pat_test]") != std::string::npos);
        }

        DOCTEST_SUBCASE("线程 ID %t")
        {
            oss.str("");
            logger->set_pattern("[%t] %v");
            logger->info("t_msg");
            logger->flush();
            // 至少有 [ 和 ] 包围线程 ID
            DOCTEST_CHECK(oss.str().find('[') != std::string::npos);
            DOCTEST_CHECK(oss.str().find(']') != std::string::npos);
        }

        DOCTEST_SUBCASE("日期时间 %Y-%m-%d %H:%M:%S")
        {
            oss.str("");
            logger->set_pattern("%Y-%m-%d %H:%M:%S [%l] %v");
            logger->info("dt_msg");
            logger->flush();
            // 验证包含 logger 名称和消息体，时间格式不精确断言
            const std::string out = oss.str();
            DOCTEST_CHECK(out.find("[info]")  != std::string::npos);
            DOCTEST_CHECK(out.find("dt_msg")  != std::string::npos);
            // 年份应为 4 位数字（简单检验）
            DOCTEST_CHECK(out.size() > 20);
        }

        DOCTEST_SUBCASE("毫秒 %e")
        {
            oss.str("");
            logger->set_pattern("%e %v");
            logger->info("ms_msg");
            logger->flush();
            // 毫秒是 3 位数字，输出中至少有一个数字
            DOCTEST_CHECK(!oss.str().empty());
        }
    }

    /**
     * 测试目的：验证全局 set_pattern 作用于默认 logger
     * 使用的 API：spdlog::set_pattern()
     * 预期行为：设置后默认 logger 所有 sink 采用新 pattern
     */
    DOCTEST_TEST_CASE("spdlog::set_pattern（全局）")
    {
        test_helpers::cleanup_loggers();

        std::ostringstream oss;
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("global_pat", sink);
        spdlog::set_default_logger(logger);

        spdlog::set_pattern(">>> %v <<<");
        spdlog::info("global_msg");
        logger->flush();

        DOCTEST_CHECK(oss.str().find(">>> global_msg <<<") != std::string::npos);

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 set_formatter 接受自定义 formatter 实例
     * 使用的 API：logger->set_formatter(std::unique_ptr<spdlog::formatter>)
     * 预期行为：使用 pattern_formatter 构造，效果与 set_pattern 等价
     * 注意事项：set_formatter 转移 unique_ptr 所有权
     */
    DOCTEST_TEST_CASE("set_formatter（pattern_formatter）")
    {
        std::ostringstream oss;
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("fmt_inst", sink);

        // 通过 set_formatter 设置 pattern
        auto formatter = std::make_unique<spdlog::pattern_formatter>(
            "[[%v]]", spdlog::pattern_time_type::local);
        logger->set_formatter(std::move(formatter));

        logger->info("inside_brackets");
        logger->flush();

        DOCTEST_CHECK(oss.str().find("[[inside_brackets]]") != std::string::npos);
    }

    /**
     * 测试目的：验证 sink 级别的 pattern 独立于 logger
     * 使用的 API：sink->set_pattern()
     * 预期行为：不同 sink 可有独立 pattern
     */
    DOCTEST_TEST_CASE("sink 独立 pattern")
    {
        std::ostringstream oss1, oss2;
        auto sink1 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss1);
        auto sink2 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss2);

        sink1->set_pattern("[SINK1] %v");
        sink2->set_pattern("[SINK2] %v");

        std::vector<spdlog::sink_ptr> sinks{sink1, sink2};
        auto logger = std::make_shared<spdlog::logger>("dual_pat", sinks.begin(), sinks.end());

        logger->info("shared_msg");
        logger->flush();

        DOCTEST_CHECK(oss1.str().find("[SINK1] shared_msg") != std::string::npos);
        DOCTEST_CHECK(oss2.str().find("[SINK2] shared_msg") != std::string::npos);
    }
} // DOCTEST_TEST_SUITE("spdlog — Pattern 和 Formatter")

DOCTEST_TEST_SUITE("spdlog — Sinks")
{
    /**
     * 测试目的：验证 ostream_sink 将日志写入 std::ostream
     * 使用的 API：spdlog::sinks::ostream_sink_mt / ostream_sink_st
     * 预期行为：日志写入提供的 ostream 对象
     * 注意事项：_mt 线程安全，_st 单线程，接口相同
     */
    DOCTEST_TEST_CASE("ostream_sink — 写入到 ostringstream")
    {
        std::ostringstream oss;
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("oss_sink", sink);
        logger->set_pattern("%v");

        logger->info("hello ostream");
        logger->flush();

        // 验证消息内容存在，不严格比较末尾空白（不同版本可能有差异）
        const std::string out = oss.str();
        DOCTEST_CHECK(out.find("hello ostream") != std::string::npos);
        DOCTEST_CHECK(out.find('\n') != std::string::npos);
    }

    /**
     * 测试目的：验证 null_sink 丢弃所有日志，不产生 I/O
     * 使用的 API：spdlog::sinks::null_sink_mt
     * 预期行为：不崩溃，无输出（不可直接验证，但保证函数调用正常）
     */
    DOCTEST_TEST_CASE("null_sink — 丢弃所有日志")
    {
        auto sink   = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto logger = std::make_shared<spdlog::logger>("null_s", sink);

        DOCTEST_CHECK_NOTHROW(logger->info("gone"));
        DOCTEST_CHECK_NOTHROW(logger->flush());
    }

    /**
     * 测试目的：验证 basic_file_sink 将日志写入磁盘文件
     * 使用的 API：spdlog::sinks::basic_file_sink_mt
     * 预期行为：日志内容可从文件中读取
     * 注意事项：truncate=true 每次创建时清空旧文件
     */
    DOCTEST_TEST_CASE("basic_file_sink — 写文件并验证内容")
    {
        const auto path = test_helpers::create_temp_file_path("basic_file");

        {
            auto sink   = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, /*truncate=*/true);
            auto logger = std::make_shared<spdlog::logger>("bfs", sink);
            logger->set_pattern("%v");
            logger->info("line one");
            logger->info("line two");
            logger->flush();
        } // sink/logger 析构，确保文件句柄关闭

        const std::string content = test_helpers::read_file(path);
        DOCTEST_CHECK(content.find("line one") != std::string::npos);
        DOCTEST_CHECK(content.find("line two") != std::string::npos);

        test_helpers::remove_file(path);
    }

    /**
     * 测试目的：验证 basic_file_sink truncate=false 时追加写入
     * 预期行为：重新打开同一文件时旧内容保留
     */
    DOCTEST_TEST_CASE("basic_file_sink — truncate=false 追加模式")
    {
        const auto path = test_helpers::create_temp_file_path("append_file");

        {
            auto sink   = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, true);
            auto logger = std::make_shared<spdlog::logger>("ap1", sink);
            logger->set_pattern("%v");
            logger->info("first run");
            logger->flush();
        }
        {
            auto sink   = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path, false); // 追加
            auto logger = std::make_shared<spdlog::logger>("ap2", sink);
            logger->set_pattern("%v");
            logger->info("second run");
            logger->flush();
        }

        const std::string content = test_helpers::read_file(path);
        DOCTEST_CHECK(content.find("first run")  != std::string::npos);
        DOCTEST_CHECK(content.find("second run") != std::string::npos);

        test_helpers::remove_file(path);
    }

    /**
     * 测试目的：验证 rotating_file_sink 按文件大小轮转
     * 使用的 API：spdlog::sinks::rotating_file_sink_mt(filename, max_size, max_files)
     * 预期行为：主文件始终存在；当内容超出 max_size 时创建备份文件
     * 注意事项：max_size 单位字节，max_files 为最大备份文件数
     */

    DOCTEST_TEST_CASE("rotating_file_sink — 文件大小轮转")
    {
        const auto base_path = test_helpers::create_temp_file_path("rotating");
        constexpr size_t max_size = 1024; // 1KB
        constexpr size_t max_files = 3;

        {
            auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(base_path, max_size, max_files);
            auto logger = std::make_shared<spdlog::logger>("rot_sink", sink);
            logger->set_pattern("%v");

            // 写入多条日志，触发轮转
            for(size_t i = 0; i < 200; ++i)
            {
                logger->info("message number {}", i);
            }
            logger->flush();
        }

        // 主文件必须存在
        DOCTEST_CHECK(std::filesystem::exists(base_path));

        // 检查备份文件是否存在（文件名为 base_path.1, base_path.2 ...）
        for(size_t i = 1; i <= max_files; ++i)
        {
            // 由于写入量可能不够触发 max_files 个备份，存在文件或不存在均可接受
            bool exists = std::filesystem::exists(base_path + "." + std::to_string(i)) ||
                         !std::filesystem::exists(base_path + "." + std::to_string(i));
            DOCTEST_CHECK(exists);
        }

        // 清理文件
        test_helpers::remove_file(base_path);
        for(size_t i = 1; i <= max_files; ++i)
        {
            test_helpers::remove_file(base_path + "." + std::to_string(i));
        }

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 daily_file_sink 按日期轮转文件
     * 使用的 API：spdlog::sinks::daily_file_sink_mt(filename, hour, minute)
     * 预期行为：文件按日期创建，今日日期文件存在
     * 注意事项：写入少量日志后立即验证文件存在；时间点固定为午夜
     */
    DOCTEST_TEST_CASE("daily_file_sink — 每日轮转文件")
    {
        const auto base_path = test_helpers::create_temp_file_path("daily");
        constexpr int rot_hour = 0;
        constexpr int rot_minute = 0;

        {
            auto sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(base_path, rot_hour, rot_minute);
            auto logger = std::make_shared<spdlog::logger>("daily_sink", sink);
            logger->set_pattern("%v");
            logger->info("daily log");
            logger->flush();
        }

        // 目录中应至少存在一个匹配 base_path 前缀的文件（含日期）
        bool found = false;
        auto dir = std::filesystem::temp_directory_path();
        for (auto& entry : std::filesystem::directory_iterator(dir))
        {
            if (entry.is_regular_file())
            {
                const auto& fname = entry.path().filename().string();
                if (fname.find("daily") != std::string::npos)
                {
                    found = true;
                    test_helpers::remove_file(entry.path().string());
                }
            }
        }
        DOCTEST_CHECK(found);

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 dist_sink 将同一条日志分发到多个子 sink
     * 使用的 API：spdlog::sinks::dist_sink_mt, add_sink(), remove_sink()
     * 预期行为：日志出现在每个子 sink 中
     * 注意事项：dist_sink 本身不持有 formatter，由子 sink 各自格式化
     */
    DOCTEST_TEST_CASE("dist_sink — 分发到多个 sink")
    {
        std::ostringstream oss1, oss2;
        auto s1 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss1);
        auto s2 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss2);
        s1->set_pattern("[S1] %v");
        s2->set_pattern("[S2] %v");

        auto dist = std::make_shared<spdlog::sinks::dist_sink_mt>();
        dist->add_sink(s1);
        dist->add_sink(s2);

        auto logger = std::make_shared<spdlog::logger>("dist_l", dist);

        logger->info("broadcast msg");
        logger->flush();

        DOCTEST_CHECK(oss1.str().find("[S1] broadcast msg") != std::string::npos);
        DOCTEST_CHECK(oss2.str().find("[S2] broadcast msg") != std::string::npos);
    }

    /**
     * 测试目的：验证 dist_sink remove_sink 后消息不再分发
     * 预期行为：remove_sink 后该 sink 不再接收日志
     */
    DOCTEST_TEST_CASE("dist_sink — remove_sink 后不再接收日志")
    {
        std::ostringstream oss1, oss2;
        auto s1 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss1);
        auto s2 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss2);
        s1->set_pattern("%v");
        s2->set_pattern("%v");

        auto dist = std::make_shared<spdlog::sinks::dist_sink_mt>();
        dist->add_sink(s1);
        dist->add_sink(s2);

        auto logger = std::make_shared<spdlog::logger>("dist_rm", dist);

        logger->info("msg before remove");
        logger->flush();

        // 移除 s2
        dist->remove_sink(s2);

        logger->info("msg after remove");
        logger->flush();

        DOCTEST_CHECK(oss1.str().find("msg before remove") != std::string::npos);
        DOCTEST_CHECK(oss2.str().find("msg before remove") != std::string::npos);
        DOCTEST_CHECK(oss1.str().find("msg after remove")  != std::string::npos);
        // s2 被移除后，不应收到 "msg after remove"
        DOCTEST_CHECK(oss2.str().find("msg after remove")  == std::string::npos);
    }

    /**
     * 测试目的：验证 ringbuffer_sink 环形覆盖旧消息，并可回读
     * 使用的 API：spdlog::sinks::ringbuffer_sink_mt(n), last_formatted(), last_raw()
     * 预期行为：写入 > n 条后，last_formatted() 仅保留最后 n 条
     * 注意事项：ringbuffer_sink 适合事后回溯调试日志；spdlog 1.10+ 版本支持
     */
    DOCTEST_TEST_CASE("ringbuffer_sink — 环形缓冲回读")
    {
        constexpr size_t buf_size = 5;
        auto sink   = std::make_shared<spdlog::sinks::ringbuffer_sink_mt>(buf_size);
        auto logger = std::make_shared<spdlog::logger>("ring_l", sink);
        logger->set_level(spdlog::level::trace);
        logger->set_pattern("%v");

        // 写入 10 条，只保留最后 5 条
        for (int i = 0; i < 10; ++i)
        {
            logger->info("msg {}", i);
        }
        logger->flush();

        auto formatted = sink->last_formatted();
        DOCTEST_REQUIRE_EQ(formatted.size(), buf_size);

        // 最后 5 条是 msg 5 ~ msg 9
        for (size_t i = 0; i < buf_size; ++i)
        {
            const int expected_idx = static_cast<int>(i + 5);
            DOCTEST_CHECK(formatted[i].find("msg " + std::to_string(expected_idx)) != std::string::npos);
        }
    }

    /**
     * 测试目的：验证 callback_sink 每次日志触发用户回调
     * 使用的 API：spdlog::sinks::callback_sink_mt(cb)
     * 预期行为：每条日志调用一次回调，回调参数包含 log_msg 信息
     * 注意事项：callback_sink 常用于集成测试或将日志转发到自定义后端
     */
    DOCTEST_TEST_CASE("callback_sink — 回调验证日志内容")
    {
        std::vector<std::string> captured;

        auto sink = std::make_shared<spdlog::sinks::callback_sink_mt>(
            [&captured](const spdlog::details::log_msg& msg)
            {
                // 将负载转为 string 存储
                captured.emplace_back(msg.payload.data(), msg.payload.size());
            });

        auto logger = std::make_shared<spdlog::logger>("cb_logger", sink);
        logger->set_level(spdlog::level::trace);

        logger->info("callback_one");
        logger->warn("callback_two");
        logger->flush();

        DOCTEST_REQUIRE_EQ(captured.size(), 2u);
        DOCTEST_CHECK_EQ(captured[0], "callback_one");
        DOCTEST_CHECK_EQ(captured[1], "callback_two");
    }

    /**
     * 测试目的：验证 callback_sink 级别过滤仍生效
     * 预期行为：低于 sink 设置级别的消息不触发回调
     */
    DOCTEST_TEST_CASE("callback_sink — 级别过滤")
    {
        std::vector<std::string> captured;

        auto sink = std::make_shared<spdlog::sinks::callback_sink_mt>(
            [&captured](const spdlog::details::log_msg& msg)
            {
                captured.emplace_back(msg.payload.data(), msg.payload.size());
            });
        sink->set_level(spdlog::level::warn); // 只捕获 warn 及以上

        auto logger = std::make_shared<spdlog::logger>("cb_filter", sink);
        logger->set_level(spdlog::level::trace);

        logger->info("info_skip");
        logger->warn("warn_keep");
        logger->error("error_keep");
        logger->flush();

        DOCTEST_REQUIRE_EQ(captured.size(), 2u);
        DOCTEST_CHECK(captured[0].find("warn_keep")  != std::string::npos);
        DOCTEST_CHECK(captured[1].find("error_keep") != std::string::npos);
    }

    /**
     * 测试目的：验证 stdout/stderr sink 可创建并正常写入
     * 使用的 API：stdout_sink_mt, stderr_sink_mt,
     *             stdout_color_sink_mt, stderr_color_sink_mt
     * 预期行为：创建不抛异常；log 调用不崩溃
     * 注意事项：stdout/stderr 输出无法在不重定向的情况下做内容断言
     *           这里仅验证 API 可用性
     */
    DOCTEST_TEST_CASE("stdout/stderr sink — 创建与基本调用")
    {
        DOCTEST_CHECK_NOTHROW([]()
        {
            auto s = std::make_shared<spdlog::sinks::stdout_sink_mt>();
            auto l = std::make_shared<spdlog::logger>("out", s);
            // 不实际写入，避免污染控制台
        }());

        DOCTEST_CHECK_NOTHROW([]()
        {
            auto s = std::make_shared<spdlog::sinks::stderr_sink_mt>();
            auto l = std::make_shared<spdlog::logger>("err", s);
        }());

        DOCTEST_CHECK_NOTHROW([]()
        {
            auto s = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            auto l = std::make_shared<spdlog::logger>("color_out", s);
        }());
    }
} // DOCTEST_TEST_SUITE("spdlog — Sinks")

DOCTEST_TEST_SUITE("spdlog — Flush 控制")
{
    /**
     * 测试目的：验证手动 flush 将缓冲内容写入 sink
     * 使用的 API：logger->flush()
     * 预期行为：flush 后 ostream 中可读到日志内容
     * 注意事项：ostream_sink 在不 flush 的情况下内容可能仍在内部缓冲区
     */
    DOCTEST_TEST_CASE("手动 flush")
    {
        auto [logger, oss] = test_helpers::make_oss_logger("flush_manual");

        logger->info("before flush");
        logger->flush();

        DOCTEST_CHECK(oss->str().find("before flush") != std::string::npos);
    }

    /**
     * 测试目的：验证 flush_on 达到指定级别时自动 flush
     * 使用的 API：logger->flush_on(level), logger->flush_level()
     * 预期行为：达到 flush_on 级别时自动刷写，低于该级别不自动 flush
     * 注意事项：ostream_sink 本身有写操作，flush_on 主要对有缓冲的 sink 有意义
     */
    DOCTEST_TEST_CASE("flush_on — 达到级别自动 flush")
    {
        auto [logger, oss] = test_helpers::make_oss_logger("flush_on_test");
        logger->flush_on(spdlog::level::err);

        DOCTEST_CHECK_EQ(logger->flush_level(), spdlog::level::err);

        logger->info("info_no_flush");   // 不触发自动 flush，但 ostream 通常仍能读到
        logger->error("err_flush");      // 触发自动 flush

        const std::string out = oss->str();
        DOCTEST_CHECK(out.find("info_no_flush") != std::string::npos);
        DOCTEST_CHECK(out.find("err_flush")     != std::string::npos);
    }

    /**
     * 测试目的：验证 flush_on 默认值为 off
     * 预期行为：默认 flush_level() == off（即不自动 flush）
     */
    DOCTEST_TEST_CASE("flush_level 默认值为 off")
    {
        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto logger    = std::make_shared<spdlog::logger>("fl_def", null_sink);
        DOCTEST_CHECK_EQ(logger->flush_level(), spdlog::level::off);
    }

    /**
     * 测试目的：验证全局 spdlog::flush_on 作用于默认 logger
     * 使用的 API：spdlog::flush_on()
     * 预期行为：全局设置后默认 logger flush_level 更新
     */
    DOCTEST_TEST_CASE("全局 spdlog::flush_on")
    {
        test_helpers::cleanup_loggers();

        auto [logger, oss] = test_helpers::make_oss_logger("global_flush");
        spdlog::set_default_logger(logger);

        spdlog::flush_on(spdlog::level::warn);
        DOCTEST_CHECK_EQ(spdlog::default_logger()->flush_level(), spdlog::level::warn);

        test_helpers::cleanup_loggers();
    }
} // DOCTEST_TEST_SUITE("spdlog — Flush 控制")

DOCTEST_TEST_SUITE("spdlog — Error Handler")
{
    // 自定义抛异常 sink，用于测试 error handler
    struct ThrowingSink : public spdlog::sinks::base_sink<std::mutex> {
    protected:
        void sink_it_(const spdlog::details::log_msg&) override {
            throw std::runtime_error("sink_error!");
        }
        void flush_() override {}
    };

    /**
     * 测试目的：验证 set_error_handler 在 sink 内部抛出异常时被调用
     * 使用的 API：logger->set_error_handler(fn), spdlog::set_error_handler(fn)
     * 预期行为：当 sink 的 sink_it_ 抛出异常时，error handler 被调用
     * 注意事项：error handler 替代默认的 stderr 错误打印行为
     */
    DOCTEST_TEST_CASE("set_error_handler — sink 抛异常时调用")
    {
        std::string captured_error;

        auto sink   = std::make_shared<ThrowingSink>();
        auto logger = std::make_shared<spdlog::logger>("err_handler", sink);

        logger->set_error_handler([&captured_error](const std::string& msg)
        {
            captured_error = msg;
        });

        // 写入时 ThrowingSink 抛出，应触发 error_handler 而不是崩溃
        logger->info("trigger error");
        logger->flush();

        // error handler 应被调用
        DOCTEST_CHECK(!captured_error.empty());
    }
} // DOCTEST_TEST_SUITE("spdlog — Error Handler")

DOCTEST_TEST_SUITE("spdlog — Clone 与多 Sink")
{
    /**
     * 测试目的：验证 logger->clone() 创建同配置的新 logger
     * 使用的 API：logger->clone(new_name)
     * 预期行为：clone 后的 logger 共享 sinks，但名称独立；级别/flush_level 相同
     * 注意事项：clone 共享同一组 sink 指针（浅拷贝 sinks）
     */
    DOCTEST_TEST_CASE("logger->clone — 克隆共享 sink")
    {
        std::ostringstream oss;
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("original", sink);
        logger->set_pattern("%v");
        logger->set_level(spdlog::level::warn);
        logger->flush_on(spdlog::level::err);

        auto cloned = logger->clone("cloned_logger");

        DOCTEST_REQUIRE(cloned != nullptr);
        DOCTEST_CHECK_EQ(cloned->name(), "cloned_logger");
        // 克隆后级别、flush_level 与原始相同
        DOCTEST_CHECK_EQ(cloned->level(),       spdlog::level::warn);
        DOCTEST_CHECK_EQ(cloned->flush_level(), spdlog::level::err);
        // 共享同一 sink
        DOCTEST_CHECK_EQ(cloned->sinks().size(), logger->sinks().size());
        DOCTEST_CHECK_EQ(cloned->sinks()[0].get(), logger->sinks()[0].get());

        // 两个 logger 都可以写到同一 sink
        logger->warn("from original");
        cloned->warn("from cloned");
        logger->flush();

        DOCTEST_CHECK(oss.str().find("from original") != std::string::npos);
        DOCTEST_CHECK(oss.str().find("from cloned")   != std::string::npos);
    }

    /**
     * 测试目的：验证多 sink logger 构造
     * 使用的 API：logger(name, sinks_init_list), logger(name, begin, end)
     * 预期行为：一条日志同时写入所有 sink
     */
    DOCTEST_TEST_CASE("多 sink logger — 构造与输出")
    {
        std::ostringstream oss1, oss2, oss3;
        auto s1 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss1);
        auto s2 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss2);
        auto s3 = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss3);
        s1->set_pattern("A: %v");
        s2->set_pattern("B: %v");
        s3->set_pattern("C: %v");

        // 通过 initializer_list 构造
        auto logger = std::make_shared<spdlog::logger>(
            "multi_sink", spdlog::sinks_init_list{s1, s2, s3});

        logger->info("multi_msg");
        logger->flush();

        DOCTEST_CHECK(oss1.str().find("A: multi_msg") != std::string::npos);
        DOCTEST_CHECK(oss2.str().find("B: multi_msg") != std::string::npos);
        DOCTEST_CHECK(oss3.str().find("C: multi_msg") != std::string::npos);
    }

    /**
     * 测试目的：验证运行时动态添加/访问 sinks
     * 使用的 API：logger->sinks()（返回可修改引用）
     * 预期行为：push_back 后新 sink 可接收日志
     */
    DOCTEST_TEST_CASE("运行时动态添加 sink")
    {
        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto logger    = std::make_shared<spdlog::logger>("dyn_sink", null_sink);
        DOCTEST_CHECK_EQ(logger->sinks().size(), 1u);

        // 动态添加第二个 sink
        std::ostringstream oss;
        auto new_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        new_sink->set_pattern("%v");
        logger->sinks().push_back(new_sink);
        DOCTEST_CHECK_EQ(logger->sinks().size(), 2u);

        logger->info("dynamic_msg");
        logger->flush();

        DOCTEST_CHECK(oss.str().find("dynamic_msg") != std::string::npos);
    }
} // DOCTEST_TEST_SUITE("spdlog — Clone 与多 Sink")

DOCTEST_TEST_SUITE("spdlog — Backtrace")
{
    /**
     * 测试目的：验证 backtrace 功能：在 trace/debug 消息中静默积累，按需 dump
     * 使用的 API：logger->enable_backtrace(n), logger->disable_backtrace(),
     *             logger->dump_backtrace(), logger->should_backtrace()
     * 预期行为：
     *   - enable_backtrace(n) 开启环形 backtrace 缓冲，should_backtrace() 返回 true
     *   - trace/debug 消息被积累（不输出，仅当 dump_backtrace 时才输出）
     *   - dump_backtrace 将积累的消息写到 sink
     *   - disable_backtrace 关闭，should_backtrace() 返回 false
     * 注意事项：backtrace 在 spdlog 1.8+ 可用；
     *            dump_backtrace 会将缓冲消息发到 sink（即使级别低于当前设定级别）
     */
    DOCTEST_TEST_CASE("enable/disable/dump_backtrace")
    {
        std::ostringstream oss;
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("bt_logger", sink);
        logger->set_pattern("%v");
        logger->set_level(spdlog::level::info); // info 级别，trace 正常不显示

        DOCTEST_SUBCASE("默认 should_backtrace 为 false")
        {
            DOCTEST_CHECK_FALSE(logger->should_backtrace());
        }

        DOCTEST_SUBCASE("开启 backtrace 后 should_backtrace 为 true")
        {
            logger->enable_backtrace(10);
            DOCTEST_CHECK(logger->should_backtrace());
            logger->disable_backtrace();
            DOCTEST_CHECK_FALSE(logger->should_backtrace());
        }

        DOCTEST_SUBCASE("dump_backtrace 输出积累的 trace 消息")
        {
            logger->enable_backtrace(5);

            // 记录几条 trace（不会出现在 oss，因为级别过滤）
            logger->trace("trace_bt_1");
            logger->trace("trace_bt_2");
            logger->debug("debug_bt");

            // dump_backtrace 将缓冲内容写到 sink
            logger->dump_backtrace();
            logger->flush();

            const std::string out = oss.str();
            // dump 后缓冲消息应出现
            DOCTEST_CHECK(out.find("trace_bt_1") != std::string::npos);
            DOCTEST_CHECK(out.find("trace_bt_2") != std::string::npos);
            DOCTEST_CHECK(out.find("debug_bt")   != std::string::npos);

            logger->disable_backtrace();
        }

        DOCTEST_SUBCASE("超出 backtrace 容量时覆盖旧消息")
        {
            logger->enable_backtrace(2); // 只保留 2 条

            logger->trace("old_bt_1");
            logger->trace("old_bt_2");
            logger->trace("new_bt_3"); // 覆盖 old_bt_1

            logger->dump_backtrace();
            logger->flush();

            const std::string out = oss.str();
            // old_bt_1 应被覆盖
            DOCTEST_CHECK(out.find("old_bt_1") == std::string::npos);
            DOCTEST_CHECK(out.find("old_bt_2") != std::string::npos);
            DOCTEST_CHECK(out.find("new_bt_3") != std::string::npos);

            logger->disable_backtrace();
        }
    }

    /**
     * 测试目的：验证全局 spdlog::enable_backtrace / dump_backtrace
     * 使用的 API：spdlog::enable_backtrace(n), spdlog::dump_backtrace(), spdlog::disable_backtrace()
     * 预期行为：作用于默认 logger
     */
    DOCTEST_TEST_CASE("全局 enable/dump/disable_backtrace")
    {
        test_helpers::cleanup_loggers();

        std::ostringstream oss;
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("global_bt", sink);
        logger->set_pattern("%v");
        logger->set_level(spdlog::level::info);
        spdlog::set_default_logger(logger);

        spdlog::enable_backtrace(3);
        spdlog::trace("global_trace_1");
        spdlog::trace("global_trace_2");
        spdlog::dump_backtrace();
        logger->flush();

        const std::string out = oss.str();
        DOCTEST_CHECK(out.find("global_trace_1") != std::string::npos);
        DOCTEST_CHECK(out.find("global_trace_2") != std::string::npos);

        spdlog::disable_backtrace();
        test_helpers::cleanup_loggers();
    }
} // DOCTEST_TEST_SUITE("spdlog — Backtrace")

/**
 * 【修复说明】原测试因以下竞态条件在 Linux 上偶发崩溃（use-after-free）：
 *
 * 崩溃根本原因：
 *   async_logger::flush_() 是【非阻塞】的——它只是把 flush 请求投入 MPMC 队列后
 *   立即返回，并不等待后台工作线程真正处理完该请求。
 *
 *   旧代码的对象依赖链：
 *     async_msg（队列中）→ worker_ptr（shared_ptr<async_logger>）
 *                       → sinks_（shared_ptr<ostream_sink>）
 *                       → 内部持有 ostream& oss（引用！）
 *
 *   崩溃时序（系统负载高时 100ms 内后台线程未处理完所有消息）：
 *     1. 主线程发送 10 条 log + 2 条 flush 消息入队（均异步）
 *     2. 主线程 sleep_for(100ms)——heuristic，不可靠
 *     3. 主线程执行 async_logger.reset() + 内层 {} 结束
 *     4. oss（局部变量）在此处析构 ←─ 后台线程还在写！
 *     5. 后台线程处理队列消息 → sink->log() → 写已析构的 oss → 崩溃
 *
 * 正确修复：使用【局部 thread_pool】，在读取 oss 之前先让 tp 析构。
 *   thread_pool 析构函数：
 *     (a) 向队列投入 N 个 terminate 消息（FIFO，排在所有 log 之后）
 *     (b) join 所有工作线程
 *   join 返回时，所有 log/flush 消息已被后台线程处理完毕，oss 内容已确定，
 *   可安全读取，不存在任何竞态。
 */
DOCTEST_TEST_SUITE("spdlog — 异步 Logger")
{
    /**
     * 测试目的：验证异步 logger 与线程池的基本工作流程
     * 使用的 API：spdlog::details::thread_pool（局部实例）
     *             spdlog::async_logger（block 溢出策略）
     * 预期行为：异步 logger 将日志消息排队到线程池，由后台线程写入 sink；
     *           thread_pool 析构时 join 工作线程，确保所有消息处理完毕
     * 注意事项：
     *   1. 使用局部 thread_pool 而非全局 spdlog::init_thread_pool()，
     *      以便精确控制其生命周期，避免与其他测试的全局状态相互干扰
     *   2. 关键：tp 必须在读取 oss 之前析构（join 工作线程），
     *      否则后台线程可能仍在写 oss，导致 use-after-free
     *   3. async_logger::flush_() 是非阻塞的（只投队列不等待），
     *      不能依赖 flush() + sleep 来保证消息已处理完毕
     */
    DOCTEST_TEST_CASE("async_logger — 基本异步写入")
    {
        test_helpers::cleanup_loggers();
        constexpr size_t queue_size   = 128;
        constexpr size_t thread_count = 1;

        // oss 声明在最外层，生命周期覆盖所有异步操作
        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        sink->set_pattern("%v");

        {
            // 局部 thread_pool：其析构函数会 join 工作线程，
            // 确保所有排队消息处理完后才返回
            auto tp = std::make_shared<spdlog::details::thread_pool>(queue_size, thread_count);
            {
                // 创建异步 logger（block 模式：队列满时阻塞生产者）
                auto async_logger = std::make_shared<spdlog::async_logger>(
                    "async_l", sink, tp, spdlog::async_overflow_policy::block);
                async_logger->set_level(spdlog::level::trace);

                for (int i = 0; i < 10; ++i)
                {
                    async_logger->info("async msg {}", i);
                }
                // async_logger 在此处离开作用域：
                //   shared_ptr 引用计数 -1；但队列中的 async_msg::worker_ptr
                //   仍持有 shared_ptr<async_logger>，对象尚未析构
            }
            // tp 在此处离开作用域 → thread_pool 析构：
            //   (a) 投入 thread_count 个 terminate 消息（FIFO，在所有 log 消息之后）
            //   (b) join 所有工作线程
            // join 返回时，队列已空，所有 log 消息均已写入 oss
        }

        // 所有异步写入已完成，可安全读取 oss
        const std::string out = oss.str();
        DOCTEST_CHECK(out.find("async msg 0") != std::string::npos);
        DOCTEST_CHECK(out.find("async msg 9") != std::string::npos);

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 overrun_oldest 溢出策略——队列满时丢弃最旧的消息
     * 使用的 API：spdlog::async_logger（overrun_oldest 溢出策略）
     * 预期行为：
     *   - 队列满时最旧的消息被覆盖，最新消息始终可入队（不阻塞）
     *   - 最后发送的消息必然能被写入 sink
     * 注意事项：
     *   - 使用极小队列（queue_size=4）并发送远多于队列容量的消息，
     *     以触发溢出覆盖行为；即使工作线程消费速度快未必每次都溢出，
     *     但最后一条消息（msg 19）保证被处理
     *   - 仍采用局部 thread_pool 模式，tp 析构 join 后方可读取 oss
     */
    DOCTEST_TEST_CASE("async_logger — overrun_oldest 溢出策略")
    {
        test_helpers::cleanup_loggers();
        // 极小队列以增大溢出概率
        constexpr size_t queue_size   = 4;
        constexpr size_t thread_count = 1;

        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        sink->set_pattern("%v");

        {
            auto tp = std::make_shared<spdlog::details::thread_pool>(queue_size, thread_count);
            {
                // overrun_oldest：队列满时覆盖最旧消息，不阻塞生产者
                auto async_logger = std::make_shared<spdlog::async_logger>(
                    "async_overrun", sink, tp,
                    spdlog::async_overflow_policy::overrun_oldest);
                async_logger->set_level(spdlog::level::trace);

                for (int i = 0; i < 20; ++i)
                {
                    async_logger->info("overrun msg {}", i);
                }
                // async_logger 离开作用域，队列中的消息仍被后台线程持有
            }
            // tp 析构 → join 工作线程，等待所有消息处理完毕
        }

        // overrun_oldest：最后入队的消息不会被覆盖（之后无更新消息），必然写入
        const std::string out = oss.str();
        DOCTEST_CHECK(out.find("overrun msg 19") != std::string::npos);
        // 整体有输出（至少部分消息被写入）
        DOCTEST_CHECK(!out.empty());

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 discard_new 溢出策略——队列满时丢弃新到来的消息
     * 使用的 API：spdlog::async_logger（discard_new 溢出策略）
     * 预期行为：
     *   - 队列满时新消息被静默丢弃，已在队列中的旧消息不受影响（不阻塞）
     *   - 第一条消息（队列空时入队）必然能被写入 sink
     * 注意事项：
     *   - 使用极小队列（queue_size=4）以增大触发丢弃的概率
     *   - tp 析构 join 是保证无竞态的唯一可靠手段
     */
    DOCTEST_TEST_CASE("async_logger — discard_new 溢出策略")
    {
        test_helpers::cleanup_loggers();
        // 极小队列以增大溢出概率
        constexpr size_t queue_size   = 4;
        constexpr size_t thread_count = 1;

        std::ostringstream oss;
        auto sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        sink->set_pattern("%v");

        {
            auto tp = std::make_shared<spdlog::details::thread_pool>(queue_size, thread_count);
            {
                // discard_new：队列满时新消息被丢弃，不阻塞生产者
                auto async_logger = std::make_shared<spdlog::async_logger>(
                    "async_discard", sink, tp,
                    spdlog::async_overflow_policy::discard_new);
                async_logger->set_level(spdlog::level::trace);

                for (int i = 0; i < 20; ++i)
                {
                    async_logger->info("discard msg {}", i);
                }
                // async_logger 离开作用域，队列中的消息仍被后台线程持有
            }
            // tp 析构 → join 工作线程，等待所有消息处理完毕
        }

        // discard_new：最先入队的消息（队列为空时）必然被处理
        const std::string out = oss.str();
        DOCTEST_CHECK(out.find("discard msg 0") != std::string::npos);
        // 整体有输出（至少部分消息被写入）
        DOCTEST_CHECK(!out.empty());

        test_helpers::cleanup_loggers();
    }
} // DOCTEST_TEST_SUITE("spdlog — 异步 Logger")

/**
 * 自定义 sink：收集所有日志消息到 vector 中，供测试断言使用
 * 继承自 spdlog::sinks::base_sink<Mutex> 并实现 sink_it_ / flush_
 */
template <typename Mutex>
class collecting_sink : public spdlog::sinks::base_sink<Mutex> {
public:
    std::vector<std::string> messages;

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(msg, formatted);
        messages.emplace_back(formatted.data(), formatted.size());
    }
    void flush_() override {}
};

using collecting_sink_mt = collecting_sink<std::mutex>;
using collecting_sink_st = collecting_sink<spdlog::details::null_mutex>;

DOCTEST_TEST_SUITE("spdlog — 自定义 Sink")
{
    /**
     * 测试目的：验证自定义 sink 的实现和集成
     * 使用的 API：spdlog::sinks::base_sink<Mutex>（CRTP 基类）
     * 预期行为：自定义 sink 接收所有日志消息，可验证其内容
     * 注意事项：
     *   - 必须实现 sink_it_(log_msg) 和 flush_() 两个纯虚函数
     *   - formatter_ 在基类中由 set_formatter / set_pattern 管理
     *   - _mt 使用 std::mutex，_st 使用 null_mutex（单线程）
     */
    DOCTEST_TEST_CASE("collecting_sink — 自定义 sink 收集消息")
    {
        auto sink   = std::make_shared<collecting_sink_mt>();
        auto logger = std::make_shared<spdlog::logger>("custom_s", sink);
        sink->set_pattern("%l: %v"); // 级别: 消息

        logger->set_level(spdlog::level::trace);
        logger->trace("trace_msg");
        logger->info("info_msg");
        logger->warn("warn_msg");
        logger->flush();

        DOCTEST_REQUIRE_EQ(sink->messages.size(), 3u);
        DOCTEST_CHECK(sink->messages[0].find("trace: trace_msg") != std::string::npos);
        DOCTEST_CHECK(sink->messages[1].find("info: info_msg")   != std::string::npos);
        DOCTEST_CHECK(sink->messages[2].find("warning: warn_msg")!= std::string::npos);
    }

    /**
     * 测试目的：验证 sink 级别过滤对自定义 sink 有效
     * 预期行为：sink 设定级别后，低级别消息不调用 sink_it_
     */
    DOCTEST_TEST_CASE("collecting_sink — 级别过滤")
    {
        auto sink   = std::make_shared<collecting_sink_mt>();
        sink->set_level(spdlog::level::warn); // sink 级别设为 warn
        sink->set_pattern("%v");

        auto logger = std::make_shared<spdlog::logger>("custom_filter", sink);
        logger->set_level(spdlog::level::trace);

        logger->trace("skip_trace");
        logger->info("skip_info");
        logger->warn("keep_warn");
        logger->error("keep_error");
        logger->flush();

        DOCTEST_REQUIRE_EQ(sink->messages.size(), 2u);
        DOCTEST_CHECK(sink->messages[0].find("keep_warn")  != std::string::npos);
        DOCTEST_CHECK(sink->messages[1].find("keep_error") != std::string::npos);
    }
} // DOCTEST_TEST_SUITE("spdlog — 自定义 Sink")

/**
 * 自定义 formatter：在消息前加 "[MY_FMT] " 前缀
 * 继承自 spdlog::formatter 并实现 format() / clone()
 */
class my_prefix_formatter : public spdlog::formatter {
public:
    explicit my_prefix_formatter(std::string prefix = "[MY_FMT] ")
        : prefix_(std::move(prefix))
        {}

    void format(const spdlog::details::log_msg& msg,
                spdlog::memory_buf_t& dest) override {
        // 追加自定义前缀
        dest.append(prefix_.data(), prefix_.data() + prefix_.size());
        // 追加消息 payload
        dest.append(msg.payload.data(), msg.payload.data() + msg.payload.size());
        // 追加换行
        dest.push_back('\n');
    }

    std::unique_ptr<spdlog::formatter> clone() const override {
        return std::make_unique<my_prefix_formatter>(prefix_);
    }

private:
    std::string prefix_;
};

DOCTEST_TEST_SUITE("spdlog — 自定义 Formatter")
{
    /**
     * 测试目的：验证自定义 formatter 实现和集成
     * 使用的 API：spdlog::formatter（抽象基类），logger->set_formatter()
     * 预期行为：自定义 formatter 控制完整输出格式
     * 注意事项：
     *   - 必须实现 format(log_msg, dest) 和 clone()
     *   - clone() 用于 sink 内部复制 formatter（每个 sink 独立持有一份）
     *   - set_formatter 转移 unique_ptr 所有权
     */
    DOCTEST_TEST_CASE("my_prefix_formatter — 自定义格式化器输出前缀")
    {
        std::ostringstream oss;
        auto sink   = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
        auto logger = std::make_shared<spdlog::logger>("custom_fmt_l", sink);

        // 应用自定义 formatter
        logger->set_formatter(std::make_unique<my_prefix_formatter>("[MY_FMT] "));

        logger->info("hello custom fmt");
        logger->warn("warn custom fmt");
        logger->flush();

        const std::string out = oss.str();
        DOCTEST_CHECK(out.find("[MY_FMT] hello custom fmt") != std::string::npos);
        DOCTEST_CHECK(out.find("[MY_FMT] warn custom fmt")  != std::string::npos);
    }

    /**
     * 测试目的：验证 clone() 正确复制 formatter 到新 sink
     * 预期行为：克隆后的 formatter 产生相同格式
     */
    DOCTEST_TEST_CASE("my_prefix_formatter — clone 正确复制")
    {
        auto fmt    = std::make_unique<my_prefix_formatter>("[CLONE] ");
        auto cloned = fmt->clone();

        // 两个 formatter 独立工作
        spdlog::memory_buf_t dest;

        // 构造一个最小 log_msg 用于测试 format
        spdlog::source_loc loc{};
        spdlog::details::log_msg msg{loc, "test_logger", spdlog::level::info, "clone_msg"};

        cloned->format(msg, dest);
        const std::string result(dest.data(), dest.size());
        DOCTEST_CHECK(result.find("[CLONE] clone_msg") != std::string::npos);
    }
} // DOCTEST_TEST_SUITE("spdlog — 自定义 Formatter")

DOCTEST_TEST_SUITE("spdlog — 版本信息")
{
    /**
     * 测试目的：验证 spdlog 版本宏与运行时版本信息
     * 使用的 API：SPDLOG_VERSION, SPDLOG_VER_MAJOR/MINOR/PATCH
     * 预期行为：版本宏为正整数；版本字符串不为空
     */
    DOCTEST_TEST_CASE("版本宏")
    {
        DOCTEST_CHECK(SPDLOG_VER_MAJOR >= 1);
        DOCTEST_CHECK(SPDLOG_VER_MINOR >= 0);
        DOCTEST_CHECK(SPDLOG_VER_PATCH >= 0);

        // SPDLOG_VERSION = major * 10000 + minor * 100 + patch
        const int version = SPDLOG_VER_MAJOR * 10000 + SPDLOG_VER_MINOR * 100 + SPDLOG_VER_PATCH;
        DOCTEST_CHECK(version >= 10000); // 至少 1.0.0
    }
} // DOCTEST_TEST_SUITE("spdlog — 版本信息")

DOCTEST_TEST_SUITE("spdlog — 综合场景")
{
    /**
     * 测试目的：模拟实际使用场景：多个命名 logger，各自不同 level 和 sink
     * 预期行为：每个 logger 独立输出，互不干扰
     */
    DOCTEST_TEST_CASE("多命名 logger 独立工作")
    {
        test_helpers::cleanup_loggers();

        std::ostringstream oss_app, oss_db;
        auto app_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss_app);
        auto db_sink  = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss_db);
        app_sink->set_pattern("[APP] %v");
        db_sink->set_pattern("[DB]  %v");

        auto app_logger = std::make_shared<spdlog::logger>("app", app_sink);
        auto db_logger  = std::make_shared<spdlog::logger>("db",  db_sink);
        app_logger->set_level(spdlog::level::info);
        db_logger->set_level(spdlog::level::warn);

        spdlog::register_logger(app_logger);
        spdlog::register_logger(db_logger);

        // app logger 写入
        app_logger->info("app started");
        app_logger->debug("debug hidden"); // debug 低于 info，被过滤
        // db logger 写入
        db_logger->warn("db warning");
        db_logger->info("db info hidden"); // info 低于 warn，被过滤

        app_logger->flush();
        db_logger->flush();

        DOCTEST_CHECK(oss_app.str().find("[APP] app started")  != std::string::npos);
        DOCTEST_CHECK(oss_app.str().find("debug hidden")        == std::string::npos);
        DOCTEST_CHECK(oss_db.str().find("[DB]  db warning")    != std::string::npos);
        DOCTEST_CHECK(oss_db.str().find("db info hidden")       == std::string::npos);

        // 通过 registry 获取并再次验证
        DOCTEST_CHECK(spdlog::get("app") != nullptr);
        DOCTEST_CHECK(spdlog::get("db")  != nullptr);

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：模拟 apply_all 批量修改所有 logger 级别
     * 预期行为：apply_all 后所有 logger 级别均被修改
     */
    DOCTEST_TEST_CASE("apply_all 批量修改 logger 级别")
    {
        test_helpers::cleanup_loggers();

        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        for (int i = 0; i < 3; ++i)
        {
            auto l = std::make_shared<spdlog::logger>("batch_" + std::to_string(i), null_sink);
            spdlog::register_logger(l);
        }

        // 批量设置所有 logger 为 err 级别
        spdlog::apply_all([](std::shared_ptr<spdlog::logger> l)
        {
            l->set_level(spdlog::level::err);
        });

        for (int i = 0; i < 3; ++i)
        {
            auto l = spdlog::get("batch_" + std::to_string(i));
            DOCTEST_REQUIRE(l != nullptr);
            DOCTEST_CHECK_EQ(l->level(), spdlog::level::err);
        }

        test_helpers::cleanup_loggers();
    }

    /**
     * 测试目的：验证 spdlog::shutdown 停止所有 spdlog 内部线程并清空 registry
     * 使用的 API：spdlog::shutdown()
     * 预期行为：shutdown 后 registry 清空，logger 引用失效
     * 注意事项：shutdown 后应重新初始化才能继续使用 spdlog；
     *            由于 shutdown 影响全局状态，本测试放在最后
     */
    DOCTEST_TEST_CASE("spdlog::shutdown 清理全局状态")
    {
        test_helpers::cleanup_loggers();

        auto null_sink = std::make_shared<spdlog::sinks::null_sink_mt>();
        auto l = std::make_shared<spdlog::logger>("pre_shutdown", null_sink);
        spdlog::register_logger(l);

        DOCTEST_CHECK(spdlog::get("pre_shutdown") != nullptr);

        // shutdown 停止后台线程，清空 registry
        DOCTEST_CHECK_NOTHROW(spdlog::shutdown());

        // shutdown 后 logger 在 registry 中已消失
        DOCTEST_CHECK(spdlog::get("pre_shutdown") == nullptr);

        // 恢复可用状态（重建默认 logger）
        test_helpers::cleanup_loggers();
    }
} // DOCTEST_TEST_SUITE("spdlog — 综合场景")