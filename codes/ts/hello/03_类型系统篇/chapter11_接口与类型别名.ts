// =============================================================================
// 第 11 章：接口与类型别名
// =============================================================================
//
// 【学习目标】
//   1. 掌握 interface 与 type 的选择边界
//   2. 理解 extends、&、索引签名和函数类型接口
//   3. 理解 readonly、结构化类型、额外属性检查和 assignment compatibility
//   4. 掌握 branded/opaque type、回调类型设计和 variance 基础
//
// 【运行方式】
//   npm run build
//   node dist/03_类型系统篇/chapter11_接口与类型别名.js
//   或 npm run chapter -- 11（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

declare const userIdBrand: unique symbol;
declare const orderIdBrand: unique symbol;

/** Branded string that represents a user identifier and cannot be mixed with OrderId by accident. */
export type UserId = string & { readonly [userIdBrand]: "UserId" };

/** Branded string that represents an order identifier and cannot be mixed with UserId by accident. */
export type OrderId = string & { readonly [orderIdBrand]: "OrderId" };

interface NamedEntity {
    readonly id: UserId;
    name: string;
}

interface Person extends NamedEntity {
    readonly kind: "person";
    readonly tags: readonly string[];
}

interface Employee extends NamedEntity {
    readonly kind: "employee";
    readonly tags: readonly string[];
    department: string;
}

type AuditInfo = {
    readonly createdAt: string;
    readonly updatedAt?: string;
};

type EmployeeRecord = Employee & AuditInfo;

interface LabelFormatter {
    (entity: NamedEntity): string;
}

interface AttributeBag {
    [key: string]: string | number | boolean;
}

interface Animal {
    kind: string;
    name: string;
}

interface Dog extends Animal {
    kind: "dog";
    bark(): string;
}

type Handler<T> = (value: T) => string;

interface LegacyHandler<T> {
    handle(value: T): string;
}

// =============================================================================
// 11.1 interface vs type
// =============================================================================
//
// C++ 对照：
//   interface 更像公共形状契约，type 更像更通用的类型表达式。
//   interface 适合描述对象和可扩展 API，type 更适合联合、交叉和条件类型。
//
// 经验法则：
//   如果核心是“对象长什么样”，优先 interface。
//   如果核心是“类型由什么组合而成”，type 更自然。

function demoInterfaceVsType(): void {
    section("11.1 interface vs type");
    note("C++ 对照", "interface 适合对象形状契约，type 更适合组合和别名。");

    const shapeAsInterface: Person = {
        id: makeUserId("u1"),
        name: "Ada",
        kind: "person",
        tags: ["team", "admin"]
    };

    const shapeAsType: AuditInfo & { note: string } = {
        createdAt: "2026-05-18T00:00:00.000Z",
        note: "type alias can combine intersections directly"
    };

    showJson("interface vs type", {
        shapeAsInterface,
        shapeAsType,
        explanation: "interface 更自然表达对象契约；type 更适合组合出来的结果类型"
    });
    note("输出解释", "这两个结构运行时都只是普通对象；差异主要存在于编译期表达能力。");
}

// =============================================================================
// 11.2 extends vs &
// =============================================================================
//
// C++ 对照：
//   extends 更像“在已有接口基础上扩展成员”。
//   & 更像把两个类型的要求同时叠加到同一个对象上。
//
// 使用边界：
//   interface extends interface 通常最直观。
//   type alias 交叉适合把多个片段拼成一个复合对象类型。

/** Format any named entity for display. */
export function describeEntity(entity: NamedEntity): string {
    return `${entity.id}:${entity.name}`;
}

function demoExtendsAndIntersection(): void {
    section("11.2 extends vs &");
    note("C++ 对照", "extends 偏向层次扩展，& 偏向把多个形状拼成一个交集。");

    const employee: EmployeeRecord = {
        id: makeUserId("u2"),
        name: "Grace",
        kind: "employee",
        tags: ["platform", "reviewer"],
        department: "platform",
        createdAt: "2026-05-18T00:00:00.000Z"
    };

    showJson("extends 与交叉类型", {
        employee,
        described: describeEntity(employee),
        department: employee.department
    });
    note("输出解释", "EmployeeRecord 通过 interface extends 和 type intersection 一起表达。");
}

// =============================================================================
// 11.3 索引签名
// =============================================================================
//
// C++ 对照：
//   索引签名让对象像动态键值表。
//   它适合配置、标签和字典，而不是固定字段 DTO。
//
// 常见坑：
//   固定字段对象不应被索引签名过度泛化，否则会丢失字段精度。
//   对动态键集合，索引签名比大量可选字段更自然。

function demoIndexSignature(): void {
    section("11.3 索引签名：动态键值对象");
    note("C++ 对照", "索引签名更像小型字典容器，不是固定布局 struct。");

    const bag: AttributeBag = {
        host: "localhost",
        port: 3000,
        debug: true,
        retries: 2
    };

    showJson("索引签名示例", {
        bag,
        debugValue: bag["debug"],
        portValue: bag["port"]
    });
    note("输出解释", "bag 的键名在编译期不是固定集合；它更适合记录可扩展配置。");
}

