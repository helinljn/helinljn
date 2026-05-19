// =============================================================================
// 第 27 章：路径、进程与环境
// =============================================================================
//
// 【学习目标】
//   1. 掌握 path 模块的跨平台路径处理方式，区分 cwd 与模块所在目录
//   2. 理解 process.argv、process.env、stdout/stderr/stdin、platform/arch 的职责边界
//   3. 学会用配置分层、secret 脱敏和 child_process 安全边界组织 CLI 程序
//
// 【运行方式】
//   npm run build
//   node dist/09_Node_API篇/chapter27_路径进程与环境.js
//   或 npm run chapter -- 27（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { exec, spawn } from "node:child_process";
import {
    basename,
    dirname,
    extname,
    isAbsolute,
    join,
    normalize,
    parse,
    relative,
    resolve,
    sep
} from "node:path";
import { fileURLToPath } from "node:url";
import { promisify } from "node:util";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

const execAsync = promisify(exec);

type AppMode = "development" | "production";
type LogLevel = "debug" | "info" | "warn";

type AppConfig = {
    mode: AppMode;
    port: number;
    verbose: boolean;
    logLevel: LogLevel;
    token: string;
};

type ConfigPatch = Partial<AppConfig>;

type PublicConfig = Omit<AppConfig, "token"> & {
    token: string;
};

type SpawnResult = {
    stdout: string;
    stderr: string;
    exitCode: number | null;
    signal: NodeJS.Signals | null;
};

type ChildProcessUnavailable = {
    unavailable: true;
    reason: string;
};

// =============================================================================
// 27.1 path 模块：用语义 API 处理路径，不手写分隔符
// =============================================================================
//
// C++ 对照：path.join/resolve/dirname/basename/extname/parse/normalize/relative
// 对应 std::filesystem::path 的组合、规范化和分解能力。
//
// C++ 思维提示：Windows 使用反斜杠，POSIX 使用斜杠；字符串拼接路径会把平台差异、
// 绝对路径和 .. 边界都留给 bug。

function demoPathModule(): void {
    section("27.1 path 模块：用语义 API 处理路径，不手写分隔符");
    note("C++ 对照", "path 模块像 std::filesystem::path，负责跨平台路径拼接、分解和规范化。");

    const input = join("src", "features", "..", "features", "report.ts");
    const normalized = normalize(input);
    const absolute = resolve(input);
    const parsed = parse(normalized);

    showJson("常用 path API", {
        join: toPortablePath(input),
        normalize: toPortablePath(normalized),
        dirname: toPortablePath(dirname(normalized)),
        basename: basename(normalized),
        extname: extname(normalized),
        parse: {
            root: parsed.root === "" ? "<relative>" : toPortablePath(parsed.root),
            dir: toPortablePath(parsed.dir),
            base: parsed.base,
            name: parsed.name,
            ext: parsed.ext
        },
        resolveFromCwd: toPortablePath(relative(process.cwd(), absolute))
    });

    showJson("relative 与 isAbsolute", {
        fromProjectToFile: toPortablePath(relative(process.cwd(), absolute)),
        isInputAbsolute: isAbsolute(input),
        isResolvedAbsolute: isAbsolute(absolute),
        platformSeparator: sep === "\\" ? "\\\\" : sep
    });
    note("常见坑", "不要用 `a + \"/\" + b` 拼路径；一旦遇到 Windows、绝对路径或 ..，行为就会变脆。");
}

// =============================================================================
// 27.2 ESM 中没有 __dirname：使用 import.meta.dirname 或 fileURLToPath
// =============================================================================
//
// C++ 对照：__dirname 更像 CommonJS 加载器提供的宏；ESM 使用 import.meta 描述当前模块。
//
// C++ 思维提示：脚本所在目录和进程当前工作目录是两个概念。CLI 从不同目录启动时，
// process.cwd() 会变，模块文件的位置不会跟着变。

function getModuleDirectory(): string {
    const meta = import.meta as ImportMeta & { dirname?: string };
    return meta.dirname ?? dirname(fileURLToPath(import.meta.url));
}

