import { resolve } from "node:path";
import { summarizeLogs } from "./analyzer.js";
import { readLogFile } from "./logParser.js";
import { formatSummary } from "./reporter.js";
import type { ReportFormat } from "./types.js";
import { runIfMain, section } from "../../shared/chapter.js";

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

export async function runLogAnalyzer(options: LogAnalyzerOptions): Promise<string> {
    const entries = await readLogFile(options.log);
    const summary = summarizeLogs(entries, options.top);
    return formatSummary(summary, options.format);
}

export async function runLogAnalyzerCli(args: readonly string[] = process.argv.slice(2)): Promise<void> {
    const options = parseLogAnalyzerArgs(args);
    console.log(await runLogAnalyzer(options));
}

export async function runLogAnalyzerDemo(): Promise<void> {
    section("第 14 章：项目 2 - 日志分析 CLI");
    const sample = resolve("11_项目实战/chapter13_项目2_日志分析工具/sample_logs/access.log");
    console.log(await runLogAnalyzer({ log: sample, format: "text", top: 3 }));
}

await runIfMain(import.meta.url, () => runLogAnalyzerCli());
