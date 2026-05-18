import type { Chapter } from "./shared/chapter.js";
import { runChapter as runChapter01 } from "./01_基础篇/chapter01_入门与环境.js";
import { runChapter as runChapter02 } from "./01_基础篇/chapter02_变量与基本类型.js";
import { runChapter as runChapter03 } from "./01_基础篇/chapter03_控制流程.js";
import { runChapter as runChapter04 } from "./01_基础篇/chapter04_字符串处理.js";
import { runChapter as runChapter05 } from "./01_基础篇/chapter05_数组与集合类型.js";
import { runChapter as runLegacyChapter03 } from "./02_JS运行时语义篇/chapter03_JS运行时语义.js";
import { runChapter as runLegacyChapter04 } from "./03_类型系统篇/chapter04_类型系统基础.js";
import { runChapter as runLegacyChapter05 } from "./04_函数与泛型篇/chapter05_函数与泛型.js";
import { runChapter as runChapter06 } from "./05_面向对象篇/chapter06_面向对象.js";
import { runChapter as runChapter07 } from "./06_模块与工程篇/chapter07_模块与工程.js";
import { runChapter as runChapter08 } from "./07_高级类型篇/chapter08_高级类型.js";
import { runChapter as runChapter09 } from "./08_Node运行时与API篇/chapter09_Node运行时与API.js";
import { runChapter as runChapter10 } from "./09_异步与网络篇/chapter10_异步与网络.js";
import { runChapter as runChapter11 } from "./10_TS6工程约束篇/chapter11_TS6工程约束.js";
import { runChapter as runChapter12 } from "./11_项目实战/chapter12_代码组织与规范.js";
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
        title: "ch03 必要的 JS 运行时语义",
        phase: "JS运行时",
        path: "02_JS运行时语义篇/chapter03_JS运行时语义.ts",
        run: runLegacyChapter03
    },
    {
        id: 7,
        title: "ch04 类型系统基础",
        phase: "类型系统",
        path: "03_类型系统篇/chapter04_类型系统基础.ts",
        run: runLegacyChapter04
    },
    {
        id: 8,
        title: "ch05 函数、重载与泛型",
        phase: "函数泛型",
        path: "04_函数与泛型篇/chapter05_函数与泛型.ts",
        run: runLegacyChapter05
    },
    {
        id: 9,
        title: "ch06 面向对象与结构化类型",
        phase: "OOP",
        path: "05_面向对象篇/chapter06_面向对象.ts",
        run: runChapter06
    },
    {
        id: 10,
        title: "ch07 模块系统与工程配置",
        phase: "工程",
        path: "06_模块与工程篇/chapter07_模块与工程.ts",
        run: runChapter07
    },
    {
        id: 11,
        title: "ch08 高级类型工具",
        phase: "高级类型",
        path: "07_高级类型篇/chapter08_高级类型.ts",
        run: runChapter08
    },
    {
        id: 12,
        title: "ch09 Node 运行时与常用 API",
        phase: "Node API",
        path: "08_Node运行时与API篇/chapter09_Node运行时与API.ts",
        run: runChapter09
    },
    {
        id: 13,
        title: "ch10 异步、事件循环与网络",
        phase: "异步网络",
        path: "09_异步与网络篇/chapter10_异步与网络.ts",
        run: runChapter10
    },
    {
        id: 14,
        title: "ch11 TypeScript 6.0 工程约束",
        phase: "TS6",
        path: "10_TS6工程约束篇/chapter11_TS6工程约束.ts",
        run: runChapter11
    },
    {
        id: 15,
        title: "ch12 项目实战前的代码组织与规范",
        phase: "项目",
        path: "11_项目实战/chapter12_代码组织与规范.ts",
        run: runChapter12
    },
    {
        id: 16,
        title: "ch13 项目1 文件批处理 CLI",
        phase: "项目",
        path: "11_项目实战/chapter12_项目1_文件批处理工具/main.ts",
        run: runFileBatchDemo
    },
    {
        id: 17,
        title: "ch14 项目2 日志分析 CLI",
        phase: "项目",
        path: "11_项目实战/chapter13_项目2_日志分析工具/main.ts",
        run: runLogAnalyzerDemo
    },
    {
        id: 18,
        title: "练习01 基础语法",
        phase: "练习",
        path: "练习题与答案/exercises_01_基础.ts",
        run: runExercises01
    },
    {
        id: 19,
        title: "练习02 类型系统",
        phase: "练习",
        path: "练习题与答案/exercises_02_类型系统.ts",
        run: runExercises02
    },
    {
        id: 20,
        title: "练习03 泛型与 OOP",
        phase: "练习",
        path: "练习题与答案/exercises_03_泛型与OOP.ts",
        run: runExercises03
    },
    {
        id: 21,
        title: "练习04 综合练习",
        phase: "练习",
        path: "练习题与答案/exercises_04_综合.ts",
        run: runExercises04
    }
];

export const TEACHING_PHASES = new Set(CHAPTERS.map((chapter) => chapter.phase));
