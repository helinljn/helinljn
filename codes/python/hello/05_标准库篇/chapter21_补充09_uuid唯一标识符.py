# =============================================================================
# 第 21 章：补充 9 - uuid 唯一标识符
# =============================================================================
#
# 【学习目标】
#   1. 掌握 uuid1 / uuid3 / uuid4 / uuid5 的区别和适用场景
#   2. 理解 UUID 的格式和版本含义
#   3. 能够在项目中正确选择和使用 UUID 类型
#
# 【运行方式】
#   python chapter21_补充09_uuid唯一标识符.py
#
# =============================================================================

import uuid


# =============================================================================
# 21.补充9.1 UUID 类型与生成
# =============================================================================

def demo_uuid_types() -> None:
    """演示五种 UUID 生成方式。"""
    print("=" * 60)
    print("21.补充9.1 UUID 类型与生成")
    print("=" * 60)

    # UUID 是一个 128 位（16 字节）的标识符，标准格式：
    # 550e8400-e29b-41d4-a716-446655440000
    # 8-4-4-4-12 的十六进制字符串

    # ── uuid1()：基于 MAC 地址 + 时间戳 ───────────────────
    uid1 = uuid.uuid1()
    print(f"uuid1()   (MAC+时间): {uid1}")
    print(f"  version = {uid1.version}  (UUID 版本 1)")
    print(f"  node    = {uid1.node:#014x}  (MAC 地址)")
    print(f"  特点: 可追溯到生成机器，不推荐用于隐私场景")

    # ── uuid3()：基于命名空间 + 名称的 MD5 ─────────────────
    ns = uuid.NAMESPACE_DNS  # 标准命名空间
    uid3 = uuid.uuid3(ns, "example.com")
    print(f"\nuuid3()   (MD5 命名): {uid3}")
    print(f"  version = {uid3.version}  (UUID 版本 3)")
    print(f"  特点: 相同的 name + namespace → 永远相同的 UUID")
    print(f"  再次生成: {uuid.uuid3(ns, 'example.com')}  (完全一致)")

    # ── uuid4()：随机 UUID（最常用）────────────────────────
    uid4_1 = uuid.uuid4()
    uid4_2 = uuid.uuid4()
    print(f"\nuuid4()   (随机):")
    print(f"  {uid4_1}")
    print(f"  {uid4_2}")
    print(f"  version = {uid4_1.version}  (UUID 版本 4)")
    print(f"  特点: 完全随机，碰撞概率极低，最常用")

    # ── uuid5()：基于命名空间 + 名称的 SHA1 ────────────────
    uid5 = uuid.uuid5(ns, "example.com")
    print(f"\nuuid5()   (SHA1 命名): {uid5}")
    print(f"  version = {uid5.version}  (UUID 版本 5)")
    print(f"  特点: 比 uuid3 更安全（SHA1 vs MD5）")


# =============================================================================
# 21.补充9.2 UUID 的常见操作
# =============================================================================

def demo_uuid_operations() -> None:
    """演示 UUID 对象的常用操作。"""
    print("\n" + "=" * 60)
    print("21.补充9.2 UUID 的常见操作")
    print("=" * 60)

    uid = uuid.UUID("550e8400-e29b-41d4-a716-446655440000")

    # ── 属性 ──────────────────────────────────────────────
    print(f"UUID: {uid}")
    print(f"  .hex       = {uid.hex}")
    print(f"  .bytes     = {uid.bytes!r}")
    print(f"  .int       = {uid.int}  (128位整数)")
    print(f"  .version   = {uid.version}")
    print(f"  .variant   = {uid.variant!r}  (RFC 4122)")

    # ── 字符串与 UUID 互转 ─────────────────────────────────
    u = uuid.UUID("550e8400-e29b-41d4-a716-446655440000")
    print(f"\n互相转换:")
    print(f"  解析字符串:  uuid.UUID('{str(u)}')  (相同: {u == uuid.UUID(str(u))})")
    print(f"  从 bytes:    uuid.UUID(bytes={u.bytes!r})")
    print(f"  从 int:      uuid.UUID(int={u.int})")

    # ── 安全相关：不要用 uuid4 做安全令牌 ──────────────────
    print(f"\n安全提示:")
    print(f"  uuid4() 是伪随机，不应替代 secrets.token_hex() 生成安全令牌")
    print(f"  安全场景应使用: import secrets; secrets.token_hex(16)")


# =============================================================================
# 21.补充9.3 实际应用场景
# =============================================================================

