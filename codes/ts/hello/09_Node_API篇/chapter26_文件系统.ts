// =============================================================================
// 第 26 章：文件系统
// =============================================================================
//
// 【学习目标】
//   1. 掌握 Node.js fs/promises 中常用的读写、目录、复制、重命名和删除 API
//   2. 理解文本读取、Buffer 读取、文件元数据、分块读取和 libuv 线程池的运行时边界
//   3. 建立可重复运行的文件系统 demo 习惯：临时目录、原子写入和资源清理
//
// 【运行方式】
//   npm run build
//   node dist/09_Node_API篇/chapter26_文件系统.js
//   或 npm run chapter -- 26（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { Buffer } from "node:buffer";
import {
    appendFile,
    copyFile,
    mkdir,
    mkdtemp,
    open,
    readFile,
    readdir,
    rename,
    rm,
    stat,
    watch,
    writeFile
} from "node:fs/promises";
import { tmpdir } from "node:os";
import { basename, isAbsolute, join, relative, resolve, sep } from "node:path";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

// =============================================================================
// 26.1 读文件：文本字符串与 Buffer 是两种边界
// =============================================================================
//
// C++ 对照：readFile(path, "utf8") 像把字节流按 UTF-8 解码成 std::string；
// 不传 encoding 时返回 Buffer，更接近 std::vector<std::byte> 或 uint8_t 缓冲区。
//
// C++ 思维提示：fs/promises 的文件 I/O 通过 Promise 暴露异步结果；多数异步文件
// 操作会交给 libuv 线程池调度到底层系统调用。等待 Promise 不会让 JS 主线程像同步
// fread 那样阻塞在当前调用栈上，但线程池不是无限资源。

async function demoReadFileEncoding(root: string): Promise<void> {
    section("26.1 读文件：文本字符串与 Buffer 是两种边界");
    note("C++ 对照", "带 encoding 的 readFile 返回字符串；不带 encoding 返回 Buffer。");

    const filePath = join(root, "read", "config.txt");
    await mkdir(join(root, "read"), { recursive: true });
    await writeFile(filePath, "target=ES2023\nstrict=true\n", "utf8");

    const text = await readFile(filePath, "utf8");
    const bytes = await readFile(filePath);

    showJson("同一文件的两种读取结果", {
        textLines: text.trimEnd().split("\n"),
        bufferByteLength: bytes.byteLength,
        bufferIsBuffer: Buffer.isBuffer(bytes),
        firstBytesUtf8: bytes.subarray(0, 6).toString("utf8")
    });
    note("输出解释", "字符串适合配置和日志；Buffer 适合二进制协议、图片、压缩包等字节级数据。");
    note("运行时边界", "多数 fs/promises 文件 I/O 会由 libuv 线程池执行；await 等结果时 JS 事件循环仍可继续处理其他任务。");
    note("常见坑", "不要把未知编码的文件直接当 UTF-8 文本；乱码不是 TypeScript 类型系统能发现的问题。");
}

// =============================================================================
// 26.2 写入、追加、复制与重命名
// =============================================================================
//
// C++ 对照：writeFile 默认覆盖文件，appendFile 类似追加模式打开流；rename 常用于
// 先写临时文件再原子替换目标文件，直觉接近先写新文件再替换指针。
//
// C++ 思维提示：每个 fs/promises 调用都是一个异步边界；多个文件操作之间如果有
// 顺序依赖，就必须 await，不能只创建 Promise 后假设顺序自然成立。

async function demoWriteAppendCopyRename(root: string): Promise<void> {
    section("26.2 写入、追加、复制与重命名");
    note("C++ 对照", "writeFile 覆盖，appendFile 追加，copyFile 复制，rename 移动或改名。");

    const workDir = join(root, "write");
    await mkdir(workDir, { recursive: true });

    const draft = join(workDir, "report.tmp");
    const finalFile = join(workDir, "report.txt");
    const backup = join(workDir, "report.copy.txt");

    await writeFile(draft, "line:created\n", "utf8");
    await appendFile(draft, "line:validated\n", "utf8");
    await copyFile(draft, backup);
    await rename(draft, finalFile);

    const names = (await readdir(workDir)).sort();
    const finalText = await readFile(finalFile, "utf8");

    showJson("写入流水线", {
        files: names,
        finalLines: finalText.trimEnd().split("\n"),
        draftStillExists: names.includes("report.tmp")
    });
    note("工程习惯", "需要避免半写入文件时，先写 .tmp，再 rename 到最终路径。");
    note("常见坑", "writeFile 会覆盖已有文件；真实工具要显式设计覆盖保护、dry-run 或备份策略。");
}

