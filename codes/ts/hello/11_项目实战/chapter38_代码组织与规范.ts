// =============================================================================
// 第 38 章：代码组织与规范
// =============================================================================
//
// 【学习目标】
//   1. 建立 TS/Node 项目的目录分层、命名规范、barrel export 与公共 API 边界
//   2. 掌握 runtime validation、配置合并、错误分层和可观测性这些真实项目约束
//   3. 理解前端最小闭环、API 契约和 Git 工作流如何进入工程规范
//
// 【运行方式】
//   npm run build
//   node dist/11_项目实战/chapter38_代码组织与规范.js
//   或 npm run chapter -- 38（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

/** Describes one project layer and what code belongs there. */
export type LayerAdvice = {
    layer: string;
    responsibility: string;
    examples: string[];
    avoid: string;
};

/** Result style used when domain code should not decide process exit or HTTP status. */
export type Result<T, E> =
    | { ok: true; value: T }
    | { ok: false; error: E };

type NamingRule = {
    target: string;
    convention: string;
    good: string;
    bad: string;
};

type UserInput = {
    id: string;
    email: string;
    role: "reader" | "admin";
};

type AppConfig = {
    port: number;
    logLevel: "debug" | "info" | "warn" | "error";
    dryRun: boolean;
    outputDir: string;
};

type ConfigSources = {
    defaults: AppConfig;
    file: Partial<AppConfig>;
    env: Partial<AppConfig>;
    cli: Partial<AppConfig>;
};

type ApiContract = {
    route: string;
    method: "GET" | "POST";
    requestSchema: Record<string, unknown>;
    responseSchema: Record<string, unknown>;
    observableFields: string[];
};

const DEFAULT_CONFIG: AppConfig = {
    port: 3000,
    logLevel: "info",
    dryRun: true,
    outputDir: "dist/reports"
};

// =============================================================================
// 38.1 目录结构：按层分工，按领域聚合
// =============================================================================
//
// C++ 对照：C++ 项目常见 include/src/tests 分离；TS 项目更常以 ESM 模块边界表达
// 公共 API、领域服务、入口层和测试。目录不是装饰，它决定读者从哪里理解系统。
//
// C++ 思维提示：不要把所有类型都塞进一个 global types 文件。TS 的类型和实现可以在同一模块中
// 相邻维护，真正需要分离的是“外部入口、领域逻辑、基础设施、测试”这些职责。

/** Return recommended layers for a small TS/Node project. */
export function suggestProjectLayers(): LayerAdvice[] {
    return [
        {
            layer: "src/domain",
            responsibility: "放业务规则、纯函数、Result 类型和领域错误，不直接读 process/env/fs。",
            examples: ["invoice.ts", "userPolicy.ts", "batchRules.ts"],
            avoid: "在领域函数中 console.log、process.exit 或读取真实文件系统。"
        },
        {
            layer: "src/adapters",
            responsibility: "封装文件系统、HTTP、数据库、第三方 SDK 等运行时边界。",
            examples: ["fileStore.ts", "httpClient.ts", "clock.ts"],
            avoid: "把第三方 SDK 类型泄漏成领域类型。"
        },
        {
            layer: "src/config",
            responsibility: "把环境变量、配置文件和 CLI 参数合并成明确 AppConfig。",
            examples: ["defaults.ts", "parseEnv.ts", "mergeConfig.ts"],
            avoid: "在业务逻辑深处随手读取 process.env。"
        },
        {
            layer: "src/cli 或 src/server",
            responsibility: "处理入口协议：参数、HTTP 请求、打印输出、exit code 或 status code。",
            examples: ["main.ts", "routes.ts", "server.ts"],
            avoid: "把核心业务写在 main.ts 里导致无法单元测试。"
        },
        {
            layer: "tests 或 *.test.ts",
            responsibility: "验证领域逻辑、边界适配和少量端到端路径。",
            examples: ["userPolicy.test.ts", "cli.test.ts"],
            avoid: "测试共享可变全局状态或依赖公网。"
        }
    ];
}

