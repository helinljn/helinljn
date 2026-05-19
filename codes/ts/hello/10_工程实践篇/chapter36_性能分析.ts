// =============================================================================
// 第 36 章：性能分析
// =============================================================================
//
// 【学习目标】
//   1. 掌握 performance.now、mark/measure、console.time、hrtime.bigint 的使用边界
//   2. 理解 V8 --prof、--prof-process 和火焰图工具 0x 的定位
//   3. 识别同步阻塞、内存泄漏、重复 JSON 序列化等常见性能陷阱
//
// 【运行方式】
//   npm run build
//   node dist/10_工程实践篇/chapter36_性能分析.js
//   或 npm run chapter -- 36（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { EventEmitter } from "node:events";
import { performance } from "node:perf_hooks";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type RequestMetrics = {
    count: number;
    errors: number;
    durationsMs: number[];
};

// =============================================================================
// 36.1 performance.now()：高精度单调时间戳
// =============================================================================
//
// C++ 对照：performance.now() 更接近 std::chrono::steady_clock，用于测量间隔；Date.now()
// 更接近 system_clock，可能受系统时间调整影响。
//
// C++ 思维提示：性能测量要用单调时钟，不要用当前墙上时间推断耗时。本章不输出真实耗时，
// 避免教程运行结果随机器和负载变化。

function demoPerformanceNow(): void {
    section("36.1 performance.now()：高精度单调时间戳");
    note("C++ 对照", "performance.now() 适合测 duration；Date.now() 适合记录墙上时间。");

    const start = performance.now();
    const end = performance.now();

    showJson("performance.now 边界", {
        unit: "milliseconds",
        monotonicForDuration: true,
        valuesAreNumbers: Number.isFinite(start) && Number.isFinite(end),
        endIsNotBeforeStart: end >= start,
        valuePrinted: false
    });
    note("常见坑", "不要把一次微小操作的单次耗时当结论；需要多次采样、预热和控制环境。");
}

// =============================================================================
// 36.2 mark/measure：给关键阶段打标
// =============================================================================
//
// C++ 对照：performance.mark/measure 像在代码路径中插入命名采样点，再生成区间测量。
//
// C++ 思维提示：命名阶段比散落的临时计时变量更适合定位慢点。测量结果仍要按统计方式看，
// 不要输出一次运行的绝对值作为测试断言。

function demoPerformanceMarks(): void {
    section("36.2 mark/measure：给关键阶段打标");
    note("C++ 对照", "mark/measure 让耗时区间有名字，便于和日志或 trace 对齐。");

    performance.clearMarks();
    performance.clearMeasures();
    performance.mark("config:start");
    const config = { strict: true, target: "ES2023" };
    JSON.stringify(config);
    performance.mark("config:end");
    performance.measure("config:serialize", "config:start", "config:end");

    const measure = performance.getEntriesByName("config:serialize", "measure")[0];
    showJson("mark/measure 结果形状", {
        markNames: performance.getEntriesByType("mark").map((entry) => entry.name),
        measureName: measure?.name,
        measureEntryType: measure?.entryType,
        durationIsFinite: typeof measure?.duration === "number" && Number.isFinite(measure.duration),
        durationPrinted: false
    });
    performance.clearMarks();
    performance.clearMeasures();
    note("输出解释", "本章只验证测量对象存在，不打印 duration，保证输出可重复。");
}

// =============================================================================
// 36.3 console.time/timeEnd：快速局部计时，不适合稳定报告
// =============================================================================
//
// C++ 对照：console.time 像临时 printf 计时；方便但难以结构化，也不适合自动化断言。
//
// C++ 思维提示：console.timeEnd 会直接向控制台输出真实耗时。本章只展示模式，不实际调用，
// 避免运行输出随环境漂移。

function demoConsoleTiming(): void {
    section("36.3 console.time/timeEnd：快速局部计时，不适合稳定报告");
    note("C++ 对照", "console.time 适合临时排查，正式监控应输出结构化 duration 字段或 metrics。");

    showJson("console.time 模式", {
        snippet: [
            "console.time(\"load-config\");",
            "loadConfig();",
            "console.timeEnd(\"load-config\");"
        ],
        outputBoundary: "prints real elapsed time to console",
        chapterDecision: "do not call it on the runnable path"
    });
    note("常见坑", "计时 label 要成对出现；重复 label 或漏掉 timeEnd 会让调试输出混乱。");
}

