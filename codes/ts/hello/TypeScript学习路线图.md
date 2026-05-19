# TypeScript 6.0 学习路线图

本文面向有 C++ 经验、刚开始学习 TypeScript/JavaScript/Node.js 的读者。

本路线图对应当前仓库中的 40 章最终目录。

阅读顺序固定从 ch01 到 ch40。

不要跳过 JS 运行时、异步模型和 Node API，因为这些内容决定了 TypeScript 程序在真实机器上的行为。

## 如何使用这份路线图

1. 先运行 `npm run build`，确认本地 TypeScript 工程可以编译。
2. 每读完一章，运行该章编译后的 dist 入口，观察输出。
3. 每个阶段结束后，回到本路线图核对学习目标和建议练习。
4. 遇到类型和运行时行为不一致时，优先判断“这是 TS 编译期规则，还是 JS/Node 运行时规则”。
5. 不使用未锁定的临时执行器作为验收依据，默认采用 build 后运行 dist 文件。

## 11 个阶段总览

| 阶段 | 目录 | 章节 | 学习目标 |
|------|------|------|----------|
| 1 | `01_基础篇/` | ch01-ch05 | 建立环境、语法、值类型、控制流和集合基础 |
| 2 | `02_JS运行时篇/` | ch06-ch08 | 理解对象引用、闭包、原型和 `this` |
| 3 | `03_类型系统篇/` | ch09-ch13 | 掌握日常类型建模、收窄、泛型和高级类型 |
| 4 | `04_函数篇/` | ch14-ch15 | 掌握函数签名、重载和函数式组合 |
| 5 | `05_面向对象篇/` | ch16-ch18 | 掌握类、继承、结构化多态和错误处理 |
| 6 | `06_模块与工程篇/` | ch19-ch21 | 理解 ESM、NodeNext、声明文件和装饰器边界 |
| 7 | `07_迭代器与生成器篇/` | ch22 | 理解同步/异步迭代和懒加载 |
| 8 | `08_异步编程篇/` | ch23-ch25 | 集中理解 Promise、async/await 和事件循环 |
| 9 | `09_Node_API篇/` | ch26-ch33 | 掌握常用 Node 标准库和运行时边界 |
| 10 | `10_工程实践篇/` | ch34-ch37 | 建立日志、测试、性能和 TS6 迁移意识 |
| 11 | `11_项目实战/` | ch38-ch40 | 把章节知识组合成可维护 CLI 项目 |

## 学习节奏建议

- 第 1 周：完成 ch01-ch08，重点把 JS 值语义和引用语义讲清楚。
- 第 2 周：完成 ch09-ch18，重点练习类型边界和 API 设计。
- 第 3 周：完成 ch19-ch25，重点理解模块系统和异步并发模型。
- 第 4 周：完成 ch26-ch37，重点建立 Node 后端工程能力。
- 第 5 周：完成 ch38-ch40 和练习，重点做端到端复盘。

## 章节详细路线

### 第一阶段：基础篇

本阶段先解决“如何编译和运行 TS 程序”以及“JS 基础值如何工作”。

读者要避免把 TypeScript 当成带标准库的 C++。

TypeScript 提供类型检查，JavaScript 和 Node.js 决定运行时行为。

#### ch01 入门与环境

- 所属目录：`01_基础篇/chapter01_入门与环境.ts`
- 学习目标：
- 能解释 Node.js、npm、TypeScript 编译器和本项目脚本的职责。
- 能用 `npm run build` 编译项目，并理解输出目录 `dist/`。
- 能读懂最小 `tsconfig.json` 中的 `target`、`module`、`rootDir`、`outDir`。
- 核心知识点：
- Node.js/npm 安装验证。
- `package.json` scripts 工作流。
- `tsc --version`、`tsc -p tsconfig.json` 和第一个 TS 程序。
- VS Code 配置和线性阅读方式。
- C++ 对照：
- `tsc` 类似编译检查入口，但不会生成本地机器码。
- `package.json` scripts 更像项目内约定的命令表，不等同于 CMake。
- 建议练习：
- 修改一个输出文本，重新执行 build 后运行 dist 文件。
- 在 `package.json` 中找出 build、chapter、test 的实际命令。
- 写下 TS、JS、Node 三者各自负责什么。
- 验收提示：
- 能清楚说明为什么源码 import 使用 `.js` 后缀。

#### ch02 变量与基本类型

- 所属目录：`01_基础篇/chapter02_变量与基本类型.ts`
- 学习目标：
- 能区分 `let`、`const`、`var` 的作用域和初始化规则。
- 能解释 `number`、`string`、`boolean`、`bigint`、`symbol` 的运行时表现。
- 能理解 `null` 与 `undefined` 的差异。
- 核心知识点：
- 块级作用域和暂时性死区。
- IEEE 754 `number` 的精度边界。
- 模板字面量和字符串插值。
- 类型标注、类型推断、`typeof` 和字面量类型。
- C++ 对照：
- `const` 约束绑定，不等于 C++ 的深度 const。
- JS `number` 不区分 `int`、`long`、`double`。
- 建议练习：
- 写一个变量表，标出每个变量的推断类型。
- 比较 `const obj = {}` 与对象属性修改的关系。
- 用 `typeof` 输出几个基础值的运行时类型。
- 验收提示：
- 能说出哪些信息会在编译后被擦除，哪些值仍在运行时存在。

#### ch03 控制流程

