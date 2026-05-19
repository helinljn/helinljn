// =============================================================================
// 第 21 章：装饰器
// =============================================================================
//
// 【学习目标】
//   1. 理解 TypeScript 当前标准装饰器与 legacy experimentalDecorators 的边界
//   2. 掌握类、方法、accessor、字段装饰器和装饰器工厂的基本写法
//   3. 观察装饰器求值与应用顺序
//   4. 用日志、计时和权限示例理解装饰器适合处理的横切关注点
//
// 【运行方式】
//   npm run build
//   node dist/06_模块与工程篇/chapter21_装饰器.js
//   或 npm run chapter -- 21（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type ConstructorInfo = {
    registeredName: string;
    className: string;
};

type AuditEntry = {
    method: string;
    argsCount: number;
    resultPreview: string;
};

type MetricEntry = {
    method: string;
    units: number;
};

type Role = "guest" | "operator" | "admin";

type RoleAware = {
    readonly role: Role;
};

const constructorRegistry: ConstructorInfo[] = [];
const auditLog: AuditEntry[] = [];
const metricLog: MetricEntry[] = [];
const decoratorOrderLog: string[] = [];

function printSnippet(label: string, lines: readonly string[]): void {
    console.log(`${label}:`);
    for (const line of lines) {
        console.log(`  ${line}`);
    }
}

function previewValue(value: unknown): string {
    if (typeof value === "string") {
        return value;
    }
    if (typeof value === "number" || typeof value === "boolean") {
        return String(value);
    }
    if (value === undefined) {
        return "undefined";
    }
    if (value === null) {
        return "null";
    }
    return JSON.stringify(value);
}

function clampNumber(value: number, min: number, max: number): number {
    return Math.min(max, Math.max(min, value));
}

function clearRuntimeLogs(): void {
    constructorRegistry.length = 0;
    auditLog.length = 0;
    metricLog.length = 0;
    decoratorOrderLog.length = 0;
}

// =============================================================================
// 21.1 当前教程采用标准装饰器，不采用 legacy 参数装饰器
// =============================================================================
//
// TypeScript 早期有一套 experimentalDecorators 语义，常见于旧版 Angular、NestJS
// 或 reflect-metadata 生态。现代 TypeScript 支持 ECMAScript 标准装饰器语义：
//   - 类装饰器
//   - 方法装饰器
//   - accessor 装饰器
//   - 字段装饰器
//
// 当前 tsconfig.json 没有启用 experimentalDecorators。
// 因此本章不写 legacy 参数装饰器，也不依赖 emitDecoratorMetadata 或 reflect-metadata。
//
// C++ 对照：
//   装饰器像 AOP 或编译期注解驱动的代码改写点，但 TS 装饰器会影响运行时类定义。
//   它不是 C++ attribute 的直接等价物，因为它可以返回包装后的函数或初始化器。

function demoDecoratorBoundary(): void {
    section("21.1 当前教程采用标准装饰器，不采用 legacy 参数装饰器");
    note("C++ 对照", "装饰器接近 AOP/注解驱动的横切逻辑，但它会参与 JS 运行时类定义。");

    showJson("本章装饰器边界", {
        tsconfigExperimentalDecorators: false,
        standardDecoratorsUsed: ["class", "method", "accessor", "field"],
        notUsed: ["parameter decorator", "emitDecoratorMetadata", "reflect-metadata"],
        reason: "当前教程采用 TS 6.0 + NodeNext 的可运行边界"
    });

    printSnippet("标准方法装饰器形状", [
        "function logged(value, context) {",
        "    return function (...args) {",
        "        console.log(context.name);",
        "        return value.call(this, ...args);",
        "    };",
        "}"
    ]);

    note("常见坑", "不要把 legacy 装饰器教程中的 `(target, key, descriptor)` 直接搬到标准装饰器代码里。");
    note("类型边界", "装饰器可以改变运行时行为；类型系统只检查包装函数形状，不会证明业务语义一定正确。");
}

// =============================================================================
// 21.2 类装饰器：注册元数据，而不是替代依赖注入容器
// =============================================================================
//
// 类装饰器接收类构造函数和 ClassDecoratorContext。
// 它适合做轻量注册、标记、校验，也可以返回一个替换类。
// 入门阶段建议先用它记录元数据，不要急着构建复杂的依赖注入框架。
//
// C++ 对照：
//   类装饰器像给类附加一段注册逻辑。它和 C++ 静态注册表有相似风险：
//   注册发生在模块加载阶段，顺序和副作用要保持可控。

function service(name: string) {
    return function (value: Function, context: ClassDecoratorContext): void {
        constructorRegistry.push({
            registeredName: name,
            className: String(context.name ?? value.name)
        });
    };
}

