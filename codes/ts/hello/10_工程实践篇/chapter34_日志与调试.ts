// =============================================================================
// 第 34 章：日志与调试
// =============================================================================
//
// 【学习目标】
//   1. 掌握 console、结构化日志、脱敏和可观测性的基本边界
//   2. 理解 debug 命名空间、Node inspector、Chrome DevTools 和 VS Code 调试配置的定位
//   3. 区分 node:assert 运行时断言与 TypeScript 类型断言
//
// 【运行方式】
//   npm run build
//   node dist/10_工程实践篇/chapter34_日志与调试.js
//   或 npm run chapter -- 34（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { deepEqual, equal, ok, strictEqual, throws } from "node:assert/strict";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type LogLevel = "debug" | "info" | "warn" | "error";

type StructuredLog = {
    timestamp: string;
    level: LogLevel;
    message: string;
    context: Record<string, unknown>;
};

type RequestContext = {
    requestId: string;
    route: string;
    userEmail: string;
    ip: string;
    token: string;
    password: string;
};

// =============================================================================
// 34.1 console 深入：输出方法、表格和计时对
// =============================================================================
//
// C++ 对照：console.log/info/warn/error/debug 类似 std::cout/std::cerr 加日志级别约定；
// console.table 像临时调试用的表格化输出。
//
// C++ 思维提示：console 是最基础的 I/O 工具，不是完整日志系统。生产服务通常要输出
// 结构化日志，并把 stdout/stderr 交给日志采集器。

function demoConsoleMethods(): void {
    section("34.1 console 深入：输出方法、表格和计时对");
    note("C++ 对照", "console 方法像 std::cout/std::cerr 的分层封装，但没有自动结构化。");

    showJson("console 方法职责", [
        { method: "console.log", stream: "stdout", use: "普通输出或一次性调试" },
        { method: "console.info", stream: "stdout", use: "信息级事件" },
        { method: "console.warn", stream: "stderr", use: "可恢复风险" },
        { method: "console.error", stream: "stderr", use: "失败或异常摘要" },
        { method: "console.debug", stream: "stdout/stderr depends on runtime", use: "调试细节，通常受环境开关控制" },
        { method: "console.table", stream: "stdout", use: "把数组/对象临时表格化查看" }
    ]);
    showJson("console.time/timeEnd 计时对", {
        pattern: ["console.time(\"load-config\")", "do work", "console.timeEnd(\"load-config\")"],
        chapterBoundary: "本章不直接输出真实耗时，避免每次运行结果变化",
        productionRule: "真实服务更常把 durationMs 放进结构化日志或 metrics"
    });
    note("常见坑", "console.timeEnd 会输出实时耗时；教程和测试里不要把它当稳定断言。");
}

// =============================================================================
// 34.2 结构化日志：JSON 行比拼接字符串更适合机器处理
// =============================================================================
//
// C++ 对照：结构化日志像把 printf 文本升级为带字段的事件对象；日志系统可以按字段过滤、
// 聚合和告警。
//
// C++ 思维提示：日志的消费者通常是机器，不只是人。固定字段比自由文本更容易被检索和统计。

function makeLog(level: LogLevel, message: string, context: Record<string, unknown>): StructuredLog {
    return {
        timestamp: "2026-05-19T08:30:00.000Z",
        level,
        message,
        context
    };
}

function demoStructuredLogs(): void {
    section("34.2 结构化日志：JSON 行比拼接字符串更适合机器处理");
    note("C++ 对照", "结构化日志把事件建模成对象，而不是只写一串不可解析的文本。");

    const log = makeLog("info", "request completed", {
        requestId: "req-001",
        route: "GET /health",
        statusCode: 200,
        durationMs: 12
    });

    showJson("结构化日志对象", {
        object: log,
        jsonLine: JSON.stringify(log),
        requiredFields: ["timestamp", "level", "message", "context"]
    });
    note("输出解释", "JSON.stringify 生成单行日志后，采集器可以按 level、route、statusCode 等字段检索。");
}

// =============================================================================
// 34.3 日志安全：脱敏 token、password、email、IP
// =============================================================================
//
// C++ 对照：日志像 core dump 或调试输出一样可能进入长期存储；敏感字段一旦写入就很难彻底删除。
//
// C++ 思维提示：不要把 secret、完整请求体、堆栈细节、cookie、Authorization header 直接写入
// 普通日志。先在边界层统一脱敏。

