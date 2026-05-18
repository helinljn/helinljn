// =============================================================================
// 第 10 章：联合类型与收窄
// =============================================================================
//
// 【学习目标】
//   1. 掌握联合类型 A | B 的读法和访问规则
//   2. 熟悉 typeof、instanceof、in、=== 等常见收窄手段
//   3. 掌握判别联合、switch 穷尽性检查和 never
//   4. 区分 unknown 与 any，并能编写类型谓词
//
// 【运行方式】
//   npm run build
//   node dist/03_类型系统篇/chapter10_联合类型与收窄.js
//   或 npm run chapter -- 10（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type Circle = {
    readonly kind: "circle";
    readonly radius: number;
};

export type Rectangle = {
    readonly kind: "rectangle";
    readonly width: number;
    readonly height: number;
};

export type Triangle = {
    readonly kind: "triangle";
    readonly base: number;
    readonly height: number;
};

export type Shape = Circle | Rectangle | Triangle;

type ApiSuccess<T> = {
    readonly ok: true;
    readonly data: T;
};

type ApiFailure = {
    readonly ok: false;
    readonly code: "bad_request" | "not_found" | "internal";
    readonly message: string;
};

type ApiResult<T> = ApiSuccess<T> | ApiFailure;

class ValidationError extends Error {
    public readonly field: string;

    public constructor(field: string, message: string) {
        super(message);
        this.name = "ValidationError";
        this.field = field;
    }
}

class NotFoundError extends Error {
    public readonly resource: string;

    public constructor(resource: string) {
        super(`${resource} not found`);
        this.name = "NotFoundError";
        this.resource = resource;
    }
}

// =============================================================================
// 10.1 联合类型：值可能属于多种形状
// =============================================================================
//
// C++ 对照：
//   联合类型可以类比 std::variant 的使用直觉：一个值在某一刻是多个候选类型之一。
//   区别是 TS 联合类型只存在于编译期，运行时仍是普通 JS 值。
//
// 常见坑：
//   对 `string | number` 不能直接调用只属于 string 的方法。
//   必须先收窄到 string，编译器才允许访问 string 专属 API。

/** Normalize a user-facing ID that may arrive as string or number. */
export function normalizeId(id: string | number): string {
    if (typeof id === "number") {
        return `#${id.toString(10)}`;
    }
    return id.trim();
}

function demoUnionBasics(): void {
    section("10.1 联合类型：一个值的多种可能");
    note("C++ 对照", "联合类型有 std::variant 的直觉，但没有运行时 variant 容器对象。");

    const values: Array<string | number> = [" u1 ", 42, "u3"];
    const normalized = values.map((value) => normalizeId(value));

    showJson("联合类型基础示例", { values, normalized });
    note("输出解释", "normalizeId 先用 typeof 收窄，再分别处理 number 和 string。");
}

// =============================================================================
// 10.2 typeof 收窄
// =============================================================================
//
// C++ 对照：
//   typeof 是 JS 运行时操作符，返回固定字符串集合。
//   TS 能理解这些检查，并在对应分支中缩小类型。
//
// 使用边界：
//   typeof null 是 object，这是历史坑。
//   判断对象时必须额外排除 null。

function describePrimitive(value: string | number | boolean | null | undefined): string {
    if (value === null) {
        return "null";
    }
    if (value === undefined) {
        return "undefined";
    }
    if (typeof value === "string") {
        return `string:${value.toUpperCase()}`;
    }
    if (typeof value === "number") {
        return Number.isFinite(value) ? `number:${value}` : "number:special";
    }
    return `boolean:${value ? "true" : "false"}`;
}

function demoTypeofNarrowing(): void {
    section("10.2 typeof 收窄");
    note("C++ 对照", "typeof 是运行时检查；TS 根据检查结果在分支内缩小联合类型。");

    const samples = ["ts", 6, true, null, undefined, Number.NaN] as const;
    const described = samples.map((sample) => describePrimitive(sample));

    showJson("typeof 收窄结果", described);
    note("输出解释", "null 必须先单独判断，否则 typeof null 会得到 object。");
}

// =============================================================================
// 10.3 instanceof 收窄
// =============================================================================
//
// C++ 对照：
//   instanceof 检查运行时原型链，不是 C++ RTTI 的直接等价物。
//   它适合 class 实例和内置构造函数，例如 Date、Error。
//
// 常见坑：
//   只存在于 TS 类型系统中的 interface 不能用于 instanceof。
//   因为 interface 编译后不存在。

function explainError(error: Error): string {
    if (error instanceof ValidationError) {
        return `validation:${error.field}:${error.message}`;
    }
    if (error instanceof NotFoundError) {
        return `not-found:${error.resource}`;
    }
    return `generic:${error.message}`;
}

