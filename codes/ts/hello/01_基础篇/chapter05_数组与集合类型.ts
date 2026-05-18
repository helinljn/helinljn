// =============================================================================
// 第 5 章：数组与集合类型
// =============================================================================
//
// 【学习目标】
//   1. 掌握数组创建、增删改查、可变操作和返回新数组操作的区别
//   2. 熟悉 map、filter、reduce、find、some、every、includes、flat、flatMap
//   3. 理解 sort 的原地排序行为，以及如何避免误改原数组
//   4. 掌握 Map、Set、WeakMap、WeakSet 的基本使用边界
//   5. 理解 readonly 数组和 as const 的类型约束
//
// 【运行方式】
//   npm run build
//   node dist/01_基础篇/chapter05_数组与集合类型.js
//   或 npm run chapter -- 5（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type User = {
    readonly id: string;
    readonly name: string;
    readonly department: "platform" | "product" | "security";
    readonly active: boolean;
    readonly tags: readonly string[];
};

type CartItem = {
    readonly sku: string;
    readonly price: number;
    readonly quantity: number;
};

type DepartmentSummary = {
    readonly department: User["department"];
    readonly count: number;
    readonly activeCount: number;
};

type MutableQueue<T> = {
    readonly before: readonly T[];
    readonly afterPush: readonly T[];
    readonly popped: T | undefined;
    readonly afterPop: readonly T[];
};

// =============================================================================
// 5.1 数组创建：字面量、Array.from、fill
// =============================================================================
//
// C++ 对照：
//   JS Array 更像动态数组和对象能力的结合体，不等同于 std::vector。
//   它可以增长、收缩，也能包含任意 JS 值；TS 类型负责约束你期望放入的元素类型。
//
// 常见坑：
//   `Array(3).fill({ count: 0 })` 会让三个位置共享同一个对象引用。
//   创建对象数组时优先用 Array.from 的回调为每个元素创建新对象。

function demoArrayCreation(): void {
    section("5.1 数组创建：字面量、Array.from、fill");
    note("C++ 对照", "Array 是动态集合，TS 的 number[] 只是编译期约束元素类型。");

    const literal = [1, 2, 3];
    const fromRange = Array.from({ length: 4 }, (_unused, index) => index * 10);
    const filledNumbers = Array<number>(3).fill(0);
    const safeObjects = Array.from({ length: 3 }, (_unused, index) => ({ index, count: 0 }));

    const shared = Array<{ count: number }>(3).fill({ count: 0 });
    shared[0]!.count = 99;

    showJson("数组创建示例", {
        literal,
        fromRange,
        filledNumbers,
        safeObjects,
        sharedObjectsAfterMutation: shared
    });
    note("输出解释", "shared 三个位置引用同一个对象；修改第一个位置会影响所有位置。");
}

// =============================================================================
// 5.2 push/pop/shift/unshift：可变队列操作
// =============================================================================
//
// C++ 对照：
//   push/pop 类似 vector 尾部操作。
//   shift/unshift 操作数组头部，可能移动大量元素，频繁使用时要评估性能。
//
// 工程建议：
//   教学中可以直接演示这些可变操作。
//   真实业务状态更新时，应明确是否允许修改原数组。

function simulateStack<T>(items: readonly T[], next: T): MutableQueue<T> {
    const queue = [...items];
    const before = [...queue];
    queue.push(next);
    const afterPush = [...queue];
    const popped = queue.pop();
    const afterPop = [...queue];
    return { before, afterPush, popped, afterPop };
}

function demoMutableQueueOperations(): void {
    section("5.2 push/pop/shift/unshift：可变操作");
    note("C++ 对照", "这些方法会修改原数组；这点接近容器成员函数，但 JS 更容易无意共享引用。");

    const stackResult = simulateStack(["parse", "typecheck"], "emit");
    const queue = ["first", "second"];
    queue.unshift("zeroth");
    const shifted = queue.shift();

    showJson("可变操作示例", {
        stackResult,
        queueAfterUnshiftAndShift: queue,
        shifted
    });
    note("输出解释", "push/pop 修改副本 queue；原始 readonly 输入 items 没有被修改。");
}

