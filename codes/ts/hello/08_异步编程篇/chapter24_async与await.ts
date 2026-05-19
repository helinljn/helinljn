// =============================================================================
// 第 24 章：async 与 await
// =============================================================================
//
// 【学习目标】
//   1. 理解 async 函数总是返回 Promise，await 会暂停当前 async 函数
//   2. 掌握 try/catch + await 的错误收敛方式
//   3. 区分串行 await、并行 Promise.all 和循环中的 await
//   4. 理解顶层 await 与 AbortController 的工程边界
//
// 【运行方式】
//   npm run build
//   node dist/08_异步编程篇/chapter24_async与await.js
//   或 npm run chapter -- 24（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type FetchStep = {
    id: string;
    payload: string;
};

type OperationResult =
    | { ok: true; value: string }
    | { ok: false; error: string };

function printSnippet(label: string, lines: readonly string[]): void {
    console.log(`${label}:`);
    for (const line of lines) {
        console.log(`  ${line}`);
    }
}

function errorMessage(error: unknown): string {
    return error instanceof Error ? error.message : String(error);
}

async function loadConfigValue(name: string): Promise<string> {
    return Promise.resolve(`config:${name}`);
}

async function failValidation(name: string): Promise<string> {
    await Promise.resolve();
    throw new Error(`invalid:${name}`);
}

async function fetchStep(id: string): Promise<FetchStep> {
    await Promise.resolve();
    return {
        id,
        payload: `payload:${id}`
    };
}

async function pauseTrace(order: string[]): Promise<void> {
    order.push("async:before await");
    await Promise.resolve();
    order.push("async:after await");
}

function abortableOperation(signal: AbortSignal): Promise<string> {
    if (signal.aborted) {
        return Promise.reject(new Error("operation aborted before start"));
    }

    return new Promise((resolve, reject) => {
        const onAbort = (): void => {
            signal.removeEventListener("abort", onAbort);
            reject(new Error("operation aborted"));
        };

        signal.addEventListener("abort", onAbort, { once: true });
        queueMicrotask(() => {
            if (signal.aborted) {
                return;
            }
            signal.removeEventListener("abort", onAbort);
            resolve("operation completed");
        });
    });
}

// =============================================================================
// 24.1 async 函数总是返回 Promise
// =============================================================================
//
// async 函数即使 return 一个普通值，调用方拿到的也是 Promise。
// 这让同步值、异步值和异常传播都进入统一的 Promise 模型。
//
// C++ 对照：
//   async 函数返回 Promise<T>，可以类比返回 future<T> 的函数。
//   但 JS Promise 的回调调度由事件循环和微任务队列控制，不等于创建线程。
//
// C++ 思维提示：
//   不要把 async 写在函数前就理解成“后台线程”。async 只是把返回值和异常包装进 Promise。

async function demoAsyncReturnPromise(): Promise<void> {
    section("24.1 async 函数总是返回 Promise");
    note("C++ 对照", "async 函数返回 Promise<T>，直觉接近返回 future<T>，但不是线程。");

    async function answer(): Promise<number> {
        return 42;
    }

    async function fail(): Promise<number> {
        throw new Error("boom");
    }

    const promise = answer();
    let failure = "not failed";
    try {
        await fail();
    } catch (error: unknown) {
        failure = errorMessage(error);
    }

    showJson("async 返回值形状", {
        directReturnType: promise.constructor.name,
        awaitedValue: await promise,
        failure
    });

    note("输出解释", "answer() 立即返回 Promise；await 后才得到 42。throw 会变成 rejected Promise。");
    note("常见坑", "忘记 await 会拿到 Promise 对象本身，而不是里面的业务值。");
}

// =============================================================================
// 24.2 await 的暂停语义：暂停当前 async 函数，不暂停整个线程
// =============================================================================
//
// await 会让当前 async 函数在该点暂停，并把后续代码排到微任务恢复。
// 调用方如果没有 await 这个 async 函数，会继续执行后面的同步代码。
//
// C++ 对照：
//   这和 co_await 有相似直觉：当前协程暂停，控制权交还调度器。
//   但 JS 中这里的调度器是事件循环/微任务，不是任意线程池。
//
// C++ 思维提示：
//   await 只让出当前 async 函数的执行权。CPU 密集循环不会因为写了 await 关键字就自动并行。

