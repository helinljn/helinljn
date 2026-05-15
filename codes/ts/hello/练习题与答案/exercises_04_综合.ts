import { parseLogLine } from "../11_项目实战/chapter13_项目2_日志分析工具/logParser.js";
import { summarizeLogs } from "../11_项目实战/chapter13_项目2_日志分析工具/analyzer.js";
import type { LogEntry, LogSummary } from "../11_项目实战/chapter13_项目2_日志分析工具/types.js";
import { runIfMain, section, showJson } from "../shared/chapter.js";

export function parseManyLogLines(lines: readonly string[]): LogEntry[] {
    return lines
        .map(parseLogLine)
        .filter((entry): entry is LogEntry => entry !== null);
}

export function summarizeLogText(text: string, top = 5): LogSummary {
    const entries = parseManyLogLines(text.split(/\r?\n/));
    return summarizeLogs(entries, top);
}

export function runExercises(): void {
    section("练习 04：综合练习");
    const text = [
        '127.0.0.1 - - [15/May/2026:10:00:00 +0800] "GET / HTTP/1.1" 200 10 "curl"',
        '127.0.0.1 - - [15/May/2026:10:00:01 +0800] "GET /missing HTTP/1.1" 404 5 "curl"'
    ].join("\n");
    showJson("日志摘要", summarizeLogText(text, 2));
}

await runIfMain(import.meta.url, runExercises);
