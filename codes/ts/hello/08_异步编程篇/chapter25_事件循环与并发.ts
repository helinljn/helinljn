// =============================================================================
// 第 25 章：事件循环与并发
// =============================================================================
//
// 【学习目标】
//   1. 建立 C++ 线程模型与 Node 事件循环模型的核心差异
//   2. 理解调用栈、宏任务队列、微任务队列与 Node 专有 nextTick 队列
//   3. 认识 Node 事件循环六个阶段，以及 setTimeout/setImmediate/nextTick 的边界
//   4. 了解 Worker Threads 的适用场景、退出路径和单线程模型优缺点
//
// 【运行方式】
//   npm run build
//   node dist/08_异步编程篇/chapter25_事件循环与并发.js
//   或 npm run chapter -- 25（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { Worker } from "node:worker_threads";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type QueueEvent = {
    step: number;
    queue: string;
    description: string;
};

type LoopPhase = {
    order: number;
    phase: string;
    typicalCallbacks: readonly string[];
    beginnerRule: string;
};

type WorkerResult = {
    input: number;
    output: number | null;
    status: string;
};

function printSnippet(label: string, lines: readonly string[]): void {
    console.log(`${label}:`);
    for (const line of lines) {
        console.log(`  ${line}`);
    }
}

function errorMessage(error: unknown): string {
    return error instanceof Error ? error.message : String(error);
}

function busyButSmallChecksum(limit: number): number {
    let checksum = 0;
    for (let index = 1; index <= limit; index++) {
        checksum += index * 3;
    }
    return checksum;
}

function waitForNextTick(): Promise<string> {
    return new Promise((resolve) => {
        process.nextTick(() => {
            resolve("process.nextTick callback completed");
        });
    });
}

function waitForTimer(): Promise<string> {
    return new Promise((resolve) => {
        const handle = setTimeout(() => {
            clearTimeout(handle);
            resolve("setTimeout(0) callback completed");
        }, 0);
    });
}

function waitForImmediate(): Promise<string> {
    return new Promise((resolve) => {
        const handle = setImmediate(() => {
            clearImmediate(handle);
            resolve("setImmediate callback completed");
        });
    });
}

function parseWorkerOutput(message: unknown): number | null {
    if (typeof message !== "object" || message === null || !("output" in message)) {
        return null;
    }
    const output = (message as { output?: unknown }).output;
    return typeof output === "number" ? output : null;
}

async function runWorkerSquare(input: number): Promise<WorkerResult> {
    const source = [
        'const { parentPort, workerData } = require("node:worker_threads");',
        "parentPort.postMessage({ output: workerData.input * workerData.input });"
    ].join("\n");

    let worker: Worker;
    try {
        worker = new Worker(source, {
            eval: true,
            workerData: { input }
        });
    } catch (error: unknown) {
        return {
            input,
            output: null,
            status: `worker unavailable: ${errorMessage(error)}`
        };
    }

    return new Promise((resolve) => {
        let settled = false;

        const finish = (result: WorkerResult): void => {
            if (settled) {
                return;
            }
            settled = true;
            clearTimeout(watchdog);
            worker.removeAllListeners();
            worker.terminate()
                .then(() => resolve(result))
                .catch((error: unknown) => {
                    resolve({
                        input,
                        output: result.output,
                        status: `${result.status}; terminate warning: ${errorMessage(error)}`
                    });
                });
        };

        const watchdog = setTimeout(() => {
            finish({
                input,
                output: null,
                status: "worker timeout"
            });
        }, 1000);

        worker.once("message", (message: unknown) => {
            finish({
                input,
                output: parseWorkerOutput(message),
                status: "worker completed"
            });
        });

        worker.once("error", (error: Error) => {
            finish({
                input,
                output: null,
                status: `worker error: ${error.message}`
            });
        });

        worker.once("exit", (code) => {
            if (!settled && code !== 0) {
                finish({
                    input,
                    output: null,
                    status: `worker exited with code ${code}`
                });
            }
        });
    });
}

