// =============================================================================
// 第 37 章：TS6 工程约束
// =============================================================================
//
// 【学习目标】
//   1. 掌握 TypeScript 6.0 项目的配置迁移重点和废弃选项边界
//   2. 理解 Node 直接 strip types、import attributes、using 等版本敏感能力的可运行边界
//   3. 建立版本锁定、依赖升级和大版本边界管理的工程流程
//
// 【运行方式】
//   npm run build
//   node dist/10_工程实践篇/chapter37_TS6工程约束.js
//   或 npm run chapter -- 37（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type BaselineSnapshot = {
    node: string;
    npm: string;
    typescript: string;
    target: string;
    module: string;
    moduleResolution: string;
    lib: string[];
    types: string[];
};

type MigrationRule = {
    topic: string;
    currentProjectChoice: string;
    reason: string;
    safeAction: string;
};

type DeprecatedOption = {
    option: string;
    oldUse: string;
    modernChoice: string;
    migrationNote: string;
};

type ErasableSyntaxCase = {
    syntax: string;
    erasable: boolean;
    reason: string;
    replacement: string;
};

type UpgradeStep = {
    step: number;
    command: string;
    purpose: string;
    boundary: string;
};

const BASELINE: BaselineSnapshot = {
    node: "24.15.0",
    npm: "11.14.1",
    typescript: "6.0.3",
    target: "ES2023",
    module: "NodeNext",
    moduleResolution: "NodeNext",
    lib: ["ES2023", "DOM"],
    types: ["node"]
};

// =============================================================================
// 37.1 本教程的可运行基线：先锁定工具链，再讨论语法
// =============================================================================
//
// C++ 对照：这类似先确定编译器版本、标准开关和平台 SDK，再讨论可用语言特性。
//
// C++ 思维提示：TS 的类型检查、JS 运行时、Node API 类型、浏览器/Web API 类型来自不同层。
// 不要因为编辑器认识某个名字，就假设当前 Node 运行时一定能直接执行它。

function demoRuntimeBaseline(): void {
    section("37.1 本教程的可运行基线：先锁定工具链，再讨论语法");
    note("C++ 对照", "先固定编译器、语言标准和平台 SDK，再判断一段代码能不能进入默认路径。");

    showJson("本地基线快照", {
        fromPlanAndLocalCheck: BASELINE,
        currentNodeProcess: process.version,
        currentTypeScriptCheck: "npx --no-install tsc --version -> Version 6.0.3",
        currentCompilerOptionsCheck: "npx --no-install tsc --all 已确认 --ignoreConfig 与 --erasableSyntaxOnly 可见",
        validationCommand: "npm run build"
    });

    note("输出解释", "本章后续所有版本敏感判断都以这个基线为边界，而不是以记忆中的旧版本行为为准。");
    note("常见坑", "把 TypeScript 版本、Node 版本和 @types/node 版本混成一个概念，会导致类型能过但运行时失败。");
}

// =============================================================================
// 37.2 rootDir：显式写出源码根，避免默认行为迁移影响产物结构
// =============================================================================
//
// C++ 对照：rootDir/outDir 像源码目录和 build 目录的映射。默认值变化会改变产物路径，
// 就像 CMake target 的输出目录变了，脚本和部署流程也会跟着坏。
//
// C++ 思维提示：迁移大版本时，最先固定的是目录映射和入口路径，而不是先追求最少配置。

function demoRootDirMigration(): void {
    section("37.2 rootDir：显式写出源码根，避免默认行为迁移影响产物结构");
    note("C++ 对照", "rootDir/outDir 对应源码树到构建产物树的稳定映射。");

    const rules: MigrationRule[] = [
        {
            topic: "rootDir",
            currentProjectChoice: "\"rootDir\": \".\"",
            reason: "教程源码、tests、练习和章节目录都位于 ts/hello 根目录下，需要保持 dist 中同构输出。",
            safeAction: "迁移 TS 6.0 时显式写 rootDir，不让默认推断决定产物路径。"
        },
        {
            topic: "outDir",
            currentProjectChoice: "\"outDir\": \"dist\"",
            reason: "所有验收命令都执行 dist 下的 JS 文件。",
            safeAction: "如果更改 outDir，必须同步 package scripts、README、plan 和 tests。"
        }
    ];

    showJson("目录映射迁移规则", rules);
    note("可运行边界", "本章不动态改写 tsconfig；只说明本教程为什么把 rootDir 写成显式值。");
    note("常见坑", "依赖 rootDir 默认值会让新增 tests 或 scripts 后的 dist 路径发生意外变化。");
}

