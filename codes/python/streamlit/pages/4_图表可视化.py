"""
模块4：图表可视化
本页面展示 Streamlit 中各种图表类型的使用方法，
包括内置折线图、面积图、柱状图、散点图、地图和外部库图表。

本模块帮助你快速掌握如何将数据可视化到页面。
"""

import streamlit as st
import pandas as pd
import numpy as np
import altair as alt
from utils.data_helper import get_time_series_data, get_bar_chart_data, get_map_data, get_scatter_data

st.set_page_config(
    page_title="图表可视化 - Streamlit 学习",
    page_icon="📈",
    layout="wide"
)

st.title("📈 模块4：图表可视化")

st.markdown(
    """
### 简介
数据可视化是数据分析和展示的重要环节。Streamlit 提供了内置的简单图表展示方法，
并支持 Matplotlib、Plotly、Altair 等多种主流绘图库，满足不同需求。

下面分别演示各类图表的用法和效果。
"""
)

st.divider()

# 内置简单图表 — 折线图、面积图、柱状图、散点图
st.header("内置简单图表")

ts_data = get_time_series_data(90, 3)
st.subheader("st.line_chart() - 折线图")
st.line_chart(ts_data)

st.subheader("st.area_chart() - 面积图")
st.area_chart(ts_data)

st.subheader("st.bar_chart() - 柱状图")
bar_data = get_bar_chart_data()
st.bar_chart(bar_data["数量"])

st.subheader("st.scatter_chart() - 散点图")
scatter_data = get_scatter_data()
scatter_data_simple = scatter_data[["x", "y"]]
st.scatter_chart(scatter_data_simple)

st.divider()

# 地图展示
st.header("地图展示")

map_data = get_map_data()
st.map(map_data)

st.markdown(
    """
- 只需要 DataFrame 中有 `lat` 和 `lon` 两列表示坐标
- 支持显示点云，适合地理数据展示
"""
)

st.divider()

# Altair 图表示例
st.header("Altair 图表示例")

ts_data_reset = ts_data.reset_index().melt(id_vars="index", var_name="产品", value_name="销量")
ts_data_reset.rename(columns={"index":"日期"}, inplace=True)

chart = (
    alt.Chart(ts_data_reset)
    .mark_line(point=True)
    .encode(
        x=alt.X("日期:T", title="日期"),
        y=alt.Y("销量:Q", title="销量"),
        color="产品:N"
    )
    .properties(
        title="Altair 折线图示例",
        width=700,
        height=400
    )
)
st.altair_chart(chart, use_container_width=True)

st.divider()

st.markdown(
    """
---
**提示：**

- Streamlit 内置图表适合快速展示数据，功能简单
- Matplotlib 灵活强大，但代码较复杂
- Plotly 提供交互和动画
- Altair 简洁声明式，适合统计图表
"""
)
