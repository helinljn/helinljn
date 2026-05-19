export const PROCESS_MODES = ["upper", "lower", "prefix-lines"] as const;

export type ProcessMode = typeof PROCESS_MODES[number];

export type FileBatchOptions = {
    input: string;
    output: string;
    mode: ProcessMode;
    prefix?: string;
};

export function isProcessMode(value: string): value is ProcessMode {
    return PROCESS_MODES.includes(value as ProcessMode);
}

function readFlag(args: readonly string[], flag: string): string | undefined {
    const index = args.indexOf(flag);
    if (index < 0) {
        return undefined;
    }
    const value = args[index + 1];
    if (value === undefined || value.startsWith("--")) {
        throw new Error(`missing value for ${flag}`);
    }
    return value;
}

function readRequiredFlag(args: readonly string[], flag: string): string {
    const value = readFlag(args, flag);
    if (value === undefined) {
        throw new Error(`required flag ${flag} is missing`);
    }
    return value;
}

export function parseFileBatchArgs(args: readonly string[]): FileBatchOptions {
    const input = readRequiredFlag(args, "--input");
    const output = readRequiredFlag(args, "--output");
    const modeText = readFlag(args, "--mode") ?? "upper";
    if (!isProcessMode(modeText)) {
        throw new Error(`unsupported --mode ${modeText}; expected one of ${PROCESS_MODES.join(", ")}`);
    }

    const options: FileBatchOptions = { input, output, mode: modeText };
    const prefix = readFlag(args, "--prefix");
    if (prefix !== undefined) {
        options.prefix = prefix;
    }
    return options;
}
