// =============================================================================
// 第 4 章：字符串处理
// =============================================================================
//
// 【学习目标】
//   1. 掌握模板字面量、标签函数和多行字符串
//   2. 熟悉 slice、substring、split、replace、trim、padStart、padEnd 等常用 API
//   3. 理解 includes、startsWith、endsWith、indexOf 的搜索边界
//   4. 建立 Unicode、length、String.raw 的基础意识
//
// 【运行方式】
//   npm run build
//   node dist/01_基础篇/chapter04_字符串处理.js
//   或 npm run chapter -- 4（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type ParsedUserLine = {
    readonly id: string;
    readonly name: string;
    readonly role: string;
};

type SearchResult = {
    readonly query: string;
    readonly includes: boolean;
    readonly startsWith: boolean;
    readonly endsWith: boolean;
    readonly index: number;
};

// =============================================================================
// 4.1 字符串是不可变值
// =============================================================================
//
// C++ 对照：
//   JS string 是不可变值，和可以原地修改缓冲区的字符串模型不同。
//   方法如 trim、replace、slice 会返回新字符串，不会修改原值。
//
// 常见坑：
//   以为 `raw.trim()` 会修改 raw，结果后续仍使用带空格的旧值。
//   工程代码应把返回值保存到新变量或重新赋给 let 变量。

function demoStringImmutability(): void {
    section("4.1 字符串是不可变值");
    note("C++ 对照", "JS 字符串方法通常返回新值，不会原地修改原字符串。");

    const raw = "  TypeScript  ";
    const trimmed = raw.trim();
    const replaced = trimmed.replace("Type", "Java");

    showJson("不可变字符串示例", {
        raw,
        trimmed,
        replaced,
        rawStillHasSpaces: raw
    });
    note("输出解释", "raw 仍保留空格；trimmed 和 replaced 都是新字符串。");
}

// =============================================================================
// 4.2 模板字面量：插值和多行文本
// =============================================================================
//
// C++ 对照：
//   模板字面量比字符串拼接更接近格式化文本。
//   它不是 C++ iostream，也不是 printf；表达式会先求值再插入。
//
// 线性阅读提示：
//   复杂国际化格式化后续会在日期、日志等章节继续出现。
//   本章先用模板字面量生成稳定文本。

/** Render a deterministic CLI greeting line. */
export function renderGreeting(name: string, chapter: number): string {
    return `Hello ${name}, you are reading chapter ${chapter}.`;
}

function demoTemplateLiteral(): void {
    section("4.2 模板字面量：插值和多行文本");
    note("C++ 对照", "模板字面量适合简单格式化，但不要把复杂模板逻辑塞进一行字符串。");

    const name = "Ada";
    const chapter = 4;
    const message = renderGreeting(name, chapter);
    const configPreview = `server:
  host: localhost
  port: 3000
  mode: tutorial`;

    showJson("模板字面量输出", { message, configPreview });
    note("输出解释", "反引号字符串保留换行，${...} 中的表达式会被求值后插入。");
}

// =============================================================================
// 4.3 标签函数：拦截模板片段
// =============================================================================
//
// C++ 对照：
//   标签函数没有 C++ 直接等价物。
//   可以把它理解成模板字面量求值前的一个拦截器：接收静态片段和插值结果。
//
// 使用边界：
//   标签函数常用于 SQL、安全转义、国际化、CSS-in-JS 等场景。
//   初学阶段只需要看懂它的调用形态，不要过度使用。

function safeText(strings: TemplateStringsArray, ...values: readonly string[]): string {
    const escapeHtml = (value: string): string =>
        value
            .replaceAll("&", "&amp;")
            .replaceAll("<", "&lt;")
            .replaceAll(">", "&gt;")
            .replaceAll("\"", "&quot;");

    let output = strings[0] ?? "";
    for (let index = 0; index < values.length; index += 1) {
        output += escapeHtml(values[index] ?? "");
        output += strings[index + 1] ?? "";
    }
    return output;
}

function demoTaggedTemplate(): void {
    section("4.3 标签函数：拦截模板字面量");
    note("C++ 对照", "标签函数更像 DSL 钩子；它不是普通函数调用的语法糖那么简单。");

    const userInput = "<script>alert(1)</script>";
    const escaped = safeText`<p>${userInput}</p>`;

    showJson("标签函数转义示例", { userInput, escaped });
    note("输出解释", "插值部分被转义，静态模板片段保持原样。真实 HTML 安全仍应使用成熟库。");
}

// =============================================================================
// 4.4 slice、substring 和边界
// =============================================================================
//
// C++ 对照：
//   JS 字符串切片返回新字符串。
//   索引按 UTF-16 code unit 计算，不是用户感知字符，也不是字节偏移。
//
// 常见坑：
//   `slice` 支持负数索引，`substring` 会把负数当成 0。
//   两者参数语义不同，团队最好统一使用一种。

