// =============================================================================
// 第 2 章：变量与基本类型
// =============================================================================
//
// 【学习目标】
//   1. 区分 let、const、var 的作用域、初始化和工程使用边界
//   2. 掌握 number、string、boolean、null、undefined、bigint、symbol 的基本语义
//   3. 理解类型标注、类型推断、typeof 和字面量类型入门
//   4. 建立 C++ const 与 TypeScript const 的差异意识
//
// 【运行方式】
//   npm run build
//   node dist/01_基础篇/chapter02_变量与基本类型.js
//   或 npm run chapter -- 2（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type DeclarationExample = {
    readonly keyword: "let" | "const" | "var";
    readonly scope: string;
    readonly canReassign: boolean;
    readonly recommendedUse: string;
};

type RuntimeTypeSample = {
    readonly expression: string;
    readonly runtimeType: string;
    readonly valuePreview: string;
};

type MoneyInCents = {
    readonly currency: "CNY" | "USD";
    readonly cents: number;
};

type FeatureFlagName = "darkMode" | "auditLog" | "betaCheckout";

type ConfigValue = string | number | boolean | undefined;

type ConfigEntry = {
    readonly key: string;
    readonly value: ConfigValue;
};

// =============================================================================
// 2.1 let、const、var 的第一原则
// =============================================================================
//
// C++ 对照：
//   `let` 更像普通可变局部变量。
//   `const` 约束的是“绑定不能重新指向别的值”，不是 C++ 意义上的深度 const。
//   `var` 是 ES6 之前的遗留声明，函数级作用域会让 C++ 开发者误判变量生命周期。
//
// 工程建议：
//   默认使用 const。
//   需要重新赋值时使用 let。
//   新代码中不要使用 var，除非你正在解释历史行为或维护旧代码。
//
// 常见坑：
//   `const user = { name: "Ada" }` 仍允许 `user.name = "Grace"`。
//   这是因为绑定没有变，变的是对象内部属性。

function demoDeclarationKeywords(): void {
    section("2.1 let、const、var 的第一原则");
    note("C++ 对照", "`const` 不是深度不可变；它只阻止变量绑定被重新赋值。");

    const declarations: readonly DeclarationExample[] = [
        {
            keyword: "const",
            scope: "块级作用域",
            canReassign: false,
            recommendedUse: "默认选择，用来表达这个绑定不会重新指向别的值"
        },
        {
            keyword: "let",
            scope: "块级作用域",
            canReassign: true,
            recommendedUse: "循环计数器、累积状态、需要后续重新赋值的局部变量"
        },
        {
            keyword: "var",
            scope: "函数级作用域",
            canReassign: true,
            recommendedUse: "只在阅读旧代码或演示历史行为时出现"
        }
    ];

    showJson("声明关键字对比", declarations);

    const user = { name: "Ada", role: "developer" };
    user.role = "reviewer";

    showJson("const 绑定中的对象仍可修改属性", user);
    note("输出解释", "user 这个绑定没有改指向新对象，因此 const 不会阻止 role 属性变化。");
}

// =============================================================================
// 2.2 块级作用域和暂时性死区
// =============================================================================
//
// C++ 对照：
//   `let` 和 `const` 的块级作用域更接近 C++ 局部变量直觉。
//   但 JS 还有“暂时性死区”：声明在块内被提升到作用域顶部，但初始化前不能访问。
//
// 线性阅读提示：
//   本章不直接写会抛 ReferenceError 的示例，因为章节应可重复稳定运行。
//   我们用说明性数据展示规则，避免让 demo 依赖异常输出。

function demoBlockScope(): void {
    section("2.2 块级作用域与暂时性死区");
    note("C++ 对照", "`let`/`const` 的作用域边界接近 C++ 花括号块，`var` 则是历史包袱。");

    const scopeFacts = [
        "let/const 只在当前块内可见",
        "var 在函数内部可见，不受普通块限制",
        "let/const 在声明前访问会进入暂时性死区并抛 ReferenceError",
        "为了稳定演示，本章不故意触发 ReferenceError"
    ];

    let visibleInsideBlock = "outer";
    if (true) {
        const visibleOnlyHere = "inner const";
        let mutableOnlyHere = "inner let";
        mutableOnlyHere = `${mutableOnlyHere} changed`;
        visibleInsideBlock = `${visibleInsideBlock} + ${visibleOnlyHere} + ${mutableOnlyHere}`;
    }

    showJson("作用域规则摘要", scopeFacts);
    console.log(`块内计算后的外层变量: ${visibleInsideBlock}`);
    note("输出解释", "内层 const/let 只能参与块内计算，块外只能看到外层 let 的最终结果。");
}

