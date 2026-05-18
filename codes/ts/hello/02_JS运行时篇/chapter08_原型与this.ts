// =============================================================================
// 第 8 章：原型与 this
// =============================================================================
//
// 【学习目标】
//   1. 理解 [[Prototype]]、__proto__、prototype 的区别
//   2. 掌握 Object.create 和原型链属性查找
//   3. 理解 this 的默认绑定、隐式绑定、显式绑定和 new 绑定
//   4. 区分普通函数 this 与箭头函数 lexical this
//
// 【运行方式】
//   npm run build
//   node dist/02_JS运行时篇/chapter08_原型与this.js
//   或 npm run chapter -- 8（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type PrototypePerson = {
    name: string;
    role?: string;
    describe(): string;
};

type MethodHost = {
    readonly label: string;
    describe(this: MethodHost | undefined): string;
};

type CounterLike = {
    count: number;
    increment(this: CounterLike): number;
};

// =============================================================================
// 8.1 原型链：属性查找不是类继承拷贝
// =============================================================================
//
// C++ 对照：
//   原型链不是 vtable。
//   JS 对象查找属性时，先看对象自身，再沿 [[Prototype]] 链向上查找。
//
// 常见坑：
//   读取继承属性和读取自有属性看起来一样。
//   需要判断属性是否属于对象自身时，应使用 Object.hasOwn。

function demoPrototypeChainLookup(): void {
    section("8.1 原型链：属性查找路径");
    note("C++ 对照", "原型链是运行时对象到对象的委托链，不是 C++ 类布局或虚表。");

    const base: { role: string; describe(this: PrototypePerson): string } = {
        role: "base-role",
        describe(): string {
            return `${this.name}:${this.role}`;
        }
    };

    const child = Object.create(base) as PrototypePerson;
    child.name = "child";

    showJson("原型链查找", {
        ownName: child.name,
        inheritedRole: child.role,
        describe: child.describe(),
        hasOwnName: Object.hasOwn(child, "name"),
        hasOwnRole: Object.hasOwn(child, "role"),
        prototypeIsBase: Object.getPrototypeOf(child) === base
    });
    note("输出解释", "name 是 child 自有属性，role 和 describe 来自 base 原型对象。");
}

// =============================================================================
// 8.2 __proto__、Object.getPrototypeOf 和 prototype
// =============================================================================
//
// C++ 对照：
//   C++ 类继承关系由类型系统和对象布局决定。
//   JS 原型关系是运行时对象关系。
//
// 使用边界：
//   `__proto__` 是历史访问器，不推荐在新代码中依赖。
//   推荐使用 Object.getPrototypeOf/Object.setPrototypeOf，但频繁改原型会影响性能和可读性。
//
// 常见坑：
//   函数的 `prototype` 属性不是对象实例的 `[[Prototype]]` 本身。
//   `new Fn()` 创建的对象会把 `Fn.prototype` 作为自己的原型。

function LegacyConstructor(this: { name: string }, name: string) {
    this.name = name;
}

LegacyConstructor.prototype.describe = function describe(this: { name: string }): string {
    return `legacy:${this.name}`;
};

function demoPrototypeTerms(): void {
    section("8.2 __proto__、[[Prototype]]、prototype");
    note("C++ 对照", "不要把 constructor.prototype 当作 C++ class 定义；它只是 new 时使用的原型对象。");

    const created = new (LegacyConstructor as unknown as new (name: string) => { name: string; describe(): string })("Ada");

    showJson("prototype 术语对比", {
        instanceName: created.name,
        describe: created.describe(),
        instancePrototypeIsFunctionPrototype: Object.getPrototypeOf(created) === LegacyConstructor.prototype,
        hasOwnDescribe: Object.hasOwn(created, "describe"),
        functionPrototypeKeys: Object.keys(LegacyConstructor.prototype)
    });
    note("输出解释", "created 自己只有 name；describe 来自 LegacyConstructor.prototype。");
}

// =============================================================================
// 8.3 Object.create：显式指定原型
// =============================================================================
//
// C++ 对照：
//   Object.create 不是调用构造函数。
//   它直接创建一个以指定对象为原型的新对象。
//
// 工程建议：
//   现代业务代码通常优先使用对象字面量、class 或组合。
//   Object.create 适合理解原型机制，或创建无原型字典对象。