function demoClassDecorator(): void {
    section("21.2 类装饰器：注册元数据，而不是替代依赖注入容器");
    note("C++ 对照", "类装饰器像静态注册表入口；注册时机是模块加载或类定义执行阶段。");

    clearRuntimeLogs();

    @service("billing")
    class BillingService {
        describe(): string {
            return "BillingService handles invoice commands";
        }
    }

    const serviceInstance = new BillingService();
    showJson("类装饰器注册结果", {
        registry: constructorRegistry,
        instanceDescription: serviceInstance.describe()
    });

    note("输出解释", "装饰器在 class 定义完成时执行，所以实例化前 registry 已经有 BillingService。");
    note("常见坑", "类装饰器的注册副作用要稳定；不要在装饰器里启动网络、读取环境密钥或写文件。");
}

// =============================================================================
// 21.3 方法装饰器：包装函数以记录日志
// =============================================================================
//
// 方法装饰器接收原方法和 ClassMethodDecoratorContext。
// 返回一个新函数时，新函数会替代原方法。
// 这很适合日志、指标、权限等横切逻辑，但要注意保留 this 和返回值。
//
// C++ 对照：
//   方法装饰器像在调用点外面套一层拦截器。和宏相比，它更结构化；
//   和虚函数相比，它不是动态分派机制，而是类定义时的包装。

function audit<This, Args extends unknown[], Return>(
    originalMethod: (this: This, ...args: Args) => Return,
    context: ClassMethodDecoratorContext<This, (this: This, ...args: Args) => Return>
): (this: This, ...args: Args) => Return {
    const methodName = String(context.name);
    return function (this: This, ...args: Args): Return {
        const result = originalMethod.apply(this, args);
        auditLog.push({
            method: methodName,
            argsCount: args.length,
            resultPreview: previewValue(result)
        });
        return result;
    };
}

function demoMethodDecorator(): void {
    section("21.3 方法装饰器：包装函数以记录日志");
    note("C++ 对照", "方法装饰器像调用拦截器，适合横切日志，但不是继承或虚函数替代品。");

    clearRuntimeLogs();

    class ConfigFormatter {
        @audit
        format(name: string, enabled: boolean): string {
            return `${name}:${enabled ? "enabled" : "disabled"}`;
        }
    }

    const formatter = new ConfigFormatter();
    const output = formatter.format("strict", true);

    showJson("方法调用结果与审计日志", {
        output,
        auditLog
    });

    note("输出解释", "format 的业务返回值保持不变；装饰器只额外写入审计日志。");
    note("类型边界", "包装函数必须使用同样的 this、参数和返回值形状，否则会破坏调用方假设。");
}

// =============================================================================
// 21.4 accessor 装饰器：拦截 get/set 和初始值
// =============================================================================
//
// 标准装饰器区分字段和 accessor。
// auto-accessor 写法是：
//   accessor retryCount = 3;
//
// accessor 装饰器可以返回 get、set、init，用来包装读取、写入和初始值。
// 这适合做范围裁剪、单位转换或简单归一化。
//
// C++ 对照：
//   accessor 装饰器接近在 getter/setter 上统一加校验逻辑；
//   它不是直接暴露裸字段，因此可以集中维护不变量。

function clampAccessor(min: number, max: number) {
    return function <This>(
        value: ClassAccessorDecoratorTarget<This, number>,
        context: ClassAccessorDecoratorContext<This, number>
    ): ClassAccessorDecoratorResult<This, number> {
        const propertyName = String(context.name);
        return {
            get(this: This): number {
                return value.get.call(this);
            },
            set(this: This, nextValue: number): void {
                value.set.call(this, clampNumber(nextValue, min, max));
            },
            init(initialValue: number): number {
                metricLog.push({
                    method: `init:${propertyName}`,
                    units: clampNumber(initialValue, min, max)
                });
                return clampNumber(initialValue, min, max);
            }
        };
    };
}

function demoAccessorDecorator(): void {
    section("21.4 accessor 装饰器：拦截 get/set 和初始值");
    note("C++ 对照", "accessor 装饰器像集中管理 getter/setter 不变量。");

    clearRuntimeLogs();

    class RetryPolicy {
        @clampAccessor(0, 10)
        accessor retryCount = 20;
    }

    const policy = new RetryPolicy();
    const afterInit = policy.retryCount;
    policy.retryCount = -3;
    const afterLowWrite = policy.retryCount;
    policy.retryCount = 8;
    const afterNormalWrite = policy.retryCount;

    showJson("accessor 范围裁剪", {
        afterInit,
        afterLowWrite,
        afterNormalWrite,
        metricLog
    });

    note("输出解释", "初始值 20 被裁剪到 10，写入 -3 被裁剪到 0，正常写入 8 保持不变。");
    note("常见坑", "accessor 装饰器包装的是访问器，不是普通字段；字段装饰器不能直接实现同样的 get/set 拦截。");
}

