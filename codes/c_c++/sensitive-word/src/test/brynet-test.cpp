#include "doctest.h"
#include "core/common.h"
#include "core/brynet.h"
#include <string_view>
#include <optional>
#include <array>
#include <memory>
#include <chrono>
#include <thread>
#include <functional>
#include <cstring>
#include <mutex>

// ====================================================================
//  辅助工具
// ====================================================================

// 获取随机可用端口号，避免测试端口冲突
inline auto get_random_port() -> uint16_t
{
    const uint16_t base = 30000;
    const uint16_t rnum = static_cast<uint16_t>(core::random_uint32() % 20000);
    return base + rnum;
}

// RAII 方式管理 buffer_s* 的 unique_ptr 删除器
struct BufferDeleter
{
    void operator()(brynet::base::buffer_s* p) const
    {
        brynet::base::buffer_delete(p);
    }
};
using BufferPtr = std::unique_ptr<brynet::base::buffer_s, BufferDeleter>;

// 创建 RAII 管理的 buffer
inline auto make_buffer(size_t size) -> BufferPtr
{
    return BufferPtr(brynet::base::buffer_new(size));
}

// 全局 WSA 初始化/清理：doctest 在同一进程中顺序运行所有测试，
// 不能在每个测试用例中调用 DestroySocket()，否则后续测试的 WSA 状态会被破坏
struct WsaListener : doctest::IReporter
{
    const doctest::ContextOptions& opt;
    WsaListener(const doctest::ContextOptions& in) : opt(in) {}

    void test_run_start() override { brynet::net::base::InitSocket(); }
    void test_run_end(const doctest::TestRunStats&) override { brynet::net::base::DestroySocket(); }

    // 必须实现的纯虚函数（空实现）
    void report_query(const doctest::QueryData&) override {}
    void test_case_start(const doctest::TestCaseData&) override {}
    void test_case_reenter(const doctest::TestCaseData&) override {}
    void test_case_end(const doctest::CurrentTestCaseStats&) override {}
    void test_case_exception(const doctest::TestCaseException&) override {}
    void subcase_start(const doctest::SubcaseSignature&) override {}
    void subcase_end() override {}
    void log_assert(const doctest::AssertData&) override {}
    void log_message(const doctest::MessageData&) override {}
    void test_case_skipped(const doctest::TestCaseData&) override {};
};
DOCTEST_REGISTER_LISTENER("wsa-init", 1, WsaListener);

// ====================================================================
//  第一层：基础工具模块
// ====================================================================

// --------------------------------------------------------------------
//  1. Packet — BasePacketWriter / BasePacketReader
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("Packet - Writer/Reader 往返验证")
{
    // BasePacketWriter: 向缓冲区写入二进制数据，支持字节序转换
    // BasePacketReader: 从缓冲区读取二进制数据，与 Writer 配对使用
    // 参数: (buffer, len, useBigEndian, isAutoMalloc)
    std::array<char, 256> buf{};

    // 小端模式写入
    brynet::base::BasePacketWriter writer(buf.data(), buf.size(), false, false);
    writer.writeUINT8(0x42);
    writer.writeUINT16(1000);
    writer.writeUINT32(123456);
    writer.writeUINT64(0xDEADBEEFCAFEULL);
    writer.writeBinary("hello");

    // 从同一缓冲区读回数据
    brynet::base::BasePacketReader reader(buf.data(), writer.getPos(), false);
    DOCTEST_CHECK(reader.readUINT8()  == 0x42);
    DOCTEST_CHECK(reader.readUINT16() == 1000);
    DOCTEST_CHECK(reader.readUINT32() == 123456);
    DOCTEST_CHECK(reader.readUINT64() == 0xDEADBEEFCAFEULL);
}

DOCTEST_TEST_CASE("Packet - 大端模式写入与读取")
{
    // useBigEndian=true: 整数按网络字节序（大端）写入
    std::array<char, 256> buf{};

    brynet::base::BasePacketWriter writer(buf.data(), buf.size(), true, false);
    writer.writeUINT16(0x1234);
    writer.writeUINT32(0x56789ABC);

    brynet::base::BasePacketReader reader(buf.data(), writer.getPos(), true);
    DOCTEST_CHECK(reader.readUINT16() == 0x1234);
    DOCTEST_CHECK(reader.readUINT32() == 0x56789ABC);
}

DOCTEST_TEST_CASE("Packet - enough() 与 getLeft() 判断剩余数据")
{
    // enough(size): 判断剩余数据是否 >= size
    // getLeft(): 获取剩余可读字节数
    std::array<char, 64> buf{};
    brynet::base::BasePacketWriter writer(buf.data(), buf.size(), false, false);
    writer.writeUINT32(0x42);

    brynet::base::BasePacketReader reader(buf.data(), writer.getPos(), false);
    DOCTEST_CHECK(reader.enough(4));
    DOCTEST_CHECK(reader.getLeft() == 4);
    DOCTEST_CHECK(!reader.enough(5));

    reader.readUINT8();
    DOCTEST_CHECK(reader.getLeft() == 3);
}

DOCTEST_TEST_CASE("BigPacket - 预分配32KB数据包")
{
    // BigPacket: AutoMallocPacket<32*1024> 别名，适合大包写入
    // 参数: (useBigEndian, isAutoMalloc)
    brynet::base::BigPacket packet(false, false);
    DOCTEST_CHECK(packet.getMaxLen() == 32 * 1024);
    DOCTEST_CHECK(packet.getPos() == 0);

    packet.writeUINT32(0xDEADBEEF);
    DOCTEST_CHECK(packet.getPos() == 4);
}