- 所属目录：`01_基础篇/chapter03_控制流程.ts`
- 学习目标：
- 能使用 if、switch、循环和短路求值表达业务分支。
- 能区分 `for-of` 和 `for-in` 的适用场景。
- 能理解 `??` 与 `||` 在默认值处理上的差异。
- 核心知识点：
- if-else、三元表达式、switch。
- for、while、do-while、break、continue。
- 可选链 `?.` 和空值合并 `??`。
- switch 穷尽性检查的基础思路。
- C++ 对照：
- JS 没有常规 `goto`。
- switch fall-through 仍需小心，逻辑上要显式防止误穿透。
- 建议练习：
- 写一个状态机 switch，并为未知状态保留错误分支。
- 对数组分别使用 `for-of` 和 `for-in`，观察输出差异。
- 用 `??` 改写一个会被空字符串影响的默认值逻辑。
- 验收提示：
- 能说明 truthy/falsy 和类型收窄之间的边界。

#### ch04 字符串处理

- 所属目录：`01_基础篇/chapter04_字符串处理.ts`
- 学习目标：
- 能完成常见字符串切片、查找、替换和格式化。
- 能意识到 Unicode 字符和 `.length` 的常见坑。
- 能读懂模板字面量和标签函数的基本用法。
- 核心知识点：
- `slice`、`substring`、`split`、`replace`、`trim`。
- `includes`、`startsWith`、`endsWith`、`indexOf`。
- `padStart`、`padEnd` 和简单格式化。
- `String.raw` 与标签函数。
- C++ 对照：
- JS `String` 是不可变值，方法通常返回新字符串。
- Unicode 处理不能简单等同于 `std::string::size()`。
- 建议练习：
- 写一个日志行清洗函数，去掉多余空白并提取字段。
- 尝试包含 emoji 的字符串，观察 `.length` 与用户感知字符数差异。
- 用模板字面量生成一段多行配置文本。
- 验收提示：
- 能解释字符串 API 返回新值，而不是原地修改。

#### ch05 数组与集合类型

- 所属目录：`01_基础篇/chapter05_数组与集合类型.ts`
- 学习目标：
- 能用数组、Map、Set 表达常见集合模型。
- 能区分可变操作和返回新数组的操作。
- 能理解 `readonly` 数组和 `as const` 的类型约束。
- 核心知识点：
- `push`、`pop`、`shift`、`unshift`、`splice`、`slice`。
- `map`、`filter`、`reduce`。
- `find`、`some`、`every`、`includes`、`flatMap`。
- `Map`、`Set`、`WeakMap`、`WeakSet`。
- C++ 对照：
- JS Array 更像动态数组加对象特性，不等同于 `std::vector`。
- `Map` 保留插入顺序，键可以是对象引用。
- 建议练习：
- 用 `Map` 统计一组日志级别出现次数。
- 用 `filter` 和 `map` 写一个数据清洗流水线。
- 把一个可变排序示例改成不修改原数组的版本。
- 验收提示：
- 能说明 `sort` 会修改原数组，并知道如何避免误改。

### 第二阶段：JS 运行时篇

本阶段解释 TypeScript 代码编译后真正运行的 JavaScript 语义。

这部分是 C++ 开发者最容易误判的基础。

#### ch06 对象与引用

- 所属目录：`02_JS运行时篇/chapter06_对象与引用.ts`
- 学习目标：
- 能解释对象字面量、属性访问和引用共享。
- 能区分浅拷贝、冻结和密封对象的边界。
- 能使用 `Object.keys`、`values`、`entries` 和 `fromEntries`。
- 核心知识点：
- 对象创建和动态属性。
- 展开运算符与 `Object.assign`。
- `Object.freeze`、`Object.seal`。
- 可选链和逻辑赋值运算符。
- C++ 对照：
- JS 对象变量保存引用，不是结构体值复制。
- `Object.freeze` 不是 C++ 类型系统里的深度 const。
- 建议练习：
- 写一个对象浅拷贝示例，观察嵌套对象是否共享。
- 用 `Object.entries` 把配置对象转换成键值对列表。
- 尝试冻结对象后修改属性，观察严格模式下行为。
- 验收提示：
- 能判断一个 bug 是由共享引用还是类型声明错误造成的。

#### ch07 闭包与作用域

- 所属目录：`02_JS运行时篇/chapter07_闭包与作用域.ts`
- 学习目标：
- 能解释词法作用域和闭包捕获。
- 能写出 `once`、计数器和模块模式。
- 能避免循环中闭包捕获的常见陷阱。
- 核心知识点：
- `let`/`const` 块级作用域与 `var` 函数级作用域。
- 闭包保存外层环境。
- IIFE 的历史用途。
- 闭包造成的内存保留。
- C++ 对照：
- C++ lambda 捕获是显式语法。
- JS 闭包按词法环境自动保留可访问变量。
- 建议练习：
- 实现一个 `once(fn)`，保证函数只执行一次。
- 写一个计数器工厂，返回 increment/reset 两个函数。
- 把 `var` 循环闭包示例改成 `let`。
- 验收提示：
- 能解释为什么函数返回后局部变量仍可能被闭包持有。

#### ch08 原型与 this

