// =============================================================================
// 第 3 章：控制流程
// =============================================================================
//
// 【学习目标】
//   1. 掌握 if-else、三元表达式、switch 的日常写法
//   2. 区分 for、while、do-while、for-of、for-in 的适用场景
//   3. 理解 break、continue、标签语句、短路求值、空值合并和可选链
//   4. 建立 switch 穷尽性检查和工程分支建模的意识
//
// 【运行方式】
//   npm run build
//   node dist/01_基础篇/chapter03_控制流程.js
//   或 npm run chapter -- 3（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type JobState = "queued" | "running" | "done" | "failed";

type AccessRole = "guest" | "member" | "admin";

type AccessRequest = {
    readonly role: AccessRole;
    readonly resourceOwnerId?: string;
    readonly currentUserId?: string;
    readonly featureEnabled?: boolean;
};

type AuditRecord = {
    readonly id: string;
    readonly status: "ok" | "skip" | "error";
    readonly message: string;
};

// =============================================================================
// 3.1 if-else：最直接的业务分支
// =============================================================================
//
// C++ 对照：
//   if-else 的基本结构和 C++ 接近，但条件表达式会按 JS truthy/falsy 规则解释。
//   为了避免误判，业务代码应尽量写出明确比较，而不是依赖隐式真假。
//
// 常见坑：
//   `if (count)` 会把 0 当成 false。
//   如果 0 是合法值，应写 `count === 0` 或 `count !== undefined` 这样的明确条件。

/** Classify a numeric score into a stable textual grade. */
export function classifyScore(score: number): "invalid" | "low" | "medium" | "high" {
    if (!Number.isFinite(score) || score < 0 || score > 100) {
        return "invalid";
    }
    if (score >= 85) {
        return "high";
    }
    if (score >= 60) {
        return "medium";
    }
    return "low";
}

function demoIfElse(): void {
    section("3.1 if-else：明确表达业务条件");
    note("C++ 对照", "语法接近 C++，但条件中的值会先按 JS 规则转成 truthy/falsy。");

    const scores = [96, 72, 41, -1, Number.NaN];
    const results = scores.map((score) => ({
        score: String(score),
        grade: classifyScore(score)
    }));

    showJson("成绩分类", results);
    note("输出解释", "NaN 和负数都进入 invalid 分支，因为它们不是合法业务分数。");
}

// =============================================================================
// 3.2 三元表达式：短分支，不要塞复杂业务
// =============================================================================
//
// C++ 对照：
//   条件运算符 `cond ? a : b` 和 C++ 形式接近。
//   但 TS/JS 项目中更强调可读性：嵌套三元很容易让线性读者迷路。
//
// 经验法则：
//   只在两个结果都很短、含义很直观时使用三元。
//   复杂分支使用 if-else，并把条件命名。

function demoTernary(): void {
    section("3.2 三元表达式：适合短小选择");
    note("C++ 对照", "三元适合表达简单选择；复杂业务判断仍应拆成命名变量或 if-else。");

    const counts = [0, 1, 5];
    const labels = counts.map((count) => ({
        count,
        label: count === 1 ? "1 item" : `${count} items`
    }));

    showJson("列表数量标签", labels);
    note("输出解释", "这里的三元只处理单复数文本，两个结果都短，因此可读性还可以。");
}

// =============================================================================
// 3.3 switch 与穷尽性检查
// =============================================================================
//
// C++ 对照：
//   JS switch 也会发生 fall-through，因此每个 case 通常要 return 或 break。
//   TS 可以借助 never 做穷尽性检查，帮助我们发现遗漏状态。
//
// 线性阅读提示：
//   `never` 会在类型系统篇深入。
//   现在先把它理解成“理论上不应该到达这里”的类型。

/** Convert a job state into a user-facing message with exhaustive checking. */
export function describeJobState(state: JobState): string {
    switch (state) {
        case "queued":
            return "任务已排队，等待执行";
        case "running":
            return "任务正在执行";
        case "done":
            return "任务已完成";
        case "failed":
            return "任务失败，需要查看日志";
        default: {
            const exhaustive: never = state;
            return exhaustive;
        }
    }
}

function demoSwitch(): void {
    section("3.3 switch：用穷尽性检查守住状态边界");
    note("C++ 对照", "和 C++ 一样要警惕 fall-through；TS 还能用 never 辅助检查状态遗漏。");

    const states: readonly JobState[] = ["queued", "running", "done", "failed"];
    const messages = states.map((state) => ({
        state,
        message: describeJobState(state)
    }));

    showJson("任务状态消息", messages);
    note("输出解释", "四个状态都被显式处理；未来新增状态时，default 中的 never 会提示遗漏。");
}

