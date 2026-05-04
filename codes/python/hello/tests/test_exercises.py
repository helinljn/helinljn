import importlib.util
import io
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
EXERCISES = ROOT / "练习题与答案"

# (module_key, relative_path, expected_first_section_header)
CHAPTER_SMOKE_DATA = [
    # 01_基础篇
    ("ch01", "01_基础篇/chapter01_入门与环境.py", "第 1 章综合演示"),
    ("ch02", "01_基础篇/chapter02_基础语法与数据类型.py", "2.1 变量与动态类型"),
    ("ch03", "01_基础篇/chapter03_控制流程.py", "3.1 条件语句（if-elif-else）"),
    ("ch04", "01_基础篇/chapter04_字符串处理.py", "4.1 字符串基础"),
    ("ch05", "01_基础篇/chapter05_容器数据类型.py", "5.1 列表（list）"),
    # 02_函数篇
    ("ch06", "02_函数篇/chapter06_函数基础.py", "6.1 函数定义与调用"),
    ("ch07", "02_函数篇/chapter07_函数进阶.py", "7.1 Lambda 匿名函数"),
    # 03_模块与面向对象篇
    ("ch08", "03_模块与面向对象篇/chapter08_模块与包.py", "8.1 模块基础"),
    ("ch09", "03_模块与面向对象篇/chapter09_面向对象基础.py", "9.1 类和对象基础"),
    ("ch10", "03_模块与面向对象篇/chapter10_面向对象进阶.py", "10.1 继承基础"),
    ("ch11", "03_模块与面向对象篇/chapter11_异常处理.py", "11.1 异常基础"),
    # 04_高级特性篇
    ("ch12", "04_高级特性篇/chapter12_文件操作.py", "12.1 文件基础操作"),
    ("ch13", "04_高级特性篇/chapter13_迭代器与生成器.py", "13.1 可迭代对象 vs 迭代器"),
    ("ch14", "04_高级特性篇/chapter14_装饰器详解.py", "14.1 装饰器基础回顾"),
    # 05_标准库篇
    ("ch15", "05_标准库篇/chapter15_系统与环境.py", "15.1 sys 模块：系统参数与标准流"),
    ("ch16", "05_标准库篇/chapter16_文本处理.py", "16.1 json 模块：JSON 数据处理"),
    ("ch17", "05_标准库篇/chapter17_日期与时间.py", "17.1 datetime.date：纯日期"),
    ("ch18", "05_标准库篇/chapter18_数据结构工具.py", "18.1 collections.Counter：计数器"),
    ("ch19", "05_标准库篇/chapter19_函数式编程工具.py", "19.1 operator 模块：将运算符变成函数"),
    ("ch20", "05_标准库篇/chapter20_正则表达式.py", "20.1 正则表达式基础：元字符与字符类"),
    ("ch21", "05_标准库篇/chapter21_日志与调试.py", "21.1 logging 模块基础：日志级别与快速入门"),
    # 06_并发与数据库
    ("ch22", "06_并发与数据库/chapter22_并发编程入门.py", "22.1 并发基础概念"),
    ("ch23", "06_并发与数据库/chapter23_数据库操作.py", "23.1 数据库基础概念"),
    # ch21 补充
    ("ch21_supp01", "05_标准库篇/chapter21_补充01_网络与HTTP基础.py", "21.补充1.1 URL 解析与构造"),
    ("ch21_supp02", "05_标准库篇/chapter21_补充02_数学与数值工具.py", "21.补充2.1 math 模块"),
    ("ch21_supp03", "05_标准库篇/chapter21_补充03_文件目录工具.py", "21.补充3.1 glob 文件查找"),
    ("ch21_supp04", "05_标准库篇/chapter21_补充04_类型注解进阶.py", "21.补充4.1 Optional、Union、Literal"),
    ("ch21_supp05", "05_标准库篇/chapter21_补充05_struct二进制数据处理.py", "21.补充5.1 struct 基础：pack 与 unpack"),
    ("ch21_supp06", "05_标准库篇/chapter21_补充06_hashlib哈希与摘要.py", "21.补充6.1 hashlib 基础：MD5、SHA1、SHA256"),
    ("ch21_supp07", "05_标准库篇/chapter21_补充07_bytes与bytearray深入.py", "21.补充7.1 bytes 基础"),
    ("ch21_supp08", "05_标准库篇/chapter21_补充08_enum枚举.py", "21.补充8.1 Enum 基础"),
    ("ch21_supp09", "05_标准库篇/chapter21_补充09_uuid唯一标识符.py", "21.补充9.1 UUID 类型与生成"),
    ("ch21_supp10", "05_标准库篇/chapter21_补充10_ctypes调用C动态库.py", "21.补充10.1 ctypes 基础：加载库与简单调用"),
    ("ch21_supp11", "05_标准库篇/chapter21_补充11_性能分析与优化.py", "21.补充11.1 timeit 模块：测量代码执行时间"),
    ("ch21_supp12", "05_标准库篇/chapter21_补充12_内存管理工具.py", "21.补充12.1 array 模块：类型化数组"),
]


