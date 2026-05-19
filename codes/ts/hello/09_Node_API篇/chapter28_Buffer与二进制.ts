// =============================================================================
// 第 28 章：Buffer 与二进制
// =============================================================================
//
// 【学习目标】
//   1. 掌握 Buffer 的创建、编码转换、整数读写和字节序处理
//   2. 理解 Buffer、Uint8Array、ArrayBuffer、DataView 之间的关系
//   3. 能写出带长度校验的简单二进制协议解析逻辑
//
// 【运行方式】
//   npm run build
//   node dist/09_Node_API篇/chapter28_Buffer与二进制.js
//   或 npm run chapter -- 28（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { Buffer } from "node:buffer";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

type MessageType = 1 | 2;

type WireMessage = {
    type: MessageType;
    payload: string;
};

const MAGIC = 0x5453;
const VERSION = 1;
const HEADER_SIZE = 8;

// =============================================================================
// 28.1 Buffer 的创建：from、alloc 与 allocUnsafe
// =============================================================================
//
// C++ 对照：Buffer 接近 uint8_t* 加长度，或者 std::vector<std::byte>；区别是它由
// V8/Node 管理内存，仍属于 JS 对象。
//
// C++ 思维提示：Buffer.allocUnsafe 像拿到未初始化内存。它快，但必须立刻覆盖全部字节，
// 不要把旧内存内容打印或发给用户。

function demoBufferCreation(): void {
    section("28.1 Buffer 的创建：from、alloc 与 allocUnsafe");
    note("C++ 对照", "Buffer 是 Node 的字节缓冲区，常用于文件、网络和二进制协议。");

    const fromText = Buffer.from("TypeScript", "utf8");
    const zeroed = Buffer.alloc(6);
    const unsafe = Buffer.allocUnsafe(6);
    unsafe.fill(0xaa);

    showJson("Buffer 创建方式", {
        fromText: {
            text: fromText.toString("utf8"),
            bytes: [...fromText]
        },
        alloc: {
            byteLength: zeroed.byteLength,
            bytes: [...zeroed]
        },
        allocUnsafe: {
            bytesAfterFill: [...unsafe],
            rule: "fill or write every byte before reading"
        }
    });
    note("常见坑", "allocUnsafe 不是随机数来源，也不是安全擦除；它只是跳过清零，读前必须完整写入。");
}

// =============================================================================
// 28.2 编码转换：UTF-8、Base64、Hex 与 Latin1
// =============================================================================
//
// C++ 对照：编码转换类似在字节数组和文本字符串之间选择 codec；字节没有“天然字符集”。
//
// C++ 思维提示：字符串长度按字符或 UTF-16 code unit 计数，Buffer.byteLength 按编码后的
// 字节计数。网络协议和文件格式通常关心字节数。

function demoEncodings(): void {
    section("28.2 编码转换：UTF-8、Base64、Hex 与 Latin1");
    note("C++ 对照", "同一组字节可以用不同编码解释；解释错了就会得到不同文本。");

    const text = "TS 类型";
    const utf8 = Buffer.from(text, "utf8");
    const latin1Bytes = Buffer.from([0x63, 0x61, 0x66, 0xe9]);

    showJson("编码转换", {
        utf8: {
            text,
            byteLength: Buffer.byteLength(text, "utf8"),
            hex: utf8.toString("hex"),
            base64: utf8.toString("base64"),
            roundTrip: Buffer.from(utf8.toString("base64"), "base64").toString("utf8")
        },
        latin1: {
            bytes: [...latin1Bytes],
            asLatin1: latin1Bytes.toString("latin1"),
            asUtf8: latin1Bytes.toString("utf8")
        }
    });
    note("输出解释", "0xe9 在 Latin1 中是 é；按 UTF-8 单字节解释则不是合法字符，会出现替换符。");
    note("常见坑", "Base64 和 Hex 是把字节编码成可打印文本，不是加密；任何人都能还原原始字节。");
}

// =============================================================================
// 28.3 整数读写：readUInt8、readUInt16BE、readUInt32LE
// =============================================================================
//
// C++ 对照：这些方法类似从 uint8_t 数组按指定偏移读写整数，但不会受当前 CPU 字节序隐式影响。
//
// C++ 思维提示：二进制协议必须写清楚字段偏移、字段宽度和字节序。不要把 C++ struct
// 直接 reinterpret_cast 成网络字节流。