DOCTEST_TEST_CASE("Packet - 流式写入 operator<<")
{
    // operator<<: 支持链式写入基本类型
    std::array<char, 128> buf{};
    brynet::base::BasePacketWriter writer(buf.data(), buf.size(), false, false);

    writer << uint8_t(0x01) << uint16_t(0x0203) << uint32_t(0x04050607);
    DOCTEST_CHECK(writer.getPos() == 7);

    brynet::base::BasePacketReader reader(buf.data(), writer.getPos(), false);
    DOCTEST_CHECK(reader.readUINT8()  == 0x01);
    DOCTEST_CHECK(reader.readUINT16() == 0x0203);
    DOCTEST_CHECK(reader.readUINT32() == 0x04050607);
}

// --------------------------------------------------------------------
//  2. Buffer — 环形缓冲区
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("Buffer - 创建、写入与读取")
{
    // buffer_new(size): 分配指定大小的环形缓冲区
    // buffer_delete: 释放缓冲区（通过 RAII BufferPtr 管理）
    auto buf = make_buffer(1024);
    DOCTEST_REQUIRE(buf != nullptr);
    DOCTEST_CHECK(brynet::base::buffer_getsize(buf.get()) == 1024);
    DOCTEST_CHECK(brynet::base::buffer_getwritevalidcount(buf.get()) == 1024);
    DOCTEST_CHECK(brynet::base::buffer_getreadvalidcount(buf.get()) == 0);

    // buffer_write: 向缓冲区写入数据
    std::string_view data = "hello";
    DOCTEST_CHECK(brynet::base::buffer_write(buf.get(), data.data(), data.size()));
    DOCTEST_CHECK(brynet::base::buffer_getreadvalidcount(buf.get()) == 5);

    // buffer_getreadptr: 获取可读数据起始指针
    DOCTEST_CHECK(std::memcmp(brynet::base::buffer_getreadptr(buf.get()), "hello", 5) == 0);
}

DOCTEST_TEST_CASE("Buffer - adjustto_head 将数据移到头部")
{
    // buffer_adjustto_head: 将未消费的数据移动到缓冲区头部，释放前方空间
    auto buf = make_buffer(64);
    brynet::base::buffer_write(buf.get(), "abcd", 4);

    // 消费前两个字节 "ab"
    brynet::base::buffer_addreadpos(buf.get(), 2);
    DOCTEST_CHECK(brynet::base::buffer_getreadpos(buf.get()) == 2);

    // adjustto_head: 将剩余 "cd" 移到头部
    brynet::base::buffer_adjustto_head(buf.get());
    DOCTEST_CHECK(brynet::base::buffer_getreadpos(buf.get()) == 0);
    DOCTEST_CHECK(brynet::base::buffer_getreadvalidcount(buf.get()) == 2);
}

DOCTEST_TEST_CASE("Buffer - buffer_init 重置读写位置")
{
    // buffer_init: 将读写位置重置为 0
    auto buf = make_buffer(64);
    brynet::base::buffer_write(buf.get(), "test", 4);
    DOCTEST_CHECK(brynet::base::buffer_getreadvalidcount(buf.get()) == 4);

    brynet::base::buffer_init(buf.get());
    DOCTEST_CHECK(brynet::base::buffer_getreadvalidcount(buf.get()) == 0);
    DOCTEST_CHECK(brynet::base::buffer_getwritevalidcount(buf.get()) == 64);
}

// --------------------------------------------------------------------
//  3. Timer / RepeatTimer / TimerMgr
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("TimerMgr - 一次性定时器触发")
{
    // TimerMgr::addTimer(timeout, callback): 添加一次性定时器，返回 weak_ptr<Timer>
    // TimerMgr::schedule(): 执行到期定时器
    auto mgr = std::make_shared<brynet::base::TimerMgr>();
    bool fired = false;

    [[maybe_unused]] auto weak_t = mgr->addTimer(std::chrono::milliseconds(1), [&] {
        fired = true;
    });

    DOCTEST_CHECK(!mgr->isEmpty());
    std::this_thread::sleep_for(std::chrono::milliseconds(2));
    mgr->schedule();
    DOCTEST_CHECK(fired);
    DOCTEST_CHECK(mgr->isEmpty());
}

DOCTEST_TEST_CASE("TimerMgr - 重复定时器与取消")
{
    // addIntervalTimer(interval, callback): 添加重复定时器，返回 shared_ptr<RepeatTimer>
    // RepeatTimer::cancel(): 取消重复定时器
    // RepeatTimer::isCancel(): 查询是否已取消
    auto mgr = std::make_shared<brynet::base::TimerMgr>();
    int count = 0;

    auto repeat_t = mgr->addIntervalTimer(std::chrono::milliseconds(1), [&] {
        ++count;
    });

    for (int i = 0; i < 5; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        mgr->schedule();
    }
    DOCTEST_CHECK(count > 0);

    repeat_t->cancel();
    DOCTEST_CHECK(repeat_t->isCancel());
}

DOCTEST_TEST_CASE("Timer - 取消一次性定时器")
{
    // Timer::cancel(): 在触发前取消定时器，使其不再执行回调
    auto mgr = std::make_shared<brynet::base::TimerMgr>();
    bool fired = false;
    auto weak_t = mgr->addTimer(std::chrono::milliseconds(10), [&] { fired = true; });

    // lock 获取 shared_ptr 后取消
    if (auto t = weak_t.lock()) { t->cancel(); }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    mgr->schedule();
    DOCTEST_CHECK(!fired);
}

