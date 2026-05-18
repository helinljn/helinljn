// =============================================================================
// 第 6 章：对象与引用
// =============================================================================
//
// 【学习目标】
//   1. 理解对象字面量、属性访问和动态属性的运行时语义
//   2. 掌握对象赋值共享引用这一 C++ 背景下的核心差异
//   3. 区分展开运算符、Object.assign、Object.freeze、Object.seal 的边界
//   4. 熟悉可选链、逻辑赋值和 Object.keys/values/entries/fromEntries
//
// 【运行方式】
//   npm run build
//   node dist/02_JS运行时篇/chapter06_对象与引用.js
//   或 npm run chapter -- 6（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type Point = {
    x: number;
    y: number;
};

type UserProfile = {
    readonly id: string;
    name: string;
    contact?: {
        email?: string;
        phone?: string;
    };
    preferences: {
        theme?: "light" | "dark";
        locale?: string;
    };
};

type ConfigRecord = Record<string, string | number | boolean>;

// =============================================================================
// 6.1 对象字面量和属性访问
// =============================================================================
//
// C++ 对照：
//   JS 对象字面量像一个运行时属性表，不是固定内存布局的 struct。
//   TypeScript 能约束对象“应该长什么样”，但编译后运行时仍是普通 JS 对象。
//
// 常见坑：
//   用点语法访问的属性名必须是静态标识符。
//   动态字段名要使用方括号语法，例如 obj[key]。

function demoObjectLiteralAndPropertyAccess(): void {
    section("6.1 对象字面量和属性访问");
    note("C++ 对照", "对象字面量是运行时属性集合；TS 类型只是编译期形状约束。");

    const fieldName = "name";
    const profile: UserProfile = {
        id: "u1",
        name: "Ada",
        preferences: {
            theme: "dark",
            locale: "zh-CN"
        }
    };

    const output = {
        dotAccess: profile.name,
        bracketAccess: profile[fieldName],
        dynamicKey: fieldName,
        theme: profile.preferences.theme
    };

    showJson("属性访问示例", output);
    note("输出解释", "profile.name 是静态访问，profile[fieldName] 会在运行时用变量值查找属性。");
}

// =============================================================================
// 6.2 引用语义：对象赋值不是结构体拷贝
// =============================================================================
//
// C++ 对照：
//   C++ 中 `Point b = a` 通常产生一个新对象副本。
//   JS 中 `const b = a` 让两个变量指向同一个对象。
//
// 常见坑：
//   修改别名会影响原对象。
//   这类 bug 在配置对象、请求上下文、缓存记录中非常常见。

/** Move a point in place to demonstrate JavaScript object reference semantics. */
export function movePointInPlace(point: Point, dx: number, dy: number): Point {
    point.x += dx;
    point.y += dy;
    return point;
}

function demoReferenceSemantics(): void {
    section("6.2 引用语义：对象赋值不是复制");
    note("C++ 对照", "对象赋值复制的是引用，不是像普通 struct 一样复制字段值。");

    const original: Point = { x: 1, y: 2 };
    const alias = original;
    const moved = movePointInPlace(alias, 10, 20);

    showJson("引用共享结果", {
        original,
        alias,
        moved,
        originalAndAliasAreSame: original === alias,
        aliasAndMovedAreSame: alias === moved
    });
    note("输出解释", "三个变量都指向同一个对象；通过 alias 修改后 original 也变化。");
}

// =============================================================================
// 6.3 展开运算符：浅拷贝和浅合并
// =============================================================================
//
// C++ 对照：
//   展开 `{ ...obj }` 不是深拷贝。
//   它只复制第一层可枚举自有属性；嵌套对象仍共享引用。
//
// 工程建议：
//   用展开表达“创建一个新顶层对象”很常见。
//   但更新嵌套字段时，要显式拷贝嵌套对象。

function demoSpreadShallowCopy(): void {
    section("6.3 展开运算符：浅拷贝");
    note("C++ 对照", "展开不是深拷贝；嵌套对象仍可能共享引用。");

    const profile: UserProfile = {
        id: "u2",
        name: "Grace",
        contact: { email: "grace@example.com" },
        preferences: { theme: "light", locale: "en-US" }
    };

    const shallowCopy = { ...profile };
    shallowCopy.preferences.theme = "dark";

    const safeCopy = {
        ...profile,
        preferences: {
            ...profile.preferences,
            locale: "zh-CN"
        }
    };

    showJson("展开拷贝示例", {
        profileAfterShallowNestedChange: profile,
        shallowCopy,
        safeCopy,
        sameNestedPreferences: profile.preferences === shallowCopy.preferences,
        safeCopyHasNewPreferences: profile.preferences === safeCopy.preferences
    });
    note("输出解释", "shallowCopy 和 profile 共享 preferences；safeCopy 显式复制了嵌套对象。");
}

