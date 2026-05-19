// =============================================================================
// 第 23 章：Promise 深入
// =============================================================================
//
// 【学习目标】
//   1. 理解 Promise 的 pending/fulfilled/rejected 状态机和单次 settle 规则
//   2. 掌握 then/catch/finally 与链式调用的返回值传播
//   3. 掌握 Promise.all/allSettled/race/any 的差异和错误边界
//   4. 理解 callback promisify、Promise.withResolvers 的版本边界和微任务队列
//
// 【运行方式】
//   npm run build
//   node dist/08_异步编程篇/chapter23_Promise深入.js
//   或 npm run chapter -- 23（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type PromiseState = "pending" | "fulfilled" | "rejected";

type StableTask<T> = {
    name: string;
    promise: Promise<T>;
};

type Deferred<T> = {
    promise: Promise<T>;
    resolve: (value: T | PromiseLike<T>) => void;
    reject: (reason?: unknown) => void;
    source: "native-withResolvers" | "fallback-constructor";
};

type PromiseWithResolvers = typeof Promise & {
    withResolvers?: <T>() => {
        promise: Promise<T>;
        resolve: (value: T | PromiseLike<T>) => void;
        reject: (reason?: unknown) => void;
    };
};

type Callback<T> = (error: Error | null, value?: T) => void;

function printSnippet(label: string, lines: readonly string[]): void {
    console.log(`${label}:`);
    for (const line of lines) {
        console.log(`  ${line}`);
    }
}

function errorMessage(error: unknown): string {
    return error instanceof Error ? error.message : String(error);
}

function makeTask<T>(name: string, value: T): StableTask<T> {
    return {
        name,
        promise: Promise.resolve(value)
    };
}

function makeFailingTask<T>(name: string, message: string): StableTask<T> {
    return {
        name,
        promise: Promise.reject(new Error(message))
    };
}

function makeDeferred<T>(): Deferred<T> {
    const promiseConstructor = Promise as PromiseWithResolvers;
    if (typeof promiseConstructor.withResolvers === "function") {
        return {
            ...promiseConstructor.withResolvers<T>(),
            source: "native-withResolvers"
        };
    }

    let resolveDeferred!: (value: T | PromiseLike<T>) => void;
    let rejectDeferred!: (reason?: unknown) => void;
    const promise = new Promise<T>((resolve, reject) => {
        resolveDeferred = resolve;
        rejectDeferred = reject;
    });

    return {
        promise,
        resolve: resolveDeferred,
        reject: rejectDeferred,
        source: "fallback-constructor"
    };
}

function readLegacySetting(key: string, callback: Callback<string>): void {
    queueMicrotask(() => {
        if (key === "missing") {
            callback(new Error(`setting not found: ${key}`));
            return;
        }
        callback(null, `value:${key}`);
    });
}

function readSettingAsync(key: string): Promise<string> {
    return new Promise((resolve, reject) => {
        readLegacySetting(key, (error, value) => {
            if (error !== null) {
                reject(error);
                return;
            }
            if (value === undefined) {
                reject(new Error("legacy callback returned no value"));
                return;
            }
            resolve(value);
        });
    });
}

// =============================================================================
// 23.1 Promise 状态机：pending、fulfilled、rejected
// =============================================================================
//
// Promise 是“未来某个结果”的占位对象。它只有三种状态：
//   - pending：尚未完成
//   - fulfilled：成功完成，并携带一个值
//   - rejected：失败完成，并携带一个 reason
//
// 关键规则：Promise 只能 settle 一次。
// 一旦 fulfilled 或 rejected，后续 resolve/reject 调用都会被忽略。
//
// C++ 对照：
//   std::promise/std::future 也表达“稍后可取得的结果”，但 JS Promise 的回调调度依赖微任务队列。
//
// C++ 思维提示：
//   Promise 不是线程句柄。创建 Promise 不等于启动新线程；executor 会同步执行，异步性来自它内部安排的任务。

async function demoPromiseStateMachine(): Promise<void> {
    section("23.1 Promise 状态机：pending、fulfilled、rejected");
    note("C++ 对照", "Promise 像 future/promise 的结果通道，但回调由 JS 微任务队列调度。");

    const states: readonly PromiseState[] = ["pending", "fulfilled", "rejected"];
    const events: string[] = [];

    const promise = new Promise<string>((resolve, reject) => {
        events.push("executor runs synchronously");
        resolve("first result");
        reject(new Error("ignored rejection"));
        resolve("ignored second result");
    });

    events.push("after Promise constructor");
    const value = await promise.then((result) => {
        events.push(`then receives ${result}`);
        return result;
    });

    showJson("Promise 状态与单次 settle", {
        possibleStates: states,
        value,
        events
    });

    note("输出解释", "executor 先同步执行；第一个 resolve 生效，后续 reject/resolve 被忽略。");
    note("常见坑", "不要把 Promise executor 当作异步入口；耗时 CPU 代码写在 executor 里仍会阻塞当前线程。");
}

