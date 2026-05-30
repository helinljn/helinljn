# 3rd/curl 异步 HTTP/HTTPS/WS/WSS Client 调用链

本文整理本仓库 `3rd/curl` 作为异步 client 使用时，一次完整 `http://`、`https://`、`ws://`、`wss://` 请求从应用层发起、DNS 解析、TCP/TLS 建连、HTTP 请求发送、HTTP 响应接收、WebSocket Upgrade、WebSocket frame 收发，到完成通知和清理的调用链。

## 1. 本仓库 curl 构建前提

当前仓库通过 `3rd/3rd-cmake/curl.cmake` 固定 curl 的能力边界：主要保留 `http`、`https`、`ws`、`wss`，其余无关协议和扩展基本关闭。

保留的核心能力：

- `CURL_DISABLE_HTTP=OFF`：启用 HTTP。`http://`、`https://`、`ws://`、`wss://` 都依赖 HTTP 协议栈。
- `CURL_DISABLE_WEBSOCKETS=OFF`：启用 WebSocket。`ws://` 和 `wss://` 通过 HTTP/1.1 `Upgrade: websocket` 建立。
- `CURL_ENABLE_SSL=ON`、`CURL_USE_OPENSSL=ON`：启用 SSL/TLS，HTTPS/WSS 走 OpenSSL。
- `ENABLE_THREADED_RESOLVER=ON`：启用 threaded DNS lookup。
- `ENABLE_ARES=OFF`：不使用 c-ares。

明确关闭的相关能力：

- `CURL_DISABLE_DOH=ON`：关闭 DNS-over-HTTPS。
- `USE_NGHTTP2=OFF`：关闭 HTTP/2。
- `USE_NGTCP2=OFF`、`USE_QUICHE=OFF`：关闭 HTTP/3/QUIC。
- `USE_HTTPSRR=OFF`、`USE_ECH=OFF`：关闭 HTTPS RR 和 ECH。
- `CURL_ZLIB=OFF`、`CURL_BROTLI=OFF`、`CURL_ZSTD=OFF`：关闭压缩依赖。
- `CURL_DISABLE_PROXY=ON`、`CURL_DISABLE_COOKIES=ON`、`CURL_DISABLE_HTTP_AUTH=ON`：关闭代理、cookie、HTTP 认证等 HTTP 侧扩展。
- FTP、SMTP、IMAP、MQTT、RTSP、TELNET、TFTP、DICT、FILE、LDAP、GOPHER、IPFS 等协议全部关闭。

构建生成的 `curl_config.h` 中启用了 `HAVE_GETADDRINFO`、`HAVE_GETADDRINFO_THREADSAFE` 和 `USE_RESOLV_THREADED`，因此普通域名解析走系统 `getaddrinfo()`，但由 libcurl 内部 DNS 线程池异步执行。

因此，这里的“异步 client”主要由以下几部分组成：

- libcurl multi 状态机驱动多个 easy handle。
- socket 采用非阻塞 connect/send/recv。
- DNS 使用 libcurl threaded resolver，把 `getaddrinfo()` 放到内部 DNS 线程队列中执行。
- HTTPS/WSS 在非阻塞 socket 上继续推进 OpenSSL TLS 握手。
- WS/WSS 在 HTTP/1.1 请求里发起 `Upgrade: websocket`，收到 `101 Switching Protocols` 后切换到 WebSocket frame 编解码。

## 2. 应用层典型调用方式

### 2.1 初始化和 easy 配置

应用层一般按下面顺序使用：

```cpp
curl_global_init(CURL_GLOBAL_DEFAULT);

CURLM *multi = curl_multi_init();
CURL *easy = curl_easy_init();

curl_easy_setopt(easy, CURLOPT_URL, "https://example.com/path");
curl_easy_setopt(easy, CURLOPT_WRITEFUNCTION, write_callback);
curl_easy_setopt(easy, CURLOPT_WRITEDATA, user_data);
curl_easy_setopt(easy, CURLOPT_HEADERFUNCTION, header_callback);
curl_easy_setopt(easy, CURLOPT_HEADERDATA, user_data);
curl_easy_setopt(easy, CURLOPT_PRIVATE, user_request);

curl_easy_setopt(easy, CURLOPT_CONNECTTIMEOUT_MS, 3000L);
curl_easy_setopt(easy, CURLOPT_TIMEOUT_MS, 10000L);

curl_multi_add_handle(multi, easy);
```

常见业务关注点：

- `CURLOPT_WRITEFUNCTION`：接收响应 body。
- `CURLOPT_HEADERFUNCTION`：接收响应头。
- `CURLOPT_READFUNCTION`：POST/PUT/upload 时提供请求 body。
- `CURLOPT_PRIVATE`：把业务请求对象挂到 easy 上，完成后通过 `curl_easy_getinfo(..., CURLINFO_PRIVATE, ...)` 取回。
- `CURLOPT_TIMEOUT_MS`：整个请求总超时。
- `CURLOPT_CONNECTTIMEOUT_MS`：DNS、TCP、TLS 等连接阶段超时。
- `CURLOPT_PROTOCOLS_STR`：如果 URL 来自外部输入，建议限制为 `http,https,ws,wss`。
- `CURLOPT_WS_OPTIONS`：WebSocket 选项，例如 `CURLWS_RAW_MODE`、`CURLWS_NOAUTOPONG`。
- `CURLOPT_CONNECT_ONLY=2L`：WebSocket connect-only 模型，完成握手后交给业务调用 `curl_ws_send()` / `curl_ws_recv()`。

### 2.2 WebSocket 两种 API 模型

WebSocket URL 使用 `ws://` 或 `wss://`：

```cpp
curl_easy_setopt(easy, CURLOPT_URL, "wss://example.com/socket");
```

libcurl 支持两种 WebSocket 使用模型。

第一种是 callback 模型，`CURLOPT_CONNECT_ONLY` 不设置或设置为 `0L`：

```text
curl_multi_perform() / curl_multi_socket_action()
  -> 建立 TCP/TLS
  -> 发送 HTTP Upgrade 请求
  -> 收到 101 后切换 WebSocket
  -> 后续收到 WebSocket payload 时调用 CURLOPT_WRITEFUNCTION
  -> write callback 内可调用 curl_ws_meta() 获取当前 frame 元信息
```

