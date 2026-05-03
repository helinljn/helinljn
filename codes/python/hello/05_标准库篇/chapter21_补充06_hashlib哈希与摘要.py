# =============================================================================
# 第 21 章：补充 6 - hashlib 哈希与摘要
# =============================================================================
#
# 【学习目标】
#   1. 掌握 MD5、SHA1、SHA256 等常用哈希算法的基本用法
#   2. 掌握分块读取大文件计算哈希（避免内存溢出）
#   3. 理解 HMAC（带密钥的哈希）的用途和用法
#   4. 了解哈希算法的安全性差异和适用场景
#
# 【与 C/C++ 的对比】
#   C/C++ 中哈希通常需要引入 OpenSSL（libcrypto）或调用平台 API（如 Windows
#   CryptoAPI / Linux libcrypto）。Python 的 hashlib 内置在标准库中，默认使用
#   系统已安装的 OpenSSL，提供了统一的 API，无需额外链接第三方库。
#
# 【运行方式】
#   python chapter21_补充06_hashlib哈希与摘要.py
#
# =============================================================================

import hashlib
import hmac
from pathlib import Path
from typing import Any


# =============================================================================
# 21.补充6.1 hashlib 基础：MD5、SHA1、SHA256
# =============================================================================

def demo_hashlib_basics() -> None:
    """演示 hashlib 的基本用法：常用哈希算法。"""
    print("=" * 60)
    print("21.补充6.1 hashlib 基础：MD5、SHA1、SHA256")
    print("=" * 60)

    # 常用哈希算法一览
    #   算法      摘要长度（字节）  块大小   安全性          典型用途
    #   ────────  ────────────────  ────────  ──────────────  ──────────────
    #   md5       16                 64       已破解         校验完整性
    #   sha1      20                 64       已破解         旧版 Git 对象
    #   sha256    32                 64       当前安全       数字签名、Git
    #   sha512    64                 128      高安全           需高强度场合
    #   sha3_256  32                 (可变)    当前安全       SHA-3 标准

    print("算法速查:")
    algos = [
        ("md5",       16, "已破解，不用于安全"),
        ("sha1",      20, "已破解，不用于安全"),
        ("sha256",    32, "当前安全 ✓"),
        ("sha384",    48, "当前安全 ✓"),
        ("sha512",    64, "当前安全 ✓"),
        ("sha3_256",  32, "SHA-3 标准 ✓"),
        ("blake2b",   64, "比 MD5 快，比 SHA3 新 ✓"),
    ]
    for name, size, note in algos:
        print(f"  {name:10s} → {size}B 摘要, {note}")

    # ── 基本使用模式：创建 → 更新 → 摘要 ──────────────────
    data = b"Hello, Python!"

    # 方式一：一次性计算（适合小数据）
    print(f"\n输入数据: {data!r}")
    print(f"  MD5        : {hashlib.md5(data).hexdigest()}")
    print(f"  SHA1       : {hashlib.sha1(data).hexdigest()}")
    print(f"  SHA256     : {hashlib.sha256(data).hexdigest()}")
    print(f"  SHA3-256   : {hashlib.sha3_256(data).hexdigest()}")

    # 方式二：分步更新（适合流式数据 / 大文件）
    print("\n分步更新（update 可多次调用）:")
    h = hashlib.sha256()
    h.update(b"Hello, ")
    h.update(b"Python!")
    print(f"  分步结果: {h.hexdigest()}")
    print(f"  (与一次性计算相同)")

    # ── digest() vs hexdigest() ──────────────────────────
    print("\ndigest vs hexdigest:")
    h = hashlib.sha256(b"abc")
    print(f"  digest()   = {h.digest()!r}          (raw bytes, {len(h.digest())}B)")
    print(f"  hexdigest() = {h.hexdigest()}  (hex string, {len(h.hexdigest())} chars)")

    # ── 空字符串的哈希也有值 ─────────────────────────────
    print(f"\n空数据的 SHA256: {hashlib.sha256(b'').hexdigest()[:32]}...")


