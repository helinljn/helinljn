// =============================================================================
// 第 9 章：类型标注与推断
// =============================================================================
//
// 【学习目标】
//   1. 掌握变量、参数、返回值、对象属性的常见标注位置
//   2. 理解 TypeScript 推断规则、字面量 widening 和 as const
//   3. 掌握 satisfies 运算符、类型断言 as T 的边界
//   4. 理解 typeof 类型操作符和运行时 typeof 的差异
//
// 【运行方式】
//   npm run build
//   node dist/03_类型系统篇/chapter09_类型标注与推断.js
//   或 npm run chapter -- 9（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type UserRecord = {
    readonly id: string;
    readonly name: string;
    readonly active: boolean;
};

type ServerConfig = {
    readonly host: string;
    readonly port: number;
    readonly mode: "development" | "production";
    readonly features: readonly string[];
};

type ParseResult = {
    readonly ok: boolean;
    readonly value?: unknown;
    readonly error?: string;
};

// =============================================================================
// 9.1 类型标注出现在哪里
// =============================================================================
//
// C++ 对照：
//   TS 标注看起来像静态语言签名，但编译后会被擦除。
//   它约束开发期代码，不会自动生成运行时校验。
//
// 经验法则：
//   公共函数参数和返回值建议显式标注。
//   局部变量能清楚推断时可以省略标注，让代码少重复。

/** Format a user record into a deterministic display string. */
export function formatUser(user: UserRecord): string {
    return `${user.id}:${user.name}:${user.active ? "active" : "inactive"}`;
}

function demoAnnotationPositions(): void {
    section("9.1 类型标注出现在哪里");
    note("C++ 对照", "函数参数和返回值标注像签名约束，但不会留到运行时。");

    const user: UserRecord = { id: "u1", name: "Ada", active: true };
    const retries: number = 3;
    const labels: string[] = ["build", "test", "chapter"];
    const formatter: (input: UserRecord) => string = formatUser;

    showJson("标注位置示例", {
        user,
        retries,
        labels,
        formatted: formatter(user)
    });
    note("输出解释", "这些标注帮助编译器检查代码；dist JS 中不会保留 UserRecord 类型。");
}

// =============================================================================
// 9.2 推断规则：能推断就别重复
// =============================================================================
//
// C++ 对照：
//   TS 推断可以类比 auto 的便利性，但它推断的是 JS 值的静态类型。
//   推断结果仍只存在于编译期。
//
// 常见坑：
//   过度标注会让代码冗长，还可能把本可推断的字面量信息写宽。
//   但外部边界不应完全依赖推断，因为读者需要明确 API 契约。

function demoInferenceBasics(): void {
    section("9.2 类型推断：局部变量通常不必重复标注");
    note("C++ 对照", "类似 auto 能减少重复，但 TS 推断不会产生运行时类型对象。");

    const language = "TypeScript";
    const count = 6;
    const enabled = true;
    const names = ["Ada", "Grace", "Linus"];
    const user = {
        id: "u2",
        name: "Grace",
        active: false
    };

    showJson("推断示例", {
        language,
        count,
        enabled,
        names,
        user,
        recommendation: "local obvious values can rely on inference"
    });
    note("输出解释", "这些局部值的类型从初始化表达式推断出来，不需要把 string/number/boolean 重写一遍。");
}

// =============================================================================
// 9.3 字面量 widening
// =============================================================================
//
// C++ 对照：
//   TS 会在某些位置把具体字面量推宽成 string/number/boolean。
//   这是为了允许后续重新赋值，而不是表示运行时值发生转换。
//
// 常见坑：
//   `let mode = "development"` 推断为 string，因为之后可能改成别的字符串。
//   `const mode = "development"` 更容易保留字面量类型。

function demoLiteralWidening(): void {
    section("9.3 字面量 widening：从具体值到宽类型");
    note("C++ 对照", "widening 是编译期类型策略，不是运行时转换。");

    let mutableMode = "development";
    mutableMode = "staging";
    const fixedMode = "production";

    const objectWidened = {
        mode: "development",
        retries: 3
    };

    showJson("字面量推宽示例", {
        mutableMode,
        fixedMode,
        objectWidened,
        explanation: "let 和普通对象属性通常会推宽，方便后续赋值"
    });
    note("输出解释", "fixedMode 的值没变；widening 只影响编译器如何理解后续赋值可能性。");
}

// =============================================================================
// 9.4 as const：保留字面量和只读信息
// =============================================================================
//
// C++ 对照：
//   as const 不是 C++ const。
//   它让 TS 把表达式推断为更窄的只读字面量类型，但运行时对象不会自动深冻结。
//
// 常见坑：
//   `as const` 是类型层约束，不会阻止其他运行时别名修改对象。
//   若需要运行时不可变，还要使用 freeze 或不可变数据策略。