- 所属目录：`02_JS运行时篇/chapter08_原型与this.ts`
- 学习目标：
- 能解释原型链查找和 `prototype` 的作用。
- 能判断 `this` 的四种绑定规则。
- 能理解箭头函数为什么没有自己的 `this`。
- 核心知识点：
- `[[Prototype]]`、`__proto__`、`prototype`。
- `Object.create`。
- 默认绑定、隐式绑定、显式绑定、new 绑定。
- `call`、`apply`、`bind`。
- C++ 对照：
- 原型链不是 vtable。
- JS 方法调用中的 `this` 取决于调用方式，不是声明所在类。
- 建议练习：
- 用 `Object.create` 构造一个继承示例。
- 把一个丢失 `this` 的回调用 `bind` 修复。
- 比较普通函数和箭头函数在对象方法中的差异。
- 验收提示：
- 能根据调用表达式判断 `this` 指向。

### 第三阶段：类型系统篇

本阶段从日常类型标注进入 TypeScript 的核心价值。

重点不是堆砌类型技巧，而是把边界写清楚。

#### ch09 类型标注与推断

- 所属目录：`03_类型系统篇/chapter09_类型标注与推断.ts`
- 学习目标：
- 能选择何时依赖推断，何时写显式类型。
- 能理解 `as const`、`satisfies` 和类型断言的边界。
- 能使用 `typeof` 类型操作符复用已有值的形状。
- 核心知识点：
- 变量、函数参数、返回值和对象属性标注。
- 字面量推断与 widening。
- `satisfies` 保留字面量信息。
- 类型断言不做运行时验证。
- C++ 对照：
- TS 推断类似 `auto` 的便利性，但运行时没有类型元信息。
- `as` 更像告诉编译器相信你，不等同于安全转换。
- 建议练习：
- 给一个配置对象加 `satisfies`，保留具体键名。
- 把过度标注的变量改成依赖推断。
- 写一个错误类型断言示例，并解释运行时风险。
- 验收提示：
- 能说出“类型系统相信什么”和“运行时实际检查什么”。

#### ch10 联合类型与收窄

- 所属目录：`03_类型系统篇/chapter10_联合类型与收窄.ts`
- 学习目标：
- 能用联合类型表达有限状态或多种输入。
- 能通过 `typeof`、`instanceof`、`in` 和相等比较收窄。
- 能写判别联合和穷尽性检查。
- 核心知识点：
- `A | B` 的读法和访问限制。
- 类型谓词。
- `never` 表示不可达分支。
- `unknown` 与 `any` 的差异。
- C++ 对照：
- 判别联合接近 `std::variant` 加 visitor 的思路。
- `any` 更像关闭检查，不应作为常规边界类型。
- 建议练习：
- 写一个 `Result` 联合，区分成功和失败。
- 为外部 JSON 写一个 type guard。
- 在 switch 默认分支使用 `never` 验证穷尽性。
- 验收提示：
- 能解释为什么联合类型访问成员前必须先收窄。

#### ch11 接口与类型别名

- 所属目录：`03_类型系统篇/chapter11_接口与类型别名.ts`
- 学习目标：
- 能选择 `interface` 或 `type` 表达对象、联合和函数类型。
- 能理解结构化类型、额外属性检查和赋值兼容。
- 能用 branded type 避免业务 ID 混用。
- 核心知识点：
- interface 声明合并和 extends。
- type 支持联合、交叉和复杂组合。
- 索引签名、函数类型接口、readonly。
- 函数参数逆变和方法双变的兼容性背景。
- C++ 对照：
- TS 默认结构化类型，不依赖类名或接口名的名义匹配。
- branded type 是模拟名义类型的工程技巧。
- 建议练习：
- 分别用 interface 和 type 建模同一个用户对象。
- 写 `UserId` 和 `OrderId` 的 branded type。
- 构造一个额外属性检查示例，比较字面量和变量赋值。
- 验收提示：
- 能解释“形状兼容”为什么既方便也可能带来误传。

#### ch12 泛型

- 所属目录：`03_类型系统篇/chapter12_泛型.ts`
- 学习目标：
- 能写泛型函数、泛型接口和泛型类型别名。
- 能使用 `extends` 约束和默认类型参数。
- 能理解 TS 泛型与 C++ 模板的根本差异。
- 核心知识点：
- 泛型参数推导。
- `keyof` 约束。
- `typeof` 与泛型组合。
- 显式类型参数和推导失败时的处理。
- C++ 对照：
- TS 泛型编译后擦除，不生成每种类型的专门代码。
- TS 没有 C++ 模板特化的运行时/编译期模型。
- 建议练习：
- 写一个 `pick(obj, key)`，让返回值类型跟 key 联动。
- 给分页响应定义泛型接口。
- 对比显式传入泛型参数和自动推导的差异。
- 验收提示：
- 能解释泛型约束保护的是编译期访问权限。

#### ch13 高级类型工具

- 所属目录：`03_类型系统篇/chapter13_高级类型工具.ts`
- 学习目标：
- 能使用常见内置工具类型改造对象类型。
- 能读懂映射类型、条件类型和 `infer`。
- 能识别高级类型的维护成本。
- 核心知识点：
- `Pick`、`Omit`、`Partial`、`Required`、`Readonly`。
- `Record`、`Exclude`、`Extract`、`NonNullable`。
- `ReturnType`、`Parameters`、`Awaited`。
- 模板字面量类型、递归类型和 `asserts`。
- C++ 对照：
- 高级类型接近模板元编程的表达力，但只服务于 TS 类型检查。
- 过度复杂的类型会降低团队可读性。
- 建议练习：
- 用 `Pick` 和 `Omit` 定义 DTO 类型。
- 写一个条件类型提取 Promise 内部值。
- 用模板字面量类型约束事件名。
- 验收提示：
- 能判断高级类型是否真的降低重复，而不是制造谜题。

### 第四阶段：函数篇

