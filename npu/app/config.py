import os
from functools import lru_cache


class Settings:
    project_name: str = "OpenVINO Qwen2 Assistant"
    model_id: str = os.getenv("MODEL_ID", "Qwen/Qwen2-7B-Instruct")
    ov_device: str = os.getenv("OV_DEVICE", "AUTO")
    hf_token: str | None = os.getenv("HF_TOKEN")
    cache_dir: str | None = os.getenv("HF_HOME")
    max_new_tokens: int = int(os.getenv("MAX_NEW_TOKENS", "512"))
    temperature: float = float(os.getenv("TEMPERATURE", "0.7"))


@lru_cache(maxsize=1)
def get_settings() -> Settings:
    return Settings()

