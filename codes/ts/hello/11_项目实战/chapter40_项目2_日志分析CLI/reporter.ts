import type { LogSummary, ReportFormat } from "./types.js";

export function formatSummary(summary: LogSummary, format: ReportFormat): string {
    if (format === "json") {
        return JSON.stringify(summary, null, 2);
    }

    const lines = [
        `Total requests: ${summary.totalRequests}`,
        `Total bytes: ${summary.totalBytes}`,
        `Status 2xx: ${summary.statusCounts["2xx"]}`,
        `Status 3xx: ${summary.statusCounts["3xx"]}`,
        `Status 4xx: ${summary.statusCounts["4xx"]}`,
        `Status 5xx: ${summary.statusCounts["5xx"]}`,
        "Top paths:"
    ];

    for (const item of summary.topPaths) {
        lines.push(`  ${item.path} ${item.count}`);
    }
    return lines.join("\n");
}
