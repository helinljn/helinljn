// =============================================================================
// 第 19 章：模块系统
// =============================================================================
//
// 【学习目标】
//   1. 理解 ESM 的 import/export、默认导出、命名导出与 import type
//   2. 掌握 NodeNext ESM 下 package.json、tsconfig.json 与 .js 后缀规则的联动
//   3. 认识 package exports/imports、JSON import attributes 与 CJS/ESM 互操作边界
//   4. 能用 barrel export 组织公共 API，同时避免循环依赖和内部实现泄漏
//
// 【运行方式】
//   npm run build
//   node dist/06_模块与工程篇/chapter19_模块系统.js
//   或 npm run chapter -- 19（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import type { MaybePromise } from "../shared/chapter.js";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type ExportSurface = {
    moduleFile: string;
    runtimeExports: readonly string[];
    typeOnlyExports: readonly string[];
    consumerImport: string;
};

type ModuleResolutionStep = {
    step: number;
    actor: string;
    decision: string;
};

type InteropRule = {
    scenario: string;
    safePattern: string;
    boundary: string;
};

type BarrelItem = {
    file: string;
    publicApi: readonly string[];
    privateImplementation: readonly string[];
};

function printSnippet(label: string, lines: readonly string[]): void {
    console.log(`${label}:`);
    for (const line of lines) {
        console.log(`  ${line}`);
    }
}

function describeRunResult(run: () => MaybePromise<void>): string {
    const result = run();
    return result instanceof Promise
        ? "返回 Promise<void>：异步章节入口"
        : "返回 void：同步章节入口";
}

// =============================================================================
// 19.1 模块是运行时边界，也是类型边界
// =============================================================================
//
// TypeScript 的模块不是“把几个文件粘在一起”的文本包含机制。
// 只要文件里出现顶层 import 或 export，它就成为一个模块：
//   - 顶层变量不会进入全局作用域
//   - 导入方只能看到被显式 export 的名字
//   - 类型检查器会沿着 import/export 关系解析类型
//   - Node 运行时会沿着编译后的 JS import/export 关系加载文件
//
// C++ 对照：
//   #include 是预处理期的文本展开，容易把宏、全局变量和声明顺序扩散到翻译单元。
//   TS/ESM 更接近现代 C++ module：导出面是显式的，未导出的实现细节不可见。
//   但 TS 的类型会在编译后擦除，真正的运行时边界仍由 JS 模块加载器负责。

function demoModuleBoundary(): void {
    section("19.1 模块是运行时边界，也是类型边界");
    note("C++ 对照", "#include 是文本包含；ESM import/export 是显式依赖边界。");

    const surfaces: readonly ExportSurface[] = [
        {
            moduleFile: "parser.ts",
            runtimeExports: ["parseConfig", "formatError"],
            typeOnlyExports: ["ConfigAst", "ConfigError"],
            consumerImport: 'import { parseConfig } from "./parser.js";'
        },
        {
            moduleFile: "parser.internal.ts",
            runtimeExports: ["tokenizeForTests"],
            typeOnlyExports: ["Token"],
            consumerImport: "不从公共入口导出；只在模块内部或测试中使用"
        }
    ];

    showJson("一个模块的公共表面", surfaces);
    note("边界", "未 export 的名字不会被导入方访问；但 private 不是安全边界，编译后的 JS 仍是普通文件。");
    note("常见坑", "把模块当作 C++ 头文件复制声明，会导致重复的运行时代码；公共 API 应从实现中导出。");
}

// =============================================================================
// 19.2 命名导出、默认导出与 import type
// =============================================================================
//
// ESM 有两类常用导出：
//   - 命名导出：export function parse() {}
//   - 默认导出：export default class Parser {}
//
// 命名导出适合库和团队代码，因为重命名、搜索、重构更明确。
// 默认导出适合一个文件只表达一个主对象的场景，例如 React 组件或 CLI 主函数。
//
// TypeScript 还提供 import type：
//   import type { Config } from "./config.js";
// 这行只参与类型检查，编译到 JS 后会被擦除，不会产生运行时加载。
//
// C++ 对照：
//   import type 有点像只引入声明，不引入对象文件里的符号。
//   但 TS 的“声明”只服务编译器，运行时不会检查这个类型是否存在。