function demoSlicing(): void {
    section("4.4 slice 与 substring：切片边界");
    note("C++ 对照", "这里的索引不是字节偏移；它按 JS 字符串的 UTF-16 code unit 计算。");

    const text = "TypeScript";
    const examples = {
        slice0to4: text.slice(0, 4),
        sliceNegative: text.slice(-6),
        substring0to4: text.substring(0, 4),
        substringNegative: text.substring(-6)
    };

    showJson("切片示例", examples);
    note("输出解释", "slice(-6) 从倒数第 6 个 code unit 开始；substring(-6) 等价于 substring(0)。");
}

// =============================================================================
// 4.5 split、join、trim：处理输入行
// =============================================================================
//
// C++ 对照：
//   JS 标准库直接提供 split/trim/join，日常文本处理比手写循环更常见。
//   但外部输入仍需要校验字段数量和空值。
//
// 真实场景：
//   CLI、日志、配置文件经常以一行文本输入。
//   这里解析一个 `id,name,role` 格式的用户行。

/** Parse a simple comma-separated user line with validation. */
export function parseUserLine(line: string): ParsedUserLine | undefined {
    const parts = line.split(",").map((part) => part.trim());
    const [id, name, role, extra] = parts;
    if (id === undefined || name === undefined || role === undefined || extra !== undefined) {
        return undefined;
    }
    if (id === "" || name === "" || role === "") {
        return undefined;
    }
    return { id, name, role };
}

function demoSplitTrimJoin(): void {
    section("4.5 split、join、trim：处理输入行");
    note("C++ 对照", "标准方法能减少手写解析代码，但字段校验仍要明确写出。");

    const lines = ["u1, Ada Lovelace, admin", "u2, , member", "u3, Grace Hopper, reviewer, extra"];
    const parsed = lines.map((line) => ({
        line,
        parsed: parseUserLine(line)
    }));
    const command = ["npm", "run", "build"].join(" ");

    showJson("用户行解析", { parsed, command });
    note("输出解释", "第二行 name 为空，第三行字段过多，因此解析结果为 undefined。");
}

// =============================================================================
// 4.6 replace 与 replaceAll
// =============================================================================
//
// C++ 对照：
//   replace 默认只替换第一个匹配；replaceAll 替换全部字符串匹配。
//   使用正则时还要关注 g、i、u 等 flag，正则细节会在 ch33 深入。
//
// 常见坑：
//   `replace(".", "_")` 只替换第一个点。
//   如果参数是正则，`.` 又代表任意字符，含义完全不同。

/** Mask a token-like string while preserving its prefix and suffix. */
export function maskToken(token: string): string {
    if (token.length <= 8) {
        return "*".repeat(token.length);
    }
    return `${token.slice(0, 4)}${"*".repeat(token.length - 8)}${token.slice(-4)}`;
}

function demoReplace(): void {
    section("4.6 replace 与 replaceAll：替换文本");
    note("C++ 对照", "字符串替换返回新值；正则替换和普通字符串替换要分清。");

    const filename = "report.final.v1.txt";
    const once = filename.replace(".", "_");
    const all = filename.replaceAll(".", "_");
    const token = "abcd12345678wxyz";

    showJson("替换示例", {
        filename,
        replaceOnce: once,
        replaceAll: all,
        maskedToken: maskToken(token)
    });
    note("输出解释", "replace 只处理第一个点；replaceAll 才把所有点替换掉。");
}

// =============================================================================
// 4.7 查找：includes、startsWith、endsWith、indexOf
// =============================================================================
//
// C++ 对照：
//   这些 API 负责简单子串查找，语义比手写循环清晰。
//   `indexOf` 找不到时返回 -1，这和许多 C/C++ API 的 sentinel 习惯类似。
//
// 工程建议：
//   需要 boolean 时优先用 includes/startsWith/endsWith。
//   需要位置时再用 indexOf。

/** Run common substring checks against a text. */
export function searchText(text: string, query: string): SearchResult {
    return {
        query,
        includes: text.includes(query),
        startsWith: text.startsWith(query),
        endsWith: text.endsWith(query),
        index: text.indexOf(query)
    };
}

function demoSearch(): void {
    section("4.7 查找 API：选择表达意图的方法");
    note("C++ 对照", "需要布尔结果时直接用 includes，比 indexOf(...) >= 0 更能表达意图。");

    const text = "GET /api/users HTTP/1.1";
    const results = ["/api", "POST", "HTTP/1.1"].map((query) => searchText(text, query));

    showJson("查找结果", { text, results });
    note("输出解释", "POST 的 index 是 -1，表示没有找到。");
}

// =============================================================================
// 4.8 padStart、padEnd：固定宽度输出
// =============================================================================
//
// C++ 对照：
//   这类方法类似格式化输出中的宽度控制。
//   它们按字符串长度补齐，不做数字四舍五入。
//
// 真实场景：
//   CLI 表格、日志级别、序号常需要固定宽度，方便人眼扫描。

