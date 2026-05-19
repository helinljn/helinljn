import assert from "node:assert/strict";
import { existsSync } from "node:fs";
import { resolve } from "node:path";
import test from "node:test";
import { CHAPTERS } from "../chapterRegistry.js";

const EXPECTED_PHASES = new Set([
    "基础",
    "JS运行时",
    "类型系统",
    "函数",
    "面向对象",
    "模块与工程",
    "迭代器与生成器",
    "异步编程",
    "Node API",
    "工程实践",
    "项目实战",
    "练习"
]);

const teachingChapters = CHAPTERS.filter((chapter) => chapter.phase !== "练习");
const exerciseEntries = CHAPTERS.filter((chapter) => chapter.phase === "练习");

test("chapter registry contains 40 teaching chapters and 4 exercise entries", () => {
    assert.equal(CHAPTERS.length, 44);
    assert.equal(teachingChapters.length, 40);
    assert.equal(exerciseEntries.length, 4);

    const teachingIds = new Set(teachingChapters.map((chapter) => chapter.id));
    for (let id = 1; id <= 40; id += 1) {
        assert.ok(teachingIds.has(id), `missing teaching chapter ${id}`);
    }

    for (const chapter of CHAPTERS) {
        assert.ok(EXPECTED_PHASES.has(chapter.phase), `unexpected phase ${chapter.phase}`);
        assert.ok(chapter.path.endsWith(".ts"), `${chapter.path} should be a TypeScript source path`);
        assert.ok(existsSync(resolve(chapter.path)), `${chapter.path} should exist`);
    }

    assert.equal(CHAPTERS.find((chapter) => chapter.id === 39)?.path, "11_项目实战/chapter39_项目1_文件批处理CLI/main.ts");
    assert.equal(CHAPTERS.find((chapter) => chapter.id === 40)?.path, "11_项目实战/chapter40_项目2_日志分析CLI/main.ts");
});

for (const chapter of teachingChapters) {
    test(`${chapter.title} runs`, async () => {
        await chapter.run();
    });
}
