import { spawnSync } from "node:child_process";
import { stdin, stdout } from "node:process";
import { createInterface } from "node:readline/promises";
import { CHAPTERS } from "./chapterRegistry.js";
import type { Chapter } from "./shared/chapter.js";

const PHASE_ORDER = [
    "基础",
    "JS运行时",
    "类型系统",
    "函数",
    "面向对象",
    "模块与工程",
    "迭代器与生成器",
    "异步编程",
    "Node API",
    "工程实践",
    "项目实战",
    "练习"
] as const;

function groupChaptersByPhase(): Map<string, Chapter[]> {
    const grouped = new Map<string, Chapter[]>();
    for (const phase of PHASE_ORDER) {
        grouped.set(phase, []);
    }
    for (const chapter of CHAPTERS) {
        const chapters = grouped.get(chapter.phase);
        if (chapters === undefined) {
            grouped.set(chapter.phase, [chapter]);
        } else {
            chapters.push(chapter);
        }
    }
    return grouped;
}

function showMenu(): void {
    console.log("=".repeat(72));
    console.log("TypeScript 6.0 学习工程 - 交互式章节导航");
    console.log("=".repeat(72));
    console.log();
    console.log("命令:");
    console.log("  L             按阶段列出全部章节");
    console.log("  P 阶段名      列出某个阶段，例如 P 基础、P Node API");
    console.log("  R编号         运行指定章节，例如 R1、R38、R40");
    console.log("  A             运行 registry 中的全部条目");
    console.log("  T             运行自动化测试");
    console.log("  H             显示帮助");
    console.log("  Q             退出");
    console.log();
}

function showHelp(): void {
    console.log();
    console.log("使用方式:");
    console.log("  npm run start                    进入交互式菜单");
    console.log("  npm run chapter -- 1             运行第 1 章");
    console.log("  node dist/main.js list           按阶段列出全部章节");
    console.log("  node dist/main.js phase 基础      列出基础阶段章节");
    console.log("  npm test                         编译并运行测试");
    console.log();
    console.log("阶段名:");
    console.log(`  ${PHASE_ORDER.join("、")}`);
    console.log();
}

function printChapterLine(chapter: Chapter): void {
    console.log(`  ${chapter.id.toString().padStart(2, " ")}. ${chapter.title}`);
}

function showChapterList(): void {
    const grouped = groupChaptersByPhase();
    for (const phase of PHASE_ORDER) {
        const chapters = grouped.get(phase) ?? [];
        if (chapters.length === 0) {
            continue;
        }
        console.log();
        console.log(`[${phase}]`);
        for (const chapter of chapters) {
            printChapterLine(chapter);
        }
    }
}

function findPhaseName(input: string): string | undefined {
    const trimmed = input.trim();
    if (trimmed.length === 0) {
        return undefined;
    }
    const normalized = trimmed.toLocaleLowerCase();
    return PHASE_ORDER.find((phase) => phase.toLocaleLowerCase() === normalized)
        ?? Array.from(new Set(CHAPTERS.map((chapter) => chapter.phase))).find(
            (phase) => phase.toLocaleLowerCase() === normalized
        );
}

function showChaptersByPhase(phaseText: string): void {
    const phase = findPhaseName(phaseText);
    if (phase === undefined) {
        console.log(`未知阶段: ${phaseText}`);
        console.log(`可用阶段: ${PHASE_ORDER.join("、")}`);
        return;
    }

    const chapters = CHAPTERS.filter((chapter) => chapter.phase === phase);
    console.log();
    console.log(`[${phase}]`);
    for (const chapter of chapters) {
        printChapterLine(chapter);
    }
}

async function runChapterByNumber(text: string): Promise<void> {
    const id = Number.parseInt(text.trim(), 10);
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

async function handleCommand(commandText: string): Promise<boolean> {
    const trimmed = commandText.trim();
    const upper = trimmed.toLocaleUpperCase();

    if (trimmed === "") {
        return true;
    }
    if (upper === "Q" || upper === "QUIT" || upper === "EXIT") {
        return false;
    }
    if (upper === "H") {
        showHelp();
    } else if (upper === "L") {
        showChapterList();
    } else if (upper === "A") {
        await runAllChapters();
    } else if (upper === "T") {
        runTests();
    } else if (upper.startsWith("P ")) {
        showChaptersByPhase(trimmed.slice(2));
    } else if (upper.startsWith("R")) {
        await runChapterByNumber(trimmed.slice(1));
    } else if (/^\d+$/.test(trimmed)) {
        await runChapterByNumber(trimmed);
    } else {
        console.log("无法识别命令，输入 H 查看帮助。");
    }

    return true;
}

async function runInteractive(): Promise<void> {
    showMenu();
    const rl = createInterface({ input: stdin, output: stdout });

    try {
        while (true) {
            const shouldContinue = await handleCommand(await rl.question("> "));
            if (!shouldContinue) {
                break;
            }
        }
    } finally {
        rl.close();
    }
}

const [, , mode, ...args] = process.argv;
if (mode === "chapter") {
    const [chapterNumber] = args;
    if (chapterNumber === undefined) {
        console.log("请提供章节编号，例如: npm run chapter -- 1");
    } else {
        await runChapterByNumber(chapterNumber);
    }
} else if (mode === "list") {
    showChapterList();
} else if (mode === "phase") {
    showChaptersByPhase(args.join(" "));
} else {
    await runInteractive();
}
