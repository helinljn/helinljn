// =============================================================================
// 第 31 章：流
// =============================================================================
//
// 【学习目标】
//   1. 理解 Readable、Writable、Duplex、Transform 四类 stream 的职责
//   2. 掌握 paused/flowing 模式、pipeline、背压、文件流、压缩流和 readline
//   3. 建立 stream cleanup 习惯：用 pipeline/finished 收敛错误和资源关闭
//
// 【运行方式】
//   npm run build
//   node dist/09_Node_API篇/chapter31_流.js
//   或 npm run chapter -- 31（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { once } from "node:events";
import { createReadStream, createWriteStream } from "node:fs";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { createInterface } from "node:readline";
import { Duplex, PassThrough, Readable, Transform, Writable } from "node:stream";
import { finished, pipeline } from "node:stream/promises";
import { createGunzip, createGzip } from "node:zlib";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type CollectingWritable = {
    chunks: string[];
    writable: Writable;
};

// =============================================================================
// 31.1 四种 stream：Readable、Writable、Duplex、Transform
// =============================================================================
//
// C++ 对照：Node stream 像异步版 iostream/channel；数据以 chunk 为单位流动，而不是一次性
// 把完整文件或网络响应放进内存。
//
// C++ 思维提示：Readable 是数据来源，Writable 是数据去向，Duplex 同时可读可写，
// Transform 是会改变数据的 Duplex。

function chunkToText(chunk: unknown): string {
    return Buffer.isBuffer(chunk) ? chunk.toString("utf8") : String(chunk);
}

function createCollectingWritable(): CollectingWritable {
    const chunks: string[] = [];
    return {
        chunks,
        writable: new Writable({
            write(chunk, _encoding, callback) {
                chunks.push(chunkToText(chunk));
                callback();
            }
        })
    };
}

class UppercaseTransform extends Transform {
    _transform(chunk: unknown, _encoding: BufferEncoding, callback: (error?: Error | null) => void): void {
        this.push(chunkToText(chunk).toUpperCase());
        callback();
    }
}

async function demoStreamKinds(): Promise<void> {
    section("31.1 四种 stream：Readable、Writable、Duplex、Transform");
    note("C++ 对照", "stream 把数据处理拆成 source、transform 和 sink，适合大文件和网络响应。");

    const collector = createCollectingWritable();
    await pipeline(Readable.from(["readable", " -> ", "transform"]), new UppercaseTransform(), collector.writable);

    const passThrough = new PassThrough();
    const duplexChunks: string[] = [];
    passThrough.on("data", (chunk: unknown) => {
        duplexChunks.push(chunkToText(chunk));
    });
    passThrough.write("duplex ");
    passThrough.end("echo");
    await finished(passThrough);

    showJson("四种 stream 角色", {
        readable: "Readable.from([...]) produces chunks",
        writable: "custom Writable collects chunks",
        transformOutput: collector.chunks.join(""),
        duplex: {
            passThroughIsDuplex: passThrough instanceof Duplex,
            passThroughIsTransform: passThrough instanceof Transform,
            echoed: duplexChunks.join("")
        }
    });
    note("输出解释", "PassThrough 不改变数据，但它同时可写可读，因此也是最小的 Duplex/Transform 示例。");
}

// =============================================================================
// 31.2 两种模式：paused mode 与 flowing mode
// =============================================================================
//
// C++ 对照：paused mode 更像手动 read；flowing mode 更像注册回调后数据主动推送。
//
// C++ 思维提示：同一个 Readable 不要混用多套消费方式。选择 data 事件、read()、pipeline
// 或 for await...of 其中一种，避免数据被意外消费。

async function demoPausedAndFlowingModes(): Promise<void> {
    section("31.2 两种模式：paused mode 与 flowing mode");
    note("C++ 对照", "paused mode 主动拉取数据；flowing mode 通过 data 事件自动推送。");

    const pausedChunks: string[] = [];
    const paused = Readable.from(["pa", "us", "ed"]);
    paused.on("readable", () => {
        let chunk: unknown;
        while ((chunk = paused.read()) !== null) {
            pausedChunks.push(chunkToText(chunk));
        }
    });
    await finished(paused);

    const flowingChunks: string[] = [];
    const flowing = Readable.from(["fl", "ow", "ing"]);
    flowing.on("data", (chunk: unknown) => {
        flowingChunks.push(chunkToText(chunk));
    });
    await finished(flowing);

    showJson("paused vs flowing", {
        pausedMode: pausedChunks.join(""),
        flowingMode: flowingChunks.join(""),
        rule: "do not mix consumers on the same stream"
    });
    note("常见坑", "给 Readable 绑定 data 事件会让它进入 flowing mode；如果你还想手动 read，就容易错过数据。");
}

// =============================================================================
// 31.3 pipeline：推荐的组合方式，自动处理错误和清理
// =============================================================================
//
// C++ 对照：pipeline 像把 source/filter/sink 组合成一条带错误传播的处理链。
//
// C++ 思维提示：readable.pipe(writable) 是老 API，错误处理需要自己串起来；pipeline 会把
// 中间错误作为 Promise rejection 收敛到一个 await 点。