// =============================================================================
// 23.2 then/catch/finally：链式调用与值传播
// =============================================================================
//
// then 返回一个新的 Promise。回调返回普通值时，新 Promise fulfilled 为该值；
// 回调抛异常或返回 rejected Promise 时，新 Promise rejected。
//
// catch 是 then(undefined, onRejected) 的常用写法。
// finally 适合释放资源或记录结束事件，但它通常不改写成功值。
//
// C++ 对照：
//   then 链像把多个 continuation 串起来；catch/finally 更像边界处的错误收敛和清理。
//
// C++ 思维提示：
//   Promise 链不会“跳到另一个线程”。每个 continuation 会排入微任务队列，等待当前同步栈清空。

async function demoThenCatchFinally(): Promise<void> {
    section("23.2 then/catch/finally：链式调用与值传播");
    note("C++ 对照", "then 链是 continuation 管道；catch/finally 用来收敛失败和清理边界。");

    const cleanupLog: string[] = [];

    const success = await Promise.resolve(2)
        .then((value) => value * 3)
        .then((value) => `result:${value}`)
        .finally(() => {
            cleanupLog.push("success cleanup");
        });

    const recovered = await Promise.resolve("raw")
        .then(() => {
            throw new Error("parse failed");
        })
        .catch((error: unknown) => `recovered:${errorMessage(error)}`)
        .finally(() => {
            cleanupLog.push("failure cleanup");
        });

    showJson("链式调用结果", {
        success,
        recovered,
        cleanupLog
    });

    note("输出解释", "finally 两条路径都会执行；catch 把 rejected 链恢复成 fulfilled 字符串。");
    note("常见坑", "catch 后如果返回普通值，后续链会继续走成功路径；想继续失败必须重新 throw。");
}

// =============================================================================
// 23.3 Promise.all：全部成功才成功，失败会快速退出
// =============================================================================
//
// Promise.all 适合多个互相独立但都必须成功的任务。
// 任何一个输入 rejected，all 返回的 Promise 就 rejected。
//
// C++ 对照：
//   它像等待多个 future 全部完成；但失败传播是 Promise 组合方法定义的，不是异常跨线程自动冒泡。
//
// C++ 思维提示：
//   Promise.all 启动不了任务。它只组合已经创建的 Promise；任务是否已经开始取决于 Promise 来源。

async function demoPromiseAll(): Promise<void> {
    section("23.3 Promise.all：全部成功才成功，失败会快速退出");
    note("C++ 对照", "Promise.all 像等待多个 future 全部成功，但它只是组合已有 Promise。");

    const parseTask = makeTask("parse", "parsed");
    const typeTask = makeTask("typecheck", "typed");
    const emitTask = makeFailingTask<string>("emit", "emit failed");

    const allSuccess = await Promise.all([parseTask.promise, typeTask.promise]);

    let failure = "not failed";
    try {
        await Promise.all([parseTask.promise, emitTask.promise]);
    } catch (error: unknown) {
        failure = errorMessage(error);
    }

    showJson("Promise.all 结果", {
        taskNames: [parseTask.name, typeTask.name, emitTask.name],
        allSuccess,
        failure
    });

    note("输出解释", "parse/typecheck 都成功时得到数组；emit 失败时组合 Promise 进入 catch。");
    note("常见坑", "Promise.all 失败不会自动取消其他已经开始的任务；需要 AbortController 或任务自身支持取消。");
}

// =============================================================================
// 23.4 allSettled、race、any：三种不同的组合语义
// =============================================================================
//
// Promise.allSettled：
//   等所有输入都 settle，返回每个结果的状态，适合批处理报表。
//
// Promise.race：
//   第一个 settle 的结果决定整体结果，适合超时或最快响应，但要处理剩余任务。
//
// Promise.any：
//   第一个 fulfilled 的结果决定整体结果；只有全部 rejected 才 rejected。
//
// C++ 对照：
//   这些方法像不同的等待策略：wait-all、wait-first、first-success。
//
// C++ 思维提示：
//   race/any 只是决定组合 Promise 何时 settle，不代表自动停止其他异步操作。

