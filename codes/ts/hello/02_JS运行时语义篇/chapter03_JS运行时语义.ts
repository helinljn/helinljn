import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type Point = {
  x: number;
  y: number;
};

export function movePoint(point: Point, dx: number, dy: number): Point {
  point.x += dx;
  point.y += dy;
  return point;
}

export function makeCounter(start = 0): () => number {
  let value = start;
  return () => {
    value += 1;
    return value;
  };
}

export function prototypeLookupDemo(): string {
  const base = { role: "base object" };
  const child = Object.create(base) as { role?: string; name: string };
  child.name = "child";
  return `${child.name} -> ${child.role}`;
}

export function runChapter(): void {
  section("第 3 章：必要的 JavaScript 运行时语义");
  note("核心边界", "TS 类型会被擦除，闭包、对象引用、原型链、异常和事件循环都按 JS 规则运行。");
  note("C++ 对照", "对象赋值不是复制结构体，默认是引用同一个对象。");

  const point: Point = { x: 1, y: 2 };
  const samePoint = movePoint(point, 10, 20);
  showJson("对象引用变化", { point, samePoint, isSameObject: point === samePoint });

  const next = makeCounter(10);
  showJson("闭包计数器", [next(), next(), next()]);
  console.log(prototypeLookupDemo());
}

await runIfMain(import.meta.url, runChapter);
