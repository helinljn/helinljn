# TypeScript 6.0 Learning

这是一套面向 C++ 背景开发者的 TypeScript 6.0 系统教程。

目标不是只学语法，而是建立三层边界：

- JavaScript：对象、闭包、原型、Promise、事件循环等真实运行时语义。
- TypeScript：类型系统、编译期检查、工程配置和版本约束。
- Node/Web API：文件、网络、进程、Buffer、fetch 等平台能力。

## 环境要求

- Node.js `24.15.0`
- npm `11.14.1`
- TypeScript `~6.0.3`，通过 `package-lock.json` 锁定
- ESLint `^10.4.0`，使用 flat config 配置

确认版本：

```bat
node --version
npm --version
```

安装依赖。推荐使用 `npm ci`，它会严格按 `package-lock.json` 复现依赖版本：

```bat
npm ci
```

当前开发依赖：

```text
@eslint/js         ^10.0.1
@types/node        25.5.0
eslint             ^10.4.0
typescript         ~6.0.3
typescript-eslint  ^8.59.4
```

## 常用命令

编译：

```bat
npm run build
```

清理编译产物：

```bat
npm run clean
```

检查代码规范：

```bat
npm run lint
```

自动修复可修复的 lint 问题：

```bat
npm run lint:fix
```

运行主菜单：

```bat
npm run start
```

按章节编号运行：

```bat
npm run chapter -- 1
```

运行测试：

```bat
npm test
```

单个章节的可复现验收方式是先 build，再运行 dist 中对应入口：

```bat
npm run build
node dist/01_基础篇/chapter01_入门与环境.js
```

项目章节入口同样先 build，再运行 dist 中的 `main.js`。

## 代码规范

项目使用 ESLint flat config，配置文件是 `eslint.config.js`。

当前配置重点：

- 对 TypeScript 源码启用 `typescript-eslint` 的类型感知规则。
- 使用 `tsconfig.json` 作为类型检查上下文。
- 忽略 `dist/`、`node_modules/`、`coverage/`。
- 对教程章节和练习文件放宽部分规则，避免教学示例中的 `any`、`var`、未 await Promise 等演示代码被强行改写。
- 对测试文件放宽 `no-floating-promises`，适配 Node 内置 test runner 的注册写法。

`.editorconfig` 负责编辑器层面的基础格式，例如 UTF-8、4 空格缩进、CRLF 换行和文件末尾换行；ESLint 负责代码质量、类型安全和工程规则。两者职责不同，当前配置不冲突。

## 学习顺序

推荐按 [TypeScript学习路线图.md](./TypeScript学习路线图.md) 从 ch01 线性读到 ch40。

11 个阶段如下：

1. `01_基础篇`：环境、变量、基本类型、控制流程、字符串、数组和集合。
2. `02_JS运行时篇`：对象引用、闭包、作用域、原型和 `this`。
3. `03_类型系统篇`：类型标注、联合收窄、接口、类型别名、泛型和高级类型。
4. `04_函数篇`：函数签名、参数、重载、`this` 参数和函数式模式。
5. `05_面向对象篇`：class、访问控制、继承、多态和异常处理。
6. `06_模块与工程篇`：ESM、NodeNext、声明文件、依赖管理和装饰器边界。
7. `07_迭代器与生成器篇`：同步迭代、生成器、异步生成器和懒加载。
8. `08_异步编程篇`：Promise、async/await、事件循环和 Node 并发模型。
9. `09_Node_API篇`：文件系统、路径进程、Buffer、HTTP、crypto、stream、日期和正则。
10. `10_工程实践篇`：日志调试、测试、性能分析和 TypeScript 6.0 工程约束。
11. `11_项目实战`：代码组织、文件批处理 CLI、日志分析 CLI。

基础概念可配合 [TypeScript基础补充.md](./TypeScript基础补充.md) 复盘。

## 最终目录结构

```text
.
├── TypeScript学习路线图.md
├── TypeScript基础补充.md
├── README.md
├── .editorconfig
├── eslint.config.js
├── package.json
├── package-lock.json
├── tsconfig.json
├── main.ts
├── chapterRegistry.ts
├── shared/
├── 01_基础篇/
├── 02_JS运行时篇/
├── 03_类型系统篇/
├── 04_函数篇/
├── 05_面向对象篇/
├── 06_模块与工程篇/
├── 07_迭代器与生成器篇/
├── 08_异步编程篇/
├── 09_Node_API篇/
├── 10_工程实践篇/
├── 11_项目实战/
├── 练习题与答案/
└── tests/
```

## 章节编号

教学章节最终为 40 章：

- ch01-ch05：基础篇。
- ch06-ch08：JS 运行时篇。
- ch09-ch13：类型系统篇。
- ch14-ch15：函数篇。
- ch16-ch18：面向对象篇。
- ch19-ch21：模块与工程篇。
- ch22：迭代器与生成器篇。
- ch23-ch25：异步编程篇。
- ch26-ch33：Node API 篇。
- ch34-ch37：工程实践篇。
- ch38-ch40：项目实战。

练习文件位于 `练习题与答案/`，测试文件位于 `tests/`。

## 版本和可运行边界

本教程默认基线：

- `target: "ES2023"`
- `module: "NodeNext"`
- `moduleResolution: "NodeNext"`
- `lib: ["ES2023", "DOM"]`
- `types: ["node"]`

涉及 TypeScript、Node、Web API 或 TC39 预览能力时，以本地 `package.json`、`package-lock.json`、`tsconfig.json` 和实际命令验证为准。

如果某 API 在当前基线不可稳定运行，章节应使用 feature detect、说明性代码或注释说明，而不是写成默认执行路径。