function demoExportForms(): void {
    section("19.2 命名导出、默认导出与 import type");
    note("C++ 对照", "命名导出像显式列出的模块接口；import type 只服务编译器，运行时被擦除。");

    printSnippet("命名导出示例", [
        'export function parseConfig(text: string): Config { /* ... */ }',
        'export function formatConfig(config: Config): string { /* ... */ }',
        'import { parseConfig, formatConfig } from "./config.js";'
    ]);

    printSnippet("默认导出示例", [
        "export default class ConfigLoader { /* ... */ }",
        'import ConfigLoader from "./ConfigLoader.js";'
    ]);

    printSnippet("import type 示例", [
        'import type { Config } from "./config.js";',
        "export function validate(config: Config): boolean { /* ... */ }"
    ]);

    const chapterEntryShape = describeRunResult(() => undefined);
    showJson("本章用 import type 约束章节入口形状", {
        importedType: "MaybePromise<void>",
        runtimeImportCreated: false,
        sampleResult: chapterEntryShape
    });

    note("类型边界", "import type 不能读取运行时值；如果后续要调用函数，必须使用普通 import。");
    note("常见坑", "在 verbatimModuleSyntax 下把纯类型写成普通 import，可能留下无意义的运行时依赖。");
}

// =============================================================================
// 19.3 import.meta 与 TypeScript 源码中的 .js 后缀
// =============================================================================
//
// 本教程的 tsconfig.json 使用：
//   module: "NodeNext"
//   moduleResolution: "NodeNext"
// package.json 又设置：
//   "type": "module"
//
// 这表示源文件按 Node 的 ESM 规则解释。Node 在运行 dist/*.js 时不会自动猜测
// "./shared/chapter" 对应 "./shared/chapter.js"，所以 TS 源码里也要写：
//   import { section } from "../shared/chapter.js";
//
// 这里的 .js 不是写错了。TypeScript 会检查 ../shared/chapter.ts，
// 但保留 import specifier，让编译产物在 dist 中继续指向 ../shared/chapter.js。
//
// C++ 对照：
//   C++ 的 include 路径通常指向真实的 .h/.hpp 文件。
//   NodeNext 下的 TS 源码写的是“未来 JS 产物的运行时路径”。

function demoImportMetaAndJsSuffix(): void {
    section("19.3 import.meta 与 TypeScript 源码中的 .js 后缀");
    note("C++ 对照", "NodeNext 源码里的 .js 后缀是运行时模块路径，不是当前文件扩展名。");

    const currentModule = {
        importMetaUrl: import.meta.url,
        directoryUrl: new URL(".", import.meta.url).href,
        directRunCheck: "shared/runIfMain 会把 import.meta.url 与 process.argv[1] 转成 file URL 后比较"
    };

    showJson("import.meta 在 ESM 中提供模块自身信息", currentModule);

    const resolutionSteps: readonly ModuleResolutionStep[] = [
        {
            step: 1,
            actor: "TypeScript 类型检查器",
            decision: "看到 ../shared/chapter.js，按 NodeNext 规则找到源码 ../shared/chapter.ts"
        },
        {
            step: 2,
            actor: "tsc emit",
            decision: "输出 JS 时保留 ../shared/chapter.js 这个 specifier"
        },
        {
            step: 3,
            actor: "Node 运行时",
            decision: "执行 dist/chapter19_模块系统.js 时加载 dist/shared/chapter.js"
        }
    ];
    showJson("NodeNext 解析链路", resolutionSteps);

    note("常见坑", "不要在本项目源码里写 ./foo.ts；allowImportingTsExtensions 只适合 noEmit 或声明产物场景。");
    note("可运行边界", "本章只使用 import.meta.url；不依赖打包器私有变量，也不使用 CommonJS 的 __dirname。");
}

// =============================================================================
// 19.4 package.json 的 type、exports 与 imports
// =============================================================================
//
// package.json 同时影响“别人怎么消费你的包”和“你的包内部怎么写导入”。
//
//   "type": "module"
//     让 .js 文件默认按 ESM 解释；如果没有它，Node 会把 .js 当作 CommonJS。
//
//   "exports"
//     定义包的公共入口。没有列出的文件即使存在，也不应该被外部依赖。
//
//   "imports"
//     定义包内部的别名，Node 要求以 # 开头，例如 "#/config" 或 "#internal/*"。
//     这和 tsconfig paths 不同：imports 是 Node 运行时认识的 package 级规则。
//
// C++ 对照：
//   exports 像库公开安装的头文件集合；internal 目录像未安装的私有头文件。
//   依赖方不应该 include 你的 src/detail/*.hpp，也不应该 import 你的 dist/internal/*.js。

