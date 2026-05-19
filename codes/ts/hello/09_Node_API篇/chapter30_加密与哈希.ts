// =============================================================================
// 第 30 章：加密与哈希
// =============================================================================
//
// 【学习目标】
//   1. 理解 Node crypto 模块与 OpenSSL 的关系，以及哈希、HMAC、随机数、密码哈希的边界
//   2. 掌握 createHash、createHmac、pbkdf2、scrypt 和 AES-GCM 的最小可运行用法
//   3. 建立安全直觉：hash 不等于 encryption，不自创密码学，secret 不写入源码或日志
//
// 【运行方式】
//   npm run build
//   node dist/09_Node_API篇/chapter30_加密与哈希.js
//   或 npm run chapter -- 30（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import {
    createCipheriv,
    createDecipheriv,
    createHash,
    createHmac,
    getHashes,
    pbkdf2,
    randomBytes,
    randomUUID,
    scryptSync,
    timingSafeEqual
} from "node:crypto";
import { promisify } from "node:util";
import { note, runIfMain, section, showJson } from "../shared/chapter.js";

const pbkdf2Async = promisify(pbkdf2);

// =============================================================================
// 30.1 crypto 模块概览：Node 对 OpenSSL 能力的封装
// =============================================================================
//
// C++ 对照：Node crypto 更像 OpenSSL C API 的高层封装；你仍要理解算法、密钥、IV、
// tag、salt、iterations 等参数的安全含义。
//
// C++ 思维提示：TypeScript 只能检查参数类型，不能证明你选的算法、模式和参数是安全的。
// 密码学错误通常是“能运行但不安全”。

function demoCryptoOverview(): void {
    section("30.1 crypto 模块概览：Node 对 OpenSSL 能力的封装");
    note("C++ 对照", "crypto 模块把 OpenSSL 能力暴露成 JS API，但安全设计仍由你负责。");

    const algorithms = new Set(getHashes());
    showJson("本地 crypto 能力边界", {
        provider: "Node.js crypto backed by OpenSSL",
        supports: {
            sha256: algorithms.has("sha256"),
            sha3_256: algorithms.has("sha3-256"),
            md5: algorithms.has("md5")
        },
        tutorialRule: "feature-detect algorithms when portability matters"
    });
    note("版本边界", "可用算法取决于当前 Node/OpenSSL 构建；本章对 SHA3-256 做 feature detect。");
}

// =============================================================================
// 30.2 哈希：createHash("sha256").update(data).digest("hex")
// =============================================================================
//
// C++ 对照：哈希类似把任意长度字节映射成固定长度摘要，常用于完整性校验和内容寻址。
//
// C++ 思维提示：哈希是单向摘要，不是加密。哈希后的内容不能“解密”回原文。

function digestHex(algorithm: string, data: string | Buffer): string {
    return createHash(algorithm).update(data).digest("hex");
}

function demoHashBasics(): void {
    section("30.2 哈希：createHash(\"sha256\").update(data).digest(\"hex\")");
    note("C++ 对照", "哈希适合完整性和去重，不提供保密性。");

    const data = "TypeScript 6.0";
    const sha256 = digestHex("sha256", data);
    const sha3 = getHashes().includes("sha3-256") ? digestHex("sha3-256", data) : "<sha3-256 unavailable>";
    const md5 = digestHex("md5", data);

    showJson("固定输入摘要", {
        input: data,
        sha256,
        sha3_256: sha3,
        md5ForNonSecurityChecksumOnly: md5
    });
    note("常见坑", "MD5 不适合安全场景；最多用于非对抗性的旧系统校验或兼容历史数据。");
}

// =============================================================================
// 30.3 分块更新：hash.update(chunk) 处理大文件或流
// =============================================================================
//
// C++ 对照：分块 update 类似 OpenSSL EVP_DigestUpdate，多次喂入 chunk 后 final。
//
// C++ 思维提示：大文件不要一次性读入内存。ch31 会讲 stream；本章先用固定 chunk 模拟。