本阶段把函数作为 JS/TS 的一等公民来学习。

函数既是业务逻辑单元，也是闭包和异步流程的基础。

#### ch14 函数详解

- 所属目录：`04_函数篇/chapter14_函数详解.ts`
- 学习目标：
- 能书写函数声明、函数表达式和箭头函数类型。
- 能处理可选参数、默认参数、剩余参数和解构参数。
- 能理解函数重载、`this` 参数和 `void`。
- 核心知识点：
- 函数签名和返回值推断。
- 参数默认值与调用方传参。
- overload signature 与 implementation signature。
- `void` 与 `undefined` 的区别。
- C++ 对照：
- TS 重载只有一个实现体。
- 默认参数语义受 JS 调用方式影响。
- 建议练习：
- 给一个解析函数写两个重载签名。
- 用剩余参数实现求和函数。
- 给普通函数添加 `this` 参数标注，限制调用方式。
- 验收提示：
- 能解释 TS 函数类型检查和 JS 实际调用参数数量的差异。

#### ch15 函数式编程模式

- 所属目录：`04_函数篇/chapter15_函数式编程模式.ts`
- 学习目标：
- 能把函数作为值传递、返回和组合。
- 能手写简化版 map/filter/reduce 理解集合转换。
- 能使用 compose、pipe、curry、memoize、once 等模式。
- 核心知识点：
- 一等函数。
- 高阶函数。
- 函数组合和柯里化。
- debounce/throttle 的异步边界。
- C++ 对照：
- `std::function` 可保存可调用对象，但 JS 函数天然携带闭包环境。
- 高阶函数在 JS 标准库中更常见。
- 建议练习：
- 手写 `pipe`，组合三个字符串转换函数。
- 实现 `memoize`，缓存纯函数结果。
- 写 `debounce` 时保证定时器最终清理。
- 验收提示：
- 能判断函数式写法是否改善可读性，而不是只追求技巧。

### 第五阶段：面向对象篇

本阶段学习 TS 中 class 的类型层和 JS 运行时层。

类语法并不会把 JS 变成 C++ 的对象模型。

#### ch16 类与对象

- 所属目录：`05_面向对象篇/chapter16_类与对象.ts`
- 学习目标：
- 能定义 class、constructor、实例成员和静态成员。
- 能区分 TS `private` 与 JS `#private`。
- 能使用 getter、setter、readonly、参数属性和 implements。
- 核心知识点：
- 类字段和方法。
- 访问修饰符。
- 运行时私有字段。
- 接口实现和结构化检查。
- C++ 对照：
- TS `private` 主要是编译期约束。
- `#field` 才是 JS 运行时私有字段。
- 建议练习：
- 写一个带 getter/setter 的配置类。
- 对比 `private name` 和 `#name` 的输出边界。
- 用接口约束类必须实现的公开方法。
- 验收提示：
- 能解释哪些访问限制会出现在运行时。

#### ch17 继承与多态

- 所属目录：`05_面向对象篇/chapter17_继承与多态.ts`
- 学习目标：
- 能使用 extends、super、override 和抽象类。
- 能理解结构化类型多态与接口实现。
- 能读懂 Mixin 模式。
- 核心知识点：
- 单继承。
- 抽象类和抽象方法。
- 多接口 implements。
- mixin 组合。
- C++ 对照：
- TS/JS class 是单继承。
- 接口和结构化类型替代了很多名义继承需求。
- 建议练习：
- 定义抽象 `Shape`，实现 `Circle` 和 `Rectangle`。
- 用接口表达可序列化对象。
- 写一个简单 mixin 给类添加日志能力。
- 验收提示：
- 能判断何时使用组合而不是继承。

#### ch18 异常处理

- 所属目录：`05_面向对象篇/chapter18_异常处理.ts`
- 学习目标：
- 能写 `try`、`catch`、`finally` 和自定义错误类。
- 能处理 `catch` 中的 `unknown`。
- 能区分 throw、Result 风格和异步异常边界。
- 核心知识点：
- Error 层级和 `Error.cause`。
- 自定义错误码。
- 可恢复与不可恢复错误。
- CLI exit code、HTTP handler 和入口层统一捕获。
- C++ 对照：
- JS 异常可以抛任意值，因此 catch 必须先做类型判断。
- Promise 异常需要在异步边界收敛。
- 建议练习：
- 写一个解析函数，返回 Result 而不是直接 throw。
- 给 CLI 错误设计 exit code。
- 用 `try/catch` 捕获 awaited Promise 的失败。
- 验收提示：
- 能说明同步 throw 和 rejected Promise 的传播差异。

### 第六阶段：模块与工程篇

本阶段进入现代 Node/TypeScript 工程结构。

目标是能解释一个包如何被编译、导入、发布和维护。

#### ch19 模块系统

- 所属目录：`06_模块与工程篇/chapter19_模块系统.ts`
- 学习目标：
- 能使用 ESM 的命名导出、默认导出和 type-only import。
- 能理解 NodeNext 下源码相对导入写 `.js` 后缀的原因。
- 能说明 CJS/ESM 互操作边界。
- 核心知识点：
- `import`、`export`、`import type`。
- `import.meta`。
- `package.json` 中 `"type": "module"`。
- `exports`、`imports` 和 JSON import attributes。
- C++ 对照：
- `import` 不是文本包含，不等同于 `#include`。
- 模块加载有运行时解析规则。
- 建议练习：
- 把一个函数改成命名导出，再从另一文件导入。
- 解释为什么 TS 源码 import 不是 `.ts` 后缀。
- 查阅一个 package 的 `exports` 字段含义。
- 验收提示：
- 能判断一个模块错误来自类型解析还是 Node 运行时解析。

