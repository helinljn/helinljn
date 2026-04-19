const API_BASE_OPTIONS = [
  "https://api.apiplus.org/v1",
  "https://api.deepseek.com/v1",
];

const API_KEY_OPTIONS = [
  "sk-aaa",
  "sk-bbb",
];

const MODEL_ID_OPTIONS = [
  "gpt-5.4",
  "deepseek-chat",
  "deepseek-reasoner",
];

const DEFAULT_SETTINGS = {
  apiBase: API_BASE_OPTIONS[0],
  apiKey: API_KEY_OPTIONS[0],
  modelId: MODEL_ID_OPTIONS[0],
};

const elements = {
  apiBase: document.getElementById("apiBase"),
  apiKey: document.getElementById("apiKey"),
  modelId: document.getElementById("modelId"),
  clearChatBtn: document.getElementById("clearChatBtn"),
  statusBox: document.getElementById("statusBox"),
  chatMessages: document.getElementById("chatMessages"),
  emptyState: document.getElementById("emptyState"),
  userInput: document.getElementById("userInput"),
  sendBtn: document.getElementById("sendBtn"),
};

const state = {
  messages: [],
  isSending: false,
};

init();

function init() {
  renderSelectOptions(elements.apiBase, API_BASE_OPTIONS);
  renderSelectOptions(elements.apiKey, API_KEY_OPTIONS);
  renderSelectOptions(elements.modelId, MODEL_ID_OPTIONS);

  applySettingsToForm(DEFAULT_SETTINGS);
  renderMessages();
  bindEvents();
  setStatus("已完成初始化");
}

function bindEvents() {
  elements.clearChatBtn.addEventListener("click", clearChat);
  elements.sendBtn.addEventListener("click", sendMessage);

  elements.userInput.addEventListener("keydown", (event) => {
    if ((event.ctrlKey || event.metaKey) && event.key === "Enter") {
      event.preventDefault();
      sendMessage();
    }
  });
}

function getSettingsFromForm() {
  return {
    apiBase: elements.apiBase.value.trim(),
    apiKey: elements.apiKey.value.trim(),
    modelId: elements.modelId.value.trim(),
  };
}

function applySettingsToForm(settings) {
  const merged = { ...DEFAULT_SETTINGS, ...(settings || {}) };
  elements.apiBase.value = merged.apiBase;
  elements.apiKey.value = merged.apiKey;
  elements.modelId.value = merged.modelId;
}

function renderSelectOptions(targetElement, items) {
  targetElement.innerHTML = "";

  for (const item of items) {
    const option = document.createElement("option");
    option.value = item;
    option.textContent = item;
    targetElement.appendChild(option);
  }
}

function renderMessages() {
  elements.chatMessages.innerHTML = "";

  if (!state.messages.length) {
    elements.chatMessages.appendChild(elements.emptyState);
    return;
  }

  for (const message of state.messages) {
    elements.chatMessages.appendChild(createMessageElement(message));
  }

  scrollMessagesToBottom();
}

function createMessageElement(message) {
  const row = document.createElement("div");
  row.className = `message-row ${message.role}`;

  const card = document.createElement("div");
  card.className = `message-card ${message.role}`;

  const meta = document.createElement("div");
  meta.className = "message-meta";
  meta.textContent = `${getRoleLabel(message.role)} · ${formatTime(message.timestamp)}`;

  const content = document.createElement("div");
  content.className = "message-content";
  content.textContent = message.content || "";

  card.appendChild(meta);
  card.appendChild(content);
  row.appendChild(card);

  return row;
}

function addMessage(role, content) {
  const message = {
    role,
    content,
    timestamp: new Date().toISOString(),
  };

  state.messages.push(message);
  renderMessages();

  return message;
}

function clearChat() {
  if (!state.messages.length) {
    setStatus("当前没有聊天记录需要清空");
    return;
  }

  const confirmed = window.confirm("确定要清空当前聊天记录吗？此操作不可撤销。");
  if (!confirmed) {
    return;
  }

  state.messages = [];
  renderMessages();
  setStatus("聊天记录已清空");
}

