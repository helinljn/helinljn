import assert from "node:assert/strict";
import { mkdir, mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import test from "node:test";
import { parseFileBatchArgs } from "../11_项目实战/chapter12_项目1_文件批处理工具/config.js";
import { runFileBatch } from "../11_项目实战/chapter12_项目1_文件批处理工具/fileProcessor.js";
import { summarizeLogs } from "../11_项目实战/chapter13_项目2_日志分析工具/analyzer.js";
import { parseLogLine } from "../11_项目实战/chapter13_项目2_日志分析工具/logParser.js";
import { formatSummary } from "../11_项目实战/chapter13_项目2_日志分析工具/reporter.js";

test("file batch project converts text files", async () => {
  const temp = await mkdtemp(join(tmpdir(), "ts-project-test-"));
  try {
    const input = join(temp, "in");
    const output = join(temp, "out");
    await mkdir(input, { recursive: true });
    await writeFile(join(input, "a.txt"), "Hello", { encoding: "utf8", flag: "w" }).catch(async () => {
      await rm(temp, { recursive: true, force: true });
      throw new Error("failed to create test input");
    });

    const options = parseFileBatchArgs(["--input", input, "--output", output, "--mode", "lower"]);
    const summary = await runFileBatch(options);
    assert.equal(summary.filesProcessed, 1);
    assert.equal(await readFile(join(output, "a.txt"), "utf8"), "hello");
  } finally {
    await rm(temp, { recursive: true, force: true });
  }
});

test("log analyzer project parses and summarizes", () => {
  const line = '127.0.0.1 - - [15/May/2026:10:00:00 +0800] "GET /api HTTP/1.1" 200 100 "curl"';
  const entry = parseLogLine(line);
  assert.ok(entry);
  const summary = summarizeLogs([entry]);
  assert.equal(summary.totalRequests, 1);
  assert.equal(summary.statusCounts["2xx"], 1);
  assert.match(formatSummary(summary, "text"), /Total requests: 1/);
});
