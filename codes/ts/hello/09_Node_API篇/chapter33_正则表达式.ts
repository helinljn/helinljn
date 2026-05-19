// =============================================================================
// 第 33 章：正则表达式
// =============================================================================
//
// 【学习目标】
//   1. 掌握 RegExp 的创建方式、常用标志、匹配方法和字符串集成方法
//   2. 理解字符类、量词、分组、反向引用、前瞻和后顾断言的边界
//   3. 能把正则用于邮箱、URL、日志解析等工程场景，并知道何时不该用正则
//
// 【运行方式】
//   npm run build
//   node dist/09_Node_API篇/chapter33_正则表达式.js
//   或 npm run chapter -- 33（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type LogRecord = {
    timestamp: string;
    level: "INFO" | "WARN" | "ERROR";
    message: string;
};

// =============================================================================
// 33.1 创建 RegExp：字面量 vs new RegExp()
// =============================================================================
//
// C++ 对照：JS RegExp 语法和 std::regex 的 ECMAScript 风格相近；字面量像把 pattern
// 写进源码，构造函数适合动态拼接，但字符串转义会多一层。
//
// C++ 思维提示：正则首先是运行时值。TypeScript 能检查 RegExp 对象类型，但不会证明
// pattern 一定符合你的业务语义。

function demoCreateRegExp(): void {
    section("33.1 创建 RegExp：字面量 vs new RegExp()");
    note("C++ 对照", "字面量适合固定 pattern；构造函数适合动态 pattern，但要注意字符串双重转义。");

    const literal = /\d{4}-\d{2}-\d{2}/;
    const constructor = new RegExp("\\d{4}-\\d{2}-\\d{2}");
    const badConstructor = new RegExp("\d{4}-\d{2}-\d{2}");
    const sample = "release: 2026-05-19";

    showJson("创建方式对比", {
        sample,
        literalMatches: literal.test(sample),
        constructorMatches: constructor.test(sample),
        badConstructorSource: badConstructor.source,
        badConstructorMatches: badConstructor.test(sample)
    });
    note("常见坑", "new RegExp(\"\\d+\") 里要写成 \"\\\\d+\"；普通字符串会先吃掉一层反斜杠。");
}

// =============================================================================
// 33.2 常用标志：g、i、m、s、u、d
// =============================================================================
//
// C++ 对照：flags 类似给 regex engine 传匹配模式选项；它们会改变匹配范围、大小写、
// 换行处理、Unicode 语义和返回信息。
//
// C++ 思维提示：带 g 的 RegExp 有可变 lastIndex 状态；把同一个全局正则复用在 test()
// 中，可能出现隔次 true/false 的错觉。

function demoFlags(): void {
    section("33.2 常用标志：g、i、m、s、u、d");
    note("C++ 对照", "flags 决定 regex engine 如何解释输入和返回结果。");

    const text = "User: Ada\nuser: Lin\nblock:<a>\n<b>";
    const globalInsensitive = [...text.matchAll(/user:\s*(\w+)/gi)].map((match) => match[1] ?? "<missing>");
    const multiline = /^user:\s*(\w+)/gim.exec(text)?.[1] ?? "<no match>";
    const dotAll = /block:<(?<content>.*)>/s.exec(text)?.groups?.content?.replaceAll("\n", "\\n") ?? "<no match>";
    const unicode = /^.$/u.test("文");
    const withIndices = /(?<kind>[a-z]+)-(?<id>\d+)/d.exec("task-42");

    showJson("flags 示例", {
        g_i: globalInsensitive,
        m: multiline,
        s: dotAll,
        u: unicode,
        d: {
            match: withIndices?.[0],
            wholeRange: withIndices?.indices?.[0],
            idRange: withIndices?.indices?.groups?.id
        }
    });
    note("版本边界", "d flag 返回 indices，当前 ES2023 lib/Node 24 可用；旧运行时需要 feature check。");
}

