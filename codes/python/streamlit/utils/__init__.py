"""
utils 包初始化文件

这个文件的存在使得 utils 目录成为一个 Python 包（package）。
Python 包是一种组织代码的方式，通过包可以将相关的模块组织在一起。

使用方式：
    from utils.data_helper import get_sample_dataframe
    from utils import data_helper
"""

# 可以在这里导入常用的工具函数，方便外部直接从 utils 导入
# 例如：from utils.data_helper import get_sample_dataframe, get_time_series_data
from utils.data_helper import (
    get_sample_dataframe,
    get_time_series_data,
    get_bar_chart_data,
    get_map_data,
    get_scatter_data,
)
