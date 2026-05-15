import { note, runIfMain, section, showJson } from "../shared/chapter.js";

export interface Identified {
  readonly id: string;
}

export abstract class Account implements Identified {
  #balance: number;

  protected constructor(
    public readonly id: string,
    initialBalance: number
  ) {
    this.#balance = initialBalance;
  }

  get balance(): number {
    return this.#balance;
  }

  deposit(amount: number): void {
    if (amount <= 0) {
      throw new RangeError("amount must be positive");
    }
    this.#balance += amount;
  }

  protected withdrawInternal(amount: number): void {
    if (amount > this.#balance) {
      throw new RangeError("insufficient balance");
    }
    this.#balance -= amount;
  }

  abstract withdraw(amount: number): void;
}

export class SavingsAccount extends Account {
  constructor(id: string, initialBalance: number) {
    super(id, initialBalance);
  }

  withdraw(amount: number): void {
    this.withdrawInternal(amount);
  }
}

export class Repository<T extends Identified> {
  readonly #items = new Map<string, T>();

  add(item: T): void {
    this.#items.set(item.id, item);
  }

  get(id: string): T | undefined {
    return this.#items.get(id);
  }

  list(): T[] {
    return [...this.#items.values()];
  }
}

export function runChapter(): void {
  section("第 6 章：面向对象与结构化类型");
  note("C++ 对照", "TS 的 `private`/`protected` 是类型系统约束，`#field` 才是 JS 运行时私有字段。");
  note("结构化类型", "类实例也按成员形状参与类型兼容，接口更像能力描述，不像纯虚基类。");

  const account = new SavingsAccount("acct-1", 100);
  account.deposit(50);
  account.withdraw(30);

  const repo = new Repository<SavingsAccount>();
  repo.add(account);
  showJson("账户列表", repo.list().map((item) => ({ id: item.id, balance: item.balance })));
}

await runIfMain(import.meta.url, runChapter);
