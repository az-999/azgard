FROM python:3.11-slim

ENV PYTHONUNBUFFERED=1 \
    PIP_NO_CACHE_DIR=1 \
    PYTHONFAULTHANDLER=1

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY requirements.txt .
RUN pip install --upgrade pip && pip install -r requirements.txt

COPY app ./app

# Значения по умолчанию можно переопределить через docker run / compose
# В Docker обычно нет прямого доступа к Intel NPU; AUTO иногда долго «висит» на выборе устройства.
# Для стабильности по умолчанию CPU; на хосте с NPU можно переопределить: -e OV_DEVICE=NPU
ENV MODEL_ID=OpenVINO/Qwen2.5-7B-Instruct-int4-ov \
    OV_DEVICE=CPU \
    HF_HOME=/app/hf-cache

EXPOSE 8000

CMD ["uvicorn", "app.main:app", "--host", "0.0.0.0", "--port", "8000"]

