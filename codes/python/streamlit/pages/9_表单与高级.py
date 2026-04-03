"""
模块9：表单与高级功能
本页面展示 Streamlit 的高级交互控件和功能，
包括表单（st.form）、对话框（st.dialog）、局部刷新（st.fragment）、
嵌入 HTML（st.html）以及第三方组件（st.components）。

这些是构建专业级 Streamlit 应用的核心高级特性。
"""

import streamlit as st
import time

st.set_page_config(
    page_title="表单与高级 - Streamlit 学习",
    page_icon="🧩",
    layout="wide",
)

st.title("🧩 模块9：表单与高级功能")

st.markdown(
    """
### 简介
掌握基础控件后，这些高级特性可以帮助你构建更专业的应用：
- **表单**：将多个输入收集后统一提交，避免每次输入都触发页面刷新
- **对话框**：弹窗式交互，用于确认、详情展示等场景
- **局部刷新**：只更新页面的一部分，提升性能
- **HTML 嵌入**：自定义样式和布局
- **组件扩展**：集成第三方 JavaScript 组件
"""
)

st.divider()

# ===================================================
# 1. 表单 st.form
# ===================================================
st.header("1️⃣ 表单 — st.form()")

st.markdown(
    """
**为什么需要 form？**

Streamlit 默认行为：每次用户操作一个控件（如滑动滑块），
脚本就会立即重新运行。如果有多个输入框，用户每输入一个字符就触发一次运行，
这不仅低效，还会丢失其他控件的中间状态。

`st.form()` 把多个控件包装成一个表单，
只有点击**提交按钮**后，才统一触发一次脚本运行。
适合：登录表单、搜索条件、数据录入等场景。
"""
)

st.code(
    """
# 基本用法：使用 with 语法
with st.form("my_form"):
    name = st.text_input("姓名")
    age  = st.slider("年龄", 0, 100, 25)

    # form 内只能有一个提交按钮
    submitted = st.form_submit_button("提交")

if submitted:
    st.write(f"提交成功！姓名：{name}，年龄：{age}")
""",
    language="python",
)

# 实际演示：用户注册表单
st.subheader("示例：用户注册表单")

with st.form("register_form"):
    st.markdown("**填写注册信息（所有字段填完后点击提交）**")

    r_col1, r_col2 = st.columns(2)
    with r_col1:
        r_name = st.text_input("用户名 *", placeholder="请输入用户名")
        r_email = st.text_input("邮箱 *", placeholder="example@email.com")
        r_age = st.slider("年龄", min_value=1, max_value=120, value=25)
    with r_col2:
        r_pwd = st.text_input("密码 *", type="password", placeholder="至少6位")
        r_pwd2 = st.text_input("确认密码 *", type="password", placeholder="再次输入密码")
        r_gender = st.radio("性别", ["男", "女", "保密"], horizontal=True)

    r_agree = st.checkbox("我已阅读并同意《用户协议》")
    r_submit = st.form_submit_button("📝 立即注册", use_container_width=True)

# 表单提交后的处理逻辑（在 form 块外面）
if r_submit:
    # 表单验证
    errors = []
    if not r_name:
        errors.append("用户名不能为空")
    if not r_email or "@" not in r_email:
        errors.append("请输入有效的邮箱地址")
    if len(r_pwd) < 6:
        errors.append("密码长度不能少于6位")
    if r_pwd != r_pwd2:
        errors.append("两次密码输入不一致")
    if not r_agree:
        errors.append("请先同意用户协议")

    if errors:
        for err in errors:
            st.error(f"❌ {err}")
    else:
        st.success(
            f"✅ 注册成功！欢迎，{r_name}（{r_gender}，{r_age}岁）！"
        )
        st.balloons()

st.markdown(
    """
**`st.form()` 关键要点：**
- 表单内所有控件的变化不会立即触发重新运行
- `st.form_submit_button()` 是表单内的专用提交按钮
- 提交按钮的返回值为 `True`（被点击时），处理逻辑写在 `with form:` 块外面
- 一个表单里只能有一个提交按钮
- `key` 参数保证多个表单不冲突
"""
)

st.divider()

# ===================================================
# 2. 对话框 st.dialog
# ===================================================
st.header("2️⃣ 对话框 — @st.dialog()（Streamlit 1.36+）")

