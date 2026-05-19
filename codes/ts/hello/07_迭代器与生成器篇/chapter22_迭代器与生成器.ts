// =============================================================================
// 第 22 章：迭代器与生成器
// =============================================================================
//
// 【学习目标】
//   1. 理解 Iterable、Iterator、Symbol.iterator 与 for-of 的运行机制
//   2. 掌握 function*、yield、yield*、next() 的同步生成器模型
//   3. 掌握 async function* 与 for await...of 的异步数据流模型
//   4. 能用生成器表达懒加载、分页读取和可提前停止的数据处理
//
// 【运行方式】
//   npm run build
//   node dist/07_迭代器与生成器篇/chapter22_迭代器与生成器.js
//   或 npm run chapter -- 22（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type IteratorTrace<T> = {
    call: string;
    done: boolean;
    value: T | null;
};

type WorkItem = {
    id: string;
    priority: "low" | "normal" | "high";
};

type Page<T> = {
    page: number;
    items: readonly T[];
};

type ProcessedItem = {
    id: string;
    accepted: boolean;
};

function printSnippet(label: string, lines: readonly string[]): void {
    console.log(`${label}:`);
    for (const line of lines) {
        console.log(`  ${line}`);
    }
}

function traceNextCalls<T>(iterator: Iterator<T>, calls: number): IteratorTrace<T>[] {
    const trace: IteratorTrace<T>[] = [];
    for (let index = 1; index <= calls; index++) {
        const result = iterator.next();
        trace.push({
            call: `next #${index}`,
            done: result.done === true,
            value: result.done === true ? null : result.value
        });
    }
    return trace;
}

function take<T>(iterable: Iterable<T>, limit: number): T[] {
    const result: T[] = [];
    for (const item of iterable) {
        result.push(item);
        if (result.length >= limit) {
            break;
        }
    }
    return result;
}

async function collectAsync<T>(iterable: AsyncIterable<T>): Promise<T[]> {
    const result: T[] = [];
    for await (const item of iterable) {
        result.push(item);
    }
    return result;
}

// =============================================================================
// 22.1 for-of 背后的协议：Iterable 与 Iterator
// =============================================================================
//
// for-of 不是数组专用语法。它依赖两个协议：
//
//   Iterable:
//     对象有一个 Symbol.iterator 方法，调用后返回 Iterator。
//
//   Iterator:
//     对象有 next() 方法，每次返回 { value, done }。
//
// 数组、字符串、Map、Set 都实现了 Iterable。
// 普通对象默认不可 for-of，因为它没有 Symbol.iterator。
//
// C++ 对照：
//   for-of 接近 C++ range-for，但 JS/TS 用运行时协议表达“可迭代”。
//   只要对象实现 Symbol.iterator，它就能被 for-of 消费，不需要继承某个基类。
//
// C++ 思维提示：
//   不要把迭代器想成线程或异步任务。同步 Iterator 的 next() 是普通函数调用。

async function demoIterableProtocol(): Promise<void> {
    section("22.1 for-of 背后的协议：Iterable 与 Iterator");
    note("C++ 对照", "for-of 类似 range-for，但 TS/JS 依赖 Symbol.iterator 运行时协议。");

    const values = ["ts", "node", "web"] as const;
    const iterator = values[Symbol.iterator]();

    showJson("数组迭代器 next() 跟踪", traceNextCalls(iterator, 4));

    showJson("常见内建 Iterable", [
        {
            value: "abc",
            forOfResult: [..."abc"]
        },
        {
            value: "new Set([1, 2])",
            forOfResult: [...new Set([1, 2])]
        },
        {
            value: "new Map([[\"a\", 1]])",
            forOfResult: [...new Map([["a", 1]])]
        }
    ]);

    note("输出解释", "第四次 next() 的 done=true，表示 for-of 会停止继续读取。");
    note("常见坑", "普通对象要用 Object.entries/Object.keys/Object.values；不能默认把它当 Iterable。");
}

// =============================================================================
// 22.2 自定义迭代器：让对象按业务顺序输出
// =============================================================================
//
// 自定义 Iterable 适合表达“这个对象的默认遍历顺序”。
// 例如任务队列可以按优先级输出，而不是暴露内部数组。
//
// C++ 对照：
//   这像给容器提供 begin()/end()，让调用方不依赖内部存储结构。
//   差别是 JS 的协议是 Symbol.iterator 方法，而不是模板约束或成员函数命名约定。
//
// C++ 思维提示：
//   自定义迭代器应该保持轻量、可重复和无隐藏 I/O；同步 for-of 不适合做异步分页。