function demoObjectCreate(): void {
    section("8.3 Object.create：显式指定原型");
    note("C++ 对照", "Object.create 是运行时对象委托，不是 C++ 构造函数或继承声明。");

    const servicePrototype = {
        kind: "service",
        describe(this: { name: string; kind: string }): string {
            return `${this.kind}:${this.name}`;
        }
    };

    const service = Object.create(servicePrototype) as { name: string; kind: string; describe(): string };
    service.name = "billing";

    const dictionary = Object.create(null) as Record<string, number>;
    dictionary["ok"] = 200;
    dictionary["missing"] = 404;

    showJson("Object.create 示例", {
        service: service.describe(),
        serviceKindFromPrototype: service.kind,
        dictionary,
        dictionaryPrototypeIsNull: Object.getPrototypeOf(dictionary) === null
    });
    note("输出解释", "service 委托到 servicePrototype；dictionary 没有普通 Object 原型。");
}

// =============================================================================
// 8.4 this 绑定规则总览
// =============================================================================
//
// C++ 对照：
//   C++ 成员函数的 this 由对象调用语法和类型系统稳定决定。
//   JS 中 this 主要由“调用方式”决定，而不是函数声明的位置。
//
// 四种绑定：
//   默认绑定：普通函数调用，在 ESM/严格模式中 this 为 undefined。
//   隐式绑定：obj.method()，this 是 obj。
//   显式绑定：call/apply/bind 指定 this。
//   new 绑定：new Fn() 创建新对象并作为 this。

function inspectThis(this: { label?: string } | undefined): string {
    return this?.label ?? "undefined-this";
}

function demoThisBindingOverview(): void {
    section("8.4 this 绑定规则总览");
    note("C++ 对照", "JS 的 this 看调用点，不看函数写在哪里；这和 C++ 成员函数直觉差异很大。");

    const host = {
        label: "implicit-host",
        inspect: inspectThis
    };

    const explicitHost = { label: "explicit-host" };

    showJson("this 绑定总览", {
        defaultBinding: inspectThis.call(undefined),
        implicitBinding: host.inspect(),
        explicitCall: inspectThis.call(explicitHost),
        explicitApply: inspectThis.apply(explicitHost)
    });
    note("输出解释", "同一个 inspectThis，因为调用方式不同，this 结果不同。");
}

// =============================================================================
// 8.5 隐式绑定丢失
// =============================================================================
//
// C++ 对照：
//   把 C++ 成员函数指针和对象拆开时也要处理绑定对象。
//   JS 中把 obj.method 赋值给变量，会丢失原来的隐式 this。
//
// 常见坑：
//   将方法直接作为回调传入时，this 可能变成 undefined。
//   常见修复方式是 bind，或使用箭头函数包装调用。

function demoLostImplicitBinding(): void {
    section("8.5 隐式绑定丢失");
    note("C++ 对照", "方法从对象上拆下来后，不会自动记住原对象。");

    const host: MethodHost = {
        label: "method-host",
        describe() {
            return `label=${this?.label ?? "missing-this"}`;
        }
    };

    const detached = host.describe;
    const detachedResult = detached.call(undefined);
    const fixedByBind = detached.bind(host);
    const fixedByWrapper = () => host.describe();

    showJson("this 丢失与修复", {
        direct: host.describe(),
        detachedResult,
        fixedByBind: fixedByBind(),
        fixedByWrapper: fixedByWrapper()
    });
    note("输出解释", "detached 没有隐式接收者；bind 或箭头包装能把调用对象固定回来。");
}

// =============================================================================
// 8.6 call、apply、bind
// =============================================================================
//
// C++ 对照：
//   call/apply/bind 没有 C++ 直接等价物。
//   可以粗略理解为以不同形式显式传入接收者对象。
//
// 使用边界：
//   call 逐个传参。
//   apply 用数组传参。
//   bind 返回一个永久绑定 this 和可选前置参数的新函数。

function formatWithPrefix(this: { prefix: string }, left: string, right: string): string {
    return `${this.prefix}:${left}:${right}`;
}

function demoCallApplyBind(): void {
    section("8.6 call、apply、bind：显式指定 this");
    note("C++ 对照", "这些 API 显式指定函数调用时的 this，属于 JS 函数对象能力。");

    const context = { prefix: "ctx" };
    const bound = formatWithPrefix.bind(context, "bound-left");

    showJson("call/apply/bind 示例", {
        call: formatWithPrefix.call(context, "a", "b"),
        apply: formatWithPrefix.apply(context, ["x", "y"]),
        bind: bound("bound-right")
    });
    note("输出解释", "bind 没有立即调用函数，而是返回一个已经绑定好 this 和部分参数的新函数。");
}

// =============================================================================
// 8.7 new 绑定和 class 语法
// =============================================================================
//
// C++ 对照：
//   JS class 是原型机制上的语法形式，不等同于 C++ 类模型。
//   使用 new 时，运行时会创建新对象，并把它作为构造函数内部的 this。
//
// 线性阅读提示：
//   class 会在面向对象篇深入。
//   这里先只用它观察 new 绑定。

class RuntimeCounter {
    #count: number;

    public constructor(start: number) {
        this.#count = start;
    }

