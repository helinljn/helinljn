"""
mypackage - 示例 Python 包

这是一个演示包结构的示例包。
__init__.py 会在包被导入时自动执行。

用途：
    1. 标识该目录为 Python 包
    2. 包的初始化代码
    3. 控制 from mypackage import * 的行为（通过 __all__）
"""

# 包的版本
__version__ = "1.0.0"

# __all__ 控制 from mypackage import * 时导出哪些名称
__all__ = ["module1", "module2"]

# 包级别的初始化代码
print(f"[mypackage] 包已加载，版本 {__version__}")

# 可以在包级别暴露常用的函数/类，方便用户直接使用
# 例如：from mypackage import greet 而不是 from mypackage.module1 import greet
from .module1 import greet
from .module2 import Calculator