async function demoPipelineVsPipe(): Promise<void> {
    section("31.3 pipeline：推荐的组合方式，自动处理错误和清理");
    note("C++ 对照", "pipeline 把多个 stream 的生命周期合并成一个可等待的操作。");

    const collector = createCollectingWritable();
    await pipeline(Readable.from(["alpha\n", "beta\n"]), new UppercaseTransform(), collector.writable);

    showJson("pipeline 结果", {
        output: collector.chunks.join("").trimEnd().split("\n"),
        pipeBoundary: "readable.pipe(writable) is usable, but errors are easier to miss",
        pipelineBoundary: "await pipeline(...) rejects when any stream fails"
    });
    note("常见坑", "不要只写 source.pipe(transform).pipe(dest) 然后忽略 error；生产代码优先用 pipeline。");
}

// =============================================================================
// 31.4 背压：Writable.write 返回 false 时要等待 drain
// =============================================================================
//
// C++ 对照：背压类似生产者写入速度超过消费者处理速度时的队列控制；不能无限塞内存。
//
// C++ 思维提示：Node stream 的背压是协作式信号。write 返回 false 不是失败，而是告诉你
// 暂停写入，等 drain 再继续。

async function demoBackpressure(): Promise<void> {
    section("31.4 背压：Writable.write 返回 false 时要等待 drain");
    note("C++ 对照", "背压让生产者尊重消费者速度，避免内存无限增长。");

    const processed: number[] = [];
    const slowSink = new Writable({
        highWaterMark: 8,
        write(chunk, _encoding, callback) {
            const size = Buffer.isBuffer(chunk) ? chunk.byteLength : Buffer.byteLength(String(chunk));
            processed.push(size);
            queueMicrotask(callback);
        }
    });

    const writes: Array<{ bytes: number; acceptedImmediately: boolean }> = [];
    for (const chunk of [Buffer.alloc(16, "a"), Buffer.alloc(16, "b")]) {
        const acceptedImmediately = slowSink.write(chunk);
        writes.push({ bytes: chunk.byteLength, acceptedImmediately });
        if (!acceptedImmediately) {
            await once(slowSink, "drain");
        }
    }
    slowSink.end();
    await finished(slowSink);

    showJson("背压观察", {
        writes,
        processed,
        rule: "when write returns false, wait for drain before writing more"
    });
    note("输出解释", "highWaterMark 很小，16 字节 chunk 会触发背压；demo 等 drain 后再继续写。");
}

// =============================================================================
// 31.5 文件流：createReadStream + createWriteStream
// =============================================================================
//
// C++ 对照：文件流类似分块 ifstream/ofstream，但 Node 通过事件循环异步处理 chunk。
//
// C++ 思维提示：文件流必须关闭文件句柄。pipeline 成功或失败时都会收敛 stream 关闭，
// 比手动监听多个事件更稳。

async function demoFileStreams(root: string): Promise<void> {
    section("31.5 文件流：createReadStream + createWriteStream");
    note("C++ 对照", "文件流避免一次性把整个文件读入内存。");

    const input = join(root, "input.txt");
    const output = join(root, "output.txt");
    await writeFile(input, "stream\npipeline\ncleanup\n", "utf8");

    await pipeline(createReadStream(input), new UppercaseTransform(), createWriteStream(output));
    const outputText = await readFile(output, "utf8");

    showJson("文件流转换", {
        outputLines: outputText.trimEnd().split("\n"),
        cleanup: "pipeline owns stream close/error propagation"
    });
    note("常见坑", "不要在大文件处理中用 readFile + writeFile 模拟流；那会把内存峰值推高。");
}

// =============================================================================
// 31.6 压缩流：zlib.createGzip/createGunzip 配合 pipeline
// =============================================================================
//
// C++ 对照：压缩流像 filter stream，输入明文 chunk，输出 gzip 格式 chunk。
//
// C++ 思维提示：压缩、加密、编码等转换通常应该作为 Transform 放进 pipeline，而不是
// 先把完整内容拼成一个巨大 Buffer。

async function demoCompressionStreams(): Promise<void> {
    section("31.6 压缩流：zlib.createGzip/createGunzip 配合 pipeline");
    note("C++ 对照", "gzip/gunzip 是 Transform 流，适合放入处理链。");

    const input = "alpha\nbeta\ngamma\n";
    const compressed = createCollectingBufferWritable();
    await pipeline(Readable.from([input]), createGzip(), compressed.writable);

    const restored = createCollectingWritable();
    await pipeline(Readable.from([Buffer.concat(compressed.chunks)]), createGunzip(), restored.writable);

    showJson("压缩再解压", {
        inputBytes: Buffer.byteLength(input),
        compressedBytes: Buffer.concat(compressed.chunks).byteLength,
        restoredLines: restored.chunks.join("").trimEnd().split("\n")
    });
    note("输出解释", "小文本 gzip 后可能比原文更大；压缩收益取决于数据规模和重复模式。");
}