class PriorityQueue implements Iterable<WorkItem> {
    private readonly items: readonly WorkItem[];

    constructor(items: readonly WorkItem[]) {
        this.items = [...items].sort((left, right) => {
            const weight = { high: 3, normal: 2, low: 1 } as const;
            return weight[right.priority] - weight[left.priority];
        });
    }

    [Symbol.iterator](): Iterator<WorkItem> {
        let index = 0;
        const snapshot = this.items;
        return {
            next(): IteratorResult<WorkItem> {
                const item = snapshot[index];
                if (item === undefined) {
                    return { value: undefined, done: true };
                }
                index += 1;
                return { value: item, done: false };
            }
        };
    }
}

async function demoCustomIterator(): Promise<void> {
    section("22.2 自定义迭代器：让对象按业务顺序输出");
    note("C++ 对照", "自定义 Iterable 像给容器定义 begin/end，调用方不需要知道内部数组。");

    const queue = new PriorityQueue([
        { id: "lint", priority: "normal" },
        { id: "deploy", priority: "high" },
        { id: "format", priority: "low" }
    ]);

    showJson("按优先级遍历任务", [...queue]);
    note("输出解释", "构造函数内部保存排序后的快照；for-of 只消费 Symbol.iterator 返回的 next()。");
    note("类型边界", "Iterable<WorkItem> 只保证能遍历出 WorkItem，不暴露队列是否用数组、堆或数据库实现。");
}

// =============================================================================
// 22.3 function*、yield 与 next()
// =============================================================================
//
// generator function 用 function* 声明。
// 调用 generator 函数不会立即执行函数体，而是返回 Generator 对象。
// 每次 next() 会让函数运行到下一个 yield。
//
// C++ 对照：
//   可以把 generator 理解成保存执行位置的状态机。它和 C++20 coroutine 有相似直觉，
//   但 JS generator 是语言级可迭代对象，不等同于 std::generator 的具体实现。
//
// C++ 思维提示：
//   yield 不是创建线程。next() 由调用方主动拉取，生成器才继续执行。

function* countFrom(start: number, count: number): Generator<number, string, void> {
    for (let offset = 0; offset < count; offset++) {
        yield start + offset;
    }
    return `generated ${count} numbers`;
}

async function demoGeneratorNext(): Promise<void> {
    section("22.3 function*、yield 与 next()");
    note("C++ 对照", "generator 像保存执行位置的状态机；调用方 next() 时才继续运行。");

    const generator = countFrom(10, 3);
    const first = generator.next();
    const second = generator.next();
    const third = generator.next();
    const final = generator.next();

    showJson("手动 next() 结果", [
        first,
        second,
        third,
        final
    ]);

    note("输出解释", "前三次得到 yield 的数字；第四次 done=true，value 是 return 的字符串。");
    note("常见坑", "for-of 会消费 yield 的值，但不会把 generator 的 return 值当作遍历元素输出。");
}

// =============================================================================
// 22.4 yield*：把一个可迭代对象委托给外层生成器
// =============================================================================
//
// yield* 会把另一个 Iterable 或 Generator 的值逐个转交给外层调用者。
// 它适合把多个数据来源拼成一个线性序列。
//
// C++ 对照：
//   yield* 类似把子 range 展平到外层 range，而不是返回嵌套容器。
//
// C++ 思维提示：
//   yield* 仍是同步拉取；如果子来源是异步数据流，要使用 async generator 和 for await...of。

function* moduleFiles(): Generator<string> {
    yield "index.ts";
    yield "parser.ts";
}

function* testFiles(): Generator<string> {
    yield "parser.test.ts";
    yield "cli.test.ts";
}

function* projectFiles(): Generator<string> {
    yield "package.json";
    yield* moduleFiles();
    yield* testFiles();
}

