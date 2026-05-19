// =============================================================================
// 第 15 章：函数式编程模式
// =============================================================================
//
// 【学习目标】
//   1. 理解函数一等公民：赋值、传参、返回
//   2. 手动实现 map/filter/reduce，理解数组高阶函数的边界
//   3. 掌握 compose/pipe、curry、memoize、once、debounce、throttle
//
// 【运行方式】
//   npm run build
//   node dist/04_函数篇/chapter15_函数式编程模式.js
//   或 npm run chapter -- 15（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type Unary<T, U> = (value: T) => U;
type Predicate<T> = (value: T) => boolean;
type Reducer<T, U> = (accumulator: U, value: T) => U;

type TimelineEvent<T> = {
    atMs: number;
    value: T;
};

type ValidationResult<T> =
    | { ok: true; value: T }
    | { ok: false; error: string };

function manualMap<T, U>(values: readonly T[], mapper: Unary<T, U>): U[] {
    const result: U[] = [];
    for (const value of values) {
        result.push(mapper(value));
    }
    return result;
}

function manualFilter<T>(values: readonly T[], predicate: Predicate<T>): T[] {
    const result: T[] = [];
    for (const value of values) {
        if (predicate(value)) {
            result.push(value);
        }
    }
    return result;
}

function manualReduce<T, U>(values: readonly T[], reducer: Reducer<T, U>, initial: U): U {
    let accumulator = initial;
    for (const value of values) {
        accumulator = reducer(accumulator, value);
    }
    return accumulator;
}

function pipe<A, B, C>(first: Unary<A, B>, second: Unary<B, C>): Unary<A, C> {
    return (value) => second(first(value));
}

function compose<A, B, C>(second: Unary<B, C>, first: Unary<A, B>): Unary<A, C> {
    return (value) => second(first(value));
}

function curry2<A, B, C>(fn: (left: A, right: B) => C): (left: A) => (right: B) => C {
    return (left) => (right) => fn(left, right);
}

function memoize<A extends string | number, R>(fn: Unary<A, R>): Unary<A, R> & { cacheSize(): number } {
    const cache = new Map<A, R>();
    const memoized = ((value: A): R => {
        if (cache.has(value)) {
            return cache.get(value) as R;
        }
        const result = fn(value);
        cache.set(value, result);
        return result;
    }) as Unary<A, R> & { cacheSize(): number };
    memoized.cacheSize = () => cache.size;
    return memoized;
}

function once<Args extends readonly unknown[], R>(fn: (...args: Args) => R): (...args: Args) => R {
    let called = false;
    let result: R | undefined;
    return (...args) => {
        if (!called) {
            result = fn(...args);
            called = true;
        }
        return result as R;
    };
}

function simulateDebounce<T>(events: readonly TimelineEvent<T>[], delayMs: number): TimelineEvent<T>[] {
    if (events.length === 0) {
        return [];
    }

    const sorted = [...events].sort((left, right) => left.atMs - right.atMs);
    const emitted: TimelineEvent<T>[] = [];
    let pending = sorted[0]!;

    for (const event of sorted.slice(1)) {
        if (event.atMs - pending.atMs >= delayMs) {
            emitted.push({ atMs: pending.atMs + delayMs, value: pending.value });
        }
        pending = event;
    }

    emitted.push({ atMs: pending.atMs + delayMs, value: pending.value });
    return emitted;
}

function simulateThrottle<T>(events: readonly TimelineEvent<T>[], intervalMs: number): TimelineEvent<T>[] {
    const emitted: TimelineEvent<T>[] = [];
    let nextAllowedAt = Number.NEGATIVE_INFINITY;

    for (const event of [...events].sort((left, right) => left.atMs - right.atMs)) {
        if (event.atMs >= nextAllowedAt) {
            emitted.push(event);
            nextAllowedAt = event.atMs + intervalMs;
        }
    }

    return emitted;
}

function parseMetricLine(line: string): ValidationResult<{ name: string; value: number }> {
    const [name, valueText] = line.split("=");
    if (name === undefined || name.trim() === "") {
        return { ok: false, error: "missing metric name" };
    }

    const value = Number(valueText);
    if (!Number.isFinite(value)) {
        return { ok: false, error: `invalid metric value for ${name}` };
    }

    return { ok: true, value: { name: name.trim(), value } };
}

function partitionResults<T>(results: readonly ValidationResult<T>[]): { values: T[]; errors: string[] } {
    return results.reduce(
        (accumulator, result) => {
            if (result.ok) {
                accumulator.values.push(result.value);
            } else {
                accumulator.errors.push(result.error);
            }
            return accumulator;
        },
        { values: [] as T[], errors: [] as string[] }
    );
}

