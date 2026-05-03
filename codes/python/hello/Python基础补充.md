# Python 基础补充：包管理与常见坑

这份补充文档适合在完成 `01_基础篇` 后阅读，也可以在学习过程中遇到环境、依赖或语义问题时回来查阅。

---

## 一、包管理最小知识

Python 自带很多标准库，本教程大部分内容都只依赖标准库。但真实项目经常会安装第三方库，例如 HTTP 请求、数据处理、Web 开发、测试工具等。

### 1. 标准库、第三方库、本地模块

- 标准库：Python 安装后自带，例如 `os`、`sys`、`json`、`pathlib`、`sqlite3`。
- 第三方库：需要额外安装，例如 `requests`、`pandas`、`pytest`。
- 本地模块：你自己写的 `.py` 文件或包，例如本项目里的 `mymodule.py`、`mypackage/`。

```python
import json          # 标准库
import pathlib       # 标准库

# import requests    # 第三方库，需要先安装
# import mymodule    # 本地模块，取决于当前运行目录和导入路径
```

### 2. 虚拟环境为什么重要

虚拟环境用于隔离项目依赖。不同项目可能需要不同版本的库，如果都装到同一个全局 Python 环境里，版本很容易互相影响。

本项目使用 conda 环境：

```bat
conda env create -f environment.yml
conda activate py311
```

Python 内置的 `venv` 也是常见方案：

```bat
python -m venv .venv
.venv\Scripts\activate.bat
```

Linux/macOS 下激活方式通常是：

```bash
source .venv/bin/activate
```

### 3. pip 的基本用法

`pip` 是 Python 常用的第三方包安装工具。

```bat
pip install requests
pip install requests==2.31.0
pip list
pip show requests
pip uninstall requests
```

如果网络较慢，可以临时使用国内镜像：

```bat
pip install requests -i https://pypi.tuna.tsinghua.edu.cn/simple
```

### 4. requirements.txt 是什么

`requirements.txt` 用来记录项目依赖，方便别人或未来的自己复现环境。

示例：

```text
requests==2.31.0
pytest==8.2.0
```

安装依赖：

```bat
pip install -r requirements.txt
```

导出当前环境里的包：

```bat
pip freeze > requirements.txt
```

注意：`pip freeze` 会导出当前环境所有包，学习项目里可以用；正式项目中通常会更精细地维护运行依赖和开发依赖。

### 5. conda 与 pip 的关系

- conda 可以管理 Python 版本、虚拟环境和部分二进制依赖。
- pip 主要管理 Python 包。
- 使用 conda 环境时，也可以在激活环境后使用 pip。

建议：先激活目标环境，再安装包。

```bat
conda activate py311
pip install requests
```

不要在不确定当前环境的情况下随手执行 `pip install`。安装前可以先确认：

```bat
python --version
where python
pip --version
```

---

## 二、Python 常见坑

这些问题不是语法难点，但很容易让初学者写出“能运行但结果不对”的代码。

### 1. 可变默认参数

默认参数只会在函数定义时创建一次。不要把 list、dict、set 直接作为默认参数。

错误示例：

```python
def add_item(item, items=[]):
    items.append(item)
    return items


print(add_item("a"))  # ['a']
print(add_item("b"))  # ['a', 'b']，不是很多人以为的 ['b']
```

推荐写法：

```python
def add_item(item, items=None):
    if items is None:
        items = []
    items.append(item)
    return items
```

### 2. 浮点数精度

二进制浮点数不能精确表示所有十进制小数。

```python
print(0.1 + 0.2)  # 0.30000000000000004
```

比较浮点数时，不要直接依赖完全相等：

```python
import math

print(math.isclose(0.1 + 0.2, 0.3))
```

处理金额时优先考虑 `decimal.Decimal`：

```python
from decimal import Decimal

total = Decimal("0.1") + Decimal("0.2")
print(total)  # 0.3
```

### 3. `is` 与 `==`

- `==` 比较值是否相等。
- `is` 比较是否是同一个对象。

```python
a = [1, 2, 3]
b = [1, 2, 3]

print(a == b)  # True
print(a is b)  # False
```

常见规则：

```python
if value is None:
    print("没有值")
```

判断是否为 `None` 时用 `is None`；判断普通值是否相等时用 `==`。

### 4. 浅拷贝与深拷贝

浅拷贝只复制外层容器，内部对象仍然共享。

