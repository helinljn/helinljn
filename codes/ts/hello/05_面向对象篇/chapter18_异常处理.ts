// =============================================================================
// 第 18 章：异常处理
// =============================================================================
//
// 【学习目标】
//   1. 掌握 throw、try-catch-finally、Error 层级和自定义错误类
//   2. 理解 catch 变量为 unknown、Promise 异常和 Error.cause
//   3. 区分 throw 与 Result/Either 风格，设计错误码、可恢复性和 CLI exit code
//
// 【运行方式】
//   npm run build
//   node dist/05_面向对象篇/chapter18_异常处理.js
//   或 npm run chapter -- 18（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type Result<T, E> =
    | { ok: true; value: T }
    | { ok: false; error: E };

type ErrorInfo = {
    name: string;
    message: string;
    code?: string;
    recoverable?: boolean;
    exitCode?: number;
    cause?: string | undefined;
};

class TutorialError extends Error {
    constructor(
        message: string,
        public readonly code: string,
        public readonly recoverable: boolean,
        public readonly exitCode = 1,
        options?: ErrorOptions
    ) {
        super(message, options);
        this.name = new.target.name;
    }
}

class ConfigError extends TutorialError {
    constructor(message: string, options?: ErrorOptions) {
        super(message, "CONFIG_INVALID", true, 2, options);
    }
}

class DataError extends TutorialError {
    constructor(message: string, options?: ErrorOptions) {
        super(message, "DATA_INVALID", true, 3, options);
    }
}

class FatalError extends TutorialError {
    constructor(message: string, options?: ErrorOptions) {
        super(message, "FATAL", false, 70, options);
    }
}

function describeUnknownError(error: unknown): ErrorInfo {
    if (error instanceof TutorialError) {
        return {
            name: error.name,
            message: error.message,
            code: error.code,
            recoverable: error.recoverable,
            exitCode: error.exitCode,
            cause: error.cause instanceof Error ? error.cause.message : undefined
        };
    }

    if (error instanceof Error) {
        return {
            name: error.name,
            message: error.message,
            cause: error.cause instanceof Error ? error.cause.message : undefined
        };
    }

    return {
        name: typeof error,
        message: String(error)
    };
}

function readRequiredConfig(config: Record<string, unknown>, key: string): string {
    const value = config[key];
    if (typeof value !== "string" || value.trim() === "") {
        throw new ConfigError(`config ${key} must be a non-empty string`);
    }
    return value.trim();
}

function parsePortOrThrow(text: string): number {
    const value = Number.parseInt(text, 10);
    if (!Number.isInteger(value)) {
        throw new DataError(`port is not an integer: ${text}`);
    }
    if (value < 1 || value > 65535) {
        throw new RangeError(`port out of range: ${value}`);
    }
    return value;
}

function parsePortResult(text: string): Result<number, ErrorInfo> {
    try {
        return { ok: true, value: parsePortOrThrow(text) };
    } catch (error: unknown) {
        return { ok: false, error: describeUnknownError(error) };
    }
}

async function loadUserProfile(id: string): Promise<{ id: string; name: string }> {
    if (id === "missing") {
        throw new DataError("user profile not found");
    }
    return { id, name: "Ada" };
}

async function runCliBoundary(action: () => Promise<void>): Promise<{ exitCode: number; message: string }> {
    try {
        await action();
        return { exitCode: 0, message: "ok" };
    } catch (error: unknown) {
        const info = describeUnknownError(error);
        return {
            exitCode: info.exitCode ?? 1,
            message: `${info.name}:${info.message}`
        };
    }
}

// =============================================================================
// 18.1 throw、try-catch-finally
// =============================================================================
//
// C++ 对照：
//   try/catch/finally 的控制流与 C++ try/catch 类似。
//   finally 更像确定会执行的清理块，但不要在 finally 里吞掉原始错误。

function demoThrowCatchFinally(): void {
    section("18.1 throw、try-catch-finally");
    note("C++ 对照", "异常会打断当前控制流；finally 适合做清理，不适合改写结果。");

    const trace: string[] = [];
    try {
        trace.push("before");
        parsePortOrThrow("70000");
        trace.push("after");
    } catch (error: unknown) {
        trace.push(`catch:${describeUnknownError(error).message}`);
    } finally {
        trace.push("finally");
    }

    showJson("try/catch/finally", trace);
    note("输出解释", "parsePortOrThrow 抛错后不会执行 after，但 finally 仍然执行。");
}

// =============================================================================
// 18.2 Error 层级与自定义错误类
// =============================================================================
//
// C++ 对照：
//   自定义错误类像业务异常层级。
//   TS/JS 里建议保留 name、message、code、recoverable、exitCode 等机器可读信息。

function demoCustomErrorsAndCause(): void {
    section("18.2 Error 层级与自定义错误类");
    note("C++ 对照", "错误类不仅给人看 message，也给程序看 code 和 recoverable。");

    const cause = new Error("env TARGET was empty");
    const error = new ConfigError("failed to load compiler target", { cause });

    showJson("自定义错误", describeUnknownError(error));
    note("输出解释", "Error.cause 保存底层原因；外层错误提供业务语义和退出码。");
}