# =============================================================================
# 21.补充6.2 大文件分块哈希
# =============================================================================

def file_sha256(path: Path, chunk_size: int = 8192) -> str:
    """计算文件的 SHA256 哈希值（分块读取，适合大文件）。"""
    h = hashlib.sha256()
    with open(path, "rb") as f:
        while chunk := f.read(chunk_size):
            h.update(chunk)
    return h.hexdigest()


def demo_large_file_hashing() -> None:
    """演示分块读取大文件计算哈希。"""
    print("\n" + "=" * 60)
    print("21.补充6.2 大文件分块哈希")
    print("=" * 60)

    # 创建一个临时文件来演示
    tmp_path = Path("test_hash_demo.bin")
    # 写入 1MB 数据
    with open(tmp_path, "wb") as f:
        for i in range(128):
            f.write(f"Block-{i:04d} ".encode() * 1024)

    file_size = tmp_path.stat().st_size

    # ── 对比：一次性读入 vs 分块计算 ─────────────────────
    # 方式一：一次性读入（小文件可以，大文件会 OOM）
    with open(tmp_path, "rb") as f:
        sha1 = hashlib.sha256(f.read()).hexdigest()

    # 方式二：分块读取（大文件推荐）
    sha2 = file_sha256(tmp_path)

    print(f"临时文件: {tmp_path} ({file_size:,} 字节)")
    print(f"  一次性 SHA256 : {sha1}")
    print(f"  分块 SHA256    : {sha2}")
    print(f"  结果一致       : {sha1 == sha2}")

    # ── 为什么需要分块？ ─────────────────────────────────
    print(f"\n为什么需要分块读取？")
    print(f"  一次性读取: 把 {file_size:,} 字节全部加载到内存 → 可能 OOM")
    print(f"  分块读取  : 每次只读 8KB，内存占用恒定")
    print(f"  对于 GB 级别的文件，分块是唯一可行的方案")

    # 清理
    tmp_path.unlink()


# =============================================================================
# 21.补充6.3 文件完整性校验实战
# =============================================================================

def demo_file_integrity() -> None:
    """演示用哈希值校验文件完整性。"""
    print("\n" + "=" * 60)
    print("21.补充6.3 文件完整性校验实战")
    print("=" * 60)

    # 创建一个文件并记录其哈希
    tmp_path = Path("test_integrity_demo.txt")
    tmp_path.write_text("重要的配置数据\nversion = 1.0\n", encoding="utf-8")

    # 记录原始哈希
    original_hash = file_sha256(tmp_path)
    print(f"原始文件 SHA256: {original_hash[:40]}...")

    # 模拟篡改
    content = tmp_path.read_bytes()
    injected = "\n# malicious payload\n".encode()
    tmp_path.write_bytes(content + injected)

    tampered_hash = file_sha256(tmp_path)
    print(f"篡改后 SHA256:  {tampered_hash[:40]}...")

    # 校验
    if original_hash == tampered_hash:
        print("  ✓ 文件未被篡改")
    else:
        print("  ✗ 检测到文件被篡改！哈希值不匹配")

    # 这就是软件下载页面提供 SHA256 校验值的原因
    print(f"\n实际应用场景:")
    print(f"  1. 软件下载: 官网提供 SHA256SUMS，用户下载后核对")
    print(f"  2. Git: 每个对象用 SHA1 标识，确保内容完整性")
    print(f"  3. 重复文件检测: 相同内容的文件哈希相同")

    tmp_path.unlink()


# =============================================================================
# 21.补充6.4 HMAC：带密钥的哈希
# =============================================================================

