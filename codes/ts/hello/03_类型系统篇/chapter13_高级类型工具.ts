// =============================================================================
// 第 13 章：高级类型工具
// =============================================================================
//
// 【学习目标】
//   1. 掌握映射类型、条件类型、infer 和模板字面量类型
//   2. 熟悉常用内置工具类型的工程用途
//   3. 理解递归类型、asserts 断言函数和运行时边界
//
// 【运行方式】
//   npm run build
//   node dist/03_类型系统篇/chapter13_高级类型工具.js
//   或 npm run chapter -- 13（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type Mutable<T> = {
    -readonly [K in keyof T]: T[K];
};

type Flags<T extends string> = {
    [K in T as `can${Capitalize<K>}`]: boolean;
};

type ElementType<T> = T extends readonly (infer U)[] ? U : never;
type AwaitedValue<T> = T extends Promise<infer U> ? U : T;

type DeepReadonly<T> = T extends (...args: never[]) => unknown
    ? T
    : T extends readonly (infer U)[]
      ? readonly DeepReadonly<U>[]
      : T extends object
        ? { readonly [K in keyof T]: DeepReadonly<T[K]> }
        : T;

type ApiEventName<Resource extends string, Action extends string> = `${Lowercase<Resource>}:${Lowercase<Action>}`;

interface UserRecord {
    readonly id: string;
    name: string;
    email?: string;
    active: boolean;
    tags: readonly string[];
}

type UserPreview = Pick<UserRecord, "id" | "name" | "active">;
type UserPatch = Partial<Pick<UserRecord, "name" | "email" | "active">>;
type UserContact = Required<Pick<UserRecord, "email">>;
type VisibleTag = Exclude<ElementType<UserRecord["tags"]>, "internal">;
type NullableUser = UserRecord | null | undefined;
type KnownUser = NonNullable<NullableUser>;
type AsyncUser = Awaited<Promise<UserRecord>>;
type UserFactory = () => UserRecord;
type UserFactoryResult = ReturnType<UserFactory>;
type UserFactoryParameters = Parameters<(id: string, active?: boolean) => UserRecord>;

function mutableCopy<T extends object>(source: T): Mutable<T> {
    return { ...source } as Mutable<T>;
}

function makeFeatureFlags<T extends string>(features: readonly T[], enabled: boolean): Flags<T> {
    const entries = features.map((feature) => {
        const key = `can${feature[0]?.toUpperCase() ?? ""}${feature.slice(1)}`;
        return [key, enabled] as const;
    });
    return Object.fromEntries(entries) as Flags<T>;
}

function makeEventName<Resource extends string, Action extends string>(
    resource: Resource,
    action: Action
): ApiEventName<Resource, Action> {
    return `${resource.toLowerCase()}:${action.toLowerCase()}` as ApiEventName<Resource, Action>;
}

function assertNonEmptyArray<T>(values: readonly T[]): asserts values is readonly [T, ...T[]] {
    if (values.length === 0) {
        throw new Error("expected at least one item");
    }
}

function assertHasKey<T extends object, K extends PropertyKey>(
    value: T,
    key: K
): asserts value is T & Record<K, unknown> {
    if (!(key in value)) {
        throw new Error(`missing key: ${String(key)}`);
    }
}

function firstAfterAssert<T>(values: readonly T[]): T {
    assertNonEmptyArray(values);
    return values[0];
}

function stripEmail(user: UserRecord): UserPreview {
    return {
        id: user.id,
        name: user.name,
        active: user.active
    };
}

function applyPatch(user: UserRecord, patch: UserPatch): UserRecord {
    return { ...user, ...patch };
}

// =============================================================================
// 13.1 映射类型
// =============================================================================
//
// C++ 对照：
//   映射类型像在编译期遍历字段集合，为每个字段生成新规则。
//   它不会遍历运行时对象；运行时转换仍要自己写代码。

function demoMappedTypes(): void {
    section("13.1 映射类型");
    note("C++ 对照", "映射类型类似模板元编程里的字段变换，但结果只影响类型检查。");

    const readonlyUser: Readonly<UserRecord> = {
        id: "u1",
        name: "Ada",
        active: true,
        tags: ["platform", "compiler"]
    };
    const editable = mutableCopy(readonlyUser);
    editable.name = "Ada Lovelace";
    const flags = makeFeatureFlags(["read", "write", "delete"] as const, true);

    showJson("映射类型示例", {
        readonlyUser,
        editable,
        flags
    });
    note("输出解释", "Readonly/Mutable/Flags 都是类型层形状；真正的对象复制仍发生在普通 JS 代码里。");
}

// =============================================================================
// 13.2 条件类型与 infer
// =============================================================================
//
// C++ 对照：
//   条件类型像在编译期做分支，infer 用来从已有类型中拆出局部类型变量。
//   与模板偏特化不同，TS 条件类型最终不会生成运行时代码。

function demoConditionalTypesAndInfer(): void {
    section("13.2 条件类型与 infer");
    note("C++ 对照", "infer 是类型层提取工具，不是运行时反射。");

    const tagExample: VisibleTag = "compiler";
    const awaitedExample: AwaitedValue<Promise<string>> = "done";
    const user: KnownUser & AsyncUser & UserFactoryResult = {
        id: "u2",
        name: "Grace",
        active: true,
        tags: ["security"]
    };
    const factoryParameters: UserFactoryParameters = ["u3", true];

    showJson("条件类型与 infer", {
        tagExample,
        awaitedExample,
        user,
        factoryParameters
    });
    note("输出解释", "ElementType/AwaitedValue/ReturnType/Parameters 都在编译期拆类型，运行时只看到普通值。");
}

