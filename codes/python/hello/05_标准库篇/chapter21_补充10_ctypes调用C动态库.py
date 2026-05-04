# =============================================================================
# 第 21 章：补充 10 - ctypes 调用 C 动态库
# =============================================================================
#
# 【学习目标】
#   1. 掌握 ctypes 加载 DLL/SO 和调用 C 函数
#   2. 理解 ctypes 数据类型与 C 类型的对应
#   3. 掌握传值与传址（byref / pointer）的区别
#   4. 理解回调函数（C 调用 Python 函数）
#   5. 理解 ctypes 对 C/C++ 开发者的价值
#
# 【与 C/C++ 的对比】
#   通常你用 C/C++ 写核心逻辑，用 Python 做胶水层。ctypes 让你直接在 Python
#   中调用 C 动态库，无需编写 C 扩展模块。它适合快速原型验证和调用第三方库。
#   对于性能要求高的场景，建议用 Cython 或 pybind11 代替。
#
# 【运行方式】
#   python chapter21_补充10_ctypes调用C动态库.py
#
# =============================================================================

import ctypes
from ctypes import (
    c_int, c_uint, c_float, c_double, c_char_p, c_void_p,
    c_size_t, c_bool, POINTER, byref, pointer, cast,
    Structure, Union, CFUNCTYPE, create_string_buffer,
)
from pathlib import Path
import sys


# =============================================================================
# 21.补充10.1 ctypes 基础：加载库与简单调用
# =============================================================================

def demo_ctypes_basics() -> None:
    """演示加载系统 C 库和调用基本函数。"""
    print("=" * 60)
    print("21.补充10.1 ctypes 基础：加载库与简单调用")
    print("=" * 60)

    # ── 加载动态库 ────────────────────────────────────────
    # Windows: 加载 msvcrt.dll (Microsoft C Runtime)
    # Linux:  加载 libc.so.6
    # macOS:  加载 libc.dylib
    if sys.platform == "win32":
        libc = ctypes.CDLL("msvcrt.dll")
        libm = ctypes.CDLL("msvcrt.dll")
        printf = libc.printf
        abs_func = libc.abs
        # msvcrt 没有 math 函数，用标准库替代
    else:
        libc = ctypes.CDLL(None)  # None 表示加载当前进程的 C 库
        libm = ctypes.CDLL(None)

    print(f"平台: {sys.platform}")
    print(f"已加载 C 运行时库")

    # ── 设置参数类型和返回值类型 ──────────────────────────
    # 不设置的话，ctypes 默认认为参数和返回值都是 int

    # 调用 C 的 abs()
    if sys.platform == "win32":
        libc.abs.argtypes = [c_int]
        libc.abs.restype = c_int
        print(f"\nC abs(-42) = {libc.abs(-42)}")
    else:
        # Linux/macOS 使用 libc 的 abs
        print(f"\n(libc.abs 示例跳过，Windows 下可用)")

    # ── 调用 C 的 printf ──────────────────────────────────
    if sys.platform == "win32":
        printf.argtypes = [c_char_p]
        printf.restype = c_int
        print(f"\nC printf:")
        msg = "  Hello from C printf!\n".encode("utf-8")
        chars_written = printf(msg)
        print(f"  C printf 返回值: {chars_written} chars")

    # ── ctypes 类型速查 ───────────────────────────────────
    print(f"\nctypes ↔ C 类型速查:")
    types = [
        ("c_char",    "char",          "1B"),
        ("c_byte",    "signed char",   "1B"),
        ("c_ubyte",   "unsigned char", "1B"),
        ("c_short",   "short",         "2B"),
        ("c_ushort",  "unsigned short","2B"),
        ("c_int",     "int",           "4B"),
        ("c_uint",    "unsigned int",  "4B"),
        ("c_longlong","long long",     "8B"),
        ("c_float",   "float",         "4B"),
        ("c_double",  "double",        "8B"),
        ("c_char_p",  "char*",         "指针"),
        ("c_void_p",  "void*",         "指针"),
        ("c_size_t",  "size_t",        "8B(64位)"),
    ]
    for ct, ctype, size in types:
        print(f"  {ct:14s} ↔ {ctype:16s} ({size})")


# =============================================================================
# 21.补充10.2 用 ctypes 调用 Windows API
# =============================================================================

