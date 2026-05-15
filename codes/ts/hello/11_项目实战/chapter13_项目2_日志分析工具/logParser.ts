import { readFile } from "node:fs/promises";
import type { LogEntry } from "./types.js";

const LOG_LINE = /^(?<ip>\S+) - - \[(?<timestamp>[^\]]+)\] "(?<method>[A-Z]+) (?<path>\S+) HTTP\/[\d.]+" (?<status>\d{3}) (?<bytes>\d+) "(?<userAgent>.*)"$/;

export function parseLogLine(line: string): LogEntry | null {
  const match = LOG_LINE.exec(line);
  const groups = match?.groups;
  if (groups === undefined) {
    return null;
  }

  const { ip, timestamp, method, path, status, bytes, userAgent } = groups;
  if (
    ip === undefined
    || timestamp === undefined
    || method === undefined
    || path === undefined
    || status === undefined
    || bytes === undefined
    || userAgent === undefined
  ) {
    return null;
  }

  return {
    ip,
    timestamp,
    method,
    path,
    status: Number.parseInt(status, 10),
    bytes: Number.parseInt(bytes, 10),
    userAgent
  };
}

export async function readLogFile(file: string): Promise<LogEntry[]> {
  const text = await readFile(file, "utf8");
  return text
    .split(/\r?\n/)
    .map((line) => line.trim())
    .filter((line) => line.length > 0)
    .map(parseLogLine)
    .filter((entry): entry is LogEntry => entry !== null);
}