def demo_hmac() -> None:
    """演示 HMAC（Hash-based Message Authentication Code）的用法。"""
    print("\n" + "=" * 60)
    print("21.补充6.4 HMAC：带密钥的哈希")
    print("=" * 60)

    # HMAC = 哈希 + 密钥，用于验证消息完整性和来源真实性
    # 与普通哈希的区别：
    #   - 普通哈希: 任何人都能算出同样的值
    #   - HMAC: 只有持有密钥的人能算出正确的值

    secret_key = b"my-secret-key-2024"
    message = b"transfer:1000:USD:alice->bob"

    # ── 生成 HMAC ─────────────────────────────────────────
    h = hmac.new(secret_key, message, hashlib.sha256)
    mac = h.hexdigest()
    print(f"密钥:    {secret_key.decode()!r}")
    print(f"消息:    {message.decode()!r}")
    print(f"HMAC-SHA256: {mac}")

    # ── 验证 HMAC（接收方） ──────────────────────────────
    # 接收方收到消息 + MAC，用共享密钥验证
    def verify_hmac(key: bytes, msg: bytes, received_mac: str) -> bool:
        """验证消息的 HMAC 是否匹配。使用 compare_digest 防时序攻击。"""
        expected = hmac.new(key, msg, hashlib.sha256)
        return hmac.compare_digest(expected.hexdigest(), received_mac)

    print(f"\n验证:")
    print(f"  正确密钥验证:    {verify_hmac(secret_key, message, mac)}")
    print(f"  错误密钥验证:    {verify_hmac(b'wrong-key', message, mac)}")
    print(f"  篡改消息验证:    {verify_hmac(secret_key, b'bad message', mac)}")

    # ── hmac.compare_digest 的重要性 ──────────────────────
    print(f"\n为什么用 hmac.compare_digest 而不是 ==?")
    print(f"  == 逐字节比较，遇到第一个不同就返回，耗时差异可被计时攻击利用")
    print(f"  hmac.compare_digest 恒定时间比较，防止时序侧信道攻击")

    # ── 实际应用：API 签名 ────────────────────────────────
    print(f"\n实际应用: Webhook / API 签名验证")
    print(f"  1. 发送方用共享密钥对请求体计算 HMAC")
    print(f"  2. 将 HMAC 放在请求头 X-Signature 中")
    print(f"  3. 接收方用同样的密钥计算 HMAC 并对比")
    print(f"  4. 匹配 → 请求来源可信且未被篡改")


# =============================================================================
# 21.补充6.5 哈希算法的安全性与选择
# =============================================================================

def demo_security_notes() -> None:
    """说明哈希算法的安全性差异和选择建议。"""
    print("\n" + "=" * 60)
    print("21.补充6.5 哈希算法的安全性与选择")
    print("=" * 60)

    print("""
安全等级速查:

  ✗ MD5、SHA1
    - 已知碰撞攻击，可构造不同内容产生相同哈希
    - 不应用于密码存储、数字签名等安全场景
    - 仅在非安全场景使用（如文件去重、数据校验）

  ✓ SHA256、SHA384、SHA512 (SHA-2 家族)
    - 当前广泛使用，安全性足够
    - Git、TLS 证书、区块链等均在使用

  ✓ SHA3-256、SHA3-512 (SHA-3 家族)
    - 更新的标准，与 SHA-2 结构不同
    - 目前尚无已知攻击

  ✓ BLAKE2 / BLAKE3
    - 比 MD5 快，安全性不低于 SHA-3
    - hashlib.blake2b / blake2s 内置于 Python 3.6+
    - 适合需要高性能的场景

密码存储特别注意:
  ✗ 绝不要用 MD5 / SHA256 直接存储密码！
    - 快速哈希意味着攻击者可以快速暴力破解
    - 应该使用专门的密码哈希函数: bcrypt、scrypt、Argon2
    - Python 的 hashlib 提供了 hashlib.scrypt()
    - 或者使用 passlib 等第三方库

简单来说:
  完整性校验 / 去重 → SHA256
  API 签名 / 认证  → HMAC-SHA256
  密码存储          → scrypt / bcrypt / Argon2（不是普通哈希！）
""")

    # ── 演示 scrypt 密码哈希 ─────────────────────────────
    print("hashlib.scrypt 示例（密码哈希）:")
    password = b"correct-horse-battery-staple"
    salt = hashlib.sha256(os.urandom(16)).digest()[:16]  # 生产中应随机生成并存储

    hashed = hashlib.scrypt(password, salt=salt, n=2**14, r=8, p=1)
    print(f"  密码:   {password.decode()!r}")
    print(f"  scrypt: {hashed.hex()[:40]}...")
    print(f"  (参数 n={2**14}, r=8, p=1 —— 生产环境建议更高)")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_hashlib_basics()
    demo_large_file_hashing()
    demo_file_integrity()
    demo_hmac()
    demo_security_notes()