function demoPackageBoundaries(): void {
    section("19.4 package.json 的 type、exports 与 imports");
    note("C++ 对照", "package exports 像库的公开头文件清单；未导出的文件属于实现细节。");

    const packageJsonShape = {
        type: "module",
        exports: {
            ".": {
                types: "./dist/index.d.ts",
                import: "./dist/index.js"
            },
            "./cli": "./dist/cli.js"
        },
        imports: {
            "#internal/*": "./dist/internal/*.js",
            "#/config": "./dist/config/index.js"
        }
    };

    showJson("库项目常见 package 边界", packageJsonShape);

    printSnippet("消费方导入", [
        'import { parseConfig } from "acme-config";',
        'import { runCli } from "acme-config/cli";',
        '// 不要写：import { helper } from "acme-config/dist/internal/helper.js";'
    ]);

    printSnippet("包内部 subpath imports", [
        'import { readDefaults } from "#/config";',
        'import { normalizePath } from "#internal/path.js";',
        "// # 前缀是 Node package imports 的要求，用来避免和真实 npm 包名混淆"
    ]);

    note("工程场景", "发布库时先设计 exports，再决定目录结构；否则用户会依赖内部路径，后续重构成本很高。");
    note("类型边界", "exports 需要同时照顾运行时 JS 入口和 .d.ts 类型入口，二者不一致会造成编辑器能过、运行时失败。");
}

// =============================================================================
// 19.5 JSON import attributes：使用 with，不使用 assert
// =============================================================================
//
// 现代 ESM 导入 JSON 时使用 import attributes：
//
//   import packageJson from "../package.json" with { type: "json" };
//
// 旧写法 assert { type: "json" } 已经不应作为新代码模板。
//
// 当前教程的 tsconfig.json 使用 NodeNext，TypeScript 6.0 的本地帮助显示
// resolveJsonModule 在 node16/nodenext/bundler 下默认可用；Node 24 也支持 JSON
// import attributes。即便如此，本章仍把它作为语法边界展示，而不是让章节运行依赖
// 某个外部 JSON 文件，因为模块系统章节的重点是规则，不是读取配置文件。
//
// C++ 对照：
//   这不像 include 一个 .json 文本；它是运行时模块加载行为，加载器需要知道文件类型。

function demoJsonImportAttributes(): void {
    section("19.5 JSON import attributes：使用 with，不使用 assert");
    note("C++ 对照", "JSON import 是模块加载器行为，不是预处理器把 JSON 文本贴进源码。");

    printSnippet("推荐写法", [
        'import packageJson from "../package.json" with { type: "json" };',
        'console.log(packageJson.name);'
    ]);

    printSnippet("不作为新模板的旧写法", [
        'import packageJson from "../package.json" assert { type: "json" };',
        "// assert 写法已经过时，新代码使用 with { type: \"json\" }"
    ]);

    showJson("本教程采用的可运行边界", {
        node: "package.json engines.node = 24.15.0",
        typescript: "devDependency typescript = ~6.0.3",
        module: "NodeNext",
        moduleResolution: "NodeNext",
        chapterRuntime: "本章打印语法片段，不把 JSON 文件作为运行时依赖"
    });

    note("常见坑", "只让 TS 识别 JSON 不够，运行时也必须支持 import attributes；不要用打包器行为推断 Node 行为。");
}

// =============================================================================
// 19.6 CommonJS / ESM 互操作边界
// =============================================================================
//
// Node 生态长期同时存在 CommonJS 和 ESM：
//   - CommonJS 使用 require/module.exports
//   - ESM 使用 import/export
//
// TypeScript 可以帮助类型检查，但不能消除运行时加载器差异。
// 在 NodeNext 项目中，推荐把新代码统一写成 ESM：
//   - 消费 CommonJS 时，优先使用默认导入或动态 import 后读取 default/命名属性
//   - 给 CommonJS 消费者提供库时，发布独立 CJS 产物或清楚说明只支持 ESM
//   - 不把 require(ESM) 当作通用策略；它受 Node 版本、顶层 await 和包边界约束
//
// C++ 对照：
//   这类似把不同 ABI、不同链接模型的库混在一起。声明能写出来，不代表链接和加载都可靠。

function demoCjsEsmInterop(): void {
    section("19.6 CommonJS / ESM 互操作边界");
    note("C++ 对照", "CJS/ESM 混用像跨 ABI 链接：类型声明不能替代运行时兼容性验证。");

    const interopRules: readonly InteropRule[] = [
        {
            scenario: "ESM 项目消费 CommonJS 包",
            safePattern: 'import legacyPackage from "legacy-package";',
            boundary: "命名导入是否可靠取决于包的导出形状和 Node 静态分析"
        },
        {
            scenario: "ESM 项目按需加载未知模块",
            safePattern: 'const module = await import(specifier);',
            boundary: "动态 import 是异步边界；错误要在 await 附近捕获"
        },
        {
            scenario: "CommonJS 消费 ESM 库",
            safePattern: "提供单独 CJS 构建，或要求调用方使用动态 import",
            boundary: "require(ESM) 不是所有 ESM 都能稳定覆盖，尤其遇到顶层 await 时"
        }
    ];

    showJson("互操作决策表", interopRules);

    printSnippet("从 ESM 中创建 require 的桥接写法", [
        'import { createRequire } from "node:module";',
        "const require = createRequire(import.meta.url);",
        'const cjsOnlyPackage = require("cjs-only-package");',
        "// 本教程只把它作为边界知识，不在章节中依赖外部 CJS 包"
    ]);

    note("类型边界", "esModuleInterop 改善 TS 写法兼容性，但不会把运行时包真的改造成 ESM。");
    note("常见坑", "不要在一个文件里混用顶层 import 和裸 require；先决定该文件属于 ESM 还是 CJS。");
}