function demoEsmDirectory(): void {
    section("27.2 ESM 中没有 __dirname：使用 import.meta.dirname 或 fileURLToPath");
    note("C++ 对照", "CommonJS 的 __dirname 类似加载器注入变量；ESM 的标准入口是 import.meta。");

    const moduleDirectory = getModuleDirectory();
    showJson("模块目录边界", {
        commonJsName: "__dirname 在本项目 ESM 文件中不可用",
        importMetaUrlProtocol: new URL(import.meta.url).protocol,
        moduleDirectoryRelativeToCwd: toPortablePath(relative(process.cwd(), moduleDirectory)),
        fallbackExpression: "dirname(fileURLToPath(import.meta.url))"
    });
    note("版本边界", "Node 24 可用 import.meta.dirname；本章仍保留 fileURLToPath fallback，便于理解 ESM URL 模型。");
}

// =============================================================================
// 27.3 process.argv 与 process.env：外部输入都是字符串边界
// =============================================================================
//
// C++ 对照：process.argv 类似 main(int argc, char** argv)，process.env 类似 getenv。
//
// C++ 思维提示：argv 和 env 都来自进程外部。TypeScript 能告诉你值可能是 undefined，
// 但不能证明字符串内容符合业务规则。

function demoProcessInputs(): void {
    section("27.3 process.argv 与 process.env：外部输入都是字符串边界");
    note("C++ 对照", "argv/env 是进程启动协议；进入 TS 后仍要做运行时校验。");

    const scriptPath = process.argv[1];
    const userArgs = process.argv.slice(2);
    const homeFromEnv = process.env.TS_HELLO_HOME ?? "<not set: fallback to process.cwd()>";

    showJson("当前进程输入形状", {
        nodeExecutable: basename(process.execPath),
        argv0: basename(process.argv[0] ?? ""),
        argv1Relative: scriptPath === undefined ? "<none>" : toPortablePath(relative(process.cwd(), scriptPath)),
        userArgs,
        envExample: {
            name: "TS_HELLO_HOME",
            value: homeFromEnv
        }
    });
    note("常见坑", "process.env.PORT 的类型是 string | undefined；即使写着 3000，也要解析成 number 后再用。");
}

// =============================================================================
// 27.4 配置分层：默认值、配置文件、环境变量、CLI 参数
// =============================================================================
//
// C++ 对照：这类似服务进程读取默认配置、配置文件、环境变量和命令行参数，然后合并成
// 一个最终 options struct。
//
// C++ 思维提示：secret 是运行时输入，不应该出现在源码、日志或错误堆栈里。

const defaultConfig: AppConfig = {
    mode: "development",
    port: 3000,
    verbose: false,
    logLevel: "info",
    token: "<missing>"
};

function parseMode(value: string): AppMode {
    if (value === "development" || value === "production") {
        return value;
    }
    throw new Error(`invalid mode: ${value}`);
}

function parseLogLevel(value: string): LogLevel {
    if (value === "debug" || value === "info" || value === "warn") {
        return value;
    }
    throw new Error(`invalid log level: ${value}`);
}

function parsePort(value: string): number {
    const port = Number(value);
    if (!Number.isInteger(port) || port < 1 || port > 65_535) {
        throw new Error(`invalid port: ${value}`);
    }
    return port;
}

function parseEnvPatch(env: Record<string, string | undefined>): ConfigPatch {
    const patch: ConfigPatch = {};

    if (env.APP_MODE !== undefined) {
        patch.mode = parseMode(env.APP_MODE);
    }
    if (env.APP_PORT !== undefined) {
        patch.port = parsePort(env.APP_PORT);
    }
    if (env.APP_LOG_LEVEL !== undefined) {
        patch.logLevel = parseLogLevel(env.APP_LOG_LEVEL);
    }
    if (env.APP_TOKEN !== undefined) {
        patch.token = env.APP_TOKEN;
    }

    return patch;
}

function readOptionValue(args: readonly string[], index: number, option: string): string {
    const value = args[index + 1];
    if (value === undefined || value.startsWith("--")) {
        throw new Error(`${option} requires a value`);
    }
    return value;
}

