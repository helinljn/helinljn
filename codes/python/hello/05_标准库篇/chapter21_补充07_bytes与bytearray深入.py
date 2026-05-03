# =============================================================================
# 第 21 章：补充 7 - bytes 与 bytearray 深入
# =============================================================================
#
# 【学习目标】
#   1. 理解 bytes（不可变）与 bytearray（可变）的区别
#   2. 掌握二进制数据的创建、索引、切片操作
#   3. 掌握 bytes/bytearray 的常用方法
#   4. 理解与 C/C++ 中 unsigned char[] 的对应关系
#   5. 掌握字符串与二进制之间的编解码
#
# 【与 C/C++ 的对比】
#   C 语言中二进制数据用 unsigned char[] 或 uint8_t* 表示，可以直接通过
#   指针修改任意字节。Python 中：
#   - bytes   ≈ const unsigned char[]（不可变，类似字符串只读）
#   - bytearray ≈ unsigned char[]（可变，可以原地修改）
#   两者的底层都是 0-255 的整数序列，但 Python 不直接暴露内存地址。
#
# 【运行方式】
#   python chapter21_补充07_bytes与bytearray深入.py
#
# =============================================================================

import struct


# =============================================================================
# 21.补充7.1 bytes 基础
# =============================================================================

def demo_bytes_basics() -> None:
    """演示 bytes 的创建和基本操作。"""
    print("=" * 60)
    print("21.补充7.1 bytes 基础")
    print("=" * 60)

    # ── 创建 bytes ────────────────────────────────────────
    # 方式 1：字面量（仅 ASCII 字符）
    b1 = b"hello"
    print(f"b'hello'          = {b1!r}")

    # 方式 2：从整数列表构造
    b2 = bytes([72, 101, 108, 108, 111])  # ASCII: H e l l o
    print(f"bytes([72,101,...]) = {b2!r}")

    # 方式 3：从十六进制字符串
    b3 = bytes.fromhex("48656c6c6f")
    print(f"bytes.fromhex('48656c6c6f') = {b3!r}")

    # 方式 4：用指定字节填充
    b4 = bytes(5)  # 5 个零字节
    print(f"bytes(5)          = {b4!r}")

    # ── bytes 索引返回整数（0-255） ────────────────────────
    b = b"ABCD"
    print(f"\nb'ABCD' 索引:")
    print(f"  b[0] = {b[0]!r}  (type: {type(b[0]).__name__})")
    print(f"  b[1] = {b[1]!r}")
    print(f"  b[-1] = {b[-1]!r}  (最后一个)")

    # ── bytes 切片返回 bytes ──────────────────────────────
    print(f"\nb'ABCD' 切片:")
    print(f"  b[1:3] = {b[1:3]!r}")

    # ── 遍历 ──────────────────────────────────────────────
    print(f"\n遍历（得到整数）:")
    for byte in b"ABC":
        print(f"  {byte:#04x} ({byte:3d}) {chr(byte)!r}")

    # ── bytes 是不可变的 ──────────────────────────────────
    print(f"\nbytes 不可变:")
    try:
        b[0] = 65  # type: ignore[index]
    except TypeError as e:
        print(f"  b[0] = 65 → TypeError: {e}")


# =============================================================================
# 21.补充7.2 bytearray：可变的二进制序列
# =============================================================================

def demo_bytearray() -> None:
    """演示 bytearray 的可变性。"""
    print("\n" + "=" * 60)
    print("21.补充7.2 bytearray：可变的二进制序列")
    print("=" * 60)

    # ── 创建 bytearray ────────────────────────────────────
    ba = bytearray(b"Hello")
    print(f"原始:    {ba!r}")

    # 原地修改元素
    ba[0] = 0x68  # 'h'
    print(f"ba[0]=0x68 → {ba!r}")

    # append / extend
    ba.append(0x21)  # '!'
    print(f"append(0x21) → {ba!r}")

    ba.extend(b" World")
    print(f"extend(b' World') → {ba!r}")

    # insert / pop
    ba.insert(5, 0x2C)  # 逗号
    print(f"insert(5, 0x2C) → {ba!r}")
    popped = ba.pop(5)
    print(f"pop(5) = {popped:#04x}, 结果: {ba!r}")

    # ── C/C++ 风格的缓冲区操作 ────────────────────────────
    print(f"\nC/C++ 风格操作:")
    buf = bytearray(8)            # 分配 8 字节缓冲区
    buf[0:4] = b"\x01\x02\x03\x04"  # 写入前 4 字节
    buf[4:8] = struct.pack("<I", 0x12345678)  # 写入 uint32
    print(f"  8字节缓冲区: {buf!r}")
    print(f"  hex: {buf.hex(' ')}")

    # 这正是 C 开发者在 Python 中操作二进制缓冲的方式


