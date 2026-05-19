// =============================================================================
// 第 29 章：网络与 HTTP
// =============================================================================
//
// 【学习目标】
//   1. 掌握 Node http.createServer、请求对象、响应对象和本地 fetch 的基本闭环
//   2. 理解 URL 解析、请求体读取、运行时校验、HTTP 错误处理和安全边界
//   3. 建立本地 server demo 的资源清理习惯：只监听 127.0.0.1，结束前关闭 server
//
// 【运行方式】
//   npm run build
//   node dist/09_Node_API篇/chapter29_网络与HTTP.js
//   或 npm run chapter -- 29（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { Buffer } from "node:buffer";
import { createServer } from "node:http";
import type { IncomingMessage, Server, ServerResponse } from "node:http";
import type { AddressInfo } from "node:net";
import { posix as pathPosix } from "node:path";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type Priority = "low" | "normal" | "high";

type TaskInput = {
    title: string;
    priority: Priority;
};

type Task = TaskInput & {
    id: number;
};

type DemoState = {
    nextRequestId: number;
    nextTaskId: number;
    tasks: Task[];
    requests: Array<{
        id: number;
        method: string;
        path: string;
    }>;
};

type FetchResult = {
    status: number;
    body: unknown;
};

class HttpError extends Error {
    readonly statusCode: number;
    readonly publicMessage: string;

    constructor(statusCode: number, publicMessage: string) {
        super(publicMessage);
        this.statusCode = statusCode;
        this.publicMessage = publicMessage;
    }
}

// =============================================================================
// 29.1 http.createServer：请求对象与响应对象
// =============================================================================
//
// C++ 对照：C++ 网络服务常见做法是每连接线程、线程池或 asio reactor；Node 的 HTTP
// server 把连接和请求事件投递给事件循环，回调中处理业务。
//
// C++ 思维提示：请求回调不是新线程。CPU 密集逻辑仍会阻塞整个 JS 主线程；I/O 等待才是
// Node HTTP 服务的主要优势场景。

function createDemoState(): DemoState {
    return {
        nextRequestId: 1,
        nextTaskId: 1,
        tasks: [],
        requests: []
    };
}

function sendJson(response: ServerResponse, statusCode: number, value: unknown, headers: Record<string, string> = {}): void {
    const body = `${JSON.stringify(value)}\n`;
    response.writeHead(statusCode, {
        "content-type": "application/json; charset=utf-8",
        "content-length": String(Buffer.byteLength(body)),
        ...headers
    });
    response.end(body);
}

function sendSse(response: ServerResponse, value: unknown): void {
    response.writeHead(200, {
        "content-type": "text/event-stream; charset=utf-8",
        "cache-control": "no-cache",
        connection: "close"
    });
    response.end(`event: ready\ndata: ${JSON.stringify(value)}\n\n`);
}

async function readRequestBody(request: IncomingMessage, limitBytes: number): Promise<string> {
    const chunks: Buffer[] = [];
    let total = 0;

    for await (const chunk of request) {
        const buffer = Buffer.isBuffer(chunk) ? chunk : Buffer.from(String(chunk));
        total += buffer.byteLength;
        if (total > limitBytes) {
            throw new HttpError(413, "request body too large");
        }
        chunks.push(buffer);
    }

    return Buffer.concat(chunks).toString("utf8");
}

function isRecord(value: unknown): value is Record<string, unknown> {
    return typeof value === "object" && value !== null && !Array.isArray(value);
}

function parsePriority(value: unknown): Priority {
    if (value === "low" || value === "normal" || value === "high") {
        return value;
    }
    throw new HttpError(400, "priority must be low, normal, or high");
}

function parseTaskInput(value: unknown): TaskInput {
    if (!isRecord(value)) {
        throw new HttpError(400, "request body must be a JSON object");
    }
    if (typeof value.title !== "string" || value.title.trim() === "") {
        throw new HttpError(400, "title is required");
    }

    return {
        title: value.title.trim(),
        priority: parsePriority(value.priority)
    };
}

function parseJson(text: string): unknown {
    try {
        return JSON.parse(text) as unknown;
    } catch {
        throw new HttpError(400, "request body must be valid JSON");
    }
}

function safeRelativePath(input: string): string {
    const rawParts = input.split(/[\\/]+/);
    const normalized = pathPosix.normalize(input);
    if (
        input.startsWith("/") ||
        input.includes("\\") ||
        rawParts.includes("..") ||
        normalized === ".." ||
        normalized.startsWith("../")
    ) {
        throw new HttpError(400, "unsafe path");
    }
    return normalized;
}