这种模型适合服务端持续推送、业务只需要在回调中处理消息的场景。发送数据可以：

- 在 write callback 内或外部调用 `curl_ws_send()`。
- 使用 `CURLOPT_READFUNCTION + CURLOPT_UPLOAD` 让 libcurl 从 read callback 读取数据并编码成 WebSocket frame。
- 在 read callback 中调用 `curl_ws_start_frame()` 指定 frame 类型和长度。

第二种是 connect-only 模型，必须设置 `CURLOPT_CONNECT_ONLY=2L`：

```cpp
curl_easy_setopt(easy, CURLOPT_CONNECT_ONLY, 2L);
```

这种模型下：

```text
curl_multi_perform() / curl_multi_socket_action()
  -> 只负责完成 DNS/TCP/TLS/HTTP Upgrade
  -> 101 切换成功后本次 transfer 完成
  -> 应用层后续显式调用 curl_ws_recv() / curl_ws_send()
```

这更适合项目里已有自己的连接调度或消息循环，希望把 WebSocket frame 的收发主动权放在业务层。

### 2.3 两种 multi 驱动方式

简单轮询方式：

```cpp
int running = 0;
curl_multi_perform(multi, &running);

while(running) {
  int numfds = 0;
  curl_multi_poll(multi, nullptr, 0, 1000, &numfds);
  curl_multi_perform(multi, &running);

  int msgs_left = 0;
  while(CURLMsg *msg = curl_multi_info_read(multi, &msgs_left)) {
    if(msg->msg == CURLMSG_DONE) {
      CURL *easy = msg->easy_handle;
      CURLcode result = msg->data.result;
      // 读取 HTTP code、业务 private、清理 easy。
    }
  }
}
```

事件驱动方式：

```cpp
curl_multi_setopt(multi, CURLMOPT_SOCKETFUNCTION, socket_callback);
curl_multi_setopt(multi, CURLMOPT_SOCKETDATA, event_loop);
curl_multi_setopt(multi, CURLMOPT_TIMERFUNCTION, timer_callback);
curl_multi_setopt(multi, CURLMOPT_TIMERDATA, event_loop);

// socket 可读/可写事件到来时：
curl_multi_socket_action(multi, sockfd, ev_bitmask, &running);

// timer 到期时：
curl_multi_socket_action(multi, CURL_SOCKET_TIMEOUT, 0, &running);
```

事件驱动方式下，业务 event loop 只负责监听 libcurl 告诉你的 socket 和 timer；真正的请求状态推进仍然发生在 `curl_multi_socket_action()` 里面。

## 3. multi 状态机总览

libcurl 的异步核心是 `multi.c` 里的状态机。外部入口大致有两条：

```text
curl_multi_perform()
  -> multi_perform()
    -> multi_runsingle()
```

或：

```text
curl_multi_socket_action()
  -> multi_socket()
    -> multi_runsingle()
```

`multi_runsingle()` 根据 easy handle 的 `mstate` 推进一次请求。典型 HTTP/HTTPS/WS/WSS 请求的状态变化如下：

```text
MSTATE_INIT
  -> Curl_pretransfer()

MSTATE_SETUP
  -> 设置本次请求的开始时间
  -> 设置总超时、连接超时

MSTATE_CONNECT
  -> state_connect()
    -> Curl_connect()

MSTATE_CONNECTING
  -> Curl_conn_connect()
  -> 等待非阻塞 TCP/TLS connect 完成

MSTATE_PROTOCONNECT
  -> protocol_connect()
  -> HTTPS 时推进 TLS/protocol connect

MSTATE_PROTOCONNECTING
  -> protocol_connecting()
  -> TLS 等协议连接未完成时继续等待 socket 事件

MSTATE_DO
  -> state_do()
    -> Curl_http()
  -> 生成并发送 HTTP request
  -> WS/WSS 在这里追加 WebSocket Upgrade 请求头

MSTATE_DOING
  -> protocol_doing()
  -> 协议 do 阶段未完成时继续推进

MSTATE_DID
  -> 准备进入真正的数据收发阶段

MSTATE_PERFORMING
  -> state_performing()
    -> Curl_sendrecv()
  -> HTTP/HTTPS：发送剩余 request body，接收 response header/body
  -> WS/WSS callback 模型：101 后继续接收 WebSocket frame 并回调业务

MSTATE_DONE
  -> multi_done()
    -> Curl_http_done()

MSTATE_COMPLETED
  -> handle_completed()
  -> 应用层可通过 curl_multi_info_read() 取完成消息
```

可以把这条状态机理解成：

```text
准备请求
-> 找连接或新建连接
-> DNS
-> TCP connect
-> HTTPS/WSS TLS handshake
-> HTTP 发请求
-> HTTP/HTTPS：收 HTTP 响应 body
-> WS/WSS：收到 101 后切换 WebSocket frame 收发
-> 结束和清理
-> 通知应用层
```

## 4. 连接建立调用链

连接建立从 `MSTATE_CONNECT` 开始：

```text
multi_runsingle()
  -> state_connect()
    -> Curl_connect()
```

`Curl_connect()` 做两件核心事情：

```text
Curl_connect()
  -> Curl_req_hard_reset()
  -> url_find_or_create_conn()
```

`url_find_or_create_conn()` 会尝试从 connection cache 里找可复用连接。如果命中：

```text
已有连接可复用
-> conn->bits.reuse = true
-> 不需要重新 DNS/TCP/TLS
-> 直接进入后续 HTTP 请求阶段
```

如果没有可复用连接，则创建新连接并设置 connection filter 链：

```text
Curl_connect()
  -> Curl_conn_setup()
    -> HTTPS 时 Curl_cf_https_setup()
    -> cf_setup_add()
    -> Curl_cf_dns_add()
```

connection filter 可以理解为一条网络 I/O 管线。HTTP/HTTPS/WS/WSS 常见层次大致是：

```text
DNS filter
  -> IP/Happy Eyeballs filter
    -> socket filter
      -> TLS filter     // HTTPS/WSS 才有，具体插入方式由 HTTPS filter setup 决定
```