async function demoAwaitPause(): Promise<void> {
    section("24.2 await 的暂停语义：暂停当前 async 函数，不暂停整个线程");
    note("C++ 对照", "await 类似 co_await 暂停当前协程；JS 通过微任务恢复后续代码。");

    const order: string[] = [];
    order.push("caller:before call");
    const running = pauseTrace(order);
    order.push("caller:after call");
    await running;
    order.push("caller:after await");

    showJson("await 暂停顺序", order);
    note("输出解释", "async 函数会先同步执行到第一个 await；调用方随后继续，恢复发生在微任务中。");
    note("常见坑", "await 后面的代码不是立即执行；共享状态在暂停期间可能已被调用方修改。");
}

// =============================================================================
// 24.3 try/catch + await：把 rejected Promise 收敛到局部边界
// =============================================================================
//
// await 一个 rejected Promise 时，rejection 会在 await 位置表现为 throw。
// 因此 try/catch 能像同步代码一样收敛异步错误。
//
// C++ 对照：
//   这类似在 future.get() 或 co_await 点观察异步失败。
//   失败不会凭空跨越到无关调用栈；你必须在 await 边界处理。
//
// C++ 思维提示：
//   catch 变量在本教程中是 unknown。先收窄，再读 message。

async function demoTryCatchAwait(): Promise<void> {
    section("24.3 try/catch + await：把 rejected Promise 收敛到局部边界");
    note("C++ 对照", "异步失败在 await 点进入 catch，类似在等待结果时观察异常。");

    const ok = await loadConfigValue("target");
    let failure = "not failed";

    try {
        await failValidation("port");
    } catch (error: unknown) {
        failure = errorMessage(error);
    }

    showJson("try/catch + await", {
        ok,
        failure
    });

    note("输出解释", "loadConfigValue 成功返回；failValidation 的 rejected Promise 在 await 位置进入 catch。");
    note("类型边界", "catch 到的 reason 是 unknown，不保证一定是 Error；这里用 errorMessage 统一转换。");
}

// =============================================================================
// 24.4 串行 await vs 并行 Promise.all
// =============================================================================
//
// 串行：
//   const a = await taskA();
//   const b = await taskB();
// taskB 会等 taskA 完成后才开始。
//
// 并行：
//   const aPromise = taskA();
//   const bPromise = taskB();
//   const [a, b] = await Promise.all([aPromise, bPromise]);
// 两个任务先都创建，再一起等待。
//
// C++ 对照：
//   这像先后等待两个 future，和先启动两个异步任务再 wait_all 的区别。
//
// C++ 思维提示：
//   Promise.all 不会自动开线程；并行度来自任务本身的异步 I/O 或调度方式。

async function demoSerialVsParallel(): Promise<void> {
    section("24.4 串行 await vs 并行 Promise.all");
    note("C++ 对照", "串行 await 像逐个等待；Promise.all 像先创建任务再 wait_all。");

    const serialTrace: string[] = [];
    serialTrace.push("start:a");
    const serialA = await fetchStep("a");
    serialTrace.push(`done:${serialA.id}`);
    serialTrace.push("start:b");
    const serialB = await fetchStep("b");
    serialTrace.push(`done:${serialB.id}`);

    const parallelTrace: string[] = [];
    parallelTrace.push("start:a");
    const parallelA = fetchStep("a");
    parallelTrace.push("start:b");
    const parallelB = fetchStep("b");
    const parallelResult = await Promise.all([parallelA, parallelB]);
    parallelTrace.push(...parallelResult.map((item) => `done:${item.id}`));

    showJson("串行与并行等待", {
        serialTrace,
        serialPayloads: [serialA.payload, serialB.payload],
        parallelTrace,
        parallelPayloads: parallelResult.map((item) => item.payload)
    });

    note("输出解释", "并行版本先创建两个 Promise，再统一 await；串行版本 b 在 a 完成后才开始。");
    note("常见坑", "互相依赖的任务不要强行 Promise.all；独立 I/O 任务才适合并行等待。");
}

