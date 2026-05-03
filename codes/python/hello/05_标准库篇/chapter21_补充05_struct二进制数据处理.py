# =============================================================================
# 第 21 章：补充 5 - struct 二进制数据处理
# =============================================================================
#
# 【学习目标】
#   1. 掌握 struct.pack / struct.unpack 的基本用法
#   2. 理解格式字符与 C 类型的对应关系
#   3. 掌握字节序（大端/小端）的指定方式
#   4. 能够解析简单的二进制文件格式
#   5. 理解 struct 对 C/C++ 开发者的意义
#
# 【与 C/C++ 的对比】
#   C/C++ 中直接操作 struct 的内存布局，用 sizeof 获取大小，用指针强转解析。
#   Python 不能直接操作内存，struct 模块通过格式字符串来打包/解包二进制数据，
#   相当于在 Python 中模拟 C 语言的 struct 读写。
#
# 【运行方式】
#   python chapter21_补充05_struct二进制数据处理.py
#
# =============================================================================

import struct
from pathlib import Path
from typing import Any


# =============================================================================
# 21.补充5.1 struct 基础：pack 与 unpack
# =============================================================================

def demo_struct_basics() -> None:
    """演示 struct.pack / struct.unpack 的基本用法。"""
    print("=" * 60)
    print("21.补充5.1 struct 基础：pack 与 unpack")
    print("=" * 60)

    # struct 通过格式字符串描述二进制数据的布局，每个字符对应一种 C 类型：
    #
    # 格式字符  | C 类型           | Python 类型  | 字节数（标准）
    # ──────────┼──────────────────┼──────────────┼────────────
    #  c        | char             | bytes(1)     | 1
    #  b / B    | signed/unsigned char | int       | 1
    #  h / H    | short / ushort   | int          | 2
    #  i / I    | int / uint       | int          | 4
    #  f        | float            | float        | 4
    #  d        | double           | float        | 8
    #  q / Q    | long long / ull  | int          | 8
    #  ?        | _Bool            | bool         | 1
    #  s        | char[]           | bytes        | 指定长度

    print("格式字符速查:")
    fmt_table = [
        ("c",  "char (1B)",  "bytes"),
        ("b",  "signed char (1B)", "int"),
        ("B",  "unsigned char (1B)", "int"),
        ("h",  "short (2B)", "int"),
        ("H",  "unsigned short (2B)", "int"),
        ("i",  "int (4B)", "int"),
        ("I",  "unsigned int (4B)", "int"),
        ("f",  "float (4B)", "float"),
        ("d",  "double (8B)", "float"),
        ("q",  "long long (8B)", "int"),
        ("?",  "_Bool (1B)", "bool"),
    ]
    for ch, ctype, pytype in fmt_table:
        print(f"  {ch:4s} → {ctype:<22s} → Python {pytype}")

    # ── pack：把 Python 值打包成 bytes ────────────────────
    print("\npack 示例（默认使用本机字节序@）:")
    print(f"  pack('i', 42)       = {struct.pack('i', 42)!r}")
    print(f"  pack('i', -1)       = {struct.pack('i', -1)!r}")
    print(f"  pack('f', 3.14)     = {struct.pack('f', 3.14)!r}")
    print(f"  pack('?', True)     = {struct.pack('?', True)!r}")
    print(f"  pack('3s', b'abc')  = {struct.pack('3s', b'abc')!r}")

    # 多个值同时打包
    print(f"  pack('iif', 1, 2, 3.0) = {struct.pack('iif', 1, 2, 3.0)!r}")

    # ── unpack：把 bytes 解包成 Python 元组 ───────────────
    print("\nunpack 示例:")
    data = struct.pack("iif", 100, 200, 1.5)
    result = struct.unpack("iif", data)
    print(f"  unpack('iif', {data!r}) = {result}")
    # result 是元组 (100, 200, 1.5)

    # ── calcsize：计算格式字符串需要的字节数 ──────────────
    print("\ncalcsize 示例:")
    print(f"  calcsize('i')    = {struct.calcsize('i')} 字节")
    print(f"  calcsize('iif')  = {struct.calcsize('iif')} 字节")
    print(f"  calcsize('iii')  = {struct.calcsize('iii')} 字节")


# =============================================================================
# 21.补充5.2 字节序（Byte Order）
# =============================================================================

