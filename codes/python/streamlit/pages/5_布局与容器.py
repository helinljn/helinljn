"""
模块5：布局与容器
本页面展示 Streamlit 提供的所有页面布局和容器控件，
包括列布局、标签页、侧边栏、折叠区域、容器、弹出气泡等。

页面布局是构建美观、易用应用的关键。
"""

import streamlit as st
import time

st.set_page_config(
    page_title="布局与容器 - Streamlit 学习",
    page_icon="🎨",
    layout="wide"
)

st.title("🎨 模块5：布局与容器")

st.markdown(
    """
### 简介
布局控件帮助我们合理排列页面元素，创造出整洁、层次清晰的界面。
Streamlit 提供了多种布局方式，从简单的多列布局，到标签页、折叠区域等。

下面逐一演示每种布局控件的用法和效果。
"""
)

st.divider()

# ===================================================
# 1. 列布局 st.columns
# ===================================================
st.header("1️⃣ 列布局 — st.columns()")

st.markdown(
    """
`st.columns()` 将页面水平分割为多列，每一列可以独立放置控件。
- 传入整数，按等比分割：`st.columns(3)`
- 传入列表，按比例分割：`st.columns([1, 2, 1])`
"""
)

st.code(
    """
# 等比三列
col1, col2, col3 = st.columns(3)
with col1:
    st.metric("指标A", 100)
with col2:
    st.metric("指标B", 200)
with col3:
    st.metric("指标C", 300)
""",
    language="python",
)

# 实际效果：等比三列
col1, col2, col3 = st.columns(3)
with col1:
    st.metric("📦 订单数", 1024, delta=42)
with col2:
    st.metric("💰 销售额", "¥88,888", delta="¥1,200")
with col3:
    st.metric("⭐ 好评率", "98.5%", delta="0.5%")

st.markdown("---")

st.markdown("**按比例分割示例 [1, 2, 1]（中间列是边列的两倍宽）：**")
st.code(
    """
left, center, right = st.columns([1, 2, 1])
with left:
    st.info("左侧（比例1）")
with center:
    st.success("中间（比例2）")
with right:
    st.warning("右侧（比例1）")
""",
    language="python",
)

left, center, right = st.columns([1, 2, 1])
with left:
    st.info("左侧（比例1）")
with center:
    st.success("这是宽度是两侧两倍的中间列（比例2）")
with right:
    st.warning("右侧（比例1）")

st.markdown(
    """
> **提示：** `st.columns()` 不支持嵌套，但可以在列内创建其他容器。
> 列内的控件写法和主页面完全一样。
"""
)

st.divider()

# ===================================================
# 2. 标签页 st.tabs
# ===================================================
st.header("2️⃣ 标签页 — st.tabs()")

st.markdown(
    """
`st.tabs()` 可以创建多个标签页，一次只显示一个标签页的内容。
适合将相关内容组织在一起，节省页面空间。
"""
)

st.code(
    """
tab1, tab2, tab3 = st.tabs(["🐱 猫猫", "🐶 狗狗", "🐰 兔兔"])

with tab1:
    st.write("这是猫猫标签页")
with tab2:
    st.write("这是狗狗标签页")
with tab3:
    st.write("这是兔兔标签页")
""",
    language="python",
)

tab1, tab2, tab3 = st.tabs(["🐱 猫猫", "🐶 狗狗", "🐰 兔兔"])

with tab1:
    st.subheader("猫猫专区")
    st.write("猫猫可爱、灵动，深受很多人喜爱。")
    st.image("https://placecats.com/300/200", caption="一只可爱的猫", use_container_width=False)

with tab2:
    st.subheader("狗狗专区")
    st.write("狗狗忠诚、活泼，是人类最好的朋友。")
    st.info("🐶 狗狗信息展示在这里")

with tab3:
    st.subheader("兔兔专区")
    st.write("兔兔软萌，毛茸茸的，超级治愈！")
    st.warning("🐰 兔兔信息展示在这里")

st.divider()

# ===================================================
# 3. 折叠区域 st.expander
# ===================================================
st.header("3️⃣ 折叠区域 — st.expander()")

st.markdown(
    """
`st.expander()` 创建可折叠的内容区域，默认可以是折叠或展开状态。
适合放次要信息、详情、说明等，让页面更简洁。
"""
)

st.code(
    """
# expanded=True 默认展开，False 默认折叠
with st.expander("点击展开详细说明", expanded=False):
    st.write("这里是折叠区域内的内容")
    st.code("print('Hello, Streamlit!')")
""",
    language="python",
)

with st.expander("📋 点击展开详细说明", expanded=False):
    st.write("这里是折叠区域内的内容！")
    st.info("在 expander 内可以使用任何 Streamlit 控件。")
    st.code("print('Hello from expander!')", language="python")