// =============================================================================
// 3.4 for、while、do-while
// =============================================================================
//
// C++ 对照：
//   传统 for/while/do-while 的控制结构很接近 C++。
//   主要差异来自 JS 数组、闭包和异步模型；本章先只看同步循环。
//
// 常见坑：
//   循环里用 `var` 声明计数器会带来函数级作用域问题。
//   新代码中循环计数器使用 `let`。

function demoClassicLoops(): void {
    section("3.4 for、while、do-while：三种基础循环");
    note("C++ 对照", "循环结构熟悉，但计数器用 let，避免 var 的函数级作用域遗留问题。");

    const forValues: number[] = [];
    for (let index = 0; index < 4; index += 1) {
        forValues.push(index * 2);
    }

    const whileValues: number[] = [];
    let cursor = 3;
    while (cursor > 0) {
        whileValues.push(cursor);
        cursor -= 1;
    }

    const doWhileValues: number[] = [];
    let attempt = 0;
    do {
        attempt += 1;
        doWhileValues.push(attempt);
    } while (attempt < 2);

    showJson("循环输出", { forValues, whileValues, doWhileValues });
    note("输出解释", "do-while 至少执行一次；这适合表达“先尝试，再判断是否继续”。");
}

// =============================================================================
// 3.5 for-of vs for-in
// =============================================================================
//
// C++ 对照：
//   `for-of` 更接近范围 for：遍历可迭代对象的值。
//   `for-in` 遍历对象的可枚举 key；用在数组上通常不是你想要的值遍历。
//
// 常见坑：
//   对数组写 `for (const x in arr)` 得到的是索引字符串，不是元素值。
//   遍历数组优先使用 for-of、map、filter、reduce 等。

function demoForOfVsForIn(): void {
    section("3.5 for-of vs for-in：值遍历和键遍历");
    note("C++ 对照", "for-of 像范围 for 遍历值；for-in 更像遍历对象 key。");

    const tools = ["node", "tsc", "npm"];
    const objectConfig = {
        target: "ES2023",
        module: "NodeNext",
        strict: "true"
    };

    const values: string[] = [];
    for (const tool of tools) {
        values.push(tool);
    }

    const arrayKeys: string[] = [];
    for (const key in tools) {
        arrayKeys.push(key);
    }

    const objectPairs: string[] = [];
    for (const key in objectConfig) {
        const value = objectConfig[key as keyof typeof objectConfig];
        objectPairs.push(`${key}=${value}`);
    }

    showJson("for-of 与 for-in 对比", { values, arrayKeys, objectPairs });
    note("输出解释", "arrayKeys 是字符串索引；这就是数组上误用 for-in 的常见问题。");
}

// =============================================================================
// 3.6 break、continue 和标签语句
// =============================================================================
//
// C++ 对照：
//   break/continue 的基础语义类似 C++。
//   JS 有标签语句，可让 break/continue 指向外层循环，但没有常规 goto。
//
// 工程建议：
//   标签语句要少用。
//   如果逻辑复杂到需要频繁跨层跳转，通常应该拆函数或重构数据处理。

function findFirstError(records: readonly AuditRecord[]): AuditRecord | undefined {
    for (const record of records) {
        if (record.status === "skip") {
            continue;
        }
        if (record.status === "error") {
            return record;
        }
    }
    return undefined;
}

function demoBreakContinueAndLabels(): void {
    section("3.6 break、continue 和标签语句");
    note("C++ 对照", "JS 没有常规 goto；标签主要服务 break/continue，代码里应谨慎使用。");

    const records: readonly AuditRecord[] = [
        { id: "a1", status: "skip", message: "disabled rule" },
        { id: "a2", status: "ok", message: "passed" },
        { id: "a3", status: "error", message: "missing owner" }
    ];

    const firstError = findFirstError(records);

    const matrix = [
        [1, 2, 3],
        [4, 5, 6],
        [7, 8, 9]
    ];
    let found = "not-found";

    outer: for (const row of matrix) {
        for (const value of row) {
            if (value === 5) {
                found = `found:${value}`;
                break outer;
            }
        }
    }

    showJson("控制跳转示例", { firstError, labelledBreakResult: found });
    note("输出解释", "continue 跳过 skip 记录；标签 break 直接跳出两层循环。");
}

// =============================================================================
// 3.7 短路求值：&&、||、??
// =============================================================================
//
// C++ 对照：
//   `&&` 和 `||` 同样短路，但 JS 返回的是参与运算的原始值，不一定是 boolean。
//   `??` 只处理 null/undefined，是配置默认值更可靠的工具。
//
// 常见坑：
//   `port || 3000` 会把合法端口 0 替换成 3000。
//   如果 0 是合法值，应使用 `port ?? 3000`。

