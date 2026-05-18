# TypeScript 基础补充

本文补充 40 章教程中反复出现的基础边界。

读者可以在 ch01 之后通读一遍，也可以在遇到相关章节时回看。

## 1. 本项目的本地基线

- Node.js：`24.15.0`
- npm：`11.14.1`
- TypeScript：`6.0.3`
- `target`：`ES2023`
- `module`：`NodeNext`
- `moduleResolution`：`NodeNext`
- `lib`：`["ES2023", "DOM"]`
- `types`：`["node"]`
- 包类型：`"type": "module"`

这些设置决定了教程里的可运行代码边界。

如果你记忆中的 TypeScript 或 Node 行为与本地验证冲突，以本项目为准。

## 2. 验证版本的推荐命令

```bat
node --version
npm --version
npm run build
npx --no-install tsc --version
npx --no-install tsc --all
```

`npx --no-install` 的含义是只使用本项目已经安装的依赖。

它不会临时联网解析一个不受 lockfile 控制的版本。

本教程默认不使用未锁定的临时执行器作为验收依据。

## 3. TS、JS、Node、Web API 的分层

TypeScript 是类型系统和编译工具。

JavaScript 是编译后真正运行的语言。

Node.js 是服务器端运行时和标准库集合。

Web API 是浏览器和部分现代运行时提供的接口集合。

本项目在 Node 环境中运行，但 `lib` 同时包含 `DOM`。

这意味着 TypeScript 能看见一些 Web API 类型。

能看见类型不等于当前运行路径一定适合使用该 API。

例如 `fetch` 在现代 Node 中内置，本教程可以用它访问本地 server。

例如 DOM 节点类型主要用于前端边界说明，不代表 Node 里有真实浏览器 DOM。

类型声明描述的是“可被检查的形状”。

运行时对象必须由 Node、浏览器或你的代码真实提供。

## 4. C++ 背景下的三层类比

`.ts` 文件像带额外类型标注的源代码。

`tsc` 像项目级编译检查工具。

`.js` 输出才是 Node 真正执行的文件。

`.d.ts` 类似头文件声明，但没有实现。

`package.json` 类似项目元数据和命令入口。

`package-lock.json` 是可复现依赖快照。

Node 标准库类似平台库。

Web API 类似另一组平台接口。

TypeScript 不负责文件系统、网络、线程或进程调度的实现。

## 5. 类型擦除

TypeScript 的类型在编译后会被擦除。

`interface` 不会出现在运行时。

`type` 别名不会出现在运行时。

泛型参数不会出现在运行时。

访问修饰符 `private` 和 `protected` 主要是 TS 类型层约束。

类型断言 `as T` 不会验证值是否真的符合 T。

因此外部输入必须做 runtime validation。

外部输入包括 JSON、环境变量、CLI 参数、HTTP body、文件内容和数据库记录。

推荐边界写法是：先按 `unknown` 接收，再用 type guard 或 validator 收窄。

`any` 会绕过类型检查，应该只用于迁移边界或不得已的第三方接口封装。

`unknown` 表示“我还不知道”，会强迫你先检查再使用。

## 6. 值、绑定和不可变性

`const` 只限制绑定不可重新赋值。

`const user = { name: "A" }` 仍然允许修改 `user.name`。

`readonly` 是 TS 类型层约束。

`Object.freeze` 是运行时浅冻结。

深度不可变需要递归冻结或通过数据建模避免修改。

C++ 的 `const` 可以深入影响成员访问。

JS/TS 的不可变语义需要分清绑定、属性、类型和运行时对象。

## 7. number、bigint 和精度

JS `number` 使用 IEEE 754 双精度浮点。

它同时承担整数和小数角色。

大整数超过安全范围后会出现精度问题。

`Number.MAX_SAFE_INTEGER` 是常见边界。

`bigint` 用于任意精度整数，但不能和 `number` 直接混算。

货币金额通常不建议直接用浮点小数表达。

常见做法是使用整数分单位，或使用专门 decimal 库。