// =============================================================================
// 37.3 types 与 lib：类型可见性要按运行环境声明
// =============================================================================
//
// C++ 对照：types/lib 像选择哪些平台 SDK 头文件可见。头文件可见不等于库一定链接成功，
// 也不等于所有运行环境都有同名 API。
//
// C++ 思维提示：Node 项目应显式写 types: ["node"]；DOM 类型来自 lib: ["DOM"]，
// 但浏览器对象和 Node 运行时对象仍是两个世界。

function demoTypesAndLibBoundary(): void {
    section("37.3 types 与 lib：类型可见性要按运行环境声明");
    note("C++ 对照", "types/lib 是可见的类型头文件集合，不是运行时能力的自动安装器。");

    showJson("本教程类型来源", [
        {
            api: "process.cwd()",
            typeSource: "@types/node 24.12.4 via types: [\"node\"]",
            runtimeSource: "Node.js 24.15.0",
            runnableInChapter: true
        },
        {
            api: "fetch()",
            typeSource: "lib DOM 与 Node Web API 类型",
            runtimeSource: "Node.js 当前版本内置 fetch",
            runnableInChapter: "只有需要网络时才使用本地 server，不依赖公网"
        },
        {
            api: "document.querySelector()",
            typeSource: "lib DOM",
            runtimeSource: "浏览器或 DOM 模拟环境",
            runnableInChapter: false
        }
    ]);

    note("类型边界", "TypeScript 看到 DOM 类型并不会让 Node 进程里自动出现 document。");
    note("常见坑", "省略 types 可能让不同机器上安装的 @types 包影响全局类型集合，教程项目必须显式收口。");
}

// =============================================================================
// 37.4 tsc file.ts 与项目配置：用 -p 或 --ignoreConfig 明确意图
// =============================================================================
//
// C++ 对照：这类似在工程根目录里绕过 CMake 直接调用编译器编译单个文件；可能缺少
// include path、宏定义、链接规则或标准开关。
//
// C++ 思维提示：教程验收必须走 npm run build，也就是 tsc -p tsconfig.json。
// 如果确实要临时编译单文件，TS 6.0 提供 --ignoreConfig 表达“我知道自己绕过项目配置”。

function demoCompilerInvocationBoundary(): void {
    section("37.4 tsc file.ts 与项目配置：用 -p 或 --ignoreConfig 明确意图");
    note("C++ 对照", "项目构建和单文件编译是两种不同意图，不能混用来做验收。");

    showJson("编译命令边界", [
        {
            command: "npm run build",
            expandsTo: "tsc -p tsconfig.json",
            useFor: "本教程默认验收",
            reason: "读取完整 tsconfig，输出 dist 供 node 执行"
        },
        {
            command: "npx --no-install tsc --version",
            expandsTo: "使用本地已安装 TypeScript",
            useFor: "确认版本",
            reason: "不临时联网解析工具版本"
        },
        {
            command: "tsc some-file.ts",
            expandsTo: "单文件入口",
            useFor: "不作为教程验收",
            reason: "在 TS 6.0 项目目录中应改用 -p，或显式加 --ignoreConfig 表达绕过配置"
        },
        {
            command: "tsc --ignoreConfig some-file.ts",
            expandsTo: "故意忽略 tsconfig 的单文件编译",
            useFor: "一次性排查",
            reason: "不会代表项目真实配置"
        }
    ]);

    note("可运行边界", "本章不执行会报错的 tsc file.ts；失败命令只作为迁移说明出现。");
    note("常见坑", "用单文件 tsc 结果证明项目能编译，是把局部检查误当成工程检查。");
}

