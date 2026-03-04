from __future__ import annotations

from functools import lru_cache
from typing import Tuple

from transformers import AutoTokenizer
from optimum.intel.openvino import OVModelForCausalLM

from .config import get_settings


@lru_cache(maxsize=1)
def load_model() -> Tuple[AutoTokenizer, OVModelForCausalLM]:
    """
    Lazy-load OpenVINO model and tokenizer.
    Respects MODEL_ID, OV_DEVICE and HF_TOKEN environment variables.
    """
    settings = get_settings()

    model_kwargs = {
        "device": settings.ov_device,
    }

    if settings.cache_dir:
        model_kwargs["cache_dir"] = settings.cache_dir

    tokenizer_kwargs = {}
    if settings.cache_dir:
        tokenizer_kwargs["cache_dir"] = settings.cache_dir

    tokenizer = AutoTokenizer.from_pretrained(
        settings.model_id,
        use_fast=True,
        **tokenizer_kwargs,
    )

    # If this is a pre-exported OpenVINO model (e.g. OpenVINO/qwen2.5-7b-instruct-int4-ov),
    # we just load it. Otherwise, optimum-intel will export it on first run.
    export_needed = not settings.model_id.lower().startswith("openvino/")

    model = OVModelForCausalLM.from_pretrained(
        settings.model_id,
        export=export_needed,
        trust_remote_code=True,
        **model_kwargs,
    )

    return tokenizer, model

