"""
模块6：状态管理
本页面展示 Streamlit 的状态管理机制，
包括 session_state、缓存（cache_data / cache_resource）和回调函数。

这是 Streamlit 开发中的核心高级概念，理解了状态管理才能构建复杂应用。
"""

import streamlit as st
import time
import pandas as pd
import numpy as np

st.set_page_config(
    page_title="状态管理 - Streamlit 学习",
    page_icon="🔄",
    layout="wide"
)

st.title("🔄 模块6：状态管理")

st.info(
    """
### 🔑 关键概念：Streamlit 的运行机制

每次用户交互（点击按钮、移动滑块等），整个 Python 脚本会**从头到尾重新运行**。
这意味着：
- **普通变量** 在每次运行后都会重置为初始值
- 要在多次运行间 **保留数据**，必须使用 `st.session_state`
- 要 **缓存**耗时的计算结果（避免重复计算），使用 `@st.cache_data` 或 `@st.cache_resource`

理解这一点，是写好 Streamlit 应用的关键！
"""
)

st.divider()

# ===================================================
# 1. 普通变量的问题（演示为何需要 session_state）
# ===================================================
st.header("1️⃣ 为何需要 Session State？")

st.markdown(
    """
先看一个 **错误示例** ——用普通变量记录点击次数：
```python
# ❌ 错误做法：count 每次脚本运行都会重置为 0
count = 0

if st.button("点击 +1"):
    count += 1       # 加了1，但下次运行 count 又变回 0

st.write(f"点击次数：{count}")   # 永远显示 0 或 1
```

每次点击后 `count` 都从 0 开始，永远记不住之前的状态。
这就是需要 **session_state** 的原因！
"""
)

st.divider()

# ===================================================
# 2. st.session_state 基础用法
# ===================================================
st.header("2️⃣ st.session_state — 跨运行状态保存")

st.markdown(
    """
`st.session_state` 是一个字典式对象，保存当前 **会话（Session）** 的状态。
浏览器刷新或关闭后状态会清除；同一会话内状态持续存在。
"""
)

st.code(
    """
# ✅ 正确做法：用 session_state 记录点击次数
if "count" not in st.session_state:
    st.session_state.count = 0    # 第一次运行时初始化

if st.button("点击 +1"):
    st.session_state.count += 1

st.write(f"点击次数：{st.session_state.count}")
""",
    language="python",
)

# 实际演示
col1, col2 = st.columns(2)

with col1:
    st.subheader("计数器示例")
    # 初始化 session_state（如果不存在则初始化）
    if "click_count" not in st.session_state:
        st.session_state.click_count = 0

    if st.button("➕ 点击 +1", key="btn_plus"):
        st.session_state.click_count += 1

    if st.button("➖ 点击 -1", key="btn_minus"):
        st.session_state.click_count = max(0, st.session_state.click_count - 1)

    if st.button("🔄 重置", key="btn_reset"):
        st.session_state.click_count = 0

    st.metric("当前计数", st.session_state.click_count)

with col2:
    st.subheader("session_state 内容查看")
    st.markdown("当前 session_state 的所有数据：")
    st.write(dict(st.session_state))

st.divider()

# ===================================================
# 3. session_state 结合输入控件
# ===================================================
st.header("3️⃣ session_state 与输入控件配合使用")

st.markdown(
    """
所有 Streamlit 输入控件都支持 `key` 参数。
设置 `key` 后，控件的值会自动与 `st.session_state[key]` 双向绑定：
- 控件值变化 → `session_state` 自动更新
- 代码中修改 `session_state[key]` → 控件值也会更新
"""
)

st.code(
    """
# 给控件设置 key，它的值自动保存到 session_state
name = st.text_input("你的名字", key="user_name")

# 可以直接读取
st.write(f"session_state['user_name'] = {st.session_state.get('user_name', '')}")

# 也可以用代码修改 session_state 来改变控件的值
if st.button("填入默认名字"):
    st.session_state.user_name = "张三"
""",
    language="python",
)

name_input = st.text_input("请输入你的名字（设置了 key='user_name'）", key="user_name_demo")

col_a, col_b = st.columns(2)
with col_a:
    if st.button("📝 填入默认名字「张三」"):
        st.session_state.user_name_demo = "张三"
with col_b:
    if st.button("🗑️ 清空名字"):
        st.session_state.user_name_demo = ""

st.write(f"session_state 中的值：`{st.session_state.get('user_name_demo', '（空）')}`")

st.divider()

# ===================================================
# 4. 回调函数 on_change / on_click
# ===================================================
st.header("4️⃣ 回调函数 — on_change / on_click")

st.markdown(
    """
控件支持传入 `on_change` 或 `on_click` 参数，指定一个**回调函数**。
当控件值发生变化时，回调函数会优先于其他代码执行。

这比普通的 "每次运行判断" 更精确，适合：
- 表单验证
- 联动更新其他控件
- 记录用户操作历史
"""
)

st.code(
    """
def on_slider_change():
    # 回调函数：滑块变化时自动调用
    st.session_state.slider_history.append(st.session_state.demo_slider)

if "slider_history" not in st.session_state:
    st.session_state.slider_history = []

st.slider("拖动我", 0, 100, 50, key="demo_slider", on_change=on_slider_change)
st.write("历史记录:", st.session_state.slider_history)
""",
    language="python",
)

