import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type Result<T, E = string> =
    | { ok: true; value: T }
    | { ok: false; error: E };

export function first<T>(values: readonly T[]): T | undefined {
    return values[0];
}

export function mapValues<T, U>(values: readonly T[], mapper: (value: T, index: number) => U): U[] {
    return values.map(mapper);
}

export function parseInteger(text: string): Result<number> {
    const value = Number.parseInt(text, 10);
    if (Number.isNaN(value)) {
        return { ok: false, error: `cannot parse integer: ${text}` };
    }
    return { ok: true, value };
}

export function formatId(id: number): string;
export function formatId(id: string): string;
export function formatId(id: number | string): string {
    return typeof id === "number" ? `id-${id.toString(10)}` : id;
}

export function runChapter(): void {
    section("第 5 章：函数、重载与泛型");
    note("C++ 对照", "TS 泛型主要用于类型约束和推导，运行时不会生成模板特化代码。");
    note("实践建议", "优先让编译器推导泛型参数，只有 API 边界不清楚时再显式标注。");

    showJson("mapValues", mapValues([1, 2, 3], (value) => value * value));
    showJson("parseInteger", [parseInteger("42"), parseInteger("oops")]);
    console.log(formatId(7));
    console.log(first(["ts", "js"]));
}

await runIfMain(import.meta.url, runChapter);