if __name__ == "__main__":
    import os  # 仅 main 块需要
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. hashlib.new(name, [data]) / hashlib.md5(data) / hashlib.sha256(data) 等
#    创建哈希对象，可选地传入初始数据
#
# 2. hash.update(data)
#    追加数据进行哈希计算，可多次调用
#
# 3. hash.digest() → bytes
#    返回原始二进制摘要
#
# 4. hash.hexdigest() → str
#    返回十六进制表示的摘要字符串
#
# 5. hash.digest_size / hash.block_size
#    摘要长度 (属性) / 内部块大小 (属性)
#
# 6. hmac.new(key, msg, digestmod)
#    创建 HMAC 对象，digestmod 指定底层哈希算法
#
# 7. hmac.compare_digest(a, b) → bool
#    恒定时间比较两个摘要，防止时序攻击
#
# 8. 分块计算大文件哈希
#    h = hashlib.sha256()
#    with open(path, "rb") as f:
#        while chunk := f.read(8192):
#            h.update(chunk)
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个函数 md5_hex(s: str) -> str，返回字符串的 MD5 十六进制摘要。
#
# 练习 2（基础）：
#   写一个函数 verify_file(path, expected_sha256) 校验文件哈希是否匹配，
#   返回 bool。使用分块读取。
#
# 练习 3（进阶）：
#   写一个函数 find_duplicate_files(directory) 使用字典按哈希分组，
#   找出目录下所有内容相同的文件。返回 group_id → [paths] 的字典。
#
# 练习答案提示：
#   练习1：hashlib.md5(s.encode()).hexdigest()
#   练习2：file_sha256(path) == expected_sha256
#   练习3：遍历文件 → 计算每个文件的 SHA256 → 用 dict 按哈希分组


# =============================================================================
# 【练习答案】
# =============================================================================


def md5_hex(s: str) -> str:
    """练习 1 答案：返回字符串的 MD5 摘要。"""
    return hashlib.md5(s.encode("utf-8")).hexdigest()


def verify_file(path: Path, expected_sha256: str) -> bool:
    """练习 2 答案：校验文件哈希是否匹配。"""
    actual = file_sha256(path)
    return hmac.compare_digest(actual, expected_sha256)


def find_duplicate_files(directory: Path) -> dict[str, list[Path]]:
    """练习 3 答案：按 SHA256 哈希找出重复文件。

    返回 {hash: [path1, path2, ...]}，仅包含有重复的项。
    """
    hash_to_paths: dict[str, list[Path]] = {}
    for fpath in directory.rglob("*"):
        if fpath.is_file():
            h = file_sha256(fpath)
            hash_to_paths.setdefault(h, []).append(fpath)
    return {h: paths for h, paths in hash_to_paths.items() if len(paths) > 1}


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 40)
#     print(f"MD5 of 'hello': {md5_hex('hello')}")
#     print()
#     tmp = Path("test_verify.txt")
#     tmp.write_text("test content")
#     print(f"verify_file: {verify_file(tmp, file_sha256(tmp))}")
#     print(f"verify_file (wrong hash): {verify_file(tmp, '0' * 64)}")
#     tmp.unlink()
#     print()
#     print("find_duplicate_files 请在实际目录上测试")
