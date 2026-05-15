import assert from "node:assert/strict";
import test from "node:test";
import { cartTotal, clamp, extractNumbers, fizzBuzz } from "../练习题与答案/exercises_01_基础.js";
import { normalizeCompilerOptions, safeParseJson, totalShapeArea } from "../练习题与答案/exercises_02_类型系统.js";
import { groupBy, MemoryRepository, Stack } from "../练习题与答案/exercises_03_泛型与OOP.js";
import { summarizeLogText } from "../练习题与答案/exercises_04_综合.js";

test("basic exercises", () => {
    assert.equal(clamp(20, 0, 10), 10);
    assert.deepEqual(extractNumbers("x=-2 y=3.5"), [-2, 3.5]);
    assert.equal(fizzBuzz(15).at(-1), "FizzBuzz");
    assert.equal(cartTotal([{ name: "book", price: 12, quantity: 3 }]), 36);
});

test("type system exercises", () => {
    assert.deepEqual(normalizeCompilerOptions({}), {
        strict: true,
        target: "ES2023",
        module: "NodeNext"
    });
    assert.equal(safeParseJson("{\"ok\":true}").ok, true);
    assert.equal(totalShapeArea([{ kind: "rectangle", width: 2, height: 5 }]), 10);
});

test("generic and oop exercises", () => {
    const stack = new Stack<number>();
    stack.push(1);
    stack.push(2);
    assert.equal(stack.pop(), 2);

    assert.deepEqual(groupBy(["ts", "js", "cpp"], (item) => item.length), {
        2: ["ts", "js"],
        3: ["cpp"]
    });

    const repo = new MemoryRepository<{ id: string; name: string }>();
    repo.upsert({ id: "u1", name: "Ada" });
    assert.equal(repo.find("u1")?.name, "Ada");
});

test("integrated log exercise", () => {
    const summary = summarizeLogText([
        '127.0.0.1 - - [15/May/2026:10:00:00 +0800] "GET / HTTP/1.1" 200 10 "curl"',
        '127.0.0.1 - - [15/May/2026:10:00:01 +0800] "GET /missing HTTP/1.1" 404 5 "curl"'
    ].join("\n"));
    assert.equal(summary.totalRequests, 2);
    assert.equal(summary.statusCounts["4xx"], 1);
});
