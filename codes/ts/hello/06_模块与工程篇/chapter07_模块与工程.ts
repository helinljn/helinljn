import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type ModuleStyle = "esm" | "commonjs" | "global-script";

export function explainModuleStyle(style: ModuleStyle): string {
    switch (style) {
        case "esm":
            return "modern import/export; this tutorial uses NodeNext ESM";
        case "commonjs":
            return "require/module.exports; common in older Node projects";
        case "global-script":
            return "no imports/exports; names live in the script scope";
        default: {
            const unreachable: never = style;
            return unreachable;
        }
    }
}

export function makeCompilerChecklist(): readonly string[] {
    return [
        "package.json sets type=module",
        "tsconfig uses module=NodeNext and moduleResolution=NodeNext",
        "relative ESM imports include the .js extension in TypeScript source",
        "source lives in .ts files and runtime executes dist/*.js"
    ];
}

export function runChapter(): void {
    section("第 7 章：模块系统与工程配置");
    note("C++ 对照", "`.d.ts` 更像头文件声明，但没有实现；实现仍来自 JS 文件或平台 API。");
    note("Node ESM", "本教程在 TS 源码中写 `./module.js`，由 tsc 保持输出路径一致。");

    showJson("模块风格", [
        explainModuleStyle("esm"),
        explainModuleStyle("commonjs")
    ]);
    showJson("编译检查清单", makeCompilerChecklist());
}

await runIfMain(import.meta.url, runChapter);