def demo_byte_order() -> None:
    """演示大端/小端字节序的区别。"""
    print("\n" + "=" * 60)
    print("21.补充5.2 字节序")
    print("=" * 60)

    # 字节序前缀（格式字符串第一个字符）：
    #   @  = 本机字节序 + 本机对齐（默认）
    #   =  = 本机字节序 + 标准大小（无填充）
    #   <  = 小端（little-endian），如 x86/ARM
    #   >  = 大端（big-endian），如网络字节序
    #   !  = 大端 + 标准大小（网络传输常用）

    value = 0x12345678

    # 用不同字节序打包同一个整数
    native = struct.pack("@I", value)
    little = struct.pack("<I", value)
    big    = struct.pack(">I", value)

    print(f"数值: 0x{value:08X} ({value})")
    print(f"  本机序 (@I):  {native.hex(' ')}")
    print(f"  小端   (<I):  {little.hex(' ')}")
    print(f"  大端   (>I):  {big.hex(' ')}")
    print()

    # C/C++ 开发者注意：
    # - x86/x64 是小端序，内存中低字节在低地址
    # - 网络传输通常用大端序（network byte order）
    # - 跨平台二进制文件建议显式指定字节序（< 或 >）

    # 解包回来，不管用什么序都能正确还原
    print(f"  unpack('@I', native) = 0x{struct.unpack('@I', native)[0]:08X}")
    print(f"  unpack('<I', little) = 0x{struct.unpack('<I', little)[0]:08X}")
    print(f"  unpack('>I', big)    = 0x{struct.unpack('>I', big)[0]:08X}")

    # ── 字节序错误的后果 ─────────────────────────────────
    print("\n用错误字节序解包（C/C++ 中常见坑）:")
    # 用小端打包，用大端解包——读到错误的值
    packed = struct.pack("<I", 0x12345678)
    wrong = struct.unpack(">I", packed)[0]
    print(f"  小端打包 0x12345678, 大端解包 → 0x{wrong:08X} ({wrong})")
    # 这就是为什么字节序必须一致！

    # ── 网络协议中常用 ! 前缀 ────────────────────────────
    print(f"\n  网络序 (!I): {struct.pack('!I', 443).hex(' ')}")
    print(f"  (HTTP/HTTPS 端口 443 的大端表示)")


# =============================================================================
# 21.补充5.3 实战：解析 BMP 文件头
# =============================================================================

def demo_bmp_header() -> None:
    """用 struct 解析 Windows BMP 文件头。

    BMP 文件头结构（C 语言）：
        struct BitmapFileHeader {
            uint16_t bfType;       // 文件类型，必须是 'BM' (0x4D42)
            uint32_t bfSize;       // 文件总大小（字节）
            uint16_t bfReserved1;  // 保留，0
            uint16_t bfReserved2;  // 保留，0
            uint32_t bfOffBits;    // 从文件头到像素数据的偏移量
        };
        // 共 14 字节（注意：无结构体对齐填充）
    """
    print("\n" + "=" * 60)
    print("21.补充5.3 实战：解析 BMP 文件头")
    print("=" * 60)

    # BMP 文件头：14 字节，小端序，各字段紧凑排列
    # 对应格式字符串：H (2B, type) + I (4B, size) + H (2B) + H (2B) + I (4B) = 14B
    BMP_HEADER_FMT = "<HIHHI"

    print("BMP 文件头格式: " + BMP_HEADER_FMT)
    print(f"  calcsize = {struct.calcsize(BMP_HEADER_FMT)} 字节")
    print()

    # 构造一个最小的 1x1 像素 BMP 文件（不含调色板）
    # 手动构建各段二进制数据
    pixel_data = b"\x00\x00\xFF\x00"  # BGRA: 红=0, 绿=0, 蓝=255, Alpha=0
    # DIB 头（BITMAPINFOHEADER, 40 字节）
    dib_header = struct.pack(
        "<IiiHHIIiiII",
        40,     # biSize
        1,      # biWidth
        1,      # biHeight
        1,      # biPlanes
        24,     # biBitCount (24-bit)
        0,      # biCompression (BI_RGB)
        0,      # biSizeImage (可为 0)
        0,      # biXPelsPerMeter
        0,      # biYPelsPerMeter
        0,      # biClrUsed
        0,      # biClrImportant
    )

    file_header_size = struct.calcsize(BMP_HEADER_FMT)
    total_size = file_header_size + len(dib_header) + len(pixel_data)
    pixel_offset = file_header_size + len(dib_header)

    file_header = struct.pack(
        BMP_HEADER_FMT,
        0x4D42,        # bfType: 'BM'
        total_size,    # bfSize
        0,             # bfReserved1
        0,             # bfReserved2
        pixel_offset,  # bfOffBits
    )

    bmp_data = file_header + dib_header + pixel_data

    # ── 保存后重新读取并解析 ──────────────────────────────
    bmp_path = Path("test_struct_demo.bmp")
    bmp_path.write_bytes(bmp_data)
    print(f"已写入测试 BMP: {bmp_path} ({len(bmp_data)} 字节)\n")

    # 读取并解析文件头
    with open(bmp_path, "rb") as f:
        header_bytes = f.read(file_header_size)

    bfType, bfSize, bfReserved1, bfReserved2, bfOffBits = struct.unpack(
        BMP_HEADER_FMT, header_bytes
    )

    print("解析结果:")
    print(f"  bfType       = 0x{bfType:04X} ({chr(bfType & 0xFF)}{chr(bfType >> 8)})")
    print(f"  bfSize       = {bfSize} 字节")
    print(f"  bfReserved1  = {bfReserved1}")
    print(f"  bfReserved2  = {bfReserved2}")
    print(f"  bfOffBits    = {bfOffBits} (像素数据从文件第 {bfOffBits} 字节开始)")

    # 验证
    assert bfType == 0x4D42, "不是有效的 BMP 文件！"
    assert bfSize == len(bmp_data), "文件大小不匹配！"
    print("\n  ✓ 文件头解析验证通过")

    # 清理
    bmp_path.unlink()