st.markdown(
    """
`@st.dialog()` 是一个装饰器，将函数变成弹出对话框。
调用该函数时，对话框弹出；对话框内也可以有完整的 Streamlit 控件。

适合：确认操作、详情查看、多步骤向导等场景。
"""
)

st.code(
    """
@st.dialog("确认删除")
def confirm_delete(item_name: str):
    st.write(f"确定要删除「{item_name}」吗？此操作不可撤销！")
    col1, col2 = st.columns(2)
    with col1:
        if st.button("✅ 确认删除", type="primary"):
            st.session_state.deleted = True
            st.rerun()
    with col2:
        if st.button("❌ 取消"):
            st.rerun()

if st.button("删除项目"):
    confirm_delete("重要文件.docx")
""",
    language="python",
)

try:
    @st.dialog("🗑️ 确认删除")
    def confirm_delete_dialog(item_name: str):
        st.warning(f"确定要删除「{item_name}」吗？此操作不可撤销！")
        d_col1, d_col2 = st.columns(2)
        with d_col1:
            if st.button("✅ 确认删除", type="primary", use_container_width=True):
                st.session_state.dialog_deleted = True
                st.rerun()
        with d_col2:
            if st.button("❌ 取消", use_container_width=True):
                st.rerun()

    if "dialog_deleted" not in st.session_state:
        st.session_state.dialog_deleted = False

    if st.session_state.dialog_deleted:
        st.success("✅ 文件已删除（演示效果）")
        if st.button("↩ 重置演示"):
            st.session_state.dialog_deleted = False
            st.rerun()
    else:
        if st.button("🗑️ 点击删除文件（会弹出确认对话框）"):
            confirm_delete_dialog("重要文件.docx")

except AttributeError:
    st.warning("当前 Streamlit 版本不支持 @st.dialog()，需要 1.36+ 版本")
    st.info("升级方式：`pip install --upgrade streamlit`")

st.divider()

# ===================================================
# 3. 局部刷新 st.fragment
# ===================================================
st.header("3️⃣ 局部刷新 — @st.fragment()（Streamlit 1.37+）")

st.markdown(
    """
**问题背景：**
Streamlit 默认每次交互都会重新运行整个脚本。
对于复杂应用，这意味着所有图表、数据、计算都要重新执行，性能差。

**解决方案：**
`@st.fragment()` 将函数标记为"片段"——
片段内的控件触发交互时，**只重新运行该片段**，不影响页面其他部分。
"""
)

st.code(
    """
@st.fragment
def my_chart_fragment():
    \"\"\"这个函数内的交互只重新运行这个函数，不触发整页刷新\"\"\"
    color = st.color_picker("选择图表颜色")
    st.write(f"当前颜色：{color}")
    # 绘制图表...

my_chart_fragment()
st.write("这部分不会因为上面的交互而刷新")
""",
    language="python",
)

try:
    # 页面级别的运行计数器（演示用）
    if "page_run_count" not in st.session_state:
        st.session_state.page_run_count = 0
    st.session_state.page_run_count += 1

    @st.fragment
    def fragment_demo():
        """片段：内部交互不触发整页重新运行"""
        if "frag_run_count" not in st.session_state:
            st.session_state.frag_run_count = 0
        st.session_state.frag_run_count += 1

        frag_color = st.color_picker("选择颜色（片段内）", "#FF4B4B")
        frag_value = st.slider("选择数值（片段内）", 0, 100, 50)
        st.info(
            f"片段已运行 **{st.session_state.frag_run_count}** 次 | "
            f"颜色：{frag_color} | 数值：{frag_value}"
        )

    with st.container(border=True):
        st.markdown("**下面是一个 @st.fragment 片段（控件交互只重新运行此区域）：**")
        fragment_demo()

    st.markdown(
        f"**整页已运行次数：{st.session_state.page_run_count}** "
        f"（片段内交互不增加此计数）"
    )

except AttributeError:
    st.warning("当前 Streamlit 版本不支持 @st.fragment()，需要 1.37+ 版本")

st.divider()

# ===================================================
# 4. 嵌入 HTML — st.html
# ===================================================
st.header("4️⃣ 嵌入 HTML — st.html()（Streamlit 1.31+）")