// =============================================================================
// 36.4 process.hrtime.bigint()：纳秒精度时间戳
// =============================================================================
//
// C++ 对照：hrtime.bigint() 像纳秒级 steady clock 读数，适合底层测量和与旧 Node 代码兼容。
//
// C++ 思维提示：纳秒单位不代表你的测量真的有纳秒级准确性。计时精度、调度、JIT、GC 都会
// 影响结果。

function demoHrtimeBigint(): void {
    section("36.4 process.hrtime.bigint()：纳秒精度时间戳");
    note("C++ 对照", "hrtime.bigint() 返回 bigint，可用于计算纳秒级间隔。");

    const start = process.hrtime.bigint();
    const end = process.hrtime.bigint();
    const delta = end - start;

    showJson("hrtime.bigint 形状", {
        type: typeof start,
        unit: "nanoseconds",
        deltaIsNonNegative: delta >= 0n,
        valuePrinted: false,
        conversionHint: "Number(delta) / 1_000_000 converts ns to ms when the value is safely small"
    });
    note("常见坑", "不要把 bigint 纳秒值直接 JSON.stringify；需要先转成字符串或可接受范围内的 number。");
}

// =============================================================================
// 36.5 V8 采样分析：--prof 与 --prof-process
// =============================================================================
//
// C++ 对照：node --prof 类似 perf 采样，先生成原始采样日志，再用工具解析成可读报告。
//
// C++ 思维提示：采样分析看的是热点分布，不是每一行精确耗时。先复现负载，再采样，再对热点
// 做小步优化和回归验证。

function demoV8Profiler(): void {
    section("36.5 V8 采样分析：--prof 与 --prof-process");
    note("C++ 对照", "V8 profiler 类似 perf：采样 CPU 栈，帮助找热点。");

    showJson("V8 profiling workflow", {
        collect: "node --prof dist/main.js chapter 36",
        output: "isolate-0x...-v8.log",
        process: "node --prof-process isolate-0x...-v8.log > processed.txt",
        reportAreas: ["JavaScript", "C++", "Shared libraries", "Bottom up profile"],
        cleanupRule: "profile logs are generated artifacts; do not commit large profiler outputs"
    });
    note("可运行边界", "本章不执行 --prof，避免生成 isolate 日志文件；只说明工作流。");
}

// =============================================================================
// 36.6 火焰图工具 0x：把采样栈可视化
// =============================================================================
//
// C++ 对照：火焰图类似 perf script + flamegraph，把采样栈宽度可视化成热点。
//
// C++ 思维提示：火焰图宽度代表采样占比，不代表调用次数。看宽块，再回到代码验证假设。

function demoFlamegraphTool(): void {
    section("36.6 火焰图工具 0x：把采样栈可视化");
    note("C++ 对照", "0x 是 Node 生态中生成火焰图的常用工具，定位类似 perf + flamegraph。");

    showJson("0x 使用边界", {
        installBoundary: "not installed in this tutorial package",
        typicalCommand: "npm exec -- 0x dist/main.js chapter 36",
        output: "interactive flamegraph HTML",
        readingGuide: [
            "wide blocks are hot paths",
            "top frames are leaf work",
            "compare before/after optimization under same workload"
        ]
    });
    note("常见坑", "不要为了漂亮火焰图而在生产机器上随意采样；先考虑负载、权限和敏感路径。");
}

// =============================================================================
// 36.7 常见性能陷阱：同步阻塞事件循环
// =============================================================================
//
// C++ 对照：在 Node 主线程上做同步 I/O 或 CPU 大循环，类似把 GUI/main thread 卡住。
//
// C++ 思维提示：C++ 可以把阻塞工作放到线程；Node 中也要显式使用异步 I/O、worker、进程
// 或外部服务。Promise 包一层不会让 CPU 循环自动并行。

function demoSyncBlockingPitfall(): void {
    section("36.7 常见性能陷阱：同步阻塞事件循环");
    note("C++ 对照", "同步大文件 I/O 和 CPU 大循环都会阻塞 Node 主线程。");

    showJson("阻塞风险清单", [
        {
            pitfall: "readFileSync on large file",
            effect: "event loop cannot handle timers, HTTP requests, or callbacks while blocked",
            better: "stream or fs/promises with clear concurrency limits"
        },
        {
            pitfall: "CPU-heavy JSON transform in request handler",
            effect: "all requests wait behind one handler",
            better: "worker thread, background job, or smaller chunks"
        },
        {
            pitfall: "new Promise(() => heavyCpuLoop())",
            effect: "executor still runs synchronously",
            better: "move CPU work out of main thread"
        }
    ]);
    note("常见坑", "async 关键字不等于异步执行 CPU 工作；函数会同步运行到第一个 await。");
}

