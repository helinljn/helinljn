import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PROJECT25 = ROOT / "07_项目实战" / "chapter25_项目1_文件批量处理工具"
PROJECT26 = ROOT / "07_项目实战" / "chapter26_项目2_日志分析系统"


def load_module(module_name: str, module_path: Path, extra_sys_path: Path):
    """Load a learning-project module from a path with its local imports enabled."""
    sys.path.insert(0, str(extra_sys_path))
    try:
        spec = importlib.util.spec_from_file_location(module_name, module_path)
        if spec is None or spec.loader is None:
            raise ImportError(f"Cannot load module from {module_path}")
        module = importlib.util.module_from_spec(spec)
        sys.modules[module_name] = module
        spec.loader.exec_module(module)
        return module
    finally:
        try:
            sys.path.remove(str(extra_sys_path))
        except ValueError:
            pass


class FileProcessorProjectTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.file_processor = load_module(
            "project25_file_processor",
            PROJECT25 / "file_processor.py",
            PROJECT25,
        )

    def test_search_finds_matching_files_recursively(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "a.txt").write_text("hello", encoding="utf-8")
            (root / "b.md").write_text("markdown", encoding="utf-8")
            (root / "nested").mkdir()
            (root / "nested" / "c.txt").write_text("nested", encoding="utf-8")

            processor = self.file_processor.FileProcessor(dry_run=True, backup=False)
            files = processor.search(root, "*.txt", recursive=True, print_results=False)

            self.assertEqual([p.relative_to(root).as_posix() for p in files], ["a.txt", "nested/c.txt"])

    def test_dry_run_rename_does_not_modify_files(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            source = root / "report.txt"
            source.write_text("version 1", encoding="utf-8")

            processor = self.file_processor.FileProcessor(dry_run=True, backup=False)
            result = processor.rename_add_prefix([source], "new_")

            self.assertEqual(result.total, 1)
            self.assertEqual(result.success, 1)
            self.assertTrue(source.exists())
            self.assertFalse((root / "new_report.txt").exists())

    def test_content_replace_updates_file(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            source = root / "config.txt"
            source.write_text("version=1\n", encoding="utf-8")

            processor = self.file_processor.FileProcessor(dry_run=False, backup=False)
            result = processor.replace_content([source], "version=1", "version=2")

            self.assertEqual(result.success, 1)
            self.assertEqual(source.read_text(encoding="utf-8"), "version=2\n")


class LogAnalyzerProjectTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.log_parser = load_module(
            "project26_log_parser",
            PROJECT26 / "log_parser.py",
            PROJECT26,
        )

    def test_parse_apache_line(self):
        line = '192.168.1.1 - - [01/Jan/2024:10:00:01 +0800] "GET /index.html?x=1 HTTP/1.1" 200 1024 0.050'

        record = self.log_parser.parse_apache_line(line, "access.log")

        self.assertIsNotNone(record)
        self.assertEqual(record.ip, "192.168.1.1")
        self.assertEqual(record.method, "GET")
        self.assertEqual(record.url, "/index.html")
        self.assertEqual(record.status_code, 200)
        self.assertEqual(record.bytes_sent, 1024)
        self.assertEqual(record.response_time, 0.05)

    def test_parse_nginx_line(self):
        line = '10.0.0.1 - - [01/Jan/2024:12:00:00 +0800] "POST /api/login HTTP/1.1" 201 256 "-" "curl/8.0" 0.120'

        record = self.log_parser.parse_nginx_line(line, "access.log")

        self.assertIsNotNone(record)
        self.assertEqual(record.method, "POST")
        self.assertEqual(record.url, "/api/login")
        self.assertEqual(record.status_code, 201)
        self.assertIsNone(record.referer)
        self.assertEqual(record.user_agent, "curl/8.0")

    def test_log_parser_iter_records_tracks_stats(self):
        content = "\n".join(
            [
                '192.168.1.1 - - [01/Jan/2024:10:00:01 +0800] "GET /index.html HTTP/1.1" 200 1024 0.050',
                "invalid line",
                '192.168.1.2 - - [01/Jan/2024:10:00:02 +0800] "GET /about.html HTTP/1.1" 404 512 0.010',
            ]
        )

        with tempfile.NamedTemporaryFile(mode="w", suffix=".log", delete=False, encoding="utf-8") as f:
            f.write(content)
            log_path = f.name

        try:
            parser = self.log_parser.LogParser(log_format="apache")
            records = list(parser.iter_records(log_path))

            self.assertEqual(len(records), 2)
            self.assertEqual(parser.stats["total_lines"], 3)
            self.assertEqual(parser.stats["parsed_lines"], 2)
            self.assertEqual(parser.stats["failed_lines"], 1)
        finally:
            Path(log_path).unlink(missing_ok=True)


if __name__ == "__main__":
    unittest.main()

