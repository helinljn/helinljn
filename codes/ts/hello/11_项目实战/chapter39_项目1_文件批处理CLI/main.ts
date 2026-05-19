// =============================================================================
// 第 39 章：项目 1 - 文件批处理 CLI
// =============================================================================
//
// 【学习目标】
//   1. 把 CLI 参数解析、runtime validation、目录遍历和文本转换组织成可测试模块
//   2. 理解 dry-run、覆盖保护、临时目录和资源清理这些 CLI 可靠性边界
//   3. 保持 main.ts 作为章节入口，同时保留可复用的项目 demo 和 CLI 函数
//
// 【运行方式】
//   npm run build
//   node dist/11_项目实战/chapter39_项目1_文件批处理CLI/main.js
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { mkdir, mkdtemp, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { parseFileBatchArgs } from "./config.js";
import { runFileBatch } from "./fileProcessor.js";
import { runIfMain, section, showJson } from "../../shared/chapter.js";

/** Run the real CLI path with process arguments or injected arguments for tests. */
export async function runFileBatchCli(args: readonly string[] = process.argv.slice(2)): Promise<void> {
    const options = parseFileBatchArgs(args);
    const summary = await runFileBatch(options);
    console.log(JSON.stringify(summary, null, 2));
}

/** Run a deterministic project demo in a temporary directory and clean it afterwards. */
export async function runFileBatchDemo(): Promise<void> {
    section("第 39 章：项目 1 - 文件批处理 CLI");
    const temp = await mkdtemp(join(tmpdir(), "ts-file-batch-"));
    const input = join(temp, "input");
    const output = join(temp, "output");

    try {
        await mkdir(join(input, "nested"), { recursive: true });
        await writeFile(join(input, "a.txt"), "Hello\nTypeScript", { encoding: "utf8", flag: "w" });
        await writeFile(join(input, "nested", "app.log"), "INFO boot\nWARN disk", "utf8");

        const summary = await runFileBatch({ input, output, mode: "prefix-lines", prefix: "demo" });
        showJson("文件批处理结果", summary);
    } catch {
        throw new Error("failed to run file batch demo");
    } finally {
        await rm(temp, { recursive: true, force: true });
    }
}

/** Run chapter 39 project demo. */
export async function runChapter(): Promise<void> {
    await runFileBatchDemo();
}

await runIfMain(import.meta.url, runChapter);