// =============================================================================
// 26.3 目录与元数据：readdir、stat 和递归清理
// =============================================================================
//
// C++ 对照：readdir/stat 类似 std::filesystem::directory_iterator 与 file_status；
// 区别是 Node 返回 Promise，并且路径字符串仍要自己管理平台差异。
//
// C++ 思维提示：目录 API 也是异步 I/O；不要在服务端请求路径中混用大量同步 fs API，
// 否则会阻塞事件循环。

async function demoDirectoryAndMetadata(root: string): Promise<void> {
    section("26.3 目录与元数据：readdir、stat 和递归清理");
    note("C++ 对照", "mkdir/readdir/stat/rm 对应 std::filesystem 的目录和元数据能力。");

    const projectDir = join(root, "tree", "src");
    const outputDir = join(root, "tree", "dist");
    await mkdir(projectDir, { recursive: true });
    await mkdir(outputDir, { recursive: true });
    await writeFile(join(projectDir, "main.ts"), "export const answer = 42;\n", "utf8");
    await writeFile(join(outputDir, "main.js"), "export const answer = 42;\n", "utf8");

    const treeRoot = join(root, "tree");
    const entries = await readdir(treeRoot, { withFileTypes: true });
    const srcStats = await stat(join(projectDir, "main.ts"));

    showJson("目录与文件元数据", {
        entries: entries
            .map((entry) => ({
                name: entry.name,
                kind: entry.isDirectory() ? "directory" : "file"
            }))
            .sort((left, right) => left.name.localeCompare(right.name)),
        srcFile: {
            size: srcStats.size,
            isFile: srcStats.isFile(),
            isDirectory: srcStats.isDirectory(),
            mtimeIsRuntimeDate: srcStats.mtime instanceof Date
        }
    });

    await rm(outputDir, { recursive: true, force: true });
    showJson("递归删除后", {
        remaining: (await readdir(treeRoot)).sort()
    });
    note("输出解释", "mtime 是运行时元数据，本章只展示它是 Date，不输出当前时间，避免不可复现。");
}

// =============================================================================
// 26.4 路径边界：相对路径基于 process.cwd()
// =============================================================================
//
// C++ 对照：相对路径依赖当前工作目录，和 C++ 程序中的 std::filesystem::current_path()
// 类似；ESM 模块所在目录不是同一个概念。
//
// C++ 思维提示：Node 进程可以从任意 cwd 启动，同一个脚本文件中的相对路径不一定指向
// 脚本旁边的文件。

function resolveInside(root: string, userPath: string): { ok: boolean; displayPath: string } {
    const candidate = resolve(root, userPath);
    const relativePath = relative(root, candidate);
    const isInside = relativePath === "" || (!relativePath.startsWith("..") && !isAbsolute(relativePath));

    return {
        ok: isInside,
        displayPath: isInside ? toPortablePath(relativePath) : "<blocked outside workspace>"
    };
}

async function demoPathBoundary(root: string): Promise<void> {
    section("26.4 路径边界：相对路径基于 process.cwd()");
    note("C++ 对照", "process.cwd() 是进程工作目录；import.meta.url 描述当前模块文件。");

    const relativeInput = join("data", "config.json");
    const resolvedFromCwd = resolve(relativeInput);

    showJson("相对路径解析", {
        input: toPortablePath(relativeInput),
        resolvedRelativeTo: "process.cwd()",
        cwdBaseName: basename(process.cwd()),
        relativeFromCwd: toPortablePath(relative(process.cwd(), resolvedFromCwd)),
        moduleUrlAvailable: import.meta.url.startsWith("file:")
    });

    showJson("用户路径必须限制在工作区内", {
        accepted: resolveInside(root, join("logs", "app.log")),
        rejected: resolveInside(root, join("..", "secret.txt"))
    });
    note("常见坑", "不要把用户输入直接拼进文件路径；先 resolve，再确认结果仍在允许的根目录内。");
}

