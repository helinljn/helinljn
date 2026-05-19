// =============================================================================
// 第 20 章：工程配置与声明文件
// =============================================================================
//
// 【学习目标】
//   1. 理解 tsconfig.json、tsc --init、项目引用和严格检查选项的职责
//   2. 掌握 .d.ts、declare、@types 与运行时实现之间的边界
//   3. 认识 declaration、declarationMap、emitDeclarationOnly、declarationDir 等声明产物选项
//   4. 能为库发布设计 package types、exports 与源码/产物目录分离策略
//   5. 建立 npm semver、package-lock、npm ci/install/outdated/audit 的维护直觉
//
// 【运行方式】
//   npm run build
//   node dist/06_模块与工程篇/chapter20_工程配置与声明文件.js
//   或 npm run chapter -- 20（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type TsConfigOption = {
    option: string;
    currentValue: string;
    responsibility: string;
    cxxAnalogy: string;
};

type BuildProfile = {
    name: string;
    goal: string;
    importantOptions: readonly string[];
    output: string;
};

type DeclarationSnippet = {
    file: string;
    purpose: string;
    snippet: readonly string[];
};

type PublicApiItem = {
    symbol: string;
    exportedFrom: string;
    appearsInDts: boolean;
    implementationDetail: boolean;
};

type DependencyRule = {
    topic: string;
    recommendedCommand: string;
    reason: string;
};

function printSnippet(label: string, lines: readonly string[]): void {
    console.log(`${label}:`);
    for (const line of lines) {
        console.log(`  ${line}`);
    }
}

function classifySemverRange(range: string): string {
    if (range.startsWith("~")) {
        return "只自动接收补丁版本，适合编译器这类版本敏感工具";
    }
    if (range.startsWith("^")) {
        return "自动接收同一主版本内的次版本和补丁版本，适合多数运行时库";
    }
    if (/^\d+\.\d+\.\d+$/.test(range)) {
        return "精确锁定单个版本，变更必须显式修改 package.json";
    }
    return "需要结合 npm semver 规则和团队策略判断";
}

function formatPackageExport(path: string, typesPath: string, importPath: string): string {
    return `${path} -> types:${typesPath}, import:${importPath}`;
}

// =============================================================================
// 20.1 tsconfig.json 是工程契约，不只是编译器参数
// =============================================================================
//
// tsconfig.json 描述“这个项目希望 TypeScript 怎样理解源码”。
// 它不只是 tsc 命令行参数的集合，而是团队共享的工程契约：
//   - 采用哪个 JS 目标版本
//   - 采用哪个模块系统
//   - 是否开启严格类型检查
//   - 哪些目录参与编译
//   - 输出目录和源码目录如何对应
//
// C++ 对照：
//   它接近 CMakeLists.txt、toolchain file 和编译选项约定的组合。
//   单个文件“能编译”不代表项目配置正确；团队要以项目级 build 为准。

function demoTsConfigAsContract(): void {
    section("20.1 tsconfig.json 是工程契约，不只是编译器参数");
    note("C++ 对照", "tsconfig 像 CMake/toolchain 约定：它定义整个工程如何被解释和构建。");

    const options: readonly TsConfigOption[] = [
        {
            option: "target",
            currentValue: "ES2023",
            responsibility: "决定输出 JS 使用的语言能力边界",
            cxxAnalogy: "类似 -std=c++20 这类语言标准选择"
        },
        {
            option: "module/moduleResolution",
            currentValue: "NodeNext / NodeNext",
            responsibility: "让 TS 按现代 Node ESM/CJS 规则解析和输出模块",
            cxxAnalogy: "类似同时约定模块系统和链接/加载规则"
        },
        {
            option: "strict",
            currentValue: "true",
            responsibility: "把隐式 any、null/undefined 等风险尽早暴露",
            cxxAnalogy: "类似打开更严格的警告并把警告当作问题处理"
        },
        {
            option: "rootDir/outDir",
            currentValue: ". / dist",
            responsibility: "保持源码目录和编译产物目录的稳定映射",
            cxxAnalogy: "类似 src 与 build 输出目录分离"
        },
        {
            option: "types/lib",
            currentValue: "types: [node], lib: [ES2023, DOM]",
            responsibility: "决定编译器认识哪些平台 API 的类型",
            cxxAnalogy: "类似选择可见的平台 SDK 头文件集合"
        }
    ];

    showJson("本教程关键 tsconfig 选项", options);
    note("输出解释", "这些值不是从运行时读取的魔法；它们来自项目配置，是所有章节共同遵守的基线。");
    note("常见坑", "直接运行 `tsc file.ts` 会绕过部分项目配置；验收应使用 `tsc -p tsconfig.json` 或 npm script。");
}

