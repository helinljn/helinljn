// =============================================================================
// 第 32 章：日期与时间
// =============================================================================
//
// 【学习目标】
//   1. 掌握 Date 的创建、时间戳、字段读取、格式化和基础计算
//   2. 理解月份从 0 开始、本地时区、Date.parse 和 DST 的常见边界
//   3. 知道 date-fns、luxon 与 Temporal 的定位，明确本教程当前可运行边界
//
// 【运行方式】
//   npm run build
//   node dist/09_Node_API篇/chapter32_日期与时间.js
//   或 npm run chapter -- 32（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

const MS_PER_SECOND = 1000;
const MS_PER_MINUTE = 60 * MS_PER_SECOND;
const MS_PER_HOUR = 60 * MS_PER_MINUTE;
const MS_PER_DAY = 24 * MS_PER_HOUR;

type TemporalLike = {
    Now?: {
        plainDateISO?: () => { toString(): string };
    };
    Duration?: {
        from?: (input: { days?: number; hours?: number }) => { toString(): string };
    };
};

// =============================================================================
// 32.1 Date 对象：本质是一个毫秒时间戳包装器
// =============================================================================
//
// C++ 对照：Date 接近 std::chrono::system_clock::time_point 的运行时包装，但类型没有 chrono
// 那么细；Date 本身可变，内部值是从 Unix epoch 起算的毫秒数。
//
// C++ 思维提示：Date 对象既能表示一个时间点，也能暴露本地时区字段。时间点和日历字段
// 是两层概念，不要混在一起推理。

function demoDateBasics(): void {
    section("32.1 Date 对象：本质是一个毫秒时间戳包装器");
    note("C++ 对照", "Date 内部是 epoch milliseconds；字段读取会套用本地时区或 UTC 方法。");

    const localDate = new Date(2026, 0, 15, 10, 30, 0);
    const utcDate = new Date(Date.UTC(2026, 0, 15, 2, 30, 0));
    const now = Date.now();

    showJson("Date 基础形状", {
        localConstructor: {
            year: localDate.getFullYear(),
            monthFromZero: localDate.getMonth(),
            humanMonth: localDate.getMonth() + 1,
            day: localDate.getDate(),
            hour: localDate.getHours()
        },
        utcConstructor: {
            iso: utcDate.toISOString(),
            epochMilliseconds: utcDate.getTime()
        },
        dateNow: {
            unit: "milliseconds since Unix epoch",
            isInteger: Number.isInteger(now),
            valuePrinted: false
        }
    });
    note("常见坑", "getMonth() 返回 0-11；给用户显示月份时通常要 +1。");
}

// =============================================================================
// 32.2 本地时区与 UTC：同一时间点，两套字段视图
// =============================================================================
//
// C++ 对照：system_clock::time_point 是时间点；把它格式化成某个时区的年月日是另一层。
//
// C++ 思维提示：toISOString 永远输出 UTC；getFullYear/getHours 使用本地时区；
// getUTCFullYear/getUTCHours 使用 UTC 字段。

function demoLocalVsUtc(): void {
    section("32.2 本地时区与 UTC：同一时间点，两套字段视图");
    note("C++ 对照", "时间点和时区格式化是两件事，类似 chrono time_point 与 zoned_time 的区别。");

    const instant = new Date("2026-05-19T12:00:00.000Z");

    showJson("本地字段与 UTC 字段", {
        instantIso: instant.toISOString(),
        localFieldsDependOnRuntimeTimeZone: true,
        localFieldMethods: ["getFullYear()", "getMonth()", "getDate()", "getHours()"],
        utcFields: {
            year: instant.getUTCFullYear(),
            monthFromZero: instant.getUTCMonth(),
            day: instant.getUTCDate(),
            hour: instant.getUTCHours()
        }
    });
    note("输出解释", "同一个 Date 的内部时间点不变；不同字段方法只是套用了不同日历视图。");
    note("常见坑", "服务端日志、接口和数据库建议存 ISO UTC；展示给用户时再按用户时区格式化。");
}

// =============================================================================
// 32.3 Date.parse：只信 ISO 8601，避免模糊字符串
// =============================================================================
//
// C++ 对照：日期字符串解析像协议解析；格式必须明确，否则不同运行时可能给出不同解释。
//
// C++ 思维提示：Date.parse 对 ISO 8601 字符串可靠得多；“05/06/2026”这种格式到底是
// 五月六日还是六月五日，取决于地区和解析器习惯。