// =============================================================================
// 26.5 大文件分块读取：过渡到 ch31 的 stream
// =============================================================================
//
// C++ 对照：FileHandle.read 接近手动 read(fd, buffer, size)；每次读取固定大小的块。
//
// C++ 思维提示：分块读取能控制内存峰值，但手写循环要负责关闭句柄；ch31 会介绍更适合
// 大文件流水线的 stream 和 backpressure。

async function readInChunks(filePath: string, chunkSize: number): Promise<string[]> {
    const handle = await open(filePath, "r");
    const buffer = Buffer.alloc(chunkSize);
    const chunks: string[] = [];
    let position = 0;

    try {
        while (true) {
            const { bytesRead } = await handle.read(buffer, 0, buffer.byteLength, position);
            if (bytesRead === 0) {
                break;
            }
            chunks.push(buffer.subarray(0, bytesRead).toString("utf8").replaceAll("\n", "\\n"));
            position += bytesRead;
        }
    } finally {
        await handle.close();
    }

    return chunks;
}

async function demoChunkedRead(root: string): Promise<void> {
    section("26.5 大文件分块读取：过渡到 ch31 的 stream");
    note("C++ 对照", "open + read + close 是低层文件句柄模型；stream 是更高层流水线。");

    const filePath = join(root, "chunks", "events.log");
    await mkdir(join(root, "chunks"), { recursive: true });
    await writeFile(filePath, "01:boot\n02:load\n03:ready\n04:done\n", "utf8");

    const chunks = await readInChunks(filePath, 10);
    showJson("固定 10 字节分块", chunks.map((chunk, index) => ({ index, chunk })));
    note("输出解释", "这里故意用很小的 chunk 展示边界；真实大文件不应一次 readFile 到内存。");
    note("类型边界", "TypeScript 能保证 chunk 是 string[]，但不能自动证明切块没有拆开多字节字符。");
}

// =============================================================================
// 26.6 文件监听：watch 是平台相关事件，不是强一致日志
// =============================================================================
//
// C++ 对照：文件监听类似 inotify/FSEvents/ReadDirectoryChangesW 的封装，不同平台语义
// 本来就不同；它适合触发重新加载，不适合作为可靠审计日志。
//
// C++ 思维提示：watch 返回异步迭代器，必须有 AbortSignal 或其他退出路径，否则章节、
// 测试或服务进程可能一直挂着。

function isAbortError(error: unknown): boolean {
    return error instanceof Error && error.name === "AbortError";
}

async function demoWatchLifecycle(root: string): Promise<void> {
    section("26.6 文件监听：watch 是平台相关事件，不是强一致日志");
    note("C++ 对照", "watch 封装平台文件事件；具体 change/rename 数量和顺序不能跨平台依赖。");

    const watchDir = join(root, "watch");
    await mkdir(watchDir, { recursive: true });

    const controller = new AbortController();
    const watcher = watch(watchDir, { signal: controller.signal });
    const iterator = watcher[Symbol.asyncIterator]();
    controller.abort();

    let closedByAbort = false;
    try {
        const result = await iterator.next();
        closedByAbort = result.done === true;
    } catch (error) {
        if (!isAbortError(error)) {
            throw error;
        }
        closedByAbort = true;
    }

    showJson("watch 生命周期", {
        api: "fs.promises.watch",
        closedByAbort,
        eventOrdering: "platform-dependent; not used as a chapter assertion"
    });
    note("常见坑", "watcher、interval、server、stream 都要有明确关闭路径；否则全量验收会挂起。");
}

// =============================================================================
// 26.7 临时目录：mkdtemp 与可重复运行
// =============================================================================
//
// C++ 对照：临时目录类似用 std::filesystem::temp_directory_path() 创建测试工作区；
// 区别是 Node API 返回 Promise，清理通常放在 finally 中。
//
// C++ 思维提示：教程、测试和 CLI demo 都应该能反复运行；不要把固定输出写到仓库根目录。