// =============================================================================
// 15.1 函数一等公民
// =============================================================================
//
// C++ 对照：
//   TS 函数值接近 std::function、lambda 和函数指针的组合体验。
//   函数可以赋值、放进数组、作为参数传入，也可以从函数返回。

function demoFirstClassFunctions(): void {
    section("15.1 函数一等公民");
    note("C++ 对照", "函数可以像普通对象一样流动，这让回调和策略模式写起来很轻。");

    const strategies: Record<string, Unary<number, number>> = {
        double: (value) => value * 2,
        square: (value) => value * value,
        negate: (value) => -value
    };

    const outputs = Object.entries(strategies).map(([name, transform]) => ({
        name,
        result: transform(4)
    }));

    showJson("函数值策略表", outputs);
    note("输出解释", "每个策略都是一个函数值；对象只负责把名字映射到函数。");
}

// =============================================================================
// 15.2 手动实现 map/filter/reduce
// =============================================================================
//
// C++ 对照：
//   map/filter/reduce 像 ranges/algorithm 的组合。
//   重点是把“遍历”与“业务变换”分离。
//
// 常见坑：
//   reduce 如果没有初始值，空数组会在运行时报错；工程代码里建议显式提供 initial。

function demoManualArrayOperators(): void {
    section("15.2 手动实现 map/filter/reduce");
    note("C++ 对照", "高阶函数把遍历容器的样板代码收起来，只暴露变换函数。");

    const values = [1, 2, 3, 4, 5];
    const squares = manualMap(values, (value) => value * value);
    const evenValues = manualFilter(values, (value) => value % 2 === 0);
    const total = manualReduce(values, (sum, value) => sum + value, 0);

    showJson("手写高阶数组操作", {
        squares,
        evenValues,
        total
    });
    note("输出解释", "手动实现能看清 map/filter/reduce 本质都是“遍历 + 回调”。");
}

// =============================================================================
// 15.3 compose 与 pipe
// =============================================================================
//
// C++ 对照：
//   compose/pipe 把多个小函数拼成一个更大的转换流程。
//   它适合纯数据转换，不适合隐藏复杂副作用。

function demoComposeAndPipe(): void {
    section("15.3 compose 与 pipe");
    note("C++ 对照", "pipe 更符合从左到右阅读，compose 更接近数学函数复合。");

    const trim = (value: string): string => value.trim();
    const toLower = (value: string): string => value.toLowerCase();
    const addPrefix = (value: string): string => `user:${value}`;

    const normalizeWithPipe = pipe(pipe(trim, toLower), addPrefix);
    const normalizeWithCompose = compose(addPrefix, compose(toLower, trim));

    showJson("compose/pipe", {
        pipeResult: normalizeWithPipe("  ADA  "),
        composeResult: normalizeWithCompose("  GRACE  ")
    });
    note("输出解释", "两个版本都只是在组织调用顺序；运行时仍是普通函数嵌套。");
}

// =============================================================================
// 15.4 curry
// =============================================================================
//
// C++ 对照：
//   curry 把多参数函数拆成一串单参数函数。
//   它常用于提前固定部分参数，类似 bind/front adaptor。

function demoCurry(): void {
    section("15.4 curry");
    note("C++ 对照", "curry 可以先固定上下文，再把剩下的函数交给 map/filter 之类的高阶函数。");

    const multiply = (left: number, right: number): number => left * right;
    const multiplyBy10 = curry2(multiply)(10);
    const scaled = manualMap([1, 2, 3], multiplyBy10);

    showJson("curry 示例", {
        multiplyBy10: scaled,
        direct: curry2((prefix: string, value: string) => `${prefix}:${value}`)("role")("admin")
    });
    note("输出解释", "curry2(multiply)(10) 先固定左侧参数，得到一个只需要右侧参数的新函数。");
}

// =============================================================================
// 15.5 memoize 与 once
// =============================================================================
//
// C++ 对照：
//   memoize 像给纯函数加缓存；once 像确保初始化逻辑只执行一次。
//   两者都依赖闭包保存私有状态。
//
// 常见坑：
//   memoize 只适合稳定、可缓存的纯函数；输入包含对象时要先设计可靠 key。

