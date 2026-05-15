import { area, type Shape } from "../03_类型系统篇/chapter04_类型系统基础.js";
import { runIfMain, section, showJson } from "../shared/chapter.js";

export type CompilerOptionsInput = {
  strict?: boolean;
  target?: string;
  module?: string;
};

export type NormalizedCompilerOptions = {
  strict: boolean;
  target: string;
  module: string;
};

export type SafeParseResult<T> =
  | { ok: true; value: T }
  | { ok: false; error: string };

export function normalizeCompilerOptions(input: CompilerOptionsInput): NormalizedCompilerOptions {
  return {
    strict: input.strict ?? true,
    target: input.target ?? "ES2023",
    module: input.module ?? "NodeNext"
  };
}

export function safeParseJson(text: string): SafeParseResult<unknown> {
  try {
    return { ok: true, value: JSON.parse(text) as unknown };
  } catch (error) {
    return { ok: false, error: error instanceof Error ? error.message : String(error) };
  }
}

export function totalShapeArea(shapes: readonly Shape[]): number {
  return shapes.reduce((total, shape) => total + area(shape), 0);
}

export function runExercises(): void {
  section("练习 02：类型系统");
  showJson("normalizeCompilerOptions", normalizeCompilerOptions({}));
  showJson("safeParseJson", [safeParseJson("{\"ok\":true}"), safeParseJson("{oops")]);
  console.log(totalShapeArea([{ kind: "rectangle", width: 2, height: 3 }]));
}

await runIfMain(import.meta.url, runExercises);