DOCTEST_TEST_CASE("TimerMgr - nearLeftTime 与 isEmpty / clear")
{
    // isEmpty(): 管理器是否没有定时器
    // nearLeftTime(): 最近定时器剩余时间
    // clear(): 清空所有定时器
    auto mgr = std::make_shared<brynet::base::TimerMgr>();
    DOCTEST_CHECK(mgr->isEmpty());
    DOCTEST_CHECK(mgr->nearLeftTime() == std::chrono::nanoseconds::zero());

    mgr->addTimer(std::chrono::milliseconds(10), [](){});
    DOCTEST_CHECK(!mgr->isEmpty());

    auto left = mgr->nearLeftTime();
    DOCTEST_CHECK(left <= std::chrono::milliseconds(10));
    DOCTEST_CHECK(left > std::chrono::nanoseconds::zero());

    mgr->clear();
    DOCTEST_CHECK(mgr->isEmpty());
}

// --------------------------------------------------------------------
//  4. WaitGroup
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("WaitGroup - 多线程同步等待")
{
    // WaitGroup::Create(): 创建 WaitGroup
    // add(n): 增加计数器 n
    // done(): 减少计数器 1
    // wait(): 阻塞直到计数器归零
    auto wg = brynet::base::WaitGroup::Create();
    wg->add(2);

    std::atomic_int counter{0};
    auto t1 = std::thread([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ++counter;
        wg->done();
    });
    auto t2 = std::thread([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        ++counter;
        wg->done();
    });
    wg->wait();

    DOCTEST_CHECK(counter.load() == 2);

    if (t1.joinable())
        t1.join();

    if (t2.joinable())
        t2.join();
}

DOCTEST_TEST_CASE("WaitGroup - 带超时的等待")
{
    // wait(timeout): 等待指定时间，超时后返回（不阻塞）
    auto wg = brynet::base::WaitGroup::Create();
    wg->add(1);
    // 无人 done()，10ms 超时后返回
    wg->wait(std::chrono::milliseconds(10));
}

// --------------------------------------------------------------------
//  5. Endian
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("Endian - 字节序转换往返一致性")
{
    using namespace brynet::base::endian;
    // hostToNetwork → networkToHost 往返后值不变
    const auto v16 = uint16_t{0x1234};
    const auto v32 = uint32_t{0x12345678};
    const auto v64 = uint64_t{0x0123456789ABCDEFULL};

    DOCTEST_CHECK(networkToHost16(hostToNetwork16(v16)) == v16);
    DOCTEST_CHECK(networkToHost32(hostToNetwork32(v32)) == v32);
    DOCTEST_CHECK(networkToHost64(hostToNetwork64(v64)) == v64);

    // convert=false 时直接返回原值，不进行字节序转换
    DOCTEST_CHECK(hostToNetwork32(v32, false) == v32);
    DOCTEST_CHECK(networkToHost32(v32, false) == v32);
}

// --------------------------------------------------------------------
//  6. Base64 / SHA1
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("Base64 - 编解码往返")
{
    // base64_encode(bytes, len): 将二进制数据编码为 Base64 字符串
    // base64_decode(encoded): 将 Base64 字符串解码为原始数据
    using namespace brynet::base::crypto;
    const std::string original = "Hello, brynet! 你好世界";
    auto encoded = base64_encode(
        reinterpret_cast<const unsigned char*>(original.data()),
        static_cast<unsigned int>(original.size()));
    auto decoded = base64_decode(encoded);
    DOCTEST_CHECK(decoded == original);
}

DOCTEST_TEST_CASE("SHA1 - 标准测试向量")
{
    // CSHA1: SHA1 哈希计算器
    // Update(data, len): 输入数据
    // Final(): 完成计算
    // GetHash(dest20): 获取 20 字节哈希值
    // ReportHashStl(str, type): 以十六进制格式输出
    // SHA1("abc") = A9993E36 4706816A BA3E2571 7850C26C 9CD0D89D
    CSHA1 sha1;
    sha1.Update(reinterpret_cast<const unsigned char*>("abc"), 3);
    sha1.Final();

    std::array<unsigned char, 20> hash{};
    DOCTEST_REQUIRE(sha1.GetHash(hash.data()));
    DOCTEST_CHECK(hash[0] == 0xA9);
    DOCTEST_CHECK(hash[1] == 0x99);
    DOCTEST_CHECK(hash[2] == 0x3E);
    DOCTEST_CHECK(hash[3] == 0x36);

    std::string hex_str;
    sha1.ReportHashStl(hex_str, CSHA1::REPORT_HEX_SHORT);
    DOCTEST_CHECK(hex_str.size() == 40);  // 20 字节 → 40 个十六进制字符
}

DOCTEST_TEST_CASE("SHA1 - Reset 后重新计算")
{
    // Reset(): 重置 SHA1 状态，可重新计算
    CSHA1 sha1;
    sha1.Update(reinterpret_cast<const unsigned char*>("test"), 4);
    sha1.Reset();
    sha1.Update(reinterpret_cast<const unsigned char*>("abc"), 3);
    sha1.Final();

    std::array<unsigned char, 20> hash{};
    DOCTEST_REQUIRE(sha1.GetHash(hash.data()));
    DOCTEST_CHECK(hash[0] == 0xA9);
}