## 8. null、undefined 和可选字段

`undefined` 常表示缺失或未赋值。

`null` 常表示有意设置为空。

`strictNullChecks` 开启后，两者不会随意赋给普通类型。

`exactOptionalPropertyTypes` 开启后，可选属性的语义更严格。

`name?: string` 表示属性可以不存在。

它不等价于随意写入 `name: undefined`。

空值合并 `??` 只在 `null` 或 `undefined` 时使用默认值。

逻辑或 `||` 会把空字符串、0、false 也当成需要默认值。

## 9. 对象是引用语义

对象、数组、函数、Map、Set 都是引用类型值。

变量保存的是对对象的引用。

赋值不会自动深拷贝对象。

展开运算符 `{ ...obj }` 是浅拷贝。

`Object.assign` 也是浅拷贝。

嵌套对象仍可能共享。

这和 C++ 中结构体值复制的直觉不同。

很多状态 bug 来自无意共享引用。

## 10. 结构化类型

TypeScript 默认采用结构化类型系统。

如果两个对象形状兼容，它们通常就能赋值。

类名或接口名不是默认的兼容条件。

这让 JS 生态中的对象组合更自然。

代价是不同业务 ID 都是 string 时容易误传。

可以用 branded type 模拟名义类型。

例如把 `UserId` 和 `OrderId` 都基于 string，但加不同品牌字段。

品牌字段只服务类型系统，运行时仍是 string。

## 11. interface 与 type

`interface` 适合描述对象形状和可扩展公共 API。

`interface` 支持声明合并。

`type` 适合联合、交叉、条件类型和工具类型组合。

团队内部应保持一致的选择规则。

不要为了形式统一牺牲可读性。

对象模型简单时，两者都可以。

涉及联合类型时，通常必须使用 `type`。

## 12. 泛型不是 C++ 模板

TS 泛型在编译后擦除。

它不会按类型参数生成多份运行时代码。

它没有 C++ 模板特化机制。

泛型约束只保证编译期可以安全访问某些成员。

如果运行时需要根据类型做分支，必须传入真实值、tag 或构造函数。

例如 `function parse<T>()` 无法在运行时知道 T 是什么。

正确做法通常是传入 validator 或 schema。

## 13. 类型断言和 satisfies

`as T` 是断言，不是转换。

它告诉编译器把某个值当成 T。

如果断言错误，运行时不会自动报错。

`satisfies` 用来检查一个表达式符合某个类型，同时尽量保留表达式自身的字面量信息。

配置对象常适合用 `satisfies`。

DTO 转换和外部输入不应只靠 `as`。

类型边界越靠近外部世界，越需要运行时检查。

## 14. Node ESM 基础

本项目使用 ESM。

`package.json` 中 `"type": "module"` 表示 `.js` 按 ESM 解释。

`tsconfig.json` 中 `module: "NodeNext"` 配合 Node 的 ESM 解析规则。

`moduleResolution: "NodeNext"` 让 TypeScript 按 NodeNext 规则检查导入。

在 TS 源码中，相对导入写 `.js` 后缀。

原因是编译后运行的是 dist 中的 `.js` 文件。

Node 运行时不会去寻找 `.ts` 源码。

因此源码中写 `./shared/chapter.js` 是正确做法。

这不是拼写错误。

## 15. import type

`import type` 表示只导入类型。

在 `verbatimModuleSyntax` 下，类型导入和值导入要更清楚。

如果一个导入只用于类型，应写成 `import type`。

这样可以避免运行时加载不存在的值。

也能让读者清楚看到哪些依赖影响运行时。

类型和值同名时，更要显式区分。

## 16. CommonJS 与 ESM 互操作

Node 历史上长期使用 CommonJS。

现代 TS/Node 项目越来越多使用 ESM。

CJS 使用 `require` 和 `module.exports`。

ESM 使用 `import` 和 `export`。

互操作存在默认导入、命名导入和动态 import 的差异。

本教程只在必要时解释互操作边界。

章节默认使用 ESM。

