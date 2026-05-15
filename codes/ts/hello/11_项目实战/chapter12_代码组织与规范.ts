import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export type LayerAdvice = {
    layer: string;
    responsibility: string;
};

export function suggestProjectLayers(): LayerAdvice[] {
    return [
        { layer: "types", responsibility: "只放公共数据结构和联合类型" },
        { layer: "parser/config", responsibility: "把外部输入转换成明确类型" },
        { layer: "domain", responsibility: "实现可测试的核心业务逻辑" },
        { layer: "cli/main", responsibility: "处理进程参数、打印结果和退出码" },
        { layer: "tests", responsibility: "验证核心函数和最小 CLI 流程" }
    ];
}

export function runChapter(): void {
    section("第 12 章：项目实战前的代码组织与规范");
    note("C++ 对照", "TS 项目通常通过模块边界组织代码，而不是头文件/源文件分离。");
    note("实践原则", "把可测试逻辑放在纯函数或类里，CLI 入口只做参数和 I/O 编排。");
    showJson("推荐分层", suggestProjectLayers());
}

await runIfMain(import.meta.url, runChapter);