async function demoYieldStar(): Promise<void> {
    section("22.4 yield*：把一个可迭代对象委托给外层生成器");
    note("C++ 对照", "yield* 像把子 range 展平到外层 range。");

    showJson("项目文件序列", [...projectFiles()]);
    note("输出解释", "调用方只看到一个线性序列，不需要知道值来自 moduleFiles 还是 testFiles。");
    note("常见坑", "yield [1, 2] 会产出整个数组；yield* [1, 2] 才会产出 1 和 2。");
}

// =============================================================================
// 22.5 生成器的懒加载：只计算真正被消费的值
// =============================================================================
//
// 生成器适合表达“可能很长甚至无限”的序列。
// 调用方可以用 take、break、return 等方式提前停止。
//
// C++ 对照：
//   这和 range/view 的惰性求值很接近：流水线只处理被请求的元素。
//
// C++ 思维提示：
//   惰性不是自动并行。它减少不必要计算，但 CPU 密集计算仍在当前线程执行。

function* fibonacci(): Generator<number> {
    let previous = 0;
    let current = 1;
    while (true) {
        yield previous;
        [previous, current] = [current, previous + current];
    }
}

function* highPriorityOnly(items: Iterable<WorkItem>): Generator<ProcessedItem> {
    for (const item of items) {
        yield {
            id: item.id,
            accepted: item.priority === "high"
        };
    }
}

async function demoLazyLoading(): Promise<void> {
    section("22.5 生成器的懒加载：只计算真正被消费的值");
    note("C++ 对照", "生成器的惰性求值接近 ranges/views；消费多少就计算多少。");

    const firstSeven = take(fibonacci(), 7);
    const jobs = [
        { id: "build", priority: "high" },
        { id: "lint", priority: "normal" },
        { id: "release", priority: "high" }
    ] as const;

    showJson("惰性序列示例", {
        firstSevenFibonacci: firstSeven,
        processedJobs: [...highPriorityOnly(jobs)]
    });

    note("输出解释", "fibonacci 是无限序列，但 take 在拿到 7 个值后 break，因此章节不会挂起。");
    note("副作用边界", "无限生成器必须由调用方限制消费数量；本章用 take 明确退出路径。");
}

// =============================================================================
// 22.6 AsyncIterable 与 async function*
// =============================================================================
//
// 异步生成器使用 async function* 声明。
// 它返回 AsyncGenerator，每次 next() 的结果包在 Promise 中。
// for await...of 会等待每个异步 next() 完成，再进入循环体。
//
// C++ 对照：
//   async generator 和 C++20 coroutine 有相似的“可暂停/恢复”直觉。
//   但 JS 中的 async generator 通过 Promise 和事件循环调度，不是系统线程。
//
// C++ 思维提示：
//   await 只表示当前 async 函数把控制权还给事件循环；它不会创建一个新的 OS 线程。

async function* readPages(): AsyncGenerator<Page<WorkItem>> {
    const pages: readonly Page<WorkItem>[] = [
        {
            page: 1,
            items: [
                { id: "parse", priority: "normal" },
                { id: "typecheck", priority: "high" }
            ]
        },
        {
            page: 2,
            items: [
                { id: "emit", priority: "normal" }
            ]
        }
    ];

    for (const page of pages) {
        yield await Promise.resolve(page);
    }
}

async function demoAsyncGenerator(): Promise<void> {
    section("22.6 AsyncIterable 与 async function*");
    note("C++ 对照", "async generator 像 Promise 驱动的协程式数据流，不是新线程。");

    const pages = await collectAsync(readPages());
    showJson("异步分页结果", pages);

    note("输出解释", "readPages 每次 yield 一个 Page；for await...of 会等待 Promise 解析后继续。");
    note("副作用边界", "本章用 Promise.resolve 模拟异步边界，不依赖网络、定时器或文件系统。");
}

// =============================================================================
// 22.7 for await...of：异步数据流的线性消费
// =============================================================================
//
// for await...of 适合消费 AsyncIterable，例如：
//   - 分页 API
//   - Node stream 的异步迭代形式
//   - 数据库游标
//   - 消息队列批次
//
// 它的优势是把异步数据流写成线性循环，同时保留 break 提前退出能力。
//
// C++ 对照：
//   这接近逐步 co_await 数据源；代码看起来线性，但每次 await 都可能把控制权交还事件循环。
//
// C++ 思维提示：
//   for await...of 默认是串行消费。若要并行处理每个元素，应明确收集 Promise 并管理并发数。

