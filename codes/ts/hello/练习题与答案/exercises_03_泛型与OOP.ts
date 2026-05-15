import { runIfMain, section, showJson } from "../shared/chapter.js";

export class Stack<T> {
    readonly #values: T[] = [];

    push(value: T): void {
        this.#values.push(value);
    }

    pop(): T | undefined {
        return this.#values.pop();
    }

    get size(): number {
        return this.#values.length;
    }
}

export function groupBy<T, K extends string | number | symbol>(
    values: readonly T[],
    keySelector: (value: T) => K
): Record<K, T[]> {
    const result = {} as Record<K, T[]>;
    for (const value of values) {
        const key = keySelector(value);
        result[key] ??= [];
        result[key].push(value);
    }
    return result;
}

export class MemoryRepository<T extends { id: string }> {
    readonly #items = new Map<string, T>();

    upsert(item: T): void {
        this.#items.set(item.id, item);
    }

    find(id: string): T | undefined {
        return this.#items.get(id);
    }
}

export function runExercises(): void {
    section("练习 03：泛型与 OOP");
    const stack = new Stack<number>();
    stack.push(1);
    stack.push(2);
    showJson("stack", { size: stack.size, pop: stack.pop() });
    showJson("groupBy", groupBy(["ts", "cpp", "js"], (item) => item.length));
}

await runIfMain(import.meta.url, runExercises);