// =============================================================================
// 2.3 number：一个类型覆盖整数和浮点
// =============================================================================
//
// C++ 对照：
//   TypeScript 的 number 对应 JavaScript 的 IEEE 754 双精度浮点。
//   它不区分 int、long、float、double。
//
// 常见坑：
//   0.1 + 0.2 不会精确等于 0.3。
//   大整数超过 Number.MAX_SAFE_INTEGER 后会丢失精度。
//
// 工程建议：
//   金额用“分”这样的整数单位表达，或者使用专门 decimal 库。
//   ID 如果超过安全整数范围，应使用 string 或 bigint。

/** Format a cent-based money value without floating point accumulation. */
export function formatMoney(value: MoneyInCents): string {
    const sign = value.cents < 0 ? "-" : "";
    const absolute = Math.abs(value.cents);
    const yuan = Math.trunc(absolute / 100);
    const cents = String(absolute % 100).padStart(2, "0");
    return `${sign}${value.currency} ${yuan}.${cents}`;
}

function demoNumberBasics(): void {
    section("2.3 number：整数和浮点都在一个类型里");
    note("C++ 对照", "不要把 number 自动理解为 int；它更接近 double，但承担了更多角色。");

    const samples = [
        { expression: "42", result: 42, description: "整数写法，运行时仍是 number" },
        { expression: "3.14", result: 3.14, description: "小数写法，运行时也是 number" },
        { expression: "0.1 + 0.2", result: 0.1 + 0.2, description: "浮点精度示例" },
        {
            expression: "Number.MAX_SAFE_INTEGER",
            result: Number.MAX_SAFE_INTEGER,
            description: "安全整数上界"
        }
    ];

    showJson("number 示例", samples);
    console.log(`金额格式化: ${formatMoney({ currency: "CNY", cents: 12345 })}`);
    note("输出解释", "金额示例使用整数分单位，避免把 123.45 作为浮点数不断累加。");
}

// =============================================================================
// 2.4 string：模板字面量和不可变文本
// =============================================================================
//
// C++ 对照：
//   JS string 是不可变值；调用 trim、replace、slice 等方法会返回新字符串。
//   它不等同于可原地修改的字符缓冲区。
//
// Unicode 坑会在 ch04 深入。
// 本章只先建立字符串和模板字面量的基本用法。

/** Create a stable display name for a user-like record. */
export function formatDisplayName(firstName: string, lastName: string): string {
    const normalizedFirst = firstName.trim();
    const normalizedLast = lastName.trim();
    return `${normalizedFirst} ${normalizedLast}`;
}

function demoStringBasics(): void {
    section("2.4 string：模板字面量和不可变文本");
    note("C++ 对照", "字符串方法通常返回新值；不要期待原字符串被原地修改。");

    const rawName = "  Ada   ";
    const trimmed = rawName.trim();
    const displayName = formatDisplayName(" Ada", "Lovelace ");
    const multiLine = `用户:
  name=${displayName}
  language=TypeScript`;

    showJson("字符串处理摘要", {
        rawName,
        trimmed,
        rawNameStillHasSpaces: rawName,
        displayName,
        multiLine
    });
    note("输出解释", "rawName 没有被 trim 原地修改，trimmed 是返回的新字符串。");
}

// =============================================================================
// 2.5 boolean：布尔值和 truthy/falsy 边界
// =============================================================================
//
// C++ 对照：
//   JS 的 if 条件可以接收任意值，再按 truthy/falsy 规则转换。
//   这比 C++ 的布尔上下文更宽松，因此默认值逻辑容易出错。
//
// 常见坑：
//   `value || defaultValue` 会把空字符串、0、false 都替换掉。
//   如果只想处理 null/undefined，应该使用 `??`。

/** Return a display limit while preserving an explicit zero. */
export function resolveLimit(input: number | undefined): number {
    return input ?? 20;
}

function demoBooleanAndFalsy(): void {
    section("2.5 boolean 与 truthy/falsy");
    note("C++ 对照", "JS 条件判断会做 truthy/falsy 转换，默认值表达式要特别小心。");

    const cases = [
        { value: "", booleanValue: Boolean(""), meaning: "空字符串是 falsy" },
        { value: "hello", booleanValue: Boolean("hello"), meaning: "非空字符串是 truthy" },
        { value: "0", booleanValue: Boolean(0), meaning: "数字 0 是 falsy" },
        { value: "false", booleanValue: Boolean(false), meaning: "false 是 falsy" },
        { value: "[]", booleanValue: Boolean([]), meaning: "空数组对象是 truthy" }
    ];

    showJson("truthy/falsy 示例", cases);
    showJson("?? 保留显式 0", {
        explicitZero: resolveLimit(0),
        missingValue: resolveLimit(undefined)
    });
    note("输出解释", "`0 ?? 20` 得到 0；如果写 `0 || 20` 会得到 20。");
}

