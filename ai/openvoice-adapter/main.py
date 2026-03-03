"""
OpenVoice TTS adapter: OpenAI-compatible /v1/audio/speech endpoint → OpenVoice /synthesize_speech/
Позволяет задать голос ассистенту: загрузите образец через OpenVoice /upload_audio/,
затем укажите имя голоса (label) в настройках TTS в Open WebUI.
"""
import os
import httpx
from fastapi import FastAPI, HTTPException
from fastapi.responses import Response
from pydantic import BaseModel

OPENVOICE_URL = os.environ.get("OPENVOICE_URL", "http://openvoice:8000")
DEFAULT_VOICE = os.environ.get("OPENVOICE_DEFAULT_VOICE", "demo_speaker0")

app = FastAPI(title="OpenVoice TTS Adapter", version="1.0")


class SpeechRequest(BaseModel):
    model: str = "tts-1"
    input: str
    voice: str | None = None
    speed: float = 1.0
    response_format: str | None = None


@app.post("/v1/audio/speech")
async def speech(req: SpeechRequest):
    if not req.input or not req.input.strip():
        raise HTTPException(status_code=400, detail="input is required")
    voice = (req.voice or DEFAULT_VOICE).strip() or DEFAULT_VOICE
    try:
        async with httpx.AsyncClient(timeout=120.0) as client:
            r = await client.get(
                f"{OPENVOICE_URL}/synthesize_speech/",
                params={
                    "text": req.input,
                    "voice": voice,
                    "speed": req.speed,
                    "accent": "en-newest",
                },
            )
            r.raise_for_status()
            return Response(
                content=r.content,
                media_type="audio/wav",
                headers={"Content-Disposition": "inline; filename=speech.wav"},
            )
    except httpx.HTTPStatusError as e:
        raise HTTPException(
            status_code=e.response.status_code,
            detail=e.response.text or "OpenVoice request failed",
        )
    except httpx.RequestError as e:
        raise HTTPException(status_code=502, detail=f"OpenVoice unreachable: {e!s}")


@app.get("/health")
def health():
    return {"status": "ok"}