function demoDirectoryLayers(): void {
    section("38.1 目录结构：按层分工，按领域聚合");
    note("C++ 对照", "TS 项目用模块和目录表达边界，不需要机械复制 .h/.cpp 分离。");

    showJson("推荐分层", suggestProjectLayers());
    showJson("两种常见组织方式", [
        {
            style: "按技术层分层",
            tree: ["src/domain", "src/adapters", "src/config", "src/cli", "tests"],
            bestFor: "小到中型 CLI、服务端、教学项目"
        },
        {
            style: "按领域分组",
            tree: ["src/users", "src/orders", "src/billing", "src/shared"],
            bestFor: "业务域较多、团队按领域协作的服务"
        }
    ]);
    note("常见坑", "按文件类型建 folders，例如 interfaces、classes、functions，会让一次业务修改横跨太多目录。");
}

// =============================================================================
// 38.2 工具链规范：ESLint、Prettier 与 pre-commit hook
// =============================================================================
//
// C++ 对照：ESLint/Prettier 类似 clang-tidy + clang-format，一个查问题，一个统一格式。
// pre-commit hook 像提交前的本地快速门禁，CI 仍然要重新跑一遍。
//
// C++ 思维提示：格式化规则要自动化，不要靠 code review 争论空格；语义规则要少而明确，
// 避免把 lint 配到开发者无法解释。

function demoLintAndFormatWorkflow(): void {
    section("38.2 工具链规范：ESLint、Prettier 与 pre-commit hook");
    note("C++ 对照", "ESLint/Prettier 对应 clang-tidy/clang-format；hook 是本地门禁，不替代 CI。");

    showJson("推荐工具链配置形状", {
        install: [
            "npm install --save-dev eslint @typescript-eslint/parser @typescript-eslint/eslint-plugin",
            "npm install --save-dev prettier eslint-config-prettier",
            "npm install --save-dev lint-staged husky"
        ],
        scripts: {
            lint: "eslint .",
            format: "prettier --write .",
            check: "npm run build && npm test && npm run lint"
        },
        lintStaged: {
            "*.{ts,tsx,js,json,md}": ["prettier --write"],
            "*.{ts,tsx,js}": ["eslint --fix"]
        },
        tutorialBoundary: "本教程当前不引入这些依赖，只说明真实项目的组织方式。"
    });

    note("常见坑", "不要让 pre-commit 跑完整慢测试；慢门禁放 CI，本地 hook 保持秒级反馈。");
    note("可运行边界", "本章不执行 npm install，也不修改 package-lock，避免把教学说明变成联网副作用。");
}

// =============================================================================
// 38.3 命名规范：让文件名、值、类型和常量可预测
// =============================================================================
//
// C++ 对照：命名规范相当于团队编码标准。TS 里文件、变量、类型、枚举式常量的命名
// 直接影响 import 可读性和搜索体验。
//
// C++ 思维提示：TS 类型在编译后会擦除，命名仍然服务读者和编辑器。不要用 I 前缀或匈牙利命名
// 模拟 C++ 旧习惯。

function demoNamingConventions(): void {
    section("38.3 命名规范：让文件名、值、类型和常量可预测");
    note("C++ 对照", "命名规范像团队 coding style，目标是降低阅读和搜索成本。");

    const rules: NamingRule[] = [
        {
            target: "文件",
            convention: "kebab-case 或明确的 chapterXX_中文标题，本教程按计划使用章节名",
            good: "user-service.ts / chapter38_代码组织与规范.ts",
            bad: "UserServiceHelperFinal2.ts"
        },
        {
            target: "变量/函数",
            convention: "camelCase",
            good: "parseUserInput",
            bad: "Parse_User_Input"
        },
        {
            target: "类/接口/类型别名",
            convention: "PascalCase",
            good: "UserRepository / AppConfig",
            bad: "user_repository"
        },
        {
            target: "常量",
            convention: "局部 const 用 camelCase；跨模块固定常量可用 UPPER_SNAKE_CASE",
            good: "DEFAULT_CONFIG / retryLimit",
            bad: "defaultconfig"
        }
    ];

    showJson("命名规范表", rules);
    note("常见坑", "不要把接口都命名成 IUser；TS 结构类型系统下接口和类型别名都是形状约束。");
}

// =============================================================================
// 38.4 Barrel export：稳定公共入口，而不是转出所有文件
// =============================================================================
//
// C++ 对照：barrel export 像聚合头文件。它能稳定消费者入口，但如果 export * 滥用，
// 就会把内部实现误暴露成公共协议。
//
// C++ 思维提示：先决定公共 API，再写 index.ts。不要反过来用 index.ts 掩盖混乱目录。

