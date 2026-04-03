"""
模块3：输入控件
本页面展示 Streamlit 提供的所有用户输入交互控件，
包括按钮、复选框、单选、滑块、下拉、文本框、日期等。

每种控件都附有用法说明和代码示例。
"""

import streamlit as st
from datetime import date, time

st.set_page_config(
    page_title="输入控件 - Streamlit 学习",
    page_icon="🎛️",
    layout="wide"
)

st.title("🎛️ 模块3：输入控件")

st.markdown(
    """
### 简介
输入控件是让用户与应用交互的关键。Streamlit 提供了丰富的输入控件，
几乎涵盖了所有常见的交互方式。

> **⚠️ 重要原理：**
> 每当用户与控件交互时，Streamlit 会 **重新执行整个页面脚本**。
> 控件的当前值作为返回值，可以赋给变量直接使用。
"""
)

st.divider()

# ===================================================
# 第1区：按钮类控件
# ===================================================
st.header("1️⃣ 按钮类控件")

col1, col2 = st.columns(2)

with col1:
    st.subheader("st.button() - 普通按钮")
    st.code("""
if st.button("点击我"):
    st.write("按钮被点击了！")
    """, language="python")

    # 实际效果
    if st.button("点击我"):
        st.write("🎉 按钮被点击了！")

    st.markdown(
        """
- 返回值：点击返回 `True`，否则 `False`
- 每次点击后整个脚本重新运行
- 可以用 `type="primary"` 显示高亮按钮
"""
    )

with col2:
    st.subheader("st.download_button() - 下载按钮")
    st.code("""
content = "这是要下载的内容"
st.download_button(
    label="下载文件",
    data=content,
    file_name="example.txt",
    mime="text/plain"
)
    """, language="python")

    content = "这是一段要下载的示例文本内容"
    st.download_button(
        label="📥 下载示例文件",
        data=content,
        file_name="example.txt",
        mime="text/plain"
    )

    st.markdown(
        """
- 点击后触发浏览器下载
- `data` 支持字符串、字节或文件类对象
- `mime` 用于指定文件类型（如 `text/csv`, `application/json`）
"""
    )

st.markdown("---")

# link_button 和 primary 按钮
col3, col4 = st.columns(2)

with col3:
    st.subheader("st.link_button() - 链接按钮")
    st.code("""
st.link_button("访问官网", "https://streamlit.io")
    """, language="python")

    st.link_button("🔗 访问 Streamlit 官网", "https://streamlit.io")

    st.markdown("点击后在新标签页打开链接，不会重新运行脚本。")

with col4:
    st.subheader("primary 按钮 - 高亮主要操作")
    st.code("""
st.button("主要操作", type="primary")
st.button("普通操作", type="secondary")
    """, language="python")

    st.button("⭐ 主要操作按钮", type="primary")
    st.button("普通操作按钮", type="secondary")

st.divider()

# ===================================================
# 第2区：开关与勾选类
# ===================================================
st.header("2️⃣ 开关与勾选类控件")

col1, col2 = st.columns(2)

with col1:
    st.subheader("st.checkbox() - 复选框")
    st.code("""
agree = st.checkbox("我同意用户协议")
if agree:
    st.write("谢谢！")
    """, language="python")

    agree = st.checkbox("我同意用户协议")
    if agree:
        st.success("✅ 谢谢您的同意！")

    st.markdown(
        """
- 返回 `True` 或 `False`
- 默认不勾选，可以用 `value=True` 设置默认勾选
"""
    )

with col2:
    st.subheader("st.toggle() - 开关切换")
    st.code("""
dark_mode = st.toggle("开启深色模式")
if dark_mode:
    st.write("深色模式已开启！")
    """, language="python")

    dark_mode = st.toggle("开启深色模式")
    if dark_mode:
        st.info("🌙 深色模式已开启！")
    else:
        st.info("☀️ 浅色模式中")

    st.markdown(
        """
- 和 checkbox 类似，但展示为开关形式
- 返回 `True` 或 `False`
"""
    )

st.divider()

# ===================================================
# 第3区：选择类控件
# ===================================================
st.header("3️⃣ 选择类控件")

col1, col2 = st.columns(2)

with col1:
    st.subheader("st.radio() - 单选按钮")
    st.code("""
choice = st.radio(
    "请选择一个选项：",
    ["选项A", "选项B", "选项C"],
    index=0           # 默认选中第0个
)
st.write(f"你选择了：{choice}")
    """, language="python")

    choice = st.radio(
        "请选择一个选项：",
        ["选项A", "选项B", "选项C"],
        index=0
    )
    st.write(f"你选择了：`{choice}`")

