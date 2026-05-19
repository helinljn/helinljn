import assert from "node:assert/strict";
import { mkdir, mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { parseFileBatchArgs } from "../11_项目实战/chapter39_项目1_文件批处理CLI/config.js";
import { runFileBatch } from "../11_项目实战/chapter39_项目1_文件批处理CLI/fileProcessor.js";
import { runChapter as runChapter39 } from "../11_项目实战/chapter39_项目1_文件批处理CLI/main.js";
import { summarizeLogs } from "../11_项目实战/chapter40_项目2_日志分析CLI/analyzer.js";
import { parseLogLine } from "../11_项目实战/chapter40_项目2_日志分析CLI/logParser.js";
import { formatSummary } from "../11_项目实战/chapter40_项目2_日志分析CLI/reporter.js";
import { parseLogAnalyzerArgs, runChapter as runChapter40 } from "../11_项目实战/chapter40_项目2_日志分析CLI/main.js";

test("file batch project converts text files", async () => {
    const temp = await mkdtemp(join(tmpdir(), "ts-project-test-"));
    try {
        const input = join(temp, "in");
        const output = join(temp, "out");
        await mkdir(input, { recursive: true });
        await writeFile(join(input, "a.txt"), "Hello", { encoding: "utf8", flag: "w" });

        const options = parseFileBatchArgs(["--input", input, "--output", output, "--mode", "lower"]);
        const summary = await runFileBatch(options);
        assert.equal(summary.filesProcessed, 1);
        assert.equal(await readFile(join(output, "a.txt"), "utf8"), "hello");
    } finally {
        await rm(temp, { recursive: true, force: true });
    }
});

test("file batch project chapter entry runs as a repeatable demo", async () => {
    await runChapter39();
});

test("log analyzer project parses arguments and summarizes", () => {
    const args = parseLogAnalyzerArgs(["--log", "access.log", "--format", "json", "--top", "3"]);
    assert.deepEqual(args, { log: "access.log", format: "json", top: 3 });

    const line = '127.0.0.1 - - [15/May/2026:10:00:00 +0800] "GET /api HTTP/1.1" 200 100 "curl"';
    const entry = parseLogLine(line);
    assert.ok(entry);
    const summary = summarizeLogs([entry]);
    assert.equal(summary.totalRequests, 1);
    assert.equal(summary.statusCounts["2xx"], 1);
    assert.match(formatSummary(summary, "text"), /Total requests: 1/);
});

test("log analyzer project chapter entry runs as a repeatable demo", async () => {
    await runChapter40();
});