function demoIntegerReadWrite(): void {
    section("28.3 整数读写：readUInt8、readUInt16BE、readUInt32LE");
    note("C++ 对照", "Buffer 的 read/write 方法显式指定偏移和字节序，比裸指针更可读。");

    const packet = Buffer.alloc(8);
    packet.writeUInt8(0x7f, 0);
    packet.writeUInt16BE(0x1234, 1);
    packet.writeUInt32LE(0x89abcdef, 3);
    packet.writeUInt8(0xff, 7);

    showJson("整数读写", {
        bytesHex: packet.toString("hex"),
        fields: {
            flag: packet.readUInt8(0),
            networkOrder16: `0x${packet.readUInt16BE(1).toString(16)}`,
            littleEndian32: `0x${packet.readUInt32LE(3).toString(16)}`,
            marker: `0x${packet.readUInt8(7).toString(16)}`
        }
    });
    note("类型边界", "TypeScript 能检查 offset 是 number，但越界读写仍是运行时错误，需要协议层先校验长度。");
}

// =============================================================================
// 28.4 Buffer 与 TypedArray：Buffer 是 Uint8Array 的子类
// =============================================================================
//
// C++ 对照：Uint8Array 像一段 typed view；Buffer 在 Node 中扩展了更多 I/O 和编码方法。
//
// C++ 思维提示：视图和拷贝要分清。subarray 是共享底层内存的 view，Buffer.from(view)
// 会创建拷贝，二者修改传播行为不同。

function demoTypedArrayRelationship(): void {
    section("28.4 Buffer 与 TypedArray：Buffer 是 Uint8Array 的子类");
    note("C++ 对照", "Buffer 既是字节数组视图，也是带编码/读写方法的 Node 类型。");

    const buffer = Buffer.from([10, 20, 30, 40]);
    const view = buffer.subarray(1, 3);
    view[0] = 99;
    const copy = Buffer.from(view);
    copy[0] = 7;

    showJson("视图与拷贝", {
        bufferIsUint8Array: buffer instanceof Uint8Array,
        viewSharesMemory: [...buffer],
        copiedBuffer: [...copy],
        originalAfterCopyMutation: [...buffer],
        underlyingArrayBufferAtLeastView: buffer.buffer.byteLength >= buffer.byteLength
    });
    note("输出解释", "view[0]=99 改动了原始 buffer；copy[0]=7 不会再影响原始 buffer。");
    note("常见坑", "Buffer.prototype.slice/subarray 都是视图语义；需要独立数据时使用 Buffer.from(view) 拷贝。");
}

// =============================================================================
// 28.5 字节序与 DataView：混合类型二进制结构
// =============================================================================
//
// C++ 对照：DataView 像在一段 ArrayBuffer 上按偏移读写多种类型；每次读写都能指定大小端。
//
// C++ 思维提示：网络协议常用大端，某些本地文件格式可能用小端。字节序是协议属性，
// 不应该依赖当前机器 CPU。

function demoEndianAndDataView(): void {
    section("28.5 字节序与 DataView：混合类型二进制结构");
    note("C++ 对照", "DataView 适合解析混合字段，避免 C++ struct padding 和 endian 陷阱。");

    const storage = new ArrayBuffer(10);
    const view = new DataView(storage);
    view.setUint16(0, 0x1234, false);
    view.setUint32(2, 0x89abcdef, true);
    view.setFloat32(6, 3.5, false);

    const bytes = new Uint8Array(storage);
    showJson("DataView 混合字段", {
        bytesHex: Buffer.from(bytes).toString("hex"),
        uint16BE: `0x${view.getUint16(0, false).toString(16)}`,
        uint32LE: `0x${view.getUint32(2, true).toString(16)}`,
        float32BE: view.getFloat32(6, false)
    });
    note("输出解释", "同一 ArrayBuffer 可以被 Uint8Array 看作字节，也可以被 DataView 按字段读取。");
}

// =============================================================================
// 28.6 二进制协议：消息头 + UTF-8 消息体
// =============================================================================
//
// C++ 对照：这类似手写一个 packet header：magic、version、type、payload length，再跟 payload。
//
// C++ 思维提示：解析外部二进制数据时，先检查长度、magic、版本和枚举值，再读取 payload。
// 不要信任对端声明的长度。

