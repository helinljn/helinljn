import importlib.util
import io
import sys
import tempfile
import unittest
from contextlib import redirect_stdout
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
EXERCISES = ROOT / "练习题与答案"
CHAPTER11 = ROOT / "03_模块与面向对象篇" / "chapter11_异常处理.py"
CHAPTER21_NETWORK = ROOT / "05_标准库篇" / "chapter21_补充01_网络与HTTP基础.py"
CHAPTER21_NUMERIC = ROOT / "05_标准库篇" / "chapter21_补充02_数学与数值工具.py"
CHAPTER21_FILE_TOOLS = ROOT / "05_标准库篇" / "chapter21_补充03_文件目录工具.py"
CHAPTER21_TYPING = ROOT / "05_标准库篇" / "chapter21_补充04_类型注解进阶.py"


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
    @classmethod
    def setUpClass(cls):
        cls.chapter11 = load_module("chapter11_exceptions", CHAPTER11)
        cls.chapter21_network = load_module("chapter21_network", CHAPTER21_NETWORK)
        cls.chapter21_numeric = load_module("chapter21_numeric", CHAPTER21_NUMERIC)
        cls.chapter21_file_tools = load_module("chapter21_file_tools", CHAPTER21_FILE_TOOLS)
        cls.chapter21_typing = load_module("chapter21_typing", CHAPTER21_TYPING)

    def test_chapter11_new_exception_sections_run(self):
        buffer = io.StringIO()

        with redirect_stdout(buffer):
            self.chapter11.demo_exception_best_practices()
            self.chapter11.demo_exception_group()

        output = buffer.getvalue()
        self.assertIn("11.7 异常处理最佳实践", output)
        self.assertIn("11.8 ExceptionGroup", output)

    def test_chapter21_network_demos_run_locally(self):
        output = io.StringIO()

        with redirect_stdout(output):
            self.chapter21_network.main()

        text = output.getvalue()
        self.assertIn("21.补充1.1 URL 解析与构造", text)
        self.assertIn("/health 响应", text)
        self.assertIn("客户端收到: echo: hello socket", text)

    def test_chapter21_numeric_supplement_runs(self):
        output = io.StringIO()

        with redirect_stdout(output):
            self.chapter21_numeric.main()

        text = output.getvalue()
        self.assertIn("21.补充2.1 math 模块", text)
        self.assertEqual(round(self.chapter21_numeric.distance(0, 0, 3, 4), 2), 5.0)
        self.assertEqual(str(self.chapter21_numeric.split_bill("10.00", 4)), "2.50")

    def test_chapter21_file_tools_supplement_runs(self):
        output = io.StringIO()

        with redirect_stdout(output):
            self.chapter21_file_tools.main()

        text = output.getvalue()
        self.assertIn("21.补充3.1 glob 文件查找", text)
        self.assertIn("离开 with 后临时目录会自动清理", text)

    def test_chapter21_typing_supplement_runs(self):
        output = io.StringIO()

        with redirect_stdout(output):
            self.chapter21_typing.main()

        text = output.getvalue()
        self.assertIn("21.补充4.1 Optional、Union、Literal", text)
        self.assertEqual(self.chapter21_typing.last_or_none([1, 2, 3]), 3)
        self.assertIsNone(self.chapter21_typing.last_or_none([]))

    def test_chapter11_main_runs(self):
        buffer = io.StringIO()

        with redirect_stdout(buffer):
            self.chapter11.main()

        output = buffer.getvalue()
        self.assertIn("11.1 异常基础", output)
        self.assertIn("11.7 异常处理最佳实践", output)
        self.assertIn("11.8 ExceptionGroup", output)


if __name__ == "__main__":
    unittest.main()
