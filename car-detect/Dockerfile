# anpr-local/Dockerfile
FROM nvidia/cuda:12.3.1-cudnn8-runtime-ubuntu22.04

# Установка системных зависимостей
RUN apt-get update && apt-get install -y \
    python3 python3-pip python3-dev \
    libgl1 libglib2.0-0 libsm6 libxext6 libxrender1 libgomp1 \
    git wget ffmpeg libopencv-dev \
    && rm -rf /var/lib/apt/lists/*

# Установка PaddlePaddle с поддержкой GPU (CUDA 12.x)
RUN pip3 install --upgrade pip && \
    pip3 install paddlepaddle-gpu==2.6.0.post123 -f https://www.paddlepaddle.org.cn/whl/linux/mkl/avx/stable.html

# Установка остальных зависимостей
RUN pip3 install \
    paddleocr==2.7.0.3 \
    ultralytics==8.2.0 \
    opencv-python==4.9.0.80 \
    numpy==1.24.4 \
    pyyaml requests tqdm

# Копируем приложение
WORKDIR /app
COPY app/ /app/

# Скачиваем модели при первом запуске (не при сборке — экономим время)
RUN chmod +x /app/recognize.py

# Порт для веб-интерфейса (опционально)
EXPOSE 5000

CMD ["python3", "/app/recognize.py", "--input", "/data", "--output", "/output"]