async function demoPromiseCombinators(): Promise<void> {
    section("23.4 allSettled、race、any：三种不同的组合语义");
    note("C++ 对照", "allSettled/race/any 是不同等待策略，不是线程调度器。");

    const allSettledRaw = await Promise.allSettled([
        Promise.resolve("config ok"),
        Promise.reject(new Error("cache miss"))
    ]);
    const allSettled = allSettledRaw.map((result) => result.status === "fulfilled"
        ? { status: result.status, value: result.value }
        : { status: result.status, reason: errorMessage(result.reason) });

    const race = await Promise.race([
        Promise.resolve("memory-cache"),
        Promise.resolve("disk-cache")
    ]);

    const any = await Promise.any([
        Promise.reject(new Error("replica-a failed")),
        Promise.resolve("replica-b ok")
    ]);

    let allRejected = "not rejected";
    try {
        await Promise.any([
            Promise.reject(new Error("first failed")),
            Promise.reject(new Error("second failed"))
        ]);
    } catch (error: unknown) {
        allRejected = error instanceof AggregateError
            ? `AggregateError(${error.errors.length})`
            : errorMessage(error);
    }

    showJson("组合方法对比", {
        allSettled,
        race,
        any,
        allRejected
    });

    note("输出解释", "allSettled 保留成功和失败；race 取第一个 settle；any 取第一个成功。");
    note("类型边界", "allSettled 的 rejected reason 是 unknown 边界，业务代码不能假设一定是 Error。");
}

// =============================================================================
// 23.5 promisify：把 callback API 收敛成 Promise API
// =============================================================================
//
// Node 早期 API 大量使用 callback(error, value) 风格。
// 把 callback 包成 Promise 后，可以用 async/await 和 try/catch 统一错误处理。
//
// C++ 对照：
//   这像把回调式异步接口适配成 future 风格接口，让调用方用同一种等待模型。
//
// C++ 思维提示：
//   promisify 只是适配接口，不会改变底层 I/O 模型。原 API 如果会并发或阻塞，包装后仍然如此。

async function demoPromisify(): Promise<void> {
    section("23.5 promisify：把 callback API 收敛成 Promise API");
    note("C++ 对照", "promisify 像把 callback 风格适配成 future 风格接口。");

    const ok = await readSettingAsync("target");

    let missing = "not failed";
    try {
        await readSettingAsync("missing");
    } catch (error: unknown) {
        missing = errorMessage(error);
    }

    showJson("callback 到 Promise 的适配", {
        ok,
        missing
    });

    note("输出解释", "legacy callback 通过 queueMicrotask 返回结果；Promise 包装后可用 await/catch 处理。");
    note("副作用边界", "本章用 queueMicrotask 模拟 callback，不依赖定时器、文件系统或外部服务。");
}

// =============================================================================
// 23.6 Promise.withResolvers：版本敏感 API 要 feature-detect
// =============================================================================
//
// Promise.withResolvers() 会返回 { promise, resolve, reject }，适合需要把 resolve/reject
// 保存到外部控制流的场景。
//
// 版本边界：
//   本教程 tsconfig lib 是 ES2023。即使当前 Node 运行时可能支持 Promise.withResolvers，
//   编译期 lib 也不一定直接声明该 API。因此本章用显式类型 + feature-detect + fallback。
//
// C++ 对照：
//   这很像 std::promise 和 std::future 拆开持有：promise 端负责完成，future/Promise 端负责等待。
//
// C++ 思维提示：
//   deferred 模式要小心生命周期。resolve/reject 泄漏到太多地方，会让完成路径变得难追踪。

async function demoPromiseWithResolvers(): Promise<void> {
    section("23.6 Promise.withResolvers：版本敏感 API 要 feature-detect");
    note("C++ 对照", "withResolvers 像把 promise 端和等待端拆开持有。");

    const deferred = makeDeferred<string>();
    deferred.resolve("deferred-ready");
    const value = await deferred.promise;

    const rejected = makeDeferred<string>();
    rejected.reject(new Error("deferred-failed"));

    let rejection = "not rejected";
    try {
        await rejected.promise;
    } catch (error: unknown) {
        rejection = errorMessage(error);
    }

    showJson("withResolvers 或 fallback", {
        source: deferred.source,
        value,
        rejection
    });

    note("版本边界", "源码没有直接调用 Promise.withResolvers；通过 feature-detect 避免 ES2023 lib 下的类型缺口。");
    note("常见坑", "不要把 deferred 当作全局事件总线；它适合局部桥接，不适合到处传递 resolve。");
}

