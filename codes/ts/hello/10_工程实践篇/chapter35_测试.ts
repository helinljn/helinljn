// =============================================================================
// 第 35 章：测试
// =============================================================================
//
// 【学习目标】
//   1. 理解测试金字塔、node:test 基本组织方式和常用断言
//   2. 掌握 mock.fn、mock.method、类型层测试、测试隔离和覆盖率的边界
//   3. 建立可重复测试习惯：临时目录、固定样例、确定性时间和异步资源清理
//
// 【运行方式】
//   npm run build
//   node dist/10_工程实践篇/chapter35_测试.js
//   或 npm run chapter -- 35（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { deepEqual, equal, ok, rejects, strictEqual, throws } from "node:assert/strict";
import { mkdtemp, readFile, rm, writeFile } from "node:fs/promises";
import { tmpdir } from "node:os";
import { join } from "node:path";
import { mock } from "node:test";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type User = {
    id: string;
    role: "reader" | "admin";
};

type UserService = {
    loadUser(id: string): Promise<User>;
};

type Report = {
    total: number;
    admins: number;
    readers: number;
};

// =============================================================================
// 35.1 测试金字塔：单元测试最多，E2E 最少
// =============================================================================
//
// C++ 对照：类似 Google Test/Catch2 中大量单元测试覆盖函数和类，再用少量集成测试覆盖模块边界。
//
// C++ 思维提示：测试越靠近顶层越慢、越脆、越依赖外部环境。核心逻辑优先写成纯函数，
// 让单元测试覆盖大部分分支。

function demoTestingPyramid(): void {
    section("35.1 测试金字塔：单元测试最多，E2E 最少");
    note("C++ 对照", "单元测试像直接测函数/类；集成测试测模块协作；E2E 测完整用户路径。");

    showJson("测试金字塔", [
        {
            layer: "unit",
            amount: "most",
            scope: "pure functions, validators, small services",
            cost: "fast and stable"
        },
        {
            layer: "integration",
            amount: "some",
            scope: "file system, local HTTP server, database adapter boundary",
            cost: "slower, needs cleanup"
        },
        {
            layer: "e2e",
            amount: "few",
            scope: "real CLI or user workflow",
            cost: "slowest, most environment-sensitive"
        }
    ]);
    note("常见坑", "不要只写 E2E 测试；它们能证明主路径能跑，但很难定位失败原因。");
}

// =============================================================================
// 35.2 node:test：test、describe、it 的组织方式
// =============================================================================
//
// C++ 对照：node:test 的 test/describe/it 类似 TEST、TEST_F 或 BDD 风格 describe/it。
//
// C++ 思维提示：本章正常运行时不启动测试 runner；真实测试文件应由 `node --test` 或
// `npm test` 执行，避免章节运行输出 TAP 测试协议。

function demoNodeTestShape(): void {
    section("35.2 node:test：test、describe、it 的组织方式");
    note("C++ 对照", "node:test 是 Node 内置测试框架，不需要额外安装 Jest/Vitest 才能入门。");

    showJson("node:test 代码形状", {
        testStyle: [
            "import test from \"node:test\";",
            "import assert from \"node:assert/strict\";",
            "test(\"parse port\", () => { assert.equal(parsePort(\"3000\"), 3000); });"
        ],
        bddStyle: [
            "import { describe, it } from \"node:test\";",
            "describe(\"config\", () => {",
            "  it(\"uses defaults\", () => { /* assert... */ });",
            "});"
        ],
        runCommand: "npm test",
        projectBoundary: "package.json pretest runs npm run build, then node --test dist/tests/*.js"
    });
    note("常见坑", "不要用裸 npx tsx 作为验收；本项目按计划使用 build + dist + node --test。");
}

// =============================================================================
// 35.3 断言：ok、equal、deepEqual、strictEqual、throws、rejects
// =============================================================================
//
// C++ 对照：这些断言对应 ASSERT_TRUE、EXPECT_EQ、EXPECT_THROW 等常见测试宏。
//
// C++ 思维提示：断言失败应该说明真实契约破坏，而不是只比较一大坨快照文本。优先断言
// 关键字段和边界行为。

function parsePort(input: string): number {
    const port = Number(input);
    if (!Number.isInteger(port) || port < 1 || port > 65_535) {
        throw new RangeError(`invalid port: ${input}`);
    }
    return port;
}

async function loadConfig(name: string): Promise<{ name: string; strict: boolean }> {
    if (name === "missing") {
        throw new Error("missing config");
    }
    return { name, strict: true };
}

