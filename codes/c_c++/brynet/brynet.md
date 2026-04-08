## brynet 库功能清单

**brynet** 是一个 Header-Only、跨平台的 C++11 异步 TCP 网络库，版本 `1.12.3`。支持 Linux(epoll)、Windows(IOCP)、macOS(kqueue)，零外部依赖。

---

### 一、核心网络模块

| 模块 | 头文件 | 功能 |
|------|--------|------|
| **EventLoop** | `net/EventLoop.hpp` | 事件循环引擎，I/O 多路复用 + 事件分发。支持 `loop()`、`wakeup()`、`runAsyncFunctor()`（跨线程投递）、`runAfter()`/`runIntervalTimer()`（定时器） |
| **TcpConnection** | `net/TcpConnection.hpp` | TCP 连接会话，管理收发/生命周期/SSL/心跳。支持 `send()`、`setDataCallback()`、`setDisConnectCallback()`、`setHeartBeat()`、`setHighWaterCallback()`、`postDisConnect()`、`postShutdown()`。Linux 下用 `writev` 零拷贝合并发送 |
| **TcpService** | `net/TcpService.hpp` | TCP 服务抽象。`IOThreadTcpService`（多 IO 线程）和 `EventLoopTcpService`（单线程）。支持 `addTcpConnection()` 将连接分配到 IO 线程 |
| **ListenThread** | `net/ListenThread.hpp` | 独立线程监听端口，`accept()` 新连接后回调。支持 IPv6、SO_REUSEPORT |
| **AsyncConnector** | `net/AsyncConnector.hpp` | 异步发起 TCP 连接，支持超时检测、自连接检测。`asyncConnect()` 配置目标地址/超时/成功失败回调 |
| **PromiseReceive** | `net/PromiseReceive.hpp` | Promise 风格消息接收：`receive(len, handler)` 按长度、`receive(shared_len, handler)` 按动态长度、`receiveUntil(str, handler)` 按分隔符 |

### 二、HTTP / WebSocket 模块

| 模块 | 头文件 | 功能 |
|------|--------|------|
| **HttpService** | `net/http/HttpService.hpp` | HTTP 会话管理，在 TCP 连接上设置 HTTP 协议处理。支持 header/body/end/WS/closed 回调 |
| **HttpFormat** | `net/http/HttpFormat.hpp` | HTTP 请求/响应构建器（`HttpRequest`/`HttpResponse`）。支持方法、URL、头部、Cookie、Content-Type、Body、查询参数 |
| **HttpParser** | `net/http/HttpParser.hpp` | 基于 C 语言 http-parser 的 C++ 封装。解析方法/路径/查询/头部/Body/状态码/KeepAlive/Upgrade |
| **WebSocketFormat** | `net/http/WebSocketFormat.hpp` | WebSocket 帧构建与解析。`wsHandshake()` 生成握手响应、`wsFrameBuild()` 构建帧、`wsFrameExtractBuffer()` 解析帧。支持 TEXT/BINARY/CLOSE/PING/PONG 帧类型 |

### 三、Builder 包装器（流式 API）

| 模块 | 头文件 | 功能 |
|------|--------|------|
| **ListenerBuilder** | `net/wrapper/ServiceBuilder.hpp` | Builder 模式构建服务端监听器：`WithService().WithAddr().WithReusePort().AddEnterCallback().WithMaxRecvBufferSize().WithSSL().asyncRun()` |
| **ConnectionBuilder** | `net/wrapper/ConnectionBuilder.hpp` | Builder 模式构建客户端连接：`WithService().WithConnector().WithAddr().WithTimeout().asyncConnect()` 或 `syncConnect()` |
| **HttpListenerBuilder** | `net/wrapper/HttpServiceBuilder.hpp` | Builder 模式构建 HTTP 服务监听器，自动设置 HTTP 协议处理 |
| **HttpConnectionBuilder** | `net/wrapper/HttpConnectionBuilder.hpp` | Builder 模式构建 HTTP 客户端连接 |

### 四、SSL/TLS 模块

| 模块 | 头文件 | 功能 |
|------|--------|------|
| **SSLHelper** | `net/SSLHelper.hpp` | SSL 辅助类。`initSSL(certificate, privatekey)` 初始化、`destroySSL()`、`getOpenSSLCTX()`。需定义 `BRYNET_USE_OPENSSL` 宏启用 |

### 五、基础工具模块

