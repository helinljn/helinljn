# TypeScript 6.0 教程改造计划

> 版本: 2.10 | 生成: 2026-05-18 | 最后更新: 2026-05-18
>
> **定位**：本文档是 `ts/hello` 教程改造的**唯一总规划**。
> **规则**：任意 agent 重新打开后，应首先阅读本文档以理解全局目标和当前进度，然后按 Phase 顺序推进，不得偏离。

---

## 一、背景与目标

### 1.1 现状

`ts/hello` 当前是一套面向 C++ 开发者的 TypeScript 6.0 入门教程，章节文件采用**骨架代码模式**：

- 每个 `.ts` 文件仅 30-60 行，包含几个类型/函数定义 + `runChapter()` 入口
- 教学内容通过 `note()` 一行带过，知识点密度极低
- 学习路线图 ~40 行，基础补充 ~40 行，过于简略
- 总计 13 个教学文件（11 章 + 2 项目）

作为对比，`python/hello` 教程采用**注释即教程模式**：

- 每个 `.py` 文件 600-1100 行，代码与中文注释深度交织（注释约占 60%+）
- 每个知识点独立 `demo` 函数，C++ 对照说明随处嵌入
- 学习路线图 638 行，基础补充 395 行，标准注释头块格式统一
- 总计 38 个教学文件（26 章 + 12 补充）

### 1.2 目标

将 `ts/hello` 改造为与 `python/hello` 一致的**深度教学模式**：

1. **格式对齐**：每一章采用标准注释头块、大段中文注释与代码交织、每个知识点独立 demo 函数
2. **内容全面**：覆盖 TS/JS/Node 全栈知识体系（不机械对照 Python 知识点）
3. **长期维护**：作为团队共享的线性教材

### 1.3 读者画像

| 维度 | 描述 |
|------|------|
| C++ 经验 | 10 年，含模板元编程 |
| TS/JS 经验 | 刚接触，零基础 |
| 学习目标 | 全栈（Node 后端为主，简单前端为辅） |
| 学习方式 | **线性通读**，从 ch01 到 ch40，每章自包含 |

### 1.4 核心决策

| 决策点 | 结论 |
|--------|------|
| 最大学习障碍 | **并发模型**——C++ 线程 vs Node 事件循环 |
| 并发教学策略 | **策略 B**：异步集中到 ch23-25，前面嵌入"C++ 思维提示" |
| 章节格式 | **策略 A**：注释量超代码量，对标 Python 端 |
| 章节覆盖 | **全保留**：40 章 |
| 维护定位 | 长期维护、团队共享 |
| Registry 更新 | **原子 checkpoint 更新**：每完成一个章节文件、项目入口或目录改名 checkpoint，即同步更新 chapterRegistry.ts 并验证 build |
| 旧文件处理 | **直接删除**：git 历史可追溯，不保留归档目录 |
| main.ts 策略 | **重写**：加入按阶段筛选、分组展示等增强功能 |

---

## 二、目录结构（最终态）

```
ts/hello/
├── plan.md
├── TypeScript学习路线图.md              # 重写（Phase 1）
├── TypeScript基础补充.md                # 重写（Phase 1）
├── README.md                            # 重写（Phase 1）
├── package.json
├── tsconfig.json
├── main.ts                              # 重写（Phase 5）
├── chapterRegistry.ts                   # 增量更新（Phase 2-4），最终审核（Phase 5）
├── shared/
│   └── chapter.ts                       # 微调（Phase 2）
│
├── 01_基础篇/                           # 5 章
│   ├── chapter01_入门与环境.ts          # 重写
│   ├── chapter02_变量与基本类型.ts      # 重写
│   ├── chapter03_控制流程.ts            # 新增
│   ├── chapter04_字符串处理.ts          # 新增
│   └── chapter05_数组与集合类型.ts      # 新增
│
├── 02_JS运行时篇/                       # 3 章（由"02_JS运行时语义篇"改名）
│   ├── chapter06_对象与引用.ts
│   ├── chapter07_闭包与作用域.ts
│   └── chapter08_原型与this.ts
│
├── 03_类型系统篇/                       # 5 章
│   ├── chapter09_类型标注与推断.ts
│   ├── chapter10_联合类型与收窄.ts
│   ├── chapter11_接口与类型别名.ts      # 新增
│   ├── chapter12_泛型.ts
│   └── chapter13_高级类型工具.ts
│
├── 04_函数篇/                           # 2 章（由"04_函数与泛型篇"改名）
│   ├── chapter14_函数详解.ts
│   └── chapter15_函数式编程模式.ts      # 新增
│
├── 05_面向对象篇/                       # 3 章
│   ├── chapter16_类与对象.ts
│   ├── chapter17_继承与多态.ts
│   └── chapter18_异常处理.ts            # 新增
│
├── 06_模块与工程篇/                     # 3 章
│   ├── chapter19_模块系统.ts
│   ├── chapter20_工程配置与声明文件.ts  # 新增
│   └── chapter21_装饰器.ts              # 新增
│
├── 07_迭代器与生成器篇/                 # 1 章（由"07_高级类型篇"改名）
│   └── chapter22_迭代器与生成器.ts
│
├── 08_异步编程篇/                       # 3 章（由"09_异步与网络篇"改名）
│   ├── chapter23_Promise深入.ts
│   ├── chapter24_async与await.ts        # 新增
│   └── chapter25_事件循环与并发.ts      # 新增
│
├── 09_Node_API篇/                       # 8 章（由"08_Node运行时与API篇"改名）
│   ├── chapter26_文件系统.ts            # 新增
│   ├── chapter27_路径进程与环境.ts      # 新增
│   ├── chapter28_Buffer与二进制.ts      # 新增
│   ├── chapter29_网络与HTTP.ts
│   ├── chapter30_加密与哈希.ts          # 新增
│   ├── chapter31_流.ts                  # 新增
│   ├── chapter32_日期与时间.ts          # 新增
│   └── chapter33_正则表达式.ts          # 新增
│
├── 10_工程实践篇/                       # 4 章（由"10_TS6工程约束篇"改名）
│   ├── chapter34_日志与调试.ts          # 新增
│   ├── chapter35_测试.ts                # 新增
│   ├── chapter36_性能分析.ts            # 新增
│   └── chapter37_TS6工程约束.ts
│
├── 11_项目实战/                         # 3 章
│   ├── chapter38_代码组织与规范.ts       # 重写
│   ├── chapter39_项目1_文件批处理CLI/
│   │   └── main.ts                       # 章节入口，导出 runChapter
│   └── chapter40_项目2_日志分析CLI/
│       └── main.ts                       # 章节入口，导出 runChapter
│
├── 练习题与答案/                        # 4 个（重写，Phase 5）
│   ├── exercises_01_基础.ts
│   ├── exercises_02_类型系统.ts
│   ├── exercises_03_泛型与OOP.ts
│   └── exercises_04_综合.ts
│
└── tests/                               # 3 个（Phase 5 更新）
    ├── chapters.test.ts
    ├── exercises.test.ts
    └── projects.test.ts
```

**统计**：教学 40 章 + 练习 4 个 + 测试 3 个 + 顶层文档 4 个 + 基础设施 5 个 = 约 56 个文件

---

## 三、章节文件格式规范

> **绝对遵守**。若某章节无对应 C++ 内容，写"（无直接对照）"也不得删除占位块。
> 所有 `.ts` 文件编码为 **UTF-8**，行尾以 `.editorconfig` 为准；当前项目为 **CRLF**。

### 3.1 文件模板（同步章节）

```typescript
// =============================================================================
// 第 X 章：章节标题
// =============================================================================
//
// 【学习目标】
//   1. xxx
//   2. xxx
//
// 【运行方式】
//   npm run build
//   node dist/对应目录/chapterXX_标题.js
//   或 npm run chapter -- X（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

// =============================================================================
// X.1 小节标题
// =============================================================================
//
// C++ 对照：...
//
// [如涉及异步] C++ 思维提示：...

function demoXxx(): void {
    section("X.1 小节标题");
    note("C++ 对照", "...");
    // 演示代码
}

// =============================================================================
// 章节入口
// =============================================================================

export function runChapter(): void {
    demoXxx();
}

await runIfMain(import.meta.url, runChapter);
```

### 3.1b 文件模板（异步章节）

> 适用于 ch23-25、ch29 等涉及 Promise/async/await/fetch/定时器的章节。

```typescript
// =============================================================================
// 第 X 章：章节标题
// =============================================================================
//
// 【学习目标】
//   1. xxx
//
// 【运行方式】
//   npm run build
//   node dist/对应目录/chapterXX_标题.js
//   或 npm run chapter -- X（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

// =============================================================================
// X.1 小节标题
// =============================================================================
//
// C++ 对照：...
//
// C++ 思维提示：...

async function demoXxx(): Promise<void> {
    section("X.1 小节标题");
    note("C++ 对照", "...");
    // 异步演示代码
}

// =============================================================================
// 章节入口
// =============================================================================

export async function runChapter(): Promise<void> {
    await demoXxx();
}

await runIfMain(import.meta.url, runChapter);
```

