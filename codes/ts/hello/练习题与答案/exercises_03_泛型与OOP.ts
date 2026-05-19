// =============================================================================
// 练习 03：泛型与 OOP
// =============================================================================
//
// 覆盖章节：ch12、ch16-ch18
//
// 目标：
//   1. 用泛型实现 Stack<T>、groupBy<T, K> 和内存仓储
//   2. 练习 readonly、私有字段、约束泛型和返回 undefined 的边界
//   3. 把面向对象封装控制在有真实状态和生命周期的地方
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

/** Generic LIFO stack implemented with a private array. */
export class Stack<T> {
    readonly #values: T[] = [];

    /** Push a value to the top of the stack. */
    push(value: T): void {
        this.#values.push(value);
    }

    /** Remove and return the top value, or undefined when the stack is empty. */
    pop(): T | undefined {
        return this.#values.pop();
    }

    /** Read the top value without removing it. */
    peek(): T | undefined {
        return this.#values.at(-1);
    }

    /** Current stack size. */
    get size(): number {
        return this.#values.length;
    }

    /** Snapshot values in insertion order for tests and demos. */
    toArray(): T[] {
        return [...this.#values];
    }
}

// =============================================================================
// 03.1 泛型函数：groupBy
// =============================================================================

/** Group values by a string/number/symbol key. */
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

// =============================================================================
// 03.2 约束泛型：MemoryRepository
// =============================================================================

/** In-memory repository for items that have a stable string id. */
export class MemoryRepository<T extends { id: string }> {
    readonly #items = new Map<string, T>();

    /** Insert or replace an item by id. */
    upsert(item: T): void {
        this.#items.set(item.id, item);
    }

    /** Find an item by id, or undefined when missing. */
    find(id: string): T | undefined {
        return this.#items.get(id);
    }

    /** Delete an item and report whether it existed. */
    delete(id: string): boolean {
        return this.#items.delete(id);
    }

    /** Return all items in insertion order. */
    list(): T[] {
        return [...this.#items.values()];
    }
}

function demoStackExercise(): void {
    section("练习 03.1：Stack<T>");
    note("要求", "pop/peek 必须返回 T | undefined，调用方要处理空栈。");
    const stack = new Stack<number>();
    stack.push(1);
    stack.push(2);
    showJson("stack", {
        before: stack.toArray(),
        peek: stack.peek(),
        pop: stack.pop(),
        afterSize: stack.size
    });
}

function demoGroupByExercise(): void {
    section("练习 03.2：groupBy<T, K>");
    note("要求", "keySelector 决定分组键，返回类型保留 key 的 string/number/symbol 约束。");
    showJson("groupBy", groupBy(["ts", "cpp", "js"], (item) => item.length));
}

function demoRepositoryExercise(): void {
    section("练习 03.3：MemoryRepository<T extends { id: string }>");
    note("要求", "仓储封装状态；领域对象仍是普通结构化数据。");
    const repo = new MemoryRepository<{ id: string; name: string }>();
    repo.upsert({ id: "u1", name: "Ada" });
    repo.upsert({ id: "u2", name: "Grace" });
    showJson("repository", {
        found: repo.find("u1"),
        missing: repo.find("missing"),
        all: repo.list()
    });
}

/** Run generic and OOP exercises. */
export function runExercises(): void {
    section("练习 03：泛型与 OOP");
    demoStackExercise();
    demoGroupByExercise();
    demoRepositoryExercise();
}

await runIfMain(import.meta.url, runExercises);