// =============================================================================
// 36.8 常见性能陷阱：内存泄漏与重复 JSON 序列化
// =============================================================================
//
// C++ 对照：闭包持有大对象、全局缓存无限增长、EventEmitter 监听器不移除，直觉上都像忘记释放
// 或忘记解除引用。
//
// C++ 思维提示：JS 有 GC，但 GC 只能回收不可达对象。只要全局数组、闭包或 listener 还引用
// 对象，它就不是垃圾。

function demoMemoryAndJsonPitfalls(): void {
    section("36.8 常见性能陷阱：内存泄漏与重复 JSON 序列化");
    note("C++ 对照", "GC 不等于没有泄漏；可达引用仍会让对象长期存活。");

    const emitter = new EventEmitter();
    const listener = (): void => {
        // demo listener intentionally empty
    };
    emitter.on("data", listener);
    const beforeCleanup = emitter.listenerCount("data");
    emitter.off("data", listener);
    const afterCleanup = emitter.listenerCount("data");

    const payload = { id: "item-001", tags: ["ts", "node"], active: true };
    const once = JSON.stringify(payload);
    const repeated = [once, once, once];

    showJson("内存与序列化边界", {
        eventEmitter: {
            beforeCleanup,
            afterCleanup,
            rule: "remove listener when lifecycle ends"
        },
        closureLeak: "avoid long-lived closures capturing large request objects",
        globalCache: "bound cache size and eviction policy",
        jsonSerialization: {
            oneSerializedByteLength: Buffer.byteLength(once),
            repeatedCount: repeated.length,
            rule: "serialize once per boundary when possible"
        }
    });
    note("常见坑", "把完整请求对象塞进全局数组做调试，会让请求体、headers 和用户数据都无法回收。");
}

// =============================================================================
// 36.9 轻量可观测性：计数器、耗时分布、错误率
// =============================================================================
//
// C++ 对照：metrics 像服务进程内部计数器和直方图；tracing 像跨函数/跨服务调用链；logs 是事件。
//
// C++ 思维提示：性能分析不是只看单次耗时。服务状态通常要看吞吐、错误率、延迟分布和热点路径。

function percentile(values: readonly number[], p: number): number {
    if (values.length === 0) {
        return 0;
    }
    const sorted = [...values].sort((left, right) => left - right);
    const rawIndex = Math.ceil((p / 100) * sorted.length) - 1;
    const index = Math.min(Math.max(rawIndex, 0), sorted.length - 1);
    return sorted[index] ?? 0;
}

function summarizeMetrics(metrics: RequestMetrics): {
    count: number;
    errors: number;
    errorRate: number;
    p50Ms: number;
    p95Ms: number;
} {
    return {
        count: metrics.count,
        errors: metrics.errors,
        errorRate: metrics.count === 0 ? 0 : metrics.errors / metrics.count,
        p50Ms: percentile(metrics.durationsMs, 50),
        p95Ms: percentile(metrics.durationsMs, 95)
    };
}

function demoLightObservability(): void {
    section("36.9 轻量可观测性：计数器、耗时分布、错误率");
    note("C++ 对照", "用计数器和分布观察服务整体状态，类似服务端自带性能计数器。");

    const metrics: RequestMetrics = {
        count: 5,
        errors: 1,
        durationsMs: [12, 18, 24, 35, 80]
    };

    showJson("轻量 metrics 摘要", {
        summary: summarizeMetrics(metrics),
        logsBoundary: "logs explain individual events",
        metricsBoundary: "metrics explain aggregate health",
        tracesBoundary: "traces explain one request path"
    });
    note("本章复盘", "先测量，再优化；先找热点，再改代码；每次优化后用同一负载验证收益。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 36 demos. */
export function runChapter(): void {
    demoPerformanceNow();
    demoPerformanceMarks();
    demoConsoleTiming();
    demoHrtimeBigint();
    demoV8Profiler();
    demoFlamegraphTool();
    demoSyncBlockingPitfall();
    demoMemoryAndJsonPitfalls();
    demoLightObservability();
}

await runIfMain(import.meta.url, runChapter);