// =============================================================================
// 37.5 TS 6.0 弃用项：老模块解析和老输出目标要从配置中退出
// =============================================================================
//
// C++ 对照：废弃编译选项类似旧 ABI、旧标准库模式或过时链接格式。继续依赖它们，
// 会让升级窗口越来越窄。
//
// C++ 思维提示：迁移不是把旧配置照搬到新版本，而是把项目运行环境重新声明清楚。

function demoDeprecatedCompilerOptions(): void {
    section("37.5 TS 6.0 弃用项：老模块解析和老输出目标要从配置中退出");
    note("C++ 对照", "废弃选项像旧标准和旧 ABI，能不用就尽早迁移。");

    const options: DeprecatedOption[] = [
        {
            option: "moduleResolution: \"node\" / \"node10\"",
            oldUse: "早期 Node/CommonJS 解析习惯",
            modernChoice: "NodeNext、node16 或 bundler",
            migrationNote: "本教程用 NodeNext，对齐 package.json type=module 和 ESM 运行方式。"
        },
        {
            option: "moduleResolution: \"classic\"",
            oldUse: "早期 TS 非 Node 解析模型",
            modernChoice: "几乎不应出现在现代 Node/前端项目",
            migrationNote: "看到 classic 时先追溯历史原因，再改成明确的现代解析策略。"
        },
        {
            option: "target: \"es5\"",
            oldUse: "兼容非常旧的浏览器",
            modernChoice: "按运行环境选择 ES2020+、ES2023 或更高",
            migrationNote: "本教程 Node 24 基线使用 ES2023，不演示 ES5 降级输出。"
        },
        {
            option: "baseUrl",
            oldUse: "配合 paths 做绝对导入",
            modernChoice: "优先 package imports、相对导入或 bundler 明确别名",
            migrationNote: "baseUrl 容易让导入路径和 Node 真实解析规则脱节。"
        },
        {
            option: "outFile + AMD/UMD/SystemJS",
            oldUse: "旧式浏览器脚本合并或非 ESM 模块系统",
            modernChoice: "ESM、NodeNext、bundler 或包管理器入口",
            migrationNote: "现代项目通常由 ESM 运行时或打包器负责模块图。"
        }
    ];

    showJson("弃用项迁移表", options);
    note("本地验证", "npx --no-install tsc --all 中 moduleResolution 可选值为 node16、nodenext、bundler。");
    note("常见坑", "为了让旧项目少改几行而保留废弃选项，会把后续 TS 7 或工具链升级变成集中爆雷。");
}

// =============================================================================
// 37.6 --erasableSyntaxOnly：Node 直接 strip types 的边界
// =============================================================================
//
// C++ 对照：erasable syntax 像只允许“能完全擦掉”的类型注解；任何会生成运行时代码的
// TS 专有语法都不能交给只会擦类型的运行器。
//
// C++ 思维提示：tsc 可以把 enum、namespace、parameter properties 编译成 JS；
// Node 直接 strip types 的路径只会擦除类型，不负责补运行时代码。