# =============================================================================
# 21.补充7.3 bytes 常用方法
# =============================================================================

def demo_bytes_methods() -> None:
    """演示 bytes/bytearray 的常用方法。"""
    print("\n" + "=" * 60)
    print("21.补充7.3 bytes 常用方法")
    print("=" * 60)

    data = b"Hello, World!"

    # 查找
    print(f"data = {data!r}")
    print(f"  find(b'World')    = {data.find(b'World')!r}")
    print(f"  find(b'X')        = {data.find(b'X')!r}    (不存在返回 -1)")
    print(f"  count(b'l')       = {data.count(b'l')}")

    # 切分与连接
    parts = data.split(b", ")
    print(f"  split(b', ')        = {parts!r}")

    joined = b" | ".join(parts)
    print(f"  b' | '.join(parts)  = {joined!r}")

    # 大小写转换（仅 ASCII）
    print(f"  upper()           = {data.upper()!r}")
    print(f"  lower()           = {data.lower()!r}")

    # 前缀/后缀检查
    print(f"  startswith(b'He') = {data.startswith(b'He')}")
    print(f"  endswith(b'!')    = {data.endswith(b'!')}")

    # ── 替换 ──────────────────────────────────────────────
    replaced = data.replace(b"World", b"Python")
    print(f"  replace(b'World', b'Python') = {replaced!r}")

    # ── 十六进制转换 ──────────────────────────────────────
    print(f"\n十六进制:")
    print(f"  hex()             = {data.hex()}")
    print(f"  hex(' ')          = {data.hex(' ')}")
    print(f"  bytes.fromhex(...) = {bytes.fromhex(data.hex())!r}")


# =============================================================================
# 21.补充7.4 字符串与二进制的编解码
# =============================================================================

def demo_encoding() -> None:
    """演示 str ↔ bytes 的编解码。"""
    print("\n" + "=" * 60)
    print("21.补充7.4 字符串与二进制的编解码")
    print("=" * 60)

    text = "你好，世界！"

    # str → bytes：编码
    utf8_bytes = text.encode("utf-8")
    gbk_bytes = text.encode("gbk")

    print(f"原文: {text!r}")
    print(f"  UTF-8 ({len(utf8_bytes)}B): {utf8_bytes.hex(' ')}")
    print(f"  GBK   ({len(gbk_bytes)}B):  {gbk_bytes.hex(' ')}")

    # bytes → str：解码
    decoded_utf8 = utf8_bytes.decode("utf-8")
    decoded_gbk = gbk_bytes.decode("gbk")
    print(f"  解码 UTF-8: {decoded_utf8!r}")
    print(f"  解码 GBK:   {decoded_gbk!r}")

    # ── 错误处理 ──────────────────────────────────────────
    print(f"\n编解码错误处理:")
    # 尝试用错误的编码解码
    try:
        utf8_bytes.decode("gbk")
    except UnicodeDecodeError as e:
        print(f"  用 GBK 解码 UTF-8 字节 → {type(e).__name__}: {e.reason}")

    # errors='replace'：用 � 替代无法解码的字节
    replaced = utf8_bytes.decode("ascii", errors="replace")
    print(f"  decode('ascii', errors='replace') = {replaced!r}")

    # errors='ignore'：跳过无法解码的字节
    ignored = utf8_bytes.decode("ascii", errors="ignore")
    print(f"  decode('ascii', errors='ignore')  = {ignored!r}")

    # ── C/C++ 开发者注意 ──────────────────────────────────
    print(f"\nC/C++ 对比:")
    print(f"  C 中 char* 可以同时表示字符串和二进制，边界模糊")
    print(f"  Python 中 str 和 bytes 严格分离，编译码是显式的")
    print(f"  这是一个好的设计，避免了 C 中常见的编码混淆 bug")


# =============================================================================
# 21.补充7.5 memoryview：零拷贝操作
# =============================================================================