// ====================================================================
//  第二层：Socket 封装与平台工具
// ====================================================================

// --------------------------------------------------------------------
//  7. SocketLibFunction — Socket 初始化与工具
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("SocketLibFunction - InitSocket / DestroySocket")
{
    // InitSocket(): 初始化 Socket 库（Windows 下执行 WSAStartup）— 已由全局 fixture 完成
    // DestroySocket(): 清理 Socket 库（Windows 下执行 WSACleanup）
    // 此处仅验证可调用性，不实际 DestroySocket（以免影响后续测试）
    DOCTEST_CHECK(brynet::net::base::InitSocket()); // 重复调用应安全返回 true
}

DOCTEST_TEST_CASE("SocketLibFunction - Socket 创建与关闭")
{
    // SocketCreate(af, type, protocol): 创建 socket 文件描述符
    // SocketClose(fd): 关闭 socket
    // SocketNonblock(fd): 设置非阻塞模式
    // SocketNodelay(fd): 禁用 Nagle 算法
    auto fd = brynet::net::base::SocketCreate(AF_INET, SOCK_STREAM, 0);
    DOCTEST_REQUIRE(fd != BRYNET_INVALID_SOCKET);

    DOCTEST_CHECK(brynet::net::base::SocketNonblock(fd));
    DOCTEST_CHECK(brynet::net::base::SocketNodelay(fd) == 0);

    brynet::net::base::SocketClose(fd);
}

// --------------------------------------------------------------------
//  8. Exception
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("Exception - ConnectException 与 BrynetCommonException")
{
    // ConnectException(msg): 网络连接失败的异常类型
    // BrynetCommonException(msg): brynet 通用运行时异常
    brynet::net::ConnectException ex("connection failed");
    DOCTEST_CHECK(std::string_view{ex.what()} == "connection failed");

    brynet::net::BrynetCommonException ex2("common error");
    DOCTEST_CHECK(std::string_view{ex2.what()} == "common error");
}

// --------------------------------------------------------------------
//  9. CurrentThread
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("CurrentThread - tid() 返回有效线程ID")
{
    // tid(): 获取当前线程的唯一标识，同一线程多次调用应一致
    const auto id1 = brynet::net::current_thread::tid();
    const auto id2 = brynet::net::current_thread::tid();
    DOCTEST_CHECK(id1 == id2);
}

// ====================================================================
//  第三层：核心网络模块
// ====================================================================

// --------------------------------------------------------------------
//  10. EventLoop
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("EventLoop - 构造与绑定线程")
{
    // EventLoop(): 创建事件循环（内部创建 epoll/kqueue/IOCP）
    // bindCurrentThread(): 绑定到当前线程（必须先调用才能 loop）
    // isInLoopThread(): 判断当前线程是否为事件循环线程
    auto ev = std::make_shared<brynet::net::EventLoop>();
    ev->bindCurrentThread();
    DOCTEST_CHECK(ev->isInLoopThread());
}

DOCTEST_TEST_CASE("EventLoop - runAfter 一次性定时器")
{
    // runAfter(timeout, callback): 注册一次性定时器
    // loop(milliseconds): 运行事件循环指定毫秒数
    auto ev = std::make_shared<brynet::net::EventLoop>();
    ev->bindCurrentThread();

    bool fired = false;
    ev->runAfter(std::chrono::milliseconds(1), [&] { fired = true; });
    // 多次 loop 确保定时器有机会触发
    for (int i = 0; i < 5 && !fired; ++i) {
        ev->loop(10);
    }
    DOCTEST_CHECK(fired);
}

DOCTEST_TEST_CASE("EventLoop - runIntervalTimer 重复定时器")
{
    // runIntervalTimer(timeout, callback): 注册重复定时器
    auto ev = std::make_shared<brynet::net::EventLoop>();
    ev->bindCurrentThread();

    int count = 0;
    auto timer = ev->runIntervalTimer(std::chrono::milliseconds(1), [&] { ++count; });
    for (int i = 0; i < 5; ++i) {
        ev->loop(5);
    }
    DOCTEST_CHECK(count > 0);
    timer->cancel();
}

DOCTEST_TEST_CASE("EventLoop - runAsyncFunctor 跨线程投递")
{
    // runAsyncFunctor(f): 从其他线程投递 functor 到事件循环线程执行
    auto ev = std::make_shared<brynet::net::EventLoop>();
    ev->bindCurrentThread();

    bool async_executed = false;
    std::thread t([&] {
        ev->runAsyncFunctor([&] { async_executed = true; });
    });
    // 多次 loop 确保 async functor 有机会被处理
    for (int i = 0; i < 5 && !async_executed; ++i) {
        ev->loop(10);
    }
    t.join();
    DOCTEST_CHECK(async_executed);
}

DOCTEST_TEST_CASE("EventLoop - wakeup 跨线程唤醒")
{
    // wakeup(): 从其他线程唤醒事件循环（使 loop 提前返回）
    auto ev = std::make_shared<brynet::net::EventLoop>();
    ev->bindCurrentThread();

    bool woken = false;
    std::thread t([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        ev->runAsyncFunctor([&] { woken = true; });
        ev->wakeup();
    });
    ev->loop(500);  // wakeup 应使其提前返回
    t.join();
    // wakeup 后可能还需要一次 loop 来处理 async functor
    if (!woken) {
        ev->loop(50);
    }
    DOCTEST_CHECK(woken);
}