后续 `Curl_conn_connect()` 会反复调用 filter 链的 connect 方法，每次只推进当前可推进的一小步；如果底层 socket/TLS 需要等待读写事件，就返回未完成，等下一次 `curl_multi_socket_action()` 或 `curl_multi_perform()` 再继续。

## 5. DNS 解析详细流程

### 5.1 DNS 入口

DNS 从 connection filter 链中的 DNS filter 开始：

```text
Curl_conn_connect()
  -> Curl_conn_cf_connect()
    -> cf_dns_connect()
      -> cf_dns_start()
        -> Curl_resolv()
```

`cf_dns_start()` 会先判断目标地址类型：

```text
如果 hostname 是 IPv4 字面量
  -> 只需要转换地址，不需要真正 DNS

如果 hostname 是 IPv6 字面量
  -> 只需要转换地址，不需要真正 DNS

如果 hostname 是 localhost / *.localhost
  -> 使用本地 localhost 解析逻辑

普通域名
  -> 进入 Curl_resolv()
```

### 5.2 DNS cache

`Curl_resolv()` 进入 `hostip_resolv()` 后，会先查 DNS cache：

```text
Curl_resolv()
  -> hostip_resolv()
    -> Curl_dnscache_get()
```

如果 cache 命中：

```text
Curl_dnscache_get()
  -> 返回 Curl_dns_entry
  -> cf_dns_connect() 标记 DNS 已完成
  -> Curl_pgrsTime(data, TIMER_NAMELOOKUP)
  -> 继续连接下一层 filter
```

如果 cache 中存在 negative DNS entry：

```text
Curl_dnscache_get()
  -> 返回解析失败
  -> 请求以 CURLE_COULDNT_RESOLVE_HOST / CURLE_COULDNT_RESOLVE_PROXY 结束
```

如果 cache 未命中，进入真正解析。

### 5.3 threaded resolver 启动

由于本仓库 `ENABLE_ARES=OFF` 且 `CURL_DISABLE_DOH=ON`，普通域名解析走 threaded resolver：

```text
hostip_resolv()
  -> hostip_resolv_start()
    -> hostip_async_new()
    -> Curl_async_getaddrinfo()
```

`Curl_async_getaddrinfo()` 会按需要投递 AAAA 和 A 查询：

```text
Curl_async_getaddrinfo()
  -> Curl_resolv_announce_start()
  -> async_thrdd_query(CURL_DNSQ_AAAA)   // IPv6 查询，启用 IPv6 时
  -> async_thrdd_query(CURL_DNSQ_A)      // IPv4 查询
```

每个查询会变成一个 DNS queue item：

```text
async_thrdd_query()
  -> async_thrdd_item_create()
  -> Curl_thrdq_send(data->multi->resolv_thrdq, item, ...)
```

如果查询被成功投递：

```text
hostip_resolv_start()
  -> 返回 CURLE_AGAIN
  -> async 对象挂到 data->state.async
  -> cf_dns_connect() 本轮不再继续 TCP connect
```

这时主线程没有阻塞在 DNS 上，请求留在 multi 状态机里，等待 DNS 线程返回结果。

### 5.4 DNS worker 线程执行 getaddrinfo

DNS 线程池由 multi handle 初始化：

```text
multi_init()
  -> Curl_async_thrdd_multi_init()
    -> Curl_thrdq_create(..., async_thrdd_item_process, ...)
```

worker 线程拿到 DNS item 后执行：

```text
async_thrdd_item_process()
  -> 准备 struct addrinfo hints
  -> Curl_getaddrinfo_ex(hostname, service, &hints, &item->res)
    -> system getaddrinfo()
  -> Curl_addrinfo_set_port()
```

其中：

- AAAA 查询使用 `PF_INET6`。
- A 查询使用 `PF_INET`。
- 如果一次查询同时允许 A/AAAA，可能使用 `PF_UNSPEC`。
- `socktype` 和 `protocol` 会根据传输类型设置，普通 HTTP/HTTPS 是 TCP。

### 5.5 主线程取 DNS 结果

multi 状态机每次运行时会处理 DNS 线程队列：

```text
multi_runsingle()
  -> Curl_async_thrdd_multi_process()
    -> Curl_thrdq_recv()
    -> Curl_multi_get_easy(multi, item->mid)
    -> Curl_async_get(data, item->resolv_id)
    -> 保存 A 或 AAAA 查询结果
    -> Curl_multi_mark_dirty(data)
```

`Curl_multi_mark_dirty()` 的作用是告诉 multi：这个 easy 又有进展了，需要尽快再跑一轮状态机。

DNS filter 后续再次被推进时：

```text
cf_dns_connect()
  -> Curl_resolv_take_result()
    -> hostip_resolv_take_result()
      -> Curl_async_take_result()
```

如果 A/AAAA 结果已经齐了，或者达到 Happy Eyeballs 所需的最低结果条件：

```text
Curl_async_take_result()
  -> Curl_dnscache_mk_entry2()
  -> Curl_dnscache_add()
  -> 返回 Curl_dns_entry
```

然后：

```text
cf_dns_connect()
  -> 标记 conn->bits.dns_resolved = true
  -> Curl_pgrsTime(data, TIMER_NAMELOOKUP)
  -> cf_dns_report()
  -> Curl_conn_cf_connect(cf->next)
```

### 5.6 DNS 和 Happy Eyeballs 的关系

curl 8.x 的 DNS filter 不一定等待所有 DNS 查询完全结束才开始连接。它会判断是否“足够开始连接”：

```text
cf_dns_ready_to_connect()
  -> 优先等待 AAAA 结果
  -> 如果超过短延迟仍未拿到期望结果，可以先用已有结果开始连接
```

本版本代码里有一个短延迟常量：

```text
CURL_HEV3_RESOLVE_DELAY_MS = 50
```

含义是：

- 优先给 IPv6/AAAA 一个很短的机会。
- 如果等待超过该延迟，允许使用已经拿到的地址继续尝试连接。
- 这和 Happy Eyeballs 的思想一致：不要因为单一地址族慢而拖住整体连接。

## 6. TCP 非阻塞 connect 流程

DNS 有结果后，filter 链继续进入下一层：