### 3.1c 项目章节入口模板（ch39/ch40）

> 项目目录可以包含多个 helper 文件，但只有 `main.ts` 是章节入口。helper 文件不要求注释头块、`runChapter` 或 `runIfMain`。

```typescript
// =============================================================================
// 第 X 章：项目标题
// =============================================================================
//
// 【学习目标】
//   1. xxx
//
// 【运行方式】
//   npm run build
//   node dist/11_项目实战/chapterXX_项目名/main.js
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { runIfMain, section } from "../../shared/chapter.js";

export async function runProjectDemo(): Promise<void> {
    section("第 X 章：项目标题");
    // 项目演示入口
}

export async function runChapter(): Promise<void> {
    await runProjectDemo();
}

await runIfMain(import.meta.url, runChapter);
```

### 3.2 要点

| 规则 | 说明 |
|------|------|
| 注释头 | 必须有 `// ====` 包围的注释头块 |
| 小节分隔 | 每个知识点 `// ====` + 编号 + 标题 |
| C++ 对照 | 每个小节必有 |
| 异步提示 | 涉及 setTimeout/fetch/Promise/async 的小节必嵌入 |
| JSDoc | 导出函数/类型/类必有 JSDoc |
| demo 函数 | 每小节一个独立 demo |
| 入口 | 末尾 runChapter() + await runIfMain()。异步章节用 `async function runChapter(): Promise<void>` |
| 行数 | 核心章节 400-800，API 章节 200-400 |
| 编码 | UTF-8，行尾以 `.editorconfig` 为准；当前项目为 CRLF，不得同一文件混用 |

### 3.3 辅助函数（shared/chapter.ts）

- `section(title)` — 输出小节标题
- `note(label, text)` — 输出标签提示
- `showJson(label, value)` — JSON 格式化输出
- `runIfMain(metaUrl, run)` — 判断直接运行，支持同步和异步 run 函数

### 3.3b 可复现执行与验证命令

> **强制规则**：默认验证命令不得依赖未锁定的 `npx tsx`。当前 `package.json` 未固定 `tsx`，因此任意 agent 重开后必须使用下面的 build + dist 方式验证。

| 场景 | 命令 |
|------|------|
| 单个章节文件验证 | `npm run build` 后执行 `node dist/<目录>/chapterXX_标题.js` |
| 项目章节入口验证 | `npm run build` 后执行 `node dist/11_项目实战/chapter39_项目1_文件批处理CLI/main.js` 或 ch40 对应入口 |
| registry 已同步后的编号运行 | `npm run chapter -- X` |
| 全量编译 | `npm run build` |
| 全量测试 | `npm test` |

若未来决定使用 `tsx` 快捷运行，必须先把 `tsx` 加入 `devDependencies` 并更新 `package-lock.json`，之后统一使用 `npm exec -- tsx <文件>`。不得使用会临时联网解析版本的裸 `npx tsx <文件>` 作为验收依据。

若 `npm test` 在受限执行环境中因 `spawn EPERM` 失败，应在允许 Node test runner 创建子进程的环境中重跑。只有重跑后仍失败才视为代码或计划问题。

### 3.4 命名规范

- 文件：`chapterXX_中文标题.ts`
- demo：`demoXxx()` 驼峰
- 导出：语义化英文名

### 3.5 文件操作类型标注

每个文件在 Phase 操作表中标注以下三种操作类型之一：

| 操作类型 | 含义 | 示例 |
|----------|------|------|
| **基于旧文件改写** | 旧文件存在且内容有参考价值，在此基础上大幅扩充 | ch01、ch09、ch12 等 |
| **纯新建** | 无旧文件对应，从零创建 | ch03、ch04、ch07 等 |
| **删除旧文件后新建** | 旧文件名或位置与新文件不同；先完成新文件和 registry 切换，确认 build 通过后再删除旧文件 | ch02（旧文件名为"变量类型与控制流"） |

### 3.6 Phase 字段值规范

> 每个注册项在 `chapterRegistry.ts` 中的 `phase` 字段必须使用以下统一值，不得自行定义。40 个教学章节使用 11 个教学阶段；4 个练习统一使用 `"练习"`。

| 目录 | phase 值 |
|------|---------|
| `01_基础篇/` | `"基础"` |
| `02_JS运行时篇/` | `"JS运行时"` |
| `03_类型系统篇/` | `"类型系统"` |
| `04_函数篇/` | `"函数"` |
| `05_面向对象篇/` | `"面向对象"` |
| `06_模块与工程篇/` | `"模块与工程"` |
| `07_迭代器与生成器篇/` | `"迭代器与生成器"` |
| `08_异步编程篇/` | `"异步编程"` |
| `09_Node_API篇/` | `"Node API"` |
| `10_工程实践篇/` | `"工程实践"` |
| `11_项目实战/` | `"项目实战"` |
| `练习题与答案/` | `"练习"` |

### 3.7 原子 checkpoint 规则

> 目标：任意对话中断后，新 agent 能只靠磁盘状态 + `plan.md` 恢复，不依赖聊天上下文。

每个 checkpoint 必须满足以下条件后才算完成：

| checkpoint 类型 | 完成条件 |
|-----------------|----------|
| 新增/重写章节入口 | 源文件完整存在；`npm run build` 通过；`node dist/<对应入口>.js` 可运行；registry 已同步且再次 `npm run build` 通过 |
| 项目入口适配 | 项目 `main.ts` 导出 `runChapter`；保留项目内原 demo 函数；registry 指向新路径；build 通过 |
| 目录改名 | 目录实际路径与最终态一致；registry 中无旧路径 import；build 通过 |
| 旧文件删除 | 对应新文件已存在并可运行；registry 无旧路径 import；删除后 build 通过 |
| Phase 完成 | 该 Phase 所有 checkpoint 完成；Phase 验收命令通过；第七章进度表更新为 completed |

第七章进度表只记录 Phase 级完成状态。若 Phase 为 pending 但磁盘上已有部分新文件，按 §10.3 和本节 checkpoint 规则恢复，不得仅凭进度表从头覆盖已有可运行文件。

### 3.8 内容质量 Rubric

> 目标：避免生成“行数达标但教学密度不足”的章节。每个章节入口文件必须同时满足以下质量要求。

| 维度 | 必须满足 |
|------|----------|
| 概念解释 | 每个小节先用中文注释解释概念、适用场景和边界，再给代码 |
| C++ 对照 | 每个小节给出 C++ 类比或明确写“无直接对照”，不得只写口号 |
| 常见坑 | 每章至少包含 2 个常见坑或误区，并说明如何避免 |
| 可运行 demo | 每个 demo 必须参与 `runChapter()` 执行，输出可读、稳定、无随机失败 |
| 输出解释 | 关键输出前后必须有注释或 `note()` 解释为什么会得到该结果 |
| 真实场景 | 每章至少包含 1 个贴近工程实践的小例子，而不是只演示孤立语法 |
| 类型边界 | 涉及 `any`、断言、类型擦除、运行时 API 时，必须说明 TS 检查范围和 JS/Node 运行时边界 |
| 线性阅读 | 不引用后续章节才会解释的概念；如确实需要提前出现，必须给“先知道即可”的过渡说明 |

章节行数只是最低约束。若某章覆盖点较多，应优先保证解释完整、demo 可运行和 C++ 对照清晰，不得为了压缩行数删掉关键讲解。

### 3.9 副作用与可重复运行边界

> 目标：所有章节和项目都能被反复运行，且适合纳入全量验收。

| 类型 | 规则 |
|------|------|
| 文件系统 | 示例写入临时目录或章节专属 `.tmp` 目录；运行结束必须清理，或明确保留为示例输出并加入 `.gitignore` |
| 网络与 HTTP | 默认只启动本地 server；不得依赖公网；server 必须在 demo 结束前关闭 |
| 定时器与异步 | 所有 `setTimeout`、interval、stream、watcher、worker 都必须有退出路径，章节运行不得挂起 |
| 随机与时间 | 涉及随机数或当前时间时，输出必须稳定；可使用固定种子、固定样例时间或只输出范围说明 |
| 环境变量 | 不要求用户机器已有特定环境变量；示例应提供 fallback |
| 子进程 | 只运行跨平台、确定性的命令；不得依赖 shell 私有语法或外部工具 |
| 资源清理 | 文件句柄、server、watcher、worker、stream 必须关闭；异常路径也要清理 |

若某个 API 无法做到跨平台稳定演示，应把代码写成说明性小 demo，并在注释中解释平台差异，不得让全量验收依赖不稳定行为。

### 3.10 版本敏感内容 Source of Truth

> 当前本地基线：Node.js `24.15.0`，TypeScript `6.0.3`，`tsconfig.json` 使用 `target: "ES2023"`、`module: "NodeNext"`、`lib: ["ES2023", "DOM"]`。