不要把网上的 CJS 片段直接粘进本项目。

## 17. JSON import attributes

现代 ESM 中 JSON import 使用 import attributes。

形式是 `with { type: "json" }`。

旧的 import assertions 写法 `assert { type: "json" }` 已不作为本教程默认写法。

是否能直接运行还取决于当前 Node 和 TypeScript 配置。

如果某 API 在当前基线不可直接稳定运行，章节只能写说明性示例或 feature detect。

## 18. DOM 类型与 Node 运行时

本项目 `lib` 包含 `DOM`。

这让 TypeScript 能识别 `fetch`、`Request`、`Response` 等 Web API 类型。

Node 24 提供了许多 Web 兼容 API。

但 Node 不是浏览器。

Node 中没有真实页面、DOM tree 或用户点击事件。

前端章节只做最小边界说明。

如果需要浏览器工程，应另建前端构建工具链。

## 19. 异步模型总览

Promise 是异步结果的抽象。

`async` 函数总是返回 Promise。

`await` 暂停当前 async 函数，等待 Promise settle。

它不会阻塞整个 Node 进程。

Node 事件循环负责调度任务。

I/O 完成后，回调或 Promise continuation 会进入相应队列。

微任务通常在当前任务结束后尽快执行。

定时器、I/O、check 等阶段属于事件循环宏观调度。

C++ 线程模型不能直接套用到 Node。

异步 I/O 不等于 CPU 并行。

CPU 密集任务仍然会阻塞事件循环。

Worker Threads 才是 Node 中处理 CPU 并行的一种选择。

## 20. Promise 不是线程

创建 Promise 不代表创建线程。

Promise executor 会立即同步执行。

Promise 的 then/catch 回调进入微任务。

如果 executor 里写一个长 CPU 循环，它仍会阻塞当前线程。

文件系统 promise API 的底层可能使用 libuv 线程池。

HTTP server 的请求处理则是事件驱动。

不同异步 API 的底层机制不同。

统一的是它们通过回调、Promise 或事件把结果交还给 JS。

## 21. 定时器和可重复运行

章节中的 `setTimeout` 必须有确定退出路径。

`setInterval` 必须清理。

server 必须关闭。

watcher 必须关闭。

stream 必须处理结束和错误。

worker 必须 terminate 或自然退出。

示例输出应稳定。

不要让章节运行挂起。

## 22. 文件系统副作用

示例写文件时应使用临时目录或章节专属目录。

运行结束应清理。

如果故意保留输出，必须在说明中写清楚，并加入忽略规则。

相对路径基于 `process.cwd()`。

脚本所在目录可以通过 `import.meta.dirname` 理解。

不要把临时文件写到仓库外不可控位置。

删除目录前必须确认目标路径。

## 23. 网络副作用

网络章节默认只启动本地 server。

不依赖公网。

不要求用户机器有外部服务。

server 监听端口应避免冲突，必要时使用系统分配端口。

demo 结束前必须关闭 server。

错误响应不应泄漏内部栈。

请求体要限制大小。

外部 JSON 要做 runtime validation。

## 24. 环境变量和配置

环境变量都是字符串或缺失。

不能因为 TypeScript 类型写成 number 就自动得到 number。

读取后需要解析。

解析失败要给出可读错误。

配置优先级应固定。

常见优先级是默认值、配置文件、环境变量、CLI 参数。

secret 不应写进源码。

secret 不应打印到普通日志。

## 25. 子进程边界

子进程示例必须跨平台。

不要依赖 shell 私有语法。

`exec` 适合简单命令并缓冲输出。

`spawn` 适合流式交互。

传参应使用参数数组，避免命令注入。

子进程错误要处理 exit code 和 signal。

教程示例不应运行破坏性命令。

## 26. Buffer 与文本编码

Node Buffer 是字节数据容器。

字符串需要编码才能变成字节。

默认常见编码是 UTF-8。

Base64 和 Hex 是字节的文本表示。

`Buffer.alloc` 会初始化。

