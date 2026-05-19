// =============================================================================
// 第 17 章：继承与多态
// =============================================================================
//
// 【学习目标】
//   1. 掌握 extends、super、override 和抽象类
//   2. 理解 implements 多个接口与结构化类型多态
//   3. 掌握 Mixin 模式的基本写法与边界
//
// 【运行方式】
//   npm run build
//   node dist/05_面向对象篇/chapter17_继承与多态.js
//   或 npm run chapter -- 17（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

interface Renderable {
    render(): string;
}

interface Measurable {
    area(): number;
}

interface Movable {
    moveBy(deltaX: number, deltaY: number): void;
}

interface Serializable {
    toJSON(): Record<string, unknown>;
}

type Constructor<T = object> = new (...args: any[]) => T;

abstract class Shape implements Renderable, Measurable {
    protected constructor(
        public readonly id: string,
        protected readonly kind: string
    ) {}

    protected baseLabel(): string {
        return `${this.kind}#${this.id}`;
    }

    abstract area(): number;
    abstract render(): string;
}

class Rectangle extends Shape implements Movable, Serializable {
    constructor(
        id: string,
        public x: number,
        public y: number,
        public width: number,
        public height: number
    ) {
        super(id, "rectangle");
    }

    override area(): number {
        return this.width * this.height;
    }

    override render(): string {
        return `${super.baseLabel()} at (${this.x},${this.y}) ${this.width}x${this.height}`;
    }

    moveBy(deltaX: number, deltaY: number): void {
        this.x += deltaX;
        this.y += deltaY;
    }

    toJSON(): Record<string, unknown> {
        return {
            id: this.id,
            kind: "rectangle",
            x: this.x,
            y: this.y,
            width: this.width,
            height: this.height,
            area: this.area()
        };
    }
}

class Circle extends Shape implements Movable, Serializable {
    constructor(
        id: string,
        public x: number,
        public y: number,
        public radius: number
    ) {
        super(id, "circle");
    }

    override area(): number {
        return Number((Math.PI * this.radius * this.radius).toFixed(2));
    }

    override render(): string {
        return `${super.baseLabel()} at (${this.x},${this.y}) radius=${this.radius}`;
    }

    moveBy(deltaX: number, deltaY: number): void {
        this.x += deltaX;
        this.y += deltaY;
    }

    toJSON(): Record<string, unknown> {
        return {
            id: this.id,
            kind: "circle",
            x: this.x,
            y: this.y,
            radius: this.radius,
            area: this.area()
        };
    }
}

class NoteBase {
    constructor(
        public readonly id: string,
        public title: string
    ) {}
}

function Timestamped<TBase extends Constructor>(Base: TBase) {
    return class extends Base {
        readonly createdAt = "2026-05-18T00:00:00.000Z";
        updatedAt = "2026-05-18T00:00:00.000Z";

        touch(): void {
            this.updatedAt = "2026-05-18T01:00:00.000Z";
        }
    };
}

class AuditNote extends Timestamped(NoteBase) implements Renderable, Serializable {
    constructor(id: string, title: string, public body: string) {
        super(id, title);
    }

    render(): string {
        return `${this.title}: ${this.body}`;
    }

    toJSON(): Record<string, unknown> {
        return {
            id: this.id,
            title: this.title,
            body: this.body,
            createdAt: this.createdAt,
            updatedAt: this.updatedAt
        };
    }
}

function renderAll(items: readonly Renderable[]): string[] {
    return items.map((item) => item.render());
}

function serializeAll(items: readonly Serializable[]): Record<string, unknown>[] {
    return items.map((item) => item.toJSON());
}

function moveThenRender<T extends Renderable & Movable>(
    items: readonly T[],
    deltaX: number,
    deltaY: number
): string[] {
    for (const item of items) {
        item.moveBy(deltaX, deltaY);
    }
    return renderAll(items);
}

