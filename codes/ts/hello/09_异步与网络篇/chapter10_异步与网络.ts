import { createServer } from "node:http";
import type { AddressInfo } from "node:net";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export function delay(ms: number): Promise<void> {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
  });
}

export async function withTimeout<T>(work: Promise<T>, ms: number): Promise<T> {
  const timeout = new Promise<never>((_, reject) => {
    setTimeout(() => reject(new Error(`timeout after ${ms}ms`)), ms);
  });
  return Promise.race([work, timeout]);
}

export async function runLocalHttpDemo(): Promise<string> {
  const server = createServer((_request, response) => {
    response.writeHead(200, { "content-type": "application/json; charset=utf-8" });
    response.end(JSON.stringify({ message: "hello from local node server" }));
  });

  await new Promise<void>((resolve) => {
    server.listen(0, "127.0.0.1", resolve);
  });

  const address = server.address();
  if (typeof address !== "object" || address === null) {
    throw new Error("server did not expose a TCP address");
  }

  try {
    const url = `http://127.0.0.1:${(address as AddressInfo).port}/`;
    const response = await fetch(url);
    return await response.text();
  } finally {
    await new Promise<void>((resolve, reject) => {
      server.close((error) => {
        if (error) {
          reject(error);
        } else {
          resolve();
        }
      });
    });
  }
}

export async function runChapter(): Promise<void> {
  section("第 10 章：异步、事件循环与网络");
  note("C++ 对照", "Promise 不是线程；Node 异步 I/O 由事件循环调度，CPU 密集任务仍会阻塞主线程。");
  note("网络边界", "`fetch` 是 Web/Node 运行时 API，TS 只检查 Request/Response 等类型。");

  await delay(1);
  const body = await withTimeout(runLocalHttpDemo(), 1000);
  showJson("本地 HTTP 响应", JSON.parse(body) as unknown);
}

await runIfMain(import.meta.url, runChapter);