function createCollectingBufferWritable(): { chunks: Buffer[]; writable: Writable } {
    const chunks: Buffer[] = [];
    return {
        chunks,
        writable: new Writable({
            write(chunk, _encoding, callback) {
                chunks.push(Buffer.isBuffer(chunk) ? chunk : Buffer.from(String(chunk)));
                callback();
            }
        })
    };
}

// =============================================================================
// 31.7 readline：逐行处理大文件
// =============================================================================
//
// C++ 对照：readline 类似按行 getline，但底层仍来自异步 Readable stream。
//
// C++ 思维提示：逐行处理日志、CSV、NDJSON 时，不要先把整个文件 split("\\n")；这会失去
// 流式处理的内存优势。

async function demoReadline(root: string): Promise<void> {
    section("31.7 readline：逐行处理大文件");
    note("C++ 对照", "readline 在 Readable stream 上提供逐行 async iterable。");

    const logFile = join(root, "app.log");
    await writeFile(logFile, "INFO boot\nWARN disk\nINFO ready\n", "utf8");

    const lineReader = createInterface({
        input: createReadStream(logFile),
        crlfDelay: Infinity
    });
    const counts = new Map<string, number>();
    try {
        for await (const line of lineReader) {
            const level = line.split(" ", 1)[0] ?? "<empty>";
            counts.set(level, (counts.get(level) ?? 0) + 1);
        }
    } finally {
        lineReader.close();
    }

    showJson("逐行统计", Object.fromEntries([...counts.entries()].sort()));
    note("工程场景", "日志分析 CLI 通常用 readline 或 stream pipeline，避免对大日志一次性 readFile。");
}

// =============================================================================
// 31.8 自定义 Transform：把 chunk 转成业务格式
// =============================================================================
//
// C++ 对照：Transform 类似自定义 filterbuf/filter stream，可以把输入 chunk 转成另一种形态。
//
// C++ 思维提示：Transform 中的错误要通过 callback(error) 或 destroy(error) 进入 pipeline，
// 不要悄悄吞掉。

class PrefixLineTransform extends Transform {
    _transform(chunk: unknown, _encoding: BufferEncoding, callback: (error?: Error | null) => void): void {
        const text = chunkToText(chunk);
        const prefixed = text
            .split("\n")
            .filter((line) => line.length > 0)
            .map((line) => `> ${line}\n`)
            .join("");
        this.push(prefixed);
        callback();
    }
}

async function demoCustomTransform(): Promise<void> {
    section("31.8 自定义 Transform：把 chunk 转成业务格式");
    note("C++ 对照", "自定义 Transform 把通用字节流适配成业务需要的格式。");

    const collector = createCollectingWritable();
    await pipeline(Readable.from(["first\nsecond\n"]), new PrefixLineTransform(), collector.writable);

    showJson("自定义 Transform 输出", collector.chunks.join("").trimEnd().split("\n"));
    note("类型边界", "chunk 的运行时形态可能是 Buffer 或 string；Transform 内部要显式转换。");
}

// =============================================================================
// 31.9 async iterable streams 与 cleanup checklist
// =============================================================================
//
// C++ 对照：for await...of 像异步范围 for，每次等待下一个 chunk。
//
// C++ 思维提示：async iterable 语法可读性好，但资源清理仍要考虑。完整流水线优先用
// pipeline；手写消费循环要确保错误路径和提前退出路径关闭资源。

async function demoAsyncIterableStreams(): Promise<void> {
    section("31.9 async iterable streams 与 cleanup checklist");
    note("C++ 对照", "for await...of 让 Readable 的异步 chunk 消费看起来像普通循环。");

    const chunks: string[] = [];
    for await (const chunk of Readable.from(["async", "-", "iterable"])) {
        chunks.push(chunkToText(chunk));
    }

    showJson("async iterable 消费", {
        output: chunks.join(""),
        cleanupChecklist: [
            "pipeline for multi-stream chains",
            "finished when manually listening to events",
            "finally close readline/server/watcher when you own it",
            "respect write() backpressure and wait for drain"
        ]
    });
    note("本章复盘", "stream 的价值在于分块、背压和资源边界；不是把回调换成更复杂的语法。");
}

async function withWorkspace<T>(useWorkspace: (root: string) => Promise<T>): Promise<T> {
    const root = await mkdtemp(join(tmpdir(), "ts-hello-ch31-"));
    try {
        return await useWorkspace(root);
    } finally {
        await rm(root, { recursive: true, force: true });
    }
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 31 demos. */
export async function runChapter(): Promise<void> {
    await withWorkspace(async (root) => {
        await demoStreamKinds();
        await demoPausedAndFlowingModes();
        await demoPipelineVsPipe();
        await demoBackpressure();
        await demoFileStreams(root);
        await demoCompressionStreams();
        await demoReadline(root);
        await demoCustomTransform();
        await demoAsyncIterableStreams();
    });
}

await runIfMain(import.meta.url, runChapter);
