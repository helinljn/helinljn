// =============================================================================
// 第 16 章：类与对象
// =============================================================================
//
// 【学习目标】
//   1. 掌握 class、constructor、实例/静态属性与方法、getter/setter
//   2. 区分 TS private/protected/public 与 JS # 私有字段
//   3. 理解 readonly、参数属性和 implements 的工程边界
//
// 【运行方式】
//   npm run build
//   node dist/05_面向对象篇/chapter16_类与对象.js
//   或 npm run chapter -- 16（registry 已更新后）
//
// 【TypeScript 版本】6.0.x
//
// =============================================================================

import { note, runIfMain, section, showJson } from "../shared/chapter.js";

interface Identified {
    readonly id: string;
}

interface AccountSnapshot extends Identified {
    owner: string;
    balance: number;
    status: string;
}

class BankAccount implements Identified {
    static #nextSerial = 1;
    static totalOpened = 0;

    #balance: number;
    protected status = "open";
    private readonly auditTrail: string[] = [];

    constructor(
        public readonly id: string,
        public owner: string,
        initialBalance: number
    ) {
        if (initialBalance < 0) {
            throw new RangeError("initial balance must not be negative");
        }
        this.#balance = initialBalance;
        BankAccount.totalOpened += 1;
        this.record(`opened:${initialBalance}`);
    }

    static open(owner: string, initialBalance: number): BankAccount {
        const id = `acct-${BankAccount.#nextSerial.toString().padStart(3, "0")}`;
        BankAccount.#nextSerial += 1;
        return new BankAccount(id, owner, initialBalance);
    }

    get balance(): number {
        return this.#balance;
    }

    get auditCount(): number {
        return this.auditTrail.length;
    }

    set displayOwner(value: string) {
        const trimmed = value.trim();
        if (trimmed === "") {
            throw new Error("owner must not be empty");
        }
        this.owner = trimmed;
        this.record(`owner:${trimmed}`);
    }

    deposit(amount: number): void {
        this.requirePositive(amount);
        this.#balance += amount;
        this.record(`deposit:${amount}`);
    }

    withdraw(amount: number): void {
        this.requirePositive(amount);
        if (amount > this.#balance) {
            throw new RangeError("insufficient balance");
        }
        this.#balance -= amount;
        this.record(`withdraw:${amount}`);
    }

    close(): void {
        this.status = "closed";
        this.record("closed");
    }

    snapshot(): AccountSnapshot {
        return {
            id: this.id,
            owner: this.owner,
            balance: this.balance,
            status: this.status
        };
    }

    protected record(event: string): void {
        this.auditTrail.push(event);
    }

    private requirePositive(amount: number): void {
        if (amount <= 0) {
            throw new RangeError("amount must be positive");
        }
    }
}

class AccountRepository<T extends Identified> {
    readonly #items = new Map<string, T>();

    add(item: T): void {
        this.#items.set(item.id, item);
    }

    get(id: string): T | undefined {
        return this.#items.get(id);
    }