// =============================================================================
// 18.3 catch 类型 unknown
// =============================================================================
//
// C++ 对照：
//   JS 可以 throw 任意值，不一定是 Error 实例。
//   因此 strict 模式下 catch 变量是 unknown，需要先收窄。

function demoCatchUnknown(): void {
    section("18.3 catch 类型 unknown");
    note("C++ 对照", "catch 到的可能不是异常对象；先判断再读取字段。");

    const samples: unknown[] = [
        new Error("plain error"),
        new DataError("bad row"),
        "string failure"
    ];

    const normalized = samples.map((sample) => {
        try {
            throw sample;
        } catch (error: unknown) {
            return describeUnknownError(error);
        }
    });

    showJson("unknown catch 归一化", normalized);
    note("常见坑", "不要直接写 error.message；在 useUnknownInCatchVariables 下这会被类型系统拦住。");
}

// =============================================================================
// 18.4 Promise 异常
// =============================================================================
//
// C++ 对照：
//   Promise rejection 更像异步结果里的失败状态。
//   C++ 思维提示：await 会在当前 async 函数里重新抛出 rejection，因此仍可用 try/catch 收敛。

async function demoPromiseErrors(): Promise<void> {
    section("18.4 Promise 异常");
    note("C++ 对照", "Promise 失败不会跨线程抛异常；它通过 rejection 传递到 await 点。");

    const ok = await loadUserProfile("u1");
    let failed: ErrorInfo | undefined;
    try {
        await loadUserProfile("missing");
    } catch (error: unknown) {
        failed = describeUnknownError(error);
    }

    showJson("Promise 异常", {
        ok,
        failed
    });
    note("输出解释", "await missing profile 时，rejection 在 await 位置进入 catch。");
}

// =============================================================================
// 18.5 throw vs Result/Either 风格
// =============================================================================
//
// C++ 对照：
//   throw 适合不可继续的异常路径。
//   Result/Either 适合预期内失败，尤其是解析、校验和批处理。

function demoThrowVsResult(): void {
    section("18.5 throw vs Result/Either 风格");
    note("C++ 对照", "Result<T,E> 类似 std::expected：失败是值，不是控制流跳转。");

    const inputs = ["3000", "oops", "70000"];
    const results = inputs.map((input) => ({
        input,
        result: parsePortResult(input)
    }));

    showJson("Result 风格", results);
    note("输出解释", "批量处理时 Result 能把每一项的成功/失败都保留下来。");
}

// =============================================================================
// 18.6 错误码、可恢复性和 CLI exit code
// =============================================================================
//
// C++ 对照：
//   系统边界通常需要机器可读错误码。
//   CLI 要返回 exit code；HTTP handler 要映射 status code；库函数要保留可恢复性。

function demoErrorCodesAndExitCodes(): void {
    section("18.6 错误码、可恢复性和 CLI exit code");
    note("C++ 对照", "错误码是跨进程/跨服务边界的稳定协议。");

    const errors = [
        new ConfigError("config file missing"),
        new DataError("input row invalid"),
        new FatalError("database schema corrupted")
    ];

    showJson("错误分类", errors.map(describeUnknownError));
    note("输出解释", "recoverable 和 exitCode 帮助入口层决定重试、提示用户还是直接终止。");
}

// =============================================================================
// 18.7 async 边界统一捕获
// =============================================================================
//
// C++ 对照：
//   入口层负责把异常收敛成进程退出码、HTTP 响应或日志记录。
//   C++ 思维提示：不要让 async rejection 漂到顶层变成未处理拒绝。

async function demoAsyncBoundary(): Promise<void> {
    section("18.7 async 边界统一捕获");
    note("C++ 对照", "项目入口像 main()，应该把错误转换成稳定输出。");

    const success = await runCliBoundary(async () => {
        readRequiredConfig({ target: "ES2023" }, "target");
    });
    const failure = await runCliBoundary(async () => {
        readRequiredConfig({}, "target");
    });

    showJson("入口边界", {
        success,
        failure
    });
    note("输出解释", "业务函数可以 throw；入口层负责转换成 exitCode 和消息。");
}

// =============================================================================
// 18.8 本章复盘
// =============================================================================
//
// C++ 对照：
//   异常、错误码和 Result 都是错误表达方式。
//   工程上应按边界选择，而不是统一用一种机制解决所有问题。

function demoChapterReview(): void {
    section("18.8 本章复盘");
    note("C++ 对照", "异常处理的目标是让失败路径清楚、可恢复性明确、入口边界稳定。");

    const summary = [
        "throw 会打断同步控制流，finally 负责清理",
        "自定义错误类应包含 code、recoverable、exitCode 等机器可读信息",
        "catch 变量是 unknown，需要先收窄",
        "Promise rejection 在 await 点进入 try/catch",
        "Result/Either 适合预期内失败和批处理",
        "入口层应统一把异常收敛成 exit code、HTTP 响应或项目输出"
    ];

    showJson("关键结论", summary);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 18. */
export async function runChapter(): Promise<void> {
    demoThrowCatchFinally();
    demoCustomErrorsAndCause();
    demoCatchUnknown();
    await demoPromiseErrors();
    demoThrowVsResult();
    demoErrorCodesAndExitCodes();
    await demoAsyncBoundary();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