涉及 TypeScript、Node、Web API 或 TC39 预览能力时，必须按以下顺序确认：

1. 先看本仓库 `package.json`、`package-lock.json`、`tsconfig.json`。
2. 再用本地命令确认：`node --version`、`npm run build`、`npx --no-install tsc --version`、`npx --no-install tsc --all`。
3. 若 API 在当前 `lib` 或 Node 运行时中不可直接使用，只能写成 feature-detect、说明性代码或注释说明，不得写成默认可运行路径。
4. 对 `Temporal`、decorators、`using`、`--erasableSyntaxOnly`、fetch/Web API 类型等版本敏感内容，必须在章节中明确“当前教程采用的可运行边界”。
5. 如果本地验证与记忆中的版本行为冲突，以本地验证结果为准。

---

## 四、各章知识点详列

> agent 生成每章时必须严格覆盖全部知识点，不得遗漏或自由发挥。

> **模板标注说明**：以下章节标题中，**未标注的默认使用同步模板(§3.1)**，标注 `[异步]` 的使用异步模板(§3.1b)。异步模板的核心区别：`demo` 函数为 `async`，`runChapter` 为 `async function runChapter(): Promise<void>`。

### 01_基础篇（ch01-05）

**ch01_入门与环境**
- Node.js/npm 安装验证、项目初始化、package.json
- tsc --version/--init、第一个 TS 程序
- tsconfig.json 最小配置、npm run build/start 工作流
- VS Code 配置、本教程学习方式
- C++ 对照：g++/CMake vs tsc/npm scripts

**ch02_变量与基本类型**
- let vs const vs var（块作用域、TDZ）
- number（IEEE 754）、string（模板字面量）、boolean
- null vs undefined、bigint、symbol
- 类型标注 vs 类型推断、typeof、字面量类型入门
- C++ 对照：声明顺序、const 语义差异

**ch03_控制流程**
- if-else、三元、switch（exhaustiveness check）
- for/while/do-while、for-of vs for-in
- break/continue、标签语句
- 短路求值（&&/||/??）、可选链 ?.
- C++ 对照：无 goto、switch fall-through

**ch04_字符串处理**
- 模板字面量、标签函数
- slice/substring/split/replace/trim/padStart/padEnd
- includes/startsWith/endsWith/indexOf
- Unicode 与 .length 的坑、String.raw
- C++ 对照：std::string 方法 vs String

**ch05_数组与集合类型**
- 数组创建、push/pop/shift/unshift/splice/slice
- map/filter/reduce（含手动实现理解）
- find/findIndex/some/every/includes、flat/flatMap
- sort、Map vs Object、Set、WeakMap/WeakSet
- readonly 数组与 as const
- C++ 对照：std::vector vs Array、std::map vs Map

### 02_JS运行时篇（ch06-08）

**ch06_对象与引用**
- 对象字面量、属性访问、引用语义（核心差异）
- 展开运算符、Object.assign、Object.freeze/seal
- 可选链 ?. 深入、逻辑赋值运算符
- Object.keys/values/entries/fromEntries

**ch07_闭包与作用域**
- 词法作用域（let/const 块级 vs var 函数级）
- 闭包定义与原理、实战（计数器/once/模块模式）
- 内存影响、IIFE、循环中的闭包陷阱
- C++ 对照：lambda 显式捕获 vs 闭包自动捕获

**ch08_原型与this**
- 原型链：[[Prototype]]/__proto__/prototype
- Object.create、this 四种绑定规则
- 箭头函数 this、call/apply/bind
- C++ 对照：虚函数表 vs 原型链

### 03_类型系统篇（ch09-13）

**ch09_类型标注与推断**
- 标注位置、推断规则、as const
- satisfies 运算符、类型断言 as T
- typeof 类型操作符
- C++ 对照：auto vs 推断、static_cast vs as

**ch10_联合类型与收窄**
- 联合类型 A|B、收窄手段（typeof/instanceof/in/===）
- 判别联合 + switch 穷尽性
- never、unknown vs any、类型谓词
- C++ 对照：std::variant vs 联合类型

**ch11_接口与类型别名**
- interface vs type 取舍指南
- extends vs &、索引签名、函数类型接口
- readonly、结构化类型 vs 名义类型
- excess property checks（对象字面量额外属性检查）vs assignment compatibility（变量赋值兼容）
- branded/opaque type 模式：用交叉类型模拟名义类型，避免把 userId/orderId 混用
- variance 基础：函数参数逆变、方法参数双变的历史兼容性、回调类型设计注意事项

**ch12_泛型**
- 泛型函数、约束 extends、泛型接口/别名
- 默认值、keyof、typeof+泛型组合、推导 vs 显式
- C++ 对照：模板 vs 泛型——无特化、运行时擦除

**ch13_高级类型工具**
- 映射类型、条件类型、infer、模板字面量类型
- Pick/Omit/Partial/Required/Readonly/Record/Exclude/Extract/NonNullable/ReturnType/Parameters/Awaited
- 递归类型、asserts 断言
- C++ 对照：模板元编程 vs 高级类型

### 04_函数篇（ch14-15）

**ch14_函数详解**
- 函数声明 vs 表达式 vs 箭头
- 参数类型、可选/默认值、剩余参数、解构
- 函数重载、this 参数标注、void vs undefined
- C++ 对照：默认参数、函数重载

**ch15_函数式编程模式**
- 函数一等公民：赋值/传参/返回
- 手动实现 map/filter/reduce
- compose/pipe、curry
- 闭包实战：memoize/once/debounce/throttle
- C++ 对照：std::function vs 闭包

### 05_面向对象篇（ch16-18）

**ch16_类与对象**
- class/constructor、实例/静态属性/方法、getter/setter
- private/protected/public（TS 类型层）vs #（JS 运行时）
- readonly、参数属性、implements
- C++ 对照：访问控制编译后消失

**ch17_继承与多态**
- extends、super、override、抽象类
- implements 多个接口、结构化类型多态
- Mixin 模式
- C++ 对照：虚继承 vs 单继承+接口

**ch18_异常处理**
- throw、try-catch-finally、Error 层级
- 自定义错误类、catch 类型 unknown
- Promise 异常、Error.cause
- throw vs Result/Either 风格：库代码、CLI、HTTP handler 中的取舍
- 自定义错误码、可恢复/不可恢复错误分类、CLI exit code 设计
- async 边界统一捕获：顶层 `runChapter()`、HTTP handler、项目入口如何收敛异常
- C++ 对照 + 异步思维提示

### 06_模块与工程篇（ch19-21）

**ch19_模块系统**
- ESM import/export、默认 vs 命名、import type
- import.meta、CJS 简要、互操作、.js 后缀规则
- NodeNext ESM 实战：`"type": "module"`、`module: "NodeNext"`、`moduleResolution: "NodeNext"` 的联动
- TS 源码中相对导入写 `.js` 后缀的原因：类型检查源码、运行时加载 dist JS
- `package.json` 的 `exports` / `imports`、Node subpath imports（含 `#/` 前缀概念）
- JSON import attributes：使用 `with { type: "json" }`，不使用已弃用的 `assert { type: "json" }`
- CJS/ESM interop 限制：默认导入、命名导入、动态 import、require(ESM) 的边界
- barrel export、C++ 对照：#include vs 模块

**ch20_工程配置与声明文件**
- tsconfig.json 核心选项、tsc --init、references
- .d.ts 声明文件、declare、@types
- 类型声明产物：`declaration`、`declarationMap`、`emitDeclarationOnly`、`declarationDir`
- 库发布基础：package `types` 字段、`exports` 搭配类型、源码/产物目录分离
- 消费方类型体验：只暴露公共 API，避免把内部 helper 类型泄漏到 `.d.ts`
- npm 依赖管理：semver、`package-lock.json`、`npm ci` vs `npm install`、`npm outdated`、安全升级策略
- C++ 对照：头文件 vs 声明文件

**ch21_装饰器**
- 标准装饰器：类/方法/accessor/字段装饰器、装饰器工厂
- 当前 `tsconfig.json` 不启用 `experimentalDecorators`，不得编写 legacy 参数装饰器示例
- 执行顺序、实战（日志/计时/权限）
- C++ 对照：类似 AOP

### 07_迭代器与生成器篇（ch22）

**ch22_迭代器与生成器** [异步]
- Symbol.iterator、for-of 机制、自定义迭代器
- function*、yield/yield*、next()
- 异步生成器、for await...of
- 懒加载实战
- C++ 对照：C++20 coroutine vs generator

### 08_异步编程篇（ch23-25）

**ch23_Promise深入** [异步]
- 状态机、then/catch/finally、链式调用
- Promise.all/allSettled/race/any
- promisify、Promise.withResolvers、微任务
- C++ 对照：std::future/std::promise vs Promise
- [全文含] C++ 思维提示

**ch24_async与await** [异步]
- async 返回 Promise、await 暂停语义
- try-catch+await、并行 vs 串行
- 循环中 await、顶层 await、AbortController
- C++ 对照：C++20 co_await