def on_slider_change():
    """回调函数：滑块值变化时，记录历史"""
    if "callback_slider_history" not in st.session_state:
        st.session_state.callback_slider_history = []
    st.session_state.callback_slider_history.append(st.session_state.callback_slider_val)

if "callback_slider_history" not in st.session_state:
    st.session_state.callback_slider_history = []

st.slider(
    "拖动我，历史会被记录",
    min_value=0, max_value=100, value=50,
    key="callback_slider_val",
    on_change=on_slider_change
)
st.write("滑块历史记录：", st.session_state.callback_slider_history[-10:])

st.divider()

# ===================================================
# 5. @st.cache_data — 缓存数据计算结果
# ===================================================
st.header("5️⃣ @st.cache_data — 数据缓存")

st.markdown(
    """
`@st.cache_data` 是一个装饰器，用于缓存 **纯数据** 的计算结果（如 DataFrame、列表等）。
- 第一次调用函数时，执行计算并缓存结果
- 之后用同样的参数调用，直接返回缓存，**不重复计算**
- 返回值被序列化（深拷贝），函数内部修改不影响缓存

适合：读取 CSV、调用 API、复杂数据处理等耗时操作
"""
)

st.code(
    """
@st.cache_data
def load_big_data(n_rows: int):
    \"\"\"这个函数只在第一次（或参数改变时）真正执行\"\"\"
    time.sleep(2)  # 模拟耗时操作
    data = pd.DataFrame(np.random.randn(n_rows, 3), columns=["A", "B", "C"])
    return data

# 第一次慢，之后会从缓存直接返回（很快）
df = load_big_data(100)
""",
    language="python",
)

@st.cache_data
def simulate_slow_data_load(n_rows: int) -> pd.DataFrame:
    """模拟耗时的数据加载（被 cache_data 缓存后只执行一次）"""
    time.sleep(2)  # 模拟 2 秒加载时间
    data = pd.DataFrame(
        np.random.randn(n_rows, 3),
        columns=["指标A", "指标B", "指标C"]
    )
    return data

n_rows = st.slider("选择数据行数（改变参数会重新计算）", 50, 500, 100, step=50)

with st.spinner("加载数据中...（第一次约需 2 秒，缓存后瞬间完成）"):
    cached_df = simulate_slow_data_load(n_rows)

st.success(f"✅ 数据加载完成！共 {len(cached_df)} 行（缓存有效时不会等待）")
st.dataframe(cached_df.head(5))

st.markdown(
    """
**`@st.cache_data` 关键参数：**
- `ttl`：缓存有效期（秒），如 `@st.cache_data(ttl=60)` 缓存60秒
- `max_entries`：最多缓存多少个不同参数的结果
- `show_spinner`：是否显示加载中动画（默认 True）
"""
)

st.divider()

# ===================================================
# 6. @st.cache_resource — 缓存全局资源
# ===================================================
st.header("6️⃣ @st.cache_resource — 全局资源缓存")

st.markdown(
    """
`@st.cache_resource` 用于缓存 **全局共享资源**，如数据库连接、机器学习模型等。
- 与 `@st.cache_data` 的区别：资源对象在所有用户、所有 session 间共享（单例）
- 不会深拷贝，所有调用者拿到的是同一个对象
- 适合：加载大模型、建立数据库连接、初始化第三方客户端

```python
@st.cache_resource
def load_ml_model():
    \"\"\"全局只加载一次 AI 模型（很耗时）\"\"\"
    from transformers import pipeline
    model = pipeline("text-generation")
    return model

model = load_ml_model()   # 多个用户共享同一个模型对象
```
"""
)

@st.cache_resource
def get_global_config():
    """模拟全局配置/资源初始化，整个应用生命周期只运行一次"""
    time.sleep(1)
    return {
        "db_host": "localhost",
        "db_port": 5432,
        "app_version": "1.0.0",
        "max_connections": 10,
    }

with st.spinner("初始化全局配置（第一次运行后缓存）..."):
    config = get_global_config()

st.success("✅ 全局资源已就绪（缓存后所有用户共享）")
st.json(config)

st.divider()

# ===================================================
# 7. 手动清除缓存
# ===================================================
st.header("7️⃣ 手动清除缓存")

st.markdown(
    """
有时需要手动让缓存失效，让数据重新加载：
```python
# 清除特定函数的缓存
simulate_slow_data_load.clear()

# 清除所有 cache_data 缓存
st.cache_data.clear()

# 清除所有 cache_resource 缓存
st.cache_resource.clear()
```
"""
)

col_clear1, col_clear2 = st.columns(2)
with col_clear1:
    if st.button("🗑️ 清除数据缓存（cache_data）"):
        st.cache_data.clear()
        st.success("数据缓存已清除，下次调用会重新计算。")

with col_clear2:
    if st.button("🗑️ 清除资源缓存（cache_resource）"):
        st.cache_resource.clear()
        st.success("资源缓存已清除，下次调用会重新初始化。")

st.divider()

st.success(
    """
🎉 状态管理总结：
- `st.session_state`：保存单个用户的会话状态，跨多次运行持久化
- `@st.cache_data`：缓存数据计算结果，避免重复执行耗时操作
- `@st.cache_resource`：缓存全局资源（模型、连接等），所有用户共享
- `on_change/on_click`：回调函数，精准响应控件变化
"""
)
