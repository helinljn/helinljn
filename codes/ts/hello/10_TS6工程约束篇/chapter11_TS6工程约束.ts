import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export const TYPESCRIPT_VERSION_RANGE = "~6.0.3";

export type TopicBoundary = {
    topic: string;
    belongsTo: "TypeScript" | "JavaScript" | "Node.js" | "Web API";
};

export function classifyTopic(topic: string): TopicBoundary {
    const lower = topic.toLowerCase();
    if (lower.includes("generic") || lower.includes("interface") || lower.includes("type")) {
        return { topic, belongsTo: "TypeScript" };
    }
    if (lower.includes("promise") || lower.includes("prototype") || lower.includes("closure")) {
        return { topic, belongsTo: "JavaScript" };
    }
    if (lower.includes("fs") || lower.includes("buffer") || lower.includes("http")) {
        return { topic, belongsTo: "Node.js" };
    }
    return { topic, belongsTo: "Web API" };
}

export function runChapter(): void {
    section("第 11 章：TypeScript 6.0 工程约束");
    note("版本锁定", `package.json 使用 ${TYPESCRIPT_VERSION_RANGE}，只接收 6.0 补丁版本。`);
    note("内容边界", "TS 6.0 是本教程的编译器版本边界，不把 TS 7 Beta 行为写成默认规则。");

    showJson("主题归属", [
        classifyTopic("generic constraints"),
        classifyTopic("Promise event loop"),
        classifyTopic("node:http server"),
        classifyTopic("fetch Request")
    ]);
}

await runIfMain(import.meta.url, runChapter);
