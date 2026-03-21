#!/usr/bin/env bash
# Сброс кеша HF (только текущая MODEL_ID или весь HF_HOME). Запуск из каталога npu.
set -euo pipefail
cd "$(dirname "$0")"

export HF_HOME="${HF_HOME:-$PWD/hf-cache}"
export MODEL_ID="${MODEL_ID:-OpenVINO/Qwen2.5-7B-Instruct-int4-ov}"

exec python -m app.hf_console reset "$@"
