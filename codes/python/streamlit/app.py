"""
主入口 app.py — Streamlit 全功能学习应用 · 主页

【Streamlit 多页应用工作原理】
- 将此文件（app.py）放在项目根目录，它就是应用的"主页"
- pages/ 目录中的每个 .py 文件会自动成为一个独立页面
- Streamlit 会在左侧侧边栏自动生成页面导航菜单
- 文件名格式：数字_页面名.py，数字决定排列顺序

【运行方式】
    conda activate <你的环境>
    streamlit run app.py
"""

import streamlit as st

# ─── 页面基本配置 ────────────────────────────────────────────────────────────
# 此函数必须是脚本中第一个调用的 Streamlit 函数
# page_title：浏览器标签页标题
# page_icon：浏览器标签页图标（可用 emoji 或图片路径）
# layout：页面布局，"centered" 居中（默认），"wide" 使用全宽
# initial_sidebar_state：侧边栏初始状态，"auto" / "expanded" / "collapsed"
st.set_page_config(
    page_title="Streamlit 全功能学习",
    page_icon="📚",
    layout="wide",
    initial_sidebar_state="expanded",
)

# ─── 主页面内容 ───────────────────────────────────────────────────────────────
st.title("📚 Streamlit 全功能控件学习示例")

st.markdown(
    """
> 本项目是专为 Streamlit 初学者设计的**交互式学习应用**，
> 涵盖几乎所有常用控件和功能，并配有详细代码示例和说明。

---

## 🗺️ 如何使用本应用？

1. **点击左侧侧边栏**的页面链接，进入对应功能模块
2. 每个模块页面都包含：
   - 📖 功能说明和参数介绍
   - 💻 可直接复制的代码示例
   - 🎮 可交互的实际演示效果
3. 边看边练，是学习 Streamlit 的最佳方式！

---
"""
)

# ─── 模块导航卡片（三列布局） ─────────────────────────────────────────────────
st.subheader("📋 学习模块概览")
st.markdown("点击左侧导航进入对应模块 👈")

col1, col2, col3 = st.columns(3)

with col1:
    with st.container(border=True):
        st.markdown("### 📝 模块1：文本与排版")
        st.markdown(
            """
- `st.title / header / subheader`
- `st.text / markdown / write`
- `st.code / latex`
- Markdown 格式支持
- `st.metric` 数值卡片
"""
        )

    with st.container(border=True):
        st.markdown("### 📊 模块2：数据展示")
        st.markdown(
            """
- `st.dataframe` 交互表格
- `st.table` 静态表格
- `st.json` JSON 展示
- `st.metric` 指标卡片
- 数据编辑器
"""
        )

    with st.container(border=True):
        st.markdown("### 🎛️ 模块3：输入控件")
        st.markdown(
            """
- 文本输入 / 数字 / 密码
- 滑块 / 选择框 / 多选
- 日期 / 时间选择器
- 颜色选择器
- 按钮 / 复选框 / 开关
"""
        )

with col2:
    with st.container(border=True):
        st.markdown("### 📈 模块4：图表可视化")
        st.markdown(
            """
- `st.line_chart / area_chart`
- `st.bar_chart / scatter_chart`
- `st.map` 地图展示
- Matplotlib 集成
- Plotly / Altair 图表
"""
        )

    with st.container(border=True):
        st.markdown("### 🎨 模块5：布局与容器")
        st.markdown(
            """
- `st.columns` 多列布局
- `st.tabs` 标签页
- `st.expander` 折叠区域
- `st.container` 容器
- `st.empty` 占位符
- `st.sidebar` 侧边栏
"""
        )

    with st.container(border=True):
        st.markdown("### 🔄 模块6：状态管理")
        st.markdown(
            """
- `st.session_state` 会话状态
- `@st.cache_data` 数据缓存
- `@st.cache_resource` 资源缓存
- `on_change / on_click` 回调
- 缓存清除
"""
        )

with col3:
    with st.container(border=True):
        st.markdown("### 🖼️ 模块7：媒体与文件")
        st.markdown(
            """
- `st.image` 图片展示
- `st.audio` 音频播放
- `st.video` 视频播放
- `st.file_uploader` 文件上传
- `st.download_button` 下载
"""
        )

    with st.container(border=True):
        st.markdown("### ⚡ 模块8：进度与提示")
        st.markdown(
            """
- `st.success / info / warning / error`
- `st.exception` 异常展示
- `st.progress` 进度条
- `st.spinner` 加载动画
- `st.status` 状态区块
- `st.toast / balloons / snow`
"""
        )

    with st.container(border=True):
        st.markdown("### 🧩 模块9：表单与高级")
        st.markdown(
            """
- `st.form` 表单容器
- `st.dialog` 对话框
- `st.fragment` 局部刷新
- `st.html` 嵌入HTML
- `st.components` 组件扩展
"""
        )

st.divider()

# ─── 项目结构说明 ──────────────────────────────────────────────────────────────
st.subheader("📁 项目目录结构")

st.code(
    """
streamlit/
│
├── app.py                   ← 主入口（你现在在这里）
│
├── pages/                   ← 所有功能模块页面（自动加入导航）
│   ├── 1_文本与排版.py
│   ├── 2_数据展示.py
│   ├── 3_输入控件.py
│   ├── 4_图表可视化.py
│   ├── 5_布局与容器.py
│   ├── 6_状态管理.py
│   ├── 7_媒体与文件.py
│   ├── 8_进度与提示.py
│   └── 9_表单与高级.py
│
├── utils/                   ← 公共工具模块
│   ├── __init__.py          ← 标识这是一个 Python 包
│   └── data_helper.py       ← 数据生成辅助函数
│
└── .streamlit/              ← Streamlit 配置目录
    └── config.toml          ← 主题、服务器等配置
""",
    language="text",
)

st.divider()

# ─── 快速开始提示 ──────────────────────────────────────────────────────────────
st.subheader("🚀 快速上手建议")

st.markdown(
    """
**推荐学习顺序：**

1. **文本与排版** → 了解如何在页面显示各种文本内容
2. **输入控件** → 掌握如何接收用户输入
3. **布局与容器** → 学会组织页面结构
4. **状态管理** → 理解 Streamlit 的运行机制（重要！）
5. **图表可视化** → 展示数据和图表
6. **数据展示** → 表格和 JSON 展示技巧
7. **媒体与文件** → 多媒体和文件处理
8. **进度与提示** → 提升用户体验
9. **表单与高级** → 高级交互和扩展功能

**💡 小提示：** 每个模块页面都有可以直接运行的代码示例，
试着修改参数观察效果，是最快的学习方式！
"""
)

# ─── 侧边栏信息 ───────────────────────────────────────────────────────────────
with st.sidebar:
    st.markdown("### 📚 Streamlit 学习导航")
    st.info(
        """
**当前版本信息**
- Python: 3.11.x
- Streamlit: 1.55.0

点击上方菜单中的模块页面开始学习 👆
"""
    )
    st.markdown("---")
    st.markdown(
        """
**有用链接：**
- [Streamlit 官网](https://streamlit.io)
- [官方文档](https://docs.streamlit.io)
- [API 参考](https://docs.streamlit.io/develop/api-reference)
- [社区论坛](https://discuss.streamlit.io)
"""
    )
