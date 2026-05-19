// =============================================================================
// 第 40 章：项目 2 - 日志分析 CLI
// =============================================================================
//
// 【学习目标】
//   1. 把日志解析、聚合统计、报告输出和 CLI 参数解析拆成可测试模块
//   2. 理解日志行 runtime validation、格式化输出和隐私边界
//   3. 保持 main.ts 作为章节入口，同时保留可复用的项目 demo 和 CLI 函数
//
// 【运行方式】
//   npm run build
//   node dist/11_项目实战/chapter40_项目2_日志分析CLI/main.js
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { resolve } from "node:path";
import { summarizeLogs } from "./analyzer.js";
import { readLogFile } from "./logParser.js";
import { formatSummary } from "./reporter.js";
import type { ReportFormat } from "./types.js";
import { runIfMain, section } from "../../shared/chapter.js";

/** Parsed CLI options for the log analyzer project. */
export type LogAnalyzerOptions = {
    log: string;
    format: ReportFormat;
    top: number;
};

function readFlag(args: readonly string[], flag: string): string | undefined {
    const index = args.indexOf(flag);
    if (index < 0) {
        return undefined;
    }
    const value = args[index + 1];
    if (value === undefined || value.startsWith("--")) {
        throw new Error(`missing value for ${flag}`);
    }
    return value;
}

/** Parse CLI arguments into validated log analyzer options. */
export function parseLogAnalyzerArgs(args: readonly string[]): LogAnalyzerOptions {
    const log = readFlag(args, "--log");
    if (log === undefined) {
        throw new Error("required flag --log is missing");
    }

    const formatText = readFlag(args, "--format") ?? "text";
    if (formatText !== "text" && formatText !== "json") {
        throw new Error("--format must be text or json");
    }

    const topText = readFlag(args, "--top") ?? "5";
    const top = Number.parseInt(topText, 10);
    if (!Number.isInteger(top) || top <= 0) {
        throw new Error("--top must be a positive integer");
    }

    return { log, format: formatText, top };
}

/** Run the reusable log analyzer workflow and return a formatted report. */
export async function runLogAnalyzer(options: LogAnalyzerOptions): Promise<string> {
    const entries = await readLogFile(options.log);
    const summary = summarizeLogs(entries, options.top);
    return formatSummary(summary, options.format);
}

/** Run the real CLI path with process arguments or injected arguments for tests. */
export async function runLogAnalyzerCli(args: readonly string[] = process.argv.slice(2)): Promise<void> {
    const options = parseLogAnalyzerArgs(args);
    console.log(await runLogAnalyzer(options));
}

/** Run a deterministic project demo using the checked-in sample access log. */
export async function runLogAnalyzerDemo(): Promise<void> {
    section("第 40 章：项目 2 - 日志分析 CLI");
    const sample = resolve("11_项目实战/chapter40_项目2_日志分析CLI/sample_logs/access.log");
    console.log(await runLogAnalyzer({ log: sample, format: "text", top: 3 }));
}

/** Run chapter 40 project demo. */
export async function runChapter(): Promise<void> {
    await runLogAnalyzerDemo();
}

await runIfMain(import.meta.url, runChapter);