# =============================================================================
# 21.补充5.4 实战：自定义二进制协议
# =============================================================================

def demo_binary_protocol() -> None:
    """模拟一个自定义二进制通信协议的消息打包与解包。

    假设一个简单的传感器数据报文：
        偏移  大小  类型   含义
        ──────┬──────┬──────┬─────────────────
         0    | 1    | B   | 消息类型 (1=温度, 2=湿度, 3=气压)
         1    | 2    | H   | 设备 ID
         3    | 4    | I   | 时间戳 (Unix timestamp)
         7    | 4    | f   | 传感器读数
        11    | 1    | B   | 校验和（以上各字节的 XOR）
        ──────┴──────┴──────┴─────────────────
        共计 12 字节
    """
    print("\n" + "=" * 60)
    print("21.补充5.4 实战：自定义二进制协议")
    print("=" * 60)

    # 协议格式（大端，模拟网络传输）
    PROTO_FMT = ">BHIfB"  # type, device_id, timestamp, reading, checksum
    MSG_SIZE = struct.calcsize(PROTO_FMT)
    print(f"协议格式: {PROTO_FMT}")
    print(f"报文大小: {MSG_SIZE} 字节\n")

    # ── 打包消息 ────────────────────────────────────────
    def pack_sensor_msg(msg_type: int, device_id: int,
                        timestamp: int, reading: float) -> bytes:
        """将传感器数据打包为二进制报文。"""
        # 先打包数据部分（不含校验和）
        body = struct.pack(">BHIf", msg_type, device_id, timestamp, reading)
        # 计算 XOR 校验和
        checksum = 0
        for b in body:
            checksum ^= b
        return struct.pack(PROTO_FMT, msg_type, device_id,
                          timestamp, reading, checksum)

    def unpack_sensor_msg(data: bytes) -> dict[str, Any] | None:
        """解包传感器报文并验证校验和。"""
        if len(data) != MSG_SIZE:
            print(f"  ✗ 报文长度错误: {len(data)} (期望 {MSG_SIZE})")
            return None

        msg_type, device_id, timestamp, reading, checksum = struct.unpack(
            PROTO_FMT, data
        )

        # 验证校验和
        body = data[:-1]  # 去掉校验和字节
        expected = 0
        for b in body:
            expected ^= b
        if expected != checksum:
            print(f"  ✗ 校验和不匹配: 收到 {checksum}, 计算 {expected}")
            return None

        import datetime
        return {
            "type": {1: "温度", 2: "湿度", 3: "气压"}.get(msg_type, "未知"),
            "device_id": device_id,
            "time": datetime.datetime.fromtimestamp(
                timestamp
            ).strftime("%Y-%m-%d %H:%M:%S"),
            "reading": reading,
        }

    # 打包几条测试消息
    msgs = [
        pack_sensor_msg(1, 1001, 1705324800, 25.6),  # 温度
        pack_sensor_msg(2, 1001, 1705324860, 68.3),  # 湿度
        pack_sensor_msg(3, 2005, 1705324920, 1013.2),  # 气压
    ]

    print("打包的消息:")
    for i, m in enumerate(msgs):
        print(f"  [{i + 1}] {m.hex(' ')} ({len(m)} 字节)")

    # 解包
    print("\n解包结果:")
    for m in msgs:
        result = unpack_sensor_msg(m)
        if result:
            print(f"  {result}")

    # ── 演示错误检测：篡改数据 ──────────────────────────
    print("\n篡改数据测试（校验和检测）:")
    corrupted = bytearray(msgs[0])
    corrupted[8] ^= 0xFF  # 翻转读数字段的一个 bit
    unpack_sensor_msg(bytes(corrupted))