def load_module(module_name: str, module_path: Path):
    spec = importlib.util.spec_from_file_location(module_name, module_path)
    if spec is None or spec.loader is None:
        raise ImportError(f"Cannot load module from {module_path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[module_name] = module
    spec.loader.exec_module(module)
    return module


class BasicExerciseTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ex01 = load_module("exercise_01_basic", EXERCISES / "exercises_01_基础.py")

    def test_kaprekar_steps(self):
        steps, sequence = self.ex01.kaprekar_steps(3524)

        self.assertEqual(steps, 3)
        self.assertEqual(sequence, [3087, 8352, 6174])
        self.assertEqual(self.ex01.kaprekar_steps(1111), (-1, []))

    def test_extract_and_analyze_numbers(self):
        result = self.ex01.extract_and_analyze("价格12.5元，数量3个，折扣0.8，温度-5度")

        self.assertEqual(result["numbers"], [12.5, 3.0, 0.8, -5.0])
        self.assertEqual(result["count"], 4)
        self.assertAlmostEqual(result["sum"], 11.3)
        self.assertEqual(result["min"], -5.0)

    def test_pascal_triangle(self):
        self.assertEqual(
            self.ex01.pascal_triangle(5),
            [[1], [1, 1], [1, 2, 1], [1, 3, 3, 1], [1, 4, 6, 4, 1]],
        )
        with self.assertRaises(ValueError):
            self.ex01.pascal_triangle(-1)

    def test_shopping_cart_checkout_uses_inventory_prices(self):
        inventory = {
            "苹果": {"price": 5.0, "stock": 100},
            "笔记本": {"price": 25.0, "stock": 10},
        }
        cart = self.ex01.ShoppingCart()

        self.assertIn("已添加", self.ex01.add_to_cart(cart, "苹果", 3, inventory))
        self.assertIn("已添加", self.ex01.add_to_cart(cart, "笔记本", 2, inventory))
        total, receipt = self.ex01.checkout(cart, inventory)

        self.assertEqual(total, 65.0)
        self.assertEqual(inventory["苹果"]["stock"], 97)
        self.assertIn("65.00", "\n".join(receipt))


class FunctionExerciseTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ex02 = load_module("exercise_02_functions", EXERCISES / "exercises_02_函数.py")

    def test_pipe_and_compose(self):
        pipe_func = self.ex02.pipe(lambda x: x + 1, lambda x: x * 2, str)
        compose_func = self.ex02.compose(str, lambda x: x * 2, lambda x: x + 1)

        self.assertEqual(pipe_func(5), "12")
        self.assertEqual(compose_func(5), "12")
        self.assertEqual(self.ex02.pipe()(42), 42)
        self.assertEqual(self.ex02.compose()(42), 42)

    def test_cache_with_ttl_reuses_cached_result(self):
        calls = {"count": 0}

        @self.ex02.cache_with_ttl(seconds=60)
        def add(a, b):
            calls["count"] += 1
            return a + b

        self.assertEqual(add(1, 2), 3)
        self.assertEqual(add(1, 2), 3)
        self.assertEqual(calls["count"], 1)

    def test_validate_rejects_wrong_type(self):
        @self.ex02.validate(name=str, age=int)
        def register(name, age):
            return f"{name}:{age}"

        self.assertEqual(register("Alice", 20), "Alice:20")
        with self.assertRaises(TypeError):
            register("Alice", "20")


class OOPExerciseTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ex03 = load_module("exercise_03_oop", EXERCISES / "exercises_03_面向对象.py")

    def test_bank_accounts(self):
        savings = self.ex03.SavingsAccount("Alice", 100.0)
        savings.deposit(50.0)
        savings.withdraw(30.0)

        self.assertEqual(savings.balance, 120.0)
        with self.assertRaises(self.ex03.InsufficientFundsError):
            savings.withdraw(1000.0)

        credit = self.ex03.CreditAccount("Bob", 0.0, credit_limit=500.0)
        credit.withdraw(400.0)
        self.assertEqual(credit.balance, -400.0)

    def test_dataclass_models(self):
        point = self.ex03.Point(1, 2) + self.ex03.Point(3, 4)
        rect = self.ex03.Rectangle(self.ex03.Point(0, 0), width=10, height=5)

        self.assertEqual((point.x, point.y), (4, 6))
        self.assertEqual(self.ex03.Color(255, 0, 16).to_hex(), "#FF0010")
        self.assertEqual(rect.area, 50)
        self.assertTrue(rect.contains(self.ex03.Point(5, 3)))
        self.assertFalse(rect.contains(self.ex03.Point(11, 3)))
        with self.assertRaises(ValueError):
            self.ex03.Color(256, 0, 0)

    def test_event_emitter_on_once_off(self):
        emitter = self.ex03.EventEmitter()
        events = []

        def listener(value):
            events.append(("on", value))

        def once_listener(value):
            events.append(("once", value))

        emitter.on("data", listener)
        emitter.once("data", once_listener)
        emitter.emit("data", 1)
        emitter.emit("data", 2)
        emitter.off("data", listener)
        emitter.emit("data", 3)

        self.assertEqual(events, [("on", 1), ("once", 1), ("on", 2)])


class ComprehensiveExerciseTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.ex04 = load_module("exercise_04_comprehensive", EXERCISES / "exercises_04_综合.py")

    def test_parse_and_analyze_log_file(self):
        content = "\n".join(
            [
                '192.168.1.1 - - [01/Jan/2024:13:00:35 +0000] "GET /index.html HTTP/1.1" 200 2326',
                '10.0.0.5 - - [01/Jan/2024:13:01:12 +0000] "POST /api/login HTTP/1.1" 500 1250',
                "invalid line",
            ]
        )

        with tempfile.NamedTemporaryFile(mode="w", suffix=".log", delete=False, encoding="utf-8") as f:
            f.write(content)
            log_path = f.name

        try:
            entries = list(self.ex04.parse_log_lines(log_path))
            report = self.ex04.analyze_log_file(log_path)

            self.assertEqual(len(entries), 3)
            self.assertTrue(entries[2]["parse_error"])
            self.assertEqual(report["total_requests"], 2)
            self.assertEqual(report["status_distribution"], {200: 1, 500: 1})
        finally:
            Path(log_path).unlink(missing_ok=True)

    def test_csv_pipeline(self):
        csv_content = "\n".join(
            [
                "name,category,price,quantity",
                "A,book,10.5,2",
                "B,book,20,1",
                "C,tool,5,3",
                "broken,tool,nope,1",
            ]
        )

        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            input_path = root / "products.csv"
            output_path = root / "report.json"
            input_path.write_text(csv_content, encoding="utf-8")

            result = self.ex04.csv_pipeline(
                str(input_path),
                str(output_path),
                group_by="category",
                aggregate_field="price",
                required_fields=["name", "category", "price"],
                numeric_fields=["price", "quantity"],
            )

            self.assertTrue(output_path.exists())
            self.assertEqual(result["results"]["book"]["count"], 2)
            self.assertEqual(result["results"]["book"]["sum"], 30.5)
            self.assertEqual(result["results"]["tool"]["count"], 1)

    def test_task_queue_lifecycle(self):
        queue = self.ex04.TaskQueue(":memory:")
        try:
            task_id = queue.enqueue("send_email", {"to": "user@example.com"})
            task = queue.dequeue()

            self.assertEqual(task["id"], task_id)
            self.assertEqual(task["task_name"], "send_email")
            self.assertEqual(task["payload"], {"to": "user@example.com"})
            self.assertEqual(queue.stats(), {"processing": 1})

            queue.complete(task_id, success=True)
            self.assertEqual(queue.stats(), {"completed": 1})
        finally:
            queue.close()


class ChapterRuntimeSmokeTests(unittest.TestCase):
    """烟雾测试：验证所有章节的 main() 能完整跑通，不校验具体业务逻辑。"""

    @classmethod
    def setUpClass(cls):
        cls.chapters = {}
        for key, rel_path, _header in CHAPTER_SMOKE_DATA:
            module_name = key.replace("ch21_supp", "chapter21_supplement")
            with redirect_stdout(io.StringIO()):
                cls.chapters[key] = load_module(module_name, ROOT / rel_path)

    def test_all_chapters_main_runs(self):
        """验证 ch01~ch23 及 ch21 全部补充章节的 main() 不崩溃且输出首节标题。"""
        for key, _rel_path, expected_header in CHAPTER_SMOKE_DATA:
            with self.subTest(chapter=key, header=expected_header):
                output = io.StringIO()
                try:
                    with redirect_stdout(output):
                        self.chapters[key].main()
                except SystemExit:
                    pass
                text = output.getvalue()
                self.assertIn(expected_header, text,
                    f"{key} 的 main() 输出中未找到节标题 '{expected_header}'")

    # ── 以下是对特定章节的深度烟雾测试（校验多个输出点） ──

    def test_chapter11_new_exception_sections_run(self):
        buffer = io.StringIO()
        with redirect_stdout(buffer):
            self.chapters["ch11"].demo_exception_best_practices()
            self.chapters["ch11"].demo_exception_group()
        output = buffer.getvalue()
        self.assertIn("11.7 异常处理最佳实践", output)
        self.assertIn("11.8 ExceptionGroup", output)

    def test_chapter21_network_demos_run_locally(self):
        output = io.StringIO()
        with redirect_stdout(output):
            self.chapters["ch21_supp01"].main()
        text = output.getvalue()
        self.assertIn("21.补充1.1 URL 解析与构造", text)
        self.assertIn("/health 响应", text)
        self.assertIn("客户端收到: echo: hello socket", text)

    def test_chapter21_numeric_supplement_runs(self):
        output = io.StringIO()
        with redirect_stdout(output):
            self.chapters["ch21_supp02"].main()
        text = output.getvalue()
        self.assertIn("21.补充2.1 math 模块", text)
        self.assertEqual(round(self.chapters["ch21_supp02"].distance(0, 0, 3, 4), 2), 5.0)
        self.assertEqual(str(self.chapters["ch21_supp02"].split_bill("10.00", 4)), "2.50")

    def test_chapter21_file_tools_supplement_runs(self):
        output = io.StringIO()
        with redirect_stdout(output):
            self.chapters["ch21_supp03"].main()
        text = output.getvalue()
        self.assertIn("21.补充3.1 glob 文件查找", text)
        self.assertIn("离开 with 后临时目录会自动清理", text)

    def test_chapter21_typing_supplement_runs(self):
        output = io.StringIO()
        with redirect_stdout(output):
            self.chapters["ch21_supp04"].main()
        text = output.getvalue()
        self.assertIn("21.补充4.1 Optional、Union、Literal", text)
        self.assertEqual(self.chapters["ch21_supp04"].last_or_none([1, 2, 3]), 3)
        self.assertIsNone(self.chapters["ch21_supp04"].last_or_none([]))


if __name__ == "__main__":
    unittest.main()
