import { runIfMain, section, showJson } from "../shared/chapter.js";

export type ShoppingItem = {
  name: string;
  price: number;
  quantity: number;
};

export function clamp(value: number, min: number, max: number): number {
  return Math.min(Math.max(value, min), max);
}

export function extractNumbers(text: string): number[] {
  return (text.match(/-?\d+(?:\.\d+)?/g) ?? []).map(Number);
}

export function fizzBuzz(limit: number): string[] {
  const result: string[] = [];
  for (let value = 1; value <= limit; value += 1) {
    if (value % 15 === 0) {
      result.push("FizzBuzz");
    } else if (value % 3 === 0) {
      result.push("Fizz");
    } else if (value % 5 === 0) {
      result.push("Buzz");
    } else {
      result.push(value.toString(10));
    }
  }
  return result;
}

export function cartTotal(items: readonly ShoppingItem[]): number {
  return items.reduce((total, item) => total + item.price * item.quantity, 0);
}

export function runExercises(): void {
  section("练习 01：基础语法");
  showJson("clamp", [clamp(20, 0, 10), clamp(-1, 0, 10)]);
  showJson("extractNumbers", extractNumbers("x=-2, y=3.5"));
  showJson("fizzBuzz", fizzBuzz(15));
  console.log(cartTotal([{ name: "book", price: 30, quantity: 2 }]));
}

await runIfMain(import.meta.url, runExercises);
