import { resolve } from "node:path";
import { pathToFileURL } from "node:url";

/** A value that may be produced synchronously or by a Promise-returning chapter. */
export type MaybePromise<T> = T | Promise<T>;

/** Runtime metadata used by main.ts to list and execute tutorial chapters. */
export type Chapter = {
    id: number;
    title: string;
    phase: string;
    path: string;
    run: () => MaybePromise<void>;
};

/** Print a visible chapter or demo section divider. */
export function section(title: string): void {
    console.log();
    console.log("=".repeat(72));
    console.log(title);
    console.log("=".repeat(72));
}

/** Print a short labelled teaching note. */
export function note(label: string, text: string): void {
    console.log(`[${label}] ${text}`);
}

/** Print stable, indented JSON for demo values. */
export function showJson(label: string, value: unknown): void {
    console.log(`${label}:`);
    console.log(JSON.stringify(value, null, 2));
}

/** Return true when a module URL matches the current Node entry script. */
export function isDirectRun(metaUrl: string): boolean {
    const entry = process.argv[1];
    if (entry === undefined) {
        return false;
    }
    return pathToFileURL(resolve(entry)).href === metaUrl;
}

/** Run a chapter only when its module is executed directly from Node. */
export async function runIfMain(metaUrl: string, run: () => MaybePromise<void>): Promise<void> {
    if (isDirectRun(metaUrl)) {
        await run();
    }
}