def demo_windows_api() -> None:
    """演示用 ctypes 调用 Windows API（MessageBox）。"""
    print("\n" + "=" * 60)
    print("21.补充10.2 调用 Windows API")
    print("=" * 60)

    if sys.platform != "win32":
        print("  此节仅在 Windows 上有效，当前平台: " + sys.platform)
        return

    # 加载 user32.dll
    user32 = ctypes.WinDLL("user32.dll")  # WinDLL 用于 stdcall 调用约定

    # MessageBoxW 的函数签名：
    # int MessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType);
    msgbox = user32.MessageBoxW
    msgbox.argtypes = [c_void_p, ctypes.c_wchar_p, ctypes.c_wchar_p, c_uint]
    msgbox.restype = c_int

    print("  调用 Windows MessageBoxW:")
    result = msgbox(
        0,  # NULL 父窗口
        "这是 Python 通过 ctypes 调用的消息框！\n\n点击确定继续。",
        "ctypes 演示",
        0x40 | 0x1,  # MB_ICONINFORMATION | MB_OKCANCEL
    )

    result_names = {1: "IDOK (确定)", 2: "IDCANCEL (取消)"}
    print(f"  用户点击: {result_names.get(result, result)}")


# =============================================================================
# 21.补充10.3 结构体与传址
# =============================================================================

def demo_structs_and_pointers() -> None:
    """演示 ctypes 的 Structure、pointer 和 byref。"""
    print("\n" + "=" * 60)
    print("21.补充10.3 结构体与传址")
    print("=" * 60)

    # ── 定义与 C 结构体对应的 Python 类 ────────────────────
    class Point(Structure):
        _fields_ = [
            ("x", c_int),
            ("y", c_int),
        ]

    class Rect(Structure):
        _fields_ = [
            ("left", c_int),
            ("top", c_int),
            ("right", c_int),
            ("bottom", c_int),
        ]

    # ── 创建和使用结构体 ──────────────────────────────────
    p = Point(10, 20)
    print(f"Point: x={p.x}, y={p.y}")
    print(f"  sizeof(Point) = {ctypes.sizeof(Point)} 字节")

    r = Rect(0, 0, 100, 100)
    print(f"Rect:  ({r.left}, {r.top}) → ({r.right}, {r.bottom})")
    print(f"  sizeof(Rect)  = {ctypes.sizeof(Rect)} 字节")

    # ── 指针操作 ──────────────────────────────────────────
    print(f"\n指针操作:")

    # byref: 获取指针（仅用于函数调用，不创建持久对象）
    p_ptr = byref(p)
    print(f"  byref(p) = {p_ptr}  (轻量指针，仅用于函数调用)")

    # pointer: 创建持久指针对象
    p_pointer = pointer(p)
    print(f"  pointer(p) = {p_pointer}  (持久指针对象)")

    # 通过指针访问
    print(f"  p_pointer.contents.x = {p_pointer.contents.x}")
    print(f"  p_pointer.contents.y = {p_pointer.contents.y}")

    # ── 指针强制转换（类似 C 的指针 cast）─────────────────
    print(f"\n指针 cast 示例:")
    # 把一个 Point 强制当作 2 个 int 的数组看待
    p2 = Point(0x41, 0x42)
    int_ptr = cast(byref(p2), POINTER(c_int))
    print(f"  Point(0x41, 0x42) → [0]={int_ptr[0]:#06x}, [1]={int_ptr[1]:#06x}")

    # ── 嵌套结构体 ────────────────────────────────────────
    class Line(Structure):
        _fields_ = [
            ("start", Point),
            ("end", Point),
        ]

    line = Line(Point(0, 0), Point(100, 100))
    print(f"\n嵌套结构体 Line:")
    print(f"  start=({line.start.x}, {line.start.y})")
    print(f"  end=({line.end.x}, {line.end.y})")
    print(f"  sizeof(Line) = {ctypes.sizeof(Line)} 字节")


# =============================================================================
# 21.补充10.4 回调函数（C 调用 Python）
# =============================================================================