// =============================================================================
// 33.3 test 与 exec：布尔判断和结构化匹配
// =============================================================================
//
// C++ 对照：test 像只问是否匹配，exec 像拿到 match object、分组和匹配位置。
//
// C++ 思维提示：全局正则的 lastIndex 是可变状态。需要独立判断时，要么不用 g，要么手动
// 重置 lastIndex。

function demoTestAndExec(): void {
    section("33.3 test 与 exec：布尔判断和结构化匹配");
    note("C++ 对照", "test 返回 boolean；exec 返回匹配细节，包含捕获组和 index。");

    const input = "ticket TS-1024 assigned";
    const ticketPattern = /(?<prefix>[A-Z]{2})-(?<id>\d+)/;
    const match = ticketPattern.exec(input);

    const stateful = /\d/g;
    const first = stateful.test("1");
    const second = stateful.test("1");
    stateful.lastIndex = 0;
    const afterReset = stateful.test("1");

    showJson("test 与 exec", {
        test: ticketPattern.test(input),
        exec: {
            whole: match?.[0],
            index: match?.index,
            prefix: match?.groups?.prefix,
            id: match?.groups?.id
        },
        globalRegexState: {
            first,
            second,
            afterReset,
            lesson: "g regex keeps lastIndex"
        }
    });
    note("常见坑", "不要把带 g 的同一个 RegExp 实例当作无状态 predicate 到处复用。");
}

// =============================================================================
// 33.4 String 方法：match、matchAll、replace、search、split
// =============================================================================
//
// C++ 对照：这些方法像把 regex engine 接入字符串工具链；不同方法返回的数据形状不同。
//
// C++ 思维提示：matchAll 返回 iterator，适合遍历所有命名分组；replace callback 适合
// 做轻量文本转换，但不要把复杂解析器塞进一个正则替换里。

function demoStringMethods(): void {
    section("33.4 String 方法：match、matchAll、replace、search、split");
    note("C++ 对照", "String 方法把正则作为搜索、切分和替换策略。");

    const text = "alpha=1; beta=20; gamma=300";
    const keyValue = /(?<key>[a-z]+)=(?<value>\d+)/g;
    const allPairs = [...text.matchAll(keyValue)].map((match) => ({
        key: match.groups?.key ?? "<missing>",
        value: Number(match.groups?.value ?? 0)
    }));
    const replaced = text.replace(keyValue, (_whole, _key: string, _value: string, _offset: number, _input: string, groups?: { key?: string; value?: string }) => {
        return `${groups?.key?.toUpperCase() ?? "UNKNOWN"}:${groups?.value ?? "0"}`;
    });

    showJson("String + RegExp", {
        matchFirstNumber: text.match(/\d+/)?.[0],
        matchAllPairs: allPairs,
        replace: replaced,
        searchBeta: text.search(/beta=\d+/),
        split: text.split(/;\s*/)
    });
    note("输出解释", "matchAll 保留每次匹配的分组；split 用正则描述分隔符，而不是只按固定字符串切。");
}

// =============================================================================
// 33.5 字符类：\d、\w、\s、自定义类与排除类
// =============================================================================
//
// C++ 对照：字符类像把可接受字符集合写成一个小型谓词；比手写多个 if 简洁。
//
// C++ 思维提示：\w 在 JS 中主要覆盖 ASCII 单词字符，不等于“所有语言的文字”。处理国际化
// 用户名时不要直接套 \w+。

function demoCharacterClasses(): void {
    section("33.5 字符类：\\d、\\w、\\s、自定义类与排除类");
    note("C++ 对照", "字符类描述一个位置允许哪些字符。");

    const input = "id: A_42, status=ready, note=[ok]";
    showJson("字符类", {
        digits: input.match(/\d+/)?.[0],
        word: input.match(/\w+/)?.[0],
        whitespaceCount: input.match(/\s/g)?.length ?? 0,
        customClass: input.match(/[A-Z_0-9]+/)?.[0],
        negatedClass: input.match(/note=\[([^\]]+)\]/)?.[1]
    });
    note("常见坑", "正则里的字符类也要转义特殊字符；比如 ]、-、\\ 在不同位置有不同含义。");
}