#### ch20 工程配置与声明文件

- 所属目录：`06_模块与工程篇/chapter20_工程配置与声明文件.ts`
- 学习目标：
- 能读懂 `tsconfig.json` 的核心选项。
- 能理解 `.d.ts`、`declare` 和 `@types` 的用途。
- 能设计库发布时的类型产物边界。
- 核心知识点：
- `declaration`、`declarationMap`、`emitDeclarationOnly`。
- package `types` 字段。
- 只暴露公共 API。
- `npm ci`、`package-lock.json` 和 semver。
- C++ 对照：
- `.d.ts` 类似头文件声明，但不参与运行时。
- npm lockfile 类似可复现依赖快照。
- 建议练习：
- 找出本项目为什么显式写 `"types": ["node"]`。
- 为一个函数写最小 `.d.ts` 声明。
- 比较 `npm ci` 与 `npm install` 的用途。
- 验收提示：
- 能解释类型声明和 JS 实现不一致时会发生什么。

#### ch21 装饰器

- 所属目录：`06_模块与工程篇/chapter21_装饰器.ts`
- 学习目标：
- 能理解标准装饰器的用途和执行边界。
- 能区分类、方法、accessor 和字段装饰器。
- 能知道本项目不启用 legacy 参数装饰器示例。
- 核心知识点：
- 装饰器工厂。
- 日志、计时和权限等横切关注点。
- 执行顺序。
- 当前 tsconfig 不启用 `experimentalDecorators`。
- C++ 对照：
- 装饰器接近 AOP 的表达方式。
- 它不是 C++ 属性系统的直接等价物。
- 建议练习：
- 阅读一个标准装饰器示例，标出装饰目标。
- 写一个说明性计时装饰器草图。
- 判断 legacy 装饰器示例为什么不能直接用于本教程默认配置。
- 验收提示：
- 能明确当前教程采用的可运行装饰器边界。

### 第七阶段：迭代器与生成器篇

本阶段连接集合遍历、懒计算和异步数据流。

#### ch22 迭代器与生成器

- 所属目录：`07_迭代器与生成器篇/chapter22_迭代器与生成器.ts`
- 学习目标：
- 能解释 `Symbol.iterator` 和 `for-of` 的协议。
- 能使用 `function*`、`yield`、`yield*` 实现懒序列。
- 能理解异步生成器和 `for await...of`。
- 核心知识点：
- 自定义迭代器。
- `next()` 返回 `{ value, done }`。
- 懒加载分页。
- 异步 iterable 的消费和退出路径。
- C++ 对照：
- 生成器和 C++20 coroutine 有相似的暂停/恢复直觉。
- JS 迭代协议是对象协议，不是模板约定。
- 建议练习：
- 写一个生成固定范围数字的 generator。
- 用 `yield*` 组合两个序列。
- 写一个异步生成器模拟分页读取，并确保循环结束。
- 验收提示：
- 能解释同步迭代和异步迭代分别等待什么。

### 第八阶段：异步编程篇

本阶段是全教程最重要的思维转换。

C++ 线程模型不能直接套到 Node 事件循环上。

#### ch23 Promise 深入

- 所属目录：`08_异步编程篇/chapter23_Promise深入.ts`
- 学习目标：
- 能解释 Promise 状态机和 then/catch/finally 链式调用。
- 能选择 `all`、`allSettled`、`race`、`any`。
- 能理解微任务和 Promise.withResolvers 的边界。
- 核心知识点：
- pending、fulfilled、rejected。
- 链式返回值展开。
- promisify。
- 微任务队列。
- C++ 对照：
- Promise 不等于线程，也不保证并行执行 CPU 工作。
- 它更像异步结果的协议和调度入口。
- 建议练习：
- 用 `Promise.allSettled` 汇总多个任务结果。
- 把回调式 API 包装成 Promise。
- 观察同步日志和 Promise 回调的输出顺序。
- 验收提示：
- 能解释 Promise 回调为什么晚于当前调用栈执行。

#### ch24 async 与 await

- 所属目录：`08_异步编程篇/chapter24_async与await.ts`
- 学习目标：
- 能说明 async 函数总是返回 Promise。
- 能用 `await` 写出可读的异步控制流。
- 能区分串行 await 和并行启动后 await。
- 核心知识点：
- await 暂停当前 async 函数。
- `try/catch` 捕获 awaited Promise。
- 循环中 await。
- 顶层 await 和 AbortController。
- C++ 对照：
- `co_await` 和 `await` 都有暂停直觉，但运行时模型不同。
- Node 默认仍是事件循环驱动。
- 建议练习：
- 把 `.then()` 链改写为 async/await。
- 同时启动三个异步任务，再用 `Promise.all` 等待。
- 写一个支持 AbortController 的超时示例。
- 验收提示：
- 能判断代码是真的并发启动，还是串行等待。

#### ch25 事件循环与并发