function demoShortCircuit(): void {
    section("3.7 短路求值：&&、||、??");
    note("C++ 对照", "JS 的 &&/|| 会返回原值；不要假设结果一定是 bool。");

    const explicitZero = 0;
    const missing: number | undefined = undefined;
    const emptyName = "";

    const summary = {
        badDefaultForZero: explicitZero || 3000,
        goodDefaultForZero: explicitZero ?? 3000,
        missingDefault: missing ?? 3000,
        emptyNameWithOr: emptyName || "anonymous",
        emptyNameWithNullish: emptyName ?? "anonymous"
    };

    showJson("默认值差异", summary);
    note("输出解释", "?? 保留 0 和空字符串；|| 会把所有 falsy 值都替换掉。");
}

// =============================================================================
// 3.8 可选链 ?.
// =============================================================================
//
// C++ 对照：
//   可选链像是对“可能为空”的访问做短路保护，但它只检查 null 和 undefined。
//   它不是 C++ 指针安全模型，也不会验证对象结构是否完整。
//
// 类型边界：
//   外部 JSON 仍需要 runtime validation。
//   可选链只是避免访问空值时报错，不会证明数据可信。

type UserProfile = {
    readonly id: string;
    readonly contact?: {
        readonly email?: string;
        readonly phone?: string;
    };
};

function getContactLabel(profile: UserProfile): string {
    return profile.contact?.email ?? profile.contact?.phone ?? "no-contact";
}

function demoOptionalChaining(): void {
    section("3.8 可选链 ?.：安全读取可缺失属性");
    note("C++ 对照", "可选链只是在访问 null/undefined 时短路，不是完整的数据校验。");

    const profiles: readonly UserProfile[] = [
        { id: "u1", contact: { email: "ada@example.com" } },
        { id: "u2", contact: { phone: "10086" } },
        { id: "u3" }
    ];

    const labels = profiles.map((profile) => ({
        id: profile.id,
        contact: getContactLabel(profile)
    }));

    showJson("联系方式读取", labels);
    note("输出解释", "u3 没有 contact，可选链让读取安全地落到默认值。");
}

// =============================================================================
// 3.9 工程场景：访问权限判断
// =============================================================================
//
// C++ 对照：
//   权限判断和 C++ 服务端代码一样，要把条件顺序写清楚。
//   TS 的联合字面量能让角色枚举更精确，但运行时请求仍需验证。
//
// 真实场景：
//   admin 可以访问所有资源。
//   member 只能在功能开启且自己是资源 owner 时访问。
//   guest 不能访问受保护资源。

/** Decide whether a request may access a protected resource. */
export function canAccessResource(request: AccessRequest): boolean {
    if (request.role === "admin") {
        return true;
    }
    if (request.role === "guest") {
        return false;
    }
    if (request.featureEnabled !== true) {
        return false;
    }
    return request.resourceOwnerId !== undefined && request.resourceOwnerId === request.currentUserId;
}

function demoAccessPolicy(): void {
    section("3.9 工程场景：权限判断");
    note("C++ 对照", "把业务规则拆成早返回，比把所有条件塞进一个表达式更容易审查。");

    const requests: readonly AccessRequest[] = [
        { role: "admin", resourceOwnerId: "u1", currentUserId: "u9", featureEnabled: false },
        { role: "guest", resourceOwnerId: "u1", currentUserId: "u1", featureEnabled: true },
        { role: "member", resourceOwnerId: "u1", currentUserId: "u1", featureEnabled: true },
        { role: "member", resourceOwnerId: "u1", currentUserId: "u2", featureEnabled: true },
        { role: "member", resourceOwnerId: "u1", currentUserId: "u1" }
    ];

    const decisions = requests.map((request) => ({
        request,
        allowed: canAccessResource(request)
    }));

    showJson("权限判断结果", decisions);
    note("输出解释", "member 的访问需要功能开启且 owner 匹配；缺失 featureEnabled 会被当成未开启。");
}

// =============================================================================
// 3.10 本章复盘
// =============================================================================
//
// C++ 对照：
//   控制流程表面熟悉，但 JS 的 truthy/falsy、可选链、空值合并和 for-in 语义必须重新建立。

function demoChapterReview(): void {
    section("3.10 本章复盘");
    note("C++ 对照", "语法相似不代表语义完全相同；默认值和遍历方式尤其容易误判。");

    const review = [
        "if 条件会按 truthy/falsy 转换，业务条件尽量写明确比较",
        "switch 每个 case 应 return 或 break，避免 fall-through",
        "for-of 遍历值，for-in 遍历 key",
        "标签语句只服务 break/continue，新代码应少用",
        "|| 会替换所有 falsy 值，?? 只处理 null/undefined",
        "可选链只避免空值访问报错，不替代 runtime validation",
        "权限判断这类业务分支应优先写成可审查的早返回"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 3. */
export function runChapter(): void {
    demoIfElse();
    demoTernary();
    demoSwitch();
    demoClassicLoops();
    demoForOfVsForIn();
    demoBreakContinueAndLabels();
    demoShortCircuit();
    demoOptionalChaining();
    demoAccessPolicy();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