with col2:
    st.subheader("st.selectbox() - 下拉框")
    st.code("""
city = st.selectbox(
    "请选择城市：",
    ["北京", "上海", "广州", "深圳"]
)
st.write(f"你选择了：{city}")
    """, language="python")

    city = st.selectbox(
        "请选择城市：",
        ["北京", "上海", "广州", "深圳"]
    )
    st.write(f"你选择了：`{city}`")

st.markdown("---")

col3, col4 = st.columns(2)

with col3:
    st.subheader("st.multiselect() - 多选框")
    st.code("""
tags = st.multiselect(
    "请选择你感兴趣的技术：",
    ["Python", "JavaScript", "Rust", "Go", "Java"],
    default=["Python"]     # 默认选中的选项
)
st.write(f"你选了：{tags}")
    """, language="python")

    tags = st.multiselect(
        "请选择你感兴趣的技术：",
        ["Python", "JavaScript", "Rust", "Go", "Java"],
        default=["Python"]
    )
    st.write(f"你选了：`{tags}`")

    st.markdown(
        """
- 返回一个列表
- 可以设置 `max_selections` 限制最多选择数量
"""
    )

with col4:
    st.subheader("st.select_slider() - 可选值滑块")
    st.code("""
level = st.select_slider(
    "选择你的技能等级：",
    options=["初级", "中级", "高级", "专家"],
    value="中级"
)
    """, language="python")

    level = st.select_slider(
        "选择你的技能等级：",
        options=["初级", "中级", "高级", "专家"],
        value="中级"
    )
    st.write(f"你的等级：`{level}`")

st.markdown("---")

st.subheader("st.pills() - 药丸选项 (Streamlit 1.45+)")
st.code("""
# pills 提供更现代的单选/多选风格
color = st.pills(
    "选择颜色",
    options=["🔴 红色", "🟢 绿色", "🔵 蓝色"],
)
    """, language="python")

try:
    color = st.pills(
        "选择颜色",
        options=["🔴 红色", "🟢 绿色", "🔵 蓝色"],
    )
    if color:
        st.write(f"你选了：`{color}`")
except AttributeError:
    st.warning("当前 Streamlit 版本不支持 st.pills()，需要 1.45+ 版本")

st.divider()

# ===================================================
# 第4区：滑块控件
# ===================================================
st.header("4️⃣ 滑块控件")

col1, col2 = st.columns(2)

with col1:
    st.subheader("st.slider() - 整数/浮点滑块")
    st.code("""
age = st.slider("选择年龄", min_value=0, max_value=100, value=25, step=1)
st.write(f"你选择的年龄：{age}")
    """, language="python")

    age = st.slider("选择年龄", min_value=0, max_value=100, value=25, step=1)
    st.write(f"你选择的年龄：`{age}`")

    st.markdown("---")

    st.subheader("范围滑块 - 使用 tuple 设置初始值")
    st.code("""
price_range = st.slider(
    "价格区间 (元)",
    min_value=0, max_value=10000,
    value=(1000, 5000)  # 元组表示范围
)
st.write(f"选择了 {price_range[0]}~{price_range[1]} 元")
    """, language="python")

    price_range = st.slider(
        "价格区间 (元)",
        min_value=0, max_value=10000,
        value=(1000, 5000)
    )
    st.write(f"选择了 `{price_range[0]}`~`{price_range[1]}` 元")

with col2:
    st.subheader("日期范围滑块")
    st.code("""
import datetime
date_range = st.slider(
    "选择日期范围",
    value=(date(2025, 1, 1), date(2025, 12, 31))
)
    """, language="python")

    import datetime
    date_range = st.slider(
        "选择日期范围",
        value=(date(2025, 1, 1), date(2025, 12, 31))
    )
    st.write(f"你选择了：`{date_range[0]}` 到 `{date_range[1]}`")

st.divider()

# ===================================================
# 第5区：文字和数字输入
# ===================================================
st.header("5️⃣ 文字与数字输入")

col1, col2 = st.columns(2)

with col1:
    st.subheader("st.text_input() - 单行文字输入")
    st.code("""
name = st.text_input(
    "请输入你的名字：",
    placeholder="在这里输入...",
    max_chars=50
)
    """, language="python")

    name = st.text_input(
        "请输入你的名字：",
        placeholder="在这里输入...",
        max_chars=50
    )
    if name:
        st.write(f"你好，`{name}`！")

    st.markdown("---")

    st.subheader("st.text_area() - 多行文字输入")
    st.code("""
feedback = st.text_area(
    "请填写反馈：",
    placeholder="多行文字输入...",
    height=150
)
    """, language="python")

    feedback = st.text_area(
        "请填写反馈：",
        placeholder="多行文字输入...",
        height=150
    )
    if feedback:
        st.write(f"你输入了 {len(feedback)} 个字符。")