// =============================================================================
// 6.4 Object.assign：另一种浅合并
// =============================================================================
//
// C++ 对照：
//   Object.assign 更像“把源对象的可枚举自有属性写入目标对象”。
//   如果目标对象就是原对象，它会被直接修改。
//
// 常见坑：
//   `Object.assign(defaults, overrides)` 会修改 defaults。
//   需要新对象时应写 `Object.assign({}, defaults, overrides)` 或对象展开。

/** Merge configuration into a fresh object so defaults are not mutated. */
export function mergeConfig(defaults: ConfigRecord, overrides: ConfigRecord): ConfigRecord {
    return Object.assign({}, defaults, overrides);
}

function demoObjectAssign(): void {
    section("6.4 Object.assign：浅合并和目标对象");
    note("C++ 对照", "Object.assign 会写入第一个参数；这点像把目标容器作为输出参数。");

    const defaults: ConfigRecord = {
        host: "localhost",
        port: 3000,
        debug: false
    };
    const overrides: ConfigRecord = {
        port: 8080,
        debug: true
    };

    const merged = mergeConfig(defaults, overrides);
    const mutatedTarget = Object.assign({ ...defaults }, { host: "127.0.0.1" });

    showJson("Object.assign 示例", { defaults, overrides, merged, mutatedTarget });
    note("输出解释", "mergeConfig 使用空对象作为目标，因此 defaults 没有被覆盖。");
}

// =============================================================================
// 6.5 Object.freeze 与 Object.seal
// =============================================================================
//
// C++ 对照：
//   Object.freeze 是运行时浅冻结。
//   TS readonly 是类型层约束。
//   两者都不等于 C++ 深度 const。
//
// 常见坑：
//   freeze 只冻结第一层。
//   嵌套对象如果没有单独 freeze，仍可被修改。

function demoFreezeAndSeal(): void {
    section("6.5 Object.freeze 与 Object.seal");
    note("C++ 对照", "freeze/seal 是运行时对象操作，readonly 是 TS 类型操作；不要混为一谈。");

    const frozen = Object.freeze({
        id: "cfg1",
        nested: {
            enabled: true
        }
    });

    frozen.nested.enabled = false;

    const sealed: { host: string; port?: number } = { host: "localhost", port: 3000 };
    Object.seal(sealed);
    sealed.host = "127.0.0.1";
    const deletePortResult = Reflect.deleteProperty(sealed, "port");

    showJson("freeze/seal 示例", {
        frozen,
        frozenTopLevel: Object.isFrozen(frozen),
        nestedFrozen: Object.isFrozen(frozen.nested),
        sealed,
        deletePortResult,
        sealedTopLevel: Object.isSealed(sealed)
    });
    note("输出解释", "frozen 顶层被冻结但 nested 没被冻结；sealed 允许改已有属性，Reflect.deleteProperty 返回 false 表示删除失败。");
}

// =============================================================================
// 6.6 可选链深入
// =============================================================================
//
// C++ 对照：
//   可选链像“空值短路访问”，但只检查 null/undefined。
//   它不会证明对象来自可信来源，也不会验证字段类型。
//
// 类型边界：
//   从 JSON、HTTP、文件读来的对象仍应先按 unknown 接收并验证。
//   可选链只是读取时避免抛出 TypeError。

function getContact(profile: UserProfile): string {
    return profile.contact?.email ?? profile.contact?.phone ?? "no-contact";
}

function demoOptionalChainingDeepDive(): void {
    section("6.6 可选链深入");
    note("C++ 对照", "?. 只负责遇到 null/undefined 时停止访问，不替代数据验证。");

    const profiles: readonly UserProfile[] = [
        { id: "u1", name: "Ada", contact: { email: "ada@example.com" }, preferences: {} },
        { id: "u2", name: "Grace", contact: { phone: "10086" }, preferences: {} },
        { id: "u3", name: "Linus", preferences: {} }
    ];

    const contacts = profiles.map((profile) => ({
        id: profile.id,
        contact: getContact(profile),
        locale: profile.preferences.locale ?? "default-locale"
    }));

    showJson("可选链读取结果", contacts);
    note("输出解释", "没有 contact 或 locale 的对象不会抛错，而是落到 ?? 后面的默认值。");
}

// =============================================================================
// 6.7 逻辑赋值运算符
// =============================================================================
//
// C++ 对照：
//   `??=`、`||=`、`&&=` 是 JS 的复合赋值语法。
//   它们不是 C++ 操作符重载；只按 JS 的空值或真假规则执行。
//
// 常见坑：
//   `||=` 会把空字符串、0、false 都当成需要替换。
//   如果只想填补缺失值，优先使用 `??=`。

