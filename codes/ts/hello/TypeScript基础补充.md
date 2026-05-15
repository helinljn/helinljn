# TypeScript 基础补充

## TS、JS、Node 的关系

TypeScript 提供类型系统和编译器。编译后运行的是 JavaScript。网络、文件、进程、Buffer、HTTP server 等能力来自 Node.js 或 Web API。

类比 C++：

- `.ts` 源码：带类型标注的源码。
- `tsc`：类型检查和转译工具。
- `.d.ts`：类似头文件声明，但没有实现。
- Node.js：实际运行时和平台库。

## 为什么要写 `.js` 后缀 import

本教程使用 Node ESM 和 `moduleResolution: "NodeNext"`。在 TS 源码中写：

```ts
import { hello } from "./hello.js";
```

虽然源文件是 `hello.ts`，但编译后运行时会查找 `hello.js`。这是现代 Node ESM 项目的常见规则。

## 为什么需要 `@types/node`

TypeScript 自带的是语言和 Web 标准库类型声明，不自带 Node 标准库实现。`@types/node` 提供 `node:fs`、`node:http`、`process` 等 Node API 的类型。

实现仍然来自本机 Node.js。

## TS 6.0 版本限定

`package.json` 使用 `typescript@~6.0.3`，只接受 6.0 补丁版本。这样教程不会因为后续 6.1/7.0 改动而改变默认行为。

## 常见坑

- 类型断言 `as T` 不会做运行时检查。
- `interface` 编译后不存在。
- `private` 是 TS 类型层约束，`#field` 才是 JS 运行时私有字段。
- `Promise` 不等于线程。
- Node 的 `http`、`fs`、`net` 不是 TS 标准库。