// =============================================================================
// 17.1 extends、super 和 override
// =============================================================================
//
// C++ 对照：
//   extends 是单继承；super 调用父类构造器或父类方法。
//   override 能让编译器检查“我确实在覆写父类成员”。
//
// 常见坑：
//   不写 override 时，父类方法改名后子类可能悄悄变成新方法。

function demoExtendsSuperOverride(): void {
    section("17.1 extends、super 和 override");
    note("C++ 对照", "TS/JS class 只有单继承；接口用于表达多种能力。");

    const rectangle = new Rectangle("r1", 0, 0, 3, 4);
    const circle = new Circle("c1", 5, 5, 2);
    rectangle.moveBy(2, 1);
    circle.moveBy(-1, 0);

    showJson("继承与覆写", {
        rectangle: rectangle.render(),
        rectangleArea: rectangle.area(),
        circle: circle.render(),
        circleArea: circle.area()
    });
    note("输出解释", "render() 里复用了 super.baseLabel()，area() 则由每个子类给出自己的实现。");
}

// =============================================================================
// 17.2 抽象类
// =============================================================================
//
// C++ 对照：
//   抽象类像带部分实现的基类。
//   它能共享代码，也能要求子类实现关键方法。

function demoAbstractClasses(): void {
    section("17.2 抽象类");
    note("C++ 对照", "抽象类适合共享状态和通用逻辑；纯能力描述优先用接口。");

    const shapes: Shape[] = [
        new Rectangle("r2", 1, 1, 10, 2),
        new Circle("c2", 0, 0, 3)
    ];

    showJson("抽象类多态", {
        rendered: renderAll(shapes),
        totalArea: Number(shapes.reduce((sum, shape) => sum + shape.area(), 0).toFixed(2))
    });
    note("输出解释", "Shape 不能直接实例化，但 Shape[] 可以持有所有子类实例。");
}

// =============================================================================
// 17.3 implements 多个接口
// =============================================================================
//
// C++ 对照：
//   TS 没有 C++ 多继承的对象布局问题。
//   一个类可以 implements 多个接口，因为接口只在类型层描述能力。

function demoMultipleInterfaces(): void {
    section("17.3 implements 多个接口");
    note("C++ 对照", "多个接口只是多组编译期能力约束，不会改变运行时对象结构。");

    const rectangle = new Rectangle("r3", 2, 3, 5, 6);
    const serializable: Serializable = rectangle;
    const movable: Movable = rectangle;
    movable.moveBy(1, 1);

    showJson("多个接口", {
        rendered: rectangle.render(),
        json: serializable.toJSON()
    });
    note("输出解释", "同一个 Rectangle 同时满足 Renderable、Measurable、Movable、Serializable。");
}

// =============================================================================
// 17.4 结构化类型多态
// =============================================================================
//
// C++ 对照：
//   TS 多态不要求共同基类或显式实现接口。
//   只要对象形状满足接口，就能作为该接口使用。

function demoStructuralPolymorphism(): void {
    section("17.4 结构化类型多态");
    note("C++ 对照", "结构化类型让普通对象也能参与多态；名字和继承关系不是默认门槛。");

    const adHocRenderable = {
        render() {
            return "plain object renderable";
        }
    };
    const items: Renderable[] = [
        new Rectangle("r4", 0, 0, 1, 2),
        new Circle("c4", 1, 1, 1),
        adHocRenderable
    ];

    showJson("结构化多态", renderAll(items));
    note("常见坑", "结构兼容很方便，但也容易让不同领域对象被误当成同一种能力；必要时加 brand 或更窄接口。");
}

// =============================================================================
// 17.5 Mixin 模式
// =============================================================================
//
// C++ 对照：
//   Mixin 不是 C++ 虚继承；它通常是“返回新子类的函数”。
//   适合把时间戳、日志、权限这类横切能力组合进类。