- 所属目录：`08_异步编程篇/chapter25_事件循环与并发.ts`
- 学习目标：
- 能用自己的话解释 C++ 线程与 Node 事件循环差异。
- 能描述调用栈、任务队列和微任务队列。
- 能知道 Worker Threads 适合 CPU 密集任务。
- 核心知识点：
- 事件循环阶段。
- `setTimeout`、`setImmediate`、`process.nextTick`。
- 微任务优先级。
- 单线程优点和阻塞风险。
- C++ 对照：
- Node HTTP handler 不意味着每个请求一个线程。
- CPU 长任务会阻塞事件循环，需要拆分或交给 worker。
- 建议练习：
- 写一个稳定的输出顺序示例，标注每行来自哪个队列。
- 把一个 CPU 循环改成分片执行。
- 说明什么时候该考虑 Worker Threads。
- 验收提示：
- 能解释“异步 I/O”与“并行 CPU 计算”不是一回事。

### 第九阶段：Node API 篇

本阶段覆盖后端开发最常用的 Node 标准库。

所有示例都应默认本地、可重复、无公网依赖。

#### ch26 文件系统

- 所属目录：`09_Node_API篇/chapter26_文件系统.ts`
- 学习目标：
- 能使用 `node:fs/promises` 完成读写、目录、元数据和临时文件操作。
- 能知道相对路径基于 `process.cwd()`。
- 能处理文件监听和清理边界。
- 核心知识点：
- `readFile`、`writeFile`、`appendFile`。
- `mkdir`、`readdir`、`stat`、`rm`。
- `mkdtemp` 和临时目录。
- 文件 I/O 在线程池中执行，不阻塞事件循环主线程。
- C++ 对照：
- Node fs promise API 是异步接口，不等同于同步 `std::fstream`。
- 文件句柄和 watcher 都需要清理。
- 建议练习：
- 创建临时目录，写入文件，读取后删除。
- 统计一个目录下文件大小。
- 写一个文件监听说明性 demo，确保退出路径。
- 验收提示：
- 能保证示例反复运行后没有残留临时文件。

#### ch27 路径进程与环境

- 所属目录：`09_Node_API篇/chapter27_路径进程与环境.ts`
- 学习目标：
- 能使用 path 模块处理跨平台路径。
- 能读取 CLI 参数、环境变量和进程信息。
- 能理解 `cwd()` 与 `import.meta.dirname` 的差异。
- 核心知识点：
- `join`、`resolve`、`dirname`、`basename`、`parse`。
- `process.argv`、`process.env`、`platform`、`arch`。
- stdout、stderr、stdin。
- child_process 的 `exec` 与 `spawn` 基础。
- C++ 对照：
- `argv` 和 `getenv` 直觉仍适用，但路径分隔符必须交给 path 模块处理。
- 子进程示例要跨平台、确定性。
- 建议练习：
- 写一个解析 `--name value` 的小函数。
- 用环境变量覆盖默认配置。
- 解释为什么不能手写 `/` 拼路径。
- 验收提示：
- 能说明配置优先级：默认值、环境变量、CLI 参数、配置文件。

#### ch28 Buffer 与二进制

- 所属目录：`09_Node_API篇/chapter28_Buffer与二进制.ts`
- 学习目标：
- 能创建 Buffer 并在 UTF-8、Base64、Hex 间转换。
- 能读写整数并处理字节序。
- 能解析一个简单二进制协议。
- 核心知识点：
- `Buffer.from`、`Buffer.alloc`、`Buffer.allocUnsafe`。
- `readUInt8`、`readUInt16BE`、`writeUInt32BE`。
- TypedArray 与 Buffer 的关系。
- DataView 的灵活读写。
- C++ 对照：
- Buffer 接近字节数组视图，但仍受 JS 对象模型和边界检查影响。
- 字节序必须显式写清楚。
- 建议练习：
- 把字符串转成 hex，再还原。
- 构造一个消息头包含长度的 Buffer。
- 比较 BE 和 LE 读取同一组字节的结果。
- 验收提示：
- 能解释为什么 `allocUnsafe` 需要立即填充。

#### ch29 网络与 HTTP

- 所属目录：`09_Node_API篇/chapter29_网络与HTTP.ts`
- 学习目标：
- 能启动本地 HTTP server 并在 demo 结束前关闭。
- 能使用 Node 内置 `fetch` 发起请求。
- 能对外部 JSON 先按 `unknown` 处理，再做 runtime validation。
- 核心知识点：
- `createServer`、request、response。
- URL 和 searchParams。
- 400、404、500 的职责边界。
- 本地 server、graceful shutdown、SSE 入门。
- C++ 对照：
- Node HTTP 不默认每连接一个线程。
- 请求回调是事件循环中的任务。
- 建议练习：
- 写一个 `/health` 路由返回 JSON。
- 限制请求体大小并返回 400。
- 用 fetch 调用本地 server 后关闭 server。
- 验收提示：
- 能保证章节运行不依赖公网、不挂起端口。

#### ch30 加密与哈希

- 所属目录：`09_Node_API篇/chapter30_加密与哈希.ts`
- 学习目标：
- 能使用 crypto 做哈希、HMAC、随机数和密码派生。
- 能区分 hash、HMAC、加密和密码哈希。
- 能理解不要自创密码学方案。
- 核心知识点：
- SHA256、SHA3-256、MD5 的用途边界。
- `createHash`、`createHmac`。
- `randomBytes`、`randomUUID`。
- `pbkdf2`、`scrypt` 和 AES-GCM 入门。
- C++ 对照：
- crypto 模块封装 OpenSSL 能力，避免直接操作底层 C API。
- 密钥和 IV 管理比 API 调用更重要。
- 建议练习：
- 对固定文本计算 SHA256。
- 用 HMAC 验证消息未被篡改。
- 说明为什么 MD5 只能用于非安全校验。
- 验收提示：
- 能明确 secret 不写入源码或普通日志。

