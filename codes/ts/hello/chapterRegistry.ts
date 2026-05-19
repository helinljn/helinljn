import type { Chapter } from "./shared/chapter.js";
import { runChapter as runChapter01 } from "./01_基础篇/chapter01_入门与环境.js";
import { runChapter as runChapter02 } from "./01_基础篇/chapter02_变量与基本类型.js";
import { runChapter as runChapter03 } from "./01_基础篇/chapter03_控制流程.js";
import { runChapter as runChapter04 } from "./01_基础篇/chapter04_字符串处理.js";
import { runChapter as runChapter05 } from "./01_基础篇/chapter05_数组与集合类型.js";
import { runChapter as runChapter06 } from "./02_JS运行时篇/chapter06_对象与引用.js";
import { runChapter as runChapter07 } from "./02_JS运行时篇/chapter07_闭包与作用域.js";
import { runChapter as runChapter08 } from "./02_JS运行时篇/chapter08_原型与this.js";
import { runChapter as runChapter09 } from "./03_类型系统篇/chapter09_类型标注与推断.js";
import { runChapter as runChapter10 } from "./03_类型系统篇/chapter10_联合类型与收窄.js";
import { runChapter as runChapter11 } from "./03_类型系统篇/chapter11_接口与类型别名.js";
import { runChapter as runChapter12 } from "./03_类型系统篇/chapter12_泛型.js";
import { runChapter as runChapter13 } from "./03_类型系统篇/chapter13_高级类型工具.js";
import { runChapter as runChapter14 } from "./04_函数篇/chapter14_函数详解.js";
import { runChapter as runChapter15 } from "./04_函数篇/chapter15_函数式编程模式.js";
import { runChapter as runChapter16 } from "./05_面向对象篇/chapter16_类与对象.js";
import { runChapter as runChapter17 } from "./05_面向对象篇/chapter17_继承与多态.js";
import { runChapter as runChapter18 } from "./05_面向对象篇/chapter18_异常处理.js";
import { runChapter as runChapter19 } from "./06_模块与工程篇/chapter19_模块系统.js";
import { runChapter as runChapter20 } from "./06_模块与工程篇/chapter20_工程配置与声明文件.js";
import { runChapter as runChapter21 } from "./06_模块与工程篇/chapter21_装饰器.js";
import { runChapter as runChapter22 } from "./07_迭代器与生成器篇/chapter22_迭代器与生成器.js";
import { runChapter as runChapter23 } from "./08_异步编程篇/chapter23_Promise深入.js";
import { runChapter as runChapter24 } from "./08_异步编程篇/chapter24_async与await.js";
import { runChapter as runChapter25 } from "./08_异步编程篇/chapter25_事件循环与并发.js";
import { runChapter as runChapter26 } from "./09_Node_API篇/chapter26_文件系统.js";
import { runChapter as runChapter27 } from "./09_Node_API篇/chapter27_路径进程与环境.js";
import { runChapter as runChapter28 } from "./09_Node_API篇/chapter28_Buffer与二进制.js";
import { runChapter as runChapter29 } from "./09_Node_API篇/chapter29_网络与HTTP.js";
import { runChapter as runChapter30 } from "./09_Node_API篇/chapter30_加密与哈希.js";
import { runChapter as runChapter31 } from "./09_Node_API篇/chapter31_流.js";
import { runChapter as runChapter32 } from "./09_Node_API篇/chapter32_日期与时间.js";
import { runChapter as runChapter33 } from "./09_Node_API篇/chapter33_正则表达式.js";
import { runChapter as runChapter34 } from "./10_工程实践篇/chapter34_日志与调试.js";
import { runChapter as runChapter35 } from "./10_工程实践篇/chapter35_测试.js";
import { runChapter as runChapter36 } from "./10_工程实践篇/chapter36_性能分析.js";
import { runChapter as runChapter37 } from "./10_工程实践篇/chapter37_TS6工程约束.js";
import { runFileBatchDemo } from "./11_项目实战/chapter12_项目1_文件批处理工具/main.js";
import { runLogAnalyzerDemo } from "./11_项目实战/chapter13_项目2_日志分析工具/main.js";
import { runExercises as runExercises01 } from "./练习题与答案/exercises_01_基础.js";
import { runExercises as runExercises02 } from "./练习题与答案/exercises_02_类型系统.js";
import { runExercises as runExercises03 } from "./练习题与答案/exercises_03_泛型与OOP.js";
import { runExercises as runExercises04 } from "./练习题与答案/exercises_04_综合.js";

