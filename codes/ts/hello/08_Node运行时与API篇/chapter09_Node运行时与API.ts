import { basename, dirname, extname, join, normalize } from "node:path";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type PathSummary = {
    input: string;
    normalized: string;
    directory: string;
    base: string;
    extension: string;
};

export function toUtf8Bytes(text: string): Uint8Array {
    return Buffer.from(text, "utf8");
}

export function summarizePath(input: string): PathSummary {
    return {
        input,
        normalized: normalize(input),
        directory: dirname(input),
        base: basename(input),
        extension: extname(input)
    };
}

export function parseJsonObject(text: string): Record<string, unknown> {
    const value = JSON.parse(text) as unknown;
    if (typeof value !== "object" || value === null || Array.isArray(value)) {
        throw new TypeError("expected a JSON object");
    }
    return value as Record<string, unknown>;
}

export function runChapter(): void {
    section("第 9 章：Node 运行时与常用 API");
    note("边界澄清", "文件、路径、进程、Buffer、网络模块来自 Node.js，不来自 TypeScript。");
    note("类型声明", "`@types/node` 提供 Node API 的类型；实现由本机 Node 运行时提供。");

    showJson("路径信息", summarizePath(join("src", "demo", "app.ts")));
    showJson("UTF-8 字节", [...toUtf8Bytes("TypeScript")]);
    showJson("JSON 对象", parseJsonObject("{\"strict\":true,\"target\":\"ES2023\"}"));
}

await runIfMain(import.meta.url, runChapter);