st.markdown(
    """
`st.html()` 允许直接在页面中渲染原始 HTML 代码，
可以实现 Streamlit 原生控件不容易做到的自定义样式和布局。

> ⚠️ 注意：`st.html()` 出于安全考虑，会在隔离的 iframe 中渲染，
> 无法执行 JavaScript。如需 JS 交互，使用 `st.components.v1`。
"""
)

st.code(
    """
st.html(\"\"\"
<div style="background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 20px; border-radius: 10px; color: white;">
    <h2>🎨 自定义 HTML 卡片</h2>
    <p>这是通过 st.html() 嵌入的纯 HTML 内容</p>
</div>
\"\"\")
""",
    language="python",
)

try:
    st.html(
        """
<div style="
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    padding: 24px 32px;
    border-radius: 12px;
    color: white;
    font-family: 'Segoe UI', sans-serif;
    margin: 8px 0;
">
    <h2 style="margin: 0 0 8px 0;">🎨 自定义 HTML 渐变卡片</h2>
    <p style="margin: 0; opacity: 0.9;">
        这是通过 <code>st.html()</code> 嵌入的纯 HTML 内容，
        可以自定义任意 CSS 样式！
    </p>
</div>
"""
    )

    st.html(
        """
<table style="width:100%; border-collapse: collapse; margin: 8px 0;">
    <thead>
        <tr style="background-color: #f0f2f6;">
            <th style="padding: 10px; text-align: left; border: 1px solid #ddd;">功能</th>
            <th style="padding: 10px; text-align: left; border: 1px solid #ddd;">st.markdown</th>
            <th style="padding: 10px; text-align: left; border: 1px solid #ddd;">st.html</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td style="padding: 8px; border: 1px solid #ddd;">Markdown 语法</td>
            <td style="padding: 8px; border: 1px solid #ddd; color: green;">✅ 支持</td>
            <td style="padding: 8px; border: 1px solid #ddd; color: red;">❌ 不支持</td>
        </tr>
        <tr style="background-color: #fafafa;">
            <td style="padding: 8px; border: 1px solid #ddd;">自定义 CSS</td>
            <td style="padding: 8px; border: 1px solid #ddd; color: orange;">⚠️ 有限</td>
            <td style="padding: 8px; border: 1px solid #ddd; color: green;">✅ 完全支持</td>
        </tr>
        <tr>
            <td style="padding: 8px; border: 1px solid #ddd;">JavaScript</td>
            <td style="padding: 8px; border: 1px solid #ddd; color: red;">❌</td>
            <td style="padding: 8px; border: 1px solid #ddd; color: red;">❌ 被隔离</td>
        </tr>
    </tbody>
</table>
"""
    )
except AttributeError:
    st.warning("当前 Streamlit 版本不支持 st.html()，需要 1.31+ 版本")
    # 兼容写法
    st.markdown(
        '<div style="background: linear-gradient(135deg, #667eea, #764ba2); '
        'padding: 20px; border-radius: 10px; color: white;">'
        "<h2>🎨 降级到 st.markdown 的 HTML</h2></div>",
        unsafe_allow_html=True,
    )

st.divider()

# ===================================================
# 5. 第三方组件 st.components.v1
# ===================================================
st.header("5️⃣ 第三方组件 — st.components.v1")

st.markdown(
    """
`st.components.v1` 提供了两种方式嵌入第三方内容：

**① `st.components.v1.html()`**
- 渲染完整 HTML 页面，**支持 JavaScript 执行**
- 可以嵌入任意第三方 JS 库（ECharts、D3.js 等）
- 通过 `height` 参数控制显示高度

**② `st.components.v1.iframe()`**
- 在 iframe 中加载外部 URL
- 适合嵌入外部网站或工具
"""
)

st.code(
    """
import streamlit.components.v1 as components

# 嵌入 HTML + JavaScript
components.html(
    \"\"\"
    <script>
        document.write('<h1>Hello from JavaScript!</h1>');
        alert('这里可以执行 JS！');
    </script>
    \"\"\",
    height=100
)

# 嵌入外部网站
components.iframe("https://streamlit.io", height=400)
""",
    language="python",
)

import streamlit.components.v1 as components