// =============================================================================
// 21.5 字段装饰器：调整实例字段初始值
// =============================================================================
//
// 字段装饰器接收的 value 通常是 undefined，因为字段值属于每个实例。
// 它可以返回一个 initializer 函数，用来转换每个实例的初始值。
//
// C++ 对照：
//   字段装饰器像统一改写成员默认初始化逻辑，但它发生在 JS 类实例初始化期间。

function prefixField(prefix: string) {
    return function <This>(
        _value: undefined,
        context: ClassFieldDecoratorContext<This, string>
    ): (initialValue: string) => string {
        const fieldName = String(context.name);
        return function (initialValue: string): string {
            return `${prefix}${fieldName}:${initialValue}`;
        };
    };
}

function demoFieldDecorator(): void {
    section("21.5 字段装饰器：调整实例字段初始值");
    note("C++ 对照", "字段装饰器像统一改写成员默认初始化逻辑；它不拦截后续读写。");

    class Endpoint {
        @prefixField("svc:")
        name = "billing";
    }

    const endpoint = new Endpoint();
    endpoint.name = "manually-overwritten";

    showJson("字段初始值转换", {
        initializedName: new Endpoint().name,
        overwrittenName: endpoint.name
    });

    note("输出解释", "字段装饰器只转换初始化值；后续普通赋值不会被 prefixField 拦截。");
    note("类型边界", "字段装饰器返回的 initializer 必须接受并返回字段类型；这里字段是 string。");
}

// =============================================================================
// 21.6 装饰器工厂：让横切逻辑可配置
// =============================================================================
//
// 装饰器工厂就是“返回装饰器的函数”。
// 它允许把配置参数提前绑定到装饰器里，例如：
//   - @service("billing")
//   - @clampAccessor(0, 10)
//   - @requireRole("admin")
//
// C++ 对照：
//   装饰器工厂像生成带配置的策略对象或模板实例，但运行时仍是普通 JS 函数闭包。

function requireRole(requiredRole: Role) {
    return function <This extends RoleAware, Args extends unknown[], Return>(
        originalMethod: (this: This, ...args: Args) => Return,
        context: ClassMethodDecoratorContext<This, (this: This, ...args: Args) => Return>
    ): (this: This, ...args: Args) => Return {
        const methodName = String(context.name);
        return function (this: This, ...args: Args): Return {
            if (this.role !== requiredRole) {
                throw new Error(`permission denied for ${methodName}: required ${requiredRole}, got ${this.role}`);
            }
            return originalMethod.apply(this, args);
        };
    };
}

function demoDecoratorFactory(): void {
    section("21.6 装饰器工厂：让横切逻辑可配置");
    note("C++ 对照", "装饰器工厂像把策略参数绑定进拦截器实例。");

    class AdminPanel implements RoleAware {
        constructor(readonly role: Role) {}

        @requireRole("admin")
        deleteUser(id: string): string {
            return `deleted:${id}`;
        }
    }

    const admin = new AdminPanel("admin");
    const guest = new AdminPanel("guest");

    let guestResult = "not-called";
    try {
        guestResult = guest.deleteUser("u1");
    } catch (error: unknown) {
        guestResult = error instanceof Error ? error.message : "unknown error";
    }

    showJson("权限装饰器工厂", {
        adminResult: admin.deleteUser("u1"),
        guestResult
    });

    note("常见坑", "权限装饰器不能替代服务端真实鉴权；它只是把重复检查集中到方法边界。");
    note("类型边界", "RoleAware 约束保证 this.role 存在，但用户身份、token 等外部输入仍需运行时校验。");
}

// =============================================================================
// 21.7 执行顺序：表达式先求值，装饰器再应用
// =============================================================================
//
// 多个装饰器叠在同一个声明上时，要区分两个阶段：
//   1. 装饰器表达式求值：从上到下
//   2. 装饰器应用：从下到上
//
// 这和函数组合类似。越靠近声明的装饰器先包装原始成员，外层装饰器再包装前一个结果。
//
// C++ 对照：
//   可以把它想成多层模板/包装器组合，但这里发生在 JS 类定义阶段，并且有运行时顺序。

function order(label: string) {
    decoratorOrderLog.push(`evaluate:${label}`);
    return function (
        _value: (this: unknown, ...args: unknown[]) => unknown,
        context: ClassMethodDecoratorContext<unknown, (this: unknown, ...args: unknown[]) => unknown>
    ): void {
        decoratorOrderLog.push(`apply:${label}:${String(context.name)}`);
    };
}