**ch25_事件循环与并发** [异步]
- **核心专题：C++ 线程 vs Node 事件循环**
- 调用栈/任务队列/微任务队列
- 事件循环六阶段
- setTimeout/setImmediate/nextTick 顺序
- Worker Threads 基础、单线程优缺点

### 09_Node_API篇（ch26-33）

**ch26_文件系统** [异步]
- 导入：`import { readFile, writeFile, mkdir, readdir, stat, rm, rename, copyFile, watch, mkdtemp } from "node:fs/promises"`
- 读取：`readFile(path, "utf8")` 返回字符串，不带编码返回 Buffer
- 写入：`writeFile`（覆盖）、`appendFile`（追加）
- 目录：`mkdir({ recursive: true })`、`readdir`、`rm({ recursive: true })`
- 元数据：`stat`（size/mtime/isDirectory/isFile）
- 大文件分块读取（过渡到 ch31 流的概念）
- 文件监听：`watch`（注意跨平台差异）
- 临时文件：`mkdtemp` 创建临时目录
- 路径注意事项：相对路径基于 `process.cwd()`
- [异步] C++ 思维提示：fs/promises 的异步操作不阻塞事件循环——文件 I/O 在线程池中执行

**ch27_路径进程与环境**
- path 模块：`join`、`resolve`、`dirname`、`basename`、`extname`、`parse`、`normalize`、`relative`
- `__dirname` vs `import.meta.dirname`（ESM 中的差异：`__dirname` 不可用）
- process：`argv`（CLI 参数，前两个是 node 路径和脚本路径）、`env`（环境变量）
- 配置管理：默认值、环境变量、CLI 参数、配置文件的优先级；secret 不写入源码或日志
- `cwd()` vs `import.meta.dirname`（当前工作目录 vs 脚本所在目录）
- `exit(code)`、`platform`、`arch`
- `process.stdout` / `stderr` / `stdin`
- child_process 基础：`exec`（简单命令，缓冲输出）、`spawn`（流式交互）
- C++ 对照：argv / getenv / fork+exec vs process / child_process

**ch28_Buffer与二进制**
- 创建：`Buffer.from(str, "utf8")`、`Buffer.alloc(size)`、`Buffer.allocUnsafe(size)`（未初始化，更快但需手动填充）
- 编码转换：UTF-8、Base64、Hex、Latin1
- 读写方法：`readUInt8` / `readUInt16BE` / `readUInt32LE` / `writeUInt32BE` 等
- `TypedArray`（Uint8Array/Uint16Array 等）与 Buffer 的关系（Buffer 是 Uint8Array 的子类）
- 字节序（大端 BE vs 小端 LE）的指定
- `DataView` 灵活读写（适用于混合类型二进制数据）
- 二进制协议解析示例（简单消息头+体）
- C++ 对照：`uint8_t*` / `std::array<std::byte>` vs Buffer

**ch29_网络与HTTP** [异步]
- http 模块：`createServer((req, res) => { ... }).listen(port)`
- 请求对象：method、url、headers
- 响应对象：`writeHead(statusCode, headers)`、`end(body)`
- `fetch`（Node 18+ 内置）：GET/POST、自定义 headers、`response.json()`
- URL 解析：`new URL(urlString)`、searchParams
- 请求体读取与 runtime validation：外部 JSON 先按 `unknown` 处理，再通过 type guard 转为业务类型
- HTTP 错误处理：400/404/500 的职责边界、错误响应不泄漏内部栈
- 安全基础：路径遍历、命令注入、请求体大小限制、只启动本地 server 做 demo
- HTTP 后端最小闭环：routing、middleware 概念、health check、graceful shutdown
- API 契约：JSON Schema/OpenAPI 的概念边界，说明如何约束请求/响应但不强制引入依赖
- https 基础（证书概念简述，不深入）
- Server-Sent Events（SSE）入门：`Content-Type: text/event-stream`
- [异步] C++ 思维提示：C++ 网络编程通常每连接一个线程或使用 asio；Node 的 HTTP 服务器天然异步，每个请求只是事件循环中的一个任务

**ch30_加密与哈希**
- crypto 模块概览（基于 OpenSSL）
- 哈希：`createHash("sha256").update(data).digest("hex")`
- 支持算法：SHA256 / SHA3-256 / MD5（MD5 仅用于非安全校验）
- 分块更新：`hash.update(chunk)` 处理大文件
- HMAC 带密钥哈希：`createHmac("sha256", key)`
- 随机数：`randomBytes(size)`、`randomUUID()`
- 密码哈希：`pbkdf2(password, salt, iterations, keylen, digest)` — 故意慢以抵抗暴力破解
- `scrypt` 简介（内存硬哈希，更强）
- 对称加密入门：`createCipheriv("aes-256-gcm", key, iv)` / `createDecipheriv`
- 安全边界：hash 不等于 encryption、不要自创密码学、secret 不写入源码或日志
- C++ 对照：OpenSSL C API vs crypto 模块

**ch31_流** [异步]
- 四种类型：Readable（可读）、Writable（可写）、Duplex（双工）、Transform（转换）
- 两种模式：paused mode（需手动 read）vs flowing mode（data 事件自动推送）
- `pipeline(readable, transform, writable, callback)` — 推荐用法，自动处理背压和清理
- `pipe`（遗留 API）vs `pipeline` 的差异
- 背压（backpressure）：生产快于消费时自动暂停 readable
- 文件流：`fs.createReadStream` + `fs.createWriteStream`
- 压缩流：`zlib.createGzip()` / `createGunzip()` 配合 pipeline
- `readline.createInterface` 逐行处理大文件
- 自定义 Transform 流（如实现大小写转换）
- stream cleanup：`pipeline`/`finished` 处理错误和关闭资源，避免文件句柄泄漏
- async iterable streams：`for await...of` 消费 readable，和 backpressure 的关系
- [异步] C++ 思维提示：iostream 是同步阻塞的；Node stream 是异步事件驱动的，数据以 chunk 为单位通过事件循环传递

**ch32_日期与时间**
- Date 对象：`new Date()`、`Date.now()`（毫秒时间戳）、`getFullYear()` / `getMonth()` / `getDate()`
- 常见坑：月份从 0 开始（getMonth() 返回 0-11）、默认本地时区、`Date.parse` 跨浏览器不一致
- 格式化：`toLocaleString()`、`Intl.DateTimeFormat`（国际化格式化，推荐）
- 日期计算：时间戳加减（ms 单位）、注意 DST（夏令时）切换日的坑
- 第三方库提示：`date-fns`（轻量，函数式）、`luxon`（时区友好）
- `Temporal` API（Stage 3 预览）：`Temporal.Now.plainDateISO()`、`Temporal.Duration`
- C++ 对照：`std::chrono::system_clock` / `time_point` vs Date——chrono 类型安全，Date 可变且易出错

**ch33_正则表达式**
- 创建：字面量 `/pattern/flags` vs `new RegExp("pattern", "flags")`（注意字符串中需双重转义）
- 常用标志：`g`（全局匹配）、`i`（忽略大小写）、`m`（多行模式）、`s`（dotAll——`.` 匹配换行）、`u`（Unicode 模式）、`d`（返回匹配索引）
- 方法：`test(str)` 返回 boolean、`exec(str)` 返回匹配详情（含分组和索引）
- String 方法：`match`、`matchAll`（返回迭代器）、`replace`、`search`、`split`
- 字符类：`\d`（数字）、`\w`（单词字符）、`\s`（空白）、`[abc]`（自定义类）、`[^abc]`（排除类）
- 量词：`*` / `+` / `?` / `{n}` / `{n,m}`、贪婪（默认）vs 懒惰（加 `?`）
- 分组：捕获组 `(...)`、非捕获 `(?:...)`、命名捕获 `(?<name>...)`、反向引用 `\1` / `$<name>`
- 断言：前瞻 `(?=...)`、负前瞻 `(?!...)`、后顾 `(?<=...)`（ES2018+）、负后顾 `(?<!...)`
- 实战案例：邮箱校验、URL 提取、日志字段解析
- C++ 对照：`std::regex` vs RegExp——语法兼容 ECMAScript 标准，但 JS 原生支持后顾断言

### 10_工程实践篇（ch34-37）

**ch34_日志与调试**
- console 深入：`console.log` / `warn` / `error` / `debug` / `info`、`console.table`（表格化数组/对象输出）、`console.time` / `timeEnd`（计时对）
- 结构化日志：JSON 格式输出规范（timestamp/level/message/context 字段）、`JSON.stringify` 配合 console
- 日志安全：脱敏 token/password/email/IP，避免把 secret、完整请求体、堆栈细节写入普通日志
- 可观测性概念：logs、metrics、traces 的职责差异；本教程只实现日志和简单计时，不引入后端监控依赖
- debug 库：命名空间 `debug("app:db")`、级别控制、`DEBUG` 环境变量（如 `DEBUG=app:*`）
- Node 调试：`node --inspect`（运行时附加 DevTools）、`node --inspect-brk`（首行暂停）
- Chrome DevTools：Sources 断点、Watch、Call Stack、Console 交互
- VS Code 调试配置：`.vscode/launch.json`、F5 启动、断点调试
- `node:assert` 基本用法：`assert.ok` / `equal` / `deepEqual` / `strictEqual` / `throws`（运行时断言，区别于类型断言）
- C++ 对照：std::cout / GDB / assert vs console / --inspect / node:assert