    public increment(): number {
        this.#count += 1;
        return this.#count;
    }
}

function demoNewBinding(): void {
    section("8.7 new 绑定和 class 语法");
    note("C++ 对照", "class 语法最终仍依赖 JS 原型和 new 绑定；# 字段是运行时私有字段。");

    const counter = new RuntimeCounter(10);
    const values = [counter.increment(), counter.increment()];

    showJson("new 绑定示例", {
        values,
        prototypeName: Object.getPrototypeOf(counter).constructor.name,
        hasOwnIncrement: Object.hasOwn(counter, "increment")
    });
    note("输出解释", "increment 在原型上，不是实例自有属性；#count 是实例私有状态。");
}

// =============================================================================
// 8.8 箭头函数的 this：词法绑定
// =============================================================================
//
// C++ 对照：
//   箭头函数没有自己的 this。
//   它读取的是创建它时外层作用域中的 this。
//
// 常见坑：
//   不要把对象方法默认写成箭头函数，除非你明确不需要方法调用的 this。
//   箭头函数也不能作为构造函数使用。

function makeArrowThisDemo(label: string): {
    readonly label: string;
    readonly normal: () => string;
    readonly arrowFromMethod: () => string;
} {
    return {
        label,
        normal() {
            return `normal:${this.label}`;
        },
        arrowFromMethod() {
            const arrow = () => `arrow:${this.label}`;
            return arrow();
        }
    };
}

function demoArrowThis(): void {
    section("8.8 箭头函数 this：词法绑定");
    note("C++ 对照", "箭头函数不会创建自己的 this，它沿用外层 this。");

    const demo = makeArrowThisDemo("demo-host");
    const normal = demo.normal();
    const arrow = demo.arrowFromMethod();

    const objectWithArrow = {
        label: "object-label",
        arrow: () => "arrow-has-no-object-this"
    };

    showJson("箭头函数 this 示例", {
        normal,
        arrow,
        objectArrow: objectWithArrow.arrow()
    });
    note("输出解释", "arrowFromMethod 内部的箭头函数沿用外层方法调用时的 this。对象属性中的箭头函数不会自动绑定对象。");
}

// =============================================================================
// 8.9 工程场景：回调中的 this 安全
// =============================================================================
//
// C++ 对照：
//   回调传递时要明确状态来自哪里。
//   在 JS/TS 中，类方法作为回调传递尤其容易丢失 this。
//
// 真实场景：
//   事件处理、数组回调、框架 hook 中都可能把函数单独保存后再调用。
//   更稳的写法是传递箭头包装，或在构造/初始化处 bind。

function runCallbacks(callbacks: readonly (() => string)[]): readonly string[] {
    return callbacks.map((callback) => callback());
}

function demoCallbackThisSafety(): void {
    section("8.9 工程场景：回调中的 this 安全");
    note("C++ 对照", "传递回调时要显式处理接收者，不能假设方法会记住对象。");

    const counter: CounterLike = {
        count: 0,
        increment() {
            this.count += 1;
            return this.count;
        }
    };

    const callbacks = [
        () => `wrapped:${counter.increment()}`,
        counter.increment.bind(counter),
        () => `after-bind-count:${counter.count}`
    ].map((callback) => () => String(callback()));

    showJson("回调 this 安全示例", {
        results: runCallbacks(callbacks),
        finalCount: counter.count
    });
    note("输出解释", "箭头包装和 bind 都能避免 this 丢失；直接传 counter.increment 会出错。");
}

// =============================================================================
// 8.10 本章复盘
// =============================================================================
//
// C++ 对照：
//   原型链和 this 是 JS 运行时的核心语义，不能套用 C++ 类继承和成员函数模型。

function demoChapterReview(): void {
    section("8.10 本章复盘");
    note("C++ 对照", "原型链是对象委托，this 由调用方式决定；这两点是 JS 和 C++ OOP 的根本差异。");

    const review = [
        "属性查找先看自有属性，再沿 [[Prototype]] 链查找",
        "__proto__ 是历史访问器，新代码优先 Object.getPrototypeOf",
        "函数的 prototype 用于 new 创建对象时设置实例原型",
        "this 看调用方式，不看函数声明位置",
        "obj.method() 是隐式绑定，拆成变量会丢失 this",
        "call/apply/bind 可以显式指定 this",
        "class 语法仍建立在原型机制之上",
        "箭头函数没有自己的 this，不适合作为需要动态接收者的方法"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 8. */
export function runChapter(): void {
    demoPrototypeChainLookup();
    demoPrototypeTerms();
    demoObjectCreate();
    demoThisBindingOverview();
    demoLostImplicitBinding();
    demoCallApplyBind();
    demoNewBinding();
    demoArrowThis();
    demoCallbackThisSafety();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