async function demoAssertions(): Promise<void> {
    section("35.3 断言：ok、equal、deepEqual、strictEqual、throws、rejects");
    note("C++ 对照", "node:assert/strict 提供运行时断言，适合测试和内部不变量。");

    ok(parsePort("3000") > 0);
    equal(parsePort("3000"), 3000);
    strictEqual(typeof parsePort("3000"), "number");
    deepEqual(await loadConfig("demo"), { name: "demo", strict: true });
    throws(() => parsePort("0"), /invalid port/);
    await rejects(loadConfig("missing"), /missing config/);

    showJson("断言覆盖点", {
        ok: "truthy condition",
        equal: "strict comparison when imported from node:assert/strict",
        strictEqual: "Object.is style strict comparison",
        deepEqual: "deep structural comparison",
        throws: "sync error path",
        rejects: "async rejection path",
        allAssertionsPassed: true
    });
    note("类型边界", "断言检查运行时值；TypeScript 类型检查不能替代 rejects/throws 这类失败路径测试。");
}

// =============================================================================
// 35.4 Mock：mock.fn、mock.method 与调用记录
// =============================================================================
//
// C++ 对照：mock.fn 类似手写 fake function；mock.method 类似替换对象方法，观察调用次数和参数。
//
// C++ 思维提示：mock 适合隔离外部边界，但不要把所有依赖都 mock 掉。核心算法应尽量纯函数，
// 只在 I/O、时间、随机数、网络等边界 mock。

async function summarizeUser(service: UserService, id: string): Promise<string> {
    const user = await service.loadUser(id);
    return `${user.id}:${user.role}`;
}

async function demoMocks(): Promise<void> {
    section("35.4 Mock：mock.fn、mock.method 与调用记录");
    note("C++ 对照", "mock 用来替代外部依赖，并检查调用参数；类似测试 double。");

    const sendMetric = mock.fn((name: string, value: number) => `${name}=${value}`);
    const metricResult = sendMetric("cache.hit", 1);

    const service: UserService = {
        async loadUser(id: string) {
            return { id, role: "reader" };
        }
    };

    const loadUserMock = mock.method(service, "loadUser", async (id: string): Promise<User> => {
        return { id, role: "admin" };
    });

    try {
        const summary = await summarizeUser(service, "u-001");
        showJson("mock 调用记录", {
            mockFn: {
                result: metricResult,
                calls: sendMetric.mock.calls.length,
                firstArguments: sendMetric.mock.calls[0]?.arguments
            },
            mockMethod: {
                summary,
                calls: loadUserMock.mock.calls.length,
                firstArguments: loadUserMock.mock.calls[0]?.arguments
            }
        });
    } finally {
        mock.restoreAll();
    }

    note("常见坑", "mock 后要恢复；否则同一进程内后续测试可能被污染。");
}

// =============================================================================
// 35.5 类型层测试：@ts-expect-error、satisfies、compile-only
// =============================================================================
//
// C++ 对照：类型层测试类似用静态断言锁住模板/概念约束；它不运行，但会在编译期保护契约。
//
// C++ 思维提示：类型测试不能证明运行时 JSON 正确；它只保证源码层的类型约束没有被改坏。

function demoTypeLevelTests(): void {
    section("35.5 类型层测试：@ts-expect-error、satisfies、compile-only");
    note("C++ 对照", "类型层测试像 static_assert，失败发生在编译阶段。");

    const adminFixture = {
        id: "u-002",
        role: "admin"
    } satisfies User;

    showJson("类型层测试片段", {
        satisfiesRuntimeValue: adminFixture,
        expectErrorSnippet: [
            "// @ts-expect-error role must be reader or admin",
            "const badUser: User = { id: \"u-003\", role: \"owner\" };"
        ],
        compileOnlyPattern: [
            "export type Expect<T extends true> = T;",
            "type _RoleIsNarrow = Expect<User[\"role\"] extends \"reader\" | \"admin\" ? true : false>;"
        ]
    });
    note("常见坑", "@ts-expect-error 只应放在确实期待报错的下一行；如果未来不报错，编译会提醒你删除或修正。");
}

// =============================================================================
// 35.6 测试隔离：临时目录、固定样例、确定性时间和随机数
// =============================================================================
//
// C++ 对照：测试隔离类似每个测试 fixture 创建自己的临时目录和固定输入，结束后 tearDown。
//
// C++ 思维提示：测试失败越依赖当前时间、随机数、用户目录、网络和环境变量，越难复现。
// 把这些边界注入成参数或 fixture。

async function withTempWorkspace<T>(useWorkspace: (root: string) => Promise<T>): Promise<T> {
    const root = await mkdtemp(join(tmpdir(), "ts-hello-ch35-"));
    try {
        return await useWorkspace(root);
    } finally {
        await rm(root, { recursive: true, force: true });
    }
}