function demoDateParse(): void {
    section("32.3 Date.parse：只信 ISO 8601，避免模糊字符串");
    note("C++ 对照", "解析外部日期字符串要约定协议格式，不要靠运行时猜。");

    const iso = "2026-05-19T08:30:00.000Z";
    const parsed = new Date(Date.parse(iso));

    showJson("Date.parse 边界", {
        safeInput: iso,
        parsedIso: parsed.toISOString(),
        ambiguousExamplesToAvoid: ["05/06/2026", "19 May 26", "2026/05/19"],
        recommendedWireFormat: "ISO 8601 with timezone, e.g. 2026-05-19T08:30:00.000Z"
    });
    note("类型边界", "TypeScript 只能知道输入是 string；字符串是否是合法日期要运行时校验。");
}

// =============================================================================
// 32.4 格式化：toLocaleString 与 Intl.DateTimeFormat
// =============================================================================
//
// C++ 对照：格式化类似把 time_point 交给 locale/time zone 规则生成用户可读字符串。
//
// C++ 思维提示：同一时间点在不同 locale/timeZone 下展示不同。API 响应用机器可读格式，
// UI 再用 Intl.DateTimeFormat 展示。

function demoFormatting(): void {
    section("32.4 格式化：toLocaleString 与 Intl.DateTimeFormat");
    note("C++ 对照", "Intl.DateTimeFormat 是标准库级的本地化格式化器。");

    const instant = new Date("2026-12-31T23:30:00.000Z");
    const shanghai = new Intl.DateTimeFormat("zh-CN", {
        timeZone: "Asia/Shanghai",
        dateStyle: "full",
        timeStyle: "short"
    });
    const newYork = new Intl.DateTimeFormat("en-US", {
        timeZone: "America/New_York",
        dateStyle: "full",
        timeStyle: "short"
    });

    showJson("格式化同一时间点", {
        isoForStorage: instant.toISOString(),
        toLocaleStringExample: instant.toLocaleString("en-CA", { timeZone: "UTC" }),
        intl: {
            shanghai: shanghai.format(instant),
            newYork: newYork.format(instant)
        }
    });
    note("工程习惯", "频繁格式化时复用 Intl.DateTimeFormat 实例，不要在循环里反复构造。");
}

// =============================================================================
// 32.5 日期计算：毫秒加减简单，但日历语义不简单
// =============================================================================
//
// C++ 对照：毫秒加减像 duration 运算；“加一天”在日历语义中不一定等于本地墙上时间加 24 小时。
//
// C++ 思维提示：到期时间、缓存 TTL、重试延迟适合用毫秒 duration；账单日、自然月、工作日
// 应使用日历语义，避免手写毫秒近似。

function demoDateMath(): void {
    section("32.5 日期计算：毫秒加减简单，但日历语义不简单");
    note("C++ 对照", "timestamp + duration 很直接；日历日期计算需要额外规则。");

    const start = new Date("2026-05-19T00:00:00.000Z");
    const ttl = 90 * MS_PER_MINUTE;
    const expiresAt = new Date(start.getTime() + ttl);
    const tomorrowByMs = new Date(start.getTime() + MS_PER_DAY);

    showJson("时间戳计算", {
        start: start.toISOString(),
        ttlMinutes: ttl / MS_PER_MINUTE,
        expiresAt: expiresAt.toISOString(),
        tomorrowByMs: tomorrowByMs.toISOString(),
        warning: "millisecond arithmetic is duration arithmetic, not full calendar arithmetic"
    });
    note("常见坑", "Date 是可变对象；setDate/setMonth 会修改原对象，工程代码中常先复制再改。");
}

// =============================================================================
// 32.6 DST：夏令时让本地墙上时间跳变
// =============================================================================
//
// C++ 对照：DST 类似时区数据库里的规则变化；它不是固定 offset。
//
// C++ 思维提示：跨 DST 的“加 1 小时”是时间点加 3600000ms，但本地墙上时间可能跳过或重复
// 某些小时。不要用固定 offset 模拟时区。

function formatInZone(date: Date, timeZone: string): string {
    return new Intl.DateTimeFormat("en-US", {
        timeZone,
        hour: "2-digit",
        minute: "2-digit",
        hourCycle: "h23",
        timeZoneName: "short"
    }).format(date);
}