function demoLogicalAssignment(): void {
    section("6.7 逻辑赋值：??=、||=、&&=");
    note("C++ 对照", "这些运算符按 JS truthy/falsy 或 nullish 规则工作，和类型无关。");

    const config: { title?: string; retries: number; enabled: boolean } = {
        title: "",
        retries: 0,
        enabled: true
    };

    config.title ||= "Untitled";
    config.retries ??= 3;
    config.enabled &&= true;

    const configWithNullish: { title?: string; retries?: number } = {
        title: "",
        retries: 0
    };
    configWithNullish.title ??= "Untitled";
    configWithNullish.retries ??= 3;

    showJson("逻辑赋值对比", { config, configWithNullish });
    note("输出解释", "title ||= 替换了空字符串；title ??= 保留空字符串。retries 为 0 时 ??= 不会覆盖。");
}

// =============================================================================
// 6.8 Object.keys/values/entries/fromEntries
// =============================================================================
//
// C++ 对照：
//   这些 API 把对象当作 key/value 记录来处理。
//   它们只处理可枚举自有属性，不遍历原型链上的属性。
//
// 使用边界：
//   Object.keys 返回 string[]，不会自动保留精确 keyof 类型。
//   后续类型系统篇会讲如何在必要时更精确地建模。

/** Normalize config entries by dropping empty string values. */
export function normalizeConfigEntries(config: ConfigRecord): ConfigRecord {
    const entries = Object.entries(config).filter(([_key, value]) => value !== "");
    return Object.fromEntries(entries) as ConfigRecord;
}

function demoObjectIterationApis(): void {
    section("6.8 Object.keys/values/entries/fromEntries");
    note("C++ 对照", "这些 API 类似把记录对象转成键值序列再处理，但它们只看自有可枚举属性。");

    const config: ConfigRecord = {
        host: "localhost",
        port: 3000,
        token: "",
        debug: false
    };

    const normalized = normalizeConfigEntries(config);

    showJson("Object.* API 示例", {
        keys: Object.keys(config),
        values: Object.values(config),
        entries: Object.entries(config),
        normalized
    });
    note("输出解释", "fromEntries 把过滤后的键值对重新组装成对象，空 token 被丢弃。");
}

// =============================================================================
// 6.9 工程场景：安全更新用户偏好
// =============================================================================
//
// C++ 对照：
//   在 C++ 中你可能明确传值、引用或 const 引用。
//   JS/TS 中要通过写法表达“是否修改原对象”。
//
// 真实场景：
//   更新用户偏好时返回新对象，避免调用方持有的旧对象被意外修改。

/** Return a new user profile with merged preferences. */
export function updatePreferences(
    profile: UserProfile,
    preferences: Partial<UserProfile["preferences"]>
): UserProfile {
    return {
        ...profile,
        preferences: {
            ...profile.preferences,
            ...preferences
        }
    };
}

function demoSafeProfileUpdate(): void {
    section("6.9 工程场景：安全更新用户偏好");
    note("C++ 对照", "通过返回新对象表达非破坏性更新，避免隐式共享引用带来的副作用。");

    const before: UserProfile = {
        id: "u9",
        name: "Margaret",
        contact: { email: "margaret@example.com" },
        preferences: { theme: "light", locale: "en-US" }
    };
    const after = updatePreferences(before, { theme: "dark" });

    showJson("偏好更新结果", {
        before,
        after,
        sameTopLevelObject: before === after,
        samePreferencesObject: before.preferences === after.preferences,
        sameContactObject: before.contact === after.contact
    });
    note("输出解释", "顶层和 preferences 是新对象；contact 没有变化，所以仍共享引用。");
}

// =============================================================================
// 6.10 本章复盘
// =============================================================================
//
// C++ 对照：
//   本章核心是放下“对象赋值等于值复制”的直觉。
//   TS 类型能帮助表达对象形状，但对象行为仍由 JS 引用语义决定。

function demoChapterReview(): void {
    section("6.10 本章复盘");
    note("C++ 对照", "对象是引用语义；是否拷贝、是否冻结、是否修改原对象，都要在代码中明确表达。");

    const review = [
        "对象字面量是运行时属性集合，TS 类型只在编译期约束形状",
        "对象赋值复制引用，不复制字段值",
        "展开运算符和 Object.assign 都是浅拷贝/浅合并",
        "Object.assign 会修改第一个目标参数",
        "Object.freeze 是浅冻结，Object.seal 允许改已有属性但不允许增删属性",
        "可选链只处理 null/undefined 访问，不替代 runtime validation",
        "??= 和 ||= 的默认值语义不同",
        "Object.entries/fromEntries 适合把对象当作键值记录处理"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 6. */
export function runChapter(): void {
    demoObjectLiteralAndPropertyAccess();
    demoReferenceSemantics();
    demoSpreadShallowCopy();
    demoObjectAssign();
    demoFreezeAndSeal();
    demoOptionalChainingDeepDive();
    demoLogicalAssignment();
    demoObjectIterationApis();
    demoSafeProfileUpdate();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