async function handleRequest(request: IncomingMessage, response: ServerResponse, state: DemoState): Promise<void> {
    const method = request.method ?? "GET";
    const url = new URL(request.url ?? "/", "http://127.0.0.1");
    const requestId = state.nextRequestId;
    state.nextRequestId += 1;
    state.requests.push({ id: requestId, method, path: url.pathname });

    response.setHeader("x-request-id", String(requestId));

    try {
        if (method === "GET" && url.pathname === "/health") {
            sendJson(response, 200, { ok: true, service: "ts-hello", requestId });
            return;
        }

        if (method === "GET" && url.pathname === "/echo") {
            sendJson(response, 200, {
                method,
                path: url.pathname,
                name: url.searchParams.get("name") ?? "<anonymous>",
                tags: url.searchParams.getAll("tag"),
                accept: request.headers.accept ?? "<none>",
                requestId
            });
            return;
        }

        if (method === "POST" && url.pathname === "/tasks") {
            const bodyText = await readRequestBody(request, 1024);
            const input = parseTaskInput(parseJson(bodyText));
            const task: Task = {
                id: state.nextTaskId,
                ...input
            };
            state.nextTaskId += 1;
            state.tasks.push(task);
            sendJson(response, 201, { task, requestId });
            return;
        }

        if (method === "GET" && url.pathname === "/safe-file") {
            const requestedPath = url.searchParams.get("path") ?? "";
            sendJson(response, 200, {
                requestedPath,
                normalizedPath: safeRelativePath(requestedPath),
                requestId
            });
            return;
        }

        if (method === "GET" && url.pathname === "/boom") {
            throw new Error("internal stack should not leak to client");
        }

        if (method === "GET" && url.pathname === "/sse") {
            sendSse(response, { message: "hello-sse", requestId });
            return;
        }

        throw new HttpError(404, "route not found");
    } catch (error) {
        if (response.headersSent) {
            response.end();
            return;
        }
        if (error instanceof HttpError) {
            sendJson(response, error.statusCode, { error: error.publicMessage, requestId });
            return;
        }
        sendJson(response, 500, { error: "internal server error", requestId });
    }
}

async function startDemoServer(state: DemoState): Promise<{ server: Server; baseUrl: string }> {
    const server = createServer((request, response) => {
        void handleRequest(request, response, state);
    });

    await new Promise<void>((resolvePromise, reject) => {
        server.once("error", reject);
        server.listen(0, "127.0.0.1", () => {
            server.off("error", reject);
            resolvePromise();
        });
    });

    const address = server.address();
    if (typeof address !== "object" || address === null) {
        throw new Error("server did not expose a TCP address");
    }

    return {
        server,
        baseUrl: `http://127.0.0.1:${(address as AddressInfo).port}`
    };
}

function closeServer(server: Server): Promise<void> {
    return new Promise((resolvePromise, reject) => {
        server.close((error) => {
            if (error) {
                reject(error);
                return;
            }
            resolvePromise();
        });
    });
}

async function fetchJson(url: string | URL, init?: RequestInit): Promise<FetchResult> {
    const response = await fetch(url, init);
    return {
        status: response.status,
        body: (await response.json()) as unknown
    };
}

async function demoHttpBasics(baseUrl: string): Promise<void> {
    section("29.1 http.createServer：请求对象与响应对象");
    note("C++ 对照", "Node 的请求回调处理 IncomingMessage 和 ServerResponse，不为每个请求创建 JS 线程。");

    const result = await fetchJson(`${baseUrl}/echo?name=Ada&tag=ts&tag=node`, {
        headers: {
            accept: "application/json"
        }
    });

    showJson("请求与响应对象", {
        localOnly: "server listens on 127.0.0.1 with an ephemeral port",
        response: result
    });
    note("输出解释", "服务端从 request.method、request.url、request.headers 读取输入，用 writeHead/end 写响应。");
}

// =============================================================================
// 29.2 fetch：GET、POST、自定义 headers 与 response.json()
// =============================================================================
//
// C++ 对照：fetch 是高层 HTTP 客户端；直觉上类似封装好的请求对象和响应对象，不是 socket API。
//
// C++ 思维提示：fetch 返回 Promise<Response>，HTTP 404/500 不会自动 throw；要检查 status
// 或 response.ok。

async function demoFetchGetPost(baseUrl: string): Promise<void> {
    section("29.2 fetch：GET、POST、自定义 headers 与 response.json()");
    note("C++ 对照", "fetch 像现代 HTTP client；响应体解析是异步步骤。");

    const health = await fetchJson(`${baseUrl}/health`);
    const created = await fetchJson(`${baseUrl}/tasks`, {
        method: "POST",
        headers: {
            "content-type": "application/json",
            "x-client": "chapter29"
        },
        body: JSON.stringify({ title: "write tutorial", priority: "high" })
    });

    showJson("fetch GET/POST", {
        health,
        created
    });
    note("常见坑", "fetch 只在网络错误时 reject；业务错误状态码要用 response.status/ok 自己判断。");
}