// =============================================================================
// 20.2 tsc --init 与最小可维护配置
// =============================================================================
//
// tsc --init 可以生成起点，但生成结果不是最终设计。
// 对团队项目来说，最小可维护配置通常要回答：
//   - 运行环境是什么：Node、浏览器、还是两者都有
//   - 模块系统是什么：NodeNext、bundler、还是 CommonJS
//   - 输出产物是否需要被 Node 直接执行
//   - 是否要发布类型声明
//
// C++ 对照：
//   自动生成的 CMakeLists.txt 只是脚手架；真正重要的是把目标、标准、依赖和产物边界讲清楚。

function demoTscInitAndProfiles(): void {
    section("20.2 tsc --init 与最小可维护配置");
    note("C++ 对照", "tsc --init 像生成工程脚手架，之后仍要按运行环境修正。");

    printSnippet("初始化命令", [
        "npm exec -- tsc --init",
        "// 本教程已固定 typescript 依赖，使用 npm exec 可复用本地版本",
        "// 不用裸 npx 去临时解析工具版本"
    ]);

    const profiles: readonly BuildProfile[] = [
        {
            name: "Node 应用",
            goal: "编译后由 Node 直接执行 dist/*.js",
            importantOptions: ["module=NodeNext", "moduleResolution=NodeNext", "types=[node]", "outDir=dist"],
            output: "JavaScript + source map"
        },
        {
            name: "前端应用",
            goal: "交给打包器处理模块和资源",
            importantOptions: ["moduleResolution=bundler", "jsx", "lib=DOM", "noEmit 或由 bundler emit"],
            output: "通常由 Vite/Webpack/Rspack 等工具生成"
        },
        {
            name: "库项目",
            goal: "发布 JS 产物和 .d.ts 类型入口",
            importantOptions: ["declaration", "declarationMap", "emitDeclarationOnly 可选", "stripInternal 可选"],
            output: "dist/*.js + dist/*.d.ts"
        }
    ];

    showJson("不同项目形态的配置重点", profiles);
    note("常见坑", "从别的项目复制 tsconfig 最容易带入错误运行环境，例如把 bundler 配置用在纯 Node 教程里。");
}

// =============================================================================
// 20.3 严格检查选项：让边界错误更早出现
// =============================================================================
//
// 本教程不只打开 strict，还打开几个团队项目常用的精细选项：
//   - noUncheckedIndexedAccess
//   - exactOptionalPropertyTypes
//   - useUnknownInCatchVariables
//   - verbatimModuleSyntax
//   - forceConsistentCasingInFileNames
//
// 它们会让入门阶段多写一点显式判断，但好处是很多运行时错误提前变成编译错误。
//
// C++ 对照：
//   这些选项类似额外的静态分析规则。短期更“啰嗦”，长期减少隐式假设。