```python
data = [[1, 2], [3, 4]]
copy_data = data.copy()

copy_data[0].append(99)
print(data)  # [[1, 2, 99], [3, 4]]
```

需要完全复制嵌套结构时使用 `copy.deepcopy`：

```python
import copy

data = [[1, 2], [3, 4]]
copy_data = copy.deepcopy(data)
copy_data[0].append(99)

print(data)       # [[1, 2], [3, 4]]
print(copy_data)  # [[1, 2, 99], [3, 4]]
```

### 5. 循环变量与闭包

闭包会引用变量本身，而不是立即保存当时的值。

容易出错的写法：

```python
funcs = []
for i in range(3):
    funcs.append(lambda: i)

print([f() for f in funcs])  # [2, 2, 2]
```

如果需要保存每轮循环的值，可以用默认参数固定：

```python
funcs = []
for i in range(3):
    funcs.append(lambda i=i: i)

print([f() for f in funcs])  # [0, 1, 2]
```

### 6. Windows 路径字符串

Windows 路径里的反斜杠可能被当成转义字符。

容易出错：

```python
path = "C:\new\test.txt"
print(path)  # \n 会被当成换行
```

推荐使用原始字符串或 `pathlib`：

```python
from pathlib import Path

path1 = r"C:\new\test.txt"
path2 = Path("C:/new/test.txt")
```

项目代码中更推荐 `pathlib.Path`，可读性和跨平台表现更好。

### 7. 赋值不是拷贝

赋值只是让新变量名指向同一个对象。

```python
a = [1, 2, 3]
b = a
b.append(4)

print(a)  # [1, 2, 3, 4]
```

如果需要新列表：

```python
a = [1, 2, 3]
b = a.copy()
b.append(4)

print(a)  # [1, 2, 3]
print(b)  # [1, 2, 3, 4]
```

### 8. 字符串拼接：join() 比 + 快得多

循环中用 `+` 拼接字符串会产生大量临时对象，时间复杂度 O(n²)。

```python
# ❌ O(n²)，循环 10000 次就明显变慢
result = ""
for word in word_list:
    result += word

# ✅ O(n)，一次性 join
result = "".join(word_list)
```

`join()` 会先计算总长度，一次性分配内存，效率高得多。

### 9. finally 的执行顺序

`finally` 块在 `return` 之后、函数真正返回之前执行，但 return 的值已经确定了。

```python
def demo():
    try:
        return "try"
    finally:
        print("finally 执行了")


print(demo())
# 输出:
#   finally 执行了
#   try
# （finally 先打印，return 后生效——但返回值已经是 "try"）
```

如果 finally 里也有 return，会覆盖 try 的 return（不推荐这样做）。

### 10. try-except 不要滥用

只捕获你能处理的异常，不要写成空 except 或 except Exception 兜底。

```python
# ❌ 吞掉所有异常（包括 KeyboardInterrupt）
try:
    do_something()
except:
    pass

# ❌ 太宽泛
try:
    do_something()
except Exception:
    pass

# ✅ 只捕获你能处理的
try:
    num = int(user_input)
except ValueError:
    print("请输入有效数字")
```

### 11. 生成器用完就没了

生成器只能消费一次，第二次迭代什么也不会产生。

```python
gen = (x * 2 for x in range(3))
print(list(gen))  # [0, 2, 4]
print(list(gen))  # []（已经耗尽了）
```

需要多次使用时，先转成 list 或用 `itertools.tee`。

### 12. 可变对象作为函数参数

调用时传入可变对象作为参数，函数内部修改会影响外部。

```python
def add_user(name, users=None):
    if users is None:
        users = []
    users.append(name)
    return users


my_list = ["alice"]
add_user("bob", my_list)
print(my_list)  # ['alice', 'bob']（外部也被修改了）
```

这不是 bug，但很多人会意外。如果需要隔离，函数内先 copy。

---

## 三、学习建议

- 基础阶段先理解语义，不急着背所有命令。
- 遇到依赖问题，先确认当前 Python 解释器和虚拟环境。
- 遇到结果异常，优先检查：可变对象、浅拷贝、浮点数、`is`/`==`、路径字符串。
- 写项目时优先使用 `pathlib`、`with`、参数化 SQL、清晰的函数边界。
- 循环拼接字符串永远用 `join()`，不要用 `+`。
- 异常捕获要具体，不要写空的 except 块。