with st.expander("📊 展开查看数据说明", expanded=True):
    st.markdown(
        """
**expander 的常见用途：**
- 隐藏不常用的高级设置
- 折叠代码示例
- 显示辅助说明和注释
- 数据详情展示
"""
    )

st.divider()

# ===================================================
# 4. 容器 st.container
# ===================================================
st.header("4️⃣ 容器 — st.container()")

st.markdown(
    """
`st.container()` 是一个通用容器，可以对内部控件分组管理。
虽然视觉上与直接写控件没有区别，但它让你可以 **控制内容出现的位置**。

例如：创建容器 → 在下面写代码 → 稍后回到容器追加内容。
"""
)

st.code(
    """
# 先创建容器（此时是空的）
placeholder_container = st.container()

# 在容器之后写一些内容
st.write("这是容器外的内容（在代码中写在后面）")

# 回到容器，追加内容（会出现在页面前面）
with placeholder_container:
    st.write("这是容器内的内容（在代码中写在后面，但显示在前面）")
""",
    language="python",
)

container_demo = st.container(border=True)
st.caption("⬆️ 上方是一个带边框的容器，内容写在下面这段代码里")

with container_demo:
    st.markdown("**📦 这是 container 内的内容**")
    st.write("容器里可以放任意控件，并可以设置 `border=True` 显示边框。")

st.divider()

# ===================================================
# 5. 空占位符 st.empty
# ===================================================
st.header("5️⃣ 空占位符 — st.empty()")

st.markdown(
    """
`st.empty()` 创建一个可动态更新的空占位符。
你可以随时往这个占位符写入新内容，**旧内容会被替换**，不会追加。
非常适合展示动态更新的内容，如倒计时、状态刷新等。
"""
)

st.code(
    """
placeholder = st.empty()

for i in range(5, 0, -1):
    placeholder.markdown(f"## ⏳ 倒计时：{i} 秒")
    time.sleep(1)

placeholder.success("✅ 倒计时完成！")
""",
    language="python",
)

if st.button("▶ 启动倒计时演示（5秒）"):
    countdown_placeholder = st.empty()
    for i in range(5, 0, -1):
        countdown_placeholder.markdown(f"## ⏳ 倒计时：**{i}** 秒")
        time.sleep(1)
    countdown_placeholder.success("✅ 倒计时完成！")

st.divider()

# ===================================================
# 6. 侧边栏 st.sidebar
# ===================================================
st.header("6️⃣ 侧边栏 — st.sidebar")

st.markdown(
    """
`st.sidebar` 让你在页面左侧的侧边栏中放置控件，
通常用于放置**全局设置**、**过滤条件**、**导航菜单**等。

两种写法等价：

```python
# 方式一：with 语法
with st.sidebar:
    st.write("侧边栏内容")

# 方式二：直接在控件上调用
st.sidebar.write("侧边栏内容")
```
"""
)

# 演示在侧边栏放置控件
with st.sidebar:
    st.markdown("---")
    st.markdown("### 🎨 布局演示控件")
    demo_theme = st.radio(
        "选择显示主题",
        ["默认", "信息蓝", "成功绿", "警告橙"],
        key="layout_demo_theme"
    )

# 根据侧边栏选择，展示对应内容
st.markdown("**根据侧边栏选择切换显示（试试左边的控件！）：**")
if "layout_demo_theme" in st.session_state:
    theme = st.session_state["layout_demo_theme"]
    if theme == "默认":
        st.write("当前主题：默认")
    elif theme == "信息蓝":
        st.info("当前主题：信息蓝 ℹ️")
    elif theme == "成功绿":
        st.success("当前主题：成功绿 ✅")
    elif theme == "警告橙":
        st.warning("当前主题：警告橙 ⚠️")

st.divider()

# ===================================================
# 7. 弹出气泡 st.popover
# ===================================================
st.header("7️⃣ 弹出气泡 — st.popover()（Streamlit 1.35+）")

st.markdown(
    """
`st.popover()` 创建一个点击后弹出的气泡面板，气泡内可以放置任意控件。
适合放置辅助设置、额外信息等不常用的内容，节省页面空间。
"""
)

st.code(
    """
with st.popover("打开气泡面板"):
    st.write("这是弹出气泡里的内容")
    option = st.selectbox("选择", ["A", "B", "C"])
""",
    language="python",
)

try:
    with st.popover("⚙️ 打开设置气泡"):
        st.write("这是弹出气泡里的内容！")
        pop_option = st.selectbox("显示密度", ["紧凑", "标准", "宽松"])
        pop_color = st.color_picker("选择主题色", value="#FF4B4B")
        st.write(f"当前选择：密度={pop_option}, 颜色={pop_color}")
except AttributeError:
    st.warning("当前 Streamlit 版本不支持 st.popover()，需要 1.35+ 版本")

st.divider()

st.success("🎉 本页面展示了 Streamlit 所有的布局与容器控件，合理利用这些工具可以让你的应用更美观整洁！")
