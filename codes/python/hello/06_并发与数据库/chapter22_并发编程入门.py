# =============================================================================
# 第 22 章：并发编程入门（threading、multiprocessing、asyncio 模块）
# =============================================================================
#
# 【学习目标】
#   1. 理解并发与并行的区别，掌握 Python 并发模型
#   2. 掌握 threading 模块：线程创建、同步原语、线程池
#   3. 理解 GIL（全局解释器锁）对多线程的影响
#   4. 掌握 multiprocessing 模块：进程创建、进程池、进程间通信
#   5. 理解异步编程思想，掌握 async/await 语法基础
#   6. 知道在什么场景下选择多线程、多进程还是异步
#
# 【这些模块解决什么问题？】
#
#   threading 模块：
#     - 在同一进程中并发执行多个任务
#     - 适合 I/O 密集型任务（网络请求、文件读写、数据库查询）
#     - 多个线程可以共享同一进程的内存空间
#     - 提供 Lock、Event、Queue 等同步工具避免竞态条件
#
#   multiprocessing 模块：
#     - 绕过 GIL 限制，真正利用多核 CPU 并行计算
#     - 适合 CPU 密集型任务（图像处理、数学计算、数据压缩）
#     - 每个进程有独立内存空间，通过 Queue/Pipe 通信
#     - ProcessPoolExecutor 提供简洁的进程池接口
#
#   asyncio 模块：
#     - 单线程内的协作式并发（协程）
#     - 适合大量 I/O 密集型任务（爬虫、高并发服务器）
#     - 比多线程更轻量，可以同时管理数万个并发任务
#     - 通过 async/await 语法让异步代码像同步代码一样易读
#
# 【三种并发模型对比】
#
#   ┌──────────────┬──────────────┬──────────────┬──────────────┐
#   │              │  threading   │multiprocessing│   asyncio    │
#   ├──────────────┼──────────────┼──────────────┼──────────────┤
#   │ 并发单元     │   线程        │    进程       │    协程      │
#   │ 适合场景     │ I/O 密集型   │ CPU 密集型   │ I/O 密集型   │
#   │ 共享内存     │    是        │    否        │    是        │
#   │ 绕过 GIL    │    否        │    是        │    否        │
#   │ 启动开销     │    小        │    大        │   极小       │
#   │ 数据传递     │ 共享变量     │ Queue/Pipe   │ 直接传递     │
#   │ 调试难度     │   较难       │   中等       │   较难       │
#   └──────────────┴──────────────┴──────────────┴──────────────┘
#
# 【与 C/C++ 对比】
#   C/C++：pthread（POSIX 线程）、OpenMP、std::thread、std::async
#   Python threading：类似 C++ std::thread，但受 GIL 限制
#   Python multiprocessing：类似 fork/exec + IPC 机制
#   Python asyncio：类似 C++ Boost.Asio 或 Node.js 事件循环
#   最大区别：Python 有 GIL，多线程无法真正并行执行 CPU 密集任务
#
# 【运行方式】
#   python chapter22_并发编程入门.py
#
# =============================================================================

import asyncio
import multiprocessing
import os
import queue
import sys
import threading
import time
from concurrent.futures import ProcessPoolExecutor, ThreadPoolExecutor, as_completed
from pathlib import Path
from typing import Any, Optional


# =============================================================================
# 22.1 并发基础概念：并发 vs 并行，以及 GIL
# =============================================================================

def demo_concurrency_concepts() -> None:
    """解释并发、并行、GIL 等核心概念。"""
    print("=" * 60)
    print("22.1 并发基础概念")
    print("=" * 60)

    print("【并发（Concurrency）vs 并行（Parallelism）】")
    print()
    print("  并发：同一时间段内处理多个任务（可以交替执行）")
    print("        就像一个人同时炒菜和等水开——交替切换注意力")
    print()
    print("  并行：同一时刻真正同时执行多个任务（需要多核 CPU）")
    print("        就像多个厨师同时各自做不同的菜")
    print()
    print("  ┌─────────────────────────────────────────┐")
    print("  │  并发（单核）：A─A─B─B─A─A─B─B         │")
    print("  │  并行（多核）：A─A─A─A─A               │")
    print("  │               B─B─B─B─B               │")
    print("  └─────────────────────────────────────────┘")
    print()

    print("【GIL（全局解释器锁）——Python 的特殊机制】")
    print()
    print("  GIL 是 CPython 解释器中的一把互斥锁：")
    print("  - 同一时刻，只允许一个线程执行 Python 字节码")
    print("  - 即使有多个 CPU 核心，Python 线程也无法真正并行")
    print()
    print("  为什么 Python 有 GIL？")
    print("  - 历史原因：简化 CPython 的内存管理（引用计数）")
    print("  - 保证线程安全：避免多线程同时修改 Python 对象")
    print("  - 代价：CPU 密集任务中多线程不能提速")
    print()
    print("  GIL 对不同类型任务的影响：")
    print()
    print("  ┌──────────────────┬────────────────────────────────────┐")
    print("  │ 任务类型         │ 多线程效果                         │")
    print("  ├──────────────────┼────────────────────────────────────┤")
    print("  │ I/O 密集型       │ 有效！I/O 等待时会释放 GIL          │")
    print("  │ (网络/文件读写)  │ 多线程可以真正并发处理 I/O 等待     │")
    print("  ├──────────────────┼────────────────────────────────────┤")
    print("  │ CPU 密集型       │ 无效甚至更慢！线程切换有开销        │")
    print("  │ (数学计算/压缩)  │ 应使用 multiprocessing 代替         │")
    print("  └──────────────────┴────────────────────────────────────┘")
    print()
    print("  【关键结论】")
    print("  I/O 密集型 → 使用 threading 或 asyncio")
    print("  CPU 密集型 → 使用 multiprocessing")
    print()

    print("【如何判断任务类型？】")
    print()
    print("  I/O 密集型（程序大部分时间在等待 I/O）：")
    print("    - 网络请求（HTTP、数据库查询、API 调用）")
    print("    - 文件读写（读大文件、写日志）")
    print("    - 等待用户输入、等待传感器数据")
    print()
    print("  CPU 密集型（程序大部分时间在做计算）：")
    print("    - 图像/视频处理")
    print("    - 加密/解密、数据压缩")
    print("    - 机器学习训练、大量数学运算")
    print("    - 排序、搜索大量数据")
    print()


# =============================================================================
# 22.2 threading 基础：线程创建与生命周期
# =============================================================================

def demo_thread_basics() -> None:
    """演示线程的创建方式、启动和等待。"""
    print("\n" + "=" * 60)
    print("22.2 threading 基础：线程创建与生命周期")
    print("=" * 60)

    print("【线程的基本概念】")
    print()
    print("  线程（Thread）是操作系统调度的最小执行单元")
    print("  同一进程的所有线程共享：内存、文件句柄、全局变量")
    print("  每个线程有自己独立的：调用栈、寄存器状态")
    print()

    # ── 方式一：使用函数创建线程 ─────────────────────────
    print("【方式一：将函数传给 Thread（最常用）】")
    print()

    def download_file(filename: str, duration: float) -> None:
        """
        模拟下载文件（用 sleep 模拟 I/O 等待）。

        Args:
            filename: 文件名
            duration: 下载耗时（秒）
        """
        tid = threading.current_thread().name  # 获取当前线程名称
        print(f"  [{tid}] 开始下载: {filename}")
        time.sleep(duration)                   # 模拟网络 I/O 等待
        print(f"  [{tid}] 下载完成: {filename} ({duration:.1f}秒)")

    print("  串行执行（单线程）：")
    start = time.perf_counter()
    download_file("文件A.zip", 0.3)
    download_file("文件B.zip", 0.3)
    download_file("文件C.zip", 0.3)
    elapsed = time.perf_counter() - start
    print(f"  串行总耗时: {elapsed:.2f}秒\n")

    print("  并发执行（多线程）：")
    start = time.perf_counter()

    # 创建三个线程
    # target：线程执行的函数
    # args：传给函数的位置参数（注意是元组，单个参数也要加逗号）
    # name：线程名称（可选，方便调试）
    t1 = threading.Thread(target=download_file, args=("文件A.zip", 0.3), name="Thread-A")
    t2 = threading.Thread(target=download_file, args=("文件B.zip", 0.3), name="Thread-B")
    t3 = threading.Thread(target=download_file, args=("文件C.zip", 0.3), name="Thread-C")

    # start() 启动线程（非阻塞，立即返回）
    t1.start()
    t2.start()
    t3.start()

    # join() 等待线程完成（阻塞，直到对应线程结束）
    t1.join()
    t2.join()
    t3.join()

    elapsed = time.perf_counter() - start
    print(f"  多线程总耗时: {elapsed:.2f}秒（约为串行的 1/3）\n")

    # ── 方式二：继承 Thread 类 ──────────────────────────
    print("【方式二：继承 Thread 类（适合复杂线程逻辑）】")
    print()

    class WorkerThread(threading.Thread):
        """
        自定义线程类：继承 threading.Thread。
        适合线程需要维护状态或返回结果的场景。
        """

        def __init__(self, task_id: int, data: list[int]) -> None:
            """
            Args:
                task_id: 任务编号
                data:    要处理的数据
            """
            # 必须调用父类 __init__
            super().__init__(name=f"Worker-{task_id}")
            self.task_id = task_id
            self.data = data
            self.result: Optional[int] = None  # 用于存储计算结果

        def run(self) -> None:
            """
            重写 run() 方法，这是线程的执行体。
            注意：调用 start() 会在新线程中自动调用 run()
            不要直接调用 run()！直接调用不会创建新线程。
            """
            print(f"  [{self.name}] 开始处理任务 {self.task_id}...")
            time.sleep(0.1)  # 模拟处理耗时

            # 计算数据之和（模拟 CPU 工作）
            self.result = sum(self.data)
            print(f"  [{self.name}] 任务 {self.task_id} 完成，结果: {self.result}")

    # 创建并启动工作线程
    workers = [
        WorkerThread(task_id=i, data=list(range(i * 100, (i + 1) * 100)))
        for i in range(3)
    ]

    for worker in workers:
        worker.start()

    for worker in workers:
        worker.join()

    # 汇总结果（join() 之后才能安全访问 result）
    total = sum(w.result for w in workers)
    print(f"  所有任务结果之和: {total}\n")

    # ── 守护线程（Daemon Thread）────────────────────────
    print("【守护线程（Daemon Thread）】")
    print()
    print("  普通线程：主程序会等待所有普通线程结束后才退出")
    print("  守护线程：主程序退出时，守护线程会被强制终止")
    print()
    print("  适合守护线程的场景：后台任务（心跳检测、日志刷新）")
    print()

    def background_monitor() -> None:
        """模拟后台监控线程（守护线程）。"""
        count = 0
        while True:
            count += 1
            print(f"  [Monitor] 心跳检测 #{count}，进程仍在运行...")
            time.sleep(0.5)
            if count >= 2:
                break  # 演示用，实际守护线程通常不会主动停止

    monitor = threading.Thread(target=background_monitor, name="Monitor")
    monitor.daemon = True    # 设置为守护线程（必须在 start() 之前设置）
    monitor.start()
    time.sleep(1.2)          # 主线程等待 1.2 秒
    print("  主线程完成工作，即将退出（守护线程会随之终止）")
    # 不需要 join() 守护线程，主线程结束后守护线程自动终止
    print()

    # ── 线程常用属性和方法 ───────────────────────────────
    print("【Thread 常用属性与方法】")
    print()

    t = threading.Thread(target=lambda: time.sleep(0.1), name="示例线程")
    t.start()

    print(f"  t.name              = {t.name}")          # 线程名称
    print(f"  t.ident             = {t.ident}")         # 线程 ID
    print(f"  t.daemon            = {t.daemon}")        # 是否守护线程
    print(f"  t.is_alive()        = {t.is_alive()}")    # 是否还在运行
    print(f"  threading.active_count() = {threading.active_count()}")  # 活跃线程数
    print(f"  threading.current_thread() = {threading.current_thread().name}")
    print(f"  threading.main_thread()    = {threading.main_thread().name}")

    t.join()
    print(f"  join() 后 t.is_alive() = {t.is_alive()}")
    print()