#### ch31 流

- 所属目录：`09_Node_API篇/chapter31_流.ts`
- 学习目标：
- 能理解 Readable、Writable、Duplex、Transform。
- 能使用 pipeline 处理背压和错误。
- 能用 async iterable 方式消费 readable。
- 核心知识点：
- paused mode 和 flowing mode。
- `pipe` 与 `pipeline` 的差异。
- 文件流和压缩流。
- readline 逐行处理大文件。
- C++ 对照：
- Node stream 是事件驱动并处理背压的抽象。
- 不等同于同步 iostream。
- 建议练习：
- 用 pipeline 把文本转换为大写后写入临时文件。
- 用 readline 统计行数。
- 写一个 Transform 流并处理错误。
- 验收提示：
- 能保证 stream 错误路径也关闭资源。

#### ch32 日期与时间

- 所属目录：`09_Node_API篇/chapter32_日期与时间.ts`
- 学习目标：
- 能使用 Date、时间戳和 Intl.DateTimeFormat。
- 能识别月份从 0 开始、时区和 DST 的坑。
- 能理解 Temporal 当前作为预览能力的边界。
- 核心知识点：
- `Date.now()` 和毫秒时间戳。
- 本地时区与 UTC。
- `toLocaleString` 和 `Intl.DateTimeFormat`。
- 第三方库选择提示。
- C++ 对照：
- `std::chrono` 类型安全更强，JS Date 可变且历史包袱多。
- 日期计算应尽量明确时区和单位。
- 建议练习：
- 用固定时间格式化中英文输出。
- 计算两个 UTC 时间戳之间的差值。
- 写下为什么直接 `Date.parse` 用户输入有风险。
- 验收提示：
- 能保证章节输出不依赖当前日期产生随机变化。

#### ch33 正则表达式

- 所属目录：`09_Node_API篇/chapter33_正则表达式.ts`
- 学习目标：
- 能创建 RegExp 并使用常用 flags。
- 能使用 `test`、`exec`、`matchAll`、`replace`。
- 能解析日志字段并避免灾难性回溯。
- 核心知识点：
- 字符类、量词、贪婪和懒惰。
- 捕获组、非捕获组、命名捕获。
- 前瞻、后顾和反向引用。
- `d` flag 返回匹配索引。
- C++ 对照：
- JS RegExp 使用 ECMAScript 语法和运行时实现。
- 复杂输入校验不要只依赖一个大正则。
- 建议练习：
- 提取 URL 中的 host 和 path。
- 用命名捕获解析一行访问日志。
- 写一个安全的邮箱格式初筛函数。
- 验收提示：
- 能解释正则适合格式提取，不适合完整业务验证。

### 第十阶段：工程实践篇

本阶段把语言和 API 放进可维护工程。

重点是稳定、可测试、可观测、可升级。

#### ch34 日志与调试

- 所属目录：`10_工程实践篇/chapter34_日志与调试.ts`
- 学习目标：
- 能使用 console、结构化日志和简单计时。
- 能理解日志安全和可观测性边界。
- 能使用 Node 调试入口和运行时断言。
- 核心知识点：
- `console.table`、`console.time`、`console.timeEnd`。
- JSON 结构化日志字段。
- 脱敏 token、password、email、IP。
- `node --inspect`、VS Code launch、`node:assert`。
- C++ 对照：
- console 与 GDB/断言各有职责。
- 生产日志不能替代调试器，也不能泄漏 secret。
- 建议练习：
- 写一个脱敏函数处理 email/token。
- 用 `console.time` 测量固定循环。
- 用 `assert.strictEqual` 保护一个运行时前置条件。
- 验收提示：
- 能判断哪些信息不应写入普通日志。

#### ch35 测试

- 所属目录：`10_工程实践篇/chapter35_测试.ts`
- 学习目标：
- 能使用 `node:test` 和 `node:assert` 写单元测试。
- 能理解 mock、异步测试和类型层测试。
- 能设计隔离、确定性的测试数据。
- 核心知识点：
- test、describe、it。
- `assert.ok`、`equal`、`deepEqual`、`strictEqual`。
- `assert.throws`、`assert.rejects`。
- `mock.method`、`mock.fn` 和覆盖率。
- C++ 对照：
- `node:test` 是 Node 内置测试工具，定位类似 Google Test 的基础测试能力。
- JS 异步测试必须等待 Promise 和资源关闭。
- 建议练习：
- 给一个纯函数写单元测试。
- 用 mock 替换一个时间函数。
- 写一个 `@ts-expect-error` 类型契约测试。
- 验收提示：
- 能保证测试不会依赖执行顺序、网络或当前时间。

#### ch36 性能分析

- 所属目录：`10_工程实践篇/chapter36_性能分析.ts`
- 学习目标：
- 能使用 performance API 和 console 计时定位热点。
- 能理解 V8 采样分析和火焰图工具的作用。
- 能识别事件循环阻塞和内存泄漏风险。
- 核心知识点：
- `performance.now`、mark、measure。
- `process.hrtime.bigint()`。
- `node --prof` 和 `node --prof-process`。
- 同步 I/O、闭包持有大对象、JSON 序列化成本。
- C++ 对照：
- 类似从 `std::chrono` 粗测进入 perf/valgrind 深测的层次。
- Node 还要关注事件循环延迟。
- 建议练习：
- 比较两种数组处理方式的耗时。
- 用 mark/measure 标记一个函数区间。
- 说明为什么大文件同步读取会阻塞服务。
- 验收提示：
- 能区分微基准和真实生产性能问题。