function demoErasableSyntaxOnly(): void {
    section("37.6 --erasableSyntaxOnly：Node 直接 strip types 的边界");
    note("C++ 对照", "只擦类型的执行路径类似预处理删除注解，不能替你生成额外目标代码。");

    const cases: ErasableSyntaxCase[] = [
        {
            syntax: "function parse(input: string): Config",
            erasable: true,
            reason: "参数和返回值类型擦掉后仍是合法 JavaScript 函数",
            replacement: "无需替代"
        },
        {
            syntax: "import type { Config } from \"./config.js\"",
            erasable: true,
            reason: "纯类型导入不会创建运行时依赖",
            replacement: "保留 import type"
        },
        {
            syntax: "enum Status { Ready, Failed }",
            erasable: false,
            reason: "enum 会生成运行时代码；只擦类型无法得到 Status 对象",
            replacement: "用 const 对象、字面量联合类型或普通 JS 对象"
        },
        {
            syntax: "namespace Internal { export const x = 1 }",
            erasable: false,
            reason: "runtime namespace 会生成对象包装代码",
            replacement: "用 ESM 模块边界表达命名空间"
        },
        {
            syntax: "constructor(private readonly id: string) {}",
            erasable: false,
            reason: "parameter property 会生成字段赋值",
            replacement: "显式声明字段并在构造函数中赋值"
        },
        {
            syntax: "import fs = require(\"node:fs\")",
            erasable: false,
            reason: "import = 属于 TS/CJS 兼容语法，不是标准 ESM",
            replacement: "在 ESM 中使用 import 或 createRequire"
        }
    ];

    showJson("erasable syntax 判断表", {
        optionAvailableLocally: "--erasableSyntaxOnly appears in npx --no-install tsc --all",
        currentProjectDefault: false,
        currentTutorialBoundary: "本教程仍使用 tsc 编译到 dist 后运行，不默认采用 Node 直接执行 .ts",
        cases
    });

    note("类型边界", "类型擦除不是转译器；需要生成运行时代码的 TS 语法必须经过 tsc 或改写成标准 JS。");
    note("常见坑", "看到 Node 支持 strip types 后就把 enum 带进直接运行路径，会在运行时或检查阶段失败。");
}

// =============================================================================
// 37.7 import attributes：JSON 使用 with，不使用 assert
// =============================================================================
//
// C++ 对照：JSON import 是模块加载器协议，不是把 JSON 文本粘进源码。TS 能否识别、
// Node 能否加载、打包器是否改写，是三个边界。
//
// C++ 思维提示：新代码使用 with { type: "json" }。不要把旧 assert 写法复制到新教程。

function demoImportAttributes(): void {
    section("37.7 import attributes：JSON 使用 with，不使用 assert");
    note("C++ 对照", "import attributes 是加载器元数据，类似告诉链接/加载系统该资源的类型。");

    showJson("JSON import 写法边界", {
        recommended: "import packageJson from \"./package.json\" with { type: \"json\" };",
        avoidInNewCode: "import packageJson from \"./package.json\" assert { type: \"json\" };",
        currentTsconfig: {
            module: BASELINE.module,
            moduleResolution: BASELINE.moduleResolution,
            resolveJsonModule: false
        },
        chapterDecision: "本章只展示语法片段，不把 JSON import 放进可运行路径。"
    });

    note("可运行边界", "如果未来章节要直接 import JSON，需要同步打开 resolveJsonModule，并用 build + dist + node 验证。");
    note("常见坑", "只在打包器里跑通过 JSON import，不代表 NodeNext + tsc + Node 直接执行也通过。");
}

// =============================================================================
// 37.8 using 与显式资源管理：现代能力，但不是 TS 6.0 独有标签
// =============================================================================
//
// C++ 对照：using/显式资源管理接近 RAII 的意图：离开作用域时调用清理逻辑。但 JS 的资源
// 生命周期仍要看当前运行时、lib 类型和具体对象是否实现 Symbol.dispose。
//
// C++ 思维提示：不要把“TS 能解析某个现代语法”理解成“当前项目默认可以到处使用”。

function demoExplicitResourceManagement(): void {
    section("37.8 using 与显式资源管理：现代能力，但不是 TS 6.0 独有标签");
    note("C++ 对照", "显式资源管理借鉴 RAII 的收口思路，但 JS 仍依赖运行时协议和类型库声明。");

    const symbolConstructor = Symbol as SymbolConstructor & {
        readonly dispose?: symbol;
        readonly asyncDispose?: symbol;
    };

    showJson("using 可运行边界", {
        syntaxSnippet: [
            "using file = openDisposableFile(path);",
            "// scope exit calls file[Symbol.dispose]()"
        ],
        localFeatureDetect: {
            symbolDisposeType: typeof symbolConstructor.dispose,
            symbolAsyncDisposeType: typeof symbolConstructor.asyncDispose
        },
        currentLib: BASELINE.lib,
        chapterDecision: "当前 tsconfig 未加入 esnext.disposable；本章不在默认运行路径使用 using 语法。",
        teachingRule: "可以作为现代 JS/TS 能力讲解，但不得写成 TS 6.0 独有特性。"
    });

    note("类型边界", "即使语法可用，对象也必须实现 dispose 协议；否则不会自动知道如何关闭资源。");
    note("常见坑", "把 using 当成所有资源的自动清理魔法，会忽略 server、watcher、stream 的实际关闭 API。");
}

