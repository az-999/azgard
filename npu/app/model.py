from __future__ import annotations

import logging
import threading
from typing import Tuple

from transformers import AutoTokenizer
from optimum.intel.openvino import OVModelForCausalLM

from .config import get_settings

logger = logging.getLogger(__name__)

_load_lock = threading.Lock()
_tokenizer: AutoTokenizer | None = None
_model: OVModelForCausalLM | None = None
_model_ready = False
_load_error: str | None = None


def is_model_ready() -> bool:
    return _model_ready


def get_model_status() -> dict:
    return {"ready": _model_ready, "error": _load_error}


def load_model() -> Tuple[AutoTokenizer, OVModelForCausalLM]:
    """
    Lazy-load OpenVINO model and tokenizer (thread-safe).
    Respects MODEL_ID, OV_DEVICE and HF_TOKEN environment variables.
    """
    global _tokenizer, _model, _model_ready, _load_error

    with _load_lock:
        if _tokenizer is not None and _model is not None:
            return _tokenizer, _model

        settings = get_settings()
        _load_error = None

        model_kwargs: dict = {
            "device": settings.ov_device,
        }

        if settings.cache_dir:
            model_kwargs["cache_dir"] = settings.cache_dir

        tokenizer_kwargs: dict = {}
        if settings.cache_dir:
            tokenizer_kwargs["cache_dir"] = settings.cache_dir

        try:
            logger.info(
                "Загрузка токенизатора: model_id=%s cache=%s",
                settings.model_id,
                settings.cache_dir,
            )
            tokenizer = AutoTokenizer.from_pretrained(
                settings.model_id,
                use_fast=True,
                **tokenizer_kwargs,
            )

            # Если это уже экспортированная OpenVINO-модель — только загрузка.
            export_needed = not settings.model_id.lower().startswith("openvino/")

            logger.info(
                "Загрузка OV-модели: device=%s export=%s (первый раз может занять много минут)",
                settings.ov_device,
                export_needed,
            )
            ov_model = OVModelForCausalLM.from_pretrained(
                settings.model_id,
                export=export_needed,
                trust_remote_code=True,
                **model_kwargs,
            )
            _tokenizer = tokenizer
            _model = ov_model
            _model_ready = True
            _load_error = None
            logger.info("Модель готова к генерации.")
            return _tokenizer, _model
        except Exception as e:
            _tokenizer = None
            _model = None
            _model_ready = False
            _load_error = f"{type(e).__name__}: {e}"
            logger.exception("Ошибка загрузки модели: %s", _load_error)
            raise