```text
cf_dns_connect()
  -> Curl_conn_cf_connect(cf->next)
    -> IP/Happy Eyeballs filter
      -> socket filter
```

socket filter 会创建 socket 并发起非阻塞连接：

```text
socket()
setsockopt()
non-blocking mode
connect()
```

如果 `connect()` 立即成功：

```text
Curl_conn_connect()
  -> connected = true
  -> multi 状态切到 MSTATE_PROTOCONNECT
```

如果 `connect()` 返回正在进行：

```text
connect() -> EINPROGRESS / WSAEWOULDBLOCK
  -> connected = false
  -> multi 状态切到 MSTATE_CONNECTING
  -> 告诉应用层关注 socket writable
```

应用层 event loop 收到 socket writable 后：

```text
curl_multi_socket_action(multi, sock, CURL_CSELECT_OUT, &running)
  -> multi_socket()
    -> multi_runsingle()
      -> Curl_conn_connect()
        -> 检查 connect 是否完成
```

connect 完成后：

```text
getsockopt(SO_ERROR)
  -> 0：连接成功
  -> 非 0：连接失败，尝试下一个地址或返回错误
```

如果同一域名有多个地址，Happy Eyeballs/IP filter 会在 IPv6、IPv4、多个 IP 之间调度连接尝试。最终：

```text
某个 IP connect 成功
  -> 选定该 socket
  -> 关闭或丢弃其他失败/未使用尝试
```

## 7. HTTPS TLS 握手流程

HTTP URL 不需要 TLS，TCP 连接成功后可以进入 HTTP 请求阶段。

HTTPS URL 在 TCP 连接成功后，还需要推进 TLS filter。由于本仓库 `CURL_USE_OPENSSL=ON`，TLS 后端是 OpenSSL。

调用链概念上是：

```text
MSTATE_PROTOCONNECT
  -> protocol_connect()
    -> Curl_conn_connect()
      -> TLS/HTTPS filter connect
        -> OpenSSL SSL_connect()
```

TLS 握手也是非阻塞的。OpenSSL 可能返回：

```text
SSL_ERROR_WANT_READ
  -> 需要等 socket readable

SSL_ERROR_WANT_WRITE
  -> 需要等 socket writable

握手成功
  -> TLS filter connected
```

握手期间会处理：

- ClientHello / ServerHello。
- SNI。
- 证书链验证。
- hostname 校验。
- ALPN 协商。
- TLS session 复用。

因为当前构建禁用了 HTTP/2，即使 ALPN 存在，常规 HTTP 请求最终也主要走 HTTP/1.x。

TLS 完成后：

```text
MSTATE_PROTOCONNECT / MSTATE_PROTOCONNECTING
  -> protocol_connected = true
  -> 状态切到 MSTATE_DO
```

## 8. HTTP 请求发送流程

进入 `MSTATE_DO` 后，HTTP 协议处理函数开始工作：

```text
multi_runsingle()
  -> state_do()
    -> Curl_http()
```

`Curl_http()` 负责构建 HTTP request：

```text
Curl_http()
  -> Curl_http_method()
  -> Curl_http_output_auth()      // 当前构建关闭 HTTP_AUTH，认证分支基本不可用
  -> 添加 Host / User-Agent / Accept / Content-Length / Transfer-Encoding 等头
  -> 处理 range、expect、upload 等 HTTP 选项
  -> Curl_req_send()
```

当前构建关闭了 proxy、cookie、HTTP auth、MIME/form、netrc 等扩展，所以文档里的 HTTP 主链路按“直连目标 host、无代理、无 cookie、无认证”的请求理解。

对于 GET 请求，典型发送内容类似：

```http
GET /path HTTP/1.1
Host: example.com
Accept: */*

```

对于 POST/PUT/upload，还会继续读取请求 body：

```text
Curl_req_send()
  -> 如果请求头能直接发完：
       xfer_send()
     否则：
       放入 data->req.sendbuf
       Curl_req_send_more()
```

底层发送链路：

```text
Curl_req_send()
  -> xfer_send()
    -> Curl_conn_send()
      -> connection filter send
        -> HTTPS: SSL_write()
        -> HTTP:  send()
```

如果 socket 当前不可写：

```text
send()/SSL_write() 返回 would-block
  -> 保留未发送数据
  -> 等 socket writable
  -> 下一轮 Curl_sendrecv() 中继续 Curl_req_send_more()
```

如果有 request body：

```text
Curl_req_send_more()
  -> add_from_client()
    -> 调用 CURLOPT_READFUNCTION
  -> req_flush()
    -> Curl_conn_send()
```

发送完成后，状态机会继续进入响应接收阶段。

## 9. HTTP 响应接收流程

进入 `MSTATE_PERFORMING` 后，核心收发函数是：

```text
multi_runsingle()
  -> state_performing()
    -> Curl_sendrecv()
```

`Curl_sendrecv()` 同时负责两件事：

- 如果还有请求数据没发完，继续发送。
- 如果 socket 可读，读取响应数据。

接收调用链：

```text
Curl_sendrecv()
  -> xfer_recv_resp()
    -> Curl_conn_recv()
      -> connection filter recv
        -> HTTPS: SSL_read()
        -> HTTP:  recv()
    -> Curl_xfer_write_resp()
      -> Curl_http_write_resp()
```

HTTP/1.x 响应解析：

```text
Curl_http_write_resp()
  -> Curl_http_write_resp_hds()
    -> 解析 status line
    -> 解析 header line
    -> Curl_client_write(CLIENTWRITE_HEADER)
       -> CURLOPT_HEADERFUNCTION

  -> header 结束后处理 body
    -> Curl_client_write(CLIENTWRITE_BODY)
       -> CURLOPT_WRITEFUNCTION
```

响应头处理期间会识别：

- HTTP status code。
- `Content-Length`。
- `Transfer-Encoding: chunked`。
- `Connection: close`。
- `Location` 重定向。
- `WWW-Authenticate` / `Proxy-Authenticate`，但当前构建关闭 HTTP auth/proxy，因此不作为主链路处理。
- `Content-Encoding`，但当前构建关闭 zlib/brotli/zstd，因此不做这些压缩格式的自动解码。
- `Set-Cookie`，但当前构建关闭 cookie，因此不作为主链路处理。

body 结束条件：