// =============================================================================
// 25.1 核心差异：C++ 线程 vs Node 事件循环
// =============================================================================
//
// C++ 服务端程序常见模型：
//   - 每连接一个线程
//   - 线程池处理任务
//   - 使用 mutex/condition_variable/atomic 协调共享状态
//
// Node 默认模型：
//   - JavaScript 主线程执行用户代码
//   - I/O 回调、定时器和微任务由事件循环调度
//   - 部分底层 I/O 由 libuv/系统线程池协助，但 JS 回调仍回到主线程执行
//
// C++ 对照：
//   不要把 Promise 或 callback 直接映射成 std::thread。
//   Node 的并发主要是“单线程事件循环 + 非阻塞 I/O + 回调调度”。
//
// C++ 思维提示：
//   如果你把 CPU 密集循环放进 JS 主线程，事件循环就无法处理其他回调；这和锁住一个 UI 线程很像。

async function demoThreadVsEventLoop(): Promise<void> {
    section("25.1 核心差异：C++ 线程 vs Node 事件循环");
    note("C++ 对照", "Promise/callback 不是 std::thread；Node 默认靠事件循环调度 JS 回调。");

    showJson("模型对比", [
        {
            topic: "执行单位",
            cpp: "多个 OS 线程可同时执行用户代码",
            node: "一个 JS 主线程执行用户 JS 回调"
        },
        {
            topic: "I/O 等待",
            cpp: "可能阻塞线程，也可能用 epoll/io_uring/asio",
            node: "常用非阻塞 I/O，把完成事件交回事件循环"
        },
        {
            topic: "共享内存",
            cpp: "线程共享内存，需要锁和原子操作",
            node: "主线程天然避免多数 JS 共享内存竞争；Worker 另算"
        },
        {
            topic: "CPU 密集任务",
            cpp: "可用线程池分摊",
            node: "会阻塞事件循环，必要时用 Worker Threads 或外部服务"
        }
    ]);

    note("输出解释", "Node 不是没有线程；关键是用户 JS 回调默认在同一个主线程上串行执行。");
    note("常见坑", "把 await 当成线程切换会误判性能；await 只是把后续代码排到 Promise 恢复路径。");
}

// =============================================================================
// 25.2 调用栈、任务队列和微任务队列
// =============================================================================
//
// 同步函数先进入调用栈，执行完当前同步栈以后，事件循环才有机会处理后续队列。
//
// 常见队列：
//   - nextTick queue：Node 专有，优先级很高
//   - microtask queue：Promise.then、queueMicrotask
//   - macrotask / event loop phases：timer、I/O、check 等阶段的回调
//
// C++ 对照：
//   这不是抢占式线程调度。当前 JS 调用栈不返回，后面的回调就没有机会执行。
//
// C++ 思维提示：
//   单线程事件循环的核心约束是“不要长时间占用调用栈”。

async function demoQueuesAndStack(): Promise<void> {
    section("25.2 调用栈、任务队列和微任务队列");
    note("C++ 对照", "事件循环不是抢占式线程调度；当前调用栈必须先返回。");

    const order: string[] = [];
    order.push("sync:start");
    Promise.resolve().then(() => {
        order.push("promise.microtask");
    });
    queueMicrotask(() => {
        order.push("queueMicrotask");
    });
    order.push("sync:end");
    await Promise.resolve();

    const queueModel: readonly QueueEvent[] = [
        {
            step: 1,
            queue: "call stack",
            description: "当前同步代码先执行到栈清空"
        },
        {
            step: 2,
            queue: "nextTick / microtask",
            description: "Node 会处理高优先级 nextTick 和 Promise 微任务"
        },
        {
            step: 3,
            queue: "event loop phases",
            description: "再进入 timers、poll、check 等阶段处理宏任务回调"
        }
    ];

    showJson("微任务观察", order);
    showJson("队列模型", queueModel);
    note("输出解释", "同步 start/end 先出现；Promise.then 和 queueMicrotask 在 await 让出后执行。");
    note("常见坑", "无限递归排 microtask 会让事件循环很久进不了 I/O 和 timer 阶段。");
}