function demoStrictOptions(): void {
    section("20.3 严格检查选项：让边界错误更早出现");
    note("C++ 对照", "严格选项像静态分析和更高 warning level，目标是把隐式风险显式化。");

    const strictRules = [
        {
            option: "noUncheckedIndexedAccess",
            catches: "array[index]、map[key] 可能是 undefined",
            habit: "读取后先判断，或使用确定存在的 API"
        },
        {
            option: "exactOptionalPropertyTypes",
            catches: "optional 属性缺失与显式 undefined 不是同一件事",
            habit: "配置合并时区分“没传”和“传了 undefined”"
        },
        {
            option: "useUnknownInCatchVariables",
            catches: "catch 到的不一定是 Error",
            habit: "先用 instanceof Error 或自定义收窄函数"
        },
        {
            option: "verbatimModuleSyntax",
            catches: "类型导入和值导入混用造成运行时依赖不清楚",
            habit: "纯类型写 import type / export type"
        }
    ] as const;

    showJson("本教程强调的严格选项", strictRules);

    printSnippet("noUncheckedIndexedAccess 下的写法", [
        "const first = values[0];",
        "if (first === undefined) {",
        "    return \"empty\";",
        "}",
        "return first.toUpperCase();"
    ]);

    note("类型边界", "严格选项只约束编译期；外部 JSON、命令行参数和网络输入仍必须做运行时校验。");
    note("常见坑", "把严格选项关掉能让错误消失，但也会把真实边界问题推迟到线上运行时。");
}

// =============================================================================
// 20.4 项目引用 references：拆分大型工程的编译图
// =============================================================================
//
// TypeScript project references 用来把大型仓库拆成多个可增量构建的项目。
// 被引用项目通常需要 composite: true，让 TypeScript 能保存构建信息并生成稳定声明。
//
// 本教程当前是单项目，所以不启用 references；这里先理解概念边界。
//
// C++ 对照：
//   references 有点像把一个大工程拆成多个 library target。
//   每个 target 有自己的 tsconfig 和输出产物，顶层 build 按依赖顺序构建。

function demoProjectReferences(): void {
    section("20.4 项目引用 references：拆分大型工程的编译图");
    note("C++ 对照", "project references 像多个 library target 组成的构建图。");

    printSnippet("根 tsconfig 示例", [
        "{",
        "  \"files\": [],",
        "  \"references\": [",
        "    { \"path\": \"./packages/core\" },",
        "    { \"path\": \"./packages/cli\" }",
        "  ]",
        "}"
    ]);

    printSnippet("被引用项目示例", [
        "{",
        "  \"compilerOptions\": {",
        "    \"composite\": true,",
        "    \"declaration\": true,",
        "    \"outDir\": \"dist\"",
        "  }",
        "}"
    ]);

    showJson("何时考虑 references", [
        "仓库中有多个可独立构建的包",
        "核心库被 CLI、服务端和测试工具同时依赖",
        "全量类型检查太慢，需要增量构建边界",
        "发布多个 npm package，需要各自稳定的 .d.ts"
    ]);

    note("可运行边界", "本章不创建多包目录；只是说明配置形态，避免影响当前单项目教程。");
}

// =============================================================================
// 20.5 .d.ts：声明文件只描述形状，不提供实现
// =============================================================================
//
// .d.ts 文件是 TypeScript 的声明文件。它告诉编译器：
//   - 某个 JS 模块导出了哪些值
//   - 某个全局变量或平台 API 的类型是什么
//   - 某个包的公共 API 如何被消费
//
// 但 .d.ts 不会生成运行时实现。
// 如果声明说有函数 parseConfig，而实际 JS 里没有这个导出，编译器可能满意，Node 仍会在运行时报错。
//
// C++ 对照：
//   .d.ts 很像头文件：能描述函数签名，却不能替代 .cpp/.so/.dll 里的实现。

function demoDeclarationFiles(): void {
    section("20.5 .d.ts：声明文件只描述形状，不提供实现");
    note("C++ 对照", ".d.ts 像头文件声明；没有对应 JS 实现时，运行时仍会失败。");

    const snippets: readonly DeclarationSnippet[] = [
        {
            file: "legacy-lib.d.ts",
            purpose: "给没有类型的 JS 包补声明",
            snippet: [
                'declare module "legacy-lib" {',
                "    export function parse(input: string): unknown;",
                "}"
            ]
        },
        {
            file: "globals.d.ts",
            purpose: "声明运行环境已经提供的全局变量",
            snippet: [
                "declare global {",
                "    const __APP_VERSION__: string;",
                "}",
                "export {};"
            ]
        }
    ];

    showJson("声明文件用途", snippets.map((item) => ({
        file: item.file,
        purpose: item.purpose
    })));

    for (const item of snippets) {
        printSnippet(item.file, item.snippet);
    }

    note("类型边界", "declare 是对运行时事实的承诺，不是创建事实；它不能让不存在的包或全局变量自动出现。");
    note("常见坑", "用 any 写声明最快，但会把第三方边界变成类型黑洞；优先声明 unknown 再在业务层收窄。");
}