// =============================================================================
// 24.5 循环中的 await：for-of、map 和 forEach 的区别
// =============================================================================
//
// for-of + await：
//   适合必须按顺序执行的异步任务。
//
// Promise.all + map：
//   适合独立任务并行执行，并统一等待全部结果。
//
// forEach(async () => ...):
//   forEach 不会等待 callback 返回的 Promise。入门阶段避免这种写法。
//
// C++ 对照：
//   这像区分串行循环、批量提交 future、以及“提交后不等待”的 fire-and-forget。
//
// C++ 思维提示：
//   fire-and-forget 需要明确生命周期和错误处理；否则 rejected Promise 很容易丢失。

async function demoAwaitInLoops(): Promise<void> {
    section("24.5 循环中的 await：for-of、map 和 forEach 的区别");
    note("C++ 对照", "循环 await 要区分串行、批量等待和 fire-and-forget。");

    const ids = ["parse", "check", "emit"] as const;

    const serial: string[] = [];
    for (const id of ids) {
        const result = await fetchStep(id);
        serial.push(result.payload);
    }

    const parallel = await Promise.all(ids.map((id) => fetchStep(id)));

    printSnippet("避免的写法", [
        "ids.forEach(async (id) => {",
        "    await fetchStep(id);",
        "});",
        "// forEach 不会等待这些 Promise，也无法自然收敛错误"
    ]);

    showJson("循环 await 对比", {
        serial,
        parallel: parallel.map((item) => item.payload)
    });

    note("输出解释", "for-of 明确串行等待；map + Promise.all 明确并行等待。");
    note("常见坑", "不要用 async forEach 表达必须等待完成的流程；用 for-of 或 Promise.all。");
}

// =============================================================================
// 24.6 顶层 await：模块加载也可以有异步边界
// =============================================================================
//
// ESM 支持顶层 await。本教程每章末尾都有：
//   await runIfMain(import.meta.url, runChapter);
//
// 这让章节入口可以同时支持同步和异步 runChapter。
// 但顶层 await 会影响模块加载：导入方必须等待该模块的顶层异步工作完成。
//
// C++ 对照：
//   这有点像动态库加载时执行初始化逻辑。初始化越重，依赖方越难预测加载成本。
//
// C++ 思维提示：
//   顶层 await 应保持短小、可失败边界清晰。不要在顶层做长时间网络调用或启动后台资源。

async function demoTopLevelAwait(): Promise<void> {
    section("24.6 顶层 await：模块加载也可以有异步边界");
    note("C++ 对照", "顶层 await 像模块加载期初始化；越重越影响依赖方。");

    printSnippet("本教程章节入口", [
        "export async function runChapter(): Promise<void> {",
        "    await demoSomething();",
        "}",
        "",
        "await runIfMain(import.meta.url, runChapter);"
    ]);

    showJson("顶层 await 边界", {
        currentModule: import.meta.url,
        directRunHelper: "runIfMain 只在当前文件作为 Node 入口时运行章节",
        whyUseful: "章节可以安全导出 runChapter 给 registry，也可以独立 node dist/... 运行"
    });

    note("输出解释", "import.meta.url 是当前 ESM 模块地址，runIfMain 用它判断是否直接运行。");
    note("常见坑", "库模块顶层 await 过重会拖慢所有导入者；优先把异步初始化放进显式函数。");
}

// =============================================================================
// 24.7 AbortController：给异步操作设计取消边界
// =============================================================================
//
// Promise 本身没有标准 cancel 方法。
// Web/Node 生态常用 AbortController/AbortSignal 把“请求取消”传给支持取消的 API。
//
// 本章不访问网络，而是用一个本地 abortableOperation 演示取消协议。
//
// C++ 对照：
//   AbortSignal 像传入一个取消令牌。任务必须主动观察它，取消才会生效。
//
// C++ 思维提示：
//   取消不是强杀线程。它是协作式协议：调用方发出 signal，任务在合适位置退出并清理资源。

