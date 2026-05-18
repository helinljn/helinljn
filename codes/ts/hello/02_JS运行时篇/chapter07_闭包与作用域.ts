// =============================================================================
// 第 7 章：闭包与作用域
// =============================================================================
//
// 【学习目标】
//   1. 理解词法作用域、块级作用域和函数级作用域的差异
//   2. 掌握闭包保存外层环境的运行时语义
//   3. 能实现计数器、once、模块模式等常见闭包场景
//   4. 识别闭包内存保留和循环捕获陷阱
//
// 【运行方式】
//   npm run build
//   node dist/02_JS运行时篇/chapter07_闭包与作用域.js
//   或 npm run chapter -- 7（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type Counter = {
    readonly next: () => number;
    readonly current: () => number;
    readonly reset: () => void;
};

type UserStore = {
    readonly add: (id: string, name: string) => void;
    readonly find: (id: string) => string | undefined;
    readonly list: () => readonly string[];
};

type CleanupTask = {
    readonly label: string;
    readonly run: () => string;
};

// =============================================================================
// 7.1 词法作用域：从声明位置向外找
// =============================================================================
//
// C++ 对照：
//   词法作用域的“从内到外查找”直觉和 C++ 块作用域接近。
//   差异在于 JS 函数可以作为值返回，并继续访问声明处的外层变量。
//
// 线性阅读提示：
//   本章先讲同步闭包。
//   异步回调中的闭包会在异步章节继续遇到。

function demoLexicalScope(): void {
    section("7.1 词法作用域：变量从声明位置向外查找");
    note("C++ 对照", "名字查找从当前作用域向外层作用域进行；函数的声明位置很重要。");

    const language = "TypeScript";

    function describeChapter(chapter: number): string {
        const topic = "closure";
        return `${language} chapter ${chapter}: ${topic}`;
    }

    showJson("词法作用域输出", {
        language,
        result: describeChapter(7)
    });
    note("输出解释", "describeChapter 能访问外层 language，因为它在这个词法环境中声明。");
}

// =============================================================================
// 7.2 let/const 块级作用域 vs var 函数级作用域
// =============================================================================
//
// C++ 对照：
//   let/const 更接近 C++ 花括号块作用域。
//   var 是函数级作用域，容易让 C++ 开发者误以为它只在 if/for 块内有效。
//
// 常见坑：
//   var 声明的变量会被提升到函数作用域。
//   新代码中应默认使用 const，需要重新赋值时用 let。

function demoBlockScopeVsVar(): void {
    section("7.2 let/const 块级作用域 vs var 函数级作用域");
    note("C++ 对照", "let/const 的块级边界更接近 C++；var 是历史遗留行为。");

    const records: string[] = [];
    if (true) {
        const blockConst = "const-in-block";
        let blockLet = "let-in-block";
        blockLet = `${blockLet}:changed`;
        records.push(blockConst, blockLet);
    }

    function legacyVarDemo(): string {
        if (true) {
            var visibleInFunction = "var escapes block";
        }
        return visibleInFunction;
    }

    showJson("作用域对比", {
        blockRecords: records,
        varResult: legacyVarDemo(),
        recommendation: "new code should avoid var"
    });
    note("输出解释", "blockConst/blockLet 不能在 if 外访问；var 变量在整个函数内可见。");
}

// =============================================================================
// 7.3 闭包定义：函数保存外层环境
// =============================================================================
//
// C++ 对照：
//   C++ lambda 捕获需要显式写 `[&]`、`[=]` 或具体变量。
//   JS 闭包自动捕获词法环境中被使用的变量。
//
// 常见坑：
//   闭包保存的是变量环境，不是简单复制某一刻的值。
//   如果闭包共享同一个可变变量，它们看到的是同一份状态。

/** Create a counter whose state is kept private by closure. */
export function createCounter(start = 0): Counter {
    let value = start;
    return {
        next: () => {
            value += 1;
            return value;
        },
        current: () => value,
        reset: () => {
            value = start;
        }
    };
}

function demoClosureDefinition(): void {
    section("7.3 闭包定义：函数保存外层环境");
    note("C++ 对照", "JS 闭包自动保留被内部函数使用的外层变量，不需要显式捕获列表。");

    const counter = createCounter(10);
    const output = [counter.next(), counter.next(), counter.current()];
    counter.reset();

    showJson("闭包计数器", {
        output,
        afterReset: counter.current()
    });
    note("输出解释", "createCounter 返回后，value 仍被 next/current/reset 这组闭包保存。");
}