// =============================================================================
// 5.3 splice vs slice：修改原数组还是返回新数组
// =============================================================================
//
// C++ 对照：
//   `splice` 会修改原数组，类似在容器中 erase/insert。
//   `slice` 返回浅拷贝片段，不修改原数组。
//
// 常见坑：
//   方法名只差一个字母，但副作用完全不同。
//   代码 review 时要特别看清是 splice 还是 slice。

function demoSpliceAndSlice(): void {
    section("5.3 splice vs slice：副作用差一个字母");
    note("C++ 对照", "splice 改原数组，slice 返回新数组；副作用边界要在变量名里表达清楚。");

    const original = ["a", "b", "c", "d"];
    const sliced = original.slice(1, 3);
    const mutated = [...original];
    const removed = mutated.splice(1, 2, "x", "y");

    showJson("slice 与 splice 对比", {
        original,
        sliced,
        mutatedAfterSplice: mutated,
        removedBySplice: removed
    });
    note("输出解释", "original 没变；mutated 是副本，splice 在副本上移除 b/c 并插入 x/y。");
}

// =============================================================================
// 5.4 map/filter/reduce：集合转换三件套
// =============================================================================
//
// C++ 对照：
//   map/filter/reduce 接近 ranges/algorithm 风格的管道化处理。
//   JS 标准库把这些高阶函数放在 Array 原型上，日常业务代码非常常见。
//
// 线性阅读提示：
//   回调和闭包会在后续函数章节深入。
//   这里先关注输入数组、返回数组、累积值之间的关系。

/** Compute a cart total from immutable line items. */
export function cartTotal(items: readonly CartItem[]): number {
    return items.reduce((total, item) => total + item.price * item.quantity, 0);
}

function demoMapFilterReduce(): void {
    section("5.4 map/filter/reduce：转换、筛选、归约");
    note("C++ 对照", "可以把它们看成常用算法组合，但回调是 JS 函数值。");

    const users = sampleUsers();
    const activeNames = users
        .filter((user) => user.active)
        .map((user) => user.name);

    const cart: readonly CartItem[] = [
        { sku: "book", price: 1200, quantity: 2 },
        { sku: "pen", price: 300, quantity: 3 }
    ];

    showJson("map/filter/reduce 示例", {
        activeNames,
        totalCents: cartTotal(cart)
    });
    note("输出解释", "filter 保留 active 用户，map 取 name，reduce 把购物车行项目累加成总价。");
}

// =============================================================================
// 5.5 手动实现 map/filter/reduce 的简化版
// =============================================================================
//
// C++ 对照：
//   手写实现能帮助理解高阶函数和泛型容器算法的对应关系。
//   真实项目优先使用标准库方法，除非你正在封装特殊行为。

/** A small teaching implementation of Array.prototype.map. */
export function manualMap<T, U>(values: readonly T[], transform: (value: T) => U): U[] {
    const output: U[] = [];
    for (const value of values) {
        output.push(transform(value));
    }
    return output;
}

/** A small teaching implementation of Array.prototype.filter. */
export function manualFilter<T>(values: readonly T[], keep: (value: T) => boolean): T[] {
    const output: T[] = [];
    for (const value of values) {
        if (keep(value)) {
            output.push(value);
        }
    }
    return output;
}

/** A small teaching implementation of Array.prototype.reduce with an explicit initial value. */
export function manualReduce<T, U>(values: readonly T[], initial: U, combine: (total: U, value: T) => U): U {
    let total = initial;
    for (const value of values) {
        total = combine(total, value);
    }
    return total;
}

function demoManualHigherOrderFunctions(): void {
    section("5.5 手动实现 map/filter/reduce");
    note("C++ 对照", "这类似手写小型 algorithm，重点是理解回调如何参与遍历。");

    const values = [1, 2, 3, 4, 5];
    const doubled = manualMap(values, (value) => value * 2);
    const even = manualFilter(values, (value) => value % 2 === 0);
    const sum = manualReduce(values, 0, (total, value) => total + value);

    showJson("手写高阶函数结果", { values, doubled, even, sum });
    note("输出解释", "manualReduce 显式要求 initial，避免空数组归约时出现不稳定边界。");
}

// =============================================================================
// 5.6 find/findIndex/some/every/includes
// =============================================================================
//
// C++ 对照：
//   这些方法分别表达“找元素”“找位置”“是否存在”“是否全部满足”“是否包含值”。
//   比把所有逻辑都写成 for 循环更能表达意图。
//
// 常见坑：
//   find 可能返回 undefined。
//   在 strict 模式下，调用方必须处理找不到的分支。

