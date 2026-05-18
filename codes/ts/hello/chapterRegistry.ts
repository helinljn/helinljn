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
import { runChapter as runLegacyChapter07 } from "./06_模块与工程篇/chapter07_模块与工程.js";
import { runChapter as runLegacyChapter09 } from "./08_Node运行时与API篇/chapter09_Node运行时与API.js";
import { runChapter as runLegacyChapter10 } from "./09_异步与网络篇/chapter10_异步与网络.js";
import { runChapter as runLegacyChapter11 } from "./10_TS6工程约束篇/chapter11_TS6工程约束.js";
import { runChapter as runLegacyChapter12 } from "./11_项目实战/chapter12_代码组织与规范.js";
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
        title: "ch13 项目1 文件批处理 CLI",
        phase: "项目",
        path: "11_项目实战/chapter12_项目1_文件批处理工具/main.ts",
        run: runFileBatchDemo
    },
    {
        id: 20,
        title: "ch14 项目2 日志分析 CLI",
        phase: "项目",
        path: "11_项目实战/chapter13_项目2_日志分析工具/main.ts",
        run: runLogAnalyzerDemo
    },
    {
        id: 21,
        title: "练习01 基础语法",
        phase: "练习",
        path: "练习题与答案/exercises_01_基础.ts",
        run: runExercises01
    },
    {
        id: 22,
        title: "练习02 类型系统",
        phase: "练习",
        path: "练习题与答案/exercises_02_类型系统.ts",
        run: runExercises02
    },
    {
        id: 23,
        title: "练习03 泛型与 OOP",
        phase: "练习",
        path: "练习题与答案/exercises_03_泛型与OOP.ts",
        run: runExercises03
    },
    {
        id: 24,
        title: "练习04 综合练习",
        phase: "练习",
        path: "练习题与答案/exercises_04_综合.ts",
        run: runExercises04
    },
    {
        id: 27,
        title: "ch07 模块系统与工程配置",
        phase: "模块与工程",
        path: "06_模块与工程篇/chapter07_模块与工程.ts",
        run: runLegacyChapter07
    },
    {
        id: 28,
        title: "ch09 Node 运行时与常用 API",
        phase: "Node API",
        path: "08_Node运行时与API篇/chapter09_Node运行时与API.ts",
        run: runLegacyChapter09
    },
    {
        id: 29,
        title: "ch10 异步、事件循环与网络",
        phase: "异步编程",
        path: "09_异步与网络篇/chapter10_异步与网络.ts",
        run: runLegacyChapter10
    },
    {
        id: 30,
        title: "ch11 TypeScript 6.0 工程约束",
        phase: "工程实践",
        path: "10_TS6工程约束篇/chapter11_TS6工程约束.ts",
        run: runLegacyChapter11
    },
    {
        id: 31,
        title: "ch12 项目实战前的代码组织与规范",
        phase: "项目实战",
        path: "11_项目实战/chapter12_代码组织与规范.ts",
        run: runLegacyChapter12
    }
];

export const TEACHING_PHASES = new Set(CHAPTERS.map((chapter) => chapter.phase));
