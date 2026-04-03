"""
模块2：数据展示
本页面展示 Streamlit 中用于表格和数据可视化的控件，
包括 dataframe、table、metric、json 等。

本模块适合学习如何优雅展示结构化数据和指标。
"""

import streamlit as st
import pandas as pd
import numpy as np
from utils.data_helper import get_sample_dataframe

st.set_page_config(
    page_title="数据展示 - Streamlit 学习",
    page_icon="📊",
    layout="wide"
)

st.title("📊 模块2：数据展示")

st.markdown(
    """
### 简介
数据展示是数据驱动型应用的关键。Streamlit 提供多种数据展示组件，
既有丰富交互的 DataFrame，也有静态表格、指标卡、JSON 展示等。

下面演示这些组件的用法和效果。
"""
)

st.divider()

# 1. 显示交互式 DataFrame
st.header("st.dataframe() - 交互式数据框")

df = get_sample_dataframe(30)
st.markdown(
    """
- 默认支持排序、过滤、调整列宽
- 可以使用 `column_config` 参数自定义列标题、格式化、图标等 (Streamlit 1.55+ 特性)
"""
)

# 示例：简单 dataframe
st.dataframe(df)

# 示例：带 column_config
st.markdown("自定义列格式和对齐示例：")
col_config = {
    "产品名称": st.column_config.TextColumn("产品名称（自定义标题）", help="这是产品名称"),
    "价格": st.column_config.NumberColumn("价格 (¥)", format="¤#,##0.00", precision=2),
    "销量": st.column_config.NumberColumn("销量", format="%,d"),
    "上架日期": st.column_config.DateColumn("上架日期"),
    "是否在售": st.column_config.BooleanColumn("在售状态"),
}
st.dataframe(df, column_config=col_config)

st.divider()

# 2. 静态表格 Table
st.header("st.table() - 静态数据表格")

st.markdown(
    """
- 用于展示静态、不支持交互的数据表
- 不会滚动或分页
- 适合简单展示
"""
)

st.table(df.head(10))

st.divider()

# 3. 指标 Metric
st.header("st.metric() - 关键指标数字展示")

st.markdown(
    """
- 适合展示关键数字和变化趋势
- 支持显示当前值和增减幅度
"""
)

st.metric(label="当天访客数", value=1234, delta=45)
st.metric(label="转化率", value="12.5%", delta="-1.2%")

st.divider()

# 4. JSON 格式化显示
st.header("st.json() - JSON 数据展示")

st.markdown(
    """
- 用于展示 JSON 格式数据
- 自动格式化和折叠
"""
)

sample_json = {
    "name": "Streamlit 教程",
    "modules": ["文本与排版", "数据展示", "输入控件", "图表可视化"],
    "version": "1.0",
    "active": True,
    "rates": {"like": 1000, "dislike": 30},
}

st.json(sample_json)

st.divider()

# 5. write 也支持展现 Pandas、NumPy、列表等多种格式
st.header("st.write() 展示 DataFrame 和列表示例")

st.write("展示 DataFrame:")
st.write(df.head(5))

st.write("展示列表数组:")
st.write(["Alpha", "Beta", "Gamma"])

st.divider()

st.markdown(
    """
---
**提示：**
在展示大量数据时，`st.dataframe` 提供了更好的交互体验，而 `st.table` 用于简洁静态展示。
`st.metric` 适合在仪表盘中显示关键数字变化。
`st.json` 用于调试或展示结构化的 JSON 数据。
"""
)
