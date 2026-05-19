// =============================================================================
// 练习 04：综合练习
// =============================================================================
//
// 覆盖章节：ch23-ch40
//
// 目标：
//   1. 复用项目 2 的日志解析和聚合模块
//   2. 练习“解析失败不中断全量处理”的工程边界
//   3. 把综合任务拆成可测试的 parseManyLogLines 和 summarizeLogText
//
// =============================================================================

import { summarizeLogs } from "../11_项目实战/chapter40_项目2_日志分析CLI/analyzer.js";
import { parseLogLine } from "../11_项目实战/chapter40_项目2_日志分析CLI/logParser.js";
import type { LogEntry, LogSummary } from "../11_项目实战/chapter40_项目2_日志分析CLI/types.js";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type ParseManyResult = {
    entries: LogEntry[];
    invalidLines: string[];
};

function parseManyWithErrors(lines: readonly string[]): ParseManyResult {
    const entries: LogEntry[] = [];
    const invalidLines: string[] = [];

    for (const line of lines) {
        if (line.trim() === "") {
            continue;
        }
        const entry = parseLogLine(line);
        if (entry === null) {
            invalidLines.push(line);
        } else {
            entries.push(entry);
        }
    }

    return { entries, invalidLines };
}

// =============================================================================
// 04.1 解析多行日志
// =============================================================================

/** Parse all valid log lines and skip invalid lines. */
export function parseManyLogLines(lines: readonly string[]): LogEntry[] {
    return parseManyWithErrors(lines).entries;
}

// =============================================================================
// 04.2 汇总日志文本
// =============================================================================

/** Summarize nginx-style access log text using the project 2 analyzer. */
export function summarizeLogText(text: string, top = 5): LogSummary {
    const { entries } = parseManyWithErrors(text.split(/\r?\n/));
    return summarizeLogs(entries, top);
}

function demoIntegratedLogExercise(): void {
    section("练习 04.1：日志解析与聚合");
    note("要求", "解析失败行要计数或收集样例，不应中断整批日志。");

    const lines = [
        '127.0.0.1 - - [15/May/2026:10:00:00 +0800] "GET / HTTP/1.1" 200 10 "curl"',
        "bad log line",
        '127.0.0.1 - - [15/May/2026:10:00:01 +0800] "GET /missing HTTP/1.1" 404 5 "curl"'
    ];
    const parsed = parseManyWithErrors(lines);

    showJson("解析结果", {
        validCount: parsed.entries.length,
        invalidCount: parsed.invalidLines.length,
        summary: summarizeLogs(parsed.entries, 2)
    });
}

function demoTextSummaryExercise(): void {
    section("练习 04.2：纯文本入口");
    note("要求", "summarizeLogText 是测试友好的纯函数，调用方负责读文件或网络输入。");

    const text = [
        '127.0.0.1 - - [15/May/2026:10:00:00 +0800] "GET /api HTTP/1.1" 200 10 "curl"',
        '127.0.0.1 - - [15/May/2026:10:00:01 +0800] "GET /api HTTP/1.1" 200 15 "curl"',
        '127.0.0.1 - - [15/May/2026:10:00:02 +0800] "GET /admin HTTP/1.1" 500 1 "curl"'
    ].join("\n");

    showJson("日志摘要", summarizeLogText(text, 2));
}

/** Run integrated exercises. */
export function runExercises(): void {
    section("练习 04：综合练习");
    demoIntegratedLogExercise();
    demoTextSummaryExercise();
}

await runIfMain(import.meta.url, runExercises);
