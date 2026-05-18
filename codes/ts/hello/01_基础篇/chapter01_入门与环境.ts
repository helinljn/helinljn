// =============================================================================
// 第 1 章：入门与环境
// =============================================================================
//
// 【学习目标】
//   1. 分清 TypeScript、JavaScript、Node.js、npm 在工程中的职责
//   2. 掌握 npm run build/start/chapter/test 的基本工作流
//   3. 读懂本教程最小 tsconfig.json 和 NodeNext ESM 约束
//   4. 理解第一段 TypeScript 程序从源码到 dist JavaScript 的路径
//
// 【运行方式】
//   npm run build
//   node dist/01_基础篇/chapter01_入门与环境.js
//   或 npm run chapter -- 1（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type ToolRole = {
    readonly name: string;
    readonly responsibility: string;
    readonly cxxAnalogy: string;
};

type ScriptCommand = {
    readonly script: string;
    readonly command: string;
    readonly whenToUse: string;
};

type CompilerOptionSummary = {
    readonly option: string;
    readonly value: string;
    readonly meaning: string;
};

type EnvironmentCheck = {
    readonly nodeVersion: string;
    readonly npmScript: string;
    readonly moduleSystem: string;
    readonly sourceKind: string;
};

// =============================================================================
// 1.1 先分清 TS、JS、Node、npm
// =============================================================================
//
// C++ 对照：
//   C++ 项目里经常把编译器、构建系统、标准库、运行平台放在同一个心智模型里。
//   TypeScript 工程更需要拆开看：tsc 做类型检查和转译，Node 执行 JS，npm 负责依赖与脚本入口。
//
// 常见坑 1：
//   “TypeScript 支持某类型”不等于“运行时有这个对象”。
//   interface、type、泛型参数都会在编译后被擦除。
//
// 常见坑 2：
//   “Node 能运行”不等于“浏览器 API 都存在”。
//   本教程的 lib 包含 DOM，是为了类型边界和 fetch 等 API；Node 仍不是浏览器。

function demoRuntimeLayers(): void {
    section("1.1 TS、JS、Node、npm 的职责边界");
    note("C++ 对照", "不要把 TypeScript 当作 C++ 那样的完整运行时语言；它主要在编译期工作。");

    const roles: readonly ToolRole[] = [
        {
            name: "TypeScript",
            responsibility: "提供类型系统、编译期检查、把 .ts 转成 .js",
            cxxAnalogy: "更接近编译检查工具链的一部分，不是运行时平台"
        },
        {
            name: "JavaScript",
            responsibility: "编译后真正运行的语言，负责对象、闭包、Promise 等语义",
            cxxAnalogy: "相当于最终被执行的目标语言语义"
        },
        {
            name: "Node.js",
            responsibility: "执行 JavaScript，并提供文件、网络、进程、Buffer 等 API",
            cxxAnalogy: "类似运行时 + 平台库，不等同于 TS 编译器"
        },
        {
            name: "npm",
            responsibility: "安装依赖、锁定版本、运行 package.json 中的 scripts",
            cxxAnalogy: "部分职责接近包管理器加项目命令入口"
        }
    ];

    showJson("工程角色拆分", roles);
    note("输出解释", "上面的列表刻意把类型检查、运行语言、平台能力和命令入口分开。");
}

// =============================================================================
// 1.2 本机环境验证
// =============================================================================
//
// C++ 对照：
//   C++ 项目通常要确认 g++/clang、CMake、运行库版本。
//   本教程要确认 Node、npm、TypeScript 依赖和 package-lock.json 锁定版本。
//
// 当前章节只读取 Node 自身提供的稳定信息，不执行外部命令。
// 真正的版本验收命令在 plan.md 中规定为：
//   node --version
//   npm run build
//   npx --no-install tsc --version
//   npx --no-install tsc --all

function demoEnvironmentCheck(): void {
    section("1.2 Node.js/npm 安装验证");
    note("C++ 对照", "先确认工具链版本，再讨论语言细节；这和确认编译器标准版本是同一类动作。");

    const check: EnvironmentCheck = {
        nodeVersion: process.version,
        npmScript: "本章通过 npm run build 后运行 dist 文件验证",
        moduleSystem: "package.json 使用 \"type\": \"module\"，tsconfig 使用 NodeNext",
        sourceKind: "源码是 .ts，运行时入口是 dist 中的 .js"
    };

    showJson("当前运行环境摘要", check);
    note("版本边界", "本教程本地基线是 Node.js 24.15.0 与 TypeScript 6.0.3；若行为冲突，以本地命令验证为准。");
}

// =============================================================================
// 1.3 package.json 与 npm scripts
// =============================================================================
//
// C++ 对照：
//   package.json 不是 CMakeLists.txt，但可以把它理解为项目元数据和常用命令表。
//   scripts 字段给团队一个统一入口，避免每个人手写不同命令。
//
// 常见坑：
//   不要使用未锁定的临时执行命令作为验收依据。
//   本教程不使用裸 npx tsx；默认先 npm run build，再 node dist/...。