// =============================================================================
// 29.3 URL 解析：new URL 与 searchParams
// =============================================================================
//
// C++ 对照：URL 不是普通字符串；路径、查询参数和编码规则应该交给解析器处理。
//
// C++ 思维提示：不要手写 split(\"?\") 解析查询参数。重复参数、百分号编码和空值都会让
// 字符串切分逻辑变脆。

async function demoUrlParsing(baseUrl: string): Promise<void> {
    section("29.3 URL 解析：new URL 与 searchParams");
    note("C++ 对照", "URL parser 像专门的协议解析器；比手写字符串切分可靠。");

    const url = new URL("/echo", baseUrl);
    url.searchParams.set("name", "C++ user");
    url.searchParams.append("tag", "http");
    url.searchParams.append("tag", "runtime");

    const response = await fetchJson(url);
    showJson("URL 与查询参数", {
        pathname: url.pathname,
        encodedSearch: url.search,
        serverView: response
    });
    note("输出解释", "searchParams 会负责空格、+、% 等编码细节，服务端再用同样的 URL API 读取。");
}

// =============================================================================
// 29.4 请求体读取与 runtime validation
// =============================================================================
//
// C++ 对照：外部 JSON 类似从网络读入的 char buffer；先解析成 unknown，再校验成业务结构。
//
// C++ 思维提示：TypeScript 类型不会跨网络传输。客户端声明 body 是 TaskInput，不代表服务端
// 收到的 JSON 真的符合 TaskInput。

async function demoBodyValidation(baseUrl: string): Promise<void> {
    section("29.4 请求体读取与 runtime validation");
    note("C++ 对照", "网络输入必须运行时校验，不能只依赖编译期类型。");

    const good = await fetchJson(`${baseUrl}/tasks`, {
        method: "POST",
        headers: { "content-type": "application/json" },
        body: JSON.stringify({ title: "validate request", priority: "normal" })
    });
    const bad = await fetchJson(`${baseUrl}/tasks`, {
        method: "POST",
        headers: { "content-type": "application/json" },
        body: JSON.stringify({ title: "", priority: "urgent" })
    });

    showJson("请求体校验", {
        good,
        bad
    });
    note("类型边界", "parseTaskInput 是从 unknown 到 TaskInput 的运行时边界；没有它，类型标注只是自我安慰。");
}

// =============================================================================
// 29.5 HTTP 错误处理：400、404、500 的职责边界
// =============================================================================
//
// C++ 对照：错误状态码类似明确的错误协议。客户端输入错是 4xx，服务端内部失败是 5xx。
//
// C++ 思维提示：500 响应不要泄漏内部异常、栈路径、SQL 或环境变量；日志和响应要分层。

async function demoHttpErrors(baseUrl: string): Promise<void> {
    section("29.5 HTTP 错误处理：400、404、500 的职责边界");
    note("C++ 对照", "HTTP status 是跨进程错误协议，不是异常对象本身。");

    const badPath = await fetchJson(`${baseUrl}/safe-file?path=../secret.txt`);
    const missing = await fetchJson(`${baseUrl}/missing`);
    const boom = await fetchJson(`${baseUrl}/boom`);

    showJson("错误响应", {
        badPath,
        missing,
        boom
    });
    note("输出解释", "400/404 返回可读业务错误；500 只返回 generic message，不把内部异常文本暴露给客户端。");
}

// =============================================================================
// 29.6 安全基础：路径遍历、命令注入、请求体大小限制
// =============================================================================
//
// C++ 对照：服务端输入都应按不可信处理，类似 C++ 中绝不直接信任网络 buffer 或 shell 参数。
//
// C++ 思维提示：HTTP 服务的安全不是 TypeScript 自动给的。路径、命令、JSON body 都要有
// 明确的白名单、长度限制和错误路径。

async function demoSecurityBasics(baseUrl: string): Promise<void> {
    section("29.6 安全基础：路径遍历、命令注入、请求体大小限制");
    note("C++ 对照", "服务端边界要像处理裸 socket 输入一样谨慎。");

    const safePath = await fetchJson(`${baseUrl}/safe-file?path=reports/2026.txt`);
    const largeBody = await fetchJson(`${baseUrl}/tasks`, {
        method: "POST",
        headers: { "content-type": "application/json" },
        body: JSON.stringify({ title: "x".repeat(1200), priority: "low" })
    });

    showJson("安全策略", {
        pathTraversal: {
            safePath,
            rule: "normalize then ensure path stays under allowed root"
        },
        commandInjection: {
            rule: "do not pass user text into shell commands; prefer spawn(file, args) with validation"
        },
        bodyLimit: largeBody
    });
    note("常见坑", "本地 demo 也要写 body limit；否则真实服务容易被超大请求拖垮内存。");
}

