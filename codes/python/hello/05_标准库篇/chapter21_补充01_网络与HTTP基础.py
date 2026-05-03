# =============================================================================
# 第 21 章：补充 1 - 网络与 HTTP 基础
# =============================================================================
#
# 【学习目标】
#   1. 理解 URL、HTTP 请求和 HTTP 响应的基本组成
#   2. 掌握 urllib.parse 处理 URL 和查询参数
#   3. 掌握 urllib.request 发送 HTTP 请求的基础用法
#   4. 了解 socket TCP 通信的基本流程
#   5. 能用 http.server 启动一个本地 HTTP 服务
#   6. 理解网络编程中的超时、状态码、请求头和异常处理
#
# 【运行方式】
#   python chapter21_补充01_网络与HTTP基础.py
#
# =============================================================================

import json
import socket
import threading
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from typing import Any
from urllib import error, parse, request


# =============================================================================
# 21.补充1.1 URL 解析与构造
# =============================================================================


def demo_url_parsing() -> None:
    """演示 URL 解析、查询参数读取和 URL 构造。"""
    print("=" * 60)
    print("21.补充1.1 URL 解析与构造")
    print("=" * 60)

    url = "https://api.example.com/users?page=2&size=20&keyword=python"
    parts = parse.urlparse(url)

    print("原始 URL:", url)
    print("协议:", parts.scheme)
    print("主机:", parts.netloc)
    print("路径:", parts.path)
    print("查询字符串:", parts.query)

    query = parse.parse_qs(parts.query)
    print("查询参数:", query)

    new_query = parse.urlencode({"page": 1, "size": 10, "keyword": "网络 编程"})
    new_url = parse.urlunparse(("https", "api.example.com", "/search", "", new_query, ""))
    print("构造 URL:", new_url)


# =============================================================================
# 21.补充1.2 HTTP 请求对象
# =============================================================================


def build_json_request(url: str, payload: dict[str, Any]) -> request.Request:
    """构造一个 JSON POST 请求对象。

    这里只构造 Request，不真正访问外部网络，便于离线学习和测试。
    """
    data = json.dumps(payload, ensure_ascii=False).encode("utf-8")
    return request.Request(
        url,
        data=data,
        method="POST",
        headers={
            "Content-Type": "application/json; charset=utf-8",
            "Accept": "application/json",
            "User-Agent": "python-learning/1.0",
        },
    )


def demo_http_request_object() -> None:
    """演示 urllib.request.Request 的常用字段。"""
    print("\n" + "=" * 60)
    print("21.补充1.2 HTTP 请求对象")
    print("=" * 60)

    req = build_json_request(
        "https://api.example.com/login",
        {"username": "alice", "remember": True},
    )

    print("请求方法:", req.get_method())
    print("请求地址:", req.full_url)
    print("请求头 Content-Type:", req.headers.get("Content-type") or req.headers.get("Content-Type"))
    print("请求体 bytes:", req.data)


# =============================================================================
# 21.补充1.3 本地 HTTP 服务与客户端请求
# =============================================================================


class JsonHandler(BaseHTTPRequestHandler):
    """只用于本章演示的本地 HTTP 处理器。"""

    def do_GET(self) -> None:
        """处理 GET 请求并返回 JSON。"""
        path = parse.urlparse(self.path)

        if path.path == "/health":
            body = {"status": "ok", "path": path.path}
            self._send_json(200, body)
            return

        if path.path == "/echo":
            body = {"query": parse.parse_qs(path.query)}
            self._send_json(200, body)
            return

        self._send_json(404, {"error": "not found"})

    def log_message(self, format: str, *args: Any) -> None:
        """关闭默认访问日志，避免教学输出被干扰。"""

    def _send_json(self, status: int, body: dict[str, Any]) -> None:
        data = json.dumps(body, ensure_ascii=False).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json; charset=utf-8")
        self.send_header("Content-Length", str(len(data)))
        self.end_headers()
        self.wfile.write(data)


def fetch_json(url: str, timeout: float = 3.0) -> dict[str, Any]:
    """请求 JSON 接口并返回解析后的 dict。"""
    with request.urlopen(url, timeout=timeout) as resp:
        charset = resp.headers.get_content_charset() or "utf-8"
        text = resp.read().decode(charset)
        return json.loads(text)


def demo_local_http_server() -> None:
    """启动本地 HTTP 服务，并用 urllib.request 访问它。"""
    print("\n" + "=" * 60)
    print("21.补充1.3 本地 HTTP 服务与客户端请求")
    print("=" * 60)

    server = ThreadingHTTPServer(("127.0.0.1", 0), JsonHandler)
    host, port = server.server_address

    thread = threading.Thread(target=server.serve_forever, daemon=True)
    thread.start()

    try:
        health = fetch_json(f"http://{host}:{port}/health")
        echo = fetch_json(f"http://{host}:{port}/echo?name=Alice&skill=Python")
        print("/health 响应:", health)
        print("/echo 响应:", echo)
    finally:
        server.shutdown()
        server.server_close()
        thread.join(timeout=2)