function demoPackageScripts(): void {
    section("1.3 package.json 与 npm scripts 工作流");
    note("C++ 对照", "npm scripts 像项目内约定的 make target，但它实际只是 npm 调用命令字符串。");

    const scripts: readonly ScriptCommand[] = [
        {
            script: "build",
            command: "tsc -p tsconfig.json",
            whenToUse: "做类型检查并输出 dist JavaScript"
        },
        {
            script: "start",
            command: "node dist/main.js",
            whenToUse: "先 build，再运行教程主菜单"
        },
        {
            script: "chapter",
            command: "node dist/main.js chapter",
            whenToUse: "先 build，再按编号运行已注册章节"
        },
        {
            script: "test",
            command: "node --test dist/tests/...",
            whenToUse: "先 build，再运行 Node 内置测试"
        }
    ];

    showJson("常用脚本", scripts);
    note("可复现性", "所有验收都基于本项目 package-lock.json 中已安装的依赖，不临时联网解析工具版本。");
}

// =============================================================================
// 1.4 第一个 TypeScript 函数
// =============================================================================
//
// C++ 对照：
//   参数和返回值标注看起来像 C++ 函数签名，但 TS 标注只在编译期存在。
//   编译后的 JavaScript 不会保留 name: string 或返回类型 : string。
//
// 类型边界：
//   这个函数接收 string。TypeScript 会在编译期阻止明显错误调用。
//   如果外部 JSON 在运行时传入了非 string，仍要在边界做 runtime validation。

/** Build a deterministic greeting used by the first runnable TypeScript demo. */
export function hello(name: string): string {
    return `Hello, ${name}. Welcome to TypeScript 6.0.`;
}

/** Sum a readonly list of numbers without mutating the input array. */
export function sumNumbers(values: readonly number[]): number {
    return values.reduce((total, value) => total + value, 0);
}

/** Classify special JavaScript number values for the environment demo. */
export function describeNumber(value: number): string {
    if (Number.isNaN(value)) {
        return "not-a-number";
    }
    if (!Number.isFinite(value)) {
        return "infinite";
    }
    return Number.isInteger(value) ? "integer" : "floating-point";
}

function demoFirstProgram(): void {
    section("1.4 第一个 TypeScript 程序");
    note("C++ 对照", "函数签名像静态语言，但运行时只剩 JavaScript 函数和值。");

    const message = hello("C++ developer");
    const total = sumNumbers([1, 2, 3, 4]);
    const classifications = [
        describeNumber(42),
        describeNumber(3.14),
        describeNumber(Number.NaN),
        describeNumber(Number.POSITIVE_INFINITY)
    ];

    console.log(message);
    console.log(`sumNumbers([1, 2, 3, 4]) = ${total}`);
    showJson("数字分类", classifications);
    note("输出解释", "NaN 和 Infinity 都属于 number 类型，但它们代表特殊运行时数值。");
}

// =============================================================================
// 1.5 tsconfig.json 最小配置
// =============================================================================
//
// C++ 对照：
//   tsconfig.json 可以类比为编译器选项文件。
//   它不描述链接本地机器码，但会决定 TS 如何检查模块、输出目标和类型库。
//
// 线性阅读提示：
//   NodeNext、ESM、类型库会在后续模块章节深入。
//   现在只需要知道：本教程用现代 Node ESM，源码 import 写 .js 后缀。

function demoTsConfig(): void {
    section("1.5 tsconfig.json 的关键选项");
    note("C++ 对照", "这一步类似先读 CMake/toolchain 的关键编译选项，再读源码。");

    const options: readonly CompilerOptionSummary[] = [
        {
            option: "target",
            value: "ES2023",
            meaning: "输出 JavaScript 的语言目标；本教程不向非常旧的 JS 环境降级"
        },
        {
            option: "module",
            value: "NodeNext",
            meaning: "按现代 Node ESM/CJS 规则生成模块代码"
        },
        {
            option: "moduleResolution",
            value: "NodeNext",
            meaning: "TypeScript 按 NodeNext 规则解析 import 路径"
        },
        {
            option: "rootDir/outDir",
            value: "./ 到 dist/",
            meaning: "源码从项目根组织，编译产物输出到 dist"
        },
        {
            option: "strict",
            value: "true",
            meaning: "开启严格类型检查，避免把入门阶段养成宽松习惯"
        },
        {
            option: "types",
            value: "[\"node\"]",
            meaning: "显式引入 Node API 类型，如 process、node:fs、node:http"
        }
    ];

    showJson("关键编译配置", options);
    note("版本敏感", "TS 6.0 对 rootDir、types、命令行 tsc file.ts 等行为有迁移约束，后续 ch37 会集中说明。");
}