- `Content-Length` 指定的字节数已经收满。
- chunked 编码读到 last chunk。
- 响应本来就没有 body，例如 HEAD、204、304。
- 服务端关闭连接，并且协议允许用 close 表示 body 结束。
- 出现错误、超时、回调中止。

当 `CURLOPT_WRITEFUNCTION` 返回值不等于 libcurl 传入的数据长度时，libcurl 会认为业务侧写入失败，请求通常以 `CURLE_WRITE_ERROR` 结束。

## 10. WebSocket Upgrade 和 frame 收发流程

`ws://` 和 `wss://` 并不是独立于 HTTP 的建连流程。它们的前半段复用 HTTP/HTTPS：

```text
ws://
  -> DNS
  -> TCP
  -> HTTP/1.1 GET + Upgrade: websocket
  -> 101 Switching Protocols
  -> WebSocket frame

wss://
  -> DNS
  -> TCP
  -> TLS/OpenSSL
  -> HTTP/1.1 GET + Upgrade: websocket
  -> 101 Switching Protocols
  -> WebSocket frame over TLS
```

### 10.1 WebSocket 强制走 HTTP/1.x

curl 的 WebSocket protocol setup 会限制 HTTP 版本：

```text
ws_setup_conn()
  -> data->state.http_neg.wanted = CURL_HTTP_V1x
  -> data->state.http_neg.allowed = CURL_HTTP_V1x
  -> Curl_http_setup_conn()
```

这和本仓库 `USE_NGHTTP2=OFF` 一致：WebSocket Upgrade 走 HTTP/1.1。

### 10.2 WebSocket Upgrade 请求生成

WS/WSS 在 `MSTATE_DO` 阶段仍然进入 HTTP 发送函数：

```text
MSTATE_DO
  -> state_do()
    -> Curl_http()
```

`Curl_http()` 组装 HTTP/1.1 请求头时，如果当前协议是 `CURLPROTO_WS` 或 `CURLPROTO_WSS`，会追加 WebSocket Upgrade 头：

```text
Curl_http()
  -> H1_HD_UPGRADE
    -> Curl_ws_request()
```

`Curl_ws_request()` 主要做：

```text
Curl_ws_request()
  -> Curl_rand() 生成 16 字节随机数
  -> base64 编码生成 Sec-WebSocket-Key
  -> 添加 Upgrade: websocket
  -> 添加 Sec-WebSocket-Version: 13
  -> 添加 Sec-WebSocket-Key: <base64 nonce>
  -> data->state.http_hd_upgrade = TRUE
  -> data->req.upgr101 = UPGR101_WS
  -> conn->bits.upgrade_in_progress = TRUE
```

最终发出的请求形态类似：

```http
GET /socket HTTP/1.1
Host: example.com
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Version: 13
Sec-WebSocket-Key: <random-base64>

```

底层发送仍然走普通 HTTP request 发送链：

```text
Curl_req_send()
  -> xfer_send()
    -> Curl_conn_send()
      -> ws://  send()
      -> wss:// SSL_write()
```

### 10.3 101 响应和协议切换

服务端正确接受 WebSocket 时返回：

```http
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: ...

```

libcurl 接收响应头的路径仍然是 HTTP：

```text
MSTATE_PERFORMING
  -> Curl_sendrecv()
    -> xfer_recv_resp()
      -> Curl_conn_recv()
      -> Curl_xfer_write_resp()
        -> Curl_http_write_resp()
          -> Curl_http_write_resp_hds()
```

当解析到 `101 Switching Protocols`：

```text
Curl_http_write_resp_hds()
  -> http_on_101_upgrade()
    -> 确认当前发送的是 HTTP/1.1
    -> 确认本次请求期望 UPGR101_WS
    -> Curl_ws_accept()
```

`Curl_ws_accept()` 完成真正的协议切换：

```text
Curl_ws_accept()
  -> 创建或重置 struct websocket
  -> 初始化 ws->recvbuf / ws->sendbuf
  -> 初始化 WebSocket decoder / encoder
  -> 把 websocket 对象挂到 connection metadata
  -> 创建 ws_cw_decode client writer
  -> Curl_cwriter_add()
  -> k->header = FALSE
  -> k->upgr101 = UPGR101_RECEIVED
```

如果 101 响应头后面同一个网络包里已经带了 WebSocket frame 数据，`Curl_ws_accept(data, buf, blen)` 会继续处理这部分剩余字节：

- callback 模型：直接把剩余数据送进 `Curl_client_write(CLIENTWRITE_BODY, ...)`，进入 WebSocket decoder。
- connect-only 模型：先写入 `ws->recvbuf`，等业务后续调用 `curl_ws_recv()` 时读取。

如果服务端没有返回 101，而是返回 200/301/4xx 等普通 HTTP 响应，WebSocket upgrade 失败，本次 transfer 会作为错误结束。

### 10.4 callback 模型接收 WebSocket frame

callback 模型下，`CURLOPT_CONNECT_ONLY` 不设置或为 `0L`。101 成功后，请求不会立即交还给业务，而是继续停留在 multi 状态机中接收 WebSocket 数据：

```text
socket readable
  -> curl_multi_socket_action()
    -> multi_runsingle()
      -> MSTATE_PERFORMING
        -> Curl_sendrecv()
          -> Curl_conn_recv()
            -> ws://  recv()
            -> wss:// SSL_read()
          -> Curl_xfer_write_resp()
            -> Curl_http_write_resp()
              -> WebSocket client writer
                -> ws_cw_write()
                  -> ws_dec_pass()
                    -> ws_cw_dec_next()
                      -> Curl_client_write(CLIENTWRITE_BODY)
                        -> CURLOPT_WRITEFUNCTION
```

`ws_dec_pass()` 会解析 WebSocket frame header 和 payload：

- TEXT。
- BINARY。
- CONT。
- CLOSE。
- PING。
- PONG。

非 raw mode 下，业务的 `CURLOPT_WRITEFUNCTION` 收到的是解码后的 payload，不包含 WebSocket frame header。回调中可以调用：

```cpp
const struct curl_ws_frame *meta = curl_ws_meta(easy);
```

获取当前 frame 的元信息：