async function demoAbortController(): Promise<void> {
    section("24.7 AbortController：给异步操作设计取消边界");
    note("C++ 对照", "AbortSignal 像协作式取消令牌；任务必须主动观察它。");

    const completed = await abortableOperation(new AbortController().signal);

    const controller = new AbortController();
    const pending = abortableOperation(controller.signal);
    controller.abort();

    let aborted = "not aborted";
    try {
        await pending;
    } catch (error: unknown) {
        aborted = errorMessage(error);
    }

    showJson("AbortController 示例", {
        completed,
        aborted
    });

    note("输出解释", "第一个操作正常完成；第二个操作在 queueMicrotask 恢复前收到 abort 并 rejected。");
    note("副作用边界", "abortableOperation 会移除 abort listener；本章没有残留定时器、网络请求或 watcher。");
}

// =============================================================================
// 24.8 工程场景：入口层统一返回 OperationResult
// =============================================================================
//
// async/await 能让入口层把异常收敛成稳定结果。
// 业务函数可以 throw，入口函数负责转换成：
//   - CLI exit code
//   - HTTP 响应
//   - 日志记录
//   - Result 风格对象
//
// C++ 对照：
//   这类似 main() 或服务边界捕获异常后转成错误码/响应，而不是让异常穿透到进程外。
//
// C++ 思维提示：
//   异步入口必须 await 业务 Promise，否则 try/catch 捕获不到后续 rejection。

async function runOperation(name: string): Promise<OperationResult> {
    try {
        const value = name === "bad"
            ? await failValidation(name)
            : await loadConfigValue(name);
        return { ok: true, value };
    } catch (error: unknown) {
        return { ok: false, error: errorMessage(error) };
    }
}

async function demoEntryBoundary(): Promise<void> {
    section("24.8 工程场景：入口层统一返回 OperationResult");
    note("C++ 对照", "入口层像 main()/HTTP handler，把异常收敛成稳定协议。");

    showJson("入口层结果", [
        await runOperation("target"),
        await runOperation("bad")
    ]);

    note("输出解释", "业务失败没有泄漏成未处理 rejection，而是转换成 { ok:false, error }。");
    note("类型边界", "OperationResult 是类型层协议；外部 JSON/HTTP 响应仍需要运行时序列化和校验。");
}

// =============================================================================
// 24.9 本章复盘
// =============================================================================
//
// C++ 对照：
//   async/await 和 co_await 的直觉相似：代码看起来线性，执行可以在 await 点暂停。
//   但 JS 的暂停和恢复由 Promise 微任务模型决定，不是默认多线程。
//
// C++ 思维提示：
//   先判断任务是否独立，再决定串行 await 还是 Promise.all；先设计取消协议，再谈超时和终止。

async function demoChapterReview(): Promise<void> {
    section("24.9 本章复盘");
    note("C++ 对照", "async/await 提供线性写法，但底层仍是 Promise 和事件循环。");

    showJson("关键结论", [
        "async 函数总是返回 Promise",
        "await 暂停当前 async 函数，不暂停整个线程",
        "rejected Promise 会在 await 点进入 try/catch",
        "独立任务适合先创建 Promise，再 Promise.all 等待",
        "for-of + await 表达串行；map + Promise.all 表达并行等待",
        "async forEach 不会等待 callback，应避免用于关键流程",
        "顶层 await 会影响模块加载，应保持轻量",
        "Promise 本身不能取消；AbortController 是协作式取消协议"
    ]);

    note("可重复运行", "本章只使用 Promise.resolve 和 queueMicrotask 模拟异步，不访问网络、文件或定时器。");
    note("下一章衔接", "ch25 会集中比较 C++ 线程模型与 Node 事件循环。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 24 demos in a stable async order. */
export async function runChapter(): Promise<void> {
    await demoAsyncReturnPromise();
    await demoAwaitPause();
    await demoTryCatchAwait();
    await demoSerialVsParallel();
    await demoAwaitInLoops();
    await demoTopLevelAwait();
    await demoAbortController();
    await demoEntryBoundary();
    await demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
