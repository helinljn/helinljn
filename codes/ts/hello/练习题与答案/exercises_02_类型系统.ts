// =============================================================================
// 练习 02：类型系统
// =============================================================================
//
// 覆盖章节：ch09-ch13
//
// 目标：
//   1. 练习可选属性、默认值、联合类型和类型收窄
//   2. 把 JSON.parse 的 unknown 边界显式表达为 Result
//   3. 复用章节中的 Shape 联合类型完成类型安全聚合
//
// =============================================================================

import { area, type Shape } from "../03_类型系统篇/chapter10_联合类型与收窄.js";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

/** Raw compiler options as they might arrive from a config file. */
export type CompilerOptionsInput = {
    strict?: boolean;
    target?: string;
    module?: string;
};

/** Fully normalized compiler options used by the application. */
export type NormalizedCompilerOptions = {
    strict: boolean;
    target: string;
    module: string;
};

/** Runtime parse result that keeps success and failure paths explicit. */
export type SafeParseResult<T> =
    | { ok: true; value: T }
    | { ok: false; error: string };

const DEFAULT_COMPILER_OPTIONS: NormalizedCompilerOptions = {
    strict: true,
    target: "ES2023",
    module: "NodeNext"
};

// =============================================================================
// 02.1 可选属性与默认值：normalizeCompilerOptions
// =============================================================================

/** Merge partial compiler options with project defaults. */
export function normalizeCompilerOptions(input: CompilerOptionsInput): NormalizedCompilerOptions {
    return {
        strict: input.strict ?? DEFAULT_COMPILER_OPTIONS.strict,
        target: input.target ?? DEFAULT_COMPILER_OPTIONS.target,
        module: input.module ?? DEFAULT_COMPILER_OPTIONS.module
    };
}

// =============================================================================
// 02.2 unknown 边界：safeParseJson
// =============================================================================

/** Parse JSON into unknown and keep parse errors as values. */
export function safeParseJson(text: string): SafeParseResult<unknown> {
    try {
        return { ok: true, value: JSON.parse(text) as unknown };
    } catch (error) {
        return { ok: false, error: error instanceof Error ? error.message : String(error) };
    }
}

// =============================================================================
// 02.3 联合类型复用：totalShapeArea
// =============================================================================

/** Sum the area of a readonly list of chapter Shape values. */
export function totalShapeArea(shapes: readonly Shape[]): number {
    return shapes.reduce((total, shape) => total + area(shape), 0);
}

function demoCompilerOptionExercise(): void {
    section("练习 02.1：配置默认值");
    note("要求", "用 ?? 区分“没有传”和 falsy 值，避免 strict=false 被误覆盖。");
    showJson("normalizeCompilerOptions", [
        normalizeCompilerOptions({}),
        normalizeCompilerOptions({ strict: false, target: "ES2022" })
    ]);
}

function demoRuntimeBoundaryExercise(): void {
    section("练习 02.2：JSON runtime boundary");
    note("要求", "JSON.parse 的结果是 unknown，后续必须用 type guard 或 validator 收窄。");
    showJson("safeParseJson", [
        safeParseJson("{\"ok\":true}"),
        safeParseJson("{oops")
    ]);
}

function demoUnionExercise(): void {
    section("练习 02.3：联合类型聚合");
    note("要求", "复用章节中的 Shape 联合类型，新增成员时让 area 的 exhaustiveness check 兜底。");
    showJson("totalShapeArea", {
        total: totalShapeArea([
            { kind: "rectangle", width: 2, height: 5 },
            { kind: "circle", radius: 1 }
        ])
    });
}

/** Run type system exercises. */
export function runExercises(): void {
    section("练习 02：类型系统");
    demoCompilerOptionExercise();
    demoRuntimeBoundaryExercise();
    demoUnionExercise();
}

await runIfMain(import.meta.url, runExercises);