// =============================================================================
// 29.7 HTTP 后端最小闭环：routing、middleware、health、graceful shutdown
// =============================================================================
//
// C++ 对照：routing 像按路径分发 handler；middleware 像请求进入业务前后的过滤器或拦截器。
//
// C++ 思维提示：server.close() 是协作式关闭，不是立刻杀进程。生产环境还要停止接收新请求、
// 等待已有请求结束，并设置超时兜底。

function demoBackendLifecycle(state: DemoState): void {
    section("29.7 HTTP 后端最小闭环：routing、middleware、health、graceful shutdown");
    note("C++ 对照", "请求先经过通用边界，再进入具体路由 handler。");

    showJson("后端闭环", {
        routing: ["/health", "/echo", "/tasks", "/safe-file", "/sse"],
        middlewareConcept: "assign deterministic requestId and collect request metadata before routing",
        healthCheck: "GET /health returns { ok: true }",
        gracefulShutdown: "runChapter closes the local server in finally",
        observedRequests: state.requests
    });
    note("输出解释", "本章的 requestId 是中间件概念演示；真实服务通常还会做日志、鉴权、限流和 trace。");
}

// =============================================================================
// 29.8 API 契约与 HTTPS：边界概念
// =============================================================================
//
// C++ 对照：API 契约像跨进程 ABI。双方都要知道请求、响应、错误和版本规则。
//
// C++ 思维提示：JSON Schema/OpenAPI 描述的是协议，不替代服务端 runtime validation；
// HTTPS 解决传输加密和身份校验，不解决业务鉴权。

function demoApiContractAndHttps(): void {
    section("29.8 API 契约与 HTTPS：边界概念");
    note("C++ 对照", "HTTP API 契约类似远程 ABI；HTTPS 证书类似对端身份和加密通道约定。");

    showJson("契约与 HTTPS 边界", {
        jsonSchemaIdea: {
            type: "object",
            required: ["title", "priority"],
            properties: {
                title: "string",
                priority: ["low", "normal", "high"]
            }
        },
        openApiIdea: "documents method/path/request/response/status codes for humans and tooling",
        httpsBasics: {
            certificate: "binds public key to a hostname",
            tls: "encrypts transport and verifies peer identity",
            tutorialBoundary: "this chapter uses plain HTTP on 127.0.0.1 only"
        }
    });
    note("常见坑", "有 HTTPS 不代表用户已登录、有权限或输入可信；鉴权和校验仍要做。");
}

// =============================================================================
// 29.9 Server-Sent Events：服务端向浏览器持续推送文本事件
// =============================================================================
//
// C++ 对照：SSE 像保持一个 HTTP 响应流，服务端按文本帧推送事件；它不是双向 socket。
//
// C++ 思维提示：长连接必须有关闭路径。真实 SSE 服务要处理断线、心跳和背压；本章只发
// 一个事件后立即关闭，保证可重复运行。

async function demoSse(baseUrl: string): Promise<void> {
    section("29.9 Server-Sent Events：服务端向浏览器持续推送文本事件");
    note("C++ 对照", "SSE 是 HTTP 响应流，不是 WebSocket；客户端不能通过同一连接回写消息。");

    const response = await fetch(`${baseUrl}/sse`, {
        headers: { accept: "text/event-stream" }
    });
    const body = await response.text();

    showJson("SSE 最小响应", {
        status: response.status,
        contentType: response.headers.get("content-type"),
        lines: body.trimEnd().split("\n")
    });
    note("可重复运行", "本章 SSE 只发送一个事件并关闭连接，不留下挂起 server、timer 或 socket。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 29 demos. */
export async function runChapter(): Promise<void> {
    const state = createDemoState();
    const { server, baseUrl } = await startDemoServer(state);

    try {
        await demoHttpBasics(baseUrl);
        await demoFetchGetPost(baseUrl);
        await demoUrlParsing(baseUrl);
        await demoBodyValidation(baseUrl);
        await demoHttpErrors(baseUrl);
        await demoSecurityBasics(baseUrl);
        demoBackendLifecycle(state);
        demoApiContractAndHttps();
        await demoSse(baseUrl);
    } finally {
        await closeServer(server);
    }
}

await runIfMain(import.meta.url, runChapter);