function demoChunkedHash(): void {
    section("30.3 分块更新：hash.update(chunk) 处理大文件或流");
    note("C++ 对照", "同一个 hash 上多次 update，等价于对拼接后的字节做一次 hash。");

    const chunks = ["alpha\n", "beta\n", "gamma\n"];
    const oneShot = digestHex("sha256", chunks.join(""));
    const chunked = createHash("sha256");
    for (const chunk of chunks) {
        chunked.update(chunk);
    }

    showJson("分块 hash", {
        chunks,
        oneShot,
        chunked: chunked.digest("hex"),
        sameInputSameDigest: true
    });
    note("输出解释", "分块更新改变的是内存使用方式，不改变同一字节序列的最终摘要。");
}

// =============================================================================
// 30.4 HMAC：带密钥的消息认证码
// =============================================================================
//
// C++ 对照：HMAC 类似用共享 secret 对消息做认证，接收方用同一个 secret 重新计算并比较。
//
// C++ 思维提示：HMAC 不是加密，它不隐藏消息内容；它验证消息没有被不知道密钥的人篡改。

function demoHmac(): void {
    section("30.4 HMAC：带密钥的消息认证码");
    note("C++ 对照", "HMAC 用共享密钥保护完整性和来源，不负责保密。");

    const key = Buffer.from("demo-shared-secret", "utf8");
    const message = "user=42&role=reader";
    const mac = createHmac("sha256", key).update(message).digest();
    const expected = createHmac("sha256", key).update(message).digest();
    const tampered = createHmac("sha256", key).update("user=42&role=admin").digest();

    showJson("HMAC 校验", {
        message,
        macHexPrefix: mac.toString("hex").slice(0, 16),
        valid: timingSafeEqual(mac, expected),
        tamperedValid: timingSafeEqual(mac, tampered)
    });
    note("常见坑", "比较 MAC 时不要用普通字符串相等做安全边界；真实服务应使用 constant-time 比较。");
}

// =============================================================================
// 30.5 随机数：randomBytes 与 randomUUID
// =============================================================================
//
// C++ 对照：randomBytes 类似从系统 CSPRNG 读取字节；不是 std::rand 这类伪随机演示工具。
//
// C++ 思维提示：随机值用于 token、nonce、salt、IV 等安全边界时，必须来自密码学安全随机源。
// 教程输出不能打印真实 token，以免养成日志泄漏习惯。

function demoRandomApis(): void {
    section("30.5 随机数：randomBytes 与 randomUUID");
    note("C++ 对照", "randomBytes 使用密码学安全随机源；randomUUID 生成 RFC 4122 风格 UUID。");

    const token = randomBytes(16);
    const uuid = randomUUID();

    showJson("随机 API 形状", {
        randomBytes: {
            requestedBytes: 16,
            actualBytes: token.byteLength,
            valuePrinted: false
        },
        randomUUID: {
            matchesV4Shape: /^[0-9a-f]{8}-[0-9a-f]{4}-4[0-9a-f]{3}-[89ab][0-9a-f]{3}-[0-9a-f]{12}$/.test(uuid),
            valuePrinted: false
        }
    });
    note("可重复运行", "本章不输出随机 token/UUID 的实际值，只验证长度和格式，避免每次运行输出变化。");
}

// =============================================================================
// 30.6 密码哈希：PBKDF2 故意慢，scrypt 更偏向内存硬
// =============================================================================
//
// C++ 对照：密码哈希不是普通 SHA256(password)。PBKDF2/scrypt 通过盐和成本参数抵抗暴力破解。
//
// C++ 思维提示：salt 不是 secret，它防止彩虹表和同密码同摘要；pepper 才是服务端额外 secret，
// 但 pepper 管理属于系统安全设计。

async function demoPasswordHashing(): Promise<void> {
    section("30.6 密码哈希：PBKDF2 故意慢，scrypt 更偏向内存硬");
    note("C++ 对照", "密码哈希要用专门 KDF，不要直接 sha256(password)。");

    const password = "correct horse battery staple";
    const salt = "fixed-demo-salt";
    const pbkdf2Key = await pbkdf2Async(password, salt, 1_000, 32, "sha256");
    const scryptKey = scryptSync(password, salt, 32, {
        N: 1024,
        r: 8,
        p: 1
    });

    showJson("密码哈希派生", {
        salt,
        pbkdf2: {
            iterations: 1_000,
            digest: "sha256",
            hexPrefix: pbkdf2Key.toString("hex").slice(0, 24)
        },
        scrypt: {
            cost: { N: 1024, r: 8, p: 1 },
            hexPrefix: scryptKey.toString("hex").slice(0, 24)
        }
    });
    note("安全边界", "本章参数为了快速演示而偏小；生产参数要按当前硬件、延迟预算和安全基线评估。");
}