// =============================================================================
// 20.6 @types：社区维护的运行时 API 类型
// =============================================================================
//
// 很多 JavaScript 包没有自带类型声明，社区会在 DefinitelyTyped 上维护 @types/* 包。
// Node 平台 API 的类型来自 @types/node，本教程 package.json 已固定：
//   "@types/node": "24.12.4"
//
// 这解释了为什么源码里可以写 process、node:fs、node:http 的类型。
// 但 @types 仍然只是类型，真正执行这些 API 的是本机 Node.js。
//
// C++ 对照：
//   @types 像平台 SDK 头文件；头文件版本和运行时库版本最好匹配，否则声明可能领先或落后实现。

function demoDefinitelyTyped(): void {
    section("20.6 @types：社区维护的运行时 API 类型");
    note("C++ 对照", "@types 像 SDK 头文件，必须和真实运行时版本保持合理匹配。");

    showJson("本教程里的类型来源", [
        {
            api: "process.cwd()",
            typeProvider: "@types/node",
            runtimeProvider: "Node.js 24.15.0"
        },
        {
            api: "fetch()",
            typeProvider: "lib: DOM 与 Node 运行时类型共同可见",
            runtimeProvider: "Node.js 内置 Web API"
        },
        {
            api: "Array.prototype.map",
            typeProvider: "lib: ES2023",
            runtimeProvider: "JavaScript 引擎"
        }
    ]);

    note("类型边界", "类型包版本正确不等于 API 在所有 Node 版本都可用；版本敏感 API 仍需本地运行验证。");
    note("工程习惯", "升级 Node 主版本时同步审查 @types/node 和 tsconfig lib，避免类型与运行时错位。");
}

// =============================================================================
// 20.7 生成声明产物：declaration 与 declarationMap
// =============================================================================
//
// 应用项目通常只需要 JS 产物；库项目通常还要 .d.ts。
// 常用选项：
//   - declaration: 生成 .d.ts
//   - declarationMap: 生成 .d.ts.map，方便编辑器跳到源码
//   - emitDeclarationOnly: 只输出声明，不输出 JS
//   - declarationDir: 把声明输出到单独目录
//
// 本教程当前是可运行教程应用，所以没有打开 declaration。
// 下面展示的是库发布配置，不是本项目当前配置。
//
// C++ 对照：
//   JS 产物像库的二进制/目标文件，.d.ts 像给消费者看的公开头文件。

function demoDeclarationEmit(): void {
    section("20.7 生成声明产物：declaration 与 declarationMap");
    note("C++ 对照", ".d.ts 是给消费者看的公共头文件；JS 才是运行时实现。");

    printSnippet("库项目声明产物配置", [
        "{",
        "  \"compilerOptions\": {",
        "    \"outDir\": \"dist\",",
        "    \"declaration\": true,",
        "    \"declarationMap\": true,",
        "    \"declarationDir\": \"dist/types\"",
        "  }",
        "}"
    ]);

    printSnippet("只生成声明的类型检查流水线", [
        "{",
        "  \"compilerOptions\": {",
        "    \"emitDeclarationOnly\": true,",
        "    \"declaration\": true",
        "  }",
        "}"
    ]);

    showJson("声明产物决策", [
        {
            option: "declaration",
            useWhen: "库要被其他 TS 项目消费"
        },
        {
            option: "declarationMap",
            useWhen: "希望消费者在编辑器中跳转到源码"
        },
        {
            option: "emitDeclarationOnly",
            useWhen: "JS 由 swc/esbuild/rollup 输出，tsc 只负责类型声明"
        },
        {
            option: "declarationDir",
            useWhen: "声明和 JS 产物需要分目录管理"
        }
    ]);

    note("常见坑", "声明产物会暴露被导出函数签名中的类型；内部 helper 类型如果出现在公共签名里，也会被迫进入 .d.ts。");
}