**ch35_测试**
- 测试金字塔概念：单元测试（最多）→ 集成测试 → E2E 测试（最少）
- `node:test` 基本用法：`test(name, fn)`、`describe(name, fn)`、`it(name, fn)`（BDD 风格）
- 断言：`assert.ok(cond)`、`assert.equal(actual, expected)`、`assert.deepEqual(actual, expected)`（深度比较对象/数组）、`assert.strictEqual(actual, expected)`（=== 比较）、`assert.throws(fn)` / `assert.rejects(promise)`
- Mock：`mock.method(obj, methodName)`（替换对象方法）、`mock.fn(impl)`（创建 mock 函数）、mock 调用记录（`.mock.calls`）
- 类型层测试：`// @ts-expect-error`、`satisfies`、compile-only 测试，用来锁定类型契约
- 测试隔离：临时目录、固定样例数据、确定性时间/随机数、异步测试必须等待资源关闭
- 覆盖率：`node --experimental-test-coverage --test`、覆盖率报告解读（行/分支/函数覆盖率）
- 测试文件组织：`*.test.ts` 命名、与源文件同目录或集中 `tests/` 目录
- C++ 对照：Google Test / Catch2 vs node:test——JS 测试框架内置，无需额外库

**ch36_性能分析**
- `performance.now()`：高精度时间戳（毫秒，精度微秒级）
- `performance.mark(name)` / `performance.measure(name, start, end)`：标记和测量区间
- `console.time(label)` / `console.timeEnd(label)`：快速计时
- `process.hrtime.bigint()`：纳秒精度时间戳
- V8 采样分析：`node --prof app.js`（生成 isolate 日志）、`node --prof-process isolate-*.log`（生成可读报告）
- 火焰图工具：`0x` 简介（一键生成火焰图）
- 常见性能陷阱：
  - 同步操作阻塞事件循环（如 `readFileSync` 处理大文件）
  - 内存泄漏（闭包持有大对象、全局变量未释放、EventEmitter 监听器未移除）
  - 不必要的 JSON 序列化/反序列化
- 轻量可观测性：用计数器、耗时分布、错误率理解服务状态；解释 metrics/tracing 与日志的边界
- C++ 对照：std::chrono / perf / valgrind vs performance API / --prof

**ch37_TS6工程约束**
- TS 6.0 迁移重点：`rootDir` 默认变为 tsconfig 所在目录；必要时显式设置 `"rootDir": "."` 或 `"rootDir": "./src"`
- TS 6.0 迁移重点：`types` 默认不再枚举所有 `@types`；Node 项目应显式写 `"types": ["node"]`
- TS 6.0 迁移重点：在含 `tsconfig.json` 的目录中执行 `tsc file.ts` 会报错；要么 `tsc -p tsconfig.json`，要么显式 `--ignoreConfig`
- TS 6.0 弃用项：`moduleResolution: "node"/"node10"`、`moduleResolution: "classic"`、`target: "es5"`、`baseUrl`、`outFile`、AMD/UMD/SystemJS 等旧模块模式
- TS 5.8 引入的 `--erasableSyntaxOnly`：配合 Node 直接 strip types 的边界；禁止 enum、runtime namespace、parameter properties、`import =`/`export =`
- import assertions → import attributes：使用 `with { type: "json" }`，不使用 `assert { type: "json" }`
- 现代 TS/JS 资源管理：`using`/显式资源管理作为现代能力讲解，但不得误写成 TS 6.0 独有特性
- 版本锁定策略：`package.json` 中 `"typescript": "~6.0.3"`（补丁自动升级）、`package-lock.json` 锁定精确版本
- 依赖升级工作流：先 `npm outdated` 评估，再小步升级，使用 `npm ci` 复现 lockfile，最后跑 build/test/章节全量验证
- 与 TS 7 Beta 的边界梳理：`--erasableSyntaxOnly` 默认开启、装饰器 Stage 3 规范、类型导入 elision 行为变更——本教程**不默认采用** TS 7 Beta 行为
- C++ 对照：C++11/14/17/20 标准版本管理 vs TS 版本演进——TS 向后兼容更好，大版本升级通常零改动

### 11_项目实战（ch38-40）

**ch38_代码组织与规范**
- 目录结构：按功能分层（如 `src/` / `tests/` / `scripts/`）、按领域分组（如 `models/` / `services/` / `routes/`）
- ESLint + Prettier：安装配置、与 TS 集成、pre-commit hook（lint-staged）
- 命名规范：文件（kebab-case）、变量/函数（camelCase）、类/接口（PascalCase）、常量（UPPER_SNAKE_CASE）
- Barrel export：`index.ts` 聚合导出、减少 import 路径深度
- Runtime validation 组织方式：外部输入层使用 `unknown`，通过 type guard/validator 转成领域类型
- 配置管理分层：默认配置、环境变量、CLI 参数、配置文件合并；优先级必须可解释、可测试
- 错误处理分层：domain 返回 Result，入口层决定打印、HTTP status 或 exit code
- 前端最小闭环：DOM 类型、事件对象、表单输入、fetch、TSX/React 边界（只做概念和最小示例，不展开框架）
- API 契约与可观测性：把 JSON Schema/OpenAPI、logs/metrics/traces 作为真实项目边界概念介绍
- Git 工作流：分支策略（main/dev/feature）、commit message 规范（Conventional Commits）

**ch39_项目1_文件批处理CLI**
- `main.ts` 作为章节入口，导出 `runChapter(): Promise<void>`；可保留 `runFileBatchDemo()` 供测试复用
- CLI 参数解析（`process.argv` 手写，不使用第三方库）
- CLI 参数 runtime validation：把字符串参数解析为明确配置类型，错误时返回可读 message 和非 0 exit code
- 目录遍历（`readdir` 递归）
- 文本转换（读文件 → 处理 → 写文件）
- 批量重命名
- 安全与可靠性：路径遍历防护、输出目录限制、dry-run、覆盖保护、临时文件 + 原子 rename
- 日志输出、配置文件支持

**ch40_项目2_日志分析CLI**
- `main.ts` 作为章节入口，导出 `runChapter(): Promise<void>`；可保留 `runLogAnalyzerDemo()` 供测试复用
- 按行解析（`readline`）
- 正则提取关键字段
- 日志行 runtime validation：解析失败行计数、错误样例收集、不中断全量处理
- 聚合统计（Map 计数、分组）
- JSON/CSV 报告输出
- 安全与隐私：日志脱敏、CSV escaping、避免把 token/IP 等敏感数据原样输出到报告
- 流式处理大文件

---

## 五、执行 Phase

> **核心规则**：
> - 每个原子 checkpoint 完成时必须更新 chapterRegistry.ts，确保 `npm run build` 零错误。
> - 旧文件处理采用**直接删除**（git 可追溯）。
> - Phase 内各文件按顺序生成，每完成一个文件用 §3.3b 的 build + dist 命令验证可独立运行。
> - 目录改名、旧文件删除、registry 修改都视为 checkpoint；checkpoint 之后不得留下 import 指向不存在文件的状态。
> - 目录改名、registry 路径迁移、`npm run build` 必须作为同一个不可中断 checkpoint 完成；若不能一次完成，不得开始目录改名。

---

### Phase 1：顶层文档

| 文件 | 操作 | 操作类型 |
|------|------|----------|
| `TypeScript学习路线图.md` | 重写 | 基于旧文件改写 |
| `TypeScript基础补充.md` | 重写 | 基于旧文件改写 |
| `README.md` | 重写 | 基于旧文件改写 |

**前置条件**：项目处于改造前初始状态（旧 13 个骨架章节文件均存在）

**完成标志**：
- 路线图覆盖 40 章全部知识点，>= 500 行
- 基础补充 >= 200 行
- README 指向新 11 阶段、含完整命令

**验收 checklist**：
- `TypeScript学习路线图.md`：列出 40 章完整顺序；每章包含学习目标、核心知识点、建议练习；并明确 11 个阶段的学习路径
- `TypeScript基础补充.md`：覆盖 TS/JS/Node/Web API 边界、类型擦除、Node ESM、异步模型、版本基线，且不少于 200 行
- `README.md`：包含环境要求、安装命令、build/test/chapter 运行命令、目录说明、11 阶段学习顺序、重开 agent 后应阅读 `plan.md` 的说明
- 三个文档不得引用已删除的旧章节编号、旧目录名或旧命令 `npx tsx`
- `npm run build` 必须仍然通过