// =============================================================================
// 7.4 once：只允许函数执行一次
// =============================================================================
//
// C++ 对照：
//   once 像用 lambda 捕获一个状态位，再在后续调用中检查它。
//   JS 中这个状态位自然保存在闭包环境里。
//
// 真实场景：
//   初始化、资源注册、事件订阅等逻辑经常需要“只执行一次”。

/** Wrap a function so the underlying implementation runs at most once. */
export function once<T>(factory: () => T): () => T {
    let called = false;
    let cached: T | undefined;
    return () => {
        if (!called) {
            cached = factory();
            called = true;
        }
        return cached as T;
    };
}

function demoOnce(): void {
    section("7.4 once：只执行一次的闭包状态");
    note("C++ 对照", "闭包保存 called 和 cached，类似 lambda 捕获私有状态。");

    let expensiveCallCount = 0;
    const loadConfig = once(() => {
        expensiveCallCount += 1;
        return { mode: "tutorial", strict: true };
    });

    const first = loadConfig();
    const second = loadConfig();

    showJson("once 示例", {
        first,
        second,
        sameObject: first === second,
        expensiveCallCount
    });
    note("输出解释", "factory 只执行一次，第二次调用直接返回闭包中缓存的结果。");
}

// =============================================================================
// 7.5 模块模式：用闭包隐藏可变数据
// =============================================================================
//
// C++ 对照：
//   这有点像把私有成员封装在类中，只暴露方法。
//   不同点是这里没有 class，私有状态来自函数作用域和闭包。
//
// 类型边界：
//   TS 能描述 add/find/list 的函数签名。
//   users 这个 Map 在运行时只被闭包引用，外部无法直接访问。

/** Create an in-memory user store backed by closure-private state. */
export function createUserStore(): UserStore {
    const users = new Map<string, string>();
    return {
        add: (id, name) => {
            users.set(id, name);
        },
        find: (id) => users.get(id),
        list: () => Array.from(users, ([id, name]) => `${id}:${name}`)
    };
}

function demoModulePattern(): void {
    section("7.5 模块模式：闭包隐藏可变数据");
    note("C++ 对照", "闭包可以承担一部分私有成员的封装职责，但它仍是 JS 函数环境。");

    const store = createUserStore();
    store.add("u1", "Ada");
    store.add("u2", "Grace");

    showJson("闭包模块模式", {
        list: store.list(),
        findU1: store.find("u1"),
        findMissing: store.find("missing")
    });
    note("输出解释", "外部只能通过 add/find/list 访问 users，不能直接拿到内部 Map。");
}

// =============================================================================
// 7.6 IIFE：立即调用函数表达式
// =============================================================================
//
// C++ 对照：
//   IIFE 没有直接 C++ 对应。
//   它是 JS 早期创建局部作用域、避免全局变量污染的常见写法。
//
// 现代边界：
//   ES module 自带模块作用域，let/const 也有块级作用域。
//   IIFE 在现代代码中少见，但阅读旧代码仍需要认识它。

function demoIife(): void {
    section("7.6 IIFE：立即调用函数表达式");
    note("C++ 对照", "IIFE 是 JS 历史中的作用域技巧；现代 ESM 中使用频率降低。");

    const result = (() => {
        const hidden = "only visible inside IIFE";
        return hidden.toUpperCase();
    })();

    showJson("IIFE 输出", {
        result,
        modernAlternative: "use block scope or ES module scope"
    });
    note("输出解释", "hidden 只存在于立即执行的函数作用域中，外部只能拿到返回值。");
}

// =============================================================================
// 7.7 循环中的闭包陷阱
// =============================================================================
//
// C++ 对照：
//   C++ lambda 捕获值或引用是显式选择。
//   JS 中 var 的函数级作用域会让多个闭包共享同一个循环变量。
//
// 常见坑：
//   用 var 创建一组回调时，所有回调可能读到循环结束后的同一个 i。
//   用 let 时，每次迭代会创建新的块级绑定。

function buildCallbacksWithVar(): Array<() => number> {
    const callbacks: Array<() => number> = [];
    for (var index = 0; index < 3; index += 1) {
        callbacks.push(() => index);
    }
    return callbacks;
}

function buildCallbacksWithLet(): Array<() => number> {
    const callbacks: Array<() => number> = [];
    for (let index = 0; index < 3; index += 1) {
        callbacks.push(() => index);
    }
    return callbacks;
}

