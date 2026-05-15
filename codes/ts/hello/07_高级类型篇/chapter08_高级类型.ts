import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type Mutable<T> = {
    -readonly [K in keyof T]: T[K];
};

export type ApiResponse<T> =
    | { status: "ok"; data: T }
    | { status: "error"; message: string };

export type AwaitedValue<T> = T extends Promise<infer U> ? U : T;

export function pick<T extends object, const K extends readonly (keyof T)[]>(
    source: T,
    keys: K
): Pick<T, K[number]> {
    const result = {} as Pick<T, K[number]>;
    for (const key of keys) {
        result[key] = source[key];
    }
    return result;
}

export type EventMap = {
    userCreated: { id: string; email: string };
    buildFinished: { ok: boolean; durationMs: number };
};

export function makeEvent<K extends keyof EventMap>(name: K, payload: EventMap[K]): {
    name: K;
    payload: EventMap[K];
} {
    return { name, payload };
}

export function runChapter(): void {
    section("第 8 章：高级类型工具");
    note("C++ 对照", "高级类型运行在编译期，但不会像模板元编程那样生成运行时代码。");
    note("实践建议", "高级类型用于表达边界和消除重复，不要为了炫技让 API 难以阅读。");

    const user = { id: "u1", email: "a@example.test", active: true };
    showJson("pick", pick(user, ["id", "active"] as const));
    showJson("event", makeEvent("buildFinished", { ok: true, durationMs: 125 }));
}

await runIfMain(import.meta.url, runChapter);