#### ch37 TS6 工程约束

- 所属目录：`10_工程实践篇/chapter37_TS6工程约束.ts`
- 学习目标：
- 能说明本教程 TypeScript 6.0 的工程边界。
- 能理解 TS6 迁移重点和弃用项。
- 能掌握版本锁定与依赖升级工作流。
- 核心知识点：
- `rootDir`、`types`、`tsc file.ts` 行为。
- 弃用旧 moduleResolution、旧 target 和旧模块模式。
- `--erasableSyntaxOnly` 的边界。
- import attributes、using、TS7 Beta 行为边界。
- C++ 对照：
- TS 版本更像工具链和语言服务持续演进。
- 大版本升级要通过 lockfile、build/test 和章节运行验证。
- 建议练习：
- 解释本项目为什么显式设置 `rootDir` 和 `types`。
- 找出 `package.json` 和 lockfile 中锁定的 TS 版本。
- 写一份依赖升级前后的验证清单。
- 验收提示：
- 能把“可用语法”和“本教程默认可运行路径”区分开。

### 第十一阶段：项目实战

本阶段把类型、Node API、错误处理、测试和工程组织组合起来。

项目章节不是语法展示，而是端到端工作流。

#### ch38 代码组织与规范

- 所属目录：`11_项目实战/chapter38_代码组织与规范.ts`
- 学习目标：
- 能设计清晰目录结构、模块边界和命名规范。
- 能理解 runtime validation、配置管理和错误处理分层。
- 能把前端最小闭环、API 契约和可观测性纳入项目边界。
- 核心知识点：
- 按功能分层和按领域分组。
- ESLint、Prettier、pre-commit hook。
- barrel export。
- JSON Schema/OpenAPI、logs、metrics、traces 概念。
- C++ 对照：
- TS 项目更依赖包管理、模块边界和自动化检查来维持长期一致性。
- 头文件式全局组织不适合现代 TS 工程。
- 建议练习：
- 为一个小服务画出 `src/`、`tests/`、`scripts/` 目录草图。
- 设计配置优先级：默认值、环境变量、CLI 参数、配置文件。
- 写三个 Conventional Commits 示例。
- 验收提示：
- 能解释为什么外部输入在入口层先是 `unknown`。

#### ch39 项目1 文件批处理 CLI

- 所属目录：`11_项目实战/chapter39_项目1_文件批处理CLI/main.ts`
- 学习目标：
- 能实现 CLI 参数解析和 runtime validation。
- 能递归遍历目录、转换文本、批量重命名。
- 能用 dry-run、覆盖保护和原子 rename 提升可靠性。
- 核心知识点：
- `process.argv` 手写解析。
- 目录遍历和文本转换。
- 输出目录限制和路径遍历防护。
- 日志输出和配置文件支持。
- C++ 对照：
- CLI 仍有 argv 直觉，但异步 fs 和跨平台路径需要 Node 风格处理。
- 文件写入要考虑失败恢复和幂等性。
- 建议练习：
- 增加一个 `--dry-run` 输出计划但不写文件。
- 为非法参数返回可读错误。
- 写测试覆盖路径限制和覆盖保护。
- 验收提示：
- 能保证 demo 写入临时目录且结束后清理。

#### ch40 项目2 日志分析 CLI

- 所属目录：`11_项目实战/chapter40_项目2_日志分析CLI/main.ts`
- 学习目标：
- 能按行解析大日志并聚合统计。
- 能用正则提取字段并处理失败行。
- 能输出 JSON/CSV 报告并做脱敏。
- 核心知识点：
- readline 流式处理。
- 日志行 runtime validation。
- Map 计数和分组。
- CSV escaping 和隐私保护。
- C++ 对照：
- 流式处理避免一次性把大文件读入内存。
- 正则解析失败要计数和采样，不应中断全量分析。
- 建议练习：
- 增加一个按 status code 分组的统计项。
- 收集前 3 条解析失败样例。
- 对报告中的 IP 或 token 做脱敏。
- 验收提示：
- 能保证大文件处理路径不会泄漏文件句柄。

## 阶段复盘清单

- 基础阶段结束后：能解释 TS、JS、Node 的职责边界。
- JS 运行时阶段结束后：能解释引用、闭包、原型和 `this`。
- 类型系统阶段结束后：能为外部输入、内部模型和输出 DTO 设计不同类型。
- 函数阶段结束后：能把函数签名写清楚，并知道闭包的内存影响。
- 面向对象阶段结束后：能区分 TS 编译期访问控制和 JS 运行时私有字段。
- 模块工程阶段结束后：能解释 NodeNext、`.js` 后缀 import 和声明文件。
- 迭代器阶段结束后：能写同步/异步 iterable，并确保退出路径。
- 异步阶段结束后：能解释 Promise、await、事件循环和 worker 的差异。
- Node API 阶段结束后：能写可重复运行、无公网依赖、会清理资源的示例。
- 工程实践阶段结束后：能写日志、测试、性能测量和升级验证清单。
- 项目阶段结束后：能把 CLI 输入、业务处理、输出报告和测试组织成一个可维护项目。

## 推荐最终验收方式

1. 执行 `npm run build`。
2. 执行 `npm test`。
3. 按 registry 或 dist 路径逐章运行 40 个章节入口。
4. 对涉及文件、网络、定时器、stream、worker 的章节检查资源清理。
5. 对涉及版本敏感 API 的章节核对本地 Node.js、TypeScript 和 tsconfig 基线。