// =============================================================================
// 13.3 常用内置工具类型
// =============================================================================
//
// C++ 对照：
//   Pick/Omit/Partial/Required/Record/Exclude/Extract/NonNullable 等工具类型
//   适合描述 API 边界，减少手写重复结构。

function demoBuiltInUtilities(): void {
    section("13.3 常用内置工具类型");
    note("C++ 对照", "工具类型像预置的类型变换模板，目的是减少重复而不是制造谜题。");

    const user: UserRecord = {
        id: "u4",
        name: "Margaret",
        active: false,
        tags: ["research"]
    };
    const preview = stripEmail(user);
    const patched = applyPatch(user, { active: true, email: "m@example.test" });
    const contact: UserContact = { email: "m@example.test" };
    const statusCounts: Record<"active" | "inactive", number> = {
        active: 1,
        inactive: 0
    };

    showJson("内置工具类型", {
        preview,
        patched,
        contact,
        statusCounts
    });
    note("常见坑", "工具类型只描述形状，不会自动过滤字段；运行时仍要显式写 stripEmail 这类转换。");
}

// =============================================================================
// 13.4 模板字面量类型
// =============================================================================
//
// C++ 对照：
//   模板字面量类型像编译期字符串格式约束。
//   它适合事件名、路由名、配置键这类字符串协议。

function demoTemplateLiteralTypes(): void {
    section("13.4 模板字面量类型");
    note("C++ 对照", "模板字面量类型把字符串协议变成可检查的编译期约束。");

    const userCreated = makeEventName("User", "Created");
    const buildFinished = makeEventName("Build", "Finished");
    const eventHandlers: Record<ApiEventName<"user" | "build", "created" | "finished">, string> = {
        "user:created": "handleUserCreated",
        "user:finished": "handleUserFinished",
        "build:created": "handleBuildCreated",
        "build:finished": "handleBuildFinished"
    };

    showJson("模板字面量类型", {
        userCreated,
        buildFinished,
        registeredHandlers: Object.keys(eventHandlers)
    });
    note("输出解释", "事件名运行时仍是字符串；类型层只是限制字符串必须符合约定格式。");
}

// =============================================================================
// 13.5 递归类型
// =============================================================================
//
// C++ 对照：
//   递归类型像递归模板，但 TS 编译器需要在可读性和复杂度之间取平衡。
//   工程代码里应优先写清楚边界，避免过深递归类型拖慢编译。

function demoRecursiveTypes(): void {
    section("13.5 递归类型");
    note("C++ 对照", "DeepReadonly<T> 会递归处理对象和数组字段，但不会冻结运行时对象。");

    const snapshot: DeepReadonly<{
        user: UserRecord;
        audit: { createdAt: string; tags: string[] };
    }> = {
        user: {
            id: "u5",
            name: "Linus",
            active: true,
            tags: ["kernel"]
        },
        audit: {
            createdAt: "2026-05-18T00:00:00.000Z",
            tags: ["demo"]
        }
    };

    showJson("递归类型示例", snapshot);
    note("常见坑", "DeepReadonly 只阻止编译期写法；如果需要运行时不可变，还要配合 Object.freeze 或不可变数据策略。");
}

// =============================================================================
// 13.6 asserts 断言函数
// =============================================================================
//
// C++ 对照：
//   asserts 把运行时检查结果反馈给编译器。
//   它比直接 as 更适合封装输入验证，因为失败路径会显式抛错。

function demoAssertsFunctions(): void {
    section("13.6 asserts 断言函数");
    note("C++ 对照", "asserts 更像带类型效果的运行时断言，而不是简单类型转换。");

    const names = ["Ada", "Grace"];
    const firstName = firstAfterAssert(names);
    const dynamicConfig: Record<string, unknown> = {
        strict: true,
        target: "ES2023"
    };
    assertHasKey(dynamicConfig, "target");

    showJson("asserts 示例", {
        firstName,
        target: dynamicConfig.target
    });
    note("输出解释", "assertNonEmptyArray 让编译器知道数组非空；assertHasKey 让动态键访问更明确。");
}

// =============================================================================
// 13.7 本章复盘
// =============================================================================
//
// C++ 对照：
//   高级类型像类型层工具箱。它能减少重复、表达边界，但不应取代清晰的运行时代码。

function demoChapterReview(): void {
    section("13.7 本章复盘");
    note("C++ 对照", "TS 高级类型是编译期约束，不是代码生成器。");

    const summary = [
        "映射类型遍历字段集合并生成新形状",
        "条件类型和 infer 用于类型层分支与提取",
        "内置工具类型减少 API 边界的重复定义",
        "模板字面量类型适合约束字符串协议",
        "递归类型要控制复杂度",
        "asserts 断言把运行时检查反馈给类型系统",
        "高级类型服务于可读边界，不应为了炫技而压缩业务语义"
    ];

    showJson("关键结论", summary);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 13. */
export function runChapter(): void {
    demoMappedTypes();
    demoConditionalTypesAndInfer();
    demoBuiltInUtilities();
    demoTemplateLiteralTypes();
    demoRecursiveTypes();
    demoAssertsFunctions();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
