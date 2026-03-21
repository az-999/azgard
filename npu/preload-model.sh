#!/usr/bin/env bash
# Предзагрузка модели в npu/hf-cache без запуска API (локально, из каталога npu).
set -euo pipefail
cd "$(dirname "$0")"

export HF_HOME="${HF_HOME:-$PWD/hf-cache}"
export MODEL_ID="${MODEL_ID:-OpenVINO/Qwen2.5-7B-Instruct-int4-ov}"
export OV_DEVICE="${OV_DEVICE:-CPU}"

echo "HF_HOME=$HF_HOME MODEL_ID=$MODEL_ID OV_DEVICE=$OV_DEVICE" >&2
exec python -m app.preload "$@"