function buildReport(users: readonly User[]): Report {
    const admins = users.filter((user) => user.role === "admin").length;
    const readers = users.filter((user) => user.role === "reader").length;
    return { total: users.length, admins, readers };
}

async function demoTestIsolation(): Promise<void> {
    section("35.6 测试隔离：临时目录、固定样例、确定性时间和随机数");
    note("C++ 对照", "fixture 管理测试资源，避免测试之间共享可变文件或全局状态。");

    const result = await withTempWorkspace(async (root) => {
        const fixture = join(root, "users.json");
        const users: User[] = [
            { id: "u-001", role: "reader" },
            { id: "u-002", role: "admin" }
        ];
        await writeFile(fixture, `${JSON.stringify(users)}\n`, "utf8");
        const loaded = JSON.parse(await readFile(fixture, "utf8")) as User[];
        return buildReport(loaded);
    });

    const fixedNow = new Date("2026-05-19T08:30:00.000Z");
    const fixedRandom = 0.42;

    showJson("隔离测试数据", {
        report: result,
        fixedNow: fixedNow.toISOString(),
        fixedRandom,
        tempDirectoryPrinted: false
    });
    note("输出解释", "临时目录在 finally 中清理；时间和随机数用固定值，避免快照和断言漂移。");
}

// =============================================================================
// 35.7 异步测试：必须等待资源关闭
// =============================================================================
//
// C++ 对照：异步测试像启动线程、socket 或 timer 后必须 join/close；否则测试进程会挂起或污染后续用例。
//
// C++ 思维提示：返回 Promise 或使用 async test callback，让 runner 知道什么时候真正结束。
// server、watcher、worker、stream、interval 都要在 finally 中清理。

async function deterministicAsyncTask(input: string): Promise<string> {
    await Promise.resolve();
    if (input === "fail") {
        throw new Error("task failed");
    }
    return input.toUpperCase();
}

async function demoAsyncTests(): Promise<void> {
    section("35.7 异步测试：必须等待资源关闭");
    note("C++ 对照", "异步测试要等待 Promise 完成；资源要像 join/close 一样明确收口。");

    const value = await deterministicAsyncTask("ok");
    await rejects(deterministicAsyncTask("fail"), /task failed/);

    showJson("异步测试模式", {
        awaitedValue: value,
        testSnippet: [
            "test(\"async task\", async () => {",
            "  assert.equal(await deterministicAsyncTask(\"ok\"), \"OK\");",
            "  await assert.rejects(deterministicAsyncTask(\"fail\"), /task failed/);",
            "});"
        ],
        cleanupRule: "close server/watcher/worker/stream in finally"
    });
    note("常见坑", "不要在测试里 fire-and-forget Promise；未处理 rejection 可能在后续测试才爆出来。");
}

// =============================================================================
// 35.8 覆盖率与测试文件组织
// =============================================================================
//
// C++ 对照：覆盖率类似 gcov/lcov 报告；它衡量执行过哪些行/分支/函数，但不等于测试质量。
//
// C++ 思维提示：覆盖率数字可以防止漏测大块代码，但不能证明断言有意义。高覆盖率 + 弱断言
// 仍可能漏掉行为 bug。

function demoCoverageAndOrganization(): void {
    section("35.8 覆盖率与测试文件组织");
    note("C++ 对照", "覆盖率报告帮助发现未执行代码，但不是质量证明书。");

    showJson("覆盖率与组织", {
        coverageCommand: "node --experimental-test-coverage --test dist/tests/*.test.js",
        reportFields: {
            line: "lines executed",
            branch: "if/switch/conditional paths executed",
            function: "functions called"
        },
        fileOrganization: [
            "src/foo.ts + src/foo.test.ts for close unit tests",
            "tests/*.test.ts for cross-module or tutorial-wide tests",
            "fixtures/ for small deterministic input files"
        ],
        currentProject: {
            buildFirst: "npm run build",
            testCommand: "npm test",
            runner: "node --test dist/tests/chapters.test.js dist/tests/exercises.test.js dist/tests/projects.test.js"
        }
    });
    note("本章复盘", "测试要覆盖行为、边界和失败路径；隔离与可重复运行比测试数量更重要。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 35 demos. */
export async function runChapter(): Promise<void> {
    demoTestingPyramid();
    demoNodeTestShape();
    await demoAssertions();
    await demoMocks();
    demoTypeLevelTests();
    await demoTestIsolation();
    await demoAsyncTests();
    demoCoverageAndOrganization();
}

await runIfMain(import.meta.url, runChapter);