with col2:
    st.subheader("st.number_input() - 数字输入")
    st.code("""
num = st.number_input(
    "输入数字：",
    min_value=0.0,
    max_value=100.0,
    value=50.0,
    step=0.5
)
st.write(f"当前值：{num}")
    """, language="python")

    num = st.number_input(
        "输入数字：",
        min_value=0.0,
        max_value=100.0,
        value=50.0,
        step=0.5
    )
    st.write(f"当前值：`{num}`")

    st.markdown(
        """
- `min_value`, `max_value` 限制范围
- `step` 控制每次增减的步长
- 类型根据 `value` 和 `step` 自动推断（int 或 float）
"""
    )

st.divider()

# ===================================================
# 第6区：日期与时间
# ===================================================
st.header("6️⃣ 日期与时间")

col1, col2 = st.columns(2)

with col1:
    st.subheader("st.date_input() - 日期选择器")
    st.code("""
selected_date = st.date_input(
    "选择日期",
    value=date.today()
)
st.write(f"你选了：{selected_date}")
    """, language="python")

    selected_date = st.date_input("选择日期", value=date.today())
    st.write(f"你选了：`{selected_date}`")

with col2:
    st.subheader("st.time_input() - 时间选择器")
    st.code("""
selected_time = st.time_input(
    "选择时间",
    value=time(9, 30)    # 默认 09:30
)
st.write(f"你选了：{selected_time}")
    """, language="python")

    selected_time = st.time_input("选择时间", value=time(9, 30))
    st.write(f"你选了：`{selected_time}`")

st.divider()

# ===================================================
# 第7区：颜色选择
# ===================================================
st.header("7️⃣ 颜色选择器")

st.subheader("st.color_picker() - 颜色选择")
st.code("""
color = st.color_picker("选择一个颜色", value="#FF4B4B")
st.write(f"你选择了颜色：{color}")
# 展示颜色预览
st.markdown(f'<div style="width:100px; height:50px; background:{color}; border-radius:8px;"></div>', unsafe_allow_html=True)
    """, language="python")

picked_color = st.color_picker("选择一个颜色", value="#FF4B4B")
st.write(f"你选择了颜色：`{picked_color}`")
st.markdown(
    f'<div style="width:100px; height:50px; background:{picked_color}; border-radius:8px; border:1px solid #ddd;"></div>',
    unsafe_allow_html=True
)

st.divider()

# ===================================================
# 第8区：文件上传
# ===================================================
st.header("8️⃣ 文件上传")

st.subheader("st.file_uploader() - 文件上传控件")
st.code("""
uploaded_file = st.file_uploader(
    "上传一个 CSV 文件",
    type=["csv", "xlsx"],
    accept_multiple_files=False   # 是否支持多文件
)
if uploaded_file is not None:
    # 读取并展示文件内容
    import pandas as pd
    df = pd.read_csv(uploaded_file)
    st.dataframe(df)
    """, language="python")

uploaded_file = st.file_uploader(
    "上传一个 CSV 文件（试试看！）",
    type=["csv"],
    accept_multiple_files=False
)
if uploaded_file is not None:
    import pandas as pd
    try:
        df_upload = pd.read_csv(uploaded_file)
        st.success(f"✅ 成功读取，共 {len(df_upload)} 行数据")
        st.dataframe(df_upload.head(20))
    except Exception as e:
        st.error(f"读取失败：{e}")

st.divider()

# ===================================================
# 第9区：用户反馈（新特性）
# ===================================================
st.header("9️⃣ 用户反馈控件（Streamlit 1.40+）")

st.subheader("st.feedback() - 反馈评价控件")
st.code("""
# thumbs: 大拇指评分  faces: 表情评分  stars: 星级评分
feedback = st.feedback("thumbs")
if feedback == 1:
    st.write("很高兴你喜欢！")
    """, language="python")

try:
    sentiment = st.feedback("thumbs")
    if sentiment == 1:
        st.success("😊 感谢你的好评！")
    elif sentiment == 0:
        st.warning("😔 谢谢反馈，我们会继续改进！")

    st.markdown("---")
    st.caption("stars 模式：")
    star_rating = st.feedback("stars")
    if star_rating is not None:
        st.write(f"你给了 {star_rating + 1} 颗星！")
except AttributeError:
    st.warning("当前 Streamlit 版本不支持 st.feedback()，需要 1.40+ 版本")

st.divider()

st.success("🎉 本页面涵盖了 Streamlit 所有主要的输入控件！你可以随意操作，观察它们的返回值。")