function demoDecoratorOrder(): void {
    section("21.7 执行顺序：表达式先求值，装饰器再应用");
    note("C++ 对照", "多装饰器像多层包装器组合，但有明确的 JS 运行时求值顺序。");

    clearRuntimeLogs();

    class OrderDemo {
        @order("outer")
        @order("inner")
        run(): string {
            return "ok";
        }
    }

    const result = new OrderDemo().run();

    showJson("装饰器顺序", {
        result,
        decoratorOrderLog
    });

    note("输出解释", "outer 表达式先求值，inner 表达式后求值；应用顺序反过来，inner 先应用。");
    note("常见坑", "多个会包装同一方法的装饰器叠加时，顺序就是行为的一部分，必须写测试固定下来。");
}

// =============================================================================
// 21.8 工程实战：日志、计时和权限都要有退出边界
// =============================================================================
//
// 装饰器最适合处理“横切关注点”：
//   - 日志：所有命令方法都记录调用
//   - 计时：所有外部 API 都记录耗时
//   - 权限：所有危险操作都检查角色
//
// 但它也容易被滥用：
//   - 把核心业务藏进装饰器，阅读代码时看不到真实行为
//   - 在装饰器里启动定时器、网络或文件 watcher，造成副作用难以清理
//   - 让测试依赖类定义顺序，导致重构时出现隐性回归
//
// C++ 对照：
//   AOP 能减少重复代码，但也可能让控制流变隐形。越靠近边界越适合装饰器，核心算法越应保持显式。

function measured<This, Args extends unknown[], Return>(
    originalMethod: (this: This, ...args: Args) => Return,
    context: ClassMethodDecoratorContext<This, (this: This, ...args: Args) => Return>
): (this: This, ...args: Args) => Return {
    const methodName = String(context.name);
    return function (this: This, ...args: Args): Return {
        const result = originalMethod.apply(this, args);
        metricLog.push({
            method: methodName,
            units: methodName.length + args.length
        });
        return result;
    };
}

function demoEngineeringUseCase(): void {
    section("21.8 工程实战：日志、计时和权限都要有退出边界");
    note("C++ 对照", "AOP 适合边界层横切逻辑；核心算法仍应保持显式和可测试。");

    clearRuntimeLogs();

    class JobController implements RoleAware {
        constructor(readonly role: Role) {}

        @measured
        @audit
        @requireRole("operator")
        restartJob(jobId: string): { jobId: string; status: "restarted" } {
            return { jobId, status: "restarted" };
        }
    }

    const controller = new JobController("operator");
    const result = controller.restartJob("job-42");

    showJson("横切逻辑组合", {
        result,
        auditLog,
        metricLog
    });

    note("输出解释", "业务方法只返回重启结果；日志和指标由装饰器集中记录。");
    note("副作用边界", "本章的日志和指标都是内存数组，并在每个 demo 前清空；没有定时器、文件或网络副作用。");
}

// =============================================================================
// 21.9 本章复盘
// =============================================================================
//
// C++ 对照：
//   装饰器像结构化的运行时 AOP 入口。它能减少重复边界代码，但也会让行为分散到声明之外。
//   对团队项目来说，装饰器越强大，越需要约定和测试。

function demoChapterReview(): void {
    section("21.9 本章复盘");
    note("C++ 对照", "装饰器像运行时 AOP：适合边界横切逻辑，不适合隐藏核心业务。");

    showJson("关键结论", [
        "本章采用标准装饰器，不启用 legacy experimentalDecorators",
        "类装饰器适合轻量注册和标记，但注册副作用要可控",
        "方法装饰器通过包装函数实现日志、计时、权限等横切逻辑",
        "accessor 装饰器可以拦截 get/set/init，字段装饰器主要转换初始值",
        "装饰器工厂用闭包绑定配置参数",
        "多个装饰器叠加时，表达式从上到下求值，应用从下到上发生",
        "装饰器不能替代运行时输入校验、鉴权系统或测试"
    ]);

    note("可重复运行", "本章只使用内存数组记录示例输出，每个 demo 都会清空状态。");
    note("下一章衔接", "ch22 会进入迭代器与生成器，开始连接同步遍历与异步数据流。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 21 demos in a stable order. */
export function runChapter(): void {
    demoDecoratorBoundary();
    demoClassDecorator();
    demoMethodDecorator();
    demoAccessorDecorator();
    demoFieldDecorator();
    demoDecoratorFactory();
    demoDecoratorOrder();
    demoEngineeringUseCase();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