function demoBarrelExports(): void {
    section("38.4 Barrel export：稳定公共入口，而不是转出所有文件");
    note("C++ 对照", "barrel 类似聚合头文件，应该只公开经过设计的 API。");

    showJson("barrel 设计示例", {
        publicIndex: [
            "export { parseUserInput, mergeConfig } from \"./config/index.js\";",
            "export type { AppConfig, UserInput } from \"./types.js\";"
        ],
        internalFiles: [
            "src/config/readEnv.ts",
            "src/config/normalizePath.ts",
            "src/adapters/fileStore.ts"
        ],
        packageExports: {
            ".": {
                types: "./dist/index.d.ts",
                import: "./dist/index.js"
            },
            "./testing": {
                types: "./dist/testing.d.ts",
                import: "./dist/testing.js"
            }
        }
    });

    note("类型边界", "export type 只导出类型；如果消费者要调用函数，必须有真实运行时 export。");
    note("常见坑", "export * from \"./internal.js\" 会把临时 helper 变成用户依赖的公共协议。");
}

// =============================================================================
// 38.5 Runtime validation：外部输入先是 unknown，再收窄成领域类型
// =============================================================================
//
// C++ 对照：外部 JSON、CLI 参数、HTTP body 类似从网络读来的字节流；不能因为声明了结构体
// 就假设数据已经合法。
//
// C++ 思维提示：TypeScript 类型检查发生在编译期。JSON.parse、process.argv、fetch 返回的数据
// 都在运行时进入系统，必须用 type guard 或 validator 收口。

function isRecord(value: unknown): value is Record<string, unknown> {
    return typeof value === "object" && value !== null && !Array.isArray(value);
}

function parseUserInput(input: unknown): Result<UserInput, string> {
    if (!isRecord(input)) {
        return { ok: false, error: "input must be an object" };
    }

    const id = input["id"];
    const email = input["email"];
    const role = input["role"];

    if (typeof id !== "string" || id.length === 0) {
        return { ok: false, error: "id must be a non-empty string" };
    }
    if (typeof email !== "string" || !email.includes("@")) {
        return { ok: false, error: "email must contain @" };
    }
    if (role !== "reader" && role !== "admin") {
        return { ok: false, error: "role must be reader or admin" };
    }

    return { ok: true, value: { id, email, role } };
}

function demoRuntimeValidation(): void {
    section("38.5 Runtime validation：外部输入先是 unknown，再收窄成领域类型");
    note("C++ 对照", "外部输入像网络字节流，进入领域层前必须解析和校验。");

    const valid = parseUserInput({ id: "u-001", email: "dev@example.com", role: "admin" });
    const invalid = parseUserInput({ id: "u-002", email: "missing-at", role: "owner" });

    showJson("runtime validation 结果", {
        valid,
        invalid,
        boundary: "UserInput 类型不验证运行时数据；parseUserInput 才验证。"
    });
    note("类型边界", "`value as UserInput` 不会检查 JSON；它只是在告诉编译器相信你。");
}

// =============================================================================
// 38.6 配置管理：默认值、配置文件、环境变量、CLI 参数按优先级合并
// =============================================================================
//
// C++ 对照：配置合并类似命令行 flag、配置文件和编译时默认值的优先级设计。优先级必须
// 可解释、可测试，不能散落在业务代码里。
//
// C++ 思维提示：process.env 读取到的永远是字符串或 undefined。先解析成 Partial<AppConfig>，
// 再和默认值合并。

function mergeConfig(sources: ConfigSources): AppConfig {
    return {
        ...sources.defaults,
        ...sources.file,
        ...sources.env,
        ...sources.cli
    };
}

function demoConfigManagement(): void {
    section("38.6 配置管理：默认值、配置文件、环境变量、CLI 参数按优先级合并");
    note("C++ 对照", "配置优先级像命令行 flag 覆盖配置文件，配置文件覆盖默认值。");

    const sources: ConfigSources = {
        defaults: DEFAULT_CONFIG,
        file: { outputDir: "reports", logLevel: "warn" },
        env: { port: 8080 },
        cli: { dryRun: false, logLevel: "debug" }
    };

    showJson("配置合并", {
        priority: "defaults < file < env < cli",
        sources,
        merged: mergeConfig(sources)
    });
    note("常见坑", "在多个模块里直接读 process.env，会让测试很难构造稳定配置。");
}

