#!/usr/bin/env bash
# Скачивание модели в hf-cache с нормальным tqdm в терминале (из каталога npu).
set -euo pipefail
cd "$(dirname "$0")"

export HF_HOME="${HF_HOME:-$PWD/hf-cache}"
export MODEL_ID="${MODEL_ID:-OpenVINO/Qwen2.5-7B-Instruct-int4-ov}"

echo "HF_HOME=$HF_HOME MODEL_ID=$MODEL_ID" >&2
exec python -m app.hf_console download "$@"