function demoAsConst(): void {
    section("9.4 as const：保留字面量类型");
    note("C++ 对照", "as const 是 TS 类型推断指令，不等于运行时深度 const。");

    const routes = ["/health", "/users", "/jobs"] as const;
    const config = {
        mode: "development",
        features: ["audit", "debug"]
    } as const;

    showJson("as const 示例", {
        routes,
        firstRoute: routes[0],
        config
    });
    note("输出解释", "routes 被推断为只读元组；config 的属性也被推断为只读字面量。");
}

// =============================================================================
// 9.5 satisfies：检查形状，同时保留具体信息
// =============================================================================
//
// C++ 对照：
//   satisfies 没有直接 C++ 等价物。
//   它更像“确认这个对象符合某接口，但不要把表达式本身强行改成那个宽类型”。
//
// 工程场景：
//   配置对象很适合 satisfies。
//   它能检查必需字段，又保留具体字段和值的推断信息。

const defaultConfig = {
    host: "localhost",
    port: 3000,
    mode: "development",
    features: ["audit", "debug"]
} satisfies ServerConfig;

function demoSatisfies(): void {
    section("9.5 satisfies：检查形状但保留具体类型");
    note("C++ 对照", "satisfies 是 TS 编译期检查工具，不会生成运行时代码。");

    showJson("satisfies 配置示例", {
        defaultConfig,
        featureCount: defaultConfig.features.length,
        explanation: "对象必须满足 ServerConfig，同时 defaultConfig 仍保留自身具体结构"
    });
    note("输出解释", "如果漏掉 port 或写错 mode，编译期会报错；运行时输出仍只是普通对象。");
}

// =============================================================================
// 9.6 类型断言 as T
// =============================================================================
//
// C++ 对照：
//   `as T` 更像告诉编译器“相信我”，不是 static_cast 的运行时检查。
//   它不会转换值，也不会验证对象是否真的有那些字段。
//
// 常见坑：
//   对外部 JSON 直接 `as UserRecord` 很危险。
//   正确做法是先按 unknown 接收，再验证字段。

function unsafeParseJson(text: string): unknown {
    return JSON.parse(text) as unknown;
}

function demoTypeAssertion(): void {
    section("9.6 类型断言 as T：只影响编译期");
    note("C++ 对照", "as T 不是 static_cast；它不会在运行时检查或转换对象。");

    const raw = unsafeParseJson("{\"id\":\"u3\",\"name\":\"Linus\",\"active\":true}");
    const asserted = raw as UserRecord;

    showJson("类型断言示例", {
        raw,
        formattedAfterAssertion: formatUser(asserted),
        warning: "这里的 JSON 是固定可信样例；真实外部输入不能只靠 as"
    });
    note("输出解释", "asserted 没有经过运行时验证；只是编译器允许把它当作 UserRecord。");
}

// =============================================================================
// 9.7 unknown 边界和安全解析
// =============================================================================
//
// C++ 对照：
//   unknown 类似“还没有证明类型”的输入边界。
//   它强迫你先检查，再把值当作具体类型使用。
//
// 类型边界：
//   JSON.parse 返回 any 的历史类型很宽松。
//   本教程在外部输入边界主动收敛到 unknown。

/** Return true when a value has the UserRecord runtime shape. */
export function isUserRecord(value: unknown): value is UserRecord {
    if (typeof value !== "object" || value === null) {
        return false;
    }
    const candidate = value as { id?: unknown; name?: unknown; active?: unknown };
    return typeof candidate.id === "string"
        && typeof candidate.name === "string"
        && typeof candidate.active === "boolean";
}

/** Parse fixed JSON text into a UserRecord with runtime validation. */
export function parseUserRecord(text: string): ParseResult {
    try {
        const parsed = JSON.parse(text) as unknown;
        if (!isUserRecord(parsed)) {
            return { ok: false, error: "JSON is not a UserRecord" };
        }
        return { ok: true, value: parsed };
    } catch (error) {
        return { ok: false, error: error instanceof Error ? error.message : "unknown parse error" };
    }
}

function demoUnknownBoundary(): void {
    section("9.7 unknown：外部输入先验证再使用");
    note("C++ 对照", "unknown 表示类型尚未证明，类似把外部字节解析成结构体前必须检查格式。");

    const cases = [
        parseUserRecord("{\"id\":\"u4\",\"name\":\"Margaret\",\"active\":true}"),
        parseUserRecord("{\"id\":\"u5\",\"name\":\"NoActive\"}"),
        parseUserRecord("{bad json")
    ];

    showJson("安全解析结果", cases);
    note("输出解释", "只有字段类型完整匹配时才返回 ok=true；错误 JSON 和缺字段都会被拒绝。");
}