function demoLoopClosureTrap(): void {
    section("7.7 循环中的闭包陷阱");
    note("C++ 对照", "var 更像所有闭包捕获同一个可变变量；let 会为每轮迭代创建新绑定。");

    const varResults = buildCallbacksWithVar().map((callback) => callback());
    const letResults = buildCallbacksWithLet().map((callback) => callback());

    showJson("循环闭包结果", { varResults, letResults });
    note("输出解释", "varResults 全是 3，因为循环结束后共享的 index 已经是 3。letResults 保留每轮值。");
}

// =============================================================================
// 7.8 闭包的内存影响
// =============================================================================
//
// C++ 对照：
//   闭包保存外层变量，类似可调用对象持有状态。
//   如果状态很大或包含资源句柄，闭包生命周期就会影响内存释放。
//
// 工程建议：
//   不要在长期存在的闭包中无意保留大对象、请求体、缓存或敏感数据。
//   只捕获真正需要的最小数据。

function createCleanupTasks(labels: readonly string[]): readonly CleanupTask[] {
    return labels.map((label) => ({
        label,
        run: () => `cleanup:${label}`
    }));
}

function demoClosureMemoryImpact(): void {
    section("7.8 闭包的内存影响");
    note("C++ 对照", "闭包持有状态会延长这些状态的生命周期，类似对象成员被可调用对象保存。");

    const largeRequest = {
        id: "req-1",
        bodyPreview: "x".repeat(16),
        metadata: ["auth", "trace", "locale"]
    };

    const badCapture = () => largeRequest.id;
    const requestId = largeRequest.id;
    const betterCapture = () => requestId;
    const tasks = createCleanupTasks(["temp-file", "server", "watcher"]);

    showJson("闭包内存边界示例", {
        badCaptureResult: badCapture(),
        betterCaptureResult: betterCapture(),
        cleanupResults: tasks.map((task) => task.run()),
        guidance: "long-lived closures should capture the minimum data needed"
    });
    note("输出解释", "badCapture 会保留整个 largeRequest；betterCapture 只保留 requestId 字符串。");
}

// =============================================================================
// 7.9 工程场景：可测试的 ID 生成器
// =============================================================================
//
// C++ 对照：
//   这类似一个轻量状态对象，但用函数工厂和闭包实现。
//   好处是测试可以创建独立实例，不共享全局计数器。
//
// 真实场景：
//   CLI、测试数据、内存仓库经常需要稳定 ID。
//   闭包能避免全局变量带来的跨测试污染。

/** Create a deterministic ID generator with closure-private state. */
export function createIdGenerator(prefix: string, start = 0): () => string {
    let nextValue = start;
    return () => {
        nextValue += 1;
        return `${prefix}-${String(nextValue).padStart(3, "0")}`;
    };
}

function demoIdGeneratorScenario(): void {
    section("7.9 工程场景：可测试的 ID 生成器");
    note("C++ 对照", "闭包可以替代一个只有私有计数器和 operator() 的小对象。");

    const userId = createIdGenerator("user", 0);
    const orderId = createIdGenerator("order", 100);

    showJson("ID 生成器", {
        users: [userId(), userId(), userId()],
        orders: [orderId(), orderId()]
    });
    note("输出解释", "两个生成器各自保存独立计数器，不依赖全局状态。");
}

// =============================================================================
// 7.10 本章复盘
// =============================================================================
//
// C++ 对照：
//   JS 闭包是理解函数式写法、模块封装和异步回调的基础。
//   与 C++ lambda 最大差异是捕获隐式发生，且变量环境可能被多个闭包共享。

function demoChapterReview(): void {
    section("7.10 本章复盘");
    note("C++ 对照", "JS 闭包自动捕获词法环境；使用时要主动管理可变状态和生命周期。");

    const review = [
        "词法作用域按函数声明位置向外查找变量",
        "let/const 是块级作用域，var 是函数级作用域",
        "闭包让函数返回后仍能访问外层变量",
        "once、计数器、模块模式都依赖闭包保存状态",
        "IIFE 是旧 JS 中常见的作用域技巧，现代 ESM 中少用",
        "var 循环闭包会共享同一个循环变量，let 可避免该坑",
        "长期存在的闭包会延长被捕获数据的生命周期",
        "工程代码应只捕获真正需要的最小数据"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 7. */
export function runChapter(): void {
    demoLexicalScope();
    demoBlockScopeVsVar();
    demoClosureDefinition();
    demoOnce();
    demoModulePattern();
    demoIife();
    demoLoopClosureTrap();
    demoClosureMemoryImpact();
    demoIdGeneratorScenario();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