function parseCliPatch(args: readonly string[]): ConfigPatch {
    const patch: ConfigPatch = {};

    for (let index = 0; index < args.length; index += 1) {
        const arg = args[index];
        if (arg === "--mode") {
            patch.mode = parseMode(readOptionValue(args, index, arg));
            index += 1;
        } else if (arg === "--port") {
            patch.port = parsePort(readOptionValue(args, index, arg));
            index += 1;
        } else if (arg === "--verbose") {
            patch.verbose = true;
        } else if (arg === "--log-level") {
            patch.logLevel = parseLogLevel(readOptionValue(args, index, arg));
            index += 1;
        } else if (arg === "--token") {
            patch.token = readOptionValue(args, index, arg);
            index += 1;
        } else {
            throw new Error(`unknown option: ${arg ?? "<empty>"}`);
        }
    }

    return patch;
}

function mergeConfig(...patches: readonly ConfigPatch[]): AppConfig {
    const merged: AppConfig = { ...defaultConfig };
    for (const patch of patches) {
        if (patch.mode !== undefined) {
            merged.mode = patch.mode;
        }
        if (patch.port !== undefined) {
            merged.port = patch.port;
        }
        if (patch.verbose !== undefined) {
            merged.verbose = patch.verbose;
        }
        if (patch.logLevel !== undefined) {
            merged.logLevel = patch.logLevel;
        }
        if (patch.token !== undefined) {
            merged.token = patch.token;
        }
    }
    return merged;
}

function redactSecret(value: string): string {
    if (value === "<missing>") {
        return value;
    }
    return `${value.slice(0, 3)}***${value.slice(-2)}`;
}

function redactConfig(config: AppConfig): PublicConfig {
    return {
        mode: config.mode,
        port: config.port,
        verbose: config.verbose,
        logLevel: config.logLevel,
        token: redactSecret(config.token)
    };
}

function demoConfigLayers(): void {
    section("27.4 配置分层：默认值、配置文件、环境变量、CLI 参数");
    note("C++ 对照", "最终配置应像一个明确的 struct；每一层输入先校验，再合并。");

    const configFilePatch: ConfigPatch = {
        mode: "development",
        port: 4000,
        logLevel: "info"
    };
    const envPatch = parseEnvPatch({
        APP_MODE: "production",
        APP_PORT: "5000",
        APP_TOKEN: "env-secret-token"
    });
    const cliPatch = parseCliPatch(["--port", "8080", "--verbose", "--log-level", "debug"]);
    const finalConfig = mergeConfig(configFilePatch, envPatch, cliPatch);

    showJson("配置层级合并", {
        precedence: ["defaults", "config file", "environment", "CLI"],
        final: redactConfig(finalConfig)
    });
    note("输出解释", "CLI 的 port 覆盖 env，env 的 mode 覆盖配置文件，secret 只展示脱敏值。");
    note("常见坑", "不要在日志里打印完整 process.env；那通常会泄漏 token、密码或云厂商密钥。");
}

// =============================================================================
// 27.5 cwd 与模块目录：一个是启动位置，一个是文件位置
// =============================================================================
//
// C++ 对照：cwd 类似 std::filesystem::current_path()；模块目录类似当前源文件或可执行文件
// 的物理位置。
//
// C++ 思维提示：读取用户输入文件通常相对 cwd；读取程序自带模板、schema、静态资源通常
// 相对模块目录。

function demoCwdVsModuleDirectory(): void {
    section("27.5 cwd 与模块目录：一个是启动位置，一个是文件位置");
    note("C++ 对照", "cwd 随启动目录变化，模块目录随文件位置变化。");

    const cwd = process.cwd();
    const moduleDirectory = getModuleDirectory();

    showJson("cwd 与模块目录", {
        cwdBase: basename(cwd),
        moduleDirectoryFromCwd: toPortablePath(relative(cwd, moduleDirectory)),
        userInputExample: toPortablePath(resolve("input/report.csv")),
        assetExample: toPortablePath(join(moduleDirectory, "schema.json")),
        sameDirectory: cwd === moduleDirectory
    });
    note("工程习惯", "CLI 参数中的相对路径一般按 cwd 解释；程序随包发布的资源按模块目录解释。");
}

