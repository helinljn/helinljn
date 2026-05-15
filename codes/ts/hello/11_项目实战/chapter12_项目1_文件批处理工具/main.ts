import { mkdir, mkdtemp, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { parseFileBatchArgs } from "./config.js";
import { runFileBatch } from "./fileProcessor.js";
import { runIfMain, section, showJson } from "../../shared/chapter.js";

export async function runFileBatchCli(args: readonly string[] = process.argv.slice(2)): Promise<void> {
    const options = parseFileBatchArgs(args);
    const summary = await runFileBatch(options);
    console.log(JSON.stringify(summary, null, 2));
}

export async function runFileBatchDemo(): Promise<void> {
    section("第 13 章：项目 1 - 文件批处理 CLI");
    const temp = await mkdtemp(join(tmpdir(), "ts-file-batch-"));
    const input = join(temp, "input");
    const output = join(temp, "output");

    try {
        await mkdir(join(input, "nested"), { recursive: true });
        await writeFile(join(input, "a.txt"), "Hello\nTypeScript", { encoding: "utf8", flag: "w" });
        await writeFile(join(input, "nested", "app.log"), "INFO boot\nWARN disk", "utf8");
    } catch {
        await rm(temp, { recursive: true, force: true });
        throw new Error("failed to create demo input directory");
    }

    const summary = await runFileBatch({ input, output, mode: "prefix-lines", prefix: "demo" });
    showJson("文件批处理结果", summary);
    await rm(temp, { recursive: true, force: true });
}

await runIfMain(import.meta.url, () => runFileBatchCli());