// ====================================================================
//  第四层：HTTP / WebSocket 模块
// ====================================================================

// --------------------------------------------------------------------
//  11. HttpFormat
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("HttpFormat - 构建 GET 请求")
{
    // HttpRequest: 构建客户端 HTTP 请求
    // setMethod: 设置 HTTP 方法
    // setUrl: 设置请求路径
    // setHost: 设置 Host 头
    // getResult: 生成完整 HTTP 请求文本
    brynet::net::http::HttpRequest req;
    req.setMethod(brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_GET);
    req.setUrl("/api/test");
    req.setHost("example.com");
    const auto req_str = req.getResult();
    DOCTEST_CHECK(req_str.find("GET /api/test") != std::string::npos);
    DOCTEST_CHECK(req_str.find("Host: example.com") != std::string::npos);
}

DOCTEST_TEST_CASE("HttpFormat - 构建 POST 请求")
{
    // setBody: 设置请求体
    // setContentType: 设置 Content-Type
    // addHeadValue: 添加自定义头部
    brynet::net::http::HttpRequest req;
    req.setMethod(brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_POST);
    req.setUrl("/api/data");
    req.setHost("example.com");
    req.setContentType("application/json");
    req.setBody(R"({"key":"value"})");
    req.addHeadValue("X-Custom", "test");
    const auto req_str = req.getResult();
    DOCTEST_CHECK(req_str.find("POST /api/data") != std::string::npos);
    DOCTEST_CHECK(req_str.find(R"({"key":"value"})") != std::string::npos);
}

DOCTEST_TEST_CASE("HttpFormat - 构建 HTTP 响应")
{
    // HttpResponse: 构建服务端 HTTP 响应
    // setStatus: 设置 HTTP 状态码
    // setContentType / setBody: 设置内容类型和响应体
    brynet::net::http::HttpResponse resp;
    resp.setStatus(brynet::net::http::HttpResponse::HTTP_RESPONSE_STATUS::OK);
    resp.setContentType("application/json");
    resp.setBody(R"({"status":"ok"})");
    const auto resp_str = resp.getResult();
    DOCTEST_CHECK(resp_str.find("HTTP/1.1 200 OK") != std::string::npos);
    DOCTEST_CHECK(resp_str.find(R"({"status":"ok"})") != std::string::npos);
}

DOCTEST_TEST_CASE("HttpFormat - HttpQueryParameter 查询参数")
{
    // HttpQueryParameter: 构建键值对查询参数
    // add(k, v): 添加参数
    // getResult(): 获取编码后的查询字符串
    brynet::net::http::HttpQueryParameter params;
    params.add("key1", "value1");
    params.add("key2", "value2");
    DOCTEST_CHECK(params.getResult() == "key1=value1&key2=value2");
}

// --------------------------------------------------------------------
//  12. HTTPParser
// --------------------------------------------------------------------
// DOCTEST_TEST_CASE("HTTPParser - 解析 GET 请求")
// {
//     // HTTPParser(type): 构造解析器（HTTP_REQUEST/HTTP_RESPONSE/HTTP_BOTH）
//     // tryParse(buf, len): 尝试解析，返回已解析字节数
//     // isCompleted(): 解析是否完成
//     // getPath / getQuery: 获取请求路径和查询参数
//     // hasKey / getValue: 查询请求头
//     brynet::net::http::HTTPParser parser(HTTP_BOTH);
//     const std::string request = "GET /test?foo=bar HTTP/1.1\r\nHost: example.com\r\n\r\n";
//     const auto parsed = parser.tryParse(request.data(), request.size());
//     DOCTEST_CHECK(parsed == request.size());
//     DOCTEST_CHECK(parser.isCompleted());
//     DOCTEST_CHECK(parser.getPath() == "/test");
//     DOCTEST_CHECK(parser.getQuery() == "foo=bar");
//     DOCTEST_CHECK(parser.hasKey("Host"));
//     DOCTEST_CHECK(parser.getValue("Host") == "example.com");
// }
// DOCTEST_TEST_CASE("HTTPParser - 解析 POST 请求并获取 Body")
// {
//     // getBody(): 获取请求体内容
//     // method(): 获取 HTTP 方法
//     brynet::net::http::HTTPParser parser(HTTP_BOTH);
//     const std::string request =
//         "POST /submit HTTP/1.1\r\n"
//         "Host: example.com\r\n"
//         "Content-Length: 13\r\n"
//         "\r\n"
//         "hello, world!";
//     const auto parsed = parser.tryParse(request.data(), request.size());
//     DOCTEST_CHECK(parser.isCompleted());
//     DOCTEST_CHECK(parser.getPath() == "/submit");
//     DOCTEST_CHECK(parser.getBody() == "hello, world!");
// }
// DOCTEST_TEST_CASE("HTTPParser - isKeepAlive 判断")
// {
//     // isKeepAlive(): 判断是否为 Keep-Alive 连接
//     brynet::net::http::HTTPParser parser(HTTP_BOTH);
//     const std::string request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
//     parser.tryParse(request.data(), request.size());
//     DOCTEST_CHECK(parser.isKeepAlive());
// }