// =============================================================================
// 33.6 量词：贪婪与懒惰
// =============================================================================
//
// C++ 对照：量词控制重复次数；默认贪婪会尽量多吃字符，懒惰量词会尽量少吃但仍满足整体匹配。
//
// C++ 思维提示：.* 很容易跨过你以为的边界。解析嵌套 HTML/XML/JSON 时，正则通常不是正确工具。

function demoQuantifiers(): void {
    section("33.6 量词：贪婪与懒惰");
    note("C++ 对照", "*, +, ?, {n}, {n,m} 都是在描述重复次数。");

    const html = "<b>first</b><b>second</b>";
    const greedy = /<b>.*<\/b>/.exec(html)?.[0] ?? "<no match>";
    const lazy = /<b>.*?<\/b>/.exec(html)?.[0] ?? "<no match>";

    showJson("量词对比", {
        zeroOrMore: /^ab*c$/.test("ac"),
        oneOrMore: /^ab+c$/.test("abbbc"),
        optional: /^colou?r$/.test("color"),
        exactRange: /^\d{2,4}$/.test("2026"),
        greedy,
        lazy
    });
    note("常见坑", "贪婪不是错误，但要知道它会尽量扩大匹配；需要最短匹配时用 *?、+?、{n,m}?。");
}

// =============================================================================
// 33.7 分组、命名捕获、非捕获与反向引用
// =============================================================================
//
// C++ 对照：捕获组像 regex match 的子匹配；命名捕获比靠数字下标更适合长期维护。
//
// C++ 思维提示：非捕获组 (?:...) 用于组织 pattern，不增加捕获下标，能减少后续维护时
// 分组编号错位。

function demoGroupsAndBackreferences(): void {
    section("33.7 分组、命名捕获、非捕获与反向引用");
    note("C++ 对照", "命名捕获让 match 结果更像结构化对象，而不是裸数组下标。");

    const date = /(?<year>\d{4})-(?<month>\d{2})-(?<day>\d{2})/.exec("2026-05-19");
    const duplicateWord = /\b(?<word>[a-z]+)\s+\k<word>\b/i.exec("too too much");
    const normalizedPhone = "tel: 010-12345678".replace(/(?:tel:\s*)?(?<area>\d{3})-(?<number>\d{8})/, "($<area>) $<number>");

    showJson("分组与反向引用", {
        namedDate: date?.groups,
        duplicateWord: duplicateWord?.groups?.word,
        nonCapturingForStructure: normalizedPhone
    });
    note("输出解释", "\\k<word> 要求后面出现和命名组 word 相同的文本，可用于发现重复词这类模式。");
}

// =============================================================================
// 33.8 断言：前瞻、负前瞻、后顾、负后顾
// =============================================================================
//
// C++ 对照：断言像零宽条件检查；它们判断当前位置前后是否满足条件，但不消耗字符。
//
// C++ 思维提示：断言让 pattern 更紧凑，但可读性会下降。业务规则复杂时，先拆成多个
// 清晰的 if/validator，别把所有逻辑塞进一个正则。

function demoAssertions(): void {
    section("33.8 断言：前瞻、负前瞻、后顾、负后顾");
    note("C++ 对照", "断言检查上下文，不把上下文本身放进匹配结果。");

    const input = "price: $42, discount: 10%, code: TS2026";
    showJson("断言示例", {
        positiveLookahead: input.match(/\d+(?=%)/)?.[0],
        negativeLookahead: input.match(/\b(?!TS)\w+\d+\b/)?.[0] ?? "<no match>",
        positiveLookbehind: input.match(/(?<=\$)\d+/)?.[0],
        negativeLookbehind: input.match(/(?<!\$)\b\d{2}\b/)?.[0]
    });
    note("版本边界", "JS 原生支持后顾断言已久，但非常旧的运行时可能不支持；本教程 Node 24 可运行。");
}