function redactEmail(email: string): string {
    const [name, domain] = email.split("@");
    if (name === undefined || domain === undefined) {
        return "<invalid-email>";
    }
    return `${name.slice(0, 2)}***@${domain}`;
}

function redactIp(ip: string): string {
    const parts = ip.split(".");
    if (parts.length !== 4) {
        return "<redacted-ip>";
    }
    return `${parts[0]}.${parts[1]}.x.x`;
}

function redactSecret(secret: string): string {
    if (secret.length <= 4) {
        return "***";
    }
    return `${secret.slice(0, 3)}***${secret.slice(-2)}`;
}

function redactRequestContext(context: RequestContext): Record<string, unknown> {
    return {
        requestId: context.requestId,
        route: context.route,
        userEmail: redactEmail(context.userEmail),
        ip: redactIp(context.ip),
        token: redactSecret(context.token),
        password: "<redacted>"
    };
}

function demoLogRedaction(): void {
    section("34.3 日志安全：脱敏 token、password、email、IP");
    note("C++ 对照", "调试输出也属于数据泄漏面；日志越集中，泄漏影响越大。");

    const context: RequestContext = {
        requestId: "req-002",
        route: "POST /login",
        userEmail: "developer@example.com",
        ip: "203.0.113.42",
        token: "tok_live_123456",
        password: "correct-horse"
    };

    showJson("脱敏后的日志上下文", redactRequestContext(context));
    note("常见坑", "不要记录完整 request body；请求体可能包含密码、token、身份证号或业务敏感字段。");
    note("常见坑", "普通错误响应和普通日志不要泄漏 stack trace；内部栈路径可留在受控错误追踪系统。");
}

// =============================================================================
// 34.4 可观测性：logs、metrics、traces 的职责差异
// =============================================================================
//
// C++ 对照：日志像离散事件，metrics 像计数器/直方图，traces 像跨函数或跨服务的调用链。
//
// C++ 思维提示：不要指望一种信号解决所有问题。排查单个请求靠 trace，观察整体健康靠 metrics，
// 理解具体事件靠 logs。

function demoObservabilitySignals(): void {
    section("34.4 可观测性：logs、metrics、traces 的职责差异");
    note("C++ 对照", "这类似把 printf、性能计数器和调用链采样分成三类工具。");

    showJson("三类信号", [
        {
            signal: "logs",
            question: "发生了什么具体事件？",
            example: makeLog("warn", "retry scheduled", { requestId: "req-003", retryAfterMs: 100 })
        },
        {
            signal: "metrics",
            question: "系统整体趋势如何？",
            example: { httpRequestsTotal: 42, errorRate: 0.02, p95LatencyMs: 80 }
        },
        {
            signal: "traces",
            question: "一次请求经过了哪些步骤？",
            example: { traceId: "trace-001", spans: ["http", "service", "db"] }
        }
    ]);
    note("工程边界", "本教程只实现日志和简单计时概念，不引入后端监控依赖。");
}

// =============================================================================
// 34.5 debug 库：命名空间与 DEBUG 环境变量
// =============================================================================
//
// C++ 对照：debug("app:db") 像按模块名开关的调试日志，比散落的 printf 更容易控制。
//
// C++ 思维提示：调试日志应默认关闭，并能按命名空间打开；不要把 debug 输出当成审计日志。

function debugEnabled(namespace: string, pattern: string): boolean {
    return pattern
        .split(",")
        .map((item) => item.trim())
        .filter((item) => item.length > 0)
        .some((item) => {
            if (item.endsWith("*")) {
                return namespace.startsWith(item.slice(0, -1));
            }
            return item === namespace;
        });
}

function demoDebugNamespaces(): void {
    section("34.5 debug 库：命名空间与 DEBUG 环境变量");
    note("C++ 对照", "命名空间调试日志类似按 logger name 或编译开关打开模块调试输出。");

    showJson("debug 命名空间模型", {
        dependencyBoundary: "本章不引入 debug 依赖，只解释常见用法",
        usage: "const log = debug(\"app:db\"); log(\"connected\")",
        env: "DEBUG=app:*",
        enabled: {
            "app:db": debugEnabled("app:db", "app:*"),
            "app:http": debugEnabled("app:http", "app:*"),
            "worker:job": debugEnabled("worker:job", "app:*")
        }
    });
    note("常见坑", "DEBUG=app:* 这类开关不应该打开 secret 输出；调试日志同样要走脱敏规则。");
}

