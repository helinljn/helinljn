# TypeScript 6.0 Learning

这是一套面向 C++ 背景开发者的 TypeScript 6.0 系统入门工程。重点不是把 TS 当成“带类型的完整标准库语言”，而是明确区分三层：

- JavaScript：真正的运行时语义，例如对象、闭包、Promise、事件循环。
- TypeScript：类型系统、编译期检查和工程配置。
- Node/Web API：文件、网络、进程、`fetch` 等平台能力。

## 环境准备

需要 Node.js `24.15.0`。本项目使用 npm `11.14.1`，并把 TypeScript 锁定在 6.0 补丁线。

可先确认版本：

```bat
node --version
npm --version
```

```bat
npm install
```

## 常用命令

```bat
npm run build
npm run start
npm run chapter -- 1
npm test
```

## 学习顺序

推荐按 [TypeScript学习路线图.md](./TypeScript学习路线图.md) 学习：

1. `01_基础篇`：环境、变量、基本类型、控制流
2. `02_JS运行时语义篇`：对象、引用、闭包、原型
3. `03_类型系统篇`：联合类型、收窄、接口、结构化类型
4. `04_函数与泛型篇`：函数签名、重载、泛型
5. `05_面向对象篇`：class、继承、访问控制
6. `06_模块与工程篇`：ESM、NodeNext、tsconfig、声明文件
7. `07_高级类型篇`：keyof、映射类型、条件类型
8. `08_Node运行时与API篇`：文件、路径、Buffer、JSON
9. `09_异步与网络篇`：Promise、async/await、本地 HTTP
10. `10_TS6工程约束篇`：TS 6.0 版本边界
11. `11_项目实战`：文件批处理 CLI、日志分析 CLI

完成基础章节后阅读 [TypeScript基础补充.md](./TypeScript基础补充.md)，再做 `练习题与答案`。

## 目录说明

```text
.
├── package.json
├── tsconfig.json
├── main.ts
├── chapterRegistry.ts
├── shared/
├── 01_基础篇/
├── 02_JS运行时语义篇/
├── 03_类型系统篇/
├── 04_函数与泛型篇/
├── 05_面向对象篇/
├── 06_模块与工程篇/
├── 07_高级类型篇/
├── 08_Node运行时与API篇/
├── 09_异步与网络篇/
├── 10_TS6工程约束篇/
├── 11_项目实战/
├── 练习题与答案/
└── tests/
```