def demo_memoryview() -> None:
    """演示 memoryview 的零拷贝切片。"""
    print("\n" + "=" * 60)
    print("21.补充7.5 memoryview：零拷贝操作")
    print("=" * 60)

    # memoryview 允许在不复制数据的情况下操作二进制序列
    # 这对于大缓冲区和 C 扩展非常有用

    data = bytearray(b"The quick brown fox jumps over the lazy dog")
    mv = memoryview(data)

    print(f"原始数据: {bytes(mv)!r}")

    # 零拷贝切片（返回新的 memoryview，共享底层数据）
    word1 = mv[4:9]   # "quick"
    word2 = mv[10:15]  # "brown"

    print(f"  mv[4:9]   = {bytes(word1)!r}")
    print(f"  mv[10:15] = {bytes(word2)!r}")

    # 修改 memoryview 会修改原始 bytearray
    word1_bytes = word1.tobytes()
    # 证明是零拷贝：修改原始 data，memoryview 也变了
    data[4] = 0x51  # 'Q'
    print(f"\n修改 data[4] 后:")
    print(f"  data      = {bytes(data)!r}")
    print(f"  mv[4:9]   = {bytes(mv[4:9])!r}  (已同步变化)")

    # ── memoryview 的类型码 ───────────────────────────────
    print(f"\nmemoryview 格式:")
    print(f"  format = {mv.format!r}  (字节类型)")
    print(f"  itemsize = {mv.itemsize}     (每元素字节数)")
    print(f"  ndim = {mv.ndim}         (维度)")
    print(f"  可读 = {mv.readonly}")

    # ── 用途：解析二进制协议时不复制数据 ──────────────────
    print(f"\n零拷贝解析示例（struct + memoryview）:")
    buf = bytearray(b"\x01\x02\x03\x04\x05\x06\x07\x08")
    mv_buf = memoryview(buf)
    # 切出前 4 字节，不解包也能零拷贝传递
    header_view = mv_buf[:4]
    payload_view = mv_buf[4:]
    print(f"  原始 {len(buf)}B: {buf.hex(' ')}")
    print(f"  头部:      {bytes(header_view).hex(' ')} ({sys.getrefcount(None)} 次拷贝)")
    print(f"  载荷:      {bytes(payload_view).hex(' ')}")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_bytes_basics()
    demo_bytearray()
    demo_bytes_methods()
    demo_encoding()
    demo_memoryview()


if __name__ == "__main__":
    import sys
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. 创建 bytes
#    b"hello"             字面量（仅 ASCII）
#    bytes([65, 66, 67])  从整数列表
#    bytes.fromhex("...") 从十六进制
#    bytes(10)             10 个零字节
#
# 2. 创建 bytearray
#    bytearray(b"hello")     从 bytes
#    bytearray([65, 66, 67]) 从整数列表
#    bytearray(10)             10 个零字节（可变）
#
# 3. 索引 / 切片
#    b[i]     → int (0-255)
#    b[a:b]   → bytes (切片)
#    ba[i]    → int (0-255)
#    ba[a:b]  = b"..."  (切片赋值)
#
# 4. 常用方法 (同 str 风格)
#    find / count / split / join / replace / startswith / endswith
#    upper / lower / hex / fromhex
#
# 5. 编解码
#    str.encode("utf-8")  → bytes
#    bytes.decode("utf-8") → str
#
# 6. memoryview
#    memoryview(buf)      零拷贝视图
#    mv[a:b]               零拷贝切片
#    bytes(mv)             转换为 bytes（此时才拷贝）
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个函数 xor_bytes(a: bytes, b: bytes) -> bytes
#   逐字节 XOR 两个等长二进制数据，用于简单的流加密。
#
# 练习 2（基础）：
#   写一个函数 to_hex_dump(data: bytes, bytes_per_line: int = 16) -> str
#   生成类似 xxd 的十六进制转储：左边十六进制，右边 ASCII 可打印字符。
#
# 练习提示：
#   练习1：[(x ^ y for x, y in zip(a, b))] 构造整数列表 → bytes()
#   练习2：按 bytes_per_line 切片 → 每切片用 .hex(' ') + ASCII 可打印字符


# =============================================================================
# 【练习答案】
# =============================================================================


def xor_bytes(a: bytes, b: bytes) -> bytes:
    """练习 1 答案：逐字节 XOR。"""
    if len(a) != len(b):
        raise ValueError("长度必须相等")
    return bytes(x ^ y for x, y in zip(a, b))


def to_hex_dump(data: bytes, bytes_per_line: int = 16) -> str:
    """练习 2 答案：生成十六进制转储字符串。"""
    lines: list[str] = []
    for i in range(0, len(data), bytes_per_line):
        chunk = data[i:i + bytes_per_line]
        hex_part = chunk.hex(" ")
        ascii_part = "".join(
            chr(b) if 32 <= b < 127 else "."
            for b in chunk
        )
        lines.append(f"{i:08x}  {hex_part:<{bytes_per_line * 3}} {ascii_part}")
    return "\n".join(lines)


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 60)
#     a = b"hello"
#     key = b"\x01\x02\x03\x04\x05"
#     encrypted = xor_bytes(a, key)
#     print(f"原文:     {a!r}")
#     print(f"加密后:   {encrypted.hex(' ')}")
#     print(f"解密后:   {xor_bytes(encrypted, key)!r}")
#     print()
#     print(to_hex_dump(b"The quick brown fox jumps over the lazy dog."))
