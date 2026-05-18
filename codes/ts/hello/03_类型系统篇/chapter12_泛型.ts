// =============================================================================
// 第 12 章：泛型
// =============================================================================
//
// 【学习目标】
//   1. 掌握泛型函数的推导与显式标注边界
//   2. 理解约束 extends、keyof、泛型接口和类型别名
//   3. 理解默认类型参数、typeof + 泛型组合、类型推导与运行时分离
//
// 【运行方式】
//   npm run build
//   node dist/03_类型系统篇/chapter12_泛型.js
//   或 npm run chapter -- 12（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type Result<T, E = string> =
    | { ok: true; value: T }
    | { ok: false; error: E };

interface Keyed {
    id: string;
}

interface Box<T> {
    value: T;
    label: string;
}

interface Repository<T extends Keyed> {
    upsert(item: T): void;
    find(id: string): T | undefined;
    list(): readonly T[];
}

const compilerDefaults = {
    strict: true,
    target: "ES2023",
    module: "NodeNext"
} as const;

type CompilerDefaults = typeof compilerDefaults;

function first<T>(values: readonly T[]): T | undefined {
    return values[0];
}

function mapValues<T, U>(values: readonly T[], mapper: (value: T, index: number) => U): U[] {
    return values.map(mapper);
}

function parseInteger(text: string): Result<number> {
    const value = Number.parseInt(text, 10);
    if (Number.isNaN(value)) {
        return { ok: false, error: `cannot parse integer: ${text}` };
    }
    return { ok: true, value };
}

function formatId(id: number): string;
function formatId(id: string): string;
function formatId(id: number | string): string {
    return typeof id === "number" ? `id-${id.toString(10)}` : id.trim();
}

function getProperty<T extends object, K extends keyof T>(source: T, key: K): T[K] {
    return source[key];
}

function pick<T extends object, const K extends readonly (keyof T)[]>(
    source: T,
    keys: K
): Pick<T, K[number]> {
    const result = {} as Pick<T, K[number]>;
    for (const key of keys) {
        result[key] = source[key];
    }
    return result;
}

function createBox<T>(value: T, label: string): Box<T> {
    return { value, label };
}

function createInbox<T = string>(seed?: readonly T[]): T[] {
    return [...(seed ?? [])];
}

function createRepository<T extends Keyed>(): Repository<T> {
    const items = new Map<string, T>();

    return {
        upsert(item) {
            items.set(item.id, item);
        },
        find(id) {
            return items.get(id);
        },
        list() {
            return [...items.values()];
        }
    };
}

function readCompilerDefault<K extends keyof CompilerDefaults>(key: K): CompilerDefaults[K] {
    return compilerDefaults[key];
}

// =============================================================================
// 12.1 泛型函数的推导与显式标注
// =============================================================================
//
// C++ 对照：
//   泛型像模板，但 TS 泛型不会生成多份特化代码。
//   运行时只有一份函数实现，类型信息在编译阶段被擦除。
//
// 常见坑：
//   看到泛型参数就急着显式写 `<T>`，通常反而会降低可读性。
//   先让编译器推导，只有边界不清楚时再显式补充。

function demoInferenceAndReturnTypes(): void {
    section("12.1 泛型函数的推导与显式标注");
    note("C++ 对照", "TS 泛型不会像模板那样在运行时实例化出不同版本的函数。");

    const words = ["ts", "js", "cpp"] as const;
    const firstWord = first(words);
    const squares = mapValues([1, 2, 3], (value, index) => value * value + index);
    const parsedOk = parseInteger("42");
    const parsedFail = parseInteger("oops");

    showJson("推导结果", {
        firstWord,
        squares,
        parsedOk,
        parsedFail,
        formattedNumber: formatId(7),
        formattedText: formatId("  chapter-12 ")
    });
    note("输出解释", "first/mapValues/parseInteger 都只生成一份运行时代码；不同调用点只是类型推导不同。");
}

// =============================================================================
// 12.2 约束 extends、keyof 和安全索引
// =============================================================================
//
// C++ 对照：
//   keyof 像把对象成员名提取成一个受限枚举。
//   约束 extends 让泛型参数至少满足某个最小接口。

function demoConstraintsAndKeyof(): void {
    section("12.2 约束 extends、keyof 和安全索引");
    note("C++ 对照", "约束让模板参数先满足最小接口，再谈具体实现细节。");

    const profile = {
        id: "u1",
        name: "Ada",
        role: "admin",
        active: true,
        score: 98
    };

    const name = getProperty(profile, "name");
    const slim = pick(profile, ["id", "role", "active"] as const);

    showJson("keyof + 约束", {
        name,
        slim,
        profileKeys: Object.keys(profile)
    });
    note("输出解释", "pick 只允许传入 profile 上真实存在的键；这类约束能在编译期挡住拼写错误。");
}

