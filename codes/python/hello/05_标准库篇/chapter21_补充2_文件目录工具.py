# =============================================================================
# 第 21 章：补充 2 - 文件目录工具
# =============================================================================
#
# 【学习目标】
#   1. 掌握 glob 按通配符查找文件
#   2. 掌握 shutil 复制、移动、删除文件和目录
#   3. 掌握 tempfile 创建临时文件和临时目录
#   4. 理解临时目录适合做测试和演示
#   5. 能写出不污染真实工作目录的文件处理代码
#
# 【运行方式】
#   python chapter21_补充2_文件目录工具.py
#
# =============================================================================

from pathlib import Path
import glob
import shutil
import tempfile


# =============================================================================
# 21.补充2.1 glob 文件查找
# =============================================================================


def find_files(root: Path, pattern: str) -> list[Path]:
    """使用 glob 递归查找匹配文件。"""
    matches = glob.glob(str(root / "**" / pattern), recursive=True)
    return sorted(Path(match) for match in matches)


def demo_glob_module(root: Path) -> None:
    """演示 glob 通配符查找。"""
    print("=" * 60)
    print("21.补充2.1 glob 文件查找")
    print("=" * 60)

    txt_files = find_files(root, "*.txt")
    py_files = find_files(root, "*.py")

    print("txt 文件:", [p.relative_to(root).as_posix() for p in txt_files])
    print("py 文件:", [p.relative_to(root).as_posix() for p in py_files])


# =============================================================================
# 21.补充2.2 shutil 文件和目录操作
# =============================================================================


def copy_by_extension(source: Path, target: Path, extension: str) -> list[Path]:
    """复制指定扩展名文件到目标目录。"""
    target.mkdir(parents=True, exist_ok=True)
    copied: list[Path] = []

    for path in find_files(source, f"*{extension}"):
        destination = target / path.name
        shutil.copy2(path, destination)
        copied.append(destination)

    return copied


def demo_shutil_module(root: Path) -> None:
    """演示 shutil 复制目录和文件。"""
    print("\n" + "=" * 60)
    print("21.补充2.2 shutil 文件和目录操作")
    print("=" * 60)

    backup = root / "backup"
    copied = copy_by_extension(root / "src", backup, ".txt")
    print("复制 txt:", [p.name for p in copied])

    clone = root / "src_clone"
    shutil.copytree(root / "src", clone)
    print("复制目录存在:", clone.exists())

    moved = root / "moved_notes.txt"
    shutil.move(str(backup / "notes.txt"), moved)
    print("移动文件存在:", moved.exists())


# =============================================================================
# 21.补充2.3 tempfile 临时文件和临时目录
# =============================================================================


def create_demo_workspace(root: Path) -> None:
    """创建用于演示的临时工作区内容。"""
    (root / "src" / "nested").mkdir(parents=True)
    (root / "src" / "notes.txt").write_text("hello\n", encoding="utf-8")
    (root / "src" / "nested" / "data.txt").write_text("data\n", encoding="utf-8")
    (root / "src" / "app.py").write_text("print('ok')\n", encoding="utf-8")


def demo_tempfile_module() -> None:
    """演示 tempfile 自动清理临时目录。"""
    print("=" * 60)
    print("21.补充2.3 tempfile 临时文件和临时目录")
    print("=" * 60)

    with tempfile.TemporaryDirectory() as tmp:
        root = Path(tmp)
        create_demo_workspace(root)

        print("临时目录:", root)
        demo_glob_module(root)
        demo_shutil_module(root)

        with tempfile.NamedTemporaryFile(mode="w+", suffix=".txt", encoding="utf-8") as f:
            f.write("temporary text")
            f.seek(0)
            print("临时文件内容:", f.read())

    print("离开 with 后临时目录会自动清理")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_tempfile_module()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. glob
#    - glob.glob("**/*.txt", recursive=True)：递归匹配文件
#    - 通配符：* 匹配任意字符，? 匹配单个字符
#
# 2. shutil
#    - copy2(src, dst)：复制文件并尽量保留元数据
#    - copytree(src, dst)：复制整个目录
#    - move(src, dst)：移动文件或目录
#    - rmtree(path)：递归删除目录，使用前必须确认路径
#
# 3. tempfile
#    - TemporaryDirectory()：创建临时目录，with 结束自动删除
#    - NamedTemporaryFile()：创建临时文件
#    - 适合测试、演示、处理中间文件


# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个函数 count_by_extension(root)，统计目录下每种扩展名的文件数量。
#
# 练习 2（基础）：
#   写一个函数 copy_txt_files(source, target)，复制所有 txt 文件。
#
# 练习 3（进阶）：
#   写一个函数 make_backup(source)，把 source 目录复制到 source_backup。
#
# 练习答案提示：
#   练习1：使用 Path.rglob("*") 或 glob.glob("**/*", recursive=True)
#   练习2：参考 copy_by_extension(source, target, ".txt")
#   练习3：使用 shutil.copytree，目标已存在时要先给出清晰错误


# =============================================================================
# 【练习答案】
# =============================================================================


def count_by_extension(root: Path) -> dict[str, int]:
    """练习 1 答案：按扩展名统计文件数量。"""
    result: dict[str, int] = {}
    for path in root.rglob("*"):
        if path.is_file():
            suffix = path.suffix or "<no_ext>"
            result[suffix] = result.get(suffix, 0) + 1
    return result


def make_backup(source: Path) -> Path:
    """练习 3 答案：复制目录为备份目录。"""
    target = source.with_name(source.name + "_backup")
    if target.exists():
        raise FileExistsError(f"备份目录已存在: {target}")
    return Path(shutil.copytree(source, target))


# 取消注释以运行练习：
# if __name__ == "__main__":
#     with tempfile.TemporaryDirectory() as tmp:
#         root = Path(tmp)
#         create_demo_workspace(root)
#
#         print("=" * 40)
#         print(count_by_extension(root))
#
#         print("\n" + "=" * 40)
#         print(make_backup(root / "src"))