// =============================================================================
// 25.3 Node 事件循环六个阶段
// =============================================================================
//
// Node/libuv 的事件循环通常用六个阶段帮助记忆：
//   1. timers
//   2. pending callbacks
//   3. idle, prepare
//   4. poll
//   5. check
//   6. close callbacks
//
// 这不是要求你在业务代码里手写状态机，而是帮助判断“某类回调大概何时运行”。
//
// C++ 对照：
//   它类似一个固定阶段的 reactor loop。网络、文件、timer 的完成事件在不同阶段被取出执行。
//
// C++ 思维提示：
//   事件循环阶段不是并行线程。每个 JS 回调仍会独占主线程直到返回。

async function demoEventLoopPhases(): Promise<void> {
    section("25.3 Node 事件循环六个阶段");
    note("C++ 对照", "Node 事件循环像 reactor loop；不同阶段处理不同类别回调。");

    const phases: readonly LoopPhase[] = [
        {
            order: 1,
            phase: "timers",
            typicalCallbacks: ["setTimeout", "setInterval"],
            beginnerRule: "到期 timer 的回调在这里执行"
        },
        {
            order: 2,
            phase: "pending callbacks",
            typicalCallbacks: ["部分系统 I/O 回调"],
            beginnerRule: "处理上一轮延迟下来的 I/O 回调"
        },
        {
            order: 3,
            phase: "idle, prepare",
            typicalCallbacks: ["libuv 内部使用"],
            beginnerRule: "业务代码通常不直接接触"
        },
        {
            order: 4,
            phase: "poll",
            typicalCallbacks: ["大多数 I/O 事件"],
            beginnerRule: "等待并处理 I/O，是 Node 服务端很核心的阶段"
        },
        {
            order: 5,
            phase: "check",
            typicalCallbacks: ["setImmediate"],
            beginnerRule: "setImmediate 回调在这里执行"
        },
        {
            order: 6,
            phase: "close callbacks",
            typicalCallbacks: ["socket close"],
            beginnerRule: "处理资源关闭回调"
        }
    ];

    showJson("事件循环阶段", phases);
    note("输出解释", "阶段表是理解模型；不要在业务逻辑里依赖过细的内部调度细节。");
    note("常见坑", "timer 到期不代表立刻执行；如果调用栈正忙，timer 回调仍要等待。");
}

// =============================================================================
// 25.4 setTimeout、setImmediate、process.nextTick 的边界
// =============================================================================
//
// 三个 API 常被混淆：
//   - process.nextTick：Node 专有，当前操作结束后尽快执行，优先级高
//   - setTimeout(fn, 0)：进入 timers 阶段，不保证“马上”
//   - setImmediate(fn)：进入 check 阶段
//
// 在顶层脚本里比较 setTimeout(0) 和 setImmediate 的先后，可能受平台和当前事件循环状态影响。
// 因此本章不把它们的相对先后写成断言；只分别演示它们能完成并清理。
//
// C++ 对照：
//   这像把回调投递到不同队列。投递位置不同，不等于创建不同线程。
//
// C++ 思维提示：
//   nextTick 优先级高，滥用会饿死 I/O；setImmediate 更适合把工作推到当前 poll 阶段之后。

async function demoSchedulingApis(): Promise<void> {
    section("25.4 setTimeout、setImmediate、process.nextTick 的边界");
    note("C++ 对照", "这些 API 是队列投递方式，不是线程创建方式。");

    const nextTick = await waitForNextTick();
    const timer = await waitForTimer();
    const immediate = await waitForImmediate();

    showJson("三个调度 API 的稳定演示", {
        nextTick,
        timer,
        immediate
    });

    showJson("顺序判断规则", [
        "process.nextTick 优先级很高，不能无限递归使用",
        "Promise 微任务会在事件循环继续进入宏任务阶段前处理",
        "setTimeout(0) 进入 timers 阶段，但不是实时保证",
        "setImmediate 进入 check 阶段",
        "顶层脚本中 setTimeout(0) 与 setImmediate 的相对先后不要写成跨平台业务契约"
    ]);

    note("输出解释", "本章按 await 顺序分别等待三个 API 完成，而不是把平台相关先后当作测试断言。");
    note("副作用边界", "timer 和 immediate 都在回调触发后清理 handle；没有残留定时器。");
}