// =============================================================================
// 38.7 错误处理分层：domain 返回 Result，入口层决定展示方式
// =============================================================================
//
// C++ 对照：领域层类似库函数，应该返回错误对象或抛出明确异常；CLI main 或 HTTP handler
// 才决定打印、exit code、HTTP status。
//
// C++ 思维提示：不要在业务函数里 process.exit。那会像库函数直接 abort 进程一样破坏复用和测试。

function authorizeDelete(user: UserInput, targetOwnerId: string): Result<string, "forbidden" | "not-found"> {
    if (targetOwnerId === "missing") {
        return { ok: false, error: "not-found" };
    }
    if (user.role !== "admin" && user.id !== targetOwnerId) {
        return { ok: false, error: "forbidden" };
    }
    return { ok: true, value: `delete allowed for ${targetOwnerId}` };
}

function mapDomainError(error: "forbidden" | "not-found"): {
    exitCode: number;
    httpStatus: number;
    message: string;
} {
    switch (error) {
        case "forbidden":
            return { exitCode: 3, httpStatus: 403, message: "permission denied" };
        case "not-found":
            return { exitCode: 4, httpStatus: 404, message: "resource not found" };
    }
}

function demoErrorBoundaries(): void {
    section("38.7 错误处理分层：domain 返回 Result，入口层决定展示方式");
    note("C++ 对照", "库层返回错误语义，CLI/HTTP 入口把错误翻译成进程或协议结果。");

    const user: UserInput = { id: "u-001", email: "dev@example.com", role: "reader" };
    const domainResult = authorizeDelete(user, "u-999");

    showJson("错误分层", {
        domainResult,
        entryLayerMapping: domainResult.ok ? "success" : mapDomainError(domainResult.error)
    });
    note("常见坑", "领域函数里直接 console.error 或 process.exit，会让同一逻辑无法复用于 HTTP 和测试。");
}

// =============================================================================
// 38.8 前端最小闭环：DOM 类型、事件、表单、fetch 与 TSX 边界
// =============================================================================
//
// C++ 对照：前端入口像另一个平台目标。DOM 类型来自 lib DOM，运行时对象来自浏览器；
// React/TSX 又是框架和编译配置的额外边界。
//
// C++ 思维提示：本教程以 Node 为主。这里给最小闭环概念，不把浏览器 DOM 或 React 放进
// Node 可运行路径。

function demoFrontendMinimumLoop(): void {
    section("38.8 前端最小闭环：DOM 类型、事件、表单、fetch 与 TSX 边界");
    note("C++ 对照", "前端是不同运行平台；DOM 类型可见不代表 Node 里有 document。");

    showJson("前端最小示例片段", {
        domTypes: [
            "const form = document.querySelector<HTMLFormElement>(\"#login\");",
            "form?.addEventListener(\"submit\", (event: SubmitEvent) => { event.preventDefault(); });"
        ],
        formInput: [
            "const email = new FormData(form).get(\"email\");",
            "if (typeof email !== \"string\") throw new Error(\"email missing\");"
        ],
        fetchBoundary: [
            "const response = await fetch(\"/api/login\", { method: \"POST\", body: JSON.stringify({ email }) });",
            "const data: unknown = await response.json();",
            "validate response before using it"
        ],
        tsxReactBoundary: "需要 jsx 配置、React 类型和框架构建工具；本教程不默认展开。"
    });

    note("类型边界", "fetch().json() 的结果应视为 unknown；不要直接断言成业务类型。");
}

// =============================================================================
// 38.9 API 契约与可观测性：Schema、OpenAPI、logs、metrics、traces
// =============================================================================
//
// C++ 对照：API 契约像跨进程 ABI；日志、指标、追踪像运行时诊断接口。它们都是团队协作协议，
// 不是最后上线前才补的文档。
//
// C++ 思维提示：JSON Schema/OpenAPI 描述外部边界；TypeScript 类型描述源码内部边界。
// 二者需要同步，但不能互相替代。