function encodeMessage(message: WireMessage): Buffer {
    const payload = Buffer.from(message.payload, "utf8");
    const packet = Buffer.alloc(HEADER_SIZE + payload.byteLength);
    packet.writeUInt16BE(MAGIC, 0);
    packet.writeUInt8(VERSION, 2);
    packet.writeUInt8(message.type, 3);
    packet.writeUInt32BE(payload.byteLength, 4);
    payload.copy(packet, HEADER_SIZE);
    return packet;
}

function readMessageType(value: number): MessageType {
    if (value === 1 || value === 2) {
        return value;
    }
    throw new Error(`unknown message type: ${value}`);
}

function decodeMessage(packet: Buffer): WireMessage {
    if (packet.byteLength < HEADER_SIZE) {
        throw new Error("packet too short");
    }
    if (packet.readUInt16BE(0) !== MAGIC) {
        throw new Error("bad magic");
    }
    if (packet.readUInt8(2) !== VERSION) {
        throw new Error("unsupported version");
    }

    const type = readMessageType(packet.readUInt8(3));
    const payloadLength = packet.readUInt32BE(4);
    if (packet.byteLength !== HEADER_SIZE + payloadLength) {
        throw new Error("payload length mismatch");
    }

    return {
        type,
        payload: packet.subarray(HEADER_SIZE).toString("utf8")
    };
}

function errorMessage(error: unknown): string {
    return error instanceof Error ? error.message : String(error);
}

function tryDecode(packet: Buffer): WireMessage | { error: string } {
    try {
        return decodeMessage(packet);
    } catch (error) {
        return { error: errorMessage(error) };
    }
}

function demoBinaryProtocol(): void {
    section("28.6 二进制协议：消息头 + UTF-8 消息体");
    note("C++ 对照", "固定头 + 变长体是很多网络协议和文件格式的基本结构。");

    const encoded = encodeMessage({ type: 1, payload: "hello" });
    const truncated = encoded.subarray(0, encoded.byteLength - 2);
    const badMagic = Buffer.from(encoded);
    badMagic.writeUInt16BE(0xffff, 0);

    showJson("协议编码与解析", {
        encodedHex: encoded.toString("hex"),
        decoded: decodeMessage(encoded),
        truncated: tryDecode(truncated),
        badMagic: tryDecode(badMagic)
    });
    note("类型边界", "WireMessage 是解析后的类型；网络或文件里的原始 Buffer 必须先经过 decodeMessage 校验。");
}

// =============================================================================
// 28.7 工程场景：二进制帧的安全摘要
// =============================================================================
//
// C++ 对照：真实服务常在日志中记录帧大小、类型和短摘要，而不是把整个二进制 payload
// 全部打印出来。
//
// C++ 思维提示：二进制日志要控制长度和敏感内容。TS 类型能帮助描述摘要结构，但不能
// 自动判断 payload 是否包含 secret。

function checksum8(buffer: Buffer): number {
    let sum = 0;
    for (const byte of buffer) {
        sum = (sum + byte) & 0xff;
    }
    return sum;
}

function summarizePacket(packet: Buffer): {
    byteLength: number;
    headerHex: string;
    checksum8: string;
    decoded: WireMessage | { error: string };
} {
    return {
        byteLength: packet.byteLength,
        headerHex: packet.subarray(0, Math.min(HEADER_SIZE, packet.byteLength)).toString("hex"),
        checksum8: `0x${checksum8(packet).toString(16).padStart(2, "0")}`,
        decoded: tryDecode(packet)
    };
}

function demoPacketSummary(): void {
    section("28.7 工程场景：二进制帧的安全摘要");
    note("C++ 对照", "日志里保留长度、头部和摘要，避免把全部原始内存倾倒出来。");

    const ok = encodeMessage({ type: 2, payload: "status=ok" });
    const damaged = Buffer.from(ok);
    damaged.writeUInt32BE(999, 4);

    showJson("二进制帧摘要", {
        ok: summarizePacket(ok),
        damaged: summarizePacket(damaged)
    });
    note("输出解释", "damaged 的声明长度和实际长度不一致，摘要仍可打印，但 decoded 返回错误。");
    note("本章复盘", "Buffer 解决字节存储和转换问题；协议安全来自显式长度、字节序和运行时校验。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 28 demos. */
export function runChapter(): void {
    demoBufferCreation();
    demoEncodings();
    demoIntegerReadWrite();
    demoTypedArrayRelationship();
    demoEndianAndDataView();
    demoBinaryProtocol();
    demoPacketSummary();
}

await runIfMain(import.meta.url, runChapter);