// =============================================================================
// 1.6 NodeNext 下为什么源码 import 写 .js
// =============================================================================
//
// C++ 对照：
//   #include 通常指向头文件，编译器会在预处理阶段处理文本。
//   ESM import 是模块加载声明，Node 运行时最终按 dist 中的 .js 文件解析。
//
// 常见坑：
//   看到 TypeScript 源码中 import "../shared/chapter.js" 时，不要改成 .ts。
//   在 NodeNext ESM 项目里，源码写 .js 后缀是为了让编译后的文件能被 Node 正确加载。

function demoNodeNextImports(): void {
    section("1.6 NodeNext ESM 与 .js 后缀 import");
    note("C++ 对照", "ESM import 不是文本包含；它同时影响类型检查和运行时模块解析。");

    const importFacts = [
        "源码文件是 chapter01_入门与环境.ts",
        "编译后运行文件是 dist/01_基础篇/chapter01_入门与环境.js",
        "源码中的相对 import 写 .js，TypeScript 会在检查时映射到对应 .ts",
        "Node 运行 dist 时只认识真实存在的 .js 产物"
    ];

    showJson("NodeNext import 规则摘要", importFacts);
    note("输出解释", "这条规则会贯穿所有章节，所以本章先建立直觉，后续模块篇再深入。");
}

// =============================================================================
// 1.7 VS Code 与线性学习方式
// =============================================================================
//
// C++ 对照：
//   IDE 能提示类型错误，但真正验收仍应来自统一命令。
//   就像编辑器里的 clangd 提示不能替代 CI 编译一样。
//
// 本教程的章节是线性教材。
// 前面章节只使用已经解释过的概念，或用“先知道即可”标注过渡。

function demoEditorWorkflow(): void {
    section("1.7 编辑器与学习工作流");
    note("C++ 对照", "编辑器类型提示是反馈工具；项目能否通过 build/test 才是团队共享事实。");

    const workflow = [
        "先读章节顶部学习目标",
        "逐个运行 demo 函数对应的小节",
        "遇到类型错误先判断是 TS 检查还是 JS 运行时问题",
        "每章结束后运行 npm run build 和 dist 入口",
        "阶段结束后再运行 npm test 或主菜单"
    ];

    showJson("推荐学习步骤", workflow);
    note("工程习惯", "同一套命令能减少“我这里能跑”的差异，是团队教程长期维护的基础。");
}

// =============================================================================
// 1.8 一个贴近工程实践的小例子：命令表校验
// =============================================================================
//
// C++ 对照：
//   C++ 项目经常维护 README、脚本和 CI 配置的一致性。
//   TS 项目也一样：文档中的命令、package.json scripts、测试入口必须保持一致。
//
// 真实场景：
//   这里用一个小的命令表生成帮助文本。
//   这类数据结构后续可以被 README、CLI 帮助和测试共享，减少重复。

function formatScriptHelp(commands: readonly ScriptCommand[]): string[] {
    return commands.map((item) => `${item.script.padEnd(8)} -> ${item.command}`);
}

function demoProjectStyleCommandTable(): void {
    section("1.8 工程实践：从结构化命令表生成帮助");
    note("C++ 对照", "把命令元数据集中维护，类似把构建 target 和文档约束放在同一套来源中。");

    const commands: readonly ScriptCommand[] = [
        {
            script: "build",
            command: "npm run build",
            whenToUse: "提交前确认 TS 编译通过"
        },
        {
            script: "chapter",
            command: "npm run chapter -- 1",
            whenToUse: "运行已注册的指定章节"
        },
        {
            script: "test",
            command: "npm test",
            whenToUse: "运行测试套件"
        }
    ];

    showJson("结构化命令表", commands);
    showJson("生成的帮助行", formatScriptHelp(commands));
    note("输出解释", "命令表本身是数据，帮助文本是派生结果；这比复制粘贴三处字符串更容易维护。");
}

// =============================================================================
// 1.9 本章复盘
// =============================================================================
//
// C++ 对照：
//   建立工具链边界后，再进入语法学习会更稳。
//   后续章节中的类型、模块、异步和 Node API 都会回到本章的三层模型。

function demoChapterReview(): void {
    section("1.9 本章复盘");
    note("C++ 对照", "先确认工具链，再写代码；先分清编译期和运行时，再判断 bug 来源。");

    const review = [
        "TypeScript 类型会在编译后擦除",
        "Node.js 执行 dist 中的 JavaScript",
        "npm scripts 是本项目统一命令入口",
        "NodeNext ESM 源码相对 import 写 .js 后缀",
        "章节验收使用 npm run build + node dist/..."
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 1. */
export function runChapter(): void {
    demoRuntimeLayers();
    demoEnvironmentCheck();
    demoPackageScripts();
    demoFirstProgram();
    demoTsConfig();
    demoNodeNextImports();
    demoEditorWorkflow();
    demoProjectStyleCommandTable();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