# =============================================================================
# 22.3 线程同步：避免竞态条件
# =============================================================================

def demo_thread_synchronization() -> None:
    """演示线程同步工具：Lock、RLock、Semaphore、Event。"""
    print("\n" + "=" * 60)
    print("22.3 线程同步：避免竞态条件")
    print("=" * 60)

    print("【什么是竞态条件（Race Condition）？】")
    print()
    print("  多个线程同时读写共享数据，导致结果不确定或错误")
    print("  类比：两个人同时从银行账户取钱，余额可能出错")
    print()

    # ── 演示竞态条件 ─────────────────────────────────────
    print("【演示竞态条件（不加锁的错误示例）】")
    print()

    counter_no_lock = 0  # 共享变量（无保护）

    def increment_without_lock(n: int) -> None:
        """不加锁地增加计数器（存在竞态条件）。"""
        nonlocal counter_no_lock
        for _ in range(n):
            # 这里的 += 操作在 Python 字节码层面不是原子的：
            # 1. 读取 counter_no_lock 的值
            # 2. 加 1
            # 3. 写回 counter_no_lock
            # 步骤 1-3 之间，GIL 可能切换到其他线程，导致丢失更新
            counter_no_lock += 1

    threads = [
        threading.Thread(target=increment_without_lock, args=(10000,))
        for _ in range(5)
    ]
    for t in threads:
        t.start()
    for t in threads:
        t.join()

    expected = 5 * 10000
    print(f"  期望值: {expected}")
    print(f"  实际值: {counter_no_lock}")
    if counter_no_lock != expected:
        print(f"  ⚠️  数据丢失！少了 {expected - counter_no_lock} 次更新")
    else:
        print(f"  （本次运行恰好没有出现问题，但不保证每次都正确）")
    print()

    # ── Lock（互斥锁）────────────────────────────────────
    print("【Lock（互斥锁）：最基本的同步原语】")
    print()
    print("  Lock 就像一把门锁：")
    print("  acquire()：拿锁（如果锁被别人占用，则等待）")
    print("  release()：放锁（让其他线程可以进入）")
    print("  同一时刻只有一个线程可以持有锁")
    print()

    counter_with_lock = 0
    lock = threading.Lock()  # 创建互斥锁

    def increment_with_lock(n: int) -> None:
        """使用 Lock 保护共享变量（正确做法）。"""
        nonlocal counter_with_lock
        for _ in range(n):
            # 推荐：使用 with 语句自动 acquire/release
            # 即使发生异常，with 也会确保锁被释放
            with lock:
                counter_with_lock += 1

    threads = [
        threading.Thread(target=increment_with_lock, args=(10000,))
        for _ in range(5)
    ]
    for t in threads:
        t.start()
    for t in threads:
        t.join()

    print(f"  加锁后期望值: {expected}")
    print(f"  加锁后实际值: {counter_with_lock}")
    print(f"  结果{'正确 ✓' if counter_with_lock == expected else '错误 ✗'}")
    print()

    print("  手动 acquire/release（等效写法，但不推荐）：")
    print("    lock.acquire()    # 获取锁（阻塞直到成功）")
    print("    try:")
    print("        counter += 1  # 临界区（受保护的代码）")
    print("    finally:")
    print("        lock.release() # 必须确保释放，即使发生异常")
    print()
    print("  lock.acquire(blocking=False)  # 非阻塞，失败立即返回 False")
    print("  lock.acquire(timeout=2.0)     # 最多等待 2 秒")
    print()

    # ── RLock（可重入锁）─────────────────────────────────
    print("【RLock（可重入锁）：同一线程可以多次获取】")
    print()
    print("  普通 Lock 的问题：同一线程第二次 acquire() 会死锁！")
    print("  RLock 解决了这个问题：同一线程可以多次 acquire()")
    print("  但每次 acquire() 都需要对应一次 release()")
    print()

    rlock = threading.RLock()  # 可重入锁

    def recursive_task(depth: int) -> int:
        """递归任务，演示 RLock 允许同一线程重复获取。"""
        with rlock:                         # 第 1 次获取
            if depth <= 0:
                return 0
            with rlock:                     # 第 2 次获取（同一线程，RLock 允许）
                return depth + recursive_task(depth - 1)

    result = recursive_task(5)
    print(f"  RLock 递归调用结果（不死锁）: {result}")
    print()

    # ── Semaphore（信号量）──────────────────────────────
    print("【Semaphore（信号量）：限制并发数量】")
    print()
    print("  Semaphore 内部维护一个计数器（初始值 = 允许的并发数）")
    print("  acquire()：计数器减 1（若为 0 则等待）")
    print("  release()：计数器加 1")
    print("  适合限制并发数量：如同时最多 3 个线程访问数据库")
    print()

    # 最多允许 3 个线程同时访问"资源"
    semaphore = threading.Semaphore(3)

    # 记录同时运行的线程数
    active_count_lock = threading.Lock()
    max_concurrent = [0]  # 用列表包装，方便在嵌套函数中修改
    current_concurrent = [0]

    def access_limited_resource(worker_id: int) -> None:
        """模拟访问有限资源（如数据库连接池）。"""
        with semaphore:
            # 进入临界区
            with active_count_lock:
                current_concurrent[0] += 1
                max_concurrent[0] = max(max_concurrent[0], current_concurrent[0])

            print(f"  Worker-{worker_id:02d} 正在使用资源 "
                  f"（当前并发数: {current_concurrent[0]}）")
            time.sleep(0.15)  # 模拟使用资源

            with active_count_lock:
                current_concurrent[0] -= 1

    print("  启动 8 个线程，但最多 3 个同时访问资源：")
    threads = [
        threading.Thread(target=access_limited_resource, args=(i,))
        for i in range(8)
    ]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    print(f"  最大并发数: {max_concurrent[0]}（应该 ≤ 3）")
    print()

    # ── Event（事件）─────────────────────────────────────
    print("【Event（事件）：线程间信号通知】")
    print()
    print("  Event 相当于一个旗帜（Flag）：")
    print("  set()   ：竖起旗帜（通知所有等待的线程）")
    print("  clear() ：放下旗帜")
    print("  wait()  ：等待旗帜竖起（阻塞）")
    print("  is_set()：检查旗帜状态（非阻塞）")
    print()

    # 场景：数据加载线程通知多个工作线程
    data_ready_event = threading.Event()

    def data_loader() -> None:
        """加载数据的线程（生产者）。"""
        print("  [加载器] 开始加载数据...")
        time.sleep(0.3)  # 模拟数据加载耗时
        print("  [加载器] 数据加载完成，通知工作线程...")
        data_ready_event.set()  # 设置事件，唤醒所有等待的线程

    def data_worker(worker_id: int) -> None:
        """等待数据就绪后开始工作（消费者）。"""
        print(f"  [Worker-{worker_id}] 等待数据...")
        data_ready_event.wait()  # 阻塞，直到 event.set() 被调用
        print(f"  [Worker-{worker_id}] 数据就绪，开始处理！")

    loader = threading.Thread(target=data_loader, name="Loader")
    workers = [
        threading.Thread(target=data_worker, args=(i,), name=f"Worker-{i}")
        for i in range(3)
    ]

    # 先启动工作线程（它们会等待）
    for w in workers:
        w.start()
    # 再启动加载器
    loader.start()

    loader.join()
    for w in workers:
        w.join()
    print()

    # ── Condition（条件变量）─────────────────────────────
    print("【Condition（条件变量）：精细控制等待条件】")
    print()
    print("  Condition 结合了 Lock 和 Event 的功能：")
    print("  wait()  ：释放锁并等待通知（原子操作）")
    print("  notify()：唤醒一个等待的线程")
    print("  notify_all()：唤醒所有等待的线程")
    print()
    print("  经典场景：生产者-消费者问题（有界缓冲区）")
    print()

    # 有界缓冲区生产者-消费者
    buffer: list[int] = []
    MAX_SIZE = 3        # 缓冲区最大容量
    condition = threading.Condition()

    def producer(items: int) -> None:
        """生产者：向缓冲区添加数据。"""
        for i in range(items):
            with condition:
                # 等待缓冲区有空间
                while len(buffer) >= MAX_SIZE:
                    print(f"  [生产者] 缓冲区满({MAX_SIZE})，等待消费者...")
                    condition.wait()  # 释放锁并等待

                buffer.append(i)
                print(f"  [生产者] 生产: {i}，缓冲区: {buffer}")
                condition.notify()  # 通知消费者有新数据
            time.sleep(0.05)

    def consumer(items: int) -> None:
        """消费者：从缓冲区取出数据。"""
        for _ in range(items):
            with condition:
                # 等待缓冲区有数据
                while len(buffer) == 0:
                    print("  [消费者] 缓冲区空，等待生产者...")
                    condition.wait()  # 释放锁并等待

                item = buffer.pop(0)
                print(f"  [消费者] 消费: {item}，缓冲区: {buffer}")
                condition.notify()  # 通知生产者有空间了
            time.sleep(0.08)

    NUM_ITEMS = 6
    prod = threading.Thread(target=producer, args=(NUM_ITEMS,), name="Producer")
    cons = threading.Thread(target=consumer, args=(NUM_ITEMS,), name="Consumer")
    prod.start()
    cons.start()
    prod.join()
    cons.join()
    print()