function createUserContract(): ApiContract {
    return {
        route: "/api/users",
        method: "POST",
        requestSchema: {
            type: "object",
            required: ["email", "role"],
            properties: {
                email: { type: "string", format: "email" },
                role: { enum: ["reader", "admin"] }
            }
        },
        responseSchema: {
            type: "object",
            required: ["id", "email", "role"],
            properties: {
                id: { type: "string" },
                email: { type: "string" },
                role: { enum: ["reader", "admin"] }
            }
        },
        observableFields: ["requestId", "route", "statusCode", "durationMs", "errorCode"]
    };
}

function demoApiContractAndObservability(): void {
    section("38.9 API 契约与可观测性：Schema、OpenAPI、logs、metrics、traces");
    note("C++ 对照", "API 契约像跨进程 ABI；可观测性是运行时诊断接口。");

    showJson("API 契约与观测字段", {
        contract: createUserContract(),
        signals: {
            logs: "记录单个请求发生了什么",
            metrics: "统计错误率、吞吐、延迟分布",
            traces: "串联一次请求经过的服务和步骤"
        }
    });
    note("真实场景", "对外 API 变更应同时更新 schema/OpenAPI、runtime validator、测试和示例。");
}

// =============================================================================
// 38.10 Git 工作流：分支策略和 Conventional Commits
// =============================================================================
//
// C++ 对照：Git 工作流像团队的变更管理流程。分支、提交信息、PR 检查和 release note
// 共同决定改动是否可追溯。
//
// C++ 思维提示：提交信息不是给 Git 看的，是给 code review、release、回滚和审计看的。

function demoGitWorkflow(): void {
    section("38.10 Git 工作流：分支策略和 Conventional Commits");
    note("C++ 对照", "Git 流程是变更管理，不只是把代码存进历史。");

    showJson("推荐 Git 工作流", {
        branches: [
            "main: 始终可发布或可教学演示",
            "dev: 可选集成分支，适合多人批量改造",
            "feature/ch38-code-organization: 单任务分支"
        ],
        conventionalCommits: [
            "feat(ts-hello): add chapter 38 code organization",
            "fix(projects): close log analyzer file handle",
            "test(ts-hello): enumerate chapter registry"
        ],
        pullRequestChecklist: [
            "scope is narrow and named",
            "npm run build passes",
            "relevant dist chapter entry runs",
            "tests updated when behavior changes",
            "generated files or temp outputs are not committed"
        ]
    });

    note("常见坑", "一个提交同时改章节内容、测试框架、格式化和目录迁移，会让 review 和回滚都变困难。");
}

// =============================================================================
// 38.11 项目实战前 checklist：让入口薄、领域厚、边界清楚
// =============================================================================
//
// C++ 对照：进入项目实战前，先像设计库 API 一样明确公共入口、内部模块、错误协议和测试策略。
//
// C++ 思维提示：后续 ch39/ch40 的 CLI 都应把可复用逻辑放在 helper 模块，main.ts 只做入口编排。

function demoProjectReadinessChecklist(): void {
    section("38.11 项目实战前 checklist：让入口薄、领域厚、边界清楚");
    note("C++ 对照", "先设计模块边界，再写入口；这和先设计库接口再实现类似。");

    showJson("项目实战 checklist", [
        "目录表达职责：domain/config/adapters/cli/tests",
        "公共 API 通过明确 barrel 或 registry 暴露",
        "外部输入先按 unknown 校验，再进入领域类型",
        "配置优先级明确且可测试",
        "领域层返回 Result 或抛明确异常，入口层决定展示和退出",
        "文件、网络、定时器、stream 等资源在 finally 或生命周期结束时关闭",
        "日志脱敏，metrics/traces 与日志职责分开",
        "构建、测试、章节 dist 入口都能重复运行"
    ]);

    note("本章复盘", "代码组织的核心不是目录漂亮，而是让边界、责任和验证方式稳定可追溯。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 38 demos. */
export function runChapter(): void {
    demoDirectoryLayers();
    demoLintAndFormatWorkflow();
    demoNamingConventions();
    demoBarrelExports();
    demoRuntimeValidation();
    demoConfigManagement();
    demoErrorBoundaries();
    demoFrontendMinimumLoop();
    demoApiContractAndObservability();
    demoGitWorkflow();
    demoProjectReadinessChecklist();
}

await runIfMain(import.meta.url, runChapter);