export const CHAPTERS: Chapter[] = [
    {
        id: 1,
        title: "ch01 入门与环境",
        phase: "基础",
        path: "01_基础篇/chapter01_入门与环境.ts",
        run: runChapter01
    },
    {
        id: 2,
        title: "ch02 变量与基本类型",
        phase: "基础",
        path: "01_基础篇/chapter02_变量与基本类型.ts",
        run: runChapter02
    },
    {
        id: 3,
        title: "ch03 控制流程",
        phase: "基础",
        path: "01_基础篇/chapter03_控制流程.ts",
        run: runChapter03
    },
    {
        id: 4,
        title: "ch04 字符串处理",
        phase: "基础",
        path: "01_基础篇/chapter04_字符串处理.ts",
        run: runChapter04
    },
    {
        id: 5,
        title: "ch05 数组与集合类型",
        phase: "基础",
        path: "01_基础篇/chapter05_数组与集合类型.ts",
        run: runChapter05
    },
    {
        id: 6,
        title: "ch06 对象与引用",
        phase: "JS运行时",
        path: "02_JS运行时篇/chapter06_对象与引用.ts",
        run: runChapter06
    },
    {
        id: 7,
        title: "ch07 闭包与作用域",
        phase: "JS运行时",
        path: "02_JS运行时篇/chapter07_闭包与作用域.ts",
        run: runChapter07
    },
    {
        id: 8,
        title: "ch08 原型与this",
        phase: "JS运行时",
        path: "02_JS运行时篇/chapter08_原型与this.ts",
        run: runChapter08
    },
    {
        id: 9,
        title: "ch09 类型标注与推断",
        phase: "类型系统",
        path: "03_类型系统篇/chapter09_类型标注与推断.ts",
        run: runChapter09
    },
    {
        id: 10,
        title: "ch10 联合类型与收窄",
        phase: "类型系统",
        path: "03_类型系统篇/chapter10_联合类型与收窄.ts",
        run: runChapter10
    },
    {
        id: 11,
        title: "ch11 接口与类型别名",
        phase: "类型系统",
        path: "03_类型系统篇/chapter11_接口与类型别名.ts",
        run: runChapter11
    },
    {
        id: 12,
        title: "ch12 泛型",
        phase: "类型系统",
        path: "03_类型系统篇/chapter12_泛型.ts",
        run: runChapter12
    },
    {
        id: 13,
        title: "ch13 高级类型工具",
        phase: "类型系统",
        path: "03_类型系统篇/chapter13_高级类型工具.ts",
        run: runChapter13
    },
    {
        id: 14,
        title: "ch14 函数详解",
        phase: "函数",
        path: "04_函数篇/chapter14_函数详解.ts",
        run: runChapter14
    },
    {
        id: 15,
        title: "ch15 函数式编程模式",
        phase: "函数",
        path: "04_函数篇/chapter15_函数式编程模式.ts",
        run: runChapter15
    },
    {
        id: 16,
        title: "ch16 类与对象",
        phase: "面向对象",
        path: "05_面向对象篇/chapter16_类与对象.ts",
        run: runChapter16
    },
    {
        id: 17,
        title: "ch17 继承与多态",
        phase: "面向对象",
        path: "05_面向对象篇/chapter17_继承与多态.ts",
        run: runChapter17
    },
    {
        id: 18,
        title: "ch18 异常处理",
        phase: "面向对象",
        path: "05_面向对象篇/chapter18_异常处理.ts",
        run: runChapter18
    },
    {
        id: 19,
        title: "ch19 模块系统",
        phase: "模块与工程",
        path: "06_模块与工程篇/chapter19_模块系统.ts",
        run: runChapter19
    },
    {
        id: 20,
        title: "ch20 工程配置与声明文件",
        phase: "模块与工程",
        path: "06_模块与工程篇/chapter20_工程配置与声明文件.ts",
        run: runChapter20
    },
    {
        id: 21,
        title: "ch21 装饰器",
        phase: "模块与工程",
        path: "06_模块与工程篇/chapter21_装饰器.ts",
        run: runChapter21
    },
    {
        id: 22,
        title: "ch22 迭代器与生成器",
        phase: "迭代器与生成器",
        path: "07_迭代器与生成器篇/chapter22_迭代器与生成器.ts",
        run: runChapter22
    },
    {
        id: 23,
        title: "ch23 Promise 深入",
        phase: "异步编程",
        path: "08_异步编程篇/chapter23_Promise深入.ts",
        run: runChapter23
    },
    {
        id: 24,
        title: "ch24 async 与 await",
        phase: "异步编程",
        path: "08_异步编程篇/chapter24_async与await.ts",
        run: runChapter24
    },
    {
        id: 25,
        title: "ch25 事件循环与并发",
        phase: "异步编程",
        path: "08_异步编程篇/chapter25_事件循环与并发.ts",
        run: runChapter25
    },
    {
        id: 26,
        title: "ch26 文件系统",
        phase: "Node API",
        path: "09_Node_API篇/chapter26_文件系统.ts",
        run: runChapter26
    },
    {
        id: 27,
        title: "ch27 路径进程与环境",
        phase: "Node API",
        path: "09_Node_API篇/chapter27_路径进程与环境.ts",
        run: runChapter27
    },
    {
        id: 28,
        title: "ch28 Buffer 与二进制",
        phase: "Node API",
        path: "09_Node_API篇/chapter28_Buffer与二进制.ts",
        run: runChapter28
    },
    {
        id: 29,
        title: "ch29 网络与 HTTP",
        phase: "Node API",
        path: "09_Node_API篇/chapter29_网络与HTTP.ts",
        run: runChapter29
    },
    {
        id: 30,
        title: "ch30 加密与哈希",
        phase: "Node API",
        path: "09_Node_API篇/chapter30_加密与哈希.ts",
        run: runChapter30
    },
    {
        id: 31,
        title: "ch31 流",
        phase: "Node API",
        path: "09_Node_API篇/chapter31_流.ts",
        run: runChapter31
    },
    {
        id: 32,
        title: "ch32 日期与时间",
        phase: "Node API",
        path: "09_Node_API篇/chapter32_日期与时间.ts",
        run: runChapter32
    },
    {
        id: 33,
        title: "ch33 正则表达式",
        phase: "Node API",
        path: "09_Node_API篇/chapter33_正则表达式.ts",
        run: runChapter33
    },
    {
        id: 34,
        title: "ch34 日志与调试",
        phase: "工程实践",
        path: "10_工程实践篇/chapter34_日志与调试.ts",
        run: runChapter34
    },
    {
        id: 35,
        title: "ch35 测试",
        phase: "工程实践",
        path: "10_工程实践篇/chapter35_测试.ts",
        run: runChapter35
    },
    {
        id: 36,
        title: "ch36 性能分析",
        phase: "工程实践",
        path: "10_工程实践篇/chapter36_性能分析.ts",
        run: runChapter36
    },
    {
        id: 37,
        title: "ch37 TS6 工程约束",
        phase: "工程实践",
        path: "10_工程实践篇/chapter37_TS6工程约束.ts",
        run: runChapter37
    },
    {
        id: 39,
        title: "ch13 项目1 文件批处理 CLI",
        phase: "项目实战",
        path: "11_项目实战/chapter12_项目1_文件批处理工具/main.ts",
        run: runFileBatchDemo
    },
    {
        id: 40,
        title: "ch14 项目2 日志分析 CLI",
        phase: "项目实战",
        path: "11_项目实战/chapter13_项目2_日志分析工具/main.ts",
        run: runLogAnalyzerDemo
    },
    {
        id: 41,
        title: "练习01 基础语法",
        phase: "练习",
        path: "练习题与答案/exercises_01_基础.ts",
        run: runExercises01
    },
    {
        id: 42,
        title: "练习02 类型系统",
        phase: "练习",
        path: "练习题与答案/exercises_02_类型系统.ts",
        run: runExercises02
    },
    {
        id: 43,
        title: "练习03 泛型与 OOP",
        phase: "练习",
        path: "练习题与答案/exercises_03_泛型与OOP.ts",
        run: runExercises03
    },
    {
        id: 44,
        title: "练习04 综合练习",
        phase: "练习",
        path: "练习题与答案/exercises_04_综合.ts",
        run: runExercises04
    },
];

export const TEACHING_PHASES = new Set(CHAPTERS.map((chapter) => chapter.phase));