// =============================================================================
// 11.4 函数类型接口
// =============================================================================
//
// C++ 对照：
//   函数类型接口像一个可调用契约。
//   它通常用于回调、格式化器、转换器这类“一件事”的抽象。

function demoFunctionTypeInterface(): void {
    section("11.4 函数类型接口");
    note("C++ 对照", "函数类型接口适合描述回调接口，而不是普通数据对象。");

    const formatter: LabelFormatter = (entity) => `${entity.id.toUpperCase()}:${entity.name}`;
    const namedEntity: NamedEntity = {
        id: makeUserId("u3"),
        name: "Linus"
    };

    showJson("函数类型接口示例", {
        formatted: formatter(namedEntity),
        formatterType: typeof formatter
    });
    note("输出解释", "LabelFormatter 只约束入参和返回值，不关心具体实现是普通函数还是箭头函数。");
}

// =============================================================================
// 11.5 readonly
// =============================================================================
//
// C++ 对照：
//   readonly 是类型层约束，不是运行时冻结。
//   它适合表达“调用方不应修改这个字段/数组”。
//
// 常见坑：
//   readonly 数组仍可能被其他别名在运行时修改。
//   如果你需要运行时不可变，应该使用 freeze 或不可变数据策略。

function demoReadonly(): void {
    section("11.5 readonly：只读接口和只读数组");
    note("C++ 对照", "readonly 只约束编译期写法，不等于运行时内存冻结。");

    const person: Person = {
        id: makeUserId("u4"),
        name: "Margaret",
        kind: "person",
        tags: ["compiler", "research"]
    };
    const tags: readonly string[] = [...person.tags];
    const mutableAlias = ["a", "b"];
    const readonlyView: readonly string[] = mutableAlias;
    mutableAlias.push("c");

    showJson("readonly 示例", {
        person,
        tags,
        readonlyView
    });
    note("输出解释", "readonlyView 不能通过自己修改，但原始 mutableAlias 仍然可以变。");
}

// =============================================================================
// 11.6 结构化类型 vs 名义类型
// =============================================================================
//
// C++ 对照：
//   TS 默认是结构化类型系统。
//   只要成员形状兼容，名字不同的类型也能赋值。
//   这和 C++ 需要同一类型名或继承关系的习惯不同。

function demoStructuralTyping(): void {
    section("11.6 结构化类型 vs 名义类型");
    note("C++ 对照", "形状一致即可赋值，类型名字本身不构成默认屏障。");

    interface Point2D {
        x: number;
        y: number;
    }

    interface Size2D {
        x: number;
        y: number;
    }

    const point: Point2D = { x: 1, y: 2 };
    const size: Size2D = point;

    showJson("结构化类型示例", {
        point,
        size,
        sameRuntimeShape: JSON.stringify(point) === JSON.stringify(size)
    });
    note("输出解释", "Point2D 和 Size2D 名字不同，但形状相同，所以 TS 允许赋值。");
}

// =============================================================================
// 11.7 额外属性检查 vs assignment compatibility
// =============================================================================
//
// C++ 对照：
//   对象字面量在传参/赋值时会触发更严格的额外属性检查。
//   一旦先放入变量，再赋值给目标类型，TS 更看重 assignment compatibility。
//
// 常见坑：
//   看到对象字面量多一个字段就报错，不代表运行时真的不能带这个字段。
//   这正是 TypeScript 想帮助你发现的“多写了一个字段但没意识到”的问题。

interface AppConfig {
    host: string;
    port: number;
}

function renderConfig(config: AppConfig): string {
    return `${config.host}:${config.port}`;
}

function demoExcessPropertyCheck(): void {
    section("11.7 额外属性检查 vs assignment compatibility");
    note("C++ 对照", "对象字面量更容易触发额外属性检查；通过变量传递时会进入兼容性检查。");

    const withExtra = {
        host: "localhost",
        port: 3000,
        debug: true
    };
    const assigned: AppConfig = withExtra;

    showJson("额外属性检查示例", {
        literalWithExtra: withExtra,
        assigned,
        rendered: renderConfig(assigned)
    });
    note("输出解释", "direct literal with extra fields would be rejected, but a named variable is assignment-compatible.");
}

// =============================================================================
// 11.8 branded / opaque type
// =============================================================================
//
// C++ 对照：
//   branded type 用交叉类型模拟名义类型。
//   运行时仍是 string，编译期却能把 UserId 和 OrderId 分开。
//
// 真实场景：
//   用户 id、订单 id、文件 id 都可能是 string。
//   只要业务上不能互换，就值得做 brand。

/** Create a branded UserId after trimming and validating the raw string. */
export function makeUserId(raw: string): UserId {
    const trimmed = raw.trim();
    if (trimmed === "") {
        throw new Error("user id must not be empty");
    }
    return trimmed as UserId;
}

/** Create a branded OrderId after trimming and validating the raw string. */
export function makeOrderId(raw: string): OrderId {
    const trimmed = raw.trim();
    if (trimmed === "") {
        throw new Error("order id must not be empty");
    }
    return trimmed as OrderId;
}