# =============================================================================
# 22.4 线程通信：Queue
# =============================================================================

def demo_thread_queue() -> None:
    """演示使用 queue.Queue 进行线程间安全通信。"""
    print("\n" + "=" * 60)
    print("22.4 线程通信：queue.Queue")
    print("=" * 60)

    print("【为什么推荐 Queue 而不是共享变量？】")
    print()
    print("  共享变量 + Lock 的缺点：")
    print("  - 需要手动管理锁，容易忘记或出现死锁")
    print("  - 数据结构复杂时，锁的粒度难以把握")
    print()
    print("  Queue 的优点：")
    print("  - 内部已经处理好线程安全（内置锁机制）")
    print("  - 天然的生产者-消费者模式")
    print("  - 支持设置最大容量（避免内存耗尽）")
    print("  - put() / get() 语义清晰，代码更简洁")
    print()
    print("  Queue 类型：")
    print("  queue.Queue       ：FIFO 先进先出（最常用）")
    print("  queue.LifoQueue   ：LIFO 后进先出（类似栈）")
    print("  queue.PriorityQueue：按优先级出队（数值小的先出）")
    print()

    # ── 基本 Queue 用法 ──────────────────────────────────
    print("【基本使用：Queue 核心方法】")
    print()

    q: queue.Queue[int] = queue.Queue(maxsize=5)  # 最多存 5 个元素

    print("  put() 和 get() 方法：")
    q.put(10)               # 放入数据（若队列满则阻塞）
    q.put(20)
    q.put(30)

    print(f"  put 3 个元素后，qsize() = {q.qsize()}")
    print(f"  get() = {q.get()}")                          # 取出数据（若队列空则阻塞）
    print(f"  get() = {q.get()}")
    print(f"  get_nowait() = {q.get_nowait()}")            # 非阻塞取出
    print(f"  empty() = {q.empty()}")                      # 是否为空

    # task_done() 和 join() 配对使用
    print()
    print("  task_done() 和 join() 的用法（跟踪任务完成情况）：")
    print("  q.task_done()  → 通知 Queue 这个任务已处理完")
    print("  q.join()       → 等待所有已放入的任务都被 task_done()")
    print()

    # ── 实战：多线程任务队列 ─────────────────────────────
    print("【实战：多线程任务队列（经典生产者-消费者）】")
    print()

    # 任务队列：存放待处理的 URL（模拟）
    task_queue: queue.Queue[Optional[str]] = queue.Queue()
    result_queue: queue.Queue[tuple[str, str]] = queue.Queue()

    def url_worker(worker_id: int) -> None:
        """
        工作线程：从任务队列取 URL，模拟下载，结果放入结果队列。
        使用哨兵值（None）通知线程退出。
        """
        while True:
            # get() 阻塞直到有任务
            url = task_queue.get()

            if url is None:
                # 哨兵值：通知线程退出
                print(f"  [Worker-{worker_id}] 收到退出信号")
                task_queue.task_done()
                break

            # 模拟处理任务
            time.sleep(0.1)
            result = f"[200 OK] {url} 下载完成"
            result_queue.put((url, result))

            # 必须调用 task_done()，否则 q.join() 会永久阻塞
            task_queue.task_done()
            print(f"  [Worker-{worker_id}] 处理完成: {url}")

    # 创建 3 个工作线程
    NUM_WORKERS = 3
    worker_threads = []
    for i in range(NUM_WORKERS):
        t = threading.Thread(
            target=url_worker,
            args=(i,),
            name=f"Worker-{i}",
            daemon=True,        # 设为守护线程（主线程退出时自动终止）
        )
        t.start()
        worker_threads.append(t)

    # 生产者：放入 8 个任务
    urls = [f"https://example.com/file_{i:02d}.zip" for i in range(8)]
    print(f"  放入 {len(urls)} 个下载任务：")
    for url in urls:
        task_queue.put(url)

    # 放入哨兵值（每个工作线程一个）
    for _ in range(NUM_WORKERS):
        task_queue.put(None)

    # 等待所有任务完成
    task_queue.join()

    # 收集结果
    print()
    print(f"  所有任务完成！收集结果：")
    results = []
    while not result_queue.empty():
        results.append(result_queue.get())
    print(f"  成功处理 {len(results)} 个任务")
    for url, res in sorted(results)[:3]:
        print(f"    {res}")
    if len(results) > 3:
        print(f"    ... 共 {len(results)} 条")
    print()

    # PriorityQueue 示例
    print("【PriorityQueue：按优先级处理任务】")
    print()

    pq: queue.PriorityQueue[tuple[int, str]] = queue.PriorityQueue()

    # 元素是 (优先级, 数据) 的元组，优先级数值越小越优先
    pq.put((3, "低优先级任务：定期报告"))
    pq.put((1, "高优先级任务：支付异常告警"))
    pq.put((2, "中优先级任务：用户登录请求"))
    pq.put((1, "高优先级任务：系统错误通知"))

    print("  按优先级出队（数值小的先出）：")
    while not pq.empty():
        priority, task = pq.get()
        print(f"    优先级 {priority}: {task}")
    print()


# =============================================================================
# 22.5 ThreadPoolExecutor：线程池
# =============================================================================

def demo_thread_pool_executor() -> None:
    """演示 ThreadPoolExecutor（线程池）的使用。"""
    print("\n" + "=" * 60)
    print("22.5 ThreadPoolExecutor：线程池")
    print("=" * 60)

    print("【为什么要用线程池？】")
    print()
    print("  频繁创建/销毁线程的开销很大：")
    print("  - 每次创建线程需要分配栈内存、系统调用")
    print("  - 1000 个任务不应该创建 1000 个线程")
    print()
    print("  线程池的解决方案：")
    print("  - 预先创建固定数量的线程（如 8 个）")
    print("  - 任务提交到队列，工作线程从队列取任务执行")
    print("  - 任务完成后线程不销毁，继续等待新任务")
    print()
    print("  ThreadPoolExecutor 是 concurrent.futures 模块提供的高级接口")
    print("  比手动管理线程更简洁，还能方便地获取返回值")
    print()

    def fetch_data(url_id: int) -> dict[str, Any]:
        """
        模拟 HTTP 请求（I/O 密集型）。
        在实际项目中，这里会使用 requests 或 urllib 等。
        """
        time.sleep(0.15)  # 模拟网络延迟
        return {
            'id': url_id,
            'url': f'https://api.example.com/data/{url_id}',
            'status': 200,
            'data': f'用户 {url_id} 的数据',
        }

    # ── submit()：提交单个任务，获取 Future ───────────────
    print("【submit()：提交任务，返回 Future 对象】")
    print()
    print("  Future 代表一个'未来会完成的任务'：")
    print("  future.result()  → 阻塞等待结果（类似线程的 join）")
    print("  future.done()    → 检查是否完成（非阻塞）")
    print("  future.cancel()  → 取消尚未执行的任务")
    print()

    start = time.perf_counter()

    # with 语句确保执行完毕后关闭线程池并回收资源
    with ThreadPoolExecutor(max_workers=5, thread_name_prefix="Fetch") as executor:
        # 提交 5 个任务，立即返回 Future 对象（不阻塞）
        futures = [executor.submit(fetch_data, i) for i in range(1, 6)]

        # 逐一获取结果
        for future in futures:
            result = future.result()  # 阻塞直到该任务完成
            print(f"  获取到: {result['url']} → {result['data']}")

    elapsed = time.perf_counter() - start
    print(f"\n  5 个请求总耗时: {elapsed:.2f}秒（约为串行的 1/5）")
    print()

    # ── as_completed()：哪个先完成先处理哪个 ─────────────
    print("【as_completed()：按完成顺序处理结果】")
    print()
    print("  上面的方式按'提交顺序'等待，任务 1 完成前任务 2 不处理")
    print("  as_completed() 按'完成顺序'处理，更高效")
    print()

    def fetch_with_variable_delay(url_id: int) -> dict[str, Any]:
        """模拟不同延迟的请求（模拟真实网络不稳定性）。"""
        delay = (url_id % 3 + 1) * 0.1  # 不同任务有不同延迟
        time.sleep(delay)
        return {'id': url_id, 'delay': delay}

    print("  按完成顺序输出（不是提交顺序）：")
    with ThreadPoolExecutor(max_workers=5) as executor:
        futures = {
            executor.submit(fetch_with_variable_delay, i): i
            for i in range(5)
        }
        # as_completed 是一个生成器，每当有任务完成就 yield
        for future in as_completed(futures):
            task_id = futures[future]
            result = future.result()
            print(f"  任务 {task_id} 完成（延迟 {result['delay']:.1f}秒）")
    print()

    # ── map()：批量提交，保持顺序 ────────────────────────
    print("【executor.map()：批量提交并保持结果顺序】")
    print()
    print("  map() 类似内置 map()，但并发执行：")
    print("  - 提交所有任务并发执行")
    print("  - 按'提交顺序'返回结果（即使先完成的也要等前面的）")
    print("  - 比手动 submit + as_completed 更简洁")
    print()

    def process_number(n: int) -> int:
        """模拟处理数字（I/O 密集型）。"""
        time.sleep(0.05)
        return n * n  # 返回平方

    numbers = list(range(1, 9))
    start = time.perf_counter()

    with ThreadPoolExecutor(max_workers=4) as executor:
        # map 返回一个生成器，按顺序 yield 结果
        results = list(executor.map(process_number, numbers))

    elapsed = time.perf_counter() - start
    print(f"  输入: {numbers}")
    print(f"  结果（平方）: {results}")
    print(f"  耗时: {elapsed:.2f}秒")
    print()

    # ── 异常处理 ─────────────────────────────────────────
    print("【线程池中的异常处理】")
    print()
    print("  线程中的异常不会自动传播到主线程！")
    print("  调用 future.result() 时，线程内的异常才会被重新抛出")
    print()

    def risky_task(task_id: int) -> str:
        """可能抛出异常的任务。"""
        if task_id == 2:
            raise ValueError(f"任务 {task_id} 遇到无效数据！")
        time.sleep(0.05)
        return f"任务 {task_id} 成功"

    with ThreadPoolExecutor(max_workers=3) as executor:
        futures = [executor.submit(risky_task, i) for i in range(4)]
        for i, future in enumerate(futures):
            try:
                result = future.result()
                print(f"  任务 {i}: {result}")
            except ValueError as e:
                print(f"  任务 {i}: 捕获异常 → {e}")
    print()


# =============================================================================
# 22.6 multiprocessing 基础：进程创建与进程池
# =============================================================================