async function* flattenPages(pages: AsyncIterable<Page<WorkItem>>): AsyncGenerator<WorkItem> {
    for await (const page of pages) {
        yield* page.items;
    }
}

async function demoForAwaitOf(): Promise<void> {
    section("22.7 for await...of：异步数据流的线性消费");
    note("C++ 对照", "for await...of 像逐步 co_await 数据源；代码线性，执行仍经过事件循环。");

    const accepted: string[] = [];
    for await (const item of flattenPages(readPages())) {
        if (item.priority === "high") {
            accepted.push(item.id);
        }
    }

    showJson("异步流中过滤高优先级任务", accepted);
    note("输出解释", "flattenPages 先展开分页，再由 for await...of 逐项过滤。");
    note("常见坑", "for await...of 不等于并行 map；如果每项处理很慢，要单独设计并发控制。");
}

// =============================================================================
// 22.8 工程场景：可提前停止的分页读取
// =============================================================================
//
// 真实服务经常需要“从分页数据里找前 N 个符合条件的项”。
// 生成器能把分页、过滤、提前停止拆成清晰的层次：
//   - 数据源：readPages()
//   - 展平：flattenPages()
//   - 业务过滤：priority === "high"
//   - 提前停止：收够 limit 后 break
//
// C++ 对照：
//   这像 range pipeline 加上短路求值。不同之处在于异步分页每一步都可能 await。
//
// C++ 思维提示：
//   提前 break 很重要。否则异步数据源可能继续拉取后续页，浪费 I/O 或延长请求时间。

async function firstHighPriority(limit: number): Promise<WorkItem[]> {
    const result: WorkItem[] = [];
    for await (const item of flattenPages(readPages())) {
        if (item.priority !== "high") {
            continue;
        }
        result.push(item);
        if (result.length >= limit) {
            break;
        }
    }
    return result;
}

async function demoEngineeringPagination(): Promise<void> {
    section("22.8 工程场景：可提前停止的分页读取");
    note("C++ 对照", "异步分页 pipeline 像 range 短路求值，但每页读取都可能 await。");

    const highPriority = await firstHighPriority(1);
    showJson("只读取需要的高优先级任务", highPriority);

    note("输出解释", "limit=1 时，拿到第一个 high 任务后 break；后续处理有明确退出路径。");
    note("类型边界", "AsyncIterable<WorkItem> 只描述异步迭代协议，不保证数据来自网络、文件还是内存。");
}

// =============================================================================
// 22.9 本章复盘
// =============================================================================
//
// C++ 对照：
//   同步生成器接近惰性 range，异步生成器接近 Promise 驱动的 coroutine 数据流。
//   但 JS/TS 的核心仍是协议和事件循环，不是 OS 线程模型。
//
// C++ 思维提示：
//   把“懒加载”“异步等待”“并行执行”分开理解。生成器负责懒加载，async/await 负责等待，
//   并行需要额外的 Promise 组合或 Worker Threads。

async function demoChapterReview(): Promise<void> {
    section("22.9 本章复盘");
    note("C++ 对照", "同步生成器像惰性 range；异步生成器像 Promise 驱动的数据流。");

    showJson("关键结论", [
        "for-of 依赖 Symbol.iterator 和 next() 协议",
        "自定义 Iterable 可以隐藏内部存储并暴露业务遍历顺序",
        "function* 调用后不会立即执行，next() 才推进到下一个 yield",
        "yield* 用来委托另一个 Iterable，把序列展平",
        "生成器适合懒加载和提前停止，但不是自动并行",
        "async function* 返回 AsyncGenerator，for await...of 会等待每个异步 next()",
        "异步生成器要有明确退出路径，避免无限拉取或挂起"
    ]);

    note("可重复运行", "本章只用内存数据和 Promise.resolve；没有网络、文件、定时器或随机输出。");
    note("下一章衔接", "ch23 会深入 Promise 状态机、组合方法与微任务队列。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 22 demos in a stable async order. */
export async function runChapter(): Promise<void> {
    await demoIterableProtocol();
    await demoCustomIterator();
    await demoGeneratorNext();
    await demoYieldStar();
    await demoLazyLoading();
    await demoAsyncGenerator();
    await demoForAwaitOf();
    await demoEngineeringPagination();
    await demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