// =============================================================================
// 27.6 exit、platform 与 arch：不要把退出当作普通控制流
// =============================================================================
//
// C++ 对照：process.exit(code) 类似从 main 返回非零状态或调用 std::exit，但它会立刻
// 终止进程，未 flush 的异步工作可能来不及完成。
//
// C++ 思维提示：库函数不要调用 process.exit；把错误返回给入口层，由入口层决定 exitCode。

function demoExitPlatformArch(): void {
    section("27.6 exit、platform 与 arch：不要把退出当作普通控制流");
    note("C++ 对照", "入口层可以决定退出码；普通业务函数应返回 Result 或抛出可处理错误。");

    const simulatedFailure = { ok: false, error: "config file is missing" };
    const recommendedExitCode = simulatedFailure.ok ? 0 : 1;

    showJson("进程平台信息与退出策略", {
        platform: process.platform,
        arch: process.arch,
        recommendedExitCode,
        currentExitCode: process.exitCode ?? "<unset>",
        libraryRule: "return error to caller; do not call process.exit inside helpers"
    });
    note("版本边界", "process.platform/arch 来自当前 Node 运行时；跨平台代码要显式处理 win32、linux、darwin 等差异。");
}

// =============================================================================
// 27.7 stdout、stderr 与 stdin：三条标准流的职责
// =============================================================================
//
// C++ 对照：process.stdout/stderr/stdin 对应 std::cout、std::cerr 和 std::cin。
//
// C++ 思维提示：CLI 的正常机器可读输出写 stdout；诊断和错误写 stderr；交互式输入要检测
// TTY，不能假设测试环境有人输入。

function demoStandardStreams(): void {
    section("27.7 stdout、stderr 与 stdin：三条标准流的职责");
    note("C++ 对照", "stdout 给正常结果，stderr 给诊断信息，stdin 读取输入。");

    showJson("标准流信息", {
        stdout: {
            writable: process.stdout.writable,
            isTTY: process.stdout.isTTY === true
        },
        stderr: {
            writable: process.stderr.writable,
            isTTY: process.stderr.isTTY === true
        },
        stdin: {
            readable: process.stdin.readable,
            isTTY: process.stdin.isTTY === true
        },
        cliRule: "machine-readable data to stdout; diagnostics to stderr"
    });
    note("常见坑", "教程和自动化测试不要等待 stdin 人工输入；交互式 CLI 要提供非交互 fallback。");
}

// =============================================================================
// 27.8 child_process：exec 适合短命令，spawn 适合流式交互
// =============================================================================
//
// C++ 对照：child_process 类似 fork+exec 或 CreateProcess；exec 会通过 shell 执行命令并
// 缓冲输出，spawn 直接启动进程并以流的方式处理 stdout/stderr。
//
// C++ 思维提示：exec 的命令字符串不要拼接未校验用户输入；需要传参时优先使用 spawn 的
// args 数组，让 Node 负责参数边界。

function quoteForCommand(value: string): string {
    return JSON.stringify(value);
}

function errorCode(error: unknown): string | undefined {
    if (typeof error !== "object" || error === null || !("code" in error)) {
        return undefined;
    }
    const code = (error as { code?: unknown }).code;
    return typeof code === "string" ? code : undefined;
}

function isChildProcessUnavailable(error: unknown): boolean {
    const code = errorCode(error);
    return code === "EPERM" || code === "EACCES";
}

function runSpawnNodeSnippet(snippet: string): Promise<SpawnResult> {
    return new Promise((resolvePromise, reject) => {
        const child = spawn(process.execPath, ["-e", snippet], {
            stdio: ["ignore", "pipe", "pipe"],
            windowsHide: true
        });

        let stdout = "";
        let stderr = "";
        child.stdout?.setEncoding("utf8");
        child.stderr?.setEncoding("utf8");
        child.stdout?.on("data", (chunk: string) => {
            stdout += chunk;
        });
        child.stderr?.on("data", (chunk: string) => {
            stderr += chunk;
        });
        child.on("error", reject);
        child.on("close", (exitCode, signal) => {
            resolvePromise({
                stdout: stdout.trim(),
                stderr: stderr.trim(),
                exitCode,
                signal
            });
        });
    });
}