# =============================================================================
# 21.补充5.5 struct 的局限性
# =============================================================================

def demo_limitations() -> None:
    """说明 struct 不适合的场景。"""
    print("\n" + "=" * 60)
    print("21.补充5.5 struct 的局限性")
    print("=" * 60)

    print("""
struct 适用于：
  ✓ 固定长度的简单二进制格式（文件头、协议头）
  ✓ C 结构体的序列化/反序列化
  ✓ 与 C/C++ 程序交换结构化二进制数据

struct 不适用于：
  ✗ 变长字符串——需要先存长度再存内容，手动拼接
  ✗ 复杂的嵌套结构——多层嵌套需要手动拆分格式
  ✗ 大量数据处理——每次 pack/unpack 创建新 bytes 对象

替代方案：
  • 变长/复杂协议 → ctypes.Structure 或 construct 库（第三方）
  • 跨语言序列化   → Protocol Buffers (protobuf)，FlatBuffers
  • 文本格式       → JSON、TOML（人类可读，但体积大）

对于 C/C++ 迁移过来的开发者：
  struct 是理解 Python 如何与 C 程序通信的桥梁。
  掌握了 struct，就掌握了 Python ↔ C 二进制数据交换的核心机制。
""")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_struct_basics()
    demo_byte_order()
    demo_bmp_header()
    demo_binary_protocol()
    demo_limitations()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. pack(fmt, *values) → bytes
#    把 Python 值按格式字符串打包为二进制数据
#
# 2. unpack(fmt, buffer) → tuple
#    把二进制数据按格式字符串解包为 Python 元组
#
# 3. calcsize(fmt) → int
#    返回格式字符串需要的字节数
#
# 4. 格式字符速查
#    b/B    signed/unsigned char    (1 字节)
#    h/H    short/ushort            (2 字节)
#    i/I    int/uint                (4 字节)
#    q/Q    long long               (8 字节)
#    f/d    float/double            (4/8 字节)
#    ?      bool                    (1 字节)
#
# 5. 字节序前缀
#    @      本机序 + 对齐（默认）
#    <      小端（x86/ARM 本机序）
#    > / !  大端（网络字节序）
#
# 6. C/C++ 开发者要点
#    - struct 相当于 Python 世界的 sizeof + 序列化
#    - 显式指定字节序（< 或 >），避免跨平台问题
#    - 网络传输建议用 !（大端，等同于 >）


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个函数 pack_point(x, y)，把两个 float 坐标打包为 8 字节的小端二进制数据。
#   再写 unpack_point(data) 解包还原。
#
# 练习 2（基础）：
#   写一个函数 is_png(data)，检查 bytes 的前 8 字节是否为 PNG 文件签名：
#   b'\x89PNG\r\n\x1a\n'。用 struct 或直接比较都可以。
#
# 练习 3（进阶）：
#   写一个函数 parse_wav_header(path)，读取 WAV 文件的 RIFF 头，返回：
#   {"chunk_id": "RIFF", "chunk_size": N, "format": "WAVE", "sample_rate": 44100, ...}
#   提示：WAV 文件头结构可查网，重点练习 struct 解包多字段格式。
#
# 练习答案提示：
#   练习1：pack('<ff', x, y)，unpack('<ff', data)
#   练习2：data[:8] == b'\x89PNG\r\n\x1a\n'
#   练习3：分两次 unpack——先读 RIFF 头（<4sI4s），再读 fmt 子块（<HHIIHH）


# =============================================================================
# 【练习答案】
# =============================================================================


def pack_point(x: float, y: float) -> bytes:
    """练习 1 答案：打包坐标点。"""
    return struct.pack("<ff", x, y)


def unpack_point(data: bytes) -> tuple[float, float]:
    """练习 1 答案：解包坐标点。"""
    return struct.unpack("<ff", data)  # type: ignore[return-value]


def is_png(data: bytes) -> bool:
    """练习 2 答案：检查 PNG 文件签名。"""
    PNG_SIGNATURE = b'\x89PNG\r\n\x1a\n'
    return data[:8] == PNG_SIGNATURE


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 60)
#     pt = pack_point(3.5, -2.8)
#     print(f"打包 Point(3.5, -2.8): {pt.hex(' ')}")
#     print(f"解包: {unpack_point(pt)}")
#
#     print("\n" + "=" * 60)
#     print(f"is_png(b'\\x89PNG...'): {is_png(b'\\x89PNG\\r\\n\\x1a\\n' + b'x' * 100)}")
#     print(f"is_png(b'GIF89a...'):   {is_png(b'GIF89a')}")