// --------------------------------------------------------------------
//  13. WebSocketFormat
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("WebSocketFormat - wsHandshake 生成握手响应")
{
    // wsHandshake(secKey): 根据 Sec-WebSocket-Key 生成握手响应
    using WSF = brynet::net::http::WebSocketFormat;
    const auto handshake = WSF::wsHandshake("dGhlIHNhbXBsZSBub25jZQ==");
    DOCTEST_CHECK(handshake.find("HTTP/1.1 101 Switching Protocols") != std::string::npos);
    DOCTEST_CHECK(handshake.find("Upgrade: websocket") != std::string::npos);
    DOCTEST_CHECK(handshake.find("Sec-WebSocket-Accept:") != std::string::npos);
}

DOCTEST_TEST_CASE("WebSocketFormat - 帧 build→extract 往返")
{
    // wsFrameBuild: 构建 WebSocket 帧
    // wsFrameExtractBuffer: 解析 WebSocket 帧
    using WSF = brynet::net::http::WebSocketFormat;

    const std::string payload = "Hello WebSocket";
    std::string frame;
    DOCTEST_CHECK(WSF::wsFrameBuild(payload.data(), payload.size(), frame,
                             WSF::WebSocketFrameType::TEXT_FRAME, true, false));

    std::string parsed_payload;
    WSF::WebSocketFrameType opcode{};
    size_t frame_size = 0;
    bool is_fin = false;
    DOCTEST_CHECK(WSF::wsFrameExtractBuffer(frame.data(), frame.size(),
                                     parsed_payload, opcode, frame_size, is_fin));
    DOCTEST_CHECK(parsed_payload == payload);
    DOCTEST_CHECK(opcode == WSF::WebSocketFrameType::TEXT_FRAME);
    DOCTEST_CHECK(is_fin);
}

DOCTEST_TEST_CASE("WebSocketFormat - BINARY 帧与 masking")
{
    // masking=true: 客户端发送帧需要 mask
    using WSF = brynet::net::http::WebSocketFormat;

    const std::string payload = "binary data";
    std::string frame;
    DOCTEST_CHECK(WSF::wsFrameBuild(payload.data(), payload.size(), frame,
                             WSF::WebSocketFrameType::BINARY_FRAME, true, true));

    std::string parsed_payload;
    WSF::WebSocketFrameType opcode{};
    size_t frame_size = 0;
    bool is_fin = false;
    DOCTEST_CHECK(WSF::wsFrameExtractBuffer(frame.data(), frame.size(),
                                     parsed_payload, opcode, frame_size, is_fin));
    DOCTEST_CHECK(parsed_payload == payload);
    DOCTEST_CHECK(opcode == WSF::WebSocketFrameType::BINARY_FRAME);
}

// ====================================================================
//  第五层：Builder 流式 API + 集成测试
// ====================================================================

// --------------------------------------------------------------------
//  14. TCP Echo 集成测试（ListenerBuilder + ConnectionBuilder）
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("集成 - TCP Echo 服务（Builder API）")
{
    // ListenerBuilder: 流式构建 TCP 监听服务
    // ConnectionBuilder: 流式构建 TCP 连接
    // IOThreadTcpService: 多 IO 线程 TCP 服务
    // AsyncConnector: 异步连接器
    const auto port = get_random_port();
    auto wg = brynet::base::WaitGroup::Create();
    wg->add(1);

    // 创建 TCP 服务并启动 1 个 IO 线程
    auto service = brynet::net::IOThreadTcpService::Create();
    service->startWorkerThread(1);

    bool server_received = false;
    bool client_received = false;

    // 使用 ListenerBuilder 搭建 Echo 服务
    brynet::net::wrapper::ListenerBuilder listener;
    listener.WithService(service)
            .WithAddr(false, "127.0.0.1", port)
            .WithMaxRecvBufferSize(1024)
            .AddEnterCallback([&](const brynet::net::TcpConnection::Ptr& session) {
                // 服务端: 收到数据后 Echo 回去
                session->setDataCallback([session, &server_received](
                        brynet::base::BasePacketReader& reader) {
                    auto data = std::string(reader.currentBuffer(), reader.getLeft());
                    server_received = true;
                    session->send(data);
                    reader.consumeAll();
                });
            })
            .asyncRun();

    // 等待监听启动
    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // 使用 ConnectionBuilder 异步连接
    auto connector = brynet::net::AsyncConnector::Create();
    connector->startWorkerThread();

    brynet::net::wrapper::ConnectionBuilder conn_builder;
    conn_builder.WithService(service)
                .WithConnector(connector)
                .WithAddr("127.0.0.1", port)
                .WithTimeout(std::chrono::seconds(2))
                .WithMaxRecvBufferSize(1024)
                .AddEnterCallback([&](const brynet::net::TcpConnection::Ptr& session) {
                    // 客户端: 连接成功后发送数据
                    session->send("hello echo");
                    session->setDataCallback([&client_received, &wg](
                            brynet::base::BasePacketReader& reader) {
                        client_received = true;
                        wg->done();  // 收到 Echo 响应，完成等待
                        reader.consumeAll();
                    });
                })
                .WithFailedCallback([&wg]() {
                    wg->done();  // 连接失败也完成等待
                })
                .asyncConnect();

    // 等待收发完成或超时
    wg->wait(std::chrono::seconds(2));
    DOCTEST_CHECK(server_received);
    DOCTEST_CHECK(client_received);

    listener.stop();
    connector->stopWorkerThread();
    service->stopWorkerThread();
}