// =============================================================================
// 20.8 库发布：types、exports 与源码/产物目录分离
// =============================================================================
//
// 发布库时，package.json 至少要让消费方同时找到：
//   - 运行时 JS 入口
//   - TypeScript 类型入口
//
// 现代 ESM 包通常用 exports 精确列出公共入口。
// 如果只有一个入口，顶层 types 字段也常见；如果有多个子路径，exports 里要为每个入口配类型。
//
// C++ 对照：
//   这类似安装库时同时安装 libacme.so 和 include/acme/*.hpp。
//   源码 src/ 可以很复杂，但安装给消费者的公共入口应该稳定。

function demoLibraryPublishing(): void {
    section("20.8 库发布：types、exports 与源码/产物目录分离");
    note("C++ 对照", "发布包时要同时交付运行时库和公开头文件；src 内部结构不该成为公共协议。");

    const exports = [
        formatPackageExport(".", "./dist/index.d.ts", "./dist/index.js"),
        formatPackageExport("./cli", "./dist/cli.d.ts", "./dist/cli.js"),
        formatPackageExport("./testing", "./dist/testing.d.ts", "./dist/testing.js")
    ];

    showJson("公共入口设计", exports);

    printSnippet("package.json 片段", [
        "{",
        "  \"type\": \"module\",",
        "  \"types\": \"./dist/index.d.ts\",",
        "  \"exports\": {",
        "    \".\": {",
        "      \"types\": \"./dist/index.d.ts\",",
        "      \"import\": \"./dist/index.js\"",
        "    },",
        "    \"./cli\": {",
        "      \"types\": \"./dist/cli.d.ts\",",
        "      \"import\": \"./dist/cli.js\"",
        "    }",
        "  }",
        "}"
    ]);

    note("工程场景", "让用户只从包名或明确子路径导入，避免他们依赖 dist/internal/helper.js 这类内部路径。");
    note("类型边界", "exports 中 types 和 import 指向的 API 必须一致；否则会出现类型能编译、运行时找不到导出的错位。");
}

// =============================================================================
// 20.9 消费方类型体验：公共 API 要少而稳定
// =============================================================================
//
// .d.ts 的质量直接决定消费方体验。
// 一个好的库声明应该：
//   - 只暴露稳定公共类型
//   - 错误类型、配置类型、返回值类型命名清楚
//   - 不把内部缓存、解析中间态、测试 helper 泄漏给普通用户
//   - 对外部输入使用 unknown 或明确的 validation 结果，而不是 any
//
// C++ 对照：
//   公开头文件越小，ABI/API 兼容维护越容易；把 private detail 放进头文件会让用户依赖实现细节。

function demoConsumerExperience(): void {
    section("20.9 消费方类型体验：公共 API 要少而稳定");
    note("C++ 对照", "公开类型越少越稳定；不要把 detail 命名空间变成公共协议。");

    const publicApi: readonly PublicApiItem[] = [
        {
            symbol: "parseConfig(text: string): Config",
            exportedFrom: "index.ts",
            appearsInDts: true,
            implementationDetail: false
        },
        {
            symbol: "ConfigError",
            exportedFrom: "errors.ts",
            appearsInDts: true,
            implementationDetail: false
        },
        {
            symbol: "ScannerToken",
            exportedFrom: "internal/scanner.ts",
            appearsInDts: false,
            implementationDetail: true
        },
        {
            symbol: "memoizeScanner",
            exportedFrom: "internal/cache.ts",
            appearsInDts: false,
            implementationDetail: true
        }
    ];

    showJson("公共 API 取舍", publicApi);

    printSnippet("对外函数签名示例", [
        "export type ParseResult =",
        "    | { ok: true; config: Config }",
        "    | { ok: false; error: ConfigError };",
        "",
        "export function parseConfig(input: unknown): ParseResult;"
    ]);

    note("类型边界", "外部输入写 unknown 可以迫使库内部做校验；写 any 会把校验责任悄悄转嫁给调用方。");
    note("常见坑", "公共函数返回内部类实例会锁死实现；更稳妥的是返回普通对象、联合类型或明确接口。");
}