function demoSearchMethods(): void {
    section("5.6 find/findIndex/some/every/includes：表达查找意图");
    note("C++ 对照", "这些方法类似标准算法，但返回值边界要按 JS/TS 规则处理。");

    const users = sampleUsers();
    const securityUser = users.find((user) => user.department === "security");
    const firstInactiveIndex = users.findIndex((user) => !user.active);
    const hasAdminTag = users.some((user) => user.tags.includes("admin"));
    const allHaveNames = users.every((user) => user.name.length > 0);
    const departments = users.map((user) => user.department);

    showJson("查找方法示例", {
        securityUser,
        firstInactiveIndex,
        hasAdminTag,
        allHaveNames,
        departmentsIncludePlatform: departments.includes("platform")
    });
    note("输出解释", "find 找不到会返回 undefined；findIndex 找不到会返回 -1。");
}

// =============================================================================
// 5.7 flat/flatMap：拍平嵌套数组
// =============================================================================
//
// C++ 对照：
//   flatMap 可以理解为先 map 成多个子序列，再拼接一层。
//   它常用于一对多转换，例如用户到 tag、订单到行项目。
//
// 使用边界：
//   flat 默认只拍平一层。
//   深层嵌套要显式传入深度，但过深嵌套通常也提示数据模型需要重看。

/** Return unique tags from users while preserving first-seen order. */
export function uniqueTags(users: readonly User[]): string[] {
    return [...new Set(users.flatMap((user) => user.tags))];
}

function demoFlatAndFlatMap(): void {
    section("5.7 flat/flatMap：一对多转换");
    note("C++ 对照", "flatMap 类似 ranges 中 transform 后再 join 一层的处理。");

    const nested = [[1, 2], [3, 4], [5]];
    const flattened = nested.flat();
    const users = sampleUsers();
    const userTags = users.flatMap((user) => user.tags.map((tag) => `${user.id}:${tag}`));

    showJson("flat/flatMap 示例", {
        nested,
        flattened,
        userTags,
        uniqueTags: uniqueTags(users)
    });
    note("输出解释", "flat 拍平数字数组一层；flatMap 把每个用户展开成多条 tag 记录。");
}

// =============================================================================
// 5.8 sort：原地排序和比较函数
// =============================================================================
//
// C++ 对照：
//   sort 会修改原数组，类似对容器原地排序。
//   JS 默认 sort 按字符串字典序比较，不适合直接排数字。
//
// 常见坑：
//   `[10, 2, 1].sort()` 得到 `[1, 10, 2]`。
//   数字排序必须传入 `(a, b) => a - b`。

/** Return a sorted copy without mutating the caller's array. */
export function sortedNumbers(values: readonly number[]): number[] {
    return [...values].sort((left, right) => left - right);
}

function demoSort(): void {
    section("5.8 sort：原地排序和比较函数");
    note("C++ 对照", "sort 会修改接收者；需要保留原数组时先拷贝。");

    const numbers = [10, 2, 1, 30];
    const defaultSorted = [...numbers].sort();
    const numericSorted = sortedNumbers(numbers);

    const usersByName = [...sampleUsers()].sort((left, right) => left.name.localeCompare(right.name));

    showJson("排序示例", {
        original: numbers,
        defaultSorted,
        numericSorted,
        usersByName: usersByName.map((user) => user.name)
    });
    note("输出解释", "默认排序按字符串比较；numericSorted 使用数字比较函数且没有修改 original。");
}

// =============================================================================
// 5.9 Map vs Object
// =============================================================================
//
// C++ 对照：
//   Map 更接近专门的键值容器，Object 更像普通记录对象。
//   Map 的键可以是对象引用，Object 普通属性键主要是 string/symbol。
//
// 工程建议：
//   固定字段的数据模型用 object/interface。
//   动态键集合、需要对象键或需要保留插入顺序时用 Map。

/** Group users by department using Map to make dynamic keys explicit. */
export function groupUsersByDepartment(users: readonly User[]): Map<User["department"], User[]> {
    const grouped = new Map<User["department"], User[]>();
    for (const user of users) {
        const bucket = grouped.get(user.department) ?? [];
        bucket.push(user);
        grouped.set(user.department, bucket);
    }
    return grouped;
}