// --------------------------------------------------------------------
//  15. HTTP 服务端+客户端 集成测试
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("集成 - HTTP 服务端与客户端")
{
    // HttpListenerBuilder: 流式构建 HTTP 监听服务
    // HttpConnectionBuilder: 流式构建 HTTP 连接
    const auto port = get_random_port();
    auto wg = brynet::base::WaitGroup::Create();
    wg->add(1);

    auto service = brynet::net::IOThreadTcpService::Create();
    service->startWorkerThread(1);

    bool http_request_received = false;

    // 使用 ListenerBuilder + HttpService::setup 搭建 HTTP 服务
    // (HttpListenerBuilder 没有 stop() 方法，改用 ListenerBuilder)
    auto http_enter_cb = [&](const brynet::net::http::HttpSession::Ptr& /*http_session*/,
                             brynet::net::http::HttpSessionHandlers& handlers) {
        // 服务端: 收到 HTTP 请求后返回响应
        handlers.setHttpEndCallback([&](const brynet::net::http::HTTPParser& /*parser*/,
                                         const brynet::net::http::HttpSession::Ptr& session) {
            http_request_received = true;
            brynet::net::http::HttpResponse resp;
            resp.setStatus(brynet::net::http::HttpResponse::HTTP_RESPONSE_STATUS::OK);
            resp.setContentType("text/plain");
            resp.setBody("OK from server");
            session->send(resp.getResult());
        });
    };

    brynet::net::wrapper::ListenerBuilder listener;
    listener.WithService(service)
            .WithAddr(false, "127.0.0.1", port)
            .WithMaxRecvBufferSize(4096)
            .AddEnterCallback([http_enter_cb](const brynet::net::TcpConnection::Ptr& session) {
                brynet::net::http::HttpService::setup(session, http_enter_cb);
            })
            .asyncRun();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // 使用 HttpConnectionBuilder 发送 HTTP 请求
    auto connector = brynet::net::AsyncConnector::Create();
    connector->startWorkerThread();

    bool http_response_received = false;

    brynet::net::wrapper::HttpConnectionBuilder http_conn;
    http_conn.WithService(service)
             .WithConnector(connector)
             .WithAddr("127.0.0.1", port)
             .WithTimeout(std::chrono::seconds(2))
             .WithMaxRecvBufferSize(4096)
             .WithEnterCallback([&](const brynet::net::http::HttpSession::Ptr& http_session,
                                     brynet::net::http::HttpSessionHandlers& handlers) {
                 // 客户端: 连接成功后发送 HTTP 请求
                 brynet::net::http::HttpRequest req;
                 req.setMethod(brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_GET);
                 req.setUrl("/test");
                 req.setHost("127.0.0.1");
                 http_session->send(req.getResult());

                 // 客户端: 接收 HTTP 响应
                 handlers.setHttpEndCallback([&](const brynet::net::http::HTTPParser& parser,
                                                  const brynet::net::http::HttpSession::Ptr& /*session*/) {
                     http_response_received = true;
                     DOCTEST_CHECK(parser.getBody() == "OK from server");
                     wg->done();
                 });
             })
             .WithFailedCallback([&wg]() {
                 wg->done();
             })
             .asyncConnect();

    wg->wait(std::chrono::seconds(2));
    DOCTEST_CHECK(http_request_received);
    DOCTEST_CHECK(http_response_received);

    listener.stop();
    connector->stopWorkerThread();
    service->stopWorkerThread();
}

