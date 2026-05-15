import { spawnSync } from "node:child_process";
import { stdin, stdout } from "node:process";
import { createInterface } from "node:readline/promises";
import { CHAPTERS } from "./chapterRegistry.js";

function showMenu(): void {
  console.log("=".repeat(72));
  console.log("TypeScript 6.0 学习工程 - 交互式章节导航");
  console.log("=".repeat(72));
  console.log();
  console.log("命令:");
  console.log("  L       列出全部章节");
  console.log("  R编号   运行指定章节，例如 R1、R10");
  console.log("  A       运行全部教学章节");
  console.log("  T       运行自动化测试");
  console.log("  H       显示帮助");
  console.log("  Q       退出");
  console.log();
}

function showHelp(): void {
  console.log();
  console.log("使用方式:");
  console.log("  npm run start              进入交互式菜单");
  console.log("  npm run chapter -- 1       运行第 1 章");
  console.log("  npm test                   编译并运行测试");
  console.log();
}

function showChapterList(): void {
  for (const chapter of CHAPTERS) {
    console.log(`  ${chapter.id.toString().padStart(2, " ")}. [${chapter.phase}] ${chapter.title}`);
  }
}

async function runChapterByNumber(text: string): Promise<void> {
  const id = Number.parseInt(text, 10);
  const chapter = CHAPTERS.find((item) => item.id === id);
  if (chapter === undefined) {
    console.log(`无效章节编号: ${text}`);
    return;
  }
  await chapter.run();
}

async function runAllChapters(): Promise<void> {
  for (const chapter of CHAPTERS) {
    await chapter.run();
  }
}

function runTests(): void {
  const result = spawnSync("npm", ["test"], {
    cwd: process.cwd(),
    stdio: "inherit",
    shell: process.platform === "win32"
  });
  if (result.error !== undefined) {
    throw result.error;
  }
  process.exitCode = result.status ?? 1;
}

async function runInteractive(): Promise<void> {
  showMenu();
  const rl = createInterface({ input: stdin, output: stdout });

  try {
    while (true) {
      const command = (await rl.question("> ")).trim().toUpperCase();
      if (command === "") {
        continue;
      }
      if (command === "Q" || command === "QUIT" || command === "EXIT") {
        break;
      }
      if (command === "H") {
        showHelp();
      } else if (command === "L") {
        showChapterList();
      } else if (command === "A") {
        await runAllChapters();
      } else if (command === "T") {
        runTests();
      } else if (command.startsWith("R")) {
        await runChapterByNumber(command.slice(1));
      } else if (/^\d+$/.test(command)) {
        await runChapterByNumber(command);
      } else {
        console.log("无法识别命令，输入 H 查看帮助。");
      }
    }
  } finally {
    rl.close();
  }
}

const [, , mode, chapterNumber] = process.argv;
if (mode === "chapter") {
  if (chapterNumber === undefined) {
    console.log("请提供章节编号，例如: npm run chapter -- 1");
  } else {
    await runChapterByNumber(chapterNumber);
  }
} else if (mode === "list") {
  showChapterList();
} else {
  await runInteractive();
}