// =============================================================================
// 25.5 CPU 密集任务会阻塞事件循环
// =============================================================================
//
// Node 适合 I/O 密集服务，但 CPU 密集任务如果直接跑在主线程，会阻塞事件循环。
// 这时 HTTP 请求、timer、Promise continuation 都要等当前计算返回。
//
// C++ 对照：
//   这像把重计算放在 UI/main thread 上。即使系统还有其他 CPU 核，当前线程也无法处理事件。
//
// C++ 思维提示：
//   Promise 包不住 CPU 计算。把 CPU 循环放进 Promise executor 仍然是同步阻塞。

async function demoCpuBlocking(): Promise<void> {
    section("25.5 CPU 密集任务会阻塞事件循环");
    note("C++ 对照", "CPU 密集循环放在 JS 主线程上，会像阻塞 UI/main thread 一样阻塞事件循环。");

    const checksum = busyButSmallChecksum(1000);
    showJson("小规模 CPU 计算示例", {
        checksum,
        whySmall: "章节只演示概念，不做耗时压测，避免阻塞教程运行"
    });

    printSnippet("不要误用 Promise 包 CPU 循环", [
        "new Promise((resolve) => {",
        "    heavyCpuLoop(); // 仍然同步阻塞 executor",
        "    resolve(undefined);",
        "});"
    ]);

    note("输出解释", "checksum 是固定小计算；真实 CPU 密集任务应考虑 Worker Threads、原生扩展或外部服务。");
    note("常见坑", "async 函数里的 while 大循环没有 await 或拆分点时，仍会长时间占用主线程。");
}

// =============================================================================
// 25.6 Worker Threads：把 CPU 工作移出主线程
// =============================================================================
//
// Worker Threads 允许在同一进程中创建额外 JS 线程。
// 它适合 CPU 密集任务，不适合把所有 I/O 都搬进去。
//
// Worker 有自己的事件循环和 JS 堆。和主线程通信通常使用 postMessage/MessagePort，
// 或在高级场景下使用 SharedArrayBuffer/Atomics。
//
// C++ 对照：
//   Worker 更接近显式创建工作线程。你要设计消息协议、错误处理和退出路径。
//
// C++ 思维提示：
//   Worker 不是免费并发。创建、序列化消息、终止都有成本；小任务未必值得开 Worker。

async function demoWorkerThreads(): Promise<void> {
    section("25.6 Worker Threads：把 CPU 工作移出主线程");
    note("C++ 对照", "Worker Threads 更接近显式工作线程，需要消息协议和退出路径。");

    const workerResult = await runWorkerSquare(7);
    showJson("Worker Threads 最小演示", workerResult);

    note("输出解释", "Worker 用 eval 内联脚本计算平方；主线程收到消息后 terminate，并有 1 秒 watchdog。");
    note("副作用边界", "Worker 成功、错误、非零退出或超时时都会走 finish，确保 terminate 或 fallback。");
}

// =============================================================================
// 25.7 单线程模型的优缺点
// =============================================================================
//
// 优点：
//   - 大多数 JS 业务状态不需要 mutex
//   - I/O 密集服务写法简单
//   - 回调串行执行，很多状态机更容易推理
//
// 缺点：
//   - CPU 密集任务会阻塞全部 JS 回调
//   - 一个回调写得太慢会拖慢整个进程的响应
//   - 需要理解微任务、nextTick、timer、I/O 阶段，避免饿死事件循环
//
// C++ 对照：
//   它减少了共享内存锁竞争，但把“不要阻塞主线程”变成核心工程约束。
//
// C++ 思维提示：
//   Node 并发优化第一步通常不是“加线程”，而是确认是否真的在阻塞事件循环。

