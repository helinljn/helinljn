import html
import streamlit as st
from openai import OpenAI

# ── 固定参数（如需调整请直接修改代码）────────────────────────────────────────
TEMPERATURE = 0.6
TOP_P       = 0.7

# ── 页面基础配置 ──────────────────────────────────────────────────────────────
st.set_page_config(
    page_title            = "AI 聊天助手",
    page_icon             = "🤖",
    layout                = "centered",
    initial_sidebar_state = "expanded",
)

# ── 全局样式 ──────────────────────────────────────────────────────────────────
st.markdown(
    """
    <style>
    .stApp { background-color: #f0f2f6; }
    footer  { visibility: hidden; }

    [data-testid="collapsedControl"] { display: none !important; }
    section[data-testid="stSidebar"] > div > div > div > button { display: none !important; }

    .chat-row {
        display: flex;
        align-items: flex-end;
        margin: 6px 0;
        gap: 8px;
    }
    .chat-row.user { flex-direction: row-reverse; }

    .bubble {
        max-width: 72%;
        padding: 10px 14px;
        border-radius: 18px;
        font-size: 15px;
        line-height: 1.6;
        word-break: break-word;
        box-shadow: 0 1px 3px rgba(0,0,0,.12);
    }
    .bubble.user {
        background: #d1e8ff;
        color: #1a1a2e;
        border-bottom-right-radius: 4px;
    }
    .bubble.assistant {
        background: #ffffff;
        color: #1a1a2e;
        border-bottom-left-radius: 4px;
    }
    .avatar {
        font-size: 26px;
        line-height: 1;
        flex-shrink: 0;
    }
    </style>
    """,
    unsafe_allow_html = True,
)

# ── Session State 初始化 ──────────────────────────────────────────────────────
if "messages" not in st.session_state:
    st.session_state.messages = []   # 仅存在于当次会话，刷新即清空

# ── 顶部标题 ──────────────────────────────────────────────────────────────────
st.title("🤖 AI 聊天助手")
st.caption("刷新页面即可清空对话及配置")

# ── 配置输入区（刷新即清空，不写入 session_state）────────────────────────────
col1, col2 = st.columns([2, 1])
with col1:
    base_url_input = st.text_input(
        "🌐 Base URL",
        placeholder = "https://router.shengsuanyun.com/api/v1",
        help        = "API 服务地址",
    )
with col2:
    model_input = st.text_input(
        "🧠 Model",
        placeholder = "anthropic/claude-sonnet-4.6",
        help        = "模型名称",
    )
api_key_input = st.text_input(
    "🔑 API Key",
    type        = "password",
    placeholder = "请输入 API 密钥（如：sk-xxxx）",
    help        = "密码仅保存在本次会话内存中，刷新后自动清除。",
)

st.divider()

# ── 侧边栏：清空按钮（固定可见，无需滚动）────────────────────────────────────
with st.sidebar:
    st.markdown("### ⚙️ 操作")
    st.markdown("---")
    if st.button(
        "🗑️ 清空聊天记录",
        use_container_width = True,
        type                = "primary",
        help                = "清除当前所有对话，配置信息保留",
    ):
        st.session_state.messages = []
        st.rerun()
    st.markdown(
        "<small style='color:#888;'>聊天记录仅存于内存<br>刷新页面自动清空</small>",
        unsafe_allow_html = True,
    )

# ── 生成气泡 HTML ─────────────────────────────────────────────────────────────
def _bubble_html(role: str, content: str, cursor: bool = False) -> str:
    """返回单条消息的气泡 HTML 片段。content 会做 HTML 转义。"""
    if role == "user":
        avatar, bubble_cls, row_cls = "🧑", "user", "user"
    else:
        avatar, bubble_cls, row_cls = "🤖", "assistant", "assistant"
    cursor_char  = "▌" if cursor else ""
    safe_content = html.escape(content)
    return (
        f'<div class="chat-row {row_cls}">'
        f'<div class="avatar">{avatar}</div>'
        f'<div class="bubble {bubble_cls}">{safe_content}{cursor_char}</div>'
        f'</div>'
    )

# ── 渲染消息气泡 ──────────────────────────────────────────────────────────────
def render_message(role: str, content: str) -> None:
    st.markdown(_bubble_html(role, content), unsafe_allow_html = True)

# ── 渲染历史记录 ──────────────────────────────────────────────────────────────
chat_area = st.container()
with chat_area:
    for msg in st.session_state.messages:
        render_message(msg["role"], msg["content"])

# ── 底部输入框 ────────────────────────────────────────────────────────────────
user_input = st.chat_input("输入消息，按 Enter 发送…")
if user_input:
    # 0. 校验配置
    if not base_url_input.strip() or not api_key_input.strip() or not model_input.strip():
        st.warning("请先填写 Base URL、API Key 和 Model 后再发送消息！", icon = "⚠️")
        st.stop()

    # 1. 保存并渲染用户消息
    st.session_state.messages.append({"role": "user", "content": user_input})
    with chat_area:
        render_message("user", user_input)

    # 2. 调用 API（流式输出）
    client = OpenAI(base_url = base_url_input.strip(), api_key = api_key_input.strip())

    with chat_area:
        response_placeholder = st.empty()
        full_response        = ""

        success = False
        try:
            stream = client.chat.completions.create(
                model       = model_input.strip(),
                messages    = st.session_state.messages,
                temperature = TEMPERATURE,
                top_p       = TOP_P,
                stream      = True,
            )

            for chunk in stream:
                if chunk.choices and chunk.choices[0].delta.content is not None:
                    full_response += chunk.choices[0].delta.content
                    response_placeholder.markdown(
                        _bubble_html("assistant", full_response, cursor = True),
                        unsafe_allow_html = True,
                    )

            # 流结束，去掉光标符号
            response_placeholder.markdown(
                _bubble_html("assistant", full_response),
                unsafe_allow_html = True,
            )
            success = True

        except Exception as e:
            response_placeholder.error(f"⚠️ 调用 API 出错：{e}")

    # 3. 仅成功时保存 AI 回复（避免错误信息污染后续上下文）
    if success and full_response:
        st.session_state.messages.append({"role": "assistant", "content": full_response})