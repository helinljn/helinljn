// =============================================================================
// 第 14 章：函数详解
// =============================================================================
//
// 【学习目标】
//   1. 区分函数声明、函数表达式和箭头函数
//   2. 掌握参数类型、可选参数、默认值、剩余参数和解构参数
//   3. 理解函数重载、this 参数标注、void 与 undefined 的边界
//
// 【运行方式】
//   npm run build
//   node dist/04_函数篇/chapter14_函数详解.js
//   或 npm run chapter -- 14（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type UserSummary = {
    name: string;
    role?: string;
    active?: boolean;
};

type CommandOptions = {
    command: string;
    args?: readonly string[];
    dryRun?: boolean;
};

type CommandHooks = {
    beforeRun?: (command: string) => void;
    shouldRun?: (command: string) => boolean;
};

type CommandRunResult = {
    command: string;
    skipped: boolean;
    output: string;
};

interface CounterContext {
    readonly name: string;
    count: number;
}

function greetingDeclaration(name: string): string {
    return `hello, ${name}`;
}

const greetingExpression = function (name: string): string {
    return `hi, ${name}`;
};

const greetingArrow = (name: string): string => `welcome, ${name}`;

function formatUser({ name, role = "guest", active = true }: UserSummary): string {
    return `${name}:${role}:${active ? "active" : "inactive"}`;
}

function sumAll(...values: readonly number[]): number {
    return values.reduce((total, value) => total + value, 0);
}

function buildRange(start = 0, end = 3): number[] {
    const values: number[] = [];
    for (let value = start; value <= end; value += 1) {
        values.push(value);
    }
    return values;
}

function normalizeId(id: number): string;
function normalizeId(id: string): string;
function normalizeId(id: number | string): string {
    return typeof id === "number" ? `id-${id.toString(10)}` : id.trim().toLowerCase();
}

function parseInput(input: string): { kind: "text"; value: string };
function parseInput(input: number): { kind: "number"; value: number };
function parseInput(input: string | number): { kind: "text"; value: string } | { kind: "number"; value: number } {
    if (typeof input === "number") {
        return { kind: "number", value: input };
    }
    return { kind: "text", value: input.trim() };
}

function incrementCounter(this: CounterContext, by = 1): string {
    this.count += by;
    return `${this.name}:${this.count}`;
}

function formatCommand({ command, args = [], dryRun = false }: CommandOptions): string {
    const prefix = dryRun ? "[dry-run] " : "";
    const suffix = args.length > 0 ? ` ${args.join(" ")}` : "";
    return `${prefix}${command}${suffix}`;
}

function runCommandQueue(commands: readonly string[], hooks: CommandHooks = {}): CommandRunResult[] {
    return commands.map((command) => {
        hooks.beforeRun?.(command);
        const shouldRun = hooks.shouldRun?.(command) ?? true;
        if (!shouldRun) {
            return {
                command,
                skipped: true,
                output: "skipped"
            };
        }
        return {
            command,
            skipped: false,
            output: `ran:${command}`
        };
    });
}

// =============================================================================
// 14.1 函数声明 vs 表达式 vs 箭头函数
// =============================================================================
//
// C++ 对照：
//   函数声明更像普通 free function。
//   函数表达式和箭头函数是值，可以赋给变量、传参或返回。
//
// 常见坑：
//   箭头函数没有自己的 this；如果 API 依赖动态 this，优先用普通函数。

function demoFunctionForms(): void {
    section("14.1 函数声明 vs 表达式 vs 箭头函数");
    note("C++ 对照", "JS/TS 函数本身是一等值，声明只是创建函数值的一种写法。");

    const formatters = [greetingDeclaration, greetingExpression, greetingArrow];
    const outputs = formatters.map((formatter) => formatter("TypeScript"));

    showJson("三种函数形态", {
        outputs,
        expressionType: typeof greetingExpression,
        arrowType: typeof greetingArrow
    });
    note("输出解释", "三种写法运行时都是 function 值；差异主要在 hoisting、this 和代码组织习惯。");
}

// =============================================================================
// 14.2 参数类型、可选/默认值、剩余参数、解构
// =============================================================================
//
// C++ 对照：
//   默认参数和剩余参数都比 C++ 更贴近运行时数组/对象。
//   解构参数常用于配置对象，但要写清楚字段类型。

function demoParameterShapes(): void {
    section("14.2 参数类型、可选/默认值、剩余参数、解构");
    note("C++ 对照", "默认参数类似 C++ 默认实参；剩余参数则直接收集成数组。");

    const formattedUser = formatUser({ name: "Ada", role: "admin" });
    const inactiveUser = formatUser({ name: "Grace", active: false });
    const total = sumAll(1, 2, 3, 4);
    const defaultRange = buildRange();
    const customRange = buildRange(2, 5);

    showJson("参数形式", {
        formattedUser,
        inactiveUser,
        total,
        defaultRange,
        customRange
    });
    note("输出解释", "解构参数让调用点更清楚；剩余参数适合数量不固定但类型一致的输入。");
}

// =============================================================================
// 14.3 函数重载
// =============================================================================
//
// C++ 对照：
//   C++ 重载会生成多个候选函数。
//   TS 重载只有多个类型签名，运行时仍是一份实现。
//
// 常见坑：
//   实现签名不是对外可见的调用签名；外部只能按上面的重载签名调用。

function demoOverloads(): void {
    section("14.3 函数重载");
    note("C++ 对照", "TS 重载是类型层 API 设计，运行时用一份实现处理联合输入。");

    const normalizedNumber = normalizeId(42);
    const normalizedText = normalizeId("  USER-7 ");
    const parsedText = parseInput(" hello ");
    const parsedNumber = parseInput(99);

    showJson("函数重载", {
        normalizedNumber,
        normalizedText,
        parsedText,
        parsedNumber
    });
    note("输出解释", "调用方看到的是重载签名；实现内部用 typeof 做运行时分支。");
}