function demoMapVsObject(): void {
    section("5.9 Map vs Object：动态键和值容器");
    note("C++ 对照", "Map 更像键值容器；Object 更适合固定字段记录。");

    const users = sampleUsers();
    const grouped = groupUsersByDepartment(users);
    const summaries: DepartmentSummary[] = [];
    for (const [department, departmentUsers] of grouped) {
        summaries.push({
            department,
            count: departmentUsers.length,
            activeCount: departmentUsers.filter((user) => user.active).length
        });
    }

    const objectRecord: Record<string, number> = {
        platform: 2,
        product: 1
    };

    showJson("Map 与 Object 示例", {
        mapEntries: Array.from(grouped, ([department, departmentUsers]) => [
            department,
            departmentUsers.map((user) => user.id)
        ]),
        summaries,
        objectRecord
    });
    note("输出解释", "Map 需要转换成数组才能稳定 JSON 展示；Object 适合固定字段或普通记录。");
}

// =============================================================================
// 5.10 Set：去重和成员关系
// =============================================================================
//
// C++ 对照：
//   Set 表示唯一值集合。
//   JS Set 使用 SameValueZero 比较，基础值按值比较，对象按引用身份比较。
//
// 常见坑：
//   两个内容相同的对象字面量在 Set 中是不同元素。
//   因为它们是两个不同对象引用。

function demoSet(): void {
    section("5.10 Set：去重和成员关系");
    note("C++ 对照", "Set 适合表达唯一集合，但对象元素按引用比较。");

    const rawTags = ["ts", "node", "ts", "cli", "node"];
    const unique = [...new Set(rawTags)];

    const firstObject = { id: "u1" };
    const secondObject = { id: "u1" };
    const objectSet = new Set([firstObject, secondObject]);

    showJson("Set 示例", {
        rawTags,
        unique,
        hasNode: new Set(rawTags).has("node"),
        objectSetSize: objectSet.size,
        explanation: "两个 { id: 'u1' } 是不同对象引用，因此 Set size 是 2"
    });
    note("输出解释", "Set 对字符串去重直观；对对象去重通常需要先提取稳定 key。");
}

// =============================================================================
// 5.11 WeakMap/WeakSet：弱引用集合边界
// =============================================================================
//
// C++ 对照：
//   WeakMap/WeakSet 和 C++ weak_ptr 不完全等价。
//   它们的键必须是对象，且不会阻止对象被垃圾回收。
//
// 使用边界：
//   WeakMap/WeakSet 不可迭代，也没有 size。
//   它们适合给对象挂元数据、缓存私有状态，而不是做可枚举集合。

function demoWeakCollections(): void {
    section("5.11 WeakMap/WeakSet：对象元数据缓存");
    note("C++ 对照", "WeakMap 不等同于 weak_ptr；重点是弱持有对象键，不阻止垃圾回收。");

    const userObject = { id: "u1", name: "Ada" };
    const metadata = new WeakMap<object, { lastSeen: string }>();
    const visited = new WeakSet<object>();

    metadata.set(userObject, { lastSeen: "2026-05-18T00:00:00.000Z" });
    visited.add(userObject);

    showJson("WeakMap/WeakSet 示例", {
        hasMetadata: metadata.has(userObject),
        metadata: metadata.get(userObject),
        visited: visited.has(userObject),
        iterable: false,
        sizeAvailable: false
    });
    note("输出解释", "WeakMap/WeakSet 不能枚举内容，因此这里只展示 has/get 结果。");
}

// =============================================================================
// 5.12 readonly 数组与 as const
// =============================================================================
//
// C++ 对照：
//   readonly 数组是 TS 类型层约束，防止调用 push/splice 这类可变方法。
//   它不会把运行时数组冻结。
//
// 常见坑：
//   `readonly string[]` 不能阻止其他别名在运行时修改同一个数组。
//   若要运行时冻结，需要 Object.freeze 或不可变数据策略。

/** Accept a readonly array and return a formatted summary without mutation. */
export function summarizeReadonlyNames(names: readonly string[]): string {
    return names.join(", ");
}