- `flags`：TEXT/BINARY/CONT/CLOSE/PING 等。
- `offset`：当前数据在 frame payload 中的偏移。
- `bytesleft`：当前 frame 剩余 payload 字节数。
- `len`：本次回调收到的 payload 长度。

如果收到 PING，默认会自动生成 PONG；设置 `CURLOPT_WS_OPTIONS` 的 `CURLWS_NOAUTOPONG` 后，自动 PONG 会关闭，需要业务自己处理。

### 10.5 connect-only 模型接收 WebSocket frame

connect-only 模型必须设置：

```cpp
curl_easy_setopt(easy, CURLOPT_CONNECT_ONLY, 2L);
```

此时 multi 状态机只跑到 WebSocket 握手完成：

```text
DNS/TCP/TLS
  -> HTTP Upgrade request
  -> 101 response
  -> Curl_ws_accept()
  -> MSTATE_DONE
  -> CURLMSG_DONE
```

握手完成后，应用层主动调用：

```cpp
size_t nrecv = 0;
const struct curl_ws_frame *meta = nullptr;
CURLcode rc = curl_ws_recv(easy, buffer, buflen, &nrecv, &meta);
```

内部接收链：

```text
curl_ws_recv()
  -> 找到 connect-only connection
  -> 从 connection metadata 取 struct websocket
  -> 如果 ws->recvbuf 为空：
       Curl_bufq_slurp()
         -> curl_easy_recv()
           -> Curl_conn_recv()
             -> ws://  recv()
             -> wss:// SSL_read()
  -> ws_dec_pass()
  -> ws_client_collect()
  -> update_meta()
  -> 返回 payload 和 curl_ws_frame
```

如果 `curl_ws_recv()` 触发了自动 PONG，它会尝试把 pending control frame 写入发送缓冲并 flush。

### 10.6 WebSocket frame 发送

业务发送 WebSocket 数据主要用：

```cpp
size_t nsent = 0;
CURLcode rc = curl_ws_send(easy,
                           data,
                           data_len,
                           &nsent,
                           0,
                           CURLWS_TEXT);
```

非 raw mode 下的发送链：

```text
curl_ws_send()
  -> 从 connection metadata 取 struct websocket
  -> ws_enc_send()
    -> ws_enc_write_head()
      -> 写入 WebSocket frame header
    -> ws_enc_write_payload()
      -> 写入 payload
    -> ws_flush()
      -> Curl_xfer_send() / Curl_senddata()
        -> Curl_conn_send()
          -> ws://  send()
          -> wss:// SSL_write()
```

发送支持的典型 flags：

- `CURLWS_TEXT`。
- `CURLWS_BINARY`。
- `CURLWS_CONT`。
- `CURLWS_CLOSE`。
- `CURLWS_PING`。
- `CURLWS_PONG`。

如果业务要先声明一个指定长度的 frame，再分段写 payload，可用：

```cpp
curl_ws_start_frame(easy, CURLWS_BINARY, frame_len);
```

后续再继续写入 payload。libcurl 会检查上一个 frame 是否已经写完，未写完时不允许开始新 frame。

### 10.7 raw mode

设置：

```cpp
curl_easy_setopt(easy, CURLOPT_WS_OPTIONS, CURLWS_RAW_MODE);
```

raw mode 下：

- 接收侧：libcurl 不解析 WebSocket frame，网络数据原样交给业务。
- 发送侧：`curl_ws_send()` 不做 frame 编码，要求 `fragsize=0` 且 `flags=0`。
- 适合项目里已经有自己的 WebSocket parser/encoder，只想复用 curl 的 DNS/TCP/TLS/HTTP Upgrade 能力。

### 10.8 WS/WSS 与普通 HTTP 的分叉点

WS/WSS 和 HTTP/HTTPS 的链路在 101 前完全共用：

```text
DNS
-> TCP
-> wss/https 时 TLS
-> Curl_http()
-> Curl_req_send()
-> Curl_http_write_resp_hds()
```

分叉点是 HTTP response：

```text
普通 HTTP/HTTPS:
  2xx/3xx/4xx/5xx
  -> 继续按 HTTP header/body 处理
  -> body 完成后 MSTATE_DONE

WS/WSS:
  101 Switching Protocols
  -> http_on_101_upgrade()
  -> Curl_ws_accept()
  -> 后续按 WebSocket frame 处理
```

## 11. 完成、复用和清理

不同协议的“完成”含义不同：

- HTTP/HTTPS：响应 body 读完后完成。
- WS/WSS callback 模型：101 成功后连接会继续用于 WebSocket frame 流，通常直到对端关闭、业务中止或出错才完成。
- WS/WSS connect-only 模型：101 成功并完成 `Curl_ws_accept()` 后 transfer 就完成，但连接仍保留给应用层后续 `curl_ws_send()` / `curl_ws_recv()` 使用。

HTTP/HTTPS 或 WS callback 结束时：

```text
Curl_sendrecv()
  -> data->req.done = true
```

状态机进入：

```text
MSTATE_DONE
  -> multi_done()
    -> Curl_http_done()
```

`Curl_http_done()` 和 `multi_done()` 会处理：

- HTTP 协议尾部逻辑。
- 重定向、重试判断。当前构建关闭 HTTP auth/proxy/cookie，因此相关分支不作为主链路。
- 请求/响应状态清理。
- WebSocket callback 模型结束时清理 WebSocket writer/reader 状态。
- DNS entry 引用释放。
- socket 是否保持连接。
- 连接是否放回 connection cache。
- 失败时是否关闭连接。

之后进入：

```text
MSTATE_COMPLETED
  -> handle_completed()
  -> 生成 CURLMSG_DONE
```

应用层读取完成消息。HTTP/HTTPS 完成后通常可以立即 remove/cleanup；WebSocket connect-only 模型下，收到握手完成的 `CURLMSG_DONE` 后不要立刻 cleanup，除非业务已经不再调用 `curl_ws_send()` / `curl_ws_recv()`：

```cpp
int msgs_left = 0;
while(CURLMsg *msg = curl_multi_info_read(multi, &msgs_left)) {
  if(msg->msg == CURLMSG_DONE) {
    CURL *easy = msg->easy_handle;
    CURLcode result = msg->data.result;

    long http_code = 0;
    curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &http_code);

    void *private_data = nullptr;
    curl_easy_getinfo(easy, CURLINFO_PRIVATE, &private_data);

    curl_multi_remove_handle(multi, easy);
    curl_easy_cleanup(easy);
  }
}
```