def demo_uuid_usecases() -> None:
    """演示 UUID 的实际应用场景。"""
    print("\n" + "=" * 60)
    print("21.补充9.3 实际应用场景")
    print("=" * 60)

    print("常见场景速查:\n")

    print("  # 1. 数据库主键")
    uid = uuid.uuid4()
    print(f"     user_id = uuid.uuid4()  →  {uid}")
    print(f"     优势: 分布式系统无需中心化 ID 生成器")

    print(f"\n  # 2. 文件名去重")
    original = "photo.jpg"
    unique_name = f"{uuid.uuid4().hex[:12]}_{original}"
    print(f"     {original} → {unique_name}")

    print(f"\n  # 3. request_id（请求追踪）")
    request_id = uuid.uuid4().hex[:16]
    print(f"     request_id = {request_id!r}  (请求链路追踪)")

    print(f"\n  # 4. 幂等键（Idempotency Key）")
    idem_key = str(uuid.uuid4())
    print(f"     idempotency_key = {idem_key!r}")
    print(f"     防止重复提交 / 重试产生副作用")

    print(f"\n  # 5. 确定性 UUID（uuid5）")
    tenant_id = uuid.uuid5(uuid.NAMESPACE_DNS, "tenant-a.example.com")
    resource_id = uuid.uuid5(uuid.NAMESPACE_DNS, "resource-42")
    print(f"     tenant 'tenant-a.example.com' → {tenant_id}")
    print(f"     resource 'resource-42'         → {resource_id}")
    print(f"     优势: 从名称确定性推导 UUID，不需要查表")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_uuid_types()
    demo_uuid_operations()
    demo_uuid_usecases()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. 生成 UUID
#    uuid.uuid1()                基于 MAC + 时间
#    uuid.uuid3(ns, name)        基于 MD5 (确定性)
#    uuid.uuid4()                随机 (最常用)
#    uuid.uuid5(ns, name)        基于 SHA1 (确定性，推荐)
#
# 2. 命名空间
#    uuid.NAMESPACE_DNS          基于域名
#    uuid.NAMESPACE_URL          基于 URL
#    uuid.NAMESPACE_OID          ISO OID
#    uuid.NAMESPACE_X500         X.500 DN
#
# 3. 解析
#    uuid.UUID(str)              从字符串
#    uuid.UUID(bytes=...)        从 bytes
#    uuid.UUID(int=...)          从整数
#
# 4. UUID 对象的属性
#    uid.hex                     十六进制字符串
#    uid.bytes                   16 字节 bytes
#    uid.int                     128 位整数
#    uid.version                 版本号 (1/3/4/5)
#    str(uid)                    标准字符串格式
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   生成 3 个 uuid4()，打印它们的 hex 和 int 形式。
#
# 练习 2（进阶）：
#   写一个函数 generate_deterministic_id(namespace: str, name: str) -> str，
#   使用 uuid5 + NAMESPACE_DNS，返回 hex 形式的前 16 个字符。
#   相同输入永远返回相同输出。
#
# 练习提示：
#   练习1：for _ in range(3): uid = uuid4(); print(uid.hex, uid.int)
#   练习2：return uuid.uuid5(uuid.NAMESPACE_DNS, f"{namespace}:{name}").hex[:16]


# =============================================================================
# 【练习答案】
# =============================================================================


def print_3_uuids() -> None:
    """练习 1 答案：生成 3 个 uuid4() 并打印 hex 和 int 形式。"""
    for i in range(3):
        uid = uuid.uuid4()
        print(f"  [{i + 1}] hex={uid.hex}  int={uid.int}")


def generate_deterministic_id(namespace: str, name: str) -> str:
    """练习 2 答案：从命名空间和名称生成确定性短 ID。

    使用 uuid5 + NAMESPACE_DNS，相同输入永远返回相同结果。
    """
    full = uuid.uuid5(uuid.NAMESPACE_DNS, f"{namespace}:{name}")
    return full.hex[:16]


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 60)
#     print("练习 1：3 个 uuid4()")
#     print_3_uuids()
#     print()
#     print("练习 2：确定性 ID")
#     a = generate_deterministic_id("myapp", "user-42")
#     b = generate_deterministic_id("myapp", "user-42")
#     c = generate_deterministic_id("myapp", "user-99")
#     print(f"  ('myapp', 'user-42') → {a}")
#     print(f"  ('myapp', 'user-42') → {b}  (相同)")
#     print(f"  ('myapp', 'user-99') → {c}  (不同)")