async function sendMessage() {
  if (state.isSending) {
    return;
  }

  const settings = getSettingsFromForm();
  const validationMessage = validateSettings(settings);
  if (validationMessage) {
    setStatus(validationMessage, true);
    return;
  }

  const userText = elements.userInput.value.trim();
  if (!userText) {
    setStatus("请输入要发送的消息", true);
    return;
  }

  addMessage("user", userText);
  elements.userInput.value = "";

  setSendingState(true);
  setStatus("正在请求 AI 响应...");

  try {
    const response = await requestChatCompletion(settings);

    if (!response.ok) {
      const errorData = await response.json().catch(() => null);
      const errorMessage =
        errorData?.error?.message ||
        errorData?.message ||
        `请求失败，状态码：${response.status}`;
      throw new Error(errorMessage);
    }

    const data = await response.json().catch(() => null);
    const assistantText = extractAssistantContent(data);

    if (!assistantText) {
      throw new Error("接口返回成功，但未获取到模型回复内容");
    }

    addMessage("assistant", assistantText);
    setStatus("AI 回复完成");
  } catch (error) {
    console.error(error);
    setStatus(`发送失败：${error.message}`, true);
  } finally {
    setSendingState(false);
  }
}

function validateSettings(settings) {
  if (!settings.apiBase) {
    return "请填写 API Base URL";
  }

  if (!settings.apiKey) {
    return "请填写 API Key";
  }

  if (!settings.modelId) {
    return "请填写模型 ID";
  }

  return "";
}

function requestChatCompletion(settings) {
  const payload = {
    model: settings.modelId,
    messages: buildRequestMessages(),
  };

  return fetch(buildChatEndpoint(settings.apiBase), {
    method: "POST",
    headers: {
      "Content-Type": "application/json",
      Authorization: `Bearer ${settings.apiKey}`,
    },
    body: JSON.stringify(payload),
  });
}

function buildRequestMessages() {
  return state.messages
    .filter((message) => message.role === "user" || message.role === "assistant")
    .map((message) => ({
      role: message.role,
      content: message.content,
    }));
}

function buildChatEndpoint(apiBase) {
  const trimmed = apiBase.trim().replace(/\/+$/, "");
  if (/\/chat\/completions$/i.test(trimmed)) {
    return trimmed;
  }

  return `${trimmed}/chat/completions`;
}

function extractAssistantContent(data) {
  const choice = data?.choices?.[0];
  const content = choice?.message?.content;

  if (typeof content === "string") {
    return content.trim();
  }

  if (Array.isArray(content)) {
    return content
      .map((item) => {
        if (typeof item === "string") {
          return item;
        }

        if (item?.type === "text") {
          return item.text || "";
        }

        return "";
      })
      .join("")
      .trim();
  }

  if (typeof data?.output_text === "string") {
    return data.output_text.trim();
  }

  return "";
}

function setSendingState(sending) {
  state.isSending = sending;
  elements.sendBtn.disabled = sending;
  elements.sendBtn.textContent = sending ? "发送中..." : "发送";
}

function setStatus(text, isError = false) {
  elements.statusBox.textContent = text;
  elements.statusBox.style.color = isError ? "#fecaca" : "#dbeafe";
  elements.statusBox.style.borderColor = isError ? "rgba(239, 68, 68, 0.45)" : "var(--border)";
}

function scrollMessagesToBottom() {
  elements.chatMessages.scrollTop = elements.chatMessages.scrollHeight;
}

function getRoleLabel(role) {
  if (role === "user") return "用户";
  if (role === "assistant") return "AI";
  if (role === "system") return "系统";
  return role;
}

function formatTime(timestamp) {
  const date = timestamp ? new Date(timestamp) : new Date();
  if (Number.isNaN(date.getTime())) {
    return "";
  }

  return date.toLocaleString("zh-CN", { hour12: false });
}
