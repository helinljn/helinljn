import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type Temperature = {
  readonly celsius: number;
  readonly label?: string;
};

export function classifyTemperature(input: Temperature): "cold" | "warm" | "hot" {
  if (input.celsius < 10) {
    return "cold";
  }
  if (input.celsius >= 30) {
    return "hot";
  }
  return "warm";
}

export function makeTable(rows: number, cols: number): number[][] {
  const table: number[][] = [];
  for (let row = 0; row < rows; row += 1) {
    const current: number[] = [];
    for (let col = 0; col < cols; col += 1) {
      current.push(row * cols + col);
    }
    table.push(current);
  }
  return table;
}

export function runChapter(): void {
  section("第 2 章：变量、基本类型与控制流");
  note("C++ 对照", "`let` 接近可变局部变量，`const` 约束绑定不可重新赋值，不等于对象深度不可变。");
  note("类型提醒", "`number` 是 IEEE 754 双精度浮点，不区分 int、long、double。");

  const room: Temperature = { celsius: 24, label: "office" };
  console.log(`${room.label}: ${classifyTemperature(room)}`);
  showJson("二维表", makeTable(2, 3));
}

await runIfMain(import.meta.url, runChapter);