def demo_callbacks() -> None:
    """演示将 Python 函数作为回调传递给 C。"""
    print("\n" + "=" * 60)
    print("21.补充10.4 回调函数（C 调用 Python）")
    print("=" * 60)

    # ── 定义回调函数类型 ──────────────────────────────────
    # 对应 C 的: int (*compare)(const void*, const void*)
    COMPARE_FUNC = CFUNCTYPE(c_int, c_void_p, c_void_p)

    # ── 实现 Python 回调 ──────────────────────────────────
    @COMPARE_FUNC
    def py_compare_int(a: c_void_p, b: c_void_p) -> int:
        """Python 函数作为 C 的 qsort 比较器。"""
        # 把 void* 转成 int*
        val_a = cast(a, POINTER(c_int))[0]
        val_b = cast(b, POINTER(c_int))[0]
        return val_a - val_b  # 升序

    if sys.platform == "win32":
        libc = ctypes.CDLL("msvcrt.dll")
    else:
        libc = ctypes.CDLL(None)

    # ── qsort 签名 ────────────────────────────────────────
    # void qsort(void *base, size_t nitems, size_t size,
    #            int (*compar)(const void *, const void *))
    try:
        qsort = libc.qsort
        qsort.argtypes = [c_void_p, c_size_t, c_size_t, COMPARE_FUNC]
        qsort.restype = None

        # 准备数组
        ArrayType = c_int * 6
        arr = ArrayType(42, 7, 19, 3, 88, 1)

        print(f"排序前: {list(arr)}")
        qsort(arr, len(arr), ctypes.sizeof(c_int), py_compare_int)
        print(f"排序后: {list(arr)}")
        print(f"  (C qsort 调用了 Python 的比较函数 6 次以上)")
    except AttributeError:
        print(f"  (当前平台无 qsort，跳过演示)")

    # ── 回调的注意事项 ────────────────────────────────────
    print(f"\n回调注意事项:")
    print(f"  1. 回调函数必须保持引用，否则可能被 GC 回收导致崩溃")
    print(f"  2. @CFUNCTYPE 装饰器会创建 C 函数指针，保持回调对象存活")
    print(f"  3. 回调中抛出的异常不会传播到 C 代码，会被吞掉")
    print(f"  4. 注意调用约定：CFUNCTYPE 用 cdecl，WINFUNCTYPE 用 stdcall")


# =============================================================================
# 21.补充10.5 实操：调用 Python 本身的 C API
# =============================================================================

def demo_practical() -> None:
    """演示用 ctypes 做点实际的事情。"""
    print("\n" + "=" * 60)
    print("21.补充10.5 实操：获取系统信息")
    print("=" * 60)

    # ── 使用 ctypes 获取屏幕分辨率（Windows） ──────────────
    if sys.platform == "win32":
        user32 = ctypes.WinDLL("user32.dll")
        # int GetSystemMetrics(int nIndex);
        user32.GetSystemMetrics.argtypes = [c_int]
        user32.GetSystemMetrics.restype = c_int

        SM_CXSCREEN = 0  # 主屏幕宽度
        SM_CYSCREEN = 1  # 主屏幕高度

        w = user32.GetSystemMetrics(SM_CXSCREEN)
        h = user32.GetSystemMetrics(SM_CYSCREEN)
        print(f"屏幕分辨率: {w} x {h}")

    # ── 获取系统内存信息 ──────────────────────────────────
    # 使用 ctypes 调用 kernel32 获取内存状态
    if sys.platform == "win32":
        class MEMORYSTATUSEX(Structure):
            _fields_ = [
                ("dwLength", c_uint),
                ("dwMemoryLoad", c_uint),
                ("ullTotalPhys", ctypes.c_ulonglong),
                ("ullAvailPhys", ctypes.c_ulonglong),
                ("ullTotalPageFile", ctypes.c_ulonglong),
                ("ullAvailPageFile", ctypes.c_ulonglong),
                ("ullTotalVirtual", ctypes.c_ulonglong),
                ("ullAvailVirtual", ctypes.c_ulonglong),
                ("ullAvailExtendedVirtual", ctypes.c_ulonglong),
            ]

        kernel32 = ctypes.WinDLL("kernel32.dll")
        kernel32.GlobalMemoryStatusEx.argtypes = [POINTER(MEMORYSTATUSEX)]
        kernel32.GlobalMemoryStatusEx.restype = c_bool

        mem = MEMORYSTATUSEX()
        mem.dwLength = ctypes.sizeof(MEMORYSTATUSEX)

        if kernel32.GlobalMemoryStatusEx(byref(mem)):
            total_gb = mem.ullTotalPhys / (1024**3)
            avail_gb = mem.ullAvailPhys / (1024**3)
            print(f"物理内存: {total_gb:.1f} GB 总量, {avail_gb:.1f} GB 可用")
            print(f"内存使用率: {mem.dwMemoryLoad}%")
    else:
        print(f"  (系统信息获取当前仅演示 Windows 平台)")
        print(f"  Linux 可使用 /proc/meminfo，macOS 可使用 sysctl")

    # ── ctypes 的限制 ─────────────────────────────────────
    print(f"\nctypes 的局限:")
    print(f"  1. 类型映射手动配置，容易出错")
    print(f"  2. 没有编译期间类型检查（C 的 header 相当于文档）")
    print(f"  3. 复杂 C++ 类/模板无法直接调用（需 C 包装层）")
    print(f"  4. 性能敏感场景建议用 Cython / pybind11 / cffi")