// =============================================================================
// 2.6 null 与 undefined
// =============================================================================
//
// C++ 对照：
//   `null` 和 `undefined` 都表达“没有值”的不同语义。
//   它们不等同于 C++ 的 nullptr，因为 JS 变量还可能是未传参、属性缺失或显式置空。
//
// 工程建议：
//   对外部输入要明确区分缺失、空值和空字符串。
//   对内部模型，团队应统一何时使用 null，何时使用 undefined。

function readConfigValue(entries: readonly ConfigEntry[], key: string): ConfigValue {
    const found = entries.find((entry) => entry.key === key);
    return found?.value;
}

function demoNullAndUndefined(): void {
    section("2.6 null 与 undefined");
    note("C++ 对照", "不要把 null/undefined 简化成 nullptr；它们常来自不同的 JS 场景。");

    const entries: readonly ConfigEntry[] = [
        { key: "host", value: "localhost" },
        { key: "port", value: 3000 },
        { key: "debug", value: false }
    ];

    const missing = readConfigValue(entries, "token");
    const explicitNull: string | null = null;

    showJson("配置读取结果", {
        host: readConfigValue(entries, "host"),
        debug: readConfigValue(entries, "debug"),
        missing,
        explicitNull,
        missingType: typeof missing,
        nullType: typeof explicitNull
    });
    note("输出解释", "typeof null 的历史结果是 object；这也是 JS 早期遗留坑之一。");
}

// =============================================================================
// 2.7 bigint：超过安全整数时的选择
// =============================================================================
//
// C++ 对照：
//   C++ 有多种固定宽度整数，JS number 只有安全整数范围。
//   bigint 提供任意精度整数，但不能和 number 隐式混算。
//
// JSON 边界：
//   JSON.stringify 不能直接序列化 bigint。
//   对外输出时通常转成 string，或者使用明确协议。

/** Add bigint counters and return a string so JSON output remains stable. */
export function addLargeCounters(left: bigint, right: bigint): string {
    return (left + right).toString();
}

function demoBigInt(): void {
    section("2.7 bigint：大整数不是 number");
    note("C++ 对照", "bigint 解决整数精度问题，但它不是 unsigned long long 的直接替代品。");

    const largeA = 9_007_199_254_740_991n;
    const largeB = 10n;
    const sum = addLargeCounters(largeA, largeB);

    showJson("bigint 示例", {
        maxSafeInteger: Number.MAX_SAFE_INTEGER,
        largeA: largeA.toString(),
        largeB: largeB.toString(),
        sum,
        note: "为了 JSON 输出稳定，bigint 转成 string 展示"
    });
    note("输出解释", "bigint 和 number 不能直接相加；需要显式转换并承担精度或范围责任。");
}

// =============================================================================
// 2.8 symbol：唯一键和值身份
// =============================================================================
//
// C++ 对照：
//   symbol 更像运行时生成的唯一标识，不像 enum 或字符串常量。
//   即使描述文本相同，两个 Symbol 也不相等。
//
// 使用边界：
//   symbol 常用于避免对象属性名冲突。
//   入门阶段只需要知道它是基础类型之一，后续迭代器会遇到 Symbol.iterator。

function demoSymbol(): void {
    section("2.8 symbol：唯一标识");
    note("C++ 对照", "symbol 的重点是唯一身份，不是整数枚举值。");

    const pluginKey: symbol = Symbol("plugin");
    const anotherPluginKey: symbol = Symbol("plugin");
    const registry = {
        [pluginKey]: "formatter",
        visibleName: "demo"
    };

    showJson("symbol 示例", {
        first: String(pluginKey),
        second: String(anotherPluginKey),
        sameDescriptionButDifferentIdentity: pluginKey === anotherPluginKey,
        visibleKeys: Object.keys(registry),
        symbolKeyCount: Object.getOwnPropertySymbols(registry).length
    });
    note("输出解释", "Object.keys 只列出字符串键；symbol 键需要用专门 API 读取。");
}

// =============================================================================
// 2.9 类型标注与类型推断
// =============================================================================
//
// C++ 对照：
//   TS 推断有点像 auto，但它服务于 JS 代码的静态检查。
//   推断出的类型不会出现在运行时。
//
// 经验法则：
//   局部变量能清楚推断时，不必重复标注。
//   函数参数、公共返回值、外部边界更适合显式标注。

/** Normalize a feature flag name and reject unknown values. */
export function parseFeatureFlagName(input: string): FeatureFlagName | undefined {
    if (input === "darkMode" || input === "auditLog" || input === "betaCheckout") {
        return input;
    }
    return undefined;
}