function demoDstPitfall(): void {
    section("32.6 DST：夏令时让本地墙上时间跳变");
    note("C++ 对照", "DST 是时区规则，不是 Date 类型自动变聪明。");

    const before = new Date("2026-03-08T06:30:00.000Z");
    const afterOneHour = new Date(before.getTime() + MS_PER_HOUR);

    showJson("America/New_York DST 跳变", {
        beforeUtc: before.toISOString(),
        afterOneHourUtc: afterOneHour.toISOString(),
        beforeNewYork: formatInZone(before, "America/New_York"),
        afterNewYork: formatInZone(afterOneHour, "America/New_York"),
        lesson: "one real hour later can look like two wall-clock hours later during spring-forward"
    });
    note("常见坑", "预约、排班、账单这类本地日历问题，应显式使用时区规则和可靠库/Temporal，而不是手写 offset。");
}

// =============================================================================
// 32.7 第三方库：date-fns 与 luxon 的定位
// =============================================================================
//
// C++ 对照：这类似选择 chrono 扩展库或时区库。标准 Date 能做基础事，但复杂日历规则需要
// 更明确的抽象。
//
// C++ 思维提示：引入日期库不是为了“格式化更漂亮”，而是为了把日历计算、时区、解析策略
// 和不可变数据模型写得更可测试。

function demoThirdPartyLibraries(): void {
    section("32.7 第三方库：date-fns 与 luxon 的定位");
    note("C++ 对照", "复杂时间业务通常需要比裸 Date 更强的日历和时区抽象。");

    showJson("库选择边界", [
        {
            library: "date-fns",
            style: "函数式、按函数导入、适合日期加减/格式化等轻量任务",
            boundary: "时区能力需要配套方案，仍要明确输入输出时区"
        },
        {
            library: "luxon",
            style: "围绕 DateTime/Duration/Interval 建模，时区表达更清晰",
            boundary: "多一层对象模型，适合时区密集型业务"
        },
        {
            library: "none",
            style: "只用 Date + Intl",
            boundary: "适合时间戳、TTL、简单展示；不适合复杂日历规则"
        }
    ]);
    note("工程习惯", "团队应统一日期库和 wire format；不要在同一项目里混用多套日期语义。");
}

// =============================================================================
// 32.8 Temporal：当前教程的可运行边界
// =============================================================================
//
// C++ 对照：Temporal 更接近把 time_point、plain date、duration、time zone 分成不同类型；
// 这是 Date 长期缺陷的现代修正方向。
//
// C++ 思维提示：本项目 tsconfig 使用 lib: ES2023/DOM，没有 esnext.temporal 类型；
// 本地 Node 24.15.0 也没有 globalThis.Temporal。默认章节不能直接调用 Temporal。

function getTemporalLike(): TemporalLike | undefined {
    const value = (globalThis as { Temporal?: unknown }).Temporal;
    if (typeof value !== "object" || value === null) {
        return undefined;
    }
    return value as TemporalLike;
}

function demoTemporalBoundary(): void {
    section("32.8 Temporal：当前教程的可运行边界");
    note("C++ 对照", "Temporal 把时间点、日期、时区和 duration 分开建模，方向上更接近 chrono 的类型安全。");

    const temporal = getTemporalLike();
    const hasPlainDate = typeof temporal?.Now?.plainDateISO === "function";
    const hasDuration = typeof temporal?.Duration?.from === "function";

    showJson("Temporal feature-detect", {
        runtimeTemporalAvailable: temporal !== undefined,
        hasTemporalNowPlainDateISO: hasPlainDate,
        hasTemporalDurationFrom: hasDuration,
        tutorialBoundary: {
            tsconfigLib: "ES2023 + DOM, no esnext.temporal",
            runnableDefault: "do not call Temporal on the default path",
            exampleSyntaxOnly: [
                "Temporal.Now.plainDateISO()",
                "Temporal.Duration.from({ days: 1 })"
            ]
        }
    });
    note("版本边界", "要在项目中使用 Temporal，需确认运行时或 polyfill，并把类型来源纳入 tsconfig/依赖管理。");
    note("本章复盘", "Date 足够处理时间点和简单展示；复杂本地日历、时区和 DST 需要更明确的抽象。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 32 demos. */
export function runChapter(): void {
    demoDateBasics();
    demoLocalVsUtc();
    demoDateParse();
    demoFormatting();
    demoDateMath();
    demoDstPitfall();
    demoThirdPartyLibraries();
    demoTemporalBoundary();
}

await runIfMain(import.meta.url, runChapter);