function demoReadonlyArrays(): void {
    section("5.12 readonly 数组与 as const");
    note("C++ 对照", "readonly 是类型层约束，不是运行时深度 const。");

    const names: readonly string[] = ["Ada", "Grace", "Linus"];
    const tuple = ["build", "test", "chapter"] as const;
    const mutableAlias = ["alpha", "beta"];
    const readonlyView: readonly string[] = mutableAlias;
    mutableAlias.push("gamma");

    showJson("readonly 与 as const 示例", {
        namesSummary: summarizeReadonlyNames(names),
        tuple,
        readonlyViewAfterMutableAliasPush: readonlyView
    });
    note("输出解释", "readonlyView 不能通过自己 push，但原 mutableAlias 仍可修改同一个运行时数组。");
}

// =============================================================================
// 5.13 工程场景：用户列表统计
// =============================================================================
//
// C++ 对照：
//   真实服务端代码经常从数组记录转换为索引、汇总和唯一集合。
//   TS 类型能让字段和返回结构清楚，但运行时仍是普通 JS 数组和 Map。
//
// 真实场景：
//   从用户列表得到部门汇总、活跃用户、唯一标签和按 id 查找表。

/** Build a stable summary from a user list. */
export function summarizeUsers(users: readonly User[]): {
    readonly activeNames: readonly string[];
    readonly summaries: readonly DepartmentSummary[];
    readonly tags: readonly string[];
    readonly ids: readonly string[];
} {
    const activeNames = users.filter((user) => user.active).map((user) => user.name);
    const grouped = groupUsersByDepartment(users);
    const summaries = Array.from(grouped, ([department, departmentUsers]) => ({
        department,
        count: departmentUsers.length,
        activeCount: departmentUsers.filter((user) => user.active).length
    }));
    const ids = users.map((user) => user.id);

    return {
        activeNames,
        summaries,
        tags: uniqueTags(users),
        ids
    };
}

function demoUserSummaryScenario(): void {
    section("5.13 工程场景：用户列表统计");
    note("C++ 对照", "数组管道适合表达数据转换；Map/Set 适合表达索引和唯一集合。");

    const users = sampleUsers();
    const byId = new Map(users.map((user) => [user.id, user] as const));
    const summary = summarizeUsers(users);

    showJson("用户统计结果", {
        summary,
        lookupU2: byId.get("u2")
    });
    note("输出解释", "数组生成汇总，Map 提供按 id 查找；两者承担不同集合职责。");
}

// =============================================================================
// 5.14 样例数据和本章复盘
// =============================================================================
//
// C++ 对照：
//   示例数据固定，输出可重复。
//   这符合教程的副作用边界：没有随机数、当前时间、文件系统或网络依赖。

function sampleUsers(): readonly User[] {
    return [
        {
            id: "u1",
            name: "Ada",
            department: "platform",
            active: true,
            tags: ["admin", "ts"]
        },
        {
            id: "u2",
            name: "Grace",
            department: "security",
            active: false,
            tags: ["security", "node"]
        },
        {
            id: "u3",
            name: "Linus",
            department: "platform",
            active: true,
            tags: ["node", "cli"]
        },
        {
            id: "u4",
            name: "Margaret",
            department: "product",
            active: true,
            tags: ["ts", "product"]
        }
    ];
}

function demoChapterReview(): void {
    section("5.14 本章复盘");
    note("C++ 对照", "Array、Map、Set 是 JS 运行时集合；TS 类型帮助你约束和表达使用意图。");

    const review = [
        "Array.from 回调适合创建互不共享的对象元素",
        "push/pop/shift/unshift/splice 会修改原数组",
        "slice/map/filter/reduce 通常返回新值",
        "find 可能返回 undefined，findIndex 找不到返回 -1",
        "flatMap 适合一对多转换",
        "sort 默认按字符串排序，并且会原地修改",
        "Map 适合动态键和值容器，Object 适合固定字段记录",
        "Set 对对象按引用身份比较",
        "WeakMap/WeakSet 不可迭代，适合对象元数据",
        "readonly 数组是类型层约束，as const 能保留字面量和只读信息"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 5. */
export function runChapter(): void {
    demoArrayCreation();
    demoMutableQueueOperations();
    demoSpliceAndSlice();
    demoMapFilterReduce();
    demoManualHigherOrderFunctions();
    demoSearchMethods();
    demoFlatAndFlatMap();
    demoSort();
    demoMapVsObject();
    demoSet();
    demoWeakCollections();
    demoReadonlyArrays();
    demoUserSummaryScenario();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