// =============================================================================
// 9.8 typeof 类型操作符
// =============================================================================
//
// C++ 对照：
//   表达式位置的 typeof 是运行时操作符，返回字符串。
//   类型位置的 typeof 是 TS 类型查询，用已有值推导类型。
//
// 线性阅读提示：
//   这里通过函数签名使用类型位置 typeof。
//   它不会生成运行时代码。

const compilerDefaults = {
    target: "ES2023",
    module: "NodeNext",
    strict: true
} as const;

/** Return compiler defaults using a type derived from the value itself. */
export function getCompilerDefaults(): typeof compilerDefaults {
    return compilerDefaults;
}

function demoTypeofTypeOperator(): void {
    section("9.8 typeof 类型操作符");
    note("C++ 对照", "类型位置 typeof 是 TS 编译期能力，不等同于运行时 typeof。");

    const defaults = getCompilerDefaults();
    const runtimeTypes = {
        target: typeof defaults.target,
        strict: typeof defaults.strict
    };

    showJson("typeof 对比", {
        defaults,
        runtimeTypes,
        explanation: "getCompilerDefaults 的返回类型来自 typeof compilerDefaults"
    });
    note("输出解释", "runtimeTypes 是 JS typeof 的字符串结果；函数返回类型则由 TS 类型查询得到。");
}

// =============================================================================
// 9.9 工程场景：配置加载边界
// =============================================================================
//
// C++ 对照：
//   配置加载像把文本配置解析成强类型结构。
//   TS 类型约束编译期，运行时仍需要检查配置字段。
//
// 真实场景：
//   从 JSON/环境变量/CLI 参数来的配置不能直接相信。
//   本章用固定 JSON 字符串演示可重复运行的边界。

/** Validate and normalize a server config-like unknown value. */
export function normalizeServerConfig(value: unknown): ServerConfig | undefined {
    if (typeof value !== "object" || value === null) {
        return undefined;
    }
    const candidate = value as {
        host?: unknown;
        port?: unknown;
        mode?: unknown;
        features?: unknown;
    };
    if (typeof candidate.host !== "string" || typeof candidate.port !== "number") {
        return undefined;
    }
    if (candidate.mode !== "development" && candidate.mode !== "production") {
        return undefined;
    }
    if (!Array.isArray(candidate.features) || !candidate.features.every((feature) => typeof feature === "string")) {
        return undefined;
    }
    return {
        host: candidate.host,
        port: candidate.port,
        mode: candidate.mode,
        features: candidate.features
    };
}

function demoConfigBoundary(): void {
    section("9.9 工程场景：配置加载边界");
    note("C++ 对照", "外部配置进入程序时，必须先验证，再交给内部强类型代码。");

    const rawConfig = JSON.parse("{\"host\":\"localhost\",\"port\":3000,\"mode\":\"development\",\"features\":[\"audit\"]}") as unknown;
    const normalized = normalizeServerConfig(rawConfig);

    showJson("配置加载结果", {
        rawConfig,
        normalized,
        mergedWithDefaults: normalized === undefined ? defaultConfig : { ...defaultConfig, ...normalized }
    });
    note("输出解释", "normalized 成功后才进入内部配置合并；否则会退回默认配置或返回错误。");
}

// =============================================================================
// 9.10 本章复盘
// =============================================================================
//
// C++ 对照：
//   本章重点是把“编译期知道什么”和“运行时真的检查什么”分清楚。

function demoChapterReview(): void {
    section("9.10 本章复盘");
    note("C++ 对照", "TS 推断像 auto，但 as T 不是安全转换；类型系统不会替你验证外部数据。");

    const review = [
        "参数和公共返回值适合显式标注，局部明显值可以依赖推断",
        "let 和普通对象属性可能发生字面量 widening",
        "as const 保留字面量和只读信息，但不是运行时冻结",
        "satisfies 检查表达式符合目标类型，同时保留表达式自身信息",
        "as T 不做运行时检查，不应直接用于外部输入",
        "unknown 迫使你先验证，再使用具体类型",
        "类型位置 typeof 从值提取类型，表达式位置 typeof 返回运行时字符串",
        "配置、JSON、环境变量、CLI 参数都属于运行时边界"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 9. */
export function runChapter(): void {
    demoAnnotationPositions();
    demoInferenceBasics();
    demoLiteralWidening();
    demoAsConst();
    demoSatisfies();
    demoTypeAssertion();
    demoUnknownBoundary();
    demoTypeofTypeOperator();
    demoConfigBoundary();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