// --------------------------------------------------------------------
//  16. WebSocket 握手+帧收发 集成测试
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("集成 - WebSocket 握手与帧收发")
{
    const auto port = get_random_port();
    auto wg = brynet::base::WaitGroup::Create();
    wg->add(1);

    auto service = brynet::net::IOThreadTcpService::Create();
    service->startWorkerThread(1);

    bool ws_connected = false;
    bool ws_frame_received = false;

    // 服务端: 使用 ListenerBuilder + HttpService::setup 处理 WebSocket 连接
    auto ws_enter_cb = [&](const brynet::net::http::HttpSession::Ptr& /*http_session*/,
                           brynet::net::http::HttpSessionHandlers& handlers) {
        // WS 连接建立回调
        handlers.setWSConnected([&](const brynet::net::http::HttpSession::Ptr& /*session*/,
                                     const brynet::net::http::HTTPParser& /*parser*/) {
            ws_connected = true;
        });
        // WS 帧接收回调
        handlers.setWSCallback([&](const brynet::net::http::HttpSession::Ptr& session,
                                    brynet::net::http::WebSocketFormat::WebSocketFrameType /*opcode*/,
                                    const std::string& payload) {
            ws_frame_received = true;
            // Echo 回 TEXT 帧
            std::string resp_frame;
            brynet::net::http::WebSocketFormat::wsFrameBuild(
                payload.data(), payload.size(), resp_frame,
                brynet::net::http::WebSocketFormat::WebSocketFrameType::TEXT_FRAME,
                true, false);
            session->send(resp_frame);
        });
    };

    brynet::net::wrapper::ListenerBuilder ws_listener;
    ws_listener.WithService(service)
               .WithAddr(false, "127.0.0.1", port)
               .WithMaxRecvBufferSize(4096)
               .AddEnterCallback([ws_enter_cb](const brynet::net::TcpConnection::Ptr& session) {
                   brynet::net::http::HttpService::setup(session, ws_enter_cb);
               })
               .asyncRun();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // 客户端: 连接并发送 WebSocket 帧
    auto connector = brynet::net::AsyncConnector::Create();
    connector->startWorkerThread();

    bool client_ws_echo_received = false;

    brynet::net::wrapper::HttpConnectionBuilder http_conn;
    http_conn.WithService(service)
             .WithConnector(connector)
             .WithAddr("127.0.0.1", port)
             .WithTimeout(std::chrono::seconds(2))
             .WithMaxRecvBufferSize(4096)
            .WithEnterCallback([&](const brynet::net::http::HttpSession::Ptr& http_session,
                                    brynet::net::http::HttpSessionHandlers& handlers) {
                // 客户端: 发送 WebSocket 升级请求
                brynet::net::http::HttpRequest req;
                req.setMethod(brynet::net::http::HttpRequest::HTTP_METHOD::HTTP_METHOD_GET);
                req.setUrl("/ws");
                req.addHeadValue("Host", "127.0.0.1");
                req.addHeadValue("Upgrade", "websocket");
                req.addHeadValue("Connection", "Upgrade");
                req.addHeadValue("Sec-WebSocket-Key", "dGhlIHNhbXBsZSBub25jZQ==");
                req.addHeadValue("Sec-WebSocket-Version", "13");
                http_session->send(req.getResult());

                // WS 连接建立后发送帧
                handlers.setWSConnected([&](const brynet::net::http::HttpSession::Ptr& session,
                                             const brynet::net::http::HTTPParser& /*parser*/) {
                     std::string frame;
                     brynet::net::http::WebSocketFormat::wsFrameBuild(
                         "hello ws", 8, frame,
                         brynet::net::http::WebSocketFormat::WebSocketFrameType::TEXT_FRAME,
                         true, true);
                     session->send(frame);
                 });
                 // 接收 Echo 回的 WS 帧
                handlers.setWSCallback([&](const brynet::net::http::HttpSession::Ptr& /*session*/,
                                            brynet::net::http::WebSocketFormat::WebSocketFrameType /*opcode*/,
                                             const std::string& payload) {
                     client_ws_echo_received = true;
                     DOCTEST_CHECK(payload == "hello ws");
                     wg->done();
                 });
             })
             .WithFailedCallback([&wg]() {
                 wg->done();
             })
             .asyncConnect();

    wg->wait(std::chrono::seconds(2));
    DOCTEST_CHECK(ws_connected);
    DOCTEST_CHECK(ws_frame_received);
    DOCTEST_CHECK(client_ws_echo_received);

    ws_listener.stop();
    connector->stopWorkerThread();
    service->stopWorkerThread();
}

// --------------------------------------------------------------------
//  17. 单线程 EventLoopTcpService 集成测试
// --------------------------------------------------------------------
DOCTEST_TEST_CASE("集成 - 单线程 EventLoopTcpService")
{
    // EventLoopTcpService: 使用指定 EventLoop 的单线程 TCP 服务
    // 适用于单线程事件驱动模型
    const auto port = get_random_port();
    auto wg = brynet::base::WaitGroup::Create();
    wg->add(1);

    auto ev = std::make_shared<brynet::net::EventLoop>();

    auto service = brynet::net::EventLoopTcpService::Create(ev);

    bool server_received = false;
    bool client_received = false;

    // 创建监听线程（监听仍用独立线程，但 IO 处理在 EventLoop 线程）
    auto listener = brynet::net::ListenThread::Create(
        false, "127.0.0.1", port,
        [&](brynet::net::TcpSocket::Ptr socket) {
            brynet::net::ConnectionOption option;
            option.maxRecvBufferSize = 1024;
            option.enterCallback.push_back([&](const brynet::net::TcpConnection::Ptr& session) {
                session->setDataCallback([session, &server_received](
                        brynet::base::BasePacketReader& reader) {
                    server_received = true;
                    auto data = std::string(reader.currentBuffer(), reader.getLeft());
                    session->send(data);
                    reader.consumeAll();
                });
            });
            service->addTcpConnection(std::move(socket), std::move(option));
        });
    listener->startListen();

    std::this_thread::sleep_for(std::chrono::milliseconds(30));

    // 客户端连接
    auto connector = brynet::net::AsyncConnector::Create();
    connector->startWorkerThread();

    auto client_service = brynet::net::IOThreadTcpService::Create();
    client_service->startWorkerThread(1);

    brynet::net::wrapper::ConnectionBuilder conn_builder;
    conn_builder.WithService(client_service)
                .WithConnector(connector)
                .WithAddr("127.0.0.1", port)
                .WithTimeout(std::chrono::seconds(2))
                .WithMaxRecvBufferSize(1024)
                .AddEnterCallback([&](const brynet::net::TcpConnection::Ptr& session) {
                    session->send("single thread test");
                    session->setDataCallback([&client_received, &wg](
                            brynet::base::BasePacketReader& reader) {
                        client_received = true;
                        wg->done();
                        reader.consumeAll();
                    });
                })
                .WithFailedCallback([&wg]() {
                    wg->done();
                })
                .asyncConnect();

    // 运行单线程 EventLoop 处理服务端 IO
    // bindCurrentThread 必须在运行 loop 的线程中调用
    auto loop_thread = std::thread([&]() {
        ev->bindCurrentThread();
        for (int i = 0; i < 100; ++i) {
            ev->loop(20);
        }
    });

    wg->wait(std::chrono::seconds(2));
    DOCTEST_CHECK(server_received);
    DOCTEST_CHECK(client_received);

    listener->stopListen();
    connector->stopWorkerThread();
    client_service->stopWorkerThread();

    loop_thread.join();
}