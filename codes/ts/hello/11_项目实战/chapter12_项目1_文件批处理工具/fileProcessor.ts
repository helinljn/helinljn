import { mkdir, readdir, readFile, writeFile } from "node:fs/promises";
import { dirname, extname, join, relative } from "node:path";
import type { FileBatchOptions, ProcessMode } from "./config.js";

const TEXT_EXTENSIONS = new Set([".txt", ".md", ".log"]);

export type FileBatchSummary = {
  mode: ProcessMode;
  filesProcessed: number;
  bytesWritten: number;
  outputFiles: string[];
};

export function processText(text: string, mode: ProcessMode, prefix = "line"): string {
  switch (mode) {
    case "upper":
      return text.toUpperCase();
    case "lower":
      return text.toLowerCase();
    case "prefix-lines":
      return text
        .split(/\r?\n/)
        .map((line, index) => `${prefix} ${index + 1}: ${line}`)
        .join("\n");
    default: {
      const unreachable: never = mode;
      return unreachable;
    }
  }
}

export async function collectTextFiles(root: string): Promise<string[]> {
  const result: string[] = [];
  const entries = await readdir(root, { withFileTypes: true });
  for (const entry of entries) {
    const fullPath = join(root, entry.name);
    if (entry.isDirectory()) {
      result.push(...await collectTextFiles(fullPath));
    } else if (entry.isFile() && TEXT_EXTENSIONS.has(extname(entry.name).toLowerCase())) {
      result.push(fullPath);
    }
  }
  return result.sort();
}

export async function runFileBatch(options: FileBatchOptions): Promise<FileBatchSummary> {
  const files = await collectTextFiles(options.input);
  const outputFiles: string[] = [];
  let bytesWritten = 0;

  for (const file of files) {
    const relativePath = relative(options.input, file);
    const target = join(options.output, relativePath);
    const text = await readFile(file, "utf8");
    const converted = processText(text, options.mode, options.prefix);
    await mkdir(dirname(target), { recursive: true });
    await writeFile(target, converted, "utf8");
    outputFiles.push(relativePath);
    bytesWritten += Buffer.byteLength(converted, "utf8");
  }

  return {
    mode: options.mode,
    filesProcessed: files.length,
    bytesWritten,
    outputFiles
  };
}