function demoMemoizeAndOnce(): void {
    section("15.5 memoize 与 once");
    note("C++ 对照", "闭包保存 cache/called 状态，调用方看不到内部实现。");

    let expensiveCalls = 0;
    const expensiveSquare = memoize((value: number) => {
        expensiveCalls += 1;
        return value * value;
    });
    const loadConfigOnce = once(() => ({ target: "ES2023", module: "NodeNext" }));

    const values = [expensiveSquare(4), expensiveSquare(4), expensiveSquare(5)];
    const firstConfig = loadConfigOnce();
    const secondConfig = loadConfigOnce();

    showJson("memoize/once", {
        values,
        expensiveCalls,
        cacheSize: expensiveSquare.cacheSize(),
        sameConfigObject: firstConfig === secondConfig,
        firstConfig
    });
    note("输出解释", "4 的平方只计算一次；once 返回第一次初始化的结果。");
}

// =============================================================================
// 15.6 debounce 与 throttle
// =============================================================================
//
// C++ 对照：
//   debounce/throttle 常用于 UI、日志、网络请求限频。
//   本章用纯时间线模拟，不启动真实定时器，因此不会留下挂起资源。

function demoDebounceAndThrottle(): void {
    section("15.6 debounce 与 throttle");
    note("C++ 对照", "真实实现通常依赖定时器；这里用纯函数模拟时间线，保证章节可重复运行。");

    const events: TimelineEvent<string>[] = [
        { atMs: 0, value: "a" },
        { atMs: 30, value: "b" },
        { atMs: 80, value: "c" },
        { atMs: 220, value: "d" }
    ];

    const debounced = simulateDebounce(events, 100);
    const throttled = simulateThrottle(events, 100);

    showJson("debounce/throttle 时间线", {
        events,
        debounced,
        throttled
    });
    note("输出解释", "debounce 取安静期后的最后一次输入；throttle 在固定窗口内只放行第一次输入。");
}

// =============================================================================
// 15.7 工程场景：用小函数管线处理指标文本
// =============================================================================
//
// C++ 对照：
//   这类似用 ranges pipeline 把“清洗、解析、过滤、聚合”拆成多个小步骤。
//   每个步骤都保持输入输出明确，测试时可以单独验证。
//
// 真实场景：
//   日志、指标、配置行这类文本输入经常包含脏数据。
//   函数式管线适合把预期内失败保留成值，而不是遇到第一条坏数据就抛异常。
//
// 常见坑：
//   不要为了链式写法把所有逻辑塞进一长串 map/filter/reduce。
//   一旦中间状态需要命名解释，就应拆成局部变量或命名函数。

function demoMetricPipelineScenario(): void {
    section("15.7 工程场景：指标文本处理管线");
    note("C++ 对照", "把小函数串起来，接近 ranges/adaptor 的阅读方式。");

    const rawLines = [" requests = 10 ", "errors=2", "bad-line", "latencyMs=125"];
    const normalized = manualMap(rawLines, (line) => line.trim().replace(/\s+/g, ""));
    const parsed = manualMap(normalized, parseMetricLine);
    const { values, errors } = partitionResults(parsed);
    const highValueMetrics = manualFilter(values, (metric) => metric.value >= 10);
    const total = manualReduce(values, (sum, metric) => sum + metric.value, 0);

    showJson("指标处理管线", {
        normalized,
        values,
        errors,
        highValueMetrics,
        total
    });
    note("输出解释", "坏行被收集到 errors；好数据继续参与 filter/reduce 聚合。");
    note("常见坑", "管线不等于少写变量；给关键中间结果命名能显著降低排查成本。");
}

// =============================================================================
// 15.8 本章复盘
// =============================================================================
//
// C++ 对照：
//   函数式模式不是为了炫技，而是把状态、遍历和转换边界分清楚。
//   对 C++ 背景读者来说，可以把它看成更轻量的 strategy/ranges/adaptor 组合。

function demoChapterReview(): void {
    section("15.8 本章复盘");
    note("C++ 对照", "函数值 + 闭包让 JS/TS 很适合表达策略和小型转换管线。");

    const summary = [
        "函数是一等值，可以赋值、传参和返回",
        "map/filter/reduce 把遍历和业务变换分离",
        "pipe/compose 组织数据转换顺序",
        "curry 用于提前固定部分参数",
        "memoize/once 依靠闭包保存私有状态",
        "debounce/throttle 要管理定时器或时间窗口，本章用纯模拟避免副作用",
        "真实管线要保留错误信息，并给关键中间结果命名"
    ];

    showJson("关键结论", summary);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 15. */
export function runChapter(): void {
    demoFirstClassFunctions();
    demoManualArrayOperators();
    demoComposeAndPipe();
    demoCurry();
    demoMemoizeAndOnce();
    demoDebounceAndThrottle();
    demoMetricPipelineScenario();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