// =============================================================================
// 33.9 实战案例：邮箱、URL 与日志字段解析
// =============================================================================
//
// C++ 对照：正则适合解析格式有限、边界明确的小文本；不适合替代完整协议解析器。
//
// C++ 思维提示：外部输入先经过 runtime validation，再变成业务类型。正则匹配成功只是第一步，
// 后续仍可能需要长度、范围、白名单和语义校验。

function isSimpleEmail(value: string): boolean {
    return /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(value);
}

function extractUrls(text: string): string[] {
    return [...text.matchAll(/\bhttps?:\/\/[^\s<>"']+/g)].map((match) => match[0].replace(/[.,;:!?]+$/u, ""));
}

function parseLogRecord(line: string): LogRecord | { error: string; line: string } {
    const match = /^(?<timestamp>\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}Z)\s+(?<level>INFO|WARN|ERROR)\s+(?<message>.+)$/.exec(line);
    if (match?.groups === undefined) {
        return { error: "invalid log line", line };
    }
    return {
        timestamp: match.groups.timestamp ?? "",
        level: (match.groups.level ?? "INFO") as LogRecord["level"],
        message: match.groups.message ?? ""
    };
}

function demoPracticalCases(): void {
    section("33.9 实战案例：邮箱、URL 与日志字段解析");
    note("C++ 对照", "正则可作为文本边界的第一层校验，但不是完整业务协议。");

    const emails = ["dev@example.com", "bad@@example", "name with space@example.com"];
    const article = "Docs: https://example.com/guide and http://localhost:3000/health.";
    const logs = [
        "2026-05-19T08:30:00Z INFO service started",
        "2026-05-19T08:31:00Z WARN retry scheduled",
        "bad line"
    ];

    showJson("工程正则", {
        emailValidation: emails.map((email) => ({ email, valid: isSimpleEmail(email) })),
        extractedUrls: extractUrls(article),
        parsedLogs: logs.map(parseLogRecord)
    });
    note("常见坑", "邮箱完整 RFC 规则很复杂；工程上常用简单格式校验 + 发送验证邮件，而不是追求一个巨型正则。");
}

// =============================================================================
// 33.10 正则边界：可读性、性能和安全
// =============================================================================
//
// C++ 对照：像 std::regex 一样，复杂 pattern 可能难维护，也可能在特定输入上性能很差。
//
// C++ 思维提示：不要让用户可控输入进入高风险正则；避免灾难性回溯，给输入长度设限，
// 复杂语法用 parser。

function demoRegexBoundaries(): void {
    section("33.10 正则边界：可读性、性能和安全");
    note("C++ 对照", "正则是强工具，但不是所有文本问题都应该交给 regex engine。");

    showJson("正则使用 checklist", [
        "固定、短小、边界明确的文本格式适合正则",
        "JSON、HTML、编程语言、嵌套语法应使用 parser",
        "外部输入先限制长度，再运行复杂 pattern",
        "避免 (a+)+ 这类容易灾难性回溯的结构",
        "命名捕获比数字下标更适合长期维护",
        "正则匹配成功后仍要做业务语义校验"
    ]);
    note("本章复盘", "RegExp 是运行时文本工具；TypeScript 能描述结果类型，但不能替代输入校验和性能边界。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 33 demos. */
export function runChapter(): void {
    demoCreateRegExp();
    demoFlags();
    demoTestAndExec();
    demoStringMethods();
    demoCharacterClasses();
    demoQuantifiers();
    demoGroupsAndBackreferences();
    demoAssertions();
    demoPracticalCases();
    demoRegexBoundaries();
}

await runIfMain(import.meta.url, runChapter);