function demoInstanceofNarrowing(): void {
    section("10.3 instanceof 收窄");
    note("C++ 对照", "instanceof 看 JS 原型链；interface 无法参与，因为运行时没有 interface 对象。");

    const errors: readonly Error[] = [
        new ValidationError("email", "invalid email"),
        new NotFoundError("user"),
        new Error("unknown")
    ];

    showJson("instanceof 收窄结果", errors.map((error) => explainError(error)));
    note("输出解释", "每个分支里 error 被缩小为对应 class，能访问 field/resource。");
}

// =============================================================================
// 10.4 in 运算符收窄
// =============================================================================
//
// C++ 对照：
//   `in` 检查属性是否存在于对象或原型链上。
//   它不是字段偏移检查，也不会验证属性值类型。
//
// 使用边界：
//   `in` 很适合区分对象联合中“某个字段只存在于一支”的情况。
//   外部输入仍需进一步检查字段值类型。

type FileJob = {
    readonly path: string;
    readonly encoding: "utf8" | "binary";
};

type HttpJob = {
    readonly url: string;
    readonly method: "GET" | "POST";
};

function describeJob(job: FileJob | HttpJob): string {
    if ("path" in job) {
        return `file:${job.path}:${job.encoding}`;
    }
    return `http:${job.method}:${job.url}`;
}

function demoInNarrowing(): void {
    section("10.4 in 运算符收窄");
    note("C++ 对照", "`in` 根据运行时属性存在性帮助 TS 缩小对象联合类型。");

    const jobs: readonly (FileJob | HttpJob)[] = [
        { path: "README.md", encoding: "utf8" },
        { url: "http://localhost/health", method: "GET" }
    ];

    showJson("in 收窄结果", jobs.map((job) => describeJob(job)));
    note("输出解释", "有 path 的分支按 FileJob 处理，否则按 HttpJob 处理。");
}

// =============================================================================
// 10.5 相等比较收窄
// =============================================================================
//
// C++ 对照：
//   字符串字面量联合经常承担 enum class 的一部分角色。
//   通过 === 比较某个字面量，TS 能在分支中知道更具体的类型。
//
// 工程建议：
//   对状态、角色、命令名这类有限集合，优先使用字面量联合。

type Role = "guest" | "member" | "admin";

function permissionsForRole(role: Role): readonly string[] {
    if (role === "admin") {
        return ["read", "write", "delete"];
    }
    if (role === "member") {
        return ["read", "write"];
    }
    return ["read"];
}

function demoEqualityNarrowing(): void {
    section("10.5 === 相等比较收窄");
    note("C++ 对照", "字面量联合常用于表达有限状态，类似 enum class 的一部分用途。");

    const roles: readonly Role[] = ["guest", "member", "admin"];
    const permissions = roles.map((role) => ({
        role,
        permissions: permissionsForRole(role)
    }));

    showJson("角色权限", permissions);
    note("输出解释", "role === 'admin' 后，TS 知道该分支 role 就是 admin。");
}

// =============================================================================
// 10.6 判别联合和 switch 穷尽性
// =============================================================================
//
// C++ 对照：
//   判别联合接近 std::variant + visit 的建模习惯。
//   每个成员都有共同的 kind 字段，switch 根据 kind 分派。
//
// 常见坑：
//   新增联合成员后忘记更新 switch。
//   never 穷尽性检查可以让编译器在遗漏分支时报错。

/** Compute the area of a discriminated union shape. */
export function area(shape: Shape): number {
    switch (shape.kind) {
        case "circle":
            return Math.PI * shape.radius * shape.radius;
        case "rectangle":
            return shape.width * shape.height;
        case "triangle":
            return (shape.base * shape.height) / 2;
        default: {
            const unreachable: never = shape;
            return unreachable;
        }
    }
}

function demoDiscriminatedUnion(): void {
    section("10.6 判别联合和 switch 穷尽性");
    note("C++ 对照", "kind 字段像 variant 的 tag；never 分支帮助发现未来新增类型但忘记处理。");

    const shapes: readonly Shape[] = [
        { kind: "circle", radius: 2 },
        { kind: "rectangle", width: 3, height: 4 },
        { kind: "triangle", base: 5, height: 6 }
    ];

    showJson("面积计算", shapes.map((shape) => ({ shape, area: area(shape) })));
    note("输出解释", "每个 kind 分支都能访问该形状独有字段，例如 circle.radius。");
}