`Buffer.allocUnsafe` 更快但可能包含旧内存内容。

读取二进制协议时必须显式指定字节序。

文本长度和字节长度不是一回事。

## 27. Date、Intl 和 Temporal

`Date` 表示时间点，但 API 有很多历史包袱。

月份从 0 开始。

默认格式化受本地时区和 locale 影响。

教程中的时间输出应使用固定样例时间。

`Intl.DateTimeFormat` 适合做国际化格式化。

`Temporal` 是更现代的日期时间 API 方向。

如果当前 lib 或 Node 默认运行路径不能稳定支持 Temporal，则只做 feature detect 或说明性代码。

不要把预览能力写成无条件可运行路径。

## 28. Decorators 边界

TypeScript 支持标准装饰器语法方向。

本项目默认不启用 `experimentalDecorators`。

因此不写 legacy 参数装饰器作为默认可运行示例。

装饰器章节需要明确当前 tsconfig 的边界。

如果使用说明性代码，应清楚标出它依赖哪些配置。

装饰器适合日志、计时、权限等横切关注点。

业务核心逻辑不要过度依赖隐式装饰器副作用。

## 29. using 与显式资源管理

现代 JS/TS 有显式资源管理相关能力。

它适合表达资源离开作用域时自动释放。

是否能作为本教程默认可运行路径，取决于当前 Node、TypeScript 和 lib 设置。

若本地基线不支持某能力，就只作为说明性内容。

资源清理的稳定做法仍包括 `try/finally`、`pipeline`、server close、watcher close。

## 30. TS 6.0 工程注意点

本项目显式设置 `"rootDir": "."`。

本项目显式设置 `"types": ["node"]`。

项目编译使用 `tsc -p tsconfig.json`，通过 `npm run build` 间接执行。

不要用 `tsc file.ts` 作为本项目的常规检查方式。

旧的 moduleResolution 和过时 target 不作为本教程默认选项。

`package.json` 使用 `typescript: "~6.0.3"`。

lockfile 锁定精确依赖版本。

升级依赖前应先看变更，再运行 build、test 和章节入口。

## 31. 练习和测试的边界

练习文件同样应可重复运行。

测试不应依赖公网。

测试不应依赖当前时间的具体值。

测试使用临时目录时要清理。

异步测试必须等待 Promise settle。

server、watcher、stream、worker 都不能遗留。

`node:test` 是本项目默认测试入口。

断言来自 `node:assert`。

类型层测试可使用 `@ts-expect-error` 锁定错误应当发生。

## 32. 读代码时的常用判断

先判断这是类型问题还是运行时问题。

再判断值来自可信内部还是外部输入。

再判断是否有资源需要关闭。

再判断输出是否稳定。

最后判断代码是否适合线性读者理解。

教学代码不追求最短。

教学代码优先把边界讲清楚。

真实项目代码也应在关键边界保留清楚的命名和错误信息。

## 33. 常见误区速查

`as T` 不是运行时转换。

`interface` 不是运行时对象。

`private` 不等于运行时私有。

Promise 不等于线程。

async 不等于并行。

`const` 不等于深度不可变。

`readonly` 不等于运行时冻结。

`.d.ts` 不包含实现。

看到 DOM 类型不代表 Node 中有浏览器 DOM。

看到 `number` 不代表它是任意精度整数。

看到 `Map` 不代表键按值比较，普通对象键仍按引用比较。

看到 `fetch` 类型不代表可以依赖公网。

## 34. 推荐复盘问题

TypeScript 编译后还剩下哪些东西。

一个对象的类型和它的运行时原型是否一定相关。

为什么外部 JSON 不能直接断言成业务类型。

为什么 NodeNext 项目源码 import 写 `.js`。

为什么一个 async 函数抛错会变成 rejected Promise。

为什么 CPU 长任务会让 HTTP server 卡住。

为什么章节示例要清理临时目录和 server。

为什么 lockfile 是教程可复现的一部分。

为什么版本敏感 API 必须按本地基线验证。