// =============================================================================
// 37.9 版本锁定：package.json 表达范围，package-lock 锁定精确版本
// =============================================================================
//
// C++ 对照：这类似声明“允许 6.0.x 补丁升级”，同时把当前构建实际用到的工具链哈希锁住。
//
// C++ 思维提示：package.json 是意图，package-lock.json 是可复现安装记录；二者缺一不可。

function demoVersionLocking(): void {
    section("37.9 版本锁定：package.json 表达范围，package-lock 锁定精确版本");
    note("C++ 对照", "版本范围像允许补丁升级的工具链约束，lockfile 像一次构建的精确依赖清单。");

    showJson("版本锁定策略", {
        packageJson: {
            typescript: "~6.0.3",
            "@types/node": "24.12.4",
            engines: {
                node: "24.15.0"
            },
            packageManager: "npm@11.14.1"
        },
        packageLock: {
            typescript: "6.0.3",
            "@types/node": "24.12.4",
            undiciTypes: "7.16.0"
        },
        commands: [
            "npm ci",
            "npm run build",
            "npm test",
            "node dist/<chapter>.js"
        ]
    });

    note("输出解释", "~6.0.3 允许 6.0 补丁版本；lockfile 让 CI 和团队机器安装同一个精确版本。");
    note("常见坑", "只提交 package.json 不提交 lockfile，会让团队成员在不同时间安装到不同补丁组合。");
}

// =============================================================================
// 37.10 依赖升级工作流：先评估，再小步升级，再全量验收
// =============================================================================
//
// C++ 对照：这类似升级编译器或标准库：先看变更范围，再分批升级，最后跑完整构建和测试。
//
// C++ 思维提示：升级依赖不是“改版本号然后祈祷”。每一步都应有可回滚、可验证的 checkpoint。

function demoDependencyUpgradeWorkflow(): void {
    section("37.10 依赖升级工作流：先评估，再小步升级，再全量验收");
    note("C++ 对照", "升级 TS/npm 依赖像升级编译器，必须让构建、测试和运行样例共同验收。");

    const steps: UpgradeStep[] = [
        {
            step: 1,
            command: "npm outdated",
            purpose: "评估可升级范围",
            boundary: "会访问 registry；不放入本章可运行路径"
        },
        {
            step: 2,
            command: "npm install --save-dev typescript@~6.0.x",
            purpose: "小步升级补丁版本",
            boundary: "升级后必须检查 package-lock 变化"
        },
        {
            step: 3,
            command: "npm ci",
            purpose: "从 lockfile 复现干净安装",
            boundary: "CI 和本地都应能复现"
        },
        {
            step: 4,
            command: "npm run build",
            purpose: "确认类型检查和 emit 通过",
            boundary: "默认验收命令，不使用裸 npx tsx"
        },
        {
            step: 5,
            command: "npm test",
            purpose: "确认章节、练习、项目测试通过",
            boundary: "如果受限环境 spawn EPERM，应在允许子进程的环境重跑"
        },
        {
            step: 6,
            command: "node dist/<对应章节>.js",
            purpose: "抽查或全量运行章节入口",
            boundary: "所有副作用必须可重复运行并清理资源"
        }
    ];

    showJson("升级 checklist", steps);
    note("可运行边界", "本章不执行 npm outdated 或 npm install，避免联网和修改 lockfile。");
    note("常见坑", "只跑 tsc 不跑章节入口，会漏掉运行时 API、资源清理和输出稳定性问题。");
}