// =============================================================================
// 23.7 微任务：then 回调在同步栈清空后执行
// =============================================================================
//
// Promise 的 then/catch/finally 回调属于微任务。
// 当前同步代码执行完以后，事件循环会先清空微任务队列，再进入后续宏任务阶段。
//
// C++ 对照：
//   微任务不是线程调度。它更像当前事件循环 tick 结束前的一段延迟 continuation 队列。
//
// C++ 思维提示：
//   大量递归排入微任务会饿死后续 I/O 阶段。不要用 Promise.then 无限自旋。

async function demoMicrotasks(): Promise<void> {
    section("23.7 微任务：then 回调在同步栈清空后执行");
    note("C++ 对照", "微任务是事件循环内的 continuation 队列，不是 OS 线程。");

    const order: string[] = [];
    order.push("sync:start");

    Promise.resolve().then(() => {
        order.push("promise.then");
    });

    queueMicrotask(() => {
        order.push("queueMicrotask");
    });

    order.push("sync:end");

    await Promise.resolve();

    showJson("微任务顺序", order);
    note("输出解释", "同步代码先完成；随后按入队顺序执行 Promise.then 和 queueMicrotask。");
    note("常见坑", "await 只让出当前 async 函数，不会让 CPU 密集计算自动并行。");
}

// =============================================================================
// 23.8 工程场景：批量任务的稳定错误报告
// =============================================================================
//
// 真实项目中，批量任务经常需要“尽量都跑完，然后汇总成功/失败”。
// 这时 Promise.all 不是最佳选择，因为它会在第一个失败时让组合 Promise rejected。
// allSettled 更适合生成报表。
//
// C++ 对照：
//   这像批量提交多个 future 后收集每个 future 的状态，而不是遇到第一个异常就丢掉其他结果。
//
// C++ 思维提示：
//   批处理里要区分“任务失败”和“调度系统失败”。allSettled 能保留每个任务的失败信息。

async function demoEngineeringBatchReport(): Promise<void> {
    section("23.8 工程场景：批量任务的稳定错误报告");
    note("C++ 对照", "批处理更像收集多个 future 的独立状态，而不是只关心第一个失败。");

    const tasks = [
        makeTask("read-config", "ok"),
        makeFailingTask<string>("compile", "type error"),
        makeTask("write-report", "ok")
    ] as const;

    const settled = await Promise.allSettled(tasks.map((task) => task.promise));
    const report = settled.map((result, index) => {
        const task = tasks[index];
        const taskName = task === undefined ? `unknown-${index}` : task.name;
        return result.status === "fulfilled"
            ? { task: taskName, ok: true, value: result.value }
            : { task: taskName, ok: false, reason: errorMessage(result.reason) };
    });

    showJson("批量任务报告", report);
    note("输出解释", "compile 失败不会阻止 write-report 的结果进入报表。");
    note("类型边界", "result.reason 是 unknown；报告层需要显式转换成稳定字符串。");
}

// =============================================================================
// 23.9 本章复盘
// =============================================================================
//
// C++ 对照：
//   Promise 可以帮助你建立 future/promise 的直觉，但必须牢记 JS 的 executor、微任务和事件循环规则。
//   它不是线程，也不是自动取消机制。
//
// C++ 思维提示：
//   把“组合等待”“错误传播”“任务取消”“并行执行”分开设计，不要把它们都归结为 Promise。

async function demoChapterReview(): Promise<void> {
    section("23.9 本章复盘");
    note("C++ 对照", "Promise 有 future/promise 直觉，但调度由 JS 微任务和事件循环决定。");

    showJson("关键结论", [
        "Promise 有 pending/fulfilled/rejected 三种状态，并且只能 settle 一次",
        "executor 同步执行，then/catch/finally 回调进入微任务队列",
        "then 返回新 Promise，catch 可以恢复失败，也可以重新 throw",
        "Promise.all 适合全部必须成功的任务，但不会自动取消其他任务",
        "allSettled/race/any 分别表达等待全部 settle、第一项 settle、第一项成功",
        "promisify 只是接口适配，不改变底层异步模型",
        "Promise.withResolvers 属于版本敏感 API，本教程采用 feature-detect + fallback",
        "微任务不是线程，CPU 密集任务仍会阻塞主线程"
    ]);

    note("可重复运行", "本章只使用 Promise.resolve/reject 与 queueMicrotask，不访问网络、文件或定时器。");
    note("下一章衔接", "ch24 会把 Promise 组合进一步收敛到 async/await 的线性写法。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 23 demos in a stable async order. */
export async function runChapter(): Promise<void> {
    await demoPromiseStateMachine();
    await demoThenCatchFinally();
    await demoPromiseAll();
    await demoPromiseCombinators();
    await demoPromisify();
    await demoPromiseWithResolvers();
    await demoMicrotasks();
    await demoEngineeringBatchReport();
    await demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