**Registry**：Phase 1 不改动 registry（文档不影响编译）

---

### Phase 2：基础篇 ch01-05 + shared 微调

| 文件 | 操作 | 操作类型 | 目标行数 |
|------|------|----------|----------|
| `shared/chapter.ts` | 微调 | 基于旧文件改写 | 按需 |
| `01_基础篇/chapter01_入门与环境.ts` | 重写 | 基于旧文件改写 | 400-600 |
| `01_基础篇/chapter02_变量与基本类型.ts` | 重写 | 删除旧文件后新建 | 500-800 |
| `01_基础篇/chapter03_控制流程.ts` | 新增 | 纯新建 | 400-600 |
| `01_基础篇/chapter04_字符串处理.ts` | 新增 | 纯新建 | 400-600 |
| `01_基础篇/chapter05_数组与集合类型.ts` | 新增 | 纯新建 | 600-800 |

**旧文件处理**：删除 `01_基础篇/chapter02_变量类型与控制流.ts`（必须在 ch02 新文件已创建、registry 已切换且 build 通过后执行）

**前置条件**：Phase 1 完成

**完成标志**：5 个章节文件均可通过 §3.3b 的 build + dist 命令独立运行输出正确

**验收步骤**：
1. 逐文件执行 `npm run build` + `node dist/01_基础篇/chapterXX_标题.js` 验证
2. 更新 chapterRegistry.ts（替换 ch01/ch02，新增 ch03/ch04/ch05）
3. `npm run build` 零错误

**Registry 更新要点**：
- ch01：路径不变，内容替换后 import 路径保持一致
- ch02：路径更新为新文件名，id=2，phase="基础"
- ch03-05：全新注册，phase="基础"

---

### Phase 3：ch06-18

| 文件 | 操作 | 操作类型 | 来源/备注 |
|------|------|----------|-----------|
| `02_JS运行时篇/chapter06_对象与引用.ts` | 重写 | 基于旧文件改写 | 旧 ch03 拆分 |
| `02_JS运行时篇/chapter07_闭包与作用域.ts` | 新增 | 纯新建 | |
| `02_JS运行时篇/chapter08_原型与this.ts` | 新增 | 纯新建 | |
| `03_类型系统篇/chapter09_类型标注与推断.ts` | 重写 | 基于旧文件改写 | 旧 ch04 拆分 |
| `03_类型系统篇/chapter10_联合类型与收窄.ts` | 重写 | 基于旧文件改写 | 旧 ch04 拆分 |
| `03_类型系统篇/chapter11_接口与类型别名.ts` | 新增 | 纯新建 | |
| `03_类型系统篇/chapter12_泛型.ts` | 重写 | 基于旧文件改写 | 旧 ch05 拆分 |
| `03_类型系统篇/chapter13_高级类型工具.ts` | 重写 | 基于旧文件改写 | 旧 ch08 |
| `04_函数篇/chapter14_函数详解.ts` | 重写 | 基于旧文件改写 | 旧 ch05 拆分 |
| `04_函数篇/chapter15_函数式编程模式.ts` | 新增 | 纯新建 | |
| `05_面向对象篇/chapter16_类与对象.ts` | 重写 | 基于旧文件改写 | 旧 ch06 |
| `05_面向对象篇/chapter17_继承与多态.ts` | 新增 | 纯新建 | |
| `05_面向对象篇/chapter18_异常处理.ts` | 新增 | 纯新建 | |

**目录改名**（本 Phase 开始立即执行，且必须与 registry 路径迁移和 `npm run build` 作为同一个 checkpoint 完成）：
1. `02_JS运行时语义篇` → `02_JS运行时篇`
2. `04_函数与泛型篇` → `04_函数篇`

**旧文件删除**（在对应新文件创建后执行）：
| 删除文件 | 时机 |
|----------|------|
| `02_JS运行时篇/chapter03_JS运行时语义.ts` | ch06-08 全部完成后（目录已改名） |
| `03_类型系统篇/chapter04_类型系统基础.ts` | ch09/ch10 完成后 |
| `04_函数篇/chapter05_函数与泛型.ts` | ch12/ch14 完成后（目录已改名） ⚠️ ch12 在 `03_类型系统篇/`，ch14 在 `04_函数篇/`，需跨目录确认两者均已完成 |
| `05_面向对象篇/chapter06_面向对象.ts` | ch16 完成后 |
| `07_高级类型篇/chapter08_高级类型.ts` | ch13 完成后 |

**前置条件**：Phase 2 完成

**完成标志**：13 个章节文件均可通过 §3.3b 的 build + dist 命令独立运行

**验收步骤**：
1. 逐文件执行 `npm run build` + `node dist/<对应目录>/chapterXX_标题.js` 验证
2. 增量更新 chapterRegistry.ts（每完成一个文件即注册）
3. `npm run build` 零错误

---

### Phase 4：ch19-37

#### Phase 4a：目录改名（本 Phase 第一步，且必须与 registry 路径迁移和 `npm run build` 作为同一个 checkpoint 完成）

> 涉及 08/09 编号互换的目录，必须按以下三步顺序操作：

```
步骤 1：08_Node运行时与API篇 → 08_tmp
步骤 2：09_异步与网络篇 → 08_异步编程篇
步骤 3：08_tmp → 09_Node_API篇
```

> ⚠️ 中断恢复：如果重新打开后发现 `08_tmp` 目录存在，说明上次在步骤 1 或步骤 2 之后中断。此时：
> - 若 `08_异步编程篇` 已存在 → 执行步骤 3
> - 若 `08_异步编程篇` 不存在 → 先执行步骤 2，再执行步骤 3

其余改名：
- `07_高级类型篇` → `07_迭代器与生成器篇`
- `10_TS6工程约束篇` → `10_工程实践篇`

#### Phase 4b：文件生成

| 文件 | 操作 | 操作类型 | 来源/备注 |
|------|------|----------|-----------|
| `06_模块与工程篇/chapter19_模块系统.ts` | 重写 | 基于旧文件改写 | 旧 ch07 |
| `06_模块与工程篇/chapter20_工程配置与声明文件.ts` | 新增 | 纯新建 | |
| `06_模块与工程篇/chapter21_装饰器.ts` | 新增 | 纯新建 | |
| `07_迭代器与生成器篇/chapter22_迭代器与生成器.ts` | 新增 | 纯新建 | |
| `08_异步编程篇/chapter23_Promise深入.ts` | 新增 | 纯新建 | |
| `08_异步编程篇/chapter24_async与await.ts` | 新增 | 纯新建 | |
| `08_异步编程篇/chapter25_事件循环与并发.ts` | 新增 | 纯新建 | |
| `09_Node_API篇/chapter26_文件系统.ts` | 新增 | 纯新建 | |
| `09_Node_API篇/chapter27_路径进程与环境.ts` | 新增 | 纯新建 | |
| `09_Node_API篇/chapter28_Buffer与二进制.ts` | 新增 | 纯新建 | |
| `09_Node_API篇/chapter29_网络与HTTP.ts` | 重写 | 基于旧文件改写 | 旧 ch10 拆分（三步互换后旧文件在 `08_异步编程篇/chapter10_异步与网络.ts`） |
| `09_Node_API篇/chapter30_加密与哈希.ts` | 新增 | 纯新建 | |
| `09_Node_API篇/chapter31_流.ts` | 新增 | 纯新建 | |
| `09_Node_API篇/chapter32_日期与时间.ts` | 新增 | 纯新建 | |
| `09_Node_API篇/chapter33_正则表达式.ts` | 新增 | 纯新建 | |
| `10_工程实践篇/chapter34_日志与调试.ts` | 新增 | 纯新建 | |
| `10_工程实践篇/chapter35_测试.ts` | 新增 | 纯新建 | |
| `10_工程实践篇/chapter36_性能分析.ts` | 新增 | 纯新建 | |
| `10_工程实践篇/chapter37_TS6工程约束.ts` | 重写 | 基于旧文件改写 | 旧 ch11 |

**旧文件删除**（在对应新文件创建后执行）：
| 删除文件 | 时机 |
|----------|------|
| `06_模块与工程篇/chapter07_模块与工程.ts` | ch19 完成后 |
| `09_Node_API篇/chapter09_Node运行时与API.ts` | ch26-33 全部完成后（Phase 4a 目录改名后此文件位于 `09_Node_API篇/`，在新章节生成期间保留作为参考材料） |
| `08_异步编程篇/chapter10_异步与网络.ts` | ch23-25 + ch29 全部完成后（目录已改名） |
| `10_工程实践篇/chapter11_TS6工程约束.ts` | ch37 完成后（目录已改名） |

**前置条件**：Phase 3 完成

**完成标志**：19 个章节文件均可通过 §3.3b 的 build + dist 命令独立运行

**验收步骤**：
1. 逐文件执行 `npm run build` + `node dist/<对应目录>/chapterXX_标题.js` 验证
2. 增量更新 chapterRegistry.ts
3. `npm run build` 零错误

