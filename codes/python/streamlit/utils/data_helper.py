"""
utils/data_helper.py — 数据生成辅助函数模块

这个模块提供了各种示例数据生成函数，供教程各页面使用。

【目录结构说明】
在实际的 Web 项目中，utils（utilities 的缩写）目录通常存放：
- 数据处理函数
- 格式化辅助函数
- 常量定义
- 配置读取函数
- API 调用封装
等"横切关注点"——即多个模块都可能用到的通用功能。

【为什么要单独建 utils 目录？】
而不是直接写在每个页面文件里？
1. 避免代码重复：多个页面需要同样数据，写一处维护
2. 代码更清晰，责任单一
3. 测试方便，能单独测试 utils 函数
4. 有利于扩展和升级

"""

import pandas as pd
import numpy as np
from datetime import datetime, timedelta


def get_sample_dataframe(n_rows: int = 20) -> pd.DataFrame:
    """
    生成一个示例 DataFrame，用于数据展示示例

    参数:
        n_rows: 数据行数

    返回:
        包含多种数据类型列的 DataFrame
    """
    np.random.seed(42)  # 固定随机种子，保证每次生成的数据一致

    categories = ["电子", "服装", "食品", "家居", "运动"]

    df = pd.DataFrame({
        "产品名称": [f"产品_{i:03d}" for i in range(n_rows)],
        "类别": np.random.choice(categories, n_rows),
        "价格": np.round(np.random.uniform(10, 1000, n_rows), 2),
        "销量": np.random.randint(1, 500, n_rows),
        "评分": np.round(np.random.uniform(3.0, 5.0, n_rows), 1),
        "是否在售": np.random.choice([True, False], n_rows),
        "上架日期": [
            datetime.now() - timedelta(days=np.random.randint(0, 365))
            for _ in range(n_rows)
        ],
    })

    df["销售额"] = (df["价格"] * df["销量"]).round(2)
    return df


def get_time_series_data(n_points: int = 90, n_series: int = 3) -> pd.DataFrame:
    """
    生成时间序列数据，用于折线图、面积图等

    参数:
        n_points: 数据点数量
        n_series: 时间序列数量

    返回:
        以日期为索引的 DataFrame
    """
    np.random.seed(42)

    dates = pd.date_range(
        start=datetime.now() - timedelta(days=n_points),
        periods=n_points,
        freq="D"
    )

    series_names = ["产品A", "产品B", "产品C", "产品D"][:n_series]

    data = {}
    for i, name in enumerate(series_names):
        # 生成带有随机游走的时间序列
        base = 100 + i * 30
        values = base + np.cumsum(np.random.randn(n_points) * 5)
        values = np.maximum(values, 10)  # 确保值为正数
        data[name] = np.round(values, 2)

    return pd.DataFrame(data, index=dates)


def get_bar_chart_data(n_categories: int = 8) -> pd.DataFrame:
    """
    生成柱状图数据

    返回:
        DataFrame，索引为类别，包含数量和目标两列
    """
    np.random.seed(42)
    categories = [f"类别{chr(65+i)}" for i in range(n_categories)]
    return pd.DataFrame({
        "类别": categories,
        "数量": np.random.randint(10, 100, n_categories),
        "目标": np.random.randint(50, 120, n_categories),
    }).set_index("类别")


def get_map_data(n_points: int = 30) -> pd.DataFrame:
    """
    生成地图数据，围绕北京市中心生成随机点

    参数:
        n_points: 地图点的数量

    返回:
        包含 lat（纬度）和 lon（经度）列的 DataFrame
    """
    np.random.seed(42)

    # 北京市中心坐标
    center_lat, center_lon = 39.9042, 116.4074

    return pd.DataFrame({
        "lat": center_lat + np.random.randn(n_points) * 0.1,
        "lon": center_lon + np.random.randn(n_points) * 0.1,
        "size": np.random.randint(10, 100, n_points),
    })


def get_scatter_data(n_points: int = 100) -> pd.DataFrame:
    """
    生成散点图数据，用于散点图演示

    返回:
        包含 x, y 坐标，大小和颜色类别的 DataFrame
    """
    np.random.seed(42)
    return pd.DataFrame({
        "x": np.random.randn(n_points),
        "y": np.random.randn(n_points),
        "大小": np.random.randint(10, 100, n_points),
        "颜色": np.random.choice(["A", "B", "C"], n_points),
    })
