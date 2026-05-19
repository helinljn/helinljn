// =============================================================================
// 练习 01：基础语法
// =============================================================================
//
// 覆盖章节：ch01-ch05
//
// 目标：
//   1. 熟悉 number/string/boolean、数组、正则和控制流程
//   2. 把“能写出来”的语法整理成可测试的小函数
//   3. 练习稳定输出，避免随机数和当前时间影响验收
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

/** Shopping cart item used by the basic collection exercises. */
export type ShoppingItem = {
    name: string;
    price: number;
    quantity: number;
};

type FizzBuzzRule = {
    divisor: number;
    word: string;
};

const FIZZ_BUZZ_RULES: readonly FizzBuzzRule[] = [
    { divisor: 3, word: "Fizz" },
    { divisor: 5, word: "Buzz" }
];

// =============================================================================
// 01.1 数值边界：clamp
// =============================================================================

/** Clamp a number into an inclusive [min, max] range. */
export function clamp(value: number, min: number, max: number): number {
    if (min > max) {
        throw new RangeError("min must be less than or equal to max");
    }
    return Math.min(Math.max(value, min), max);
}

// =============================================================================
// 01.2 字符串与正则：提取数字
// =============================================================================

/** Extract signed integers and decimal numbers from text. */
export function extractNumbers(text: string): number[] {
    return (text.match(/-?\d+(?:\.\d+)?/g) ?? []).map(Number);
}

// =============================================================================
// 01.3 控制流程：FizzBuzz
// =============================================================================

/** Return FizzBuzz values from 1 to limit. */
export function fizzBuzz(limit: number): string[] {
    const result: string[] = [];
    for (let value = 1; value <= limit; value += 1) {
        const word = FIZZ_BUZZ_RULES
            .filter((rule) => value % rule.divisor === 0)
            .map((rule) => rule.word)
            .join("");
        result.push(word === "" ? value.toString(10) : word);
    }
    return result;
}

// =============================================================================
// 01.4 数组与 reduce：购物车总价
// =============================================================================

/** Sum item price * quantity for a readonly shopping cart. */
export function cartTotal(items: readonly ShoppingItem[]): number {
    return items.reduce((total, item) => total + item.price * item.quantity, 0);
}

function demoNumberAndStringExercises(): void {
    section("练习 01.1：数值和字符串");
    note("要求", "实现 clamp 与 extractNumbers，并为越界、空输入和负数写测试。");
    showJson("clamp 样例", {
        high: clamp(20, 0, 10),
        low: clamp(-1, 0, 10),
        inside: clamp(7, 0, 10)
    });
    showJson("extractNumbers 样例", extractNumbers("x=-2, y=3.5, count=10"));
}

function demoControlFlowAndArrayExercises(): void {
    section("练习 01.2：控制流程和数组");
    note("要求", "实现 fizzBuzz 与 cartTotal，注意 readonly 输入不应被修改。");
    showJson("fizzBuzz", fizzBuzz(15));
    showJson("cartTotal", {
        total: cartTotal([
            { name: "book", price: 12, quantity: 3 },
            { name: "pen", price: 2, quantity: 5 }
        ])
    });
}

/** Run basic syntax exercises. */
export function runExercises(): void {
    section("练习 01：基础语法");
    demoNumberAndStringExercises();
    demoControlFlowAndArrayExercises();
}

await runIfMain(import.meta.url, runExercises);