async function withChapterWorkspace<T>(useWorkspace: (root: string) => Promise<T>): Promise<T> {
    const root = await mkdtemp(join(tmpdir(), "ts-hello-ch26-"));
    try {
        return await useWorkspace(root);
    } finally {
        await rm(root, { recursive: true, force: true });
    }
}

async function demoTemporaryWorkspace(): Promise<void> {
    section("26.7 临时目录：mkdtemp 与可重复运行");
    note("C++ 对照", "mkdtemp 在系统临时目录下创建唯一工作区；finally 中用 rm 递归清理。");

    const summary = await withChapterWorkspace(async (workspace) => {
        await mkdir(join(workspace, "out"), { recursive: true });
        await writeFile(join(workspace, "out", "hello.txt"), "hello\n", "utf8");
        return {
            tempParent: "os.tmpdir()",
            createdFile: "out/hello.txt",
            content: (await readFile(join(workspace, "out", "hello.txt"), "utf8")).trim()
        };
    });

    showJson("临时工作区结果", summary);
    note("输出解释", "真实临时路径包含随机后缀，本章不打印它，避免输出随机器变化。");
}

// =============================================================================
// 26.8 工程场景：生成目录清单并原子写报告
// =============================================================================
//
// C++ 对照：这相当于一个小型文件批处理工具：扫描目录、读取元数据、生成报告，再用
// rename 提交结果。
//
// C++ 思维提示：文件系统程序的核心不是 API 会不会调，而是能否在失败和重跑时保持
// 输出一致、资源关闭、路径受控。

async function demoManifestWorkflow(root: string): Promise<void> {
    section("26.8 工程场景：生成目录清单并原子写报告");
    note("C++ 对照", "把多个 fs API 组合成稳定流水线，类似小型构建步骤或资源打包工具。");

    const sourceDir = join(root, "manifest", "source");
    const outputDir = join(root, "manifest", "out");
    await mkdir(sourceDir, { recursive: true });
    await mkdir(outputDir, { recursive: true });
    await writeFile(join(sourceDir, "alpha.txt"), "alpha\n", "utf8");
    await writeFile(join(sourceDir, "beta.txt"), "beta\n", "utf8");
    await writeFile(join(sourceDir, "ignore.bin"), Buffer.from([0, 1, 2]));

    const entries = await readdir(sourceDir);
    const textFiles = entries.filter((entry) => entry.endsWith(".txt")).sort();
    const manifest = [];
    for (const fileName of textFiles) {
        const filePath = join(sourceDir, fileName);
        const fileStats = await stat(filePath);
        manifest.push({
            fileName,
            size: fileStats.size,
            preview: (await readFile(filePath, "utf8")).trim()
        });
    }

    const tempReport = join(outputDir, "manifest.json.tmp");
    const finalReport = join(outputDir, "manifest.json");
    await writeFile(tempReport, `${JSON.stringify(manifest, null, 2)}\n`, "utf8");
    await rename(tempReport, finalReport);

    showJson("目录清单", {
        scanned: textFiles,
        reportFile: toPortablePath(relative(root, finalReport)),
        manifest
    });
    note("类型边界", "目录内容来自运行时，entry 是字符串；业务规则仍要用后缀、stat 或校验函数筛选。");
    note("本章复盘", "读写、目录、元数据、分块读取、watch 和临时目录都要围绕可重复运行来设计。");
}

function toPortablePath(pathText: string): string {
    return pathText.split(sep).join("/");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 26 demos. */
export async function runChapter(): Promise<void> {
    await withChapterWorkspace(async (root) => {
        await demoReadFileEncoding(root);
        await demoWriteAppendCopyRename(root);
        await demoDirectoryAndMetadata(root);
        await demoPathBoundary(root);
        await demoChunkedRead(root);
        await demoWatchLifecycle(root);
        await demoTemporaryWorkspace();
        await demoManifestWorkflow(root);
    });
}

await runIfMain(import.meta.url, runChapter);
