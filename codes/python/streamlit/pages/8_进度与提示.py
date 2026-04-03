"""
模块8：进度与提示
展示 Streamlit 提供的各种进度展示和用户提示控件，
包括进度条、旋转加载、状态消息、toast 通知和庆祝动画。
"""

import streamlit as st
import time

st.set_page_config(
    page_title="进度与提示 - Streamlit 学习",
    page_icon="⚡",
    layout="wide"
)

st.title("⚡ 模块8：进度与提示")

st.markdown(
    """
### 简介
良好的用户体验需要在应用执行耗时操作时给出及时反馈。
Streamlit 提供多种进度提示控件：进度条、加载动画、消息提示框、
Toast 通知和庆祝动画等，让用户始终了解当前状态。
"""
)

st.divider()

# ===================================================
# 1. 消息提示框
# ===================================================
st.header("1️⃣ 消息提示框")

st.markdown(
    """
4种内置消息类型：success（成功）、info（信息）、warning（警告）、error（错误）。
每种都有对应的图标和配色，方便用户识别。
"""
)

st.success("✅ st.success() —— 操作成功提示，绿色")
st.info("ℹ️ st.info() —— 普通信息提示，蓝色")
st.warning("⚠️ st.warning() —— 警告提示，橙色")
st.error("❌ st.error() —— 错误提示，红色")

st.code(
    """
st.success("操作成功！")
st.info("这是一条普通信息。")
st.warning("请注意检查输入！")
st.error("发生错误，请联系管理员。")
""",
    language="python",
)

st.divider()

# ===================================================
# 2. 异常展示 st.exception
# ===================================================
st.header("2️⃣ 异常信息展示 - st.exception()")

st.markdown("用于将 Python 异常信息以用户友好的方式显示在页面上。")

try:
    result = 1 / 0
except ZeroDivisionError as e:
    st.exception(e)

st.code(
    """
try:
    result = 1 / 0
except ZeroDivisionError as e:
    st.exception(e)   # 在页面显示异常堆栈
""",
    language="python",
)

st.divider()

# ===================================================
# 3. 进度条 st.progress
# ===================================================
st.header("3️⃣ 进度条 - st.progress()")

st.markdown(
    """
`st.progress()` 展示一个从 0 到 100 的进度条，
适用于文件处理、批量计算等可分步展示进度的场景。
"""
)

st.code(
    """
import time

progress = st.progress(0, text="正在处理...")
for i in range(100):
    progress.progress(i + 1, text=f"正在处理：{i+1}%")
    time.sleep(0.05)
progress.success("完成！")
""",
    language="python",
)

if st.button("▶ 启动进度条演示"):
    progress_bar = st.progress(0, text="正在处理...")
    for i in range(100):
        progress_bar.progress(i + 1, text=f"正在处理：{i + 1}%")
        time.sleep(0.03)
    progress_bar.empty()
    st.success("✅ 处理完成！")

st.divider()

# ===================================================
# 4. 旋转加载 st.spinner
# ===================================================
st.header("4️⃣ 旋转加载动画 - st.spinner()")

st.markdown(
    """
`st.spinner()` 在代码块执行期间显示旋转动画，执行完毕后自动消失。
适合包裹耗时的计算任务，给用户明确的等待反馈。
"""
)

st.code(
    """
with st.spinner("正在加载数据，请稍候..."):
    time.sleep(3)    # 模拟耗时操作

st.success("数据加载完成！")
""",
    language="python",
)

if st.button("▶ 触发加载动画（模拟 2 秒）"):
    with st.spinner("⏳ 正在执行耗时操作，请稍候..."):
        time.sleep(2)
    st.success("✅ 操作完成！")

st.divider()

# ===================================================
# 5. 状态区块 st.status
# ===================================================
st.header("5️⃣ 状态展示区块 - st.status()（Streamlit 1.28+）")

st.markdown(
    """
`st.status()` 是一个可折叠的状态展示区块，适合展示多步骤的任务进度。
完成后可以变为成功或错误状态。
"""
)

st.code(
    """
with st.status("正在执行多步操作...") as status:
    st.write("步骤1：读取数据...")
    time.sleep(1)
    st.write("步骤2：处理数据...")
    time.sleep(1)
    st.write("步骤3：保存结果...")
    time.sleep(1)
    status.update(label="完成！", state="complete")
""",
    language="python",
)

if st.button("▶ 启动多步骤状态演示"):
    with st.status("正在执行多步骤操作...", expanded=True) as status:
        st.write("🔵 步骤1：读取数据...")
        time.sleep(1)
        st.write("🔵 步骤2：处理数据...")
        time.sleep(1)
        st.write("🔵 步骤3：保存结果...")
        time.sleep(1)
        status.update(label="✅ 所有步骤完成！", state="complete", expanded=False)

st.divider()

# ===================================================
# 6. Toast 通知
# ===================================================
st.header("6️⃣ Toast 通知 - st.toast()（Streamlit 1.28+）")

st.markdown(
    """
`st.toast()` 在页面右下角弹出短暂的提示通知，几秒后自动消失。
适合操作成功/失败后给予轻量提示，不打断用户。
"""
)

st.code(
    """
st.toast("🎉 保存成功！", icon="✅")
""",
    language="python",
)

if st.button("🔔 触发 Toast 通知"):
    try:
        st.toast("🎉 操作成功！", icon="✅")
    except Exception:
        st.info("当前版本不支持 st.toast()，需要 1.28+ 版本")

st.divider()

# ===================================================
# 7. 庆祝动画
# ===================================================
st.header("7️⃣ 庆祝动画 - st.balloons() / st.snow()")

st.markdown(
    """
两种开箱即用的庆祝特效，适合操作完成或里程碑达成时使用。
"""
)

col1, col2 = st.columns(2)

with col1:
    st.subheader("🎈 气球特效")
    st.code("st.balloons()", language="python")
    if st.button("🎈 放气球！"):
        st.balloons()

with col2:
    st.subheader("❄️ 下雪特效")
    st.code("st.snow()", language="python")
    if st.button("❄️ 下雪了！"):
        st.snow()

st.divider()

st.success(
    """
🎉 本模块涵盖了 Streamlit 所有的进度展示与提示控件：
- `st.success / info / warning / error`：4种消息提示框
- `st.exception()`：友好展示异常信息
- `st.progress()`：进度条，适合批处理场景
- `st.spinner()`：旋转加载动画，适合包裹耗时操作
- `st.status()`：多步骤任务状态区块（1.28+）
- `st.toast()`：右下角轻量通知（1.28+）
- `st.balloons() / st.snow()`：庆祝动画
"""
)