// =============================================================================
// 14.4 this 参数标注
// =============================================================================
//
// C++ 对照：
//   this 参数标注不是实际入参，只是告诉 TS 这个函数要求怎样的调用上下文。
//   它适合旧式对象方法、回调适配和需要显式绑定 this 的 API。

function demoThisParameter(): void {
    section("14.4 this 参数标注");
    note("C++ 对照", "this 参数像把隐藏的对象指针写成类型约束，但运行时调用方式仍由 JS 决定。");

    const counter: CounterContext = {
        name: "jobs",
        count: 0
    };
    const first = incrementCounter.call(counter, 1);
    const second = incrementCounter.call(counter, 4);

    showJson("this 参数", {
        first,
        second,
        finalCount: counter.count
    });
    note("常见坑", "把依赖 this 的普通函数改成箭头函数，往往会悄悄改变绑定语义。");
}

// =============================================================================
// 14.5 void vs undefined
// =============================================================================
//
// C++ 对照：
//   void 表示调用方不应关心返回值。
//   undefined 是一个真实运行时值，可以被比较、传递和序列化为缺失字段。

function demoVoidVsUndefined(): void {
    section("14.5 void vs undefined");
    note("C++ 对照", "void 更像“忽略返回值”的契约；undefined 是 JS 里的具体值。");

    const auditLog: string[] = [];
    const writeAudit = (message: string): void => {
        auditLog.push(message);
    };
    const maybeFind = (enabled: boolean): string | undefined => (enabled ? "feature-a" : undefined);

    const callbackResults: unknown[] = [];
    const runCallback = (callback: () => void): void => {
        callbackResults.push(callback());
    };

    writeAudit("created");
    runCallback(() => auditLog.push("callback-created"));

    showJson("void 与 undefined", {
        auditLog,
        callbackResults,
        found: maybeFind(true),
        missing: maybeFind(false) ?? null
    });
    note("输出解释", "() => void 的调用方会忽略返回值；需要表达可能没有值时，用 T | undefined。");
}

// =============================================================================
// 14.6 工程场景：命令格式化器
// =============================================================================
//
// C++ 对照：
//   在 CLI、HTTP handler 和任务调度代码里，函数签名就是最小 API 文档。
//   清楚的参数对象通常比多个位置参数更抗变更。

function demoCommandFormatterScenario(): void {
    section("14.6 工程场景：命令格式化器");
    note("C++ 对照", "配置对象参数类似把一组 option struct 传给函数。");

    const commands = [
        formatCommand({ command: "build" }),
        formatCommand({ command: "test", args: ["--watch"] }),
        formatCommand({ command: "deploy", args: ["staging"], dryRun: true })
    ];

    showJson("命令格式化", commands);
    note("输出解释", "参数对象让 dryRun、args 这类可选字段具名化，后续扩展不会破坏调用顺序。");
}

// =============================================================================
// 14.7 回调 API 设计：副作用回调、谓词回调和返回值边界
// =============================================================================
//
// C++ 对照：
//   回调签名类似把 std::function 放进 options struct。
//   关键不是“能传函数”，而是把回调的职责和返回值语义写清楚。
//
// 类型边界：
//   `(command) => void` 表达调用方不应依赖返回值。
//   `(command) => boolean` 表达返回值会参与控制流，调用方必须返回明确布尔值。
//
// 常见坑：
//   把所有 hook 都写成 `Function` 或 `(...args: any[]) => any` 会丢掉调用协议。
//   这会让后续重构时很难知道哪些回调能影响流程，哪些只是记录日志。

function demoCallbackApiDesign(): void {
    section("14.7 回调 API 设计");
    note("C++ 对照", "把回调放进 options 对象时，签名就是这个小型扩展点的契约。");

    const audit: string[] = [];
    const results = runCommandQueue(["build", "test", "deploy"], {
        beforeRun(command) {
            audit.push(`before:${command}`);
        },
        shouldRun(command) {
            return command !== "deploy";
        }
    });

    showJson("回调 API 设计", {
        audit,
        results
    });
    note("输出解释", "beforeRun 只记录副作用；shouldRun 的 boolean 返回值决定命令是否执行。");
    note("常见坑", "void 回调的返回值会被调用方忽略；如果返回值影响流程，应在类型里明确写成 boolean 或 Result。");
}

// =============================================================================
// 14.8 本章复盘
// =============================================================================
//
// C++ 对照：
//   TS 函数设计的重点是把调用边界讲清楚。
//   位置参数、对象参数、重载、this 和返回类型都服务于这个边界。

function demoChapterReview(): void {
    section("14.8 本章复盘");
    note("C++ 对照", "TS 重载与 this 参数都是类型层 API 设计，不会自动生成多份运行时代码。");

    const summary = [
        "函数声明、表达式和箭头函数运行时都是 function 值",
        "默认参数、剩余参数和解构参数能让调用点更清楚",
        "TS 重载是多签名一实现",
        "this 参数标注只约束调用上下文，不是实际入参",
        "void 表达忽略返回值，undefined 是具体值",
        "配置对象参数适合工程 API 的长期演进",
        "回调 API 要区分副作用 hook 和参与控制流的谓词"
    ];

    showJson("关键结论", summary);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 14. */
export function runChapter(): void {
    demoFunctionForms();
    demoParameterShapes();
    demoOverloads();
    demoThisParameter();
    demoVoidVsUndefined();
    demoCommandFormatterScenario();
    demoCallbackApiDesign();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