// =============================================================================
// 20.10 npm 依赖管理：semver、lockfile 与升级策略
// =============================================================================
//
// package.json 表达“允许的版本范围”，package-lock.json 记录“实际安装的版本树”。
//
// 常见命令：
//   - npm install：安装或更新依赖，并可能更新 package-lock.json
//   - npm ci：严格按 lockfile 安装，适合 CI 和可复现构建
//   - npm outdated：查看可升级版本
//   - npm audit：检查已知安全漏洞
//
// C++ 对照：
//   package.json 像依赖约束，package-lock.json 像锁定后的依赖解析结果。
//   可复现构建不能只依赖“版本范围看起来差不多”。

function demoNpmDependencyManagement(): void {
    section("20.10 npm 依赖管理：semver、lockfile 与升级策略");
    note("C++ 对照", "package-lock 像锁定的依赖解析结果；CI 应按锁文件复现安装。");

    const ranges = ["~6.0.3", "^24.12.4", "1.2.3", ">=1.0.0 <2.0.0"] as const;
    showJson("semver 范围解释", ranges.map((range) => ({
        range,
        meaning: classifySemverRange(range)
    })));

    const dependencyRules: readonly DependencyRule[] = [
        {
            topic: "新机器或 CI 安装",
            recommendedCommand: "npm ci",
            reason: "严格按 package-lock.json 复现依赖树"
        },
        {
            topic: "添加新依赖",
            recommendedCommand: "npm install package-name",
            reason: "同时更新 package.json 和 package-lock.json"
        },
        {
            topic: "评估可升级版本",
            recommendedCommand: "npm outdated",
            reason: "先看 wanted/latest，再决定补丁、次版本或主版本升级"
        },
        {
            topic: "安全审计",
            recommendedCommand: "npm audit",
            reason: "识别已知漏洞，再人工确认修复是否会破坏兼容性"
        }
    ];

    showJson("依赖维护命令", dependencyRules);
    note("版本边界", "TypeScript 编译器属于版本敏感工具，本教程使用 ~6.0.3，避免自动跳到新的次版本。");
    note("常见坑", "不要手工删 lockfile 来“修复依赖”；这会改变整棵依赖树，必须作为明确升级审查处理。");
}

// =============================================================================
// 20.11 本章复盘：配置、声明与发布边界
// =============================================================================
//
// 本章把“工程配置”和“声明文件”放在一起，是因为它们共同决定团队项目能否长期维护。
//
// C++ 对照：
//   tsconfig 是构建契约，.d.ts 是公共头文件，package exports 是安装后的可见入口。
//   三者对齐，消费者才会同时获得正确类型和正确运行时行为。

function demoChapterReview(): void {
    section("20.11 本章复盘：配置、声明与发布边界");
    note("C++ 对照", "构建配置、头文件和安装入口必须一致；TS 项目也是同一类问题。");

    showJson("关键结论", [
        "tsconfig.json 是团队共享的项目级契约",
        "tsc --init 只是起点，配置必须匹配真实运行环境",
        "严格选项把潜在运行时问题提前到编译期",
        "project references 适合多包和大型仓库，不必在单项目中过早引入",
        ".d.ts 只描述类型，不提供 JS 实现",
        "@types 是类型来源，不是运行时 polyfill",
        "库发布要同时设计 JS 入口和 types 入口",
        "公共 API 要少而稳定，内部 helper 不应泄漏到声明产物",
        "package-lock.json 是可复现安装的关键文件"
    ]);

    note("可重复运行", "本章只打印固定配置片段和说明，不写文件、不联网、不启动子进程。");
    note("下一章衔接", "ch21 会进入装饰器，重点是 TS 6.0 当前可运行的标准装饰器边界。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 20 demos in a stable order. */
export function runChapter(): void {
    demoTsConfigAsContract();
    demoTscInitAndProfiles();
    demoStrictOptions();
    demoProjectReferences();
    demoDeclarationFiles();
    demoDefinitelyTyped();
    demoDeclarationEmit();
    demoLibraryPublishing();
    demoConsumerExperience();
    demoNpmDependencyManagement();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
