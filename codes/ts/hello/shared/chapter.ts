import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

export type MaybePromise<T> = T | Promise<T>;

export type Chapter = {
    id: number;
    title: string;
    phase: string;
    path: string;
    run: () => MaybePromise<void>;
};

export function section(title: string): void {
    console.log();
    console.log("=".repeat(72));
    console.log(title);
    console.log("=".repeat(72));
}

export function note(label: string, text: string): void {
    console.log(`[${label}] ${text}`);
}

export function showJson(label: string, value: unknown): void {
    console.log(`${label}:`);
    console.log(JSON.stringify(value, null, 2));
}

export function isDirectRun(metaUrl: string): boolean {
    const entry = process.argv[1];
    if (entry === undefined) {
        return false;
    }
    return pathToFileURL(resolve(entry)).href === metaUrl;
}

export async function runIfMain(metaUrl: string, run: () => MaybePromise<void>): Promise<void> {
    if (isDirectRun(metaUrl)) {
        await run();
    }
}
