export type LogEntry = {
  ip: string;
  timestamp: string;
  method: string;
  path: string;
  status: number;
  bytes: number;
  userAgent: string;
};

export type StatusBucket = "2xx" | "3xx" | "4xx" | "5xx" | "other";

export type LogSummary = {
  totalRequests: number;
  totalBytes: number;
  statusCounts: Record<StatusBucket, number>;
  topPaths: Array<{ path: string; count: number }>;
};

export type ReportFormat = "text" | "json";
