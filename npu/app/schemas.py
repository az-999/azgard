from typing import List, Literal

from pydantic import BaseModel, Field


Role = Literal["system", "user", "assistant"]


class Message(BaseModel):
    role: Role
    content: str = Field(..., description="Текст сообщения")


class ChatRequest(BaseModel):
    messages: List[Message]
    max_new_tokens: int | None = Field(
        default=None,
        ge=1,
        le=4096,
        description="Максимум генерируемых токенов",
    )
    temperature: float | None = Field(
        default=None,
        ge=0.0,
        le=2.0,
        description="Температура сэмплинга",
    )


class ChatResponse(BaseModel):
    reply: str