# =============================================================================
# 21.补充1.4 socket TCP 基础
# =============================================================================


def start_echo_server() -> tuple[socket.socket, threading.Thread]:
    """启动一个本地 TCP echo server。"""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(("127.0.0.1", 0))
    server.listen(1)

    def serve_once() -> None:
        conn, _addr = server.accept()
        with conn:
            data = conn.recv(1024)
            conn.sendall(b"echo: " + data)

    thread = threading.Thread(target=serve_once, daemon=True)
    thread.start()
    return server, thread


def demo_socket_tcp() -> None:
    """演示 TCP 服务端和客户端的最小通信流程。"""
    print("\n" + "=" * 60)
    print("21.补充1.4 socket TCP 基础")
    print("=" * 60)

    server, thread = start_echo_server()
    host, port = server.getsockname()

    try:
        with socket.create_connection((host, port), timeout=3) as client:
            client.sendall("hello socket".encode("utf-8"))
            data = client.recv(1024)
            print("客户端收到:", data.decode("utf-8"))
    finally:
        server.close()
        thread.join(timeout=2)


# =============================================================================
# 21.补充1.5 网络异常处理
# =============================================================================


def classify_http_status(status: int) -> str:
    """按 HTTP 状态码分类。"""
    if 200 <= status < 300:
        return "success"
    if 300 <= status < 400:
        return "redirect"
    if 400 <= status < 500:
        return "client_error"
    if 500 <= status < 600:
        return "server_error"
    return "unknown"


def demo_error_handling() -> None:
    """演示网络编程中常见异常和状态码分类。"""
    print("\n" + "=" * 60)
    print("21.补充1.5 网络异常处理")
    print("=" * 60)

    for status in [200, 301, 404, 500]:
        print(f"HTTP {status}: {classify_http_status(status)}")

    print("\n常见异常类型：")
    print("  URLError: DNS 失败、连接失败、超时等底层网络错误")
    print("  HTTPError: 服务端返回 4xx/5xx，urllib 会把它作为异常抛出")
    print("  TimeoutError/socket.timeout: 超时，应设置 timeout 并做降级处理")

    try:
        raise error.URLError("演示：连接失败")
    except error.URLError as exc:
        print("捕获 URLError:", exc.reason)


# =============================================================================
# 21.补充1.6 HTTP 客户端请求（urllib.request）
# =============================================================================
#
# 本节要点：
#   - urllib.request.urlopen() 发送 GET / POST 请求
#   - 读取响应的状态码、响应头、响应体
#   - 使用 urllib.error.HTTPError / URLError 做异常处理
#   - 了解第三方库 requests（比 urllib 更简洁，建议实际项目使用）
#   - 实战：用 urllib 访问 httpbin.org 公开测试接口