# 嵌入带 JS 动画的 HTML
components.html(
    """
<!DOCTYPE html>
<html>
<head>
<style>
  body { margin: 0; font-family: 'Segoe UI', sans-serif; background: #0e1117; }
  .box {
    display: flex; justify-content: center; align-items: center;
    height: 100px; gap: 12px;
  }
  .ball {
    width: 20px; height: 20px; border-radius: 50%;
    animation: bounce 0.6s infinite alternate;
  }
  .ball:nth-child(1) { background: #FF4B4B; animation-delay: 0s; }
  .ball:nth-child(2) { background: #FFD700; animation-delay: 0.1s; }
  .ball:nth-child(3) { background: #00CC88; animation-delay: 0.2s; }
  .ball:nth-child(4) { background: #4B8BFF; animation-delay: 0.3s; }
  .ball:nth-child(5) { background: #CC4BFF; animation-delay: 0.4s; }
  @keyframes bounce {
    from { transform: translateY(0); }
    to   { transform: translateY(-50px); }
  }
  .label {
    color: #aaa; text-align: center; font-size: 13px; padding-bottom: 8px;
  }
</style>
</head>
<body>
  <p class="label">st.components.v1.html() — 支持 CSS 动画和 JavaScript</p>
  <div class="box">
    <div class="ball"></div>
    <div class="ball"></div>
    <div class="ball"></div>
    <div class="ball"></div>
    <div class="ball"></div>
  </div>
</body>
</html>
""",
    height=130,
)

st.divider()

# ===================================================
# 6. st.rerun() — 手动触发重新运行
# ===================================================
st.header("6️⃣ 手动触发重新运行 — st.rerun()")

st.markdown(
    """
`st.rerun()` 立即从头重新运行当前脚本，相当于手动刷新页面。
常见用途：
- 对话框关闭后刷新主页面
- 状态更新后立即反映到界面
- 轮询刷新数据
"""
)

st.code(
    """
import time

# 每3秒自动刷新一次（轮询示例）
st.write(f"当前时间: {time.strftime('%H:%M:%S')}")

time.sleep(3)
st.rerun()   # 重新运行整个脚本（谨慎使用！会无限循环）
""",
    language="python",
)

# 手动触发刷新演示（不用自动循环，避免无限刷新）
if "rerun_count" not in st.session_state:
    st.session_state.rerun_count = 0

st.info(f"本次会话内手动刷新次数：**{st.session_state.rerun_count}**")

if st.button("🔄 手动触发 st.rerun()"):
    st.session_state.rerun_count += 1
    st.rerun()

st.divider()

# ===================================================
# 7. st.stop() — 提前停止脚本
# ===================================================
st.header("7️⃣ 提前停止脚本执行 — st.stop()")

st.markdown(
    """
`st.stop()` 立即停止脚本在当前位置的继续执行，
页面只显示 `st.stop()` 之前渲染的内容。

常用于：
- 用户未登录时阻止显示受保护内容
- 表单未填写完毕时提前返回
- 条件门控（guard clauses）
"""
)

st.code(
    """
# 常见用法：登录门控
if not user_logged_in:
    st.error("请先登录！")
    st.stop()   # 阻止下面的内容继续显示

# 只有登录后才能看到下面的内容
st.write("欢迎回来，用户！")
""",
    language="python",
)

show_protected = st.checkbox("✅ 模拟「已登录」状态（勾选后才显示受保护内容）")

if not show_protected:
    st.error("🔒 请先登录！（未勾选复选框）")
    # st.stop()  # 取消注释即可实际阻止后续内容
    st.info("（演示中注释了 st.stop()，取消注释查看效果）")
else:
    st.success("🎉 欢迎！这是登录后才能看到的受保护内容。")

st.divider()

st.success(
    """
🎉 模块9 总结 — 高级功能全览：
- `st.form()`：将多输入统一提交，避免频繁刷新
- `@st.dialog()`：弹出对话框，用于确认/详情（1.36+）
- `@st.fragment()`：局部刷新，提升复杂应用性能（1.37+）
- `st.html()`：嵌入自定义 HTML/CSS（1.31+）
- `st.components.v1.html()`：嵌入 HTML+JavaScript
- `st.rerun()`：手动触发页面重新运行
- `st.stop()`：提前停止脚本执行（用于门控逻辑）
"""
)
