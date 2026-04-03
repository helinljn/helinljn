"""
模块7：媒体与文件
介绍 Streamlit 中图片、音频、视频的展示及文件上传下载功能，
包括 st.image、st.audio、st.video、st.file_uploader 等控件。
"""

import streamlit as st
import numpy as np
from io import BytesIO

st.set_page_config(
    page_title="媒体与文件 - Streamlit 学习",
    page_icon="🖼️",
    layout="wide"
)

st.title("🖼️ 模块7：媒体与文件")

st.markdown(
    """
### 简介
多媒体展示丰富了网页的表现力。Streamlit 支持图片、音频、视频等内容展示，
并且提供了方便的文件上传下载控件。

下面展示这些控件的具体用法和示例。
"""
)

st.divider()

# ===================================================
# 1. 图片显示 st.image()
# ===================================================
st.header("1️⃣ 图片展示 - st.image()")

st.markdown(
    """
支持的输入：
- 图片文件路径
- URL
- NumPy 数组
- PIL Image 对象

参数支持自动缩放、宽度控制
"""
)

st.code(
    """
from PIL import Image
image = Image.open("cat.jpg")
st.image(image, caption="猫猫")
""", language="python"
)

try:
    from PIL import Image
    img_url = "https://placekitten.com/400/300"
    st.image(img_url, caption="网络上的猫猫图片", width=400)
except ImportError:
    st.warning("缺少 Pillow 包，无法加载示例图片。")

# 生成彩色随机图像
img_array = np.random.randint(0, 255, (100, 150, 3), dtype=np.uint8)
st.image(img_array, caption="随机生成的彩色图像 (NumPy 数组)", width=150)

st.divider()

# ===================================================
# 2. 音频播放 st.audio()
# ===================================================
st.header("2️⃣ 音频播放 - st.audio()")

st.markdown(
    """
支持音频文件路径，URL，二进制bytes或文件对象。
使用参数 `start_time` 指定播放起点（秒）
""")

# 示例音频 URL
audio_url = "https://www2.cs.uic.edu/~i101/SoundFiles/CantinaBand3.wav"
st.audio(audio_url, format="audio/wav")

st.markdown("上传你的音频文件后也支持在线播放。")

uploaded_audio = st.file_uploader("上传音频文件 (mp3/wav)", type=["mp3", "wav"])
if uploaded_audio is not None:
    st.audio(uploaded_audio)

st.divider()

# ===================================================
# 3. 视频播放 st.video()
# ===================================================
st.header("3️⃣ 视频播放 - st.video()")

st.markdown(
    """
支持视频文件路径，URL，二进制bytes或文件对象。
参数 `start_time` 也是秒数起点。
"""
)

# 示例视频 URL（公共可用短视频）
video_url = "https://streamlit-demo-video.s3-us-west-2.amazonaws.com/pexels-pixabay-854913.mp4"
st.video(video_url)

uploaded_video = st.file_uploader("上传视频文件 (mp4)", type=["mp4"])
if uploaded_video is not None:
    st.video(uploaded_video)

st.divider()

# ===================================================
# 4. 文件上传 st.file_uploader()
# ===================================================
st.header("4️⃣ 文件上传 - st.file_uploader()")

st.markdown(
    """
文件上传控件，支持多种文件类型，支持单文件或多文件上传。
传入参数 `type`控制可选文件扩展名列表。
"""
)

uploaded_file = st.file_uploader("上传文件（支持单个文件）", accept_multiple_files=False)
if uploaded_file is not None:
    st.write(f"已上传文件：{uploaded_file.name}，大小：{uploaded_file.size} 字节")

uploaded_files = st.file_uploader("上传多个文件（按住 Ctrl/Shift 多选）", accept_multiple_files=True)
if uploaded_files:
    st.write(f"共上传了 {len(uploaded_files)} 个文件")
    for i, file in enumerate(uploaded_files):
        st.write(f"{i+1}. {file.name} ({file.size} 字节)")

st.divider()

# ===================================================
# 5. 文件下载 st.download_button()
# ===================================================
st.header("5️⃣ 文件下载 - st.download_button()")

st.markdown(
    """
下载按钮，可以下载字符串、bytes、文件对象等。
常用于导出数据、生成报告的下载。
"""
)

csv_sample = "姓名,年龄\n张三,30\n李四,25"
st.download_button(
    label="下载示例 CSV 文件",
    data=csv_sample,
    file_name="sample.csv",
    mime="text/csv"
)

st.markdown(
    """
你也可以动态生成并提供下载，例如：
- 导出表格
- 导出图片
- 导出文本文件
"""
)

st.divider()

st.success("🎉 本模块示范了多媒体及文件上传下载相关控件，支持丰富交互场景！")