function demoMixinPattern(): void {
    section("17.5 Mixin 模式");
    note("C++ 对照", "Mixin 用函数组合类能力，避免单继承层级无限加深。");

    const auditNote = new AuditNote("n1", "Release", "ship TypeScript tutorial");
    auditNote.touch();

    showJson("Mixin 示例", {
        rendered: auditNote.render(),
        json: auditNote.toJSON()
    });
    note("输出解释", "Timestamped(NoteBase) 返回一个带 createdAt/updatedAt/touch 的新基类。");
}

// =============================================================================
// 17.6 工程场景：可渲染画布
// =============================================================================
//
// C++ 对照：
//   面向接口写代码可以让渲染、序列化、移动等能力分离。
//   这比把所有方法塞进一个巨大基类更灵活。

function demoCanvasScenario(): void {
    section("17.6 工程场景：可渲染画布");
    note("C++ 对照", "单继承表达核心层级，多接口表达横向能力。");

    const items = [
        new Rectangle("r5", 0, 0, 8, 3),
        new Circle("c5", 4, 4, 2)
    ];

    showJson("画布场景", {
        rendered: renderAll(items),
        serialized: serializeAll(items)
    });
    note("输出解释", "渲染器只需要 Renderable；保存逻辑只需要 Serializable。");
}

// =============================================================================
// 17.7 设计边界：继承表达核心层级，接口表达能力组合
// =============================================================================
//
// C++ 对照：
//   C++ 项目里很容易通过继承层级表达所有关系，最后得到深而脆的基类树。
//   TS 里接口没有运行时布局成本，更适合表达“这个对象能做什么”。
//
// 真实场景：
//   渲染、移动、序列化、审计这些能力经常横跨多个领域对象。
//   让函数依赖最小接口，可以避免为了复用一个函数而强迫所有对象继承同一个基类。
//
// 常见坑：
//   看到两个类都有 render() 就抽一个巨大父类，通常会把无关状态也绑在一起。
//   先问调用方需要哪些方法，再决定用基类、接口还是简单对象。

function demoInheritanceVsCompositionBoundary(): void {
    section("17.7 设计边界：继承 vs 能力组合");
    note("C++ 对照", "接口表达横向能力，能让函数少依赖具体基类。");

    const movableItems = [
        new Rectangle("r6", 0, 0, 2, 2),
        new Circle("c6", 3, 3, 1)
    ];
    const auditNote = new AuditNote("n2", "Audit", "renderable but not movable");

    showJson("继承与组合边界", {
        moved: moveThenRender(movableItems, 10, 0),
        renderedOnly: renderAll([auditNote]),
        serializedShapes: serializeAll(movableItems)
    });
    note("输出解释", "moveThenRender 只要求 Renderable & Movable；AuditNote 能渲染但不能移动，因此不传给移动流程。");
    note("常见坑", "不要为了复用函数强行扩大基类；让函数依赖最小能力接口通常更稳。");
}

// =============================================================================
// 17.8 本章复盘
// =============================================================================
//
// C++ 对照：
//   TS 更鼓励单继承 + 多接口 + 结构化多态。
//   遇到横切能力时，Mixin 通常比深继承链更实际。

function demoChapterReview(): void {
    section("17.8 本章复盘");
    note("C++ 对照", "TS 没有 C++ 虚继承问题；接口不参与运行时对象布局。");

    const summary = [
        "extends 是单继承，super 用于调用父类逻辑",
        "override 能让覆写更安全",
        "抽象类适合共享状态和部分实现",
        "implements 多接口只做编译期形状检查",
        "结构化类型让普通对象也能参与多态",
        "Mixin 用类工厂组合横切能力",
        "函数应依赖最小能力接口，而不是默认依赖巨大基类"
    ];

    showJson("关键结论", summary);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 17. */
export function runChapter(): void {
    demoExtendsSuperOverride();
    demoAbstractClasses();
    demoMultipleInterfaces();
    demoStructuralPolymorphism();
    demoMixinPattern();
    demoCanvasScenario();
    demoInheritanceVsCompositionBoundary();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
