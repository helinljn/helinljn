import type { LogEntry, LogSummary, StatusBucket } from "./types.js";

export function bucketStatus(status: number): StatusBucket {
  if (status >= 200 && status < 300) {
    return "2xx";
  }
  if (status >= 300 && status < 400) {
    return "3xx";
  }
  if (status >= 400 && status < 500) {
    return "4xx";
  }
  if (status >= 500 && status < 600) {
    return "5xx";
  }
  return "other";
}

export function summarizeLogs(entries: readonly LogEntry[], top = 5): LogSummary {
  const statusCounts: Record<StatusBucket, number> = {
    "2xx": 0,
    "3xx": 0,
    "4xx": 0,
    "5xx": 0,
    other: 0
  };
  const pathCounts = new Map<string, number>();
  let totalBytes = 0;

  for (const entry of entries) {
    statusCounts[bucketStatus(entry.status)] += 1;
    pathCounts.set(entry.path, (pathCounts.get(entry.path) ?? 0) + 1);
    totalBytes += entry.bytes;
  }

  const topPaths = [...pathCounts.entries()]
    .sort((left, right) => right[1] - left[1] || left[0].localeCompare(right[0]))
    .slice(0, top)
    .map(([path, count]) => ({ path, count }));

  return {
    totalRequests: entries.length,
    totalBytes,
    statusCounts,
    topPaths
  };
}
