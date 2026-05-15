import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export function hello(name: string): string {
    return `Hello, ${name}. Welcome to TypeScript 6.0.`;
}

export function sumNumbers(values: readonly number[]): number {
    return values.reduce((total, value) => total + value, 0);
}

export function describeNumber(value: number): string {
    if (Number.isNaN(value)) {
        return "not-a-number";
    }
    if (!Number.isFinite(value)) {
        return "infinite";
    }
    return Number.isInteger(value) ? "integer" : "floating-point";
}

export function runChapter(): void {
    section("第 1 章：TypeScript 入门与环境");
    note("C++ 对照", "TS 源码先经过 tsc 做类型检查和转译，真正运行的是 JavaScript。");
    note("版本边界", "本教程使用 TypeScript 6.0.x，不默认采用 TypeScript 7 Beta 行为。");

    console.log(hello("C++ developer"));
    showJson("数字分类", [
        describeNumber(42),
        describeNumber(3.14),
        describeNumber(Number.NaN),
        describeNumber(Number.POSITIVE_INFINITY)
    ]);
    console.log(`sumNumbers([1, 2, 3]) = ${sumNumbers([1, 2, 3])}`);
}

await runIfMain(import.meta.url, runChapter);