| 模块 | 头文件 | 功能 |
|------|--------|------|
| **Packet** | `base/Packet.hpp` | 二进制数据包读写器。`BasePacketWriter` 支持大/小端写入 bool/int8~int64/binary，自动扩容。`BasePacketReader` 支持大/小端读取，位置保存/恢复。`BigPacket` 预分配 32KB |
| **Buffer** | `base/Buffer.hpp` | C 风格环形缓冲区 `buffer_s`，读写指针管理、自动调整头部空间 |
| **Timer** | `base/Timer.hpp` | 定时器。`Timer`（一次性）、`RepeatTimer`（重复）、`TimerMgr`（优先队列管理器）。支持 `addTimer()`/`addIntervalTimer()`/`schedule()`/`nearLeftTime()` |
| **WaitGroup** | `base/WaitGroup.hpp` | 类 Go 语言的 WaitGroup 同步原语。`add(n)` / `done()` / `wait()` |
| **NonCopyable** | `base/NonCopyable.hpp` | 禁止拷贝和赋值的基类 |
| **AppStatus** | `base/AppStatus.hpp` | 应用状态检测：`isKeyboardHit()`、`getInputChar()`（检测键盘输入，用于优雅退出） |
| **Array/Stack** | `base/Array.hpp` / `base/Stack.hpp` | 数组和栈工具 |

### 六、字节序与加密

| 模块 | 头文件 | 功能 |
|------|--------|------|
| **Endian** | `base/endian/Endian.hpp` | 跨平台字节序转换 `hostToNetwork`/`networkToHost`（16/32/64 位） |
| **Base64** | `base/crypto/Base64.hpp` | Base64 编码/解码 |
| **SHA1** | `base/crypto/SHA1.hpp` | SHA1 哈希计算（用于 WebSocket 握手） |

### 七、Socket 封装

| 模块 | 头文件 | 功能 |
|------|--------|------|
| **Socket** | `net/Socket.hpp` | `UniqueFd`（RAII fd 封装）、`TcpSocket`（TCP Socket，支持 NoDelay/Nonblock/缓冲区设置）、`ListenSocket`（监听 Socket，处理 EMFILE 错误） |
| **SocketLibFunction** | `net/SocketLibFunction.hpp` | 跨平台 Socket 系统调用：InitSocket/SocketCreate/SocketClose/SocketNonblock/SocketNodelay/Connect/Listen/Accept/GetIPOfSocket/IsSelfConnect 等 |
| **SocketLibTypes** | `net/SocketLibTypes.hpp` | 平台相关类型定义（Windows 下 SOCKET 类型适配） |
| **Poller** | `net/Poller.hpp` | 基于 `poll`/`WSAPoll` 的 I/O 多路复用封装（用于异步连接检测） |
| **CurrentThread** | `net/CurrentThread.hpp` | 线程 ID 获取 `tid()` |

### 八、辅助与内部实现

| 模块 | 头文件 | 功能 |
|------|--------|------|
| **Channel** | `net/Channel.hpp` | 通道抽象基类 |
| **SendableMsg** | `net/SendableMsg.hpp` | 可发送消息抽象接口 |
| **Exception** | `net/Exception.hpp` | 网络异常类型 `ConnectException` |
| **ConnectionOption** | `net/detail/ConnectionOption.hpp` | 连接选项：enterCallback/enterFailedCallback/sslHelper/maxRecvBufferSize/forceSameThreadLoop |
| **Win Port** | `net/port/Win.hpp` | Windows Overlapped 扩展函数加载 |

---

### 模块依赖关系

```
EventLoop (事件循环核心)
  ├── TimerMgr (定时器)
  └── TcpConnection (TCP连接)
        ├── TcpService (多IO线程/单线程服务)
        │     ├── ListenThread (服务端监听)
        │     └── AsyncConnector (客户端连接)
        │           └── wrapper/Builder (流式API)
        │                 ├── ListenerBuilder
        │                 ├── ConnectionBuilder
        │                 ├── HttpListenerBuilder
        │                 └── HttpConnectionBuilder
        └── Http模块
              ├── HttpService (HTTP会话)
              ├── HttpParser (解析)
              ├── HttpFormat (构建)
              └── WebSocketFormat (WS帧)
```

### 关键特性总结

- **跨平台 I/O 模型**：Windows IOCP / Linux epoll / macOS kqueue
- **零拷贝发送**：Linux/macOS 使用 `writev` 合并发送
- **自适应缓冲区**：tanh 增长策略扩容
- **SSL/TLS 支持**：可选，通过 `BRYNET_USE_OPENSSL` 启用
- **HTTP/WebSocket**：内置完整的 HTTP 解析和 WebSocket 协议支持
- **Promise 接收**：按长度/分隔符的流式消息接收模式
- **Builder 模式**：流式 API 简化服务端/客户端构建