如果 HTTP/HTTPS 连接可复用：

```text
Connection: keep-alive
响应体完整读完
没有协议错误
没有要求关闭连接
  -> connection 放回 conncache
```

WebSocket 升级后的连接已经不再是普通 HTTP request/response 连接，不能作为普通 HTTP keep-alive 连接复用；它要么继续承载 WebSocket frame，要么关闭。

下一次 HTTP/HTTPS 请求如果 host、port、scheme、SSL 配置等匹配：

```text
url_find_or_create_conn()
  -> 找到可复用连接
  -> 跳过 DNS/TCP/TLS
  -> 直接进入 HTTP 请求发送
```

## 12. 错误和超时路径

常见错误阶段：

### 12.1 DNS 错误

```text
Curl_resolv()
  -> hostip_resolv()
    -> Curl_async_take_result()
      -> 没有 A/AAAA 结果
```

结果通常是：

- `CURLE_COULDNT_RESOLVE_HOST`
- `CURLE_COULDNT_RESOLVE_PROXY`

### 12.2 TCP 连接错误

```text
connect()
  -> EINPROGRESS
  -> 后续 getsockopt(SO_ERROR) 非 0
```

结果通常是：

- `CURLE_COULDNT_CONNECT`
- `CURLE_OPERATION_TIMEDOUT`

### 12.3 TLS 错误

```text
SSL_connect()
  -> 证书验证失败
  -> hostname 不匹配
  -> TLS 协议错误
```

结果可能是：

- `CURLE_SSL_CONNECT_ERROR`
- `CURLE_PEER_FAILED_VERIFICATION`
- `CURLE_SSL_CACERT_BADFILE`

### 12.4 HTTP/WS 收发错误

WebSocket upgrade 失败：

```text
ws:// 或 wss://
  -> 已发送 Upgrade: websocket
  -> 服务端没有返回 101 Switching Protocols
  -> http_on_101_upgrade() 不会切到 WebSocket
```

结果通常是：

- `CURLE_HTTP_RETURNED_ERROR`
- `CURLE_WEIRD_SERVER_REPLY`

普通收发错误：

```text
send()/recv()/SSL_read()/SSL_write()
  -> socket 错误
```

或：

```text
CURLOPT_WRITEFUNCTION 返回长度不匹配
```

结果可能是：

- `CURLE_SEND_ERROR`
- `CURLE_RECV_ERROR`
- `CURLE_WRITE_ERROR`
- `CURLE_GOT_NOTHING`

### 12.5 超时

multi 状态机会统一检查超时：

```text
multi_runsingle()
  -> multi_handle_timeout()
```

主要超时包括：

- `CURLOPT_TIMEOUT_MS`：整个请求总时间。
- `CURLOPT_CONNECTTIMEOUT_MS`：连接阶段总时间，覆盖 DNS、TCP、TLS。
- DNS threaded resolver 自身也会根据 async timeout 触发 `EXPIRE_ASYNC_NAME`。

## 13. 完整调用链汇总

一次没有连接复用的 `http/https/ws/wss` 请求，公共前缀可以压缩成下面这条链：

```text
应用层
  curl_easy_setopt()
    -> URL = http:// / https:// / ws:// / wss://
    -> callbacks / timeout / optional WS options
  curl_multi_add_handle()
  curl_multi_socket_action() / curl_multi_perform()

multi 状态机
  multi_runsingle()
  MSTATE_INIT
  MSTATE_SETUP
  MSTATE_CONNECT

连接准备
  state_connect()
  Curl_connect()
  url_find_or_create_conn()
  Curl_conn_setup()
  Curl_cf_dns_add()

DNS
  Curl_conn_connect()
  Curl_conn_cf_connect()
  cf_dns_connect()
  cf_dns_start()
  Curl_resolv()
  hostip_resolv()
  Curl_dnscache_get()
  hostip_resolv_start()
  Curl_async_getaddrinfo()
  async_thrdd_query(A/AAAA)
  Curl_thrdq_send()

DNS worker
  async_thrdd_item_process()
  Curl_getaddrinfo_ex()
  getaddrinfo()

DNS result
  Curl_async_thrdd_multi_process()
  Curl_resolv_take_result()
  Curl_async_take_result()
  Curl_dnscache_mk_entry2()
  Curl_dnscache_add()

TCP
  cf_dns_connect()
  Curl_conn_cf_connect(next)
  socket()
  non-blocking connect()
  MSTATE_CONNECTING
  socket writable
  getsockopt(SO_ERROR)
```

`http://` 和 `ws://` 在 TCP 成功后直接进入 HTTP/1.x；`https://` 和 `wss://` 还会先走 TLS：

```text
TLS for https/wss
  MSTATE_PROTOCONNECT
  protocol_connect()
  TLS/OpenSSL filter
  SSL_connect()
  SSL_ERROR_WANT_READ / SSL_ERROR_WANT_WRITE
  certificate / hostname verification
  TLS handshake done
```

之后进入 HTTP 协议阶段。普通 HTTP/HTTPS 分支：

```text
HTTP/HTTPS request
  MSTATE_DO
  state_do()
  Curl_http()
  Curl_http_method()
  Curl_req_send()
  xfer_send()
  Curl_conn_send()
  https: SSL_write()
  http:  send()

HTTP/HTTPS response
  MSTATE_PERFORMING
  state_performing()
  Curl_sendrecv()
  xfer_recv_resp()
  Curl_conn_recv()
  https: SSL_read()
  http:  recv()
  Curl_xfer_write_resp()
  Curl_http_write_resp()
  Curl_http_write_resp_hds()
  Curl_client_write(HEADER/BODY)
  CURLOPT_HEADERFUNCTION / CURLOPT_WRITEFUNCTION

完成
  MSTATE_DONE
  multi_done()
  Curl_http_done()
  MSTATE_COMPLETED
  handle_completed()
  curl_multi_info_read()
  curl_multi_remove_handle()
  curl_easy_cleanup()
```