function demoAnnotationAndInference(): void {
    section("2.9 类型标注与类型推断");
    note("C++ 对照", "可以把推断看成 auto 的便利性，但不要忘记 TS 类型会被擦除。");

    const inferredNumber = 42;
    const inferredText = "hello";
    const explicitList: readonly string[] = ["build", "test", "chapter"];
    const parsedFlag = parseFeatureFlagName("auditLog");
    const invalidFlag = parseFeatureFlagName("unknown");

    showJson("推断与标注示例", {
        inferredNumber,
        inferredText,
        explicitList,
        parsedFlag,
        invalidFlag
    });
    note("输出解释", "invalidFlag 是 undefined；调用方必须处理这个分支，不能假设解析一定成功。");
}

// =============================================================================
// 2.10 typeof：运行时操作符与类型查询的区别
// =============================================================================
//
// C++ 对照：
//   `typeof value` 在表达式位置是运行时操作符。
//   TS 还有类型位置的 typeof，用来从已有值提取类型；它不产生运行时代码。
//
// 本章先演示运行时 typeof。
// 类型位置的 typeof 会在类型系统篇继续展开。

function collectRuntimeTypes(): readonly RuntimeTypeSample[] {
    const samples = [
        { expression: "42", value: 42 },
        { expression: "\"text\"", value: "text" },
        { expression: "true", value: true },
        { expression: "undefined", value: undefined },
        { expression: "null", value: null },
        { expression: "10n", value: 10n },
        { expression: "Symbol(\"id\")", value: Symbol("id") },
        { expression: "{}", value: {} },
        { expression: "() => undefined", value: () => undefined }
    ] as const;

    return samples.map((sample) => ({
        expression: sample.expression,
        runtimeType: typeof sample.value,
        valuePreview: typeof sample.value === "symbol" ? String(sample.value) : String(sample.value)
    }));
}

function demoTypeof(): void {
    section("2.10 typeof：运行时类型观察");
    note("C++ 对照", "运行时 typeof 不是 RTTI；它只返回 JS 规定的一小组字符串结果。");

    showJson("typeof 结果", collectRuntimeTypes());
    note("输出解释", "typeof null 得到 object 是历史遗留；判断 null 应使用 value === null。");
}

// =============================================================================
// 2.11 字面量类型入门
// =============================================================================
//
// C++ 对照：
//   字面量类型可以把某个字符串或数字本身作为类型。
//   它常用于限制状态、命令名和配置键，比随意 string 更精确。
//
// 线性阅读提示：
//   联合类型和收窄会在 ch10 深入。
//   这里先把字面量类型当成“更窄的 string/number”即可。

function enableFeature(name: FeatureFlagName): string {
    return `feature:${name}:enabled`;
}

function demoLiteralTypes(): void {
    section("2.11 字面量类型入门");
    note("C++ 对照", "字面量联合能表达有限取值，接近枚举的一部分用途，但运行时仍是字符串。");

    const defaultFlag: FeatureFlagName = "darkMode";
    const enabled = enableFeature(defaultFlag);
    const config = {
        mode: "development",
        retry: 3,
        strict: true
    } as const;

    showJson("字面量类型示例", {
        defaultFlag,
        enabled,
        config,
        explanation: "as const 会让对象属性推断为只读字面量，而不是宽泛 string/number/boolean"
    });
    note("输出解释", "enableFeature 只接受三个已知 feature 名称，减少字符串拼写错误。");
}

// =============================================================================
// 2.12 本章复盘
// =============================================================================
//
// C++ 对照：
//   本章最重要的转换是：TS 变量声明和类型标注服务于 JS 运行时。
//   不要把 C++ 的值类别、整数族、const 深度语义直接套过来。

function demoChapterReview(): void {
    section("2.12 本章复盘");
    note("C++ 对照", "先把 JS 基础值模型学稳，再用 TypeScript 类型系统约束它们。");

    const review = [
        "默认 const，需要重新赋值才用 let，新代码避免 var",
        "number 是 IEEE 754 双精度浮点，不区分 int/long/double",
        "string 不可变，字符串方法返回新值",
        "?? 只处理 null/undefined，|| 会吞掉 0、false、空字符串",
        "bigint 不能直接 JSON.stringify，常转成 string 输出",
        "symbol 表示唯一身份，描述相同也不相等",
        "类型标注和泛型都会在编译后擦除",
        "typeof null === \"object\" 是历史坑"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 2. */
export function runChapter(): void {
    demoDeclarationKeywords();
    demoBlockScope();
    demoNumberBasics();
    demoStringBasics();
    demoBooleanAndFalsy();
    demoNullAndUndefined();
    demoBigInt();
    demoSymbol();
    demoAnnotationAndInference();
    demoTypeof();
    demoLiteralTypes();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