// =============================================================================
// 10.7 unknown vs any
// =============================================================================
//
// C++ 对照：
//   any 类似关闭类型检查，unknown 类似“还没证明是什么”的安全边界。
//   初学者应优先使用 unknown 表达外部输入。
//
// 常见坑：
//   any 会把错误传播到后续代码，编译器无法再帮你。
//   unknown 会要求你先做收窄。

function parseUnknownJson(text: string): unknown {
    return JSON.parse(text) as unknown;
}

function demoUnknownVsAny(): void {
    section("10.7 unknown vs any");
    note("C++ 对照", "unknown 保留检查压力；any 则像强行关闭编译器保护。");

    const parsed = parseUnknownJson("{\"name\":\"Ada\",\"age\":36}");
    const safeName = hasName(parsed) ? parsed.name : "missing";

    const unsafeAny = JSON.parse("{\"name\":123}") as any;
    const unsafeName = String(unsafeAny.name);

    showJson("unknown 与 any 对比", {
        safeName,
        unsafeName,
        guidance: "real code should narrow unknown instead of spreading any"
    });
    note("输出解释", "safeName 经过类型谓词验证；unsafeAny.name 没有任何编译期保护。");
}

// =============================================================================
// 10.8 类型谓词
// =============================================================================
//
// C++ 对照：
//   类型谓词像把运行时检查结果反馈给编译器。
//   它不改变运行时对象，只告诉 TS 在 true 分支中可以相信某个更窄类型。
//
// 类型边界：
//   谓词函数必须认真校验字段。
//   如果谓词撒谎，后续代码会在运行时出错。

/** Return true when a value is an object with a string name property. */
export function hasName(value: unknown): value is { name: string } {
    return typeof value === "object"
        && value !== null
        && "name" in value
        && typeof (value as { name?: unknown }).name === "string";
}

function demoTypePredicate(): void {
    section("10.8 类型谓词：把运行时检查反馈给编译器");
    note("C++ 对照", "谓词不是转换；它只是让编译器知道 true 分支已经检查过形状。");

    const values: readonly unknown[] = [
        { name: "Ada" },
        { name: 42 },
        null,
        "Grace"
    ];
    const names = values.map((value) => (hasName(value) ? value.name.toUpperCase() : "not-a-name"));

    showJson("类型谓词结果", names);
    note("输出解释", "只有 name 为 string 的对象进入 true 分支。");
}

// =============================================================================
// 10.9 工程场景：API Result
// =============================================================================
//
// C++ 对照：
//   Result 风格类似 `std::expected<T, E>` 的思路。
//   成功和失败分支都有明确形状，调用方必须处理。
//
// 真实场景：
//   HTTP handler、CLI、库函数都可以用判别联合表达可恢复错误。

/** Convert a result into a stable display string. */
export function renderApiResult<T>(result: ApiResult<T>): string {
    if (result.ok) {
        return `ok:${JSON.stringify(result.data)}`;
    }
    return `error:${result.code}:${result.message}`;
}

function demoApiResultScenario(): void {
    section("10.9 工程场景：API Result");
    note("C++ 对照", "Result 联合让调用方显式处理成功/失败，接近 expected 的使用直觉。");

    const results: readonly ApiResult<{ id: string }>[] = [
        { ok: true, data: { id: "u1" } },
        { ok: false, code: "not_found", message: "user not found" }
    ];

    showJson("API Result 输出", results.map((result) => renderApiResult(result)));
    note("输出解释", "if (result.ok) 后，成功分支能访问 data；失败分支能访问 code/message。");
}

// =============================================================================
// 10.10 本章复盘
// =============================================================================
//
// C++ 对照：
//   联合类型是 TS 日常类型建模的核心。
//   学会收窄后，才能安全地访问每个分支独有字段。

function demoChapterReview(): void {
    section("10.10 本章复盘");
    note("C++ 对照", "把联合类型理解成需要先判断 tag 或形状，再安全访问分支字段。");

    const review = [
        "联合类型表示值可能是多个候选类型之一",
        "访问联合成员独有字段前必须先收窄",
        "typeof 适合基础值，instanceof 适合运行时 class 实例",
        "in 适合对象属性存在性检查，=== 适合字面量联合",
        "判别联合用共同 kind 字段驱动 switch",
        "never 可用于穷尽性检查",
        "unknown 是安全外部输入边界，any 会关闭类型保护",
        "类型谓词必须真实检查运行时形状"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 10. */
export function runChapter(): void {
    demoUnionBasics();
    demoTypeofNarrowing();
    demoInstanceofNarrowing();
    demoInNarrowing();
    demoEqualityNarrowing();
    demoDiscriminatedUnion();
    demoUnknownVsAny();
    demoTypePredicate();
    demoApiResultScenario();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