WS/WSS 分支在 HTTP request 阶段追加 Upgrade，并在 HTTP response 阶段分叉：

```text
WS/WSS Upgrade request
  MSTATE_DO
  state_do()
  Curl_http()
  H1_HD_UPGRADE
  Curl_ws_request()
    -> Upgrade: websocket
    -> Sec-WebSocket-Version: 13
    -> Sec-WebSocket-Key: <random-base64>
    -> req.upgr101 = UPGR101_WS
  Curl_req_send()
  Curl_conn_send()
  wss: SSL_write()
  ws:  send()

WS/WSS 101 response
  MSTATE_PERFORMING
  Curl_sendrecv()
  Curl_conn_recv()
  wss: SSL_read()
  ws:  recv()
  Curl_http_write_resp()
  Curl_http_write_resp_hds()
  http_on_101_upgrade()
  Curl_ws_accept()
    -> create struct websocket
    -> init recvbuf/sendbuf
    -> init decoder/encoder
    -> install ws_cw_decode writer
    -> mark UPGR101_RECEIVED
```

WS/WSS callback 模型后续收发：

```text
Receive frame
  socket readable
  curl_multi_socket_action()
  MSTATE_PERFORMING
  Curl_sendrecv()
  Curl_conn_recv()
  Curl_http_write_resp()
  ws_cw_write()
  ws_dec_pass()
  Curl_client_write(CLIENTWRITE_BODY)
  CURLOPT_WRITEFUNCTION
  curl_ws_meta() 可读取 frame metadata

Send frame
  curl_ws_send()
  ws_enc_send()
  ws_enc_write_head()
  ws_enc_write_payload()
  ws_flush()
  Curl_xfer_send() / Curl_senddata()
  Curl_conn_send()
```

WS/WSS connect-only 模型后续收发：

```text
Handshake transfer
  CURLOPT_CONNECT_ONLY = 2L
  DNS/TCP/TLS/HTTP Upgrade/101
  Curl_ws_accept()
  MSTATE_DONE
  CURLMSG_DONE

Application receive
  curl_ws_recv()
  curl_easy_recv()
  Curl_conn_recv()
  ws_dec_pass()
  return payload + curl_ws_frame

Application send
  curl_ws_send()
  ws_enc_send()
  ws_flush()
  Curl_senddata()
  Curl_conn_send()
```

## 14. 调试建议

### 14.1 打开 verbose

```cpp
curl_easy_setopt(easy, CURLOPT_VERBOSE, 1L);
```

可以看到：

- DNS cache 命中情况。
- connect 到哪个 IP。
- TLS 握手摘要。
- HTTP request/response header。
- WebSocket Upgrade 请求头和 `101 Switching Protocols`。
- WebSocket 建立后的 `[WS]` trace 信息，如果构建和运行日志启用了对应 verbose trace。

### 14.2 设置 debug callback

```cpp
curl_easy_setopt(easy, CURLOPT_DEBUGFUNCTION, debug_callback);
curl_easy_setopt(easy, CURLOPT_DEBUGDATA, user_data);
```

可以把 libcurl 的 text/header/data 日志接入项目日志系统。

### 14.3 关注关键 getinfo 指标

请求完成后可读取：

```cpp
curl_easy_getinfo(easy, CURLINFO_NAMELOOKUP_TIME_T, &name_us);
curl_easy_getinfo(easy, CURLINFO_CONNECT_TIME_T, &connect_us);
curl_easy_getinfo(easy, CURLINFO_APPCONNECT_TIME_T, &tls_us);
curl_easy_getinfo(easy, CURLINFO_PRETRANSFER_TIME_T, &pretransfer_us);
curl_easy_getinfo(easy, CURLINFO_STARTTRANSFER_TIME_T, &ttfb_us);
curl_easy_getinfo(easy, CURLINFO_TOTAL_TIME_T, &total_us);
curl_easy_getinfo(easy, CURLINFO_PRIMARY_IP, &primary_ip);
curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &http_code);
```

这些指标能对应到调用链阶段：

- `NAMELOOKUP`：DNS。
- `CONNECT`：TCP connect 完成。
- `APPCONNECT`：TLS 握手完成。
- `PRETRANSFER`：准备发送 HTTP request。
- `STARTTRANSFER`：收到第一个响应字节。
- `TOTAL`：整个请求完成。

## 15. 源码定位

本仓库中最相关的源码位置：

- `3rd/3rd-cmake/curl.cmake`：curl 构建能力开关。
- `3rd/curl/lib/multi.c`：multi 状态机、`curl_multi_perform()`、`curl_multi_socket_action()`。
- `3rd/curl/lib/url.c`：`Curl_connect()`、连接查找和创建。
- `3rd/curl/lib/connect.c`：connection filter 链 setup。
- `3rd/curl/lib/cfilters.c`：filter 链 connect/send/recv 调度。
- `3rd/curl/lib/cf-dns.c`：DNS filter。
- `3rd/curl/lib/hostip.c`：DNS cache、`Curl_resolv()`、resolver 调度。
- `3rd/curl/lib/asyn-thrdd.c`：threaded resolver、DNS 线程队列、`getaddrinfo()` 执行。
- `3rd/curl/lib/cf-socket.c`：socket connect/send/recv。
- `3rd/curl/lib/vtls/openssl.c`：OpenSSL TLS 握手和读写。
- `3rd/curl/lib/http.c`：HTTP/1.x 请求构建、响应解析。
- `3rd/curl/lib/ws.c`：WebSocket Upgrade 请求、101 接受、frame 编解码、`curl_ws_send()` / `curl_ws_recv()`。
- `3rd/curl/lib/ws.h`：WebSocket 内部接口声明。
- `3rd/curl/include/curl/websockets.h`：公开 WebSocket API 和 `curl_ws_frame`。
- `3rd/curl/lib/request.c`：request send buffer、upload/read callback。
- `3rd/curl/lib/transfer.c`：`Curl_sendrecv()` 收发循环。
- `3rd/curl/lib/sendf.c`：`Curl_client_write()`，最终调用用户 header/body callback。
- `3rd/curl/docs/libcurl/libcurl-ws.md`：libcurl WebSocket callback/connect-only/raw 模型说明。
