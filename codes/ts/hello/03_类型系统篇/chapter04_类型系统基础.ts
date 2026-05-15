import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type Circle = {
    kind: "circle";
    radius: number;
};

export type Rectangle = {
    kind: "rectangle";
    width: number;
    height: number;
};

export type Shape = Circle | Rectangle;

export function area(shape: Shape): number {
    switch (shape.kind) {
        case "circle":
            return Math.PI * shape.radius * shape.radius;
        case "rectangle":
            return shape.width * shape.height;
        default: {
            const unreachable: never = shape;
            return unreachable;
        }
    }
}

export function normalizeId(id: string | number): string {
    return typeof id === "number" ? `#${id.toString(10)}` : id.trim();
}

export function hasName(value: unknown): value is { name: string } {
    return typeof value === "object"
        && value !== null
        && "name" in value
        && typeof (value as { name?: unknown }).name === "string";
}

export function runChapter(): void {
    section("第 4 章：TypeScript 类型系统基础");
    note("C++ 对照", "TS 使用结构化类型，形状兼容即可赋值，不要求显式继承同一个基类。");
    note("关键能力", "联合类型加类型收窄是 TS 日常代码的核心，不是 C++ enum class 的简单替代。");

    showJson("面积", [
        area({ kind: "circle", radius: 2 }),
        area({ kind: "rectangle", width: 3, height: 4 })
    ]);
    console.log(normalizeId(42));
    console.log(hasName({ name: "Ada" }) ? "has name" : "no name");
}

await runIfMain(import.meta.url, runChapter);