// =============================================================================
// 12.3 泛型接口与类型别名
// =============================================================================
//
// C++ 对照：
//   泛型接口/别名像带模板参数的容器和别名。
//   它们适合表达“同一形状，承载不同数据”。

function demoGenericInterfaceAndAlias(): void {
    section("12.3 泛型接口与类型别名");
    note("C++ 对照", "Box<T>、Repository<T> 很像模板容器，但 TS 不做运行时特化。");

    const stringBox = createBox("chapter", "title");
    const numberBox = createBox(12, "chapter number");
    const repo = createRepository<{ id: string; name: string }>();
    repo.upsert({ id: "u1", name: "Ada" });
    repo.upsert({ id: "u2", name: "Grace" });

    showJson("泛型接口示例", {
        stringBox,
        numberBox,
        repoItems: repo.list()
    });
    note("输出解释", "同一个 Repository<T> 既能装用户，也能装订单，只要 T 满足 id 约束。");
}

// =============================================================================
// 12.4 默认类型参数
// =============================================================================
//
// C++ 对照：
//   默认模板参数能减少调用方重复。
//   但默认值只是在类型层给一个更顺手的起点，不会改变运行时行为。

function demoDefaultTypeParameters(): void {
    section("12.4 默认类型参数");
    note("C++ 对照", "默认类型参数只是省掉显式写法，不会生成新的运行时代码。");

    const inbox = createInbox();
    const numericInbox = createInbox<number>([1, 2, 3]);

    showJson("默认类型参数", {
        inbox,
        numericInbox,
        inboxIsStringArray: typeof inbox[0] === "string" || inbox.length === 0
    });
    note("输出解释", "createInbox() 默认推成 string[]；显式传入 number 时，返回值就跟着变。");
}

// =============================================================================
// 12.5 typeof + 泛型组合
// =============================================================================
//
// C++ 对照：
//   typeof 可以把现有值的形状转成类型，再配合泛型约束复用。
//   这比手写一遍字段清单更稳。

function demoTypeofWithGenerics(): void {
    section("12.5 typeof + 泛型组合");
    note("C++ 对照", "先定义默认值对象，再用 typeof 把它的形状直接拿来当类型。");

    const strictMode = readCompilerDefault("strict");
    const target = readCompilerDefault("target");
    const moduleKind = readCompilerDefault("module");
    const defaultsSnapshot = pick(compilerDefaults, ["strict", "target", "module"] as const);

    showJson("typeof + 泛型", {
        strictMode,
        target,
        moduleKind,
        defaultsSnapshot
    });
    note("输出解释", "compilerDefaults 是运行时对象；CompilerDefaults 是它在编译期的静态形状。");
}

// =============================================================================
// 12.6 工程场景：有类型的配置仓库
// =============================================================================
//
// C++ 对照：
//   配置表、仓库和索引表都很适合用泛型封装。
//   关键是让数据模型和存取逻辑分离，而不是把它们塞进 any。

function demoRepositoryScenario(): void {
    section("12.6 工程场景：有类型的配置仓库");
    note("C++ 对照", "模板仓库让不同数据模型共享同一套 CRUD 逻辑。");

    type Employee = {
        id: string;
        name: string;
        department: string;
    };

    const employees = createRepository<Employee>();
    employees.upsert({ id: "e1", name: "Ada", department: "platform" });
    employees.upsert({ id: "e2", name: "Grace", department: "security" });

    showJson("仓库快照", {
        found: employees.find("e1"),
        missing: employees.find("e9"),
        ids: employees.list().map((item) => item.id)
    });
    note("输出解释", "仓库只接受带 id 的实体，调用方不需要也不应该碰内部 Map。");
}

// =============================================================================
// 12.7 本章复盘
// =============================================================================
//
// C++ 对照：
//   TS 泛型的重点不是“会不会写模板”，而是“怎样让接口更清楚”。
//   先推导，后显式；先约束边界，再谈灵活性。

function demoChapterReview(): void {
    section("12.7 本章复盘");
    note("C++ 对照", "TS 泛型是编译期工具，运行时只保留一份实现。");

    const summary = [
        "泛型函数优先让编译器推导参数",
        "extends 约束让类型参数满足最小接口",
        "keyof 让属性名变成受限的类型集合",
        "泛型接口/类型别名适合表达复用的对象形状",
        "默认类型参数减少边界处的重复",
        "typeof 可以把现有值的形状转成类型",
        "Repository<T> 这类封装比 any 更适合工程代码"
    ];

    showJson("关键结论", summary);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 12. */
export function runChapter(): void {
    demoInferenceAndReturnTypes();
    demoConstraintsAndKeyof();
    demoGenericInterfaceAndAlias();
    demoDefaultTypeParameters();
    demoTypeofWithGenerics();
    demoRepositoryScenario();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