async function demoChildProcess(): Promise<void> {
    section("27.8 child_process：exec 适合短命令，spawn 适合流式交互");
    note("C++ 对照", "exec 缓冲完整输出；spawn 暴露 stdout/stderr 流，更适合长输出或持续进程。");

    const execSnippet = "console.log('exec-ok')";
    const execCommand = `${quoteForCommand(process.execPath)} -e ${quoteForCommand(execSnippet)}`;
    let execSummary:
        | {
              commandShape: string;
              stdout: string;
              stderr: string;
              buffering: string;
          }
        | ChildProcessUnavailable;
    let spawnSummary:
        | ({
              argsShape: string[];
              streaming: string;
          } & SpawnResult)
        | ChildProcessUnavailable;

    try {
        const execResult = await execAsync(execCommand, {
            timeout: 2_000,
            windowsHide: true
        });
        execSummary = {
            commandShape: "node -e <snippet>",
            stdout: String(execResult.stdout).trim(),
            stderr: String(execResult.stderr).trim(),
            buffering: "returns stdout/stderr after command exits"
        };
    } catch (error) {
        if (!isChildProcessUnavailable(error)) {
            throw error;
        }
        execSummary = {
            unavailable: true,
            reason: `current environment rejected child process creation with ${errorCode(error)}`
        };
    }

    try {
        const spawnResult = await runSpawnNodeSnippet(
            "process.stdout.write('spawn-out\\n'); process.stderr.write('spawn-warn\\n');"
        );
        spawnSummary = {
            argsShape: ["-e", "<snippet>"],
            ...spawnResult,
            streaming: "stdout/stderr are streams while process is running"
        };
    } catch (error) {
        if (!isChildProcessUnavailable(error)) {
            throw error;
        }
        spawnSummary = {
            unavailable: true,
            reason: `current environment rejected child process creation with ${errorCode(error)}`
        };
    }

    showJson("exec 与 spawn 对比", {
        exec: execSummary,
        spawn: spawnSummary
    });
    note("安全边界", "exec 会经过 shell，不能拼接未校验用户输入；spawn(file, args) 更容易保持参数边界。");
    note("可运行边界", "若教学环境禁止创建子进程，本节输出受限说明；在普通 Node 环境中会运行当前 node 可执行文件。");
}

// =============================================================================
// 27.9 工程场景：CLI 入口层统一收敛外部输入
// =============================================================================
//
// C++ 对照：这类似 main() 解析 argv/env 后构造强类型配置，再调用业务函数；业务函数不应
// 直接读全局 argv/env。
//
// C++ 思维提示：入口层负责把字符串世界转换成类型世界。越早收敛，后面的代码越接近
// 普通 C++ options struct 的清晰边界。

function buildCliSummary(args: readonly string[], env: Record<string, string | undefined>): PublicConfig {
    const config = mergeConfig(parseEnvPatch(env), parseCliPatch(args));
    return redactConfig(config);
}

function demoCliBoundary(): void {
    section("27.9 工程场景：CLI 入口层统一收敛外部输入");
    note("C++ 对照", "main 负责解析 argv/env，业务函数接收已经校验过的配置对象。");

    const summary = buildCliSummary(["--mode", "production", "--port", "9000"], {
        APP_LOG_LEVEL: "warn",
        APP_TOKEN: "production-token"
    });

    showJson("CLI 边界收敛", {
        parsedConfig: summary,
        businessFunctionInput: "AppConfig with concrete mode/port/logLevel/verbose/token fields",
        errorStrategy: "return readable message and non-zero exit code at the entry layer"
    });
    note("类型边界", "TypeScript 无法信任 argv/env 的字符串内容；parseCliPatch 和 parseEnvPatch 是运行时校验层。");
    note("本章复盘", "路径、进程、环境和子进程都是运行时边界；TS 类型要和显式校验一起使用。");
}

function toPortablePath(pathText: string): string {
    return pathText.split(sep).join("/");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 27 demos. */
export async function runChapter(): Promise<void> {
    demoPathModule();
    demoEsmDirectory();
    demoProcessInputs();
    demoConfigLayers();
    demoCwdVsModuleDirectory();
    demoExitPlatformArch();
    demoStandardStreams();
    await demoChildProcess();
    demoCliBoundary();
}

await runIfMain(import.meta.url, runChapter);