async function demoSingleThreadTradeoffs(): Promise<void> {
    section("25.7 单线程模型的优缺点");
    note("C++ 对照", "Node 减少共享内存锁竞争，但强化了不要阻塞主线程的约束。");

    showJson("单线程事件循环取舍", {
        strengths: [
            "I/O 密集场景代码简单",
            "多数业务状态无需 mutex",
            "回调串行执行，局部状态更容易推理"
        ],
        costs: [
            "CPU 密集任务会阻塞所有 JS 回调",
            "滥用 nextTick/microtask 可能饿死 I/O",
            "需要显式设计 Worker、进程或服务拆分来利用多核"
        ]
    });

    note("输出解释", "Node 的默认并发模型适合高 I/O，但不是所有性能问题都能靠 await 解决。");
    note("常见坑", "看到单线程就以为不能并发 I/O，或看到异步就以为能并行 CPU，都是错误直觉。");
}

// =============================================================================
// 25.8 工程场景：选择并发策略
// =============================================================================
//
// 面对一个任务，先问三个问题：
//   1. 它是 I/O 密集还是 CPU 密集？
//   2. 它是否需要共享大量内存状态？
//   3. 它是否需要取消、超时、背压或并发限制？
//
// C++ 对照：
//   这类似在 thread pool、async I/O、进程隔离之间做架构选择。
//
// C++ 思维提示：
//   不要从工具出发选方案。先识别瓶颈，再选 Promise、stream、Worker 或多进程。

async function demoConcurrencyDecision(): Promise<void> {
    section("25.8 工程场景：选择并发策略");
    note("C++ 对照", "先识别瓶颈，再选择 async I/O、Worker、线程池或进程隔离。");

    showJson("策略选择表", [
        {
            workload: "读取多个 HTTP API",
            strategy: "Promise.all + AbortController + 并发限制",
            reason: "I/O 密集，等待外部响应，不应阻塞主线程"
        },
        {
            workload: "压缩大文件或图像处理",
            strategy: "Worker Threads 或外部服务",
            reason: "CPU 密集，放主线程会阻塞事件循环"
        },
        {
            workload: "持续读取大日志",
            strategy: "stream + backpressure",
            reason: "需要控制内存和消费速度"
        },
        {
            workload: "小型配置解析",
            strategy: "普通同步函数",
            reason: "任务很小，过度异步化只会增加复杂度"
        }
    ]);

    note("输出解释", "并发策略应该由工作负载决定，不是所有任务都需要 Promise.all 或 Worker。");
    note("类型边界", "TypeScript 能约束策略配置的形状，但无法自动证明任务是 I/O 密集还是 CPU 密集。");
}

// =============================================================================
// 25.9 本章复盘
// =============================================================================
//
// C++ 对照：
//   Node 事件循环不是 C++ 线程池的语法糖。它是一套围绕非阻塞 I/O、回调队列和单 JS 主线程的模型。
//
// C++ 思维提示：
//   把线程、协程、Promise、事件循环、Worker 分开理解。这样遇到性能或挂起问题时，才知道该看哪一层。

async function demoChapterReview(): Promise<void> {
    section("25.9 本章复盘");
    note("C++ 对照", "Node 事件循环不是线程池语法糖；它是非阻塞 I/O 和回调队列模型。");

    showJson("关键结论", [
        "Node 用户 JS 默认在主线程串行执行",
        "Promise 和 await 不等于创建线程",
        "调用栈不清空，timer、I/O、microtask 都无法继续推进",
        "Node 事件循环可用 timers、pending callbacks、idle/prepare、poll、check、close callbacks 六阶段理解",
        "process.nextTick 优先级高，setTimeout 属于 timers，setImmediate 属于 check",
        "顶层 setTimeout(0) 与 setImmediate 的相对先后不应写成跨平台业务契约",
        "CPU 密集任务要考虑 Worker Threads、进程或外部服务",
        "并发策略取决于工作负载，而不是关键字"
    ]);

    note("可重复运行", "本章 timer/immediate/Worker 都有完成或清理路径，输出不依赖随机数或外部服务。");
    note("下一章衔接", "ch26 会进入 Node 文件系统 API，继续应用异步边界和资源清理原则。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 25 demos in a stable async order. */
export async function runChapter(): Promise<void> {
    await demoThreadVsEventLoop();
    await demoQueuesAndStack();
    await demoEventLoopPhases();
    await demoSchedulingApis();
    await demoCpuBlocking();
    await demoWorkerThreads();
    await demoSingleThreadTradeoffs();
    await demoConcurrencyDecision();
    await demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