// =============================================================================
// 19.7 Barrel export：组织公共 API，而不是隐藏复杂度的魔法
// =============================================================================
//
// barrel export 指在 index.ts 里集中转出多个模块：
//
//   export { parseConfig } from "./parser.js";
//   export type { Config } from "./types.js";
//
// 好处：
//   - 消费方导入路径稳定
//   - 公共 API 集中可见
//   - 内部文件可以重命名或拆分
//
// 风险：
//   - index.ts 过度转出会泄漏内部 helper
//   - 循环依赖可能被 barrel 放大
//   - 只为了“少写路径”而层层 barrel，会让依赖图难以追踪
//
// C++ 对照：
//   barrel 有点像一个聚合头文件。但 C++ 聚合头文件会增加编译耦合；
//   TS barrel 也会增加运行时加载边和循环依赖风险。

function demoBarrelExports(): void {
    section("19.7 Barrel export：组织公共 API，而不是隐藏复杂度的魔法");
    note("C++ 对照", "barrel 像聚合头文件：方便入口稳定，但不能滥用到遮住真实依赖。");

    const moduleLayout: readonly BarrelItem[] = [
        {
            file: "src/index.ts",
            publicApi: ["parseConfig", "formatConfig", "type Config"],
            privateImplementation: []
        },
        {
            file: "src/parser.ts",
            publicApi: ["parseConfig"],
            privateImplementation: ["scanToken", "recoverFromSyntaxError"]
        },
        {
            file: "src/internal/cache.ts",
            publicApi: [],
            privateImplementation: ["memoizeParser", "clearParserCache"]
        }
    ];

    showJson("barrel 前先区分公共 API 与内部实现", moduleLayout);

    printSnippet("index.ts 推荐转出方式", [
        'export { parseConfig, formatConfig } from "./parser.js";',
        'export type { Config, ParseError } from "./types.js";',
        '// 不转出 ./internal/cache.js'
    ]);

    note("工程场景", "库的 README、exports 字段和 index.ts 应描述同一组公共能力。");
    note("常见坑", "export * from 多个文件容易制造命名冲突；公共入口推荐显式列出要导出的名字。");
}

// =============================================================================
// 19.8 本教程的模块系统检查清单
// =============================================================================
//
// 到这里可以把本章规则收敛成一个工程检查清单。
// 对团队项目来说，模块系统最怕“局部能跑、整体不可维护”：
//   - 某个脚本靠 tsx 或打包器能跑，但 npm run build 后 dist 无法由 Node 直接执行
//   - 某个文件从内部路径导入，短期省事，长期锁死目录结构
//   - 某个类型导入残留为运行时导入，让启动路径变慢或触发循环依赖
//
// C++ 对照：
//   模块边界和链接边界一样，需要项目级约定；不是每个文件各自“能编译”就够。

function demoProjectChecklist(): void {
    section("19.8 本教程的模块系统检查清单");
    note("C++ 对照", "像维护库的头文件、链接产物和 ABI 约定一样维护 TS 模块边界。");

    const checklist = [
        "package.json 设置 type=module",
        "tsconfig.json 使用 module=NodeNext 与 moduleResolution=NodeNext",
        "TS 源码中的相对 ESM 导入写编译后 JS 的 .js 后缀",
        "纯类型依赖使用 import type 或 export type",
        "公共 API 通过明确的 index.ts/barrel 和 package exports 暴露",
        "内部 helper 不从 barrel 或 exports 泄漏",
        "JSON 模块使用 with { type: \"json\" }，不使用旧 assert 写法",
        "CJS/ESM 互操作必须用当前 Node 版本实际验证"
    ] as const;

    showJson("模块系统 checkpoint", checklist);

    note("可重复运行", "本章不写文件、不启动网络、不创建定时器；所有输出来自固定示例和 import.meta。");
    note("下一章衔接", "ch20 会继续讨论 tsconfig、声明文件和库发布，把本章的运行时边界扩展到类型产物。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 19 demos in a stable order. */
export function runChapter(): void {
    demoModuleBoundary();
    demoExportForms();
    demoImportMetaAndJsSuffix();
    demoPackageBoundaries();
    demoJsonImportAttributes();
    demoCjsEsmInterop();
    demoBarrelExports();
    demoProjectChecklist();
}

await runIfMain(import.meta.url, runChapter);
