from __future__ import annotations

from typing import List
from pathlib import Path

from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import HTMLResponse
from fastapi.staticfiles import StaticFiles

from .config import get_settings
from .schemas import ChatRequest, ChatResponse, Message


settings = get_settings()
app = FastAPI(title=settings.project_name)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

static_dir = Path(__file__).parent / "static"
if static_dir.exists():
    app.mount("/static", StaticFiles(directory=str(static_dir), html=True), name="static")


@app.on_event("startup")
async def startup_event() -> None:
    # Ленивый импорт: тяжёлые optimum/openvino не грузятся при импорте app.main
    from .model import load_model
    load_model()


@app.get("/", response_class=HTMLResponse)
async def root() -> HTMLResponse:
    return HTMLResponse(
        "<html><body><h1>OpenVINO Qwen2 Assistant</h1><p>Перейдите на /static для веб‑интерфейса.</p></body></html>"
    )


@app.post("/api/chat", response_model=ChatResponse)
async def chat(request: ChatRequest) -> ChatResponse:
    from .model import load_model
    tokenizer, model = load_model()

    messages: List[dict] = [m.model_dump() for m in request.messages]

    input_ids = tokenizer.apply_chat_template(
        messages,
        tokenize=True,
        add_generation_prompt=True,
        return_tensors="pt",
    )

    max_new_tokens = request.max_new_tokens or settings.max_new_tokens
    temperature = request.temperature if request.temperature is not None else settings.temperature
    do_sample = temperature > 0.0

    outputs = model.generate(
        input_ids=input_ids,
        max_new_tokens=max_new_tokens,
        do_sample=do_sample,
        temperature=temperature,
        pad_token_id=tokenizer.eos_token_id,
    )

    generated_ids = outputs[0, input_ids.shape[1] :]
    text = tokenizer.decode(generated_ids, skip_special_tokens=True)

    return ChatResponse(reply=text.strip())