    list(): readonly T[] {
        return [...this.#items.values()];
    }
}

function describeIdentified(item: Identified): string {
    return `#${item.id}`;
}

function transferFunds(from: BankAccount, to: BankAccount, amount: number): {
    from: AccountSnapshot;
    to: AccountSnapshot;
} {
    from.withdraw(amount);
    to.deposit(amount);
    return {
        from: from.snapshot(),
        to: to.snapshot()
    };
}

// =============================================================================
// 16.1 class、constructor 和实例成员
// =============================================================================
//
// C++ 对照：
//   class 语法看起来熟悉，但 JS 对象仍然基于原型和属性。
//   constructor 是初始化入口，实例字段会落在对象实例上。

function demoClassConstructorAndInstances(): void {
    section("16.1 class、constructor 和实例成员");
    note("C++ 对照", "TS class 编译后仍是 JS class/prototype 机制，不是 C++ 对象模型。");

    const account = new BankAccount("acct-manual", "Ada", 100);
    account.deposit(25);
    account.withdraw(10);

    showJson("实例成员", {
        snapshot: account.snapshot(),
        auditCount: account.auditCount,
        described: describeIdentified(account)
    });
    note("输出解释", "id/owner 是实例属性，balance 通过 getter 暴露，内部余额存放在 #balance。");
}

// =============================================================================
// 16.2 静态属性与静态方法
// =============================================================================
//
// C++ 对照：
//   static 成员挂在类构造器上，而不是每个实例上。
//   它适合工厂方法、计数器和共享配置。

function demoStaticMembers(): void {
    section("16.2 静态属性与静态方法");
    note("C++ 对照", "static 成员属于类本身；实例只能通过类名间接共享这些状态。");

    const first = BankAccount.open("Grace", 200);
    const second = BankAccount.open("Linus", 300);

    showJson("静态成员", {
        first: first.snapshot(),
        second: second.snapshot(),
        totalOpened: BankAccount.totalOpened
    });
    note("输出解释", "open() 统一生成账号 id；totalOpened 记录本进程中创建过的账户数量。");
}

// =============================================================================
// 16.3 getter、setter 与封装验证
// =============================================================================
//
// C++ 对照：
//   getter/setter 像属性语法包住方法调用。
//   它适合只读视图、校验写入和保持内部不变量。

function demoGetterSetter(): void {
    section("16.3 getter、setter 与封装验证");
    note("C++ 对照", "getter/setter 让调用点像访问字段，但内部可以执行校验逻辑。");

    const account = BankAccount.open("Margaret", 150);
    account.displayOwner = "  Margaret Hamilton  ";
    account.deposit(50);

    showJson("getter/setter", {
        owner: account.owner,
        balance: account.balance,
        auditCount: account.auditCount
    });
    note("输出解释", "displayOwner setter 会 trim 并校验；balance 只读暴露，外部不能直接改 #balance。");
}

// =============================================================================
// 16.4 TS private/protected/public vs JS # 私有字段
// =============================================================================
//
// C++ 对照：
//   TS private/protected 是类型检查规则，编译到 JS 后不提供真正运行时隔离。
//   #field 是 JS 运行时私有字段，外部无法通过属性名访问。
//
// 常见坑：
//   不要把 TS private 当成安全边界；它主要防止同一 TS 项目里的误用。

function demoPrivateBoundaries(): void {
    section("16.4 TS private/protected/public vs JS # 私有字段");
    note("C++ 对照", "TS 访问控制编译后会消失；# 私有字段才由 JS 运行时维护。");

    const account = BankAccount.open("Barbara", 500);
    account.deposit(20);
    const visibleKeys = Object.keys(account as unknown as Record<string, unknown>);
    const hasRuntimeBalanceProperty = "#balance" in (account as unknown as Record<string, unknown>);

    showJson("访问控制边界", {
        visibleKeys,
        hasRuntimeBalanceProperty,
        snapshot: account.snapshot()
    });
    note("输出解释", "auditTrail/status 这类 TS private/protected 仍是普通字段；#balance 不会出现在对象键里。");
}

// =============================================================================
// 16.5 readonly、参数属性和 implements
// =============================================================================
//
// C++ 对照：
//   readonly 是类型层只读，不是运行时 const 对象。
//   implements 检查类实例是否满足接口，但不会在运行时附加接口表。

function demoReadonlyAndImplements(): void {
    section("16.5 readonly、参数属性和 implements");
    note("C++ 对照", "implements 更像结构化契约检查，不像 C++ 继承一个纯虚基类。");

    const account = BankAccount.open("Donald", 90);
    const identified: Identified = account;

    showJson("readonly 与 implements", {
        id: identified.id,
        described: describeIdentified(identified),
        snapshot: account.snapshot()
    });
    note("常见坑", "implements 不会让运行时对象带上接口元数据；它只在编译期确认形状兼容。");
}

// =============================================================================
// 16.6 工程场景：账户仓库
// =============================================================================
//
// C++ 对照：
//   仓库类把存储细节封装起来，外部只按接口操作。
//   泛型约束让仓库能复用在所有带 id 的对象上。

function demoRepositoryScenario(): void {
    section("16.6 工程场景：账户仓库");
    note("C++ 对照", "类负责维护不变量，泛型仓库负责复用存储逻辑。");

    const repo = new AccountRepository<BankAccount>();
    const first = BankAccount.open("Ada", 100);
    const second = BankAccount.open("Grace", 250);
    first.deposit(15);
    second.withdraw(50);
    repo.add(first);
    repo.add(second);

    showJson("账户仓库", {
        all: repo.list().map((account) => account.snapshot()),
        found: repo.get(first.id)?.snapshot() ?? null
    });
    note("输出解释", "仓库暴露 add/get/list，不暴露内部 Map；账户自己维护余额不变量。");
}

// =============================================================================
// 16.7 运行时边界：用方法维护不变量，用快照暴露状态
// =============================================================================
//
// C++ 对照：
//   C++ 类常用 private 字段和 const 成员函数维护不变量。
//   TS class 也可以表达这个意图，但要分清类型层 private 与 JS # 私有字段。
//
// 真实场景：
//   金额、库存、配额这类数据不能让外部任意改字段。
//   入口层应调用方法完成校验、状态变更和审计记录，再把只读快照传给外部。
//
// 常见坑：
//   为了“方便测试”把内部字段改成 public，会让业务不变量失去集中维护点。
//   更稳的做法是提供查询方法、快照方法和少量明确的命令方法。

function demoRuntimeInvariantBoundary(): void {
    section("16.7 运行时边界：方法维护不变量");
    note("C++ 对照", "把余额变更收敛到 deposit/withdraw，类似把状态修改封进成员函数。");

    const source = BankAccount.open("Source", 500);
    const target = BankAccount.open("Target", 100);
    const afterTransfer = transferFunds(source, target, 125);
    let rejected = "";
    try {
        transferFunds(source, target, 1000);
    } catch (error: unknown) {
        rejected = error instanceof Error ? error.message : String(error);
    }

    showJson("账户不变量边界", {
        afterTransfer,
        rejected,
        sourceAuditCount: source.auditCount,
        targetAuditCount: target.auditCount
    });
    note("输出解释", "外部只能通过方法转账；余额不足会被 withdraw 拒绝，不会留下半更新状态。");
    note("常见坑", "快照是对外阅读模型，不应把内部可变集合或私有字段直接暴露给调用方。");
}

// =============================================================================
// 16.8 本章复盘
// =============================================================================
//
// C++ 对照：
//   TS class 是 JS 原型模型上的语法与类型增强。
//   学习重点是明确哪些约束存在于编译期，哪些存在于运行时。

function demoChapterReview(): void {
    section("16.8 本章复盘");
    note("C++ 对照", "不要把 TS class 当成 C++ class；运行时模型和访问控制边界不同。");

    const summary = [
        "constructor 初始化实例字段",
        "static 成员属于类构造器",
        "getter/setter 用属性语法包住校验逻辑",
        "TS private/protected 是类型层约束",
        "#field 是 JS 运行时私有字段",
        "readonly 与 implements 都主要服务于编译期边界",
        "类适合封装不变量，仓库适合封装存储细节",
        "对外暴露快照和命令方法，而不是暴露内部可变字段"
    ];

    showJson("关键结论", summary);
}

// =============================================================================
// 章节入口
// =============================================================================

/** Run all demos for chapter 16. */
export function runChapter(): void {
    demoClassConstructorAndInstances();
    demoStaticMembers();
    demoGetterSetter();
    demoPrivateBoundaries();
    demoReadonlyAndImplements();
    demoRepositoryScenario();
    demoRuntimeInvariantBoundary();
    demoChapterReview();
}

await runIfMain(import.meta.url, runChapter);