// =============================================================================
// 34.6 Node 调试：--inspect、Chrome DevTools 与 VS Code
// =============================================================================
//
// C++ 对照：node --inspect 类似启动进程后让调试器附加；--inspect-brk 类似在 main 开始前断住。
//
// C++ 思维提示：调试器是运行时工具，不改变 TypeScript 类型检查。调试前仍要 build，确保
// source map 和 dist 产物对应。

function demoNodeInspector(): void {
    section("34.6 Node 调试：--inspect、Chrome DevTools 与 VS Code");
    note("C++ 对照", "Node inspector 对应 GDB/lldb 的附加调试入口，DevTools/VS Code 是前端界面。");

    showJson("调试入口", {
        commands: [
            "node --inspect dist/main.js",
            "node --inspect-brk dist/main.js"
        ],
        chromeDevTools: ["Sources breakpoints", "Watch expressions", "Call Stack", "Console evaluation"],
        vscodeLaunchJson: {
            type: "node",
            request: "launch",
            name: "Debug current built chapter",
            program: "${workspaceFolder}/dist/main.js",
            args: ["chapter", "34"],
            sourceMaps: true
        }
    });
    note("安全边界", "不要把 inspector 端口暴露到公网；调试端口可以执行进程内代码。");
}

// =============================================================================
// 34.7 node:assert：运行时断言，不是类型断言
// =============================================================================
//
// C++ 对照：node:assert 接近运行时 assert/测试断言；TypeScript 的 as 是编译期类型断言，
// 不会在运行时检查数据。
//
// C++ 思维提示：assert 适合验证不变量和测试期望。外部输入错误应返回可读业务错误，
// 不要把用户输错参数都做成 AssertionError。

function parseJsonObject(text: string): Record<string, unknown> {
    const value = JSON.parse(text) as unknown;
    if (typeof value !== "object" || value === null || Array.isArray(value)) {
        throw new TypeError("expected object");
    }
    return value as Record<string, unknown>;
}

function demoNodeAssert(): void {
    section("34.7 node:assert：运行时断言，不是类型断言");
    note("C++ 对照", "node:assert 在运行时检查条件；TS 的 as 只影响编译器，不生成校验代码。");

    ok(true);
    equal(1 + 1, 2);
    strictEqual("ts".toUpperCase(), "TS");
    deepEqual(parseJsonObject("{\"strict\":true}"), { strict: true });
    throws(() => parseJsonObject("[]"), /expected object/);

    showJson("assert 用法", {
        ok: "assert.ok(condition)",
        equal: "assert.equal(actual, expected)",
        deepEqual: "assert.deepEqual(object, expected)",
        strictEqual: "assert.strictEqual(actual, expected)",
        throws: "assert.throws(fn, /message/)",
        allAssertionsPassed: true
    });
    note("类型边界", "`value as User` 不会验证 JSON；需要像 parseJsonObject 这样的运行时校验。");
}

// =============================================================================
// 34.8 工程场景：请求日志、调试开关和断言分层
// =============================================================================
//
// C++ 对照：入口层负责日志和错误协议，业务层维护不变量，调试器用于局部排查。
//
// C++ 思维提示：日志、debug、assert 和 debugger 各有职责。把它们混成一个工具，会导致
// 生产噪音、敏感泄漏或错误处理边界混乱。

function demoEngineeringWorkflow(): void {
    section("34.8 工程场景：请求日志、调试开关和断言分层");
    note("C++ 对照", "像服务端 main/request handler 一样，把诊断能力分层放置。");

    const requestContext = redactRequestContext({
        requestId: "req-004",
        route: "GET /api/items",
        userEmail: "reader@example.com",
        ip: "198.51.100.8",
        token: "tok_test_abcdef",
        password: "never-log-this"
    });

    showJson("诊断分层", {
        accessLog: makeLog("info", "request completed", {
            ...requestContext,
            statusCode: 200,
            durationMs: 18
        }),
        debugLog: {
            namespace: "app:items",
            enabledWhen: "DEBUG=app:*",
            contentRule: "implementation details, still redacted"
        },
        assertionLayer: "internal invariants and tests",
        debuggerLayer: "breakpoints, watch expressions, call stack"
    });
    note("本章复盘", "日志要结构化，敏感数据要脱敏，调试端口要保护，assert 不替代外部输入校验。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 34 demos. */
export function runChapter(): void {
    demoConsoleMethods();
    demoStructuredLogs();
    demoLogRedaction();
    demoObservabilitySignals();
    demoDebugNamespaces();
    demoNodeInspector();
    demoNodeAssert();
    demoEngineeringWorkflow();
}

await runIfMain(import.meta.url, runChapter);