function demoPadding(): void {
    section("4.8 padStart/padEnd：固定宽度输出");
    note("C++ 对照", "可以把它理解为轻量格式化宽度控制，但它只处理字符串补齐。");

    const rows = [
        { id: 1, level: "info", message: "started" },
        { id: 12, level: "warn", message: "slow query" },
        { id: 305, level: "error", message: "failed" }
    ];

    const table = rows.map((row) => `${String(row.id).padStart(4, "0")} ${row.level.padEnd(5)} ${row.message}`);

    showJson("固定宽度日志行", table);
    note("输出解释", "id 用 0 补齐到 4 位，level 用空格补齐到 5 位。");
}

// =============================================================================
// 4.9 Unicode 与 length 的坑
// =============================================================================
//
// C++ 对照：
//   `std::string::size()` 常表示字节数，JS `.length` 表示 UTF-16 code unit 数。
//   两者都不一定等于用户看到的字符数。
//
// 常见坑：
//   emoji、组合字符、某些 CJK 扩展字符会让 length 和用户感知字符数不一致。
//   本章只做入门提醒，复杂文本处理应使用成熟 Unicode 工具或 Intl 能力。

function demoUnicodeLength(): void {
    section("4.9 Unicode 与 .length 的坑");
    note("C++ 对照", "不要把 JS length 当作字节数，也不要直接当作用户感知字符数。");

    const samples = ["A", "中", "😀", "e\u0301"];
    const lengths = samples.map((text) => ({
        text,
        codeUnitLength: text.length,
        codePointCount: Array.from(text).length,
        utf8Bytes: Buffer.byteLength(text, "utf8")
    }));

    showJson("Unicode 长度对比", lengths);
    note("输出解释", "emoji 的 code unit 长度通常是 2；组合字符也可能让用户感知和计数不同。");
}

// =============================================================================
// 4.10 String.raw：保留反斜杠
// =============================================================================
//
// C++ 对照：
//   String.raw 接近原始字符串字面量的用途：减少转义干扰。
//   但它是 JS 的标签函数，不是 C++ raw string literal 的同一机制。

function demoStringRaw(): void {
    section("4.10 String.raw：保留反斜杠");
    note("C++ 对照", "String.raw 适合展示路径或正则片段，但跨平台路径仍应使用 path 模块。");

    const normal = "C:\\temp\\demo";
    const raw = String.raw`C:\temp\demo`;
    const regexSnippet = String.raw`\d{4}-\d{2}-\d{2}`;

    showJson("String.raw 示例", { normal, raw, regexSnippet });
    note("输出解释", "raw 模板中反斜杠不触发普通字符串转义，适合展示正则片段。");
}

// =============================================================================
// 4.11 工程场景：清洗日志行
// =============================================================================
//
// C++ 对照：
//   日志清洗常见于服务端工具。
//   与 C++ 一样，文本清洗要保持规则明确、输出稳定、不要误删业务信息。
//
// 真实场景：
//   去掉多余空白、脱敏 token、统一日志级别大小写。

/** Clean a compact log line without relying on external state. */
export function sanitizeLogLine(line: string): string {
    const compact = line.trim().replace(/\s+/g, " ");
    const masked = compact.replace(/token=[A-Za-z0-9_-]+/g, "token=***");
    return masked.replace(/\[(info|warn|error)\]/i, (_match, level: string) => `[${level.toUpperCase()}]`);
}

function demoSanitizeLogLine(): void {
    section("4.11 工程场景：清洗日志行");
    note("C++ 对照", "文本清洗应明确输入输出规则；不要让正则变成不可审查的黑盒。");

    const lines = [
        "  [info]   user login   token=abc123XYZ  ",
        "[WARN] disk almost full token=secret_001",
        "[error] failed without token"
    ];
    const cleaned = lines.map((line) => sanitizeLogLine(line));

    showJson("日志清洗结果", { lines, cleaned });
    note("输出解释", "多余空白被压缩，token 被脱敏，日志级别统一成大写。");
}

// =============================================================================
// 4.12 本章复盘
// =============================================================================
//
// C++ 对照：
//   JS 字符串处理 API 很丰富，但索引、Unicode、正则和运行时安全边界需要重新建立。

function demoChapterReview(): void {
    section("4.12 本章复盘");
    note("C++ 对照", "不要把 JS 字符串当作字节数组；多数 API 都返回新字符串。");

    const review = [
        "字符串不可变，trim/replace/slice 返回新值",
        "模板字面量适合插值和多行文本",
        "标签函数能拦截模板片段和插值值",
        "slice 支持负数，substring 会把负数当成 0",
        "replace 默认只替换第一个匹配，replaceAll 替换全部字符串匹配",
        "需要 boolean 查找结果时优先 includes/startsWith/endsWith",
        ".length 是 UTF-16 code unit 数，不是字节数或用户感知字符数",
        "String.raw 适合展示反斜杠密集的文本"
    ];

    showJson("关键结论", review);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 4. */
export function runChapter(): void {
    demoStringImmutability();
    demoTemplateLiteral();
    demoTaggedTemplate();
    demoSlicing();
    demoSplitTrimJoin();
    demoReplace();
    demoSearch();
    demoPadding();
    demoUnicodeLength();
    demoStringRaw();
    demoSanitizeLogLine();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