def demo_http_client() -> None:
    """演示 urllib.request 的 GET/POST 请求、响应解析和异常处理。

    与前面章节不同，本节面向真实的外部 HTTP 服务（httpbin.org），
    把请求构造、响应读取、错误处理串联成一个完整的客户端示例。
    """
    print("\n" + "=" * 60)
    print("21.补充1.6 HTTP 客户端请求（urllib.request）")
    print("=" * 60)

    base = "https://httpbin.org"

    # ── 1. GET 请求 ──────────────────────────────────────────────────────
    print("─" * 60)
    print("1. GET 请求 —— 读取状态码、响应头和响应体")
    print("─" * 60)

    get_url = f"{base}/get?name=Python&limit=5"
    try:
        with request.urlopen(get_url, timeout=10) as resp:
            print(f"  状态码: {resp.status} {resp.reason}")
            print(f"  Content-Type: {resp.headers.get('Content-Type')}")
            print(f"  响应头数量: {len(resp.headers)}")
            body = resp.read().decode("utf-8")
            data = json.loads(body)
            # 只展示 args 字段，避免输出太长
            print(f"  返回的查询参数: {data.get('args', {})}")
    except error.URLError as exc:
        print(f"  [跳过] GET 请求失败: {exc.reason}")

    # ── 2. POST 请求 —— 发送 JSON 数据 ──────────────────────────────────
    print("\n" + "─" * 60)
    print("2. POST 请求 —— 发送 JSON 数据")
    print("─" * 60)

    post_url = f"{base}/post"
    payload = json.dumps({"username": "alice", "role": "engineer"}, ensure_ascii=False)
    post_req = request.Request(
        post_url,
        data=payload.encode("utf-8"),
        method="POST",
        headers={
            "Content-Type": "application/json; charset=utf-8",
            "User-Agent": "python-urllib-demo/3.11",
        },
    )
    try:
        with request.urlopen(post_req, timeout=10) as resp:
            result = json.loads(resp.read().decode("utf-8"))
            print(f"  状态码: {resp.status}")
            print(f"  服务端回显的 JSON: {result.get('json', {})}")
    except error.URLError as exc:
        print(f"  [跳过] POST 请求失败: {exc.reason}")

    # ── 3. 异常处理 —— HTTP 4xx/5xx 与网络错误 ──────────────────────────
    print("\n" + "─" * 60)
    print("3. 异常处理 —— HTTPError 与 URLError")
    print("─" * 60)

    test_cases: list[tuple[str, str]] = [
        ("不存在的路径", f"{base}/status/404"),
        ("错误的域名", "http://this-domain-does-not-exist-42.invalid"),
    ]
    for label, test_url in test_cases:
        try:
            with request.urlopen(test_url, timeout=5) as _resp:
                print(f"  [{label}] 状态码: {_resp.status}")
        except error.HTTPError as exc:
            # HTTPError 是 URLError 的子类，所以先捕获它
            print(f"  [{label}] HTTPError → 状态码={exc.code}, 原因={exc.reason}")
        except error.URLError as exc:
            print(f"  [{label}] URLError → {exc.reason}")
        except TimeoutError:
            print(f"  [{label}] 请求超时")

    # ── 4. 关于 requests 库的说明 ────────────────────────────────────────
    print("\n" + "─" * 60)
    print("4. 第三方库 requests —— 更简洁的 HTTP 客户端")
    print("─" * 60)
    print(
        "  标准库 urllib 功能完备但 API 略显繁琐。\n"
        "  第三方库 requests 是事实上的 HTTP 客户端标准：\n"
        "    pip install requests\n"
        "  同样的 GET 请求用 requests 只需一行:\n"
        "    r = requests.get('https://httpbin.org/get', params={'name': 'Python'})\n"
        "    print(r.json()['args'])\n"
        "  建议：学习阶段掌握 urllib 理解 HTTP 细节，项目开发优先使用 requests。"
    )


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章所有演示函数。"""
    demo_url_parsing()
    demo_http_request_object()
    demo_local_http_server()
    demo_socket_tcp()
    demo_error_handling()
    demo_http_client()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. urllib.parse
#    - urlparse(url)：拆分 URL
#    - parse_qs(query)：把查询字符串转为 dict[str, list[str]]
#    - urlencode(mapping)：把参数 dict 编码为查询字符串
#    - urlunparse(parts)：把 URL 各部分重新组合
#
# 2. urllib.request
#    - Request(url, data=None, headers=None, method=None)：构造请求
#    - urlopen(req_or_url, timeout=3)：发送请求
#    - 响应对象可读取 status、headers、read()
#
# 3. http.server
#    - BaseHTTPRequestHandler：自定义请求处理器
#    - ThreadingHTTPServer((host, port), Handler)：启动本地 HTTP 服务
#    - 本地服务适合做教学、调试、小工具，不适合生产部署
#
# 4. socket
#    - socket.socket(AF_INET, SOCK_STREAM)：创建 TCP socket
#    - bind/listen/accept：服务端流程
#    - create_connection/sendall/recv：客户端流程
#
# 5. 网络编程注意事项
#    - 永远设置 timeout
#    - 检查 HTTP 状态码
#    - 明确编码，常见为 UTF-8
#    - 捕获 URLError、HTTPError、TimeoutError 等异常
#    - 外部接口不稳定，测试时优先使用本地服务或 mock


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个函数 build_search_url(base_url, keyword, page)，返回带查询参数的 URL。
#
# 练习 2（基础）：
#   修改 JsonHandler，新增 /time 路径，返回当前时间字符串。
#
# 练习 3（进阶）：
#   写一个 download_text(url, timeout=3) 函数：
#   - 成功时返回文本内容
#   - HTTP 4xx/5xx 时返回清晰错误信息
#   - 网络失败或超时时不要让程序崩溃
#
# 练习答案提示：
#   练习1：使用 urllib.parse.urlencode 和 urlunparse
#   练习2：在 do_GET 中判断 path.path == "/time"
#   练习3：分别捕获 urllib.error.HTTPError 和 urllib.error.URLError


# =============================================================================
# 【练习答案】
# =============================================================================


def build_search_url(base_url: str, keyword: str, page: int) -> str:
    """练习 1 答案：构造搜索 URL。"""
    parts = parse.urlparse(base_url)
    query = parse.urlencode({"keyword": keyword, "page": page})
    return parse.urlunparse((parts.scheme, parts.netloc, parts.path, "", query, ""))


def download_text(url: str, timeout: float = 3.0) -> str:
    """练习 3 答案：下载文本并做基础异常处理。"""
    try:
        with request.urlopen(url, timeout=timeout) as resp:
            charset = resp.headers.get_content_charset() or "utf-8"
            return resp.read().decode(charset)
    except error.HTTPError as exc:
        return f"HTTP 错误: {exc.code} {exc.reason}"
    except error.URLError as exc:
        return f"网络错误: {exc.reason}"
    except TimeoutError:
        return "网络错误: 请求超时"


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 60)
#     print(build_search_url("https://example.com/search", "python 网络", 1))
#
#     print("\n" + "=" * 60)
#     print(download_text("http://127.0.0.1:8000/health"))