function demoBrandedTypes(): void {
    section("11.8 branded / opaque type");
    note("C++ 对照", "brand 模式用于表达名义区分；运行时值仍是字符串。");

    const userId = makeUserId(" user-001 ");
    const orderId = makeOrderId(" order-900 ");
    const userMap = new Map<UserId, string>();
    userMap.set(userId, "Ada");

    showJson("brand 类型示例", {
        userId,
        orderId,
        lookupUser: userMap.get(userId),
        runtimeTypeUserId: typeof userId,
        runtimeTypeOrderId: typeof orderId
    });
    note("输出解释", "userId 和 orderId 在运行时都只是 string；区别只存在于编译器检查里。");
}

// =============================================================================
// 11.9 variance 基础和回调设计
// =============================================================================
//
// C++ 对照：
//   回调函数的参数类型不是越窄越好。
//   对“消费值”的回调，接受更宽类型的函数通常更安全。
//
// 设计建议：
//   需要更严格检查时，优先使用函数类型别名而不是把一切都写成对象方法。

function runDogHandler(handler: Handler<Dog>): string {
    const dog: Dog = {
        kind: "dog",
        name: "Rex",
        bark: () => "woof"
    };
    return handler(dog);
}

function runDogLegacyHandler(handler: LegacyHandler<Dog>): string {
    const dog: Dog = {
        kind: "dog",
        name: "Rex",
        bark: () => "woof"
    };
    return handler.handle(dog);
}

function demoVarianceBasics(): void {
    section("11.9 variance 基础和回调设计");
    note("C++ 对照", "消费值的回调通常希望能接受更宽的输入；方法型回调历史上更宽松。");

    const animalHandler: Handler<Animal> = (animal) => `${animal.kind}:${animal.name}`;
    const animalLegacyHandler: LegacyHandler<Animal> = {
        handle(animal) {
            return `${animal.kind}:${animal.name}`;
        }
    };

    showJson("variance 示例", {
        functionTypeResult: runDogHandler(animalHandler),
        methodTypeResult: runDogLegacyHandler(animalLegacyHandler),
        guidance: "prefer function-type callbacks when you want clear variance behavior"
    });
    note("输出解释", "animalHandler 能消费 Dog，因为它接受更宽的 Animal；回调设计里通常优先用函数类型。");
}

// =============================================================================
// 11.10 工程场景：用户注册表
// =============================================================================
//
// C++ 对照：
//   通过 branded id 和接口组合，可以把用户记录、订单记录和配置记录分开。
//   这比统一用 string 和 any 更可维护。

/** Build a small immutable user registry for demonstration purposes. */
export function buildUserRegistry(users: readonly EmployeeRecord[]): Map<UserId, EmployeeRecord> {
    return new Map(users.map((user) => [user.id, user] as const));
}

function demoUserRegistryScenario(): void {
    section("11.10 工程场景：用户注册表");
    note("C++ 对照", "品牌 ID + 接口组合可以在编译期降低 ID 混用的概率。");

    const users: readonly EmployeeRecord[] = [
        {
            id: makeUserId("u1"),
            name: "Ada",
            kind: "employee",
            tags: ["platform"],
            department: "platform",
            createdAt: "2026-05-18T00:00:00.000Z"
        },
        {
            id: makeUserId("u2"),
            name: "Grace",
            kind: "employee",
            tags: ["security"],
            department: "security",
            createdAt: "2026-05-18T00:00:00.000Z",
            updatedAt: "2026-05-18T01:00:00.000Z"
        }
    ];

    const registry = buildUserRegistry(users);

    showJson("用户注册表", {
        keys: Array.from(registry.keys()),
        firstUser: registry.get(makeUserId("u1")),
        secondUser: registry.get(makeUserId("u2"))
    });
    note("输出解释", "registry 以 UserId 作为键；OrderId 不能在类型层面误用到这里。");
}

// =============================================================================
// 11.11 本章复盘
// =============================================================================
//
// C++ 对照：
//   interface 适合对象契约，type 适合组合表达。
//   branded type 用来在结构化类型系统里补一层名义区分。

function demoChapterReview(): void {
    section("11.11 本章复盘");
    note("C++ 对照", "JS/TS 的形状兼容很强，brand 和更清晰的回调类型设计能帮助你补回一些边界。");

    const review = [
        "interface 适合对象契约，type 更适合联合/交叉/工具类型",
        "extends 表达层次扩展，& 表达类型交集",
        "索引签名适合动态键值对象",
        "函数类型接口适合回调和 formatter",
        "readonly 是编译期约束，不等于运行时冻结",
        "结构化类型默认按成员形状兼容，不依赖名字",
        "额外属性检查主要发生在对象字面量边界",
        "brand 类型让 string 在编译期表现得更像不同的名义类型",
        "消费值的回调要认真设计参数边界"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 11. */
export function runChapter(): void {
    demoInterfaceVsType();
    demoExtendsAndIntersection();
    demoIndexSignature();
    demoFunctionTypeInterface();
    demoReadonly();
    demoStructuralTyping();
    demoExcessPropertyCheck();
    demoBrandedTypes();
    demoVarianceBasics();
    demoUserRegistryScenario();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