---

### Phase 5：收尾

> **执行顺序**（按依赖关系，必须从上到下执行）：
> 1. 重命名项目目录（ch39/ch40）—— 目录操作，无代码依赖；若新旧目录同时存在，按 §11.2 先合并再清理
> 2. 创建/重写 `11_项目实战/chapter38_代码组织与规范.ts`
> 3. 适配 ch39/ch40 项目入口 `main.ts`：作为章节入口导出 `runChapter`，并保留项目内可复用 demo 函数
> 4. **最终审核 `chapterRegistry.ts`** —— 确认 40 个教学章节 + 4 个练习全部注册、路径正确、phase 值符合 §3.6
> 5. 确认 registry 不再 import 旧 `chapter12_代码组织与规范.ts` 后，删除该旧文件
> 6. 重写 `main.ts` —— 依赖 registry 条目正确
> 7. 重写 4 个练习文件 —— 依赖章节内容
> 8. 更新 3 个测试文件 —— 依赖练习和章节
> 9. `npm run build` + `npm test` 验证
> 10. 全局验收标准检查（见第六章）

---

| 文件/任务 | 操作 | 操作类型 |
|-----------|------|----------|
| `main.ts` | 重写 | 基于旧文件改写 |
| `chapterRegistry.ts` | 最终审核 | 基于 Phase 2-4 增量版本审核 |
| `11_项目实战/chapter38_代码组织与规范.ts` | 重写 | 基于旧文件改写 |
| `11_项目实战/chapter39_项目1_文件批处理CLI/` | 目录重命名 | 由 `chapter12_项目1_文件批处理工具` 改名 |
| `11_项目实战/chapter39_项目1_文件批处理CLI/main.ts` | 入口适配 | 基于旧文件改写 |
| `11_项目实战/chapter40_项目2_日志分析CLI/` | 目录重命名 | 由 `chapter13_项目2_日志分析工具` 改名 |
| `11_项目实战/chapter40_项目2_日志分析CLI/main.ts` | 入口适配 | 基于旧文件改写 |
| `练习题与答案/exercises_01_基础.ts` | 重写 | 基于旧文件改写 |
| `练习题与答案/exercises_02_类型系统.ts` | 重写 | 基于旧文件改写 |
| `练习题与答案/exercises_03_泛型与OOP.ts` | 重写 | 基于旧文件改写 |
| `练习题与答案/exercises_04_综合.ts` | 重写 | 基于旧文件改写 |
| `tests/chapters.test.ts` | 更新 | 基于旧文件改写 |
| `tests/exercises.test.ts` | 更新 | 基于旧文件改写 |
| `tests/projects.test.ts` | 更新 | 基于旧文件改写 |
| 旧文件清理 | 删除 | `11_项目实战/chapter12_代码组织与规范.ts`（仅在 ch38 已创建、registry 已切换且 build 通过后删除） |

**main.ts 重写范围**：
- 加入按阶段（phase）筛选功能：新增命令 `P 基础` 列出该阶段所有章节
- `showChapterList()` 改为按 phase 分组展示（11 个教学阶段 + 练习分组）
- `showMenu()` 更新命令列表（新增 P 命令）
- `showHelp()` 更新说明
- 章节编号从旧 1-14 更新为新 1-40
- `runAllChapters()` 保持遍历逻辑不变

**chapterRegistry.ts 最终审核**：
- 确认 40 个教学章节 + 4 个练习全部注册
- 确认所有导入路径正确
- 确认 phase 字段值符合 §3.6（40 章对应 11 个教学阶段，练习对应 `"练习"`）

**前置条件**：Phase 4 完成

**完成标志**：`npm run build` 零错误 + `npm test` 全部通过 + 全部 40 章可通过 §3.3b 的 build + dist 命令运行

**验收步骤**：
1. `npm run build`
2. `npm test`
3. 全量运行 40 个教学章节入口：逐个执行 `node dist/<对应目录>/<入口>.js`
4. 全量做章节质量 checklist：40 个教学章节逐个对照 §3.8 检查知识点覆盖、C++ 对照、常见坑、真实场景和类型边界
5. 抽查 5-10 个章节做人工深度复核，重点检查讲解密度、线性阅读体验和输出解释
6. 检查运行后无残留临时目录、未关闭 server、挂起 watcher 或多余输出文件

---

## 六、全局验收标准

- [ ] `npm run build` 编译零错误（tsc --noEmit 等效检查）
- [ ] `npm test` 全部通过
- [ ] 所有 40 个章节可通过 §3.3b 的 build + dist 命令独立运行
- [ ] 所有 40 个章节已按 §3.8 做内容质量复核：讲解、C++ 对照、常见坑、真实场景均达标
- [ ] 涉及文件、网络、定时器、stream、watcher、worker、子进程的章节符合 §3.9 副作用边界
- [ ] 涉及版本敏感 API 的章节符合 §3.10，并已按本地 Node/TypeScript 基线验证
- [ ] 全部章节已在 chapterRegistry.ts 注册
- [ ] 每个章节入口 `.ts` 文件第一行以 `// ====` 开头（格式规范自动化检查）
- [ ] 每个章节入口 `.ts` 文件包含 `export function runChapter` 或 `export async function runChapter`
- [ ] 每个章节入口 `.ts` 文件末尾包含 `await runIfMain(import.meta.url, runChapter)`
- [ ] 项目章节 ch39/ch40 的 `main.ts` 作为章节入口；项目内部 helper、types、config 文件不要求 `runChapter`
- [ ] 学习路线图覆盖 40 章全部知识点
- [ ] TypeScript基础补充.md >= 200 行
- [ ] README 含完整命令、11 阶段学习顺序和目录说明
- [ ] `tests/chapters.test.ts` 或等效脚本从 `chapterRegistry.ts` 枚举 40 个教学章节并逐个运行，避免手工漏跑
- [ ] 无残留旧文件（旧目录 `02_JS运行时语义篇`、`04_函数与泛型篇`、`07_高级类型篇`、`08_Node运行时与API篇`、`09_异步与网络篇`、`10_TS6工程约束篇` 均不存在）
- [ ] 无残留临时目录（如 `08_tmp`）

---

## 七、进度追踪

| Phase | 状态 | 完成日期 | 备注 |
|-------|------|---------|------|
| Phase 1 | ✅ completed | 2026-05-18 | 顶层文档重写 |
| Phase 2 | ✅ completed | 2026-05-18 | ch01-05 + shared 微调 + 首次 registry 更新 |
| Phase 3 | ✅ completed | 2026-05-18 | ch06-18 + 2 个目录改名 |
| Phase 4 | ⬜ pending | - | ch19-37 + 4 个目录改名（含 08/09 互换） |
| Phase 5 | ⬜ pending | - | main.ts 重写 + registry 最终审核 + 练习 + 测试 |

---

## 八、旧文件处理指南

> 策略：**直接删除**。git 历史可追溯，不保留归档目录。

| 旧文件（初始路径） | 执行 Phase 时的实际路径 | 处理 | 阶段 | 删除时机 |
|--------|------|------|------|----------|
| `01_基础篇/chapter02_变量类型与控制流.ts` | 同左（此目录不改名） | 删除 | Phase 2 | ch02 新文件完成、registry 切换且 build 通过后 |
| `02_JS运行时语义篇/chapter03_JS运行时语义.ts` | Phase 3 改名后：`02_JS运行时篇/chapter03_JS运行时语义.ts` | 删除 | Phase 3 | ch06-08 全部完成、registry 切换且 build 通过后 |
| `03_类型系统篇/chapter04_类型系统基础.ts` | 同左（此目录不改名） | 删除 | Phase 3 | ch09-10 完成、registry 切换且 build 通过后 |
| `04_函数与泛型篇/chapter05_函数与泛型.ts` | Phase 3 改名后：`04_函数篇/chapter05_函数与泛型.ts` | 删除 | Phase 3 | ch12+ch14 完成、registry 切换且 build 通过后 |
| `05_面向对象篇/chapter06_面向对象.ts` | 同左（此目录不改名） | 删除 | Phase 3 | ch16 完成、registry 切换且 build 通过后 |
| `06_模块与工程篇/chapter07_模块与工程.ts` | 同左（此目录不改名） | 删除 | Phase 4 | ch19 完成、registry 切换且 build 通过后 |
| `07_高级类型篇/chapter08_高级类型.ts` | 同左（Phase 4 才改此目录名，Phase 3 删除时路径不变） | 删除 | Phase 3 | ch13 完成、registry 切换且 build 通过后 |
| `08_Node运行时与API篇/chapter09_Node运行时与API.ts` | Phase 4a 三步互换后：`09_Node_API篇/chapter09_Node运行时与API.ts` | 删除 | Phase 4 | ch26-33 全部完成、registry 切换且 build 通过后 |
| `09_异步与网络篇/chapter10_异步与网络.ts` | Phase 4a 三步互换后：`08_异步编程篇/chapter10_异步与网络.ts` | 删除 | Phase 4 | ch23-25+ch29 全部完成、registry 切换且 build 通过后 |
| `10_TS6工程约束篇/chapter11_TS6工程约束.ts` | Phase 4a 改名后：`10_工程实践篇/chapter11_TS6工程约束.ts` | 删除 | Phase 4 | ch37 完成、registry 切换且 build 通过后 |
| `11_项目实战/chapter12_代码组织与规范.ts` | 同左（此目录不改名） | 删除 | Phase 5 | ch38 已创建、registry 已切换且 build 通过后 |
| `11_项目实战/chapter12_项目1_文件批处理工具/` | 同左 | 目录重命名→ch39 | Phase 5 | 重命名即完成 |
| `11_项目实战/chapter13_项目2_日志分析工具/` | 同左 | 目录重命名→ch40 | Phase 5 | 重命名即完成 |