def demo_multiprocessing_basics() -> None:
    """演示 multiprocessing 模块的基本用法。"""
    print("\n" + "=" * 60)
    print("22.6 multiprocessing：绕过 GIL 实现真正的并行")
    print("=" * 60)

    print("【为什么需要 multiprocessing？】")
    print()
    print("  由于 GIL，Python 多线程无法并行执行 CPU 密集任务")
    print("  multiprocessing 创建多个独立进程，每个进程有自己的 GIL")
    print("  真正利用多核 CPU 并行计算")
    print()
    print("  代价：进程启动开销更大，进程间通信比线程复杂")
    print("  建议：每个进程的任务耗时足够长（>0.1秒）才值得并行")
    print()

    # ── CPU 密集型任务对比 ───────────────────────────────
    def cpu_intensive_task(n: int) -> int:
        """
        CPU 密集型任务：计算从 1 到 n 的质数数量。
        注意：这个函数必须定义在模块顶层（不能是嵌套函数）
        才能被 multiprocessing 的 spawn 方式序列化传给子进程。
        这里作为演示，实际运行依赖操作系统的 fork 机制。
        """
        count = 0
        for num in range(2, n + 1):
            is_prime = True
            for divisor in range(2, int(num ** 0.5) + 1):
                if num % divisor == 0:
                    is_prime = False
                    break
            if is_prime:
                count += 1
        return count

    # 演示对比：不实际运行多进程（避免在某些平台的 Jupyter/交互环境中出问题）
    print("【CPU 密集型任务：多线程 vs 多进程对比】")
    print()
    print("  任务：计算 1~50000 范围内的质数数量（4份）")
    print()

    N = 50000
    PARTS = 4

    # 串行执行
    start = time.perf_counter()
    serial_results = [cpu_intensive_task(N // PARTS) for _ in range(PARTS)]
    serial_time = time.perf_counter() - start
    print(f"  串行执行: {serial_time:.2f}秒，结果: {serial_results[0]} 个质数/份")

    # 多线程执行（受 GIL 限制，不会加速）
    start = time.perf_counter()
    with ThreadPoolExecutor(max_workers=PARTS) as executor:
        thread_results = list(executor.map(cpu_intensive_task, [N // PARTS] * PARTS))
    thread_time = time.perf_counter() - start
    print(f"  多线程执行: {thread_time:.2f}秒（GIL 限制，基本没有提升）")

    print()
    print("  [注意] 以下多进程示例需要在 __main__ 块中运行")
    print("  为避免 Windows 平台的多进程启动问题，此处展示代码结构")
    print()

    print("【Process 类：手动创建进程】")
    print()
    print("  # 与 Thread 类接口几乎完全相同：")
    print("  p = multiprocessing.Process(target=func, args=(...))")
    print("  p.start()   # 启动进程")
    print("  p.join()    # 等待进程完成")
    print("  p.pid       # 进程 ID")
    print("  p.is_alive() # 是否还在运行")
    print("  p.terminate() # 发送 SIGTERM 信号终止进程")
    print("  p.kill()    # 发送 SIGKILL 信号强制终止")
    print()
    print("  示例代码：")
    print("    def worker(n):")
    print("        print(f'子进程 PID: {os.getpid()}, 计算结果: {n*n}')")
    print()
    print("    p = multiprocessing.Process(target=worker, args=(10,))")
    print("    p.start()")
    print("    p.join()")
    print()

    print("【ProcessPoolExecutor：进程池（推荐）】")
    print()
    print("  用法与 ThreadPoolExecutor 完全一致，只需换一个类名！")
    print()
    print("  # 使用进程池（4 个进程）并行处理 CPU 密集任务：")
    print("  with ProcessPoolExecutor(max_workers=4) as executor:")
    print("      results = list(executor.map(cpu_intensive_task, ranges))")
    print()
    print("  重要限制（Windows/macOS）：")
    print("  - 进程池代码必须放在 if __name__ == '__main__': 块中")
    print("  - 传给子进程的函数必须在模块顶层定义（可被 pickle 序列化）")
    print("  - 不能使用 lambda 或嵌套函数作为进程任务")
    print()

    print("【进程间通信（IPC）方式】")
    print()

    ipc_methods = [
        ('multiprocessing.Queue', '类似 queue.Queue，进程安全的消息队列'),
        ('multiprocessing.Pipe', '点对点双向通信管道（两端各一个连接对象）'),
        ('multiprocessing.Value', '共享的单个值（支持 int/float 等 C 类型）'),
        ('multiprocessing.Array', '共享数组（连续内存，支持 C 类型）'),
        ('multiprocessing.Manager', '管理器：共享 list、dict 等复杂对象'),
    ]

    for name, desc in ipc_methods:
        print(f"  {name}")
        print(f"    → {desc}")
        print()

    print("【Queue 进程间通信示例代码】")
    print()
    print("  def producer(q):")
    print("      for i in range(5):")
    print("          q.put(i)")
    print("          print(f'生产: {i}')")
    print("      q.put(None)  # 哨兵值")
    print()
    print("  def consumer(q):")
    print("      while True:")
    print("          item = q.get()")
    print("          if item is None: break")
    print("          print(f'消费: {item}')")
    print()
    print("  if __name__ == '__main__':")
    print("      q = multiprocessing.Queue()")
    print("      p1 = multiprocessing.Process(target=producer, args=(q,))")
    print("      p2 = multiprocessing.Process(target=consumer, args=(q,))")
    print("      p1.start(); p2.start()")
    print("      p1.join(); p2.join()")
    print()

    print("【Value 和 Array：共享内存示例代码】")
    print()
    print("  # 共享整数")
    print("  shared_val = multiprocessing.Value('i', 0)  # 'i' = int")
    print("  with shared_val.get_lock():")
    print("      shared_val.value += 1  # 必须加锁！Value 不是自动线程/进程安全")
    print()
    print("  # 共享数组")
    print("  shared_arr = multiprocessing.Array('d', [1.0, 2.0, 3.0])  # 'd' = double")
    print("  with shared_arr.get_lock():")
    print("      shared_arr[0] = 100.0")
    print()

    print("【选择建议】")
    print()
    print("  任务耗时 < 0.01秒   → 不要并行（开销 > 收益）")
    print("  I/O 密集型任务      → threading 或 asyncio")
    print("  CPU 密集型任务      → multiprocessing")
    print("  数据量大、计算复杂  → ProcessPoolExecutor + executor.map()")
    print()


# =============================================================================
# 22.7 asyncio 入门：异步编程基础
# =============================================================================

def demo_asyncio_basics() -> None:
    """演示 asyncio 异步编程的核心概念和基本用法。"""
    print("\n" + "=" * 60)
    print("22.7 asyncio 入门：异步编程")
    print("=" * 60)

    print("【异步编程是什么？】")
    print()
    print("  同步（阻塞）：")
    print("    函数 A 调用 B，A 必须等待 B 完成才能继续")
    print("    就像打电话：你说话，对方回应，交替进行")
    print()
    print("  异步（非阻塞）：")
    print("    函数 A 发起 I/O 操作后，不等待结果，继续做其他事")
    print("    等 I/O 完成时，通过回调或 await 获取结果")
    print("    就像发邮件：发出去后继续干别的，有回复再处理")
    print()
    print("  asyncio 的核心思想：")
    print("  - 单个线程内，通过协程（coroutine）实现并发")
    print("  - 当一个协程在等待 I/O 时，切换去执行另一个协程")
    print("  - 没有线程切换开销，可以轻松管理数万个并发任务")
    print()

    print("【核心概念】")
    print()
    print("  协程（Coroutine）：")
    print("    用 async def 定义的特殊函数")
    print("    调用协程函数不会立即执行，而是返回一个协程对象")
    print("    必须通过 await 或事件循环来运行")
    print()
    print("  async def my_coroutine():")
    print("      await asyncio.sleep(1)  # 异步等待 1 秒（不阻塞其他协程）")
    print("      return 42")
    print()
    print("  事件循环（Event Loop）：")
    print("    调度和运行协程的核心机制")
    print("    asyncio.run(main()) 创建事件循环并运行入口协程")
    print()
    print("  await 关键字：")
    print("    只能在 async def 函数内使用")
    print("    暂停当前协程，让事件循环切换去执行其他协程")
    print("    await 后面接'可等待对象'：协程、Task、Future")
    print()

    # ── 基本异步函数 ─────────────────────────────────────
    print("【基本异步函数示例】")
    print()

    async def greet(name: str, delay: float) -> str:
        """
        异步问候函数。
        await asyncio.sleep() 模拟异步 I/O 等待（如网络请求）。
        """
        print(f"  开始处理: {name}（预计 {delay:.1f}秒后完成）")
        await asyncio.sleep(delay)   # 异步睡眠：等待时释放事件循环
        message = f"你好，{name}！"
        print(f"  完成处理: {name}")
        return message

    async def main_basic() -> None:
        """基本异步程序入口。"""
        print("  【顺序执行（await 一个接一个）】")
        start = time.perf_counter()
        # await 会等待每个协程完成，效果类似同步
        r1 = await greet("Alice", 0.3)
        r2 = await greet("Bob", 0.3)
        r3 = await greet("Charlie", 0.3)
        elapsed = time.perf_counter() - start
        print(f"  结果: {r1}, {r2}, {r3}")
        print(f"  顺序执行耗时: {elapsed:.2f}秒\n")

        print("  【并发执行（gather 同时运行多个协程）】")
        start = time.perf_counter()
        # asyncio.gather() 同时运行多个协程，等待全部完成
        results = await asyncio.gather(
            greet("Alice", 0.3),
            greet("Bob", 0.3),
            greet("Charlie", 0.3),
        )
        elapsed = time.perf_counter() - start
        print(f"  结果: {results}")
        print(f"  并发执行耗时: {elapsed:.2f}秒（约 1/3）")

    # asyncio.run() 是 Python 3.7+ 推荐的入口方式
    asyncio.run(main_basic())
    print()

    # ── Task：显式创建和管理协程任务 ────────────────────
    print("【asyncio.create_task()：显式创建任务】")
    print()
    print("  Task 和 Future 是协程的'包装器'：")
    print("  - create_task() 立即将协程调度到事件循环中运行")
    print("  - 不需要 await 就开始排队执行（与 gather 不同）")
    print("  - 适合需要在协程执行期间做其他事情的场景")
    print()

    async def fetch_resource(resource_id: int, delay: float) -> dict[str, Any]:
        """模拟异步获取资源。"""
        await asyncio.sleep(delay)
        return {'id': resource_id, 'data': f'资源 {resource_id} 的内容'}

    async def main_tasks() -> None:
        """演示 Task 的创建和管理。"""
        print("  创建任务后立即执行其他工作：")

        # create_task 立即将协程调度到事件循环，开始排队
        task1 = asyncio.create_task(
            fetch_resource(1, 0.2),
            name="fetch-1"
        )
        task2 = asyncio.create_task(
            fetch_resource(2, 0.3),
            name="fetch-2"
        )
        task3 = asyncio.create_task(
            fetch_resource(3, 0.1),
            name="fetch-3"
        )

        # 任务已经开始执行，这里可以做其他工作
        print("  任务已提交，现在可以做其他事情...")
        await asyncio.sleep(0.05)
        print("  做了一些其他工作...")

        # 等待所有任务完成
        results = await asyncio.gather(task1, task2, task3)
        print(f"  所有任务完成: {results}")

    asyncio.run(main_tasks())
    print()

    # ── 实战示例：异步爬虫 ───────────────────────────────
    print("【实战示例：模拟异步爬虫】")
    print()

    async def fetch_url(url: str, session_id: int) -> dict[str, Any]:
        """
        模拟异步 HTTP 请求。
        实际项目中使用 aiohttp 或 httpx 等异步 HTTP 库。
        """
        print(f"  [Session-{session_id}] 开始请求: {url}")
        # 模拟网络延迟（不同 URL 延迟不同）
        delay = 0.1 + (hash(url) % 10) * 0.05
        await asyncio.sleep(delay)
        print(f"  [Session-{session_id}] 完成请求: {url} ({delay:.2f}秒)")
        return {
            'url': url,
            'status': 200,
            'content_length': len(url) * 100,
        }

    async def main_crawler() -> None:
        """异步爬虫主函数。"""
        urls = [
            f"https://example.com/page{i}.html"
            for i in range(1, 11)
        ]

        print(f"  开始爬取 {len(urls)} 个页面...")
        start = time.perf_counter()

        # 并发爬取所有 URL
        tasks = [
            fetch_url(url, i)
            for i, url in enumerate(urls, 1)
        ]
        results = await asyncio.gather(*tasks)

        elapsed = time.perf_counter() - start
        print(f"\n  爬取完成！")
        print(f"  总耗时: {elapsed:.2f}秒")
        print(f"  平均每个页面: {elapsed / len(urls):.2f}秒")
        print(f"  成功: {len(results)} 个")

    asyncio.run(main_crawler())
    print()

    print("【asyncio 常用函数】")
    print()
    print("  asyncio.run(coro)           → 运行协程（Python 3.7+，推荐）")
    print("  asyncio.create_task(coro)   → 创建 Task 并调度执行")
    print("  asyncio.gather(*coros)      → 并发运行多个协程，等待全部完成")
    print("  asyncio.wait(tasks)         → 等待任务集合（更底层，灵活）")
    print("  asyncio.sleep(seconds)      → 异步睡眠（不阻塞事件循环）")
    print("  asyncio.wait_for(coro, timeout) → 带超时的等待")
    print("  asyncio.as_completed(tasks) → 按完成顺序迭代任务")
    print()

    print("【async/await 使用规则】")
    print()
    print("  1. async def 定义协程函数")
    print("  2. await 只能在 async def 内部使用")
    print("  3. await 后面必须是可等待对象（协程、Task、Future）")
    print("  4. 普通函数不能 await，协程函数必须被 await 或调度执行")
    print("  5. asyncio.run() 是程序入口，只能在最外层调用一次")
    print()


# =============================================================================
# 22.8 并发模型选择指南
# =============================================================================

def demo_concurrency_comparison() -> None:
    """对比三种并发模型的性能和适用场景。"""
    print("\n" + "=" * 60)
    print("22.8 并发模型选择指南")
    print("=" * 60)

    print("【I/O 密集型任务对比】")
    print()

    def io_task_sync(task_id: int) -> str:
        """同步 I/O 任务（模拟）。"""
        time.sleep(0.1)
        return f"任务 {task_id} 完成"

    async def io_task_async(task_id: int) -> str:
        """异步 I/O 任务（模拟）。"""
        await asyncio.sleep(0.1)
        return f"任务 {task_id} 完成"

    NUM_TASKS = 10

    # 串行执行
    print(f"  1. 串行执行 {NUM_TASKS} 个 I/O 任务：")
    start = time.perf_counter()
    results = [io_task_sync(i) for i in range(NUM_TASKS)]
    serial_time = time.perf_counter() - start
    print(f"     耗时: {serial_time:.2f}秒")
    print()

    # 多线程
    print(f"  2. 多线程执行 {NUM_TASKS} 个 I/O 任务：")
    start = time.perf_counter()
    with ThreadPoolExecutor(max_workers=5) as executor:
        results = list(executor.map(io_task_sync, range(NUM_TASKS)))
    thread_time = time.perf_counter() - start
    print(f"     耗时: {thread_time:.2f}秒")
    print(f"     提速: {serial_time / thread_time:.1f}x")
    print()

    # 异步
    print(f"  3. asyncio 执行 {NUM_TASKS} 个 I/O 任务：")
    async def run_async_tasks():
        start = time.perf_counter()
        results = await asyncio.gather(*[io_task_async(i) for i in range(NUM_TASKS)])
        return time.perf_counter() - start

    async_time = asyncio.run(run_async_tasks())
    print(f"     耗时: {async_time:.2f}秒")
    print(f"     提速: {serial_time / async_time:.1f}x")
    print()

    print("【决策树：如何选择并发模型】")
    print()
    print("  ┌─ 任务类型？")
    print("  │")
    print("  ├─ CPU 密集型（计算、加密、压缩）")
    print("  │  └→ multiprocessing.ProcessPoolExecutor")
    print("  │     理由：绕过 GIL，真正并行利用多核")
    print("  │")
    print("  ├─ I/O 密集型（网络、文件、数据库）")
    print("  │  │")
    print("  │  ├─ 并发数量 < 100")
    print("  │  │  └→ threading.ThreadPoolExecutor")
    print("  │  │     理由：简单直观，易于调试")
    print("  │  │")
    print("  │  └─ 并发数量 > 100（高并发）")
    print("  │     └→ asyncio")
    print("  │        理由：轻量级，可管理数万并发")
    print("  │")
    print("  └─ 混合型（既有 I/O 又有 CPU）")
    print("     └→ asyncio + ProcessPoolExecutor")
    print("        理由：asyncio 处理 I/O，ProcessPoolExecutor 处理 CPU")
    print()

    print("【实际项目建议】")
    print()

    scenarios = [
        ("Web 爬虫（爬取 1000+ 页面）", "asyncio + aiohttp"),
        ("批量图像处理（CPU 密集）", "multiprocessing.Pool"),
        ("批量文件下载（10-50 个）", "ThreadPoolExecutor"),
        ("高并发 Web 服务器", "asyncio + uvicorn/aiohttp"),
        ("数据分析（大量计算）", "multiprocessing + pandas"),
        ("批量数据库查询（100+ 查询）", "asyncio + asyncpg/aiomysql"),
        ("后台任务队列", "threading.Thread + queue.Queue"),
        ("实时数据处理", "asyncio + websockets"),
    ]

    print(f"  {'场景':<30} 推荐方案")
    print(f"  {'-'*30:<30} {'-'*30}")
    for scenario, solution in scenarios:
        print(f"  {scenario:<30} {solution}")
    print()


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行所有演示函数。"""
    demo_concurrency_concepts()
    demo_thread_basics()
    demo_thread_synchronization()
    demo_thread_queue()
    demo_thread_pool_executor()
    demo_multiprocessing_basics()
    demo_asyncio_basics()
    demo_concurrency_comparison()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# ── threading 模块核心 API ───────────────────────────────
#
#   创建线程：
#     t = threading.Thread(target=func, args=(...), name='MyThread')
#     t.start()                    # 启动线程
#     t.join(timeout=None)         # 等待线程完成
#     t.is_alive()                 # 是否还在运行
#     t.daemon = True              # 设为守护线程（start 前设置）
#
#   同步原语：
#     lock = threading.Lock()      # 互斥锁
#     rlock = threading.RLock()    # 可重入锁
#     sem = threading.Semaphore(n) # 信号量（限制并发数）
#     event = threading.Event()    # 事件（线程间信号）
#     cond = threading.Condition() # 条件变量
#
#   Lock 使用：
#     with lock:                   # 推荐：自动 acquire/release
#         # 临界区代码
#     lock.acquire(blocking=True, timeout=-1)
#     lock.release()
#
#   Event 使用：
#     event.set()                  # 设置事件（唤醒等待线程）
#     event.clear()                # 清除事件
#     event.wait(timeout=None)     # 等待事件（阻塞）
#     event.is_set()               # 检查状态（非阻塞）
#
#   Condition 使用：
#     cond = threading.Condition(lock=None)
#     with cond:
#         cond.wait()              # 释放锁并等待
#         cond.notify()           # 唤醒一个等待线程
#         cond.notify_all()       # 唤醒所有等待线程
#
#   获取线程信息：
#     threading.current_thread()   # 当前线程对象
#     threading.main_thread()      # 主线程对象
#     threading.active_count()     # 活跃线程数
#     threading.enumerate()        # 所有活跃线程列表
#
# ── queue 模块核心 API ───────────────────────────────────
#
#   queue.Queue(maxsize=0)         # FIFO 队列（0 表示无限）
#   queue.LifoQueue(maxsize=0)     # LIFO 队列（栈）
#   queue.PriorityQueue(maxsize=0) # 优先级队列
#
#   q.put(item, block=True, timeout=None)   # 放入数据
#   q.get(block=True, timeout=None)         # 取出数据
#   q.put_nowait(item)             # 非阻塞放入（满则抛 Full）
#   q.get_nowait()                 # 非阻塞取出（空则抛 Empty）
#   q.task_done()                  # 标记任务完成
#   q.join()                       # 等待所有任务完成
#   q.qsize()                      # 当前元素数量
#   q.empty()                      # 是否为空
#   q.full()                       # 是否已满
#
# ── concurrent.futures 模块核心 API ─────────────────────
#
#   ThreadPoolExecutor(max_workers=None, thread_name_prefix='')
#   ProcessPoolExecutor(max_workers=None)
#
#   with executor:
#       future = executor.submit(fn, *args, **kwargs)  # 提交单任务
#       results = executor.map(fn, iterable)           # 批量提交
#       # with 退出时自动 shutdown(wait=True)
#
#   Future 对象：
#     future.result(timeout=None)  # 获取结果（阻塞）
#     future.done()                # 是否完成
#     future.cancel()              # 取消（未执行时有效）
#     future.exception()           # 获取异常（不重新抛出）
#
#   as_completed(futures, timeout=None)
#     → 生成器，按完成顺序 yield Future
#
# ── multiprocessing 模块核心 API ─────────────────────────
#
#   Process(target, args=(), kwargs={}, name=None, daemon=None)
#     p.start()                    # 启动进程
#     p.join(timeout=None)         # 等待进程完成
#     p.terminate()                # 发送 SIGTERM
#     p.kill()                     # 发送 SIGKILL
#     p.pid                        # 进程 ID
#     p.exitcode                   # 退出码（None 表示还在运行）
#
#   进程间通信：
#     multiprocessing.Queue()      # 进程安全队列
#     multiprocessing.Pipe()       # 返回 (conn1, conn2) 管道两端
#     multiprocessing.Value('i', 0) # 共享整数
#     multiprocessing.Array('d', n) # 共享浮点数组
#     multiprocessing.Manager()    # 管理器（共享复杂对象）
#
#   类型码（C 类型）：
#     'b' → signed char    'B' → unsigned char
#     'h' → short          'H' → unsigned short
#     'i' → int            'I' → unsigned int
#     'l' → long           'L' → unsigned long
#     'f' → float          'd' → double
#
# ── asyncio 模块核心 API ─────────────────────────────────
#
#   程序入口：
#     asyncio.run(coro)            # 运行协程（Python 3.7+）
#
#   协程定义：
#     async def func():            # 协程函数
#         await some_coroutine()   # 等待可等待对象
#         return result
#
#   任务管理：
#     task = asyncio.create_task(coro, name=None)
#     task.cancel()                # 请求取消任务
#     task.done()                  # 是否完成
#     task.result()                # 获取结果
#
#   并发工具：
#     await asyncio.gather(*coros, return_exceptions=False)
#       → 并发运行，全部完成后返回结果列表
#     await asyncio.wait(tasks, timeout=None, return_when=ALL_COMPLETED)
#       → 返回 (done, pending) 两个集合
#     await asyncio.wait_for(coro, timeout)
#       → 超时后抛 asyncio.TimeoutError
#     asyncio.as_completed(coros)
#       → 按完成顺序迭代的生成器
#
#   时间与调度：
#     await asyncio.sleep(seconds) # 异步睡眠
#     asyncio.get_event_loop()     # 获取当前事件循环
#     asyncio.get_running_loop()   # 获取正在运行的事件循环
#
#   asyncio + 线程/进程（混合使用）：
#     loop.run_in_executor(executor, func, *args)
#       → 在线程池/进程池中运行同步函数，返回 awaitable
#
# =============================================================================
# 【常见错误与解决方案】
# =============================================================================
#
# 1. 竞态条件（忘记加锁）
#    问题：多线程修改共享变量，结果不正确
#    原因：+= 等操作在字节码层面不是原子操作
#    解决：使用 threading.Lock() 保护临界区
#
# 2. 死锁（Deadlock）
#    问题：两个线程互相等待对方释放锁，程序卡死
#    原因：A 持有锁1等待锁2，B 持有锁2等待锁1
#    解决：
#      - 固定加锁顺序（所有线程按同一顺序获取多把锁）
#      - 使用 lock.acquire(timeout=X) 设置超时
#      - 使用 threading.RLock() 避免同一线程自我死锁
#
# 3. 线程泄漏（忘记 join）
#    问题：主线程结束但子线程还在运行（非守护线程）
#    解决：始终对非守护线程调用 join()
#          或将后台线程设为 daemon=True
#
# 4. Windows 多进程报错
#    问题：Windows 下使用 multiprocessing 出现无限递归或 PicklingError
#    原因：Windows 使用 spawn 方式创建进程，会重新导入模块
#    解决：所有多进程代码必须放在 if __name__ == '__main__': 块中
#          子进程函数必须在模块顶层定义（不能是 lambda 或嵌套函数）
#
# 5. 线程池任务中的异常被吞掉
#    问题：线程任务抛出异常，但程序没有任何报错
#    原因：线程中的异常被 Future 捕获，不会自动打印
#    解决：调用 future.result() 时用 try/except 捕获
#          或使用 future.exception() 检查是否有异常
#
# 6. asyncio 中调用阻塞函数
#    问题：在协程中调用 time.sleep() 等阻塞函数，导致事件循环卡住
#    原因：阻塞调用会占用整个事件循环，其他协程无法执行
#    解决：
#      - 将 time.sleep() 替换为 await asyncio.sleep()
#      - 将阻塞 I/O 替换为异步库（aiohttp、aiofiles 等）
#      - 使用 loop.run_in_executor() 在线程池中运行阻塞函数
#
# 7. "RuntimeError: This event loop is already running"
#    问题：在已有事件循环中再次调用 asyncio.run()
#    原因：asyncio.run() 不能在已运行的事件循环内调用
#    解决：
#      - 使用 await 代替 asyncio.run()（在协程内）
#      - 在 Jupyter Notebook 中使用 nest_asyncio 库
#
# 8. 进程间无法传递不可 pickle 的对象
#    问题：multiprocessing 传递 lambda、本地函数时报 PicklingError
#    原因：进程间通信使用 pickle 序列化，有些对象不支持
#    解决：
#      - 只传递简单数据（数字、字符串、列表、字典）
#      - 子进程函数定义在模块顶层
#      - 使用 multiprocessing.Manager() 共享复杂对象
#
# 9. Queue.join() 永久阻塞
#    问题：调用 q.join() 后程序卡住不退出
#    原因：某个工作线程处理任务后忘记调用 q.task_done()
#    解决：确保每次 q.get() 之后都有对应的 q.task_done()
#          推荐使用 try/finally 确保 task_done() 被调用
#
# 10. 信号量不平衡（Semaphore 泄漏）
#     问题：release() 比 acquire() 多，导致并发数超出限制
#     解决：始终使用 with semaphore: 语句，避免手动 acquire/release
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   编写函数 run_five_threads()，创建并启动 5 个线程：
#   - 每个线程打印自己的编号（1~5）和当前时间（格式 %H:%M:%S）
#   - 确保所有线程都完成后，主线程再打印"全部完成"
#   返回格式：None（控制台输出）
#   例如：
#     run_five_threads()
#     # [线程 1] 当前时间: 10:23:45
#     # [线程 2] 当前时间: 10:23:45
#     # [线程 3] 当前时间: 10:23:45
#     # [线程 4] 当前时间: 10:23:45
#     # [线程 5] 当前时间: 10:23:45
#     # 全部完成
#
# 练习 2（基础）：
#   实现线程安全的 ThreadSafeCounter 类：
#   - increment()：将计数器加 1
#   - decrement()：将计数器减 1
#   - value 属性：返回当前计数值（受 Lock 保护）
#   使用 threading.Lock 保证线程安全
#   返回格式：ThreadSafeCounter 类
#   例如：
#     counter = ThreadSafeCounter()
#     threads = [threading.Thread(target=lambda: [counter.increment() for _ in range(1000)])
#                for _ in range(10)]
#     for t in threads: t.start()
#     for t in threads: t.join()
#     print(counter.value)
#     # 10000（始终正确，无竞态条件）
#
# 练习 3（进阶）：
#   编写函数 producer_consumer_demo()，使用 queue.Queue 实现生产者-消费者模式：
#   - 2 个生产者线程，每个生成 10 个随机整数（1~100）并放入队列
#   - 3 个消费者线程，从队列取出数字并计算平方
#   - 主线程等待所有任务完成后，返回所有平方值的总和
#   返回格式：int（所有平方值之和）
#   例如：
#     total = producer_consumer_demo()
#     print(f"所有平方值之和: {total}")
#     # 所有平方值之和: 144236（结果因随机数而异）
#
# 练习 4（进阶）：
#   编写函数 batch_download(file_count, max_workers)，使用 ThreadPoolExecutor 模拟批量下载：
#   - file_count 个"文件"，每个下载耗时 0.1~0.5 秒（随机）
#   - 最多 max_workers 个线程同时下载
#   - 返回包含每个文件信息的字典列表，按耗时降序排列
#   返回格式：list[dict]，每个 dict 含 'filename'（str）和 'elapsed'（float）
#   例如：
#     results = batch_download(file_count=5, max_workers=3)
#     for r in results:
#         print(f"{r['filename']}: {r['elapsed']:.2f}秒")
#     # file_03.zip: 0.48秒
#     # file_01.zip: 0.35秒
#     # ...
#
# 练习 5（进阶）：
#   编写函数 run_task_with_timeout(task_duration, timeout)，使用 threading.Event
#   实现带超时的任务控制：
#   - 启动一个模拟耗时 task_duration 秒的后台线程
#   - 后台线程每隔 0.1 秒检查停止事件，若被设置则提前退出
#   - 主线程最多等待 timeout 秒，超时后设置停止事件通知后台线程退出
#   - 返回任务是否在超时前正常完成（True 表示正常完成，False 表示超时）
#   返回格式：bool
#   例如：
#     result = run_task_with_timeout(task_duration=0.5, timeout=1.0)
#     print(result)
#     # True（0.5秒完成，未超时）
#     result = run_task_with_timeout(task_duration=3.0, timeout=1.0)
#     print(result)
#     # False（3秒未完成，1秒超时）
#
# 练习 6（基础）：
#   编写异步函数 compare_concurrent_vs_sequential()，对比并发与顺序执行：
#   - 定义 3 个协程，分别模拟耗时 1.0、0.5、0.8 秒的 I/O 操作
#   - 用 asyncio.gather() 并发运行并记录总耗时
#   - 再用顺序 await 逐一运行并记录总耗时
#   - 返回 (并发耗时, 顺序耗时) 的元组
#   返回格式：tuple[float, float]
#   例如：
#     concurrent_t, sequential_t = asyncio.run(compare_concurrent_vs_sequential())
#     print(f"并发: {concurrent_t:.2f}秒，顺序: {sequential_t:.2f}秒")
#     # 并发: 1.00秒，顺序: 2.30秒
#
# 练习 7（基础）：
#   编写异步函数 fetch_with_timeout(url, timeout)，实现带超时的异步请求：
#   - 模拟一个耗时 2 秒的网络请求
#   - 使用 asyncio.wait_for() 控制超时（timeout 秒）
#   - 超时时捕获 asyncio.TimeoutError，返回 None 并打印友好的错误信息
#   - 正常完成时返回包含 url 和状态码的字典
#   返回格式：dict（{'url': str, 'status': int}）或 None（超时）
#   例如：
#     result = asyncio.run(fetch_with_timeout('https://example.com', timeout=3.0))
#     print(result)
#     # {'url': 'https://example.com', 'status': 200}
#     result = asyncio.run(fetch_with_timeout('https://example.com', timeout=1.0))
#     print(result)
#     # 请求超时（1.0秒），URL: https://example.com
#     # None
#
# 练习 8（进阶）：
#   实现 ConcurrentBatchProcessor 类，并发批量处理任务：
#   - add_task(func, *args, **kwargs)：添加一个待执行的任务
#   - run(max_workers=4, timeout=None)：使用 ThreadPoolExecutor 并发执行所有任务，
#     返回结果列表，每项为 {'status': 'ok'/'error', 'result': 返回值/异常信息字符串}
#   返回格式：list[dict]
#   例如：
#     processor = ConcurrentBatchProcessor()
#     processor.add_task(lambda: 1 + 1)
#     processor.add_task(lambda: 10 / 0)        # 会抛出异常
#     processor.add_task(lambda: "hello world")
#     results = processor.run(max_workers=2)
#     for r in results:
#         print(r)
#     # {'status': 'ok',    'result': 2}
#     # {'status': 'error', 'result': 'division by zero'}
#     # {'status': 'ok',    'result': 'hello world'}
#
# 练习答案提示：
#   练习1：用 threading.Thread 创建线程，start() 启动，join() 等待；
#           time.strftime('%H:%M:%S') 获取当前时间字符串
#   练习2：在 __init__ 中创建 self._lock = threading.Lock()；
#           所有修改和读取 _value 的地方都用 with self._lock: 包裹
#   练习3：生产者全部结束后向队列放哨兵值（每个消费者一个 None），消费者检测到 None 则退出；
#           用另一个 Queue 收集计算结果，最后汇总求和
#   练习4：simulate_download(filename) 用 random.uniform(0.1, 0.5) 产生随机延迟；
#           用 as_completed 收集 Future 结果，最后按 elapsed 降序排列
#   练习5：用 stop_event.is_set() 让后台线程可中断地检查信号；
#           主线程用 completed_event.wait(timeout) 判断是否超时
#   练习6：用 asyncio.sleep() 模拟 I/O 等待；记录 time.perf_counter() 前后的差值
#   练习7：在 asyncio.wait_for() 外层用 try/except asyncio.TimeoutError 捕获
#   练习8：在 run() 中用 executor.submit() 提交任务，try/except 捕获每个 future.result()


# =============================================================================
# 【练习答案】
# =============================================================================


import random


# ── 练习 1：run_five_threads ──────────────────────────────────────────────────

def run_five_threads() -> None:
    """
    练习 1：创建并启动 5 个线程，每个线程打印自己的编号和当前时间。

    Returns:
        None（控制台输出）

    Example:
        run_five_threads()
        # [线程 1] 当前时间: 10:23:45
        # ...
        # 全部完成
    """
    def thread_task(thread_id: int) -> None:
        current_time = time.strftime('%H:%M:%S')
        print(f"  [线程 {thread_id}] 当前时间: {current_time}")

    threads = [
        threading.Thread(target=thread_task, args=(i,), name=f"Thread-{i}")
        for i in range(1, 6)
    ]
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    print("  全部完成")


# ── 练习 2：ThreadSafeCounter ─────────────────────────────────────────────────

class ThreadSafeCounter:
    """
    练习 2：线程安全的计数器。

    使用 threading.Lock 保护内部状态，确保多线程并发访问时不发生竞态条件。

    Example:
        counter = ThreadSafeCounter()
        threads = [threading.Thread(target=lambda: [counter.increment() for _ in range(1000)])
                   for _ in range(10)]
        for t in threads: t.start()
        for t in threads: t.join()
        print(counter.value)   # 10000
    """

    def __init__(self) -> None:
        self._value: int = 0
        self._lock: threading.Lock = threading.Lock()

    def increment(self) -> None:
        """将计数器加 1（线程安全）。"""
        with self._lock:
            self._value += 1

    def decrement(self) -> None:
        """将计数器减 1（线程安全）。"""
        with self._lock:
            self._value -= 1

    @property
    def value(self) -> int:
        """返回当前计数值（线程安全）。"""
        with self._lock:
            return self._value


# ── 练习 3：producer_consumer_demo ───────────────────────────────────────────

def producer_consumer_demo() -> int:
    """
    练习 3：使用 queue.Queue 实现生产者-消费者模式。

    2 个生产者各生成 10 个随机整数（1~100），3 个消费者计算平方。

    Returns:
        所有平方值之和（int）

    Example:
        total = producer_consumer_demo()
        print(f"所有平方值之和: {total}")
    """
    PRODUCER_COUNT = 2
    CONSUMER_COUNT = 3
    ITEMS_PER_PRODUCER = 10

    task_queue: queue.Queue[Optional[int]] = queue.Queue()
    result_queue: queue.Queue[int] = queue.Queue()

    def producer(producer_id: int) -> None:
        for _ in range(ITEMS_PER_PRODUCER):
            num = random.randint(1, 100)
            task_queue.put(num)

    def consumer(consumer_id: int) -> None:
        while True:
            item = task_queue.get()
            if item is None:        # 哨兵值，退出
                task_queue.task_done()
                break
            result_queue.put(item * item)
            task_queue.task_done()

    # 启动生产者和消费者
    producers = [
        threading.Thread(target=producer, args=(i,), name=f"Producer-{i}")
        for i in range(PRODUCER_COUNT)
    ]
    consumers = [
        threading.Thread(target=consumer, args=(i,), name=f"Consumer-{i}")
        for i in range(CONSUMER_COUNT)
    ]
    for t in producers + consumers:
        t.start()

    # 等待所有生产者完成后，再放入哨兵值（每个消费者一个）
    for t in producers:
        t.join()
    for _ in range(CONSUMER_COUNT):
        task_queue.put(None)

    task_queue.join()
    for t in consumers:
        t.join()

    # 汇总结果
    total = 0
    while not result_queue.empty():
        total += result_queue.get()
    return total


# ── 练习 4：batch_download ────────────────────────────────────────────────────

def batch_download(file_count: int, max_workers: int) -> list[dict]:
    """
    练习 4：使用 ThreadPoolExecutor 模拟批量下载。

    Args:
        file_count:  文件数量
        max_workers: 最大并发线程数

    Returns:
        按耗时降序排列的下载信息列表，每项含 'filename' 和 'elapsed'

    Example:
        results = batch_download(file_count=5, max_workers=3)
        for r in results:
            print(f"{r['filename']}: {r['elapsed']:.2f}秒")
    """
    def simulate_download(filename: str) -> dict:
        """模拟单个文件下载。"""
        delay = random.uniform(0.1, 0.5)
        start = time.perf_counter()
        time.sleep(delay)
        elapsed = time.perf_counter() - start
        return {'filename': filename, 'elapsed': round(elapsed, 3)}

    filenames = [f"file_{i:02d}.zip" for i in range(file_count)]

    with ThreadPoolExecutor(max_workers=max_workers) as executor:
        futures = {executor.submit(simulate_download, f): f for f in filenames}
        results = []
        for future in as_completed(futures):
            results.append(future.result())

    # 按耗时降序排列
    results.sort(key=lambda x: x['elapsed'], reverse=True)
    return results


# ── 练习 5：run_task_with_timeout ─────────────────────────────────────────────

def run_task_with_timeout(task_duration: float, timeout: float) -> bool:
    """
    练习 5：使用 threading.Event 实现带超时的任务控制。

    Args:
        task_duration: 后台任务耗时（秒）
        timeout:       主线程等待超时（秒）

    Returns:
        True 表示任务在超时前正常完成，False 表示超时

    Example:
        result = run_task_with_timeout(task_duration=0.5, timeout=1.0)
        print(result)  # True
        result = run_task_with_timeout(task_duration=3.0, timeout=1.0)
        print(result)  # False
    """
    stop_event = threading.Event()
    completed_event = threading.Event()

    def slow_task() -> None:
        """后台任务：每 0.1 秒检查一次停止信号。"""
        elapsed = 0.0
        while elapsed < task_duration:
            if stop_event.is_set():
                return              # 收到停止信号，提前退出
            time.sleep(0.1)
            elapsed += 0.1
        completed_event.set()       # 正常完成，设置完成事件

    t = threading.Thread(target=slow_task, name="SlowTask")
    t.start()

    finished = completed_event.wait(timeout=timeout)
    if not finished:
        stop_event.set()            # 超时，通知后台线程退出

    t.join()
    return finished


# ── 练习 6：compare_concurrent_vs_sequential ─────────────────────────────────

async def compare_concurrent_vs_sequential() -> tuple[float, float]:
    """
    练习 6：对比 asyncio 并发与顺序执行的耗时。

    Returns:
        (并发耗时, 顺序耗时) 的元组

    Example:
        concurrent_t, sequential_t = asyncio.run(compare_concurrent_vs_sequential())
        print(f"并发: {concurrent_t:.2f}秒，顺序: {sequential_t:.2f}秒")
        # 并发: 1.00秒，顺序: 2.30秒
    """
    async def io_task(name: str, delay: float) -> str:
        await asyncio.sleep(delay)
        return f"{name} 完成（{delay}秒）"

    # 并发执行
    start = time.perf_counter()
    await asyncio.gather(
        io_task("任务A", 1.0),
        io_task("任务B", 0.5),
        io_task("任务C", 0.8),
    )
    concurrent_time = time.perf_counter() - start

    # 顺序执行
    start = time.perf_counter()
    await io_task("任务A", 1.0)
    await io_task("任务B", 0.5)
    await io_task("任务C", 0.8)
    sequential_time = time.perf_counter() - start

    return (concurrent_time, sequential_time)


# ── 练习 7：fetch_with_timeout ────────────────────────────────────────────────

async def fetch_with_timeout(url: str, timeout: float) -> Optional[dict]:
    """
    练习 7：带超时的异步 HTTP 请求（模拟）。

    Args:
        url:     请求的 URL
        timeout: 超时时间（秒）

    Returns:
        正常完成时返回 {'url': str, 'status': int}，超时返回 None

    Example:
        result = asyncio.run(fetch_with_timeout('https://example.com', timeout=3.0))
        print(result)   # {'url': 'https://example.com', 'status': 200}
        result = asyncio.run(fetch_with_timeout('https://example.com', timeout=1.0))
        print(result)   # None（并打印超时信息）
    """
    async def _do_request() -> dict:
        """模拟耗时 2 秒的网络请求。"""
        await asyncio.sleep(2.0)
        return {'url': url, 'status': 200}

    try:
        result = await asyncio.wait_for(_do_request(), timeout=timeout)
        return result
    except asyncio.TimeoutError:
        print(f"  请求超时（{timeout}秒），URL: {url}")
        return None


# ── 练习 8：ConcurrentBatchProcessor ─────────────────────────────────────────

class ConcurrentBatchProcessor:
    """
    练习 8：并发批量任务处理器。

    使用 ThreadPoolExecutor 并发执行任务列表，自动捕获每个任务的异常。

    Example:
        processor = ConcurrentBatchProcessor()
        processor.add_task(lambda: 1 + 1)
        processor.add_task(lambda: 10 / 0)
        processor.add_task(lambda: "hello world")
        results = processor.run(max_workers=2)
        for r in results:
            print(r)
        # {'status': 'ok',    'result': 2}
        # {'status': 'error', 'result': 'division by zero'}
        # {'status': 'ok',    'result': 'hello world'}
    """

    def __init__(self) -> None:
        self._tasks: list[tuple] = []   # (func, args, kwargs)

    def add_task(self, func, *args, **kwargs) -> None:
        """
        添加一个待执行的任务。

        Args:
            func:     任务函数
            *args:    位置参数
            **kwargs: 关键字参数
        """
        self._tasks.append((func, args, kwargs))

    def run(
        self,
        max_workers: int = 4,
        timeout: Optional[float] = None,
    ) -> list[dict]:
        """
        并发执行所有已添加的任务。

        Args:
            max_workers: 最大并发线程数（默认 4）
            timeout:     等待所有任务完成的超时时间（秒，None 表示不限）

        Returns:
            结果列表，顺序与 add_task() 顺序一致，每项为：
            {'status': 'ok', 'result': 返回值} 或
            {'status': 'error', 'result': 异常信息字符串}
        """
        results: list[Optional[dict]] = [None] * len(self._tasks)

        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            future_to_index = {
                executor.submit(func, *args, **kwargs): idx
                for idx, (func, args, kwargs) in enumerate(self._tasks)
            }
            for future in as_completed(future_to_index, timeout=timeout):
                idx = future_to_index[future]
                try:
                    results[idx] = {'status': 'ok', 'result': future.result()}
                except Exception as e:
                    results[idx] = {'status': 'error', 'result': str(e)}

        return results


# ── 练习答案演示函数 ─────────────────────────────────────────────────────────

def exercise1_answer() -> None:
    """练习 1：演示 run_five_threads 函数。"""
    print("run_five_threads 演示:")
    run_five_threads()


def exercise2_answer() -> None:
    """练习 2：演示 ThreadSafeCounter 类。"""
    print("ThreadSafeCounter 演示:")

    counter = ThreadSafeCounter()
    print(f"  初始值: {counter.value}")

    def inc_task() -> None:
        for _ in range(1000):
            counter.increment()

    threads = [threading.Thread(target=inc_task) for _ in range(10)]
    for t in threads:
        t.start()
    for t in threads:
        t.join()

    expected = 10 * 1000
    print(f"  期望值: {expected}")
    print(f"  实际值: {counter.value}")
    print(f"  结果{'正确 ✓' if counter.value == expected else '错误 ✗'}")

    # 测试 decrement
    counter.decrement()
    counter.decrement()
    print(f"  decrement 两次后: {counter.value}")


def exercise3_answer() -> None:
    """练习 3：演示 producer_consumer_demo 函数。"""
    print("producer_consumer_demo 演示:")
    print("  2 个生产者 × 10 个随机数（1~100）= 20 个数")
    print("  3 个消费者计算平方后求和")
    total = producer_consumer_demo()
    print(f"  所有平方值之和: {total}")


def exercise4_answer() -> None:
    """练习 4：演示 batch_download 函数。"""
    print("batch_download 演示:")

    results = batch_download(file_count=6, max_workers=3)
    print(f"  文件数: 6，最大并发: 3")
    print()
    print(f"  {'文件名':<20} 耗时")
    print(f"  {'-'*20:<20} {'-'*10}")
    for r in results:
        print(f"  {r['filename']:<20} {r['elapsed']:.3f}秒")

    total_elapsed = sum(r['elapsed'] for r in results)
    slowest = results[0]
    print()
    print(f"  最慢的文件: {slowest['filename']} ({slowest['elapsed']:.3f}秒)")
    print(f"  各文件耗时总和: {total_elapsed:.3f}秒（并发执行，实际远小于此）")


def exercise5_answer() -> None:
    """练习 5：演示 run_task_with_timeout 函数。"""
    print("run_task_with_timeout 演示:")

    # 场景 1：任务在超时前完成
    print()
    print("  场景 1：task_duration=0.5秒，timeout=1.0秒")
    result = run_task_with_timeout(task_duration=0.5, timeout=1.0)
    print(f"  结果: {'正常完成 ✓' if result else '超时退出 ✗'}")

    # 场景 2：任务超时
    print()
    print("  场景 2：task_duration=3.0秒，timeout=0.5秒")
    result = run_task_with_timeout(task_duration=3.0, timeout=0.5)
    print(f"  结果: {'正常完成 ✗' if result else '超时退出 ✓'}")


def exercise6_answer() -> None:
    """练习 6：演示 compare_concurrent_vs_sequential 函数。"""
    print("compare_concurrent_vs_sequential 演示:")
    print("  3 个协程耗时分别为：1.0秒、0.5秒、0.8秒")
    print()

    concurrent_t, sequential_t = asyncio.run(compare_concurrent_vs_sequential())
    print(f"  并发执行总耗时: {concurrent_t:.2f}秒（取决于最慢的 1.0秒）")
    print(f"  顺序执行总耗时: {sequential_t:.2f}秒（1.0 + 0.5 + 0.8 = 2.3秒）")
    print(f"  加速比: {sequential_t / concurrent_t:.1f}x")


def exercise7_answer() -> None:
    """练习 7：演示 fetch_with_timeout 函数。"""
    print("fetch_with_timeout 演示:")

    # 场景 1：超时充足，请求成功
    print()
    print("  场景 1：timeout=3.0秒（请求需要 2 秒，应成功）")
    result = asyncio.run(fetch_with_timeout('https://example.com/api', timeout=3.0))
    print(f"  结果: {result}")

    # 场景 2：超时不足，请求失败
    print()
    print("  场景 2：timeout=1.0秒（请求需要 2 秒，应超时）")
    result = asyncio.run(fetch_with_timeout('https://example.com/api', timeout=1.0))
    print(f"  结果: {result}")


def exercise8_answer() -> None:
    """练习 8：演示 ConcurrentBatchProcessor 类。"""
    print("ConcurrentBatchProcessor 演示:")

    processor = ConcurrentBatchProcessor()

    # 添加各种任务
    processor.add_task(lambda: 1 + 1)
    processor.add_task(lambda: 10 / 0)          # 会抛出 ZeroDivisionError
    processor.add_task(lambda: "hello world")
    processor.add_task(int, "not a number")      # 会抛出 ValueError
    processor.add_task(sum, range(1, 6))         # 计算 1+2+3+4+5=15

    print()
    print(f"  任务数量: {len(processor._tasks)}")
    print()
    results = processor.run(max_workers=3)

    print(f"  {'任务':<6} {'状态':<8} 结果")
    print(f"  {'-'*6:<6} {'-'*8:<8} {'-'*25}")
    for i, r in enumerate(results, 1):
        status_icon = '✓' if r['status'] == 'ok' else '✗'
        print(f"  {i:<6} {status_icon} {r['status']:<6} {r['result']}")

    ok_count = sum(1 for r in results if r['status'] == 'ok')
    err_count = len(results) - ok_count
    print()
    print(f"  成功: {ok_count} 个，失败: {err_count} 个")


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 40)
#     exercise1_answer()
#
#     print("\n" + "=" * 40)
#     exercise2_answer()
#
#     print("\n" + "=" * 40)
#     exercise3_answer()
#
#     print("\n" + "=" * 40)
#     exercise4_answer()
#
#     print("\n" + "=" * 40)
#     exercise5_answer()
#
#     print("\n" + "=" * 40)
#     exercise6_answer()
#
#     print("\n" + "=" * 40)
#     exercise7_answer()
#
#     print("\n" + "=" * 40)
#     exercise8_answer()


# =============================================================================
# 【进阶话题】
# =============================================================================
#
# 1. threading.local()：线程本地存储
#    - 每个线程拥有自己独立的数据副本
#    - 适合存储线程私有状态（如数据库连接、用户会话）
#    - 示例：local_data = threading.local()
#            local_data.user_id = 42  # 只有当前线程能看到
#
# 2. asyncio + 同步代码：run_in_executor
#    - 在协程中执行阻塞的同步函数（如 requests、sqlite3）
#    - loop.run_in_executor(None, blocking_func, *args)
#    - None 表示使用默认线程池
#
# 3. asyncio 异步上下文管理器
#    - async with：用于需要异步 acquire/release 的场景
#    - asyncio.Lock()、asyncio.Semaphore() 的异步版本
#    - async with asyncio.Lock() as lock:  # 协程安全的锁
#
# 4. asyncio 异步迭代器
#    - async for：异步迭代（如流式数据处理）
#    - 定义 __aiter__ 和 __anext__ 方法
#    - async for chunk in async_stream:  # 处理异步数据流
#
# 5. QueueHandler + QueueListener（异步日志）
#    - 日志写入线程安全的 Queue
#    - 后台 Listener 线程异步写入文件
#    - 避免日志 I/O 阻塞主业务线程
#
# 6. multiprocessing.Pool（旧接口）
#    - pool.map(func, iterable)    # 阻塞直到全部完成
#    - pool.apply_async(func, args) # 异步提交单个任务
#    - pool.starmap(func, iterable) # 多参数版 map
#    - 推荐优先使用 ProcessPoolExecutor（更现代）
#
# 7. 协程中的异常传播
#    - gather(return_exceptions=True)：异常作为结果返回，不抛出
#    - gather(return_exceptions=False)：第一个异常立即抛出（默认）
#    - 取消任务：task.cancel() → 协程内收到 CancelledError
#
# 8. 性能调优
#    - 线程池大小：I/O 密集型可以设 CPU 核心数 * 2~4
#    - 进程池大小：CPU 密集型建议等于 CPU 核心数（os.cpu_count()）
#    - asyncio：单线程，避免在协程中做 CPU 密集运算
#    - 避免过细粒度的并发（任务太小，调度开销大于收益）
#
# 9. 调试并发程序
#    - 使用日志而非 print（线程安全）
#    - 为线程/任务设置有意义的名称（name 参数）
#    - asyncio.run(main(), debug=True) 开启调试模式
#    - 使用 threading.settrace() 追踪线程执行
#    - 工具：py-spy（采样分析）、deadlock-monitor（死锁检测）
#
# 10. Python 3.12+ 的变化
#     - 实验性的无 GIL 模式（--disable-gil 编译选项）
#     - 未来可能真正实现 Python 多线程并行
#     - PEP 703：Making the Global Interpreter Lock Optional