# =============================================================================
# 主程序
# =============================================================================

def main() -> None:
    """运行本章补充的所有演示函数。"""
    demo_ctypes_basics()
    demo_windows_api()
    demo_structs_and_pointers()
    demo_callbacks()
    demo_practical()


if __name__ == "__main__":
    main()


# =============================================================================
# 【语法总结】
# =============================================================================
#
# 1. 加载动态库
#    ctypes.CDLL("xxx.dll")       cdll 调用约定
#    ctypes.WinDLL("xxx.dll")     Win32 stdcall 约定
#    ctypes.CDLL(None)            加载当前进程的 C 库
#
# 2. 类型 (ctypes 对象)
#    c_int, c_float, c_double, c_char_p, c_void_p, c_size_t
#    c_int(42)                    创建 ctypes 值
#    c_int * 10                   创建数组类型
#
# 3. 函数签名
#    func.argtypes = [c_int, c_char_p]
#    func.restype = c_int
#
# 4. 结构体
#    class MyStruct(Structure):
#        _fields_ = [("field", c_int), ...]
#
# 5. 指针
#    byref(obj)                   轻量指针（仅函数参数）
#    pointer(obj)                 持久指针对象
#    cast(ptr, POINTER(c_int))    强制转换
#
# 6. 回调
#    CB = CFUNCTYPE(restype, *argtypes)
#    @CB  def my_func(...): ...
#
# =============================================================================
# 【小练习】
# =============================================================================
#
# 练习 1（基础）：
#   写一个 ctypes 调用 C 库的 pow 函数，计算 2 的 10 次方。
#   提示：Windows 用 msvcrt.dll 的 pow，Linux 用 libm.so 的 pow。
#
# 练习 2（进阶）：
#   用 ctypes 定义一个结构体 Vector3（x, y, z 三个 c_float），
#   写函数 vec_length(v: Vector3) -> float 计算向量的长度。
#
# 练习提示：
#   练习1：libc.pow.argtypes=[c_double,c_double]; libc.pow.restype=c_double
#   练习2：from math import sqrt; 用 v.x/v.y/v.z 读取字段


# =============================================================================
# 【练习答案】
# =============================================================================


def call_c_pow(base: float, exp: float) -> float:
    """练习 1 答案：通过 ctypes 调用 C 的 pow 函数。"""
    if sys.platform == "win32":
        lib = ctypes.CDLL("msvcrt.dll")
    else:
        lib = ctypes.CDLL("libm.so.6")
    lib.pow.argtypes = [c_double, c_double]
    lib.pow.restype = c_double
    return lib.pow(base, exp)


class Vector3(Structure):
    """练习 2 答案：三维向量结构体。"""
    _fields_ = [
        ("x", c_float),
        ("y", c_float),
        ("z", c_float),
    ]


def vec_length(v: Vector3) -> float:
    """练习 2 答案：计算向量长度。"""
    import math
    return math.sqrt(v.x ** 2 + v.y ** 2 + v.z ** 2)


# 取消注释以运行练习：
# if __name__ == "__main__":
#     print("=" * 60)
#     result = call_c_pow(2.0, 10.0)
#     print(f"C pow(2.0, 10.0) = {result}")
#     print()
#     v = Vector3(3.0, 4.0, 0.0)
#     print(f"Vector3(3, 4, 0) length = {vec_length(v)}")