---

## 九、给 Agent 的操作指南

1. **首次打开**：执行第十章自检清单
2. **查看进度**：跳到第七章进度表，找到第一个 pending Phase
3. **执行**：
   - 读取该 Phase 的文件表
   - 按顺序逐文件生成（每个文件完成后按 §3.3b 验证）
   - 每完成一个文件或项目入口，更新 chapterRegistry.ts
   - Phase 内全部文件完成后，执行 Phase 验收步骤
4. **Phase 完成**：更新第七章进度表
5. **旧文件与目录改名**：参照第八章删除旧文件，参照第十二章执行目录改名（Phase 3 → 序号 1-2，Phase 4 → 序号 3-7，Phase 5 → 序号 8-9）
6. **不偏离**：不改目录结构、编号、格式规范
7. **中断恢复**：参照第十一章
8. **生成每个章节时**：参照第四章知识点详列，不得遗漏
9. **进度更新**：Phase 完成后直接编辑 plan.md 第七章进度表，将状态改为 `✅ completed` 并填写日期
10. **验证命令参考**：
    - 单文件：`npm run build` 后 `node dist/<目录>/chapterXX_标题.js`
    - 全量编译：`npm run build`
    - 全量测试：`npm test`
    - 注意：`npm run chapter -- X` 依赖 registry 和 build，仅在 registry 已更新的情况下可用

---

## 十、Agent 首次打开自检清单

> 任意 agent 重新打开对话后，在开始任何工作前执行以下检查：

### 10.1 基础自检

1. 读取本文档（`plan.md`），确认版本号和最后更新日期
2. 跳到第七章进度表，确认当前进度
3. 检查磁盘目录列表，对比第十二章的目录操作总表

### 10.2 目录状态检查

执行以下检查（用 `list_dir` 工具逐一查看）：

| 检查项 | 应存在 | 不应存在 |
|--------|--------|----------|
| `01_基础篇/` | ✅（始终） | - |
| `02_JS运行时篇/` | Phase 3 完成后 | - |
| `02_JS运行时语义篇/` | - | Phase 3 完成后 |
| `03_类型系统篇/` | ✅（始终） | - |
| `04_函数篇/` | Phase 3 完成后 | - |
| `04_函数与泛型篇/` | - | Phase 3 完成后 |
| `05_面向对象篇/` | ✅（始终） | - |
| `06_模块与工程篇/` | ✅（始终） | - |
| `07_迭代器与生成器篇/` | Phase 4 完成后 | - |
| `07_高级类型篇/` | - | Phase 4 完成后 |
| `08_Node运行时与API篇/` | - | Phase 4 完成后 |
| `08_异步编程篇/` | Phase 4 完成后 | - |
| `09_Node_API篇/` | Phase 4 完成后 | - |
| `09_异步与网络篇/` | - | Phase 4 完成后 |
| `10_工程实践篇/` | Phase 4 完成后 | - |
| `10_TS6工程约束篇/` | - | Phase 4 完成后 |
| `11_项目实战/` | ✅（始终） | - |
| `shared/` | ✅（始终） | - |
| `tests/` | ✅（始终） | - |
| `练习题与答案/` | ✅（始终） | - |
| `08_tmp/` | - | **永远不应存在**（若存在说明中断，见 11.2） |

### 10.3 Phase 内进度判断

若进度表显示某 Phase 为 `pending`，但该 Phase 的部分文件已存在：
- 检查已有文件的行数是否达到目标范围
- 检查已有文件按 §3.3b 是否可运行
- 检查已有文件是否满足 §3.8 内容质量 Rubric、§3.9 副作用边界和 §3.10 版本边界
- 检查已有文件是否已在 chapterRegistry.ts 注册
- 根据检查结果决定从哪个文件继续

### 10.4 chapterRegistry.ts 状态检查

检查 registry 中注册的章节数量。若 registry 条目数明显少于该 Phase 应有的文件数，说明该 Phase 未完成，需要从缺失的文件继续生成。

---

## 十一、中断恢复指南

### 11.1 一般原则

- 每个 `.ts` 章节入口文件的生成是**原子操作**——文件要么完整存在且可按 §3.3b 运行，要么不存在
- 如果文件存在但内容不完整：删除后重新生成
- 如果文件存在且可运行：跳过，继续下一个

### 11.2 目录改名操作中断恢复

**场景 A**：`08_tmp` 目录存在

```
若 08_异步编程篇/ 已存在：
  执行：08_tmp → 09_Node_API篇（Phase 4 步骤 3）
若 08_异步编程篇/ 不存在：
  执行：09_异步与网络篇 → 08_异步编程篇（Phase 4 步骤 2）
  再执行：08_tmp → 09_Node_API篇（Phase 4 步骤 3）
```

**场景 B**：旧目录名和新目录名同时存在

不得直接删除旧目录。按以下顺序处理：

1. 分别列出旧目录和新目录的文件清单。
2. 若旧目录中存在新目录缺失的文件，先迁移或重新生成对应文件。
3. 确认 registry 已指向新目录，且 `npm run build` 通过。
4. 仅在旧目录无独有内容、registry 无旧路径 import 时，删除旧目录。
5. 若无法判断某个文件是否应迁移，停止修改并在最终回复中列出冲突文件，等待人工确认。

### 11.3 chapterRegistry.ts 中断恢复

如果 registry 中存在导入路径指向不存在的文件：
- 删除对应的 import 行和 CHAPTERS 条目，或立即生成完整章节文件后再注册。
- 不允许为了通过 build 创建空占位章节文件；占位会破坏“可重开继续执行”的真实进度判断。

### 11.4 Phase 4b 内部中断恢复

Phase 4b 涉及 19 个章节文件，若中途中断：

1. 检查 `chapterRegistry.ts`，删除所有导入路径指向不存在文件的 import 和 CHAPTERS 条目
2. 列出 Phase 4b 文件表（见 §Phase 4b），跳过磁盘上已存在且可按 §3.3b 运行的文件
3. 从第一个不存在（或存在但无法运行）的文件开始继续生成
4. 每完成一个文件立即更新 registry（增量注册），确保 registry 状态与磁盘一致
5. 对涉及文件、网络、stream、watcher、worker 的章节额外检查 §3.9，确保可重复运行且不会挂起
6. 旧文件删除条件满足时及时删除（见 Phase 4b 旧文件删除表）

> 注意：ch29 标注为"基于旧文件改写"，旧文件在三步互换后位于 `08_异步编程篇/chapter10_异步与网络.ts`。若该文件在中断前已被删除但 ch29 尚未完成，agent 应从第四章知识点详列重新生成 ch29。

---

## 十二、目录操作总表

> 按执行顺序列出所有目录重命名操作。agent 在每个 Phase 的第一步执行该表。

| 序号 | 旧目录名 | 新目录名 | Phase | 备注 |
|------|----------|----------|-------|------|
| 1 | `02_JS运行时语义篇` | `02_JS运行时篇` | Phase 3 | 简单改名 |
| 2 | `04_函数与泛型篇` | `04_函数篇` | Phase 3 | 简单改名 |
| 3 | `08_Node运行时与API篇` | `08_tmp` | Phase 4 | 三步互换第 1 步 |
| 4 | `09_异步与网络篇` | `08_异步编程篇` | Phase 4 | 三步互换第 2 步 |
| 5 | `08_tmp` | `09_Node_API篇` | Phase 4 | 三步互换第 3 步 |
| 6 | `07_高级类型篇` | `07_迭代器与生成器篇` | Phase 4 | 简单改名 |
| 7 | `10_TS6工程约束篇` | `10_工程实践篇` | Phase 4 | 简单改名 |
| 8 | `11_项目实战/chapter12_项目1_文件批处理工具` | `11_项目实战/chapter39_项目1_文件批处理CLI` | Phase 5 | 子目录改名 |
| 9 | `11_项目实战/chapter13_项目2_日志分析工具` | `11_项目实战/chapter40_项目2_日志分析CLI` | Phase 5 | 子目录改名 |

---

*本文档版本 2.10。任何修改须更新版本号和最后更新日期。*