// =============================================================================
// 37.11 TS 7 Beta 边界：本教程不默认采用下一代行为
// =============================================================================
//
// C++ 对照：这类似团队在 C++20 教程中可以提到 C++23/26，但默认代码仍按当前标准编译。
//
// C++ 思维提示：预览版能力适合跟踪，不适合作为入门教程的默认运行路径，除非已在本项目
// package.json、tsconfig、测试和文档中一起升级。

function demoTs7Boundary(): void {
    section("37.11 TS 7 Beta 边界：本教程不默认采用下一代行为");
    note("C++ 对照", "可以了解下一代标准，但默认示例必须按当前工程标准可编译可运行。");

    showJson("TS 7 Beta 边界梳理", [
        {
            topic: "--erasableSyntaxOnly 默认行为",
            tutorialDecision: "TS 6.0 项目不默认采用；本章只解释边界",
            requiredBeforeAdoption: "更新 package.json、tsconfig、README、测试和章节运行方式"
        },
        {
            topic: "装饰器 Stage 3 规范",
            tutorialDecision: "ch21 已用现代装饰器教学；不引入 legacy emitDecoratorMetadata 路径",
            requiredBeforeAdoption: "继续以本地 tsc 和 Node 运行结果为准"
        },
        {
            topic: "类型导入 elision 行为变化",
            tutorialDecision: "当前项目继续依赖 verbatimModuleSyntax，纯类型使用 import type",
            requiredBeforeAdoption: "逐文件检查运行时 import 是否仍符合预期"
        }
    ]);

    note("类型边界", "大版本预览的默认值变化可能改变 emit 和运行时依赖图；教程默认路径必须保持可复现。");
    note("常见坑", "把 Beta 博客里的默认行为写进 TS 6.0 教程，会让读者在本地按教程运行时踩版本错位。");
}

// =============================================================================
// 37.12 工程迁移复盘：像管理 C++ 标准版本一样管理 TS 演进
// =============================================================================
//
// C++ 对照：C++11/14/17/20 的升级通常需要编译器、标准库、构建系统和代码风格一起调整；
// TS 也要同时管理编译器版本、Node 版本、类型包、tsconfig 和运行命令。
//
// C++ 思维提示：TS 的向后兼容通常更平滑，但“更平滑”不等于“可以无验证升级”。

function demoMigrationReview(): void {
    section("37.12 工程迁移复盘：像管理 C++ 标准版本一样管理 TS 演进");
    note("C++ 对照", "TS 大版本管理和 C++ 标准升级一样，要有版本、配置、验证和回滚边界。");

    showJson("本教程 TS6 checkpoint", {
        configuration: [
            "rootDir/outDir explicit",
            "module/moduleResolution = NodeNext",
            "types = [node]",
            "lib = [ES2023, DOM]",
            "strict family enabled"
        ],
        commands: [
            "npm run build",
            "node dist/<chapter>.js",
            "npm run chapter -- <id> after registry sync",
            "npm test for phase or global validation"
        ],
        versionSensitiveRules: [
            "do not use bare npx tsx for validation",
            "do not depend on public network in runnable chapters",
            "do not run preview syntax unless tsconfig and runtime boundary are verified",
            "keep package-lock in sync with package.json"
        ],
        cxxAnalogy: "compiler + standard flag + SDK headers + build graph + test suite"
    });

    note("真实场景", "团队升级 TS 版本时，应把这个 checkpoint 放进 PR 描述和 CI 验收，而不是只看本机编译。");
    note("本章复盘", "工程约束的目标不是限制写法，而是让教程、团队机器和 CI 都能重复得到同一个结果。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 37 demos. */
export function runChapter(): void {
    demoRuntimeBaseline();
    demoRootDirMigration();
    demoTypesAndLibBoundary();
    demoCompilerInvocationBoundary();
    demoDeprecatedCompilerOptions();
    demoErasableSyntaxOnly();
    demoImportAttributes();
    demoExplicitResourceManagement();
    demoVersionLocking();
    demoDependencyUpgradeWorkflow();
    demoTs7Boundary();
    demoMigrationReview();
}

await runIfMain(import.meta.url, runChapter);