// =============================================================================
// 30.7 对称加密入门：AES-256-GCM
// =============================================================================
//
// C++ 对照：AES-GCM 是带认证的对称加密模式，输出 ciphertext 和 auth tag；解密时 tag
// 校验失败就应该拒绝明文。
//
// C++ 思维提示：同一 key 下不要复用 GCM IV。固定 IV 只适合本章确定性演示，生产必须使用
// 安全随机或严格唯一的 IV 策略。

function encryptAesGcm(plainText: string, key: Buffer, iv: Buffer): { cipherText: Buffer; authTag: Buffer } {
    const cipher = createCipheriv("aes-256-gcm", key, iv);
    const cipherText = Buffer.concat([cipher.update(plainText, "utf8"), cipher.final()]);
    return {
        cipherText,
        authTag: cipher.getAuthTag()
    };
}

function decryptAesGcm(cipherText: Buffer, authTag: Buffer, key: Buffer, iv: Buffer): string {
    const decipher = createDecipheriv("aes-256-gcm", key, iv);
    decipher.setAuthTag(authTag);
    return Buffer.concat([decipher.update(cipherText), decipher.final()]).toString("utf8");
}

function demoSymmetricEncryption(): void {
    section("30.7 对称加密入门：AES-256-GCM");
    note("C++ 对照", "对称加密用同一把 key 加密和解密；GCM 还会校验 auth tag。");

    const key = Buffer.alloc(32, 0x11);
    const iv = Buffer.alloc(12, 0x22);
    const plainText = "deploy-token:demo";
    const encrypted = encryptAesGcm(plainText, key, iv);
    const decrypted = decryptAesGcm(encrypted.cipherText, encrypted.authTag, key, iv);

    showJson("AES-GCM 固定样例", {
        keyPrinted: false,
        ivHexForDemoOnly: iv.toString("hex"),
        cipherTextHex: encrypted.cipherText.toString("hex"),
        authTagHex: encrypted.authTag.toString("hex"),
        decrypted
    });
    note("常见坑", "hash 不能解密；encryption 才提供保密性。AES-GCM 的 IV 复用会破坏安全性。");
}

// =============================================================================
// 30.8 工程安全边界：不要自创密码学，secret 不进日志
// =============================================================================
//
// C++ 对照：像 C++ 里不要手写自己的 TLS、随机数生成器或块加密模式；TS/Node 里也一样。
//
// C++ 思维提示：安全代码的正确性依赖算法、参数、密钥管理、错误处理和审计，远不只是 API
// 调通。能编译通过不代表安全。

function demoSecurityChecklist(): void {
    section("30.8 工程安全边界：不要自创密码学，secret 不进日志");
    note("C++ 对照", "密码学 API 是底层能力，工程上还需要密钥管理、轮换、审计和最小暴露。");

    showJson("安全 checklist", [
        {
            topic: "hash vs encryption",
            rule: "hash proves integrity; encryption protects confidentiality"
        },
        {
            topic: "password storage",
            rule: "use PBKDF2/scrypt/Argon2-like KDF with salt and reviewed parameters"
        },
        {
            topic: "random",
            rule: "tokens, salts and IVs need CSPRNG; do not use Math.random"
        },
        {
            topic: "secrets",
            rule: "do not hard-code production secrets or print them in logs"
        },
        {
            topic: "algorithms",
            rule: "use reviewed algorithms and library defaults; do not design custom crypto"
        }
    ]);
    note("本章复盘", "crypto 能力来自运行时；TypeScript 负责形状检查，安全性来自正确算法、参数和运行时边界。");
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run chapter 30 demos. */
export async function runChapter(): Promise<void> {
    demoCryptoOverview();
    demoHashBasics();
    demoChunkedHash();
    demoHmac();
    demoRandomApis();
    await demoPasswordHashing();
    demoSymmetricEncryption();
    demoSecurityChecklist();
}

await runIfMain(import.meta.url, runChapter);
