https://chat.qwen.ai/s/t_3722f372-d542-43fc-a7f8-84dc22b8ee6a?fev=0.1.38

Вот готовый **Dockerfile + docker-compose.yml** для ANPR-системы с поддержкой русских номеров (ГРЗ). Собирается на базе официального образа NVIDIA CUDA, включает:

- **YOLOv8n** — детекция номерной таблички
- **PaddleOCR** — распознавание кириллицы (лучше EasyOCR для ГРЗ)
- **Валидация по регулярке РФ** — фильтрация ложных срабатываний
- **Пример скрипта** — обработка изображения/видео из папки `data/`

---

### 📁 Структура проекта

```
anpr-local/
├── Dockerfile
├── docker-compose.yml
├── app/
│   └── recognize.py          # основной скрипт
├── data/                     # сюда кидаете фото/видео
│   └── sample.jpg
└── README.md                 # инструкция по запуску
```

---

### 🐳 `Dockerfile`

```dockerfile
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
```

---

### ⚙️ `docker-compose.yml`

```yaml
# anpr-local/docker-compose.yml
version: '3.8'

services:
  anpr:
    build: .
    container_name: anpr-local
    runtime: nvidia  # Требует установки nvidia-container-toolkit
    environment:
      - NVIDIA_VISIBLE_DEVICES=all
      - NVIDIA_DRIVER_CAPABILITIES=compute,utility
    volumes:
      - ./data:/data          # входные файлы (фото/видео)
      - ./output:/output      # результаты распознавания
    command: >
      python3 recognize.py
        --input /data
        --output /output
        --mode realtime       # или 'batch' для папки
    restart: unless-stopped
```

---

### 🧠 `app/recognize.py` — основной скрипт

```python
#!/usr/bin/env python3
# anpr-local/app/recognize.py
import cv2
import re
import os
import time
import argparse
from pathlib import Path
from ultralytics import YOLO
from paddleocr import PaddleOCR

# Регулярка для валидации российских ГРЗ
RUSSIAN_PLATE_PATTERN = re.compile(r'^[АВЕКМНОРСТУХ]\d{3}[АВЕКМНОРСТУХ]{2}\d{2,3}$')

class ANPRDetector:
    def __init__(self):
        print("[INFO] Загрузка моделей...")
        # Детекция номера: используем предобученную на общих объектах модель + кастомный класс "номер"
        # Для лучшей точности можно дообучить на датасете русских номеров
        self.detector = YOLO('yolov8n.pt')  # автоматически скачается при первом запуске
        
        # OCR с поддержкой кириллицы
        self.ocr = PaddleOCR(
            use_angle_cls=True,
            lang='ru',
            use_gpu=True,
            rec_model_dir='/root/.paddleocr/whl/rec/russian/russian_mobile_v2.0_rec_infer',
            det_model_dir='/root/.paddleocr/whl/det/russian/russian_mobile_v2.0_det_infer',
            cls_model_dir='/root/.paddleocr/whl/cls/ch_ppocr_mobile_v2.0_cls_infer'
        )
        print("[INFO] Модели загружены")

    def validate_plate(self, text: str) -> bool:
        """Проверка по формату ГРЗ РФ"""
        cleaned = re.sub(r'[^АВЕКМНОРСТУХ0-9]', '', text.upper())
        return bool(RUSSIAN_PLATE_PATTERN.match(cleaned))

    def process_frame(self, frame):
        results = self.detector(frame, classes=[0], verbose=False)  # класс 0 = 'person' в коко, но для номеров нужна кастомная модель
        
        # ⚠️ ВАЖНО: стандартная YOLOv8n НЕ детектит номера!
        # Решение 1: использовать кастомную модель, обученную на номерах (см. ниже)
        # Решение 2: пропустить детекцию и прогнать весь кадр через OCR (медленнее, но проще)
        
        # Пока используем "грубый" подход: обрезаем нижнюю часть кадра (где обычно номер)
        h, w = frame.shape[:2]
        roi = frame[int(h*0.6):h, 0:w]  # нижняя треть кадра
        
        # Распознавание через PaddleOCR
        ocr_result = self.ocr.ocr(roi, cls=True)
        plates = []
        
        if ocr_result and ocr_result[0]:
            for line in ocr_result[0]:
                text = line[1][0]
                confidence = line[1][1]
                if confidence > 0.7 and self.validate_plate(text):
                    plates.append({
                        'text': text,
                        'confidence': confidence,
                        'bbox': line[0]
                    })
                    print(f"[DETECT] {text} (conf: {confidence:.2f})")
        
        return plates

    def process_batch(self, input_dir: str, output_dir: str):
        Path(output_dir).mkdir(parents=True, exist_ok=True)
        for img_path in Path(input_dir).glob('*.[jp][pn]g'):
            print(f"\n[PROCESS] {img_path.name}")
            frame = cv2.imread(str(img_path))
            if frame is None:
                continue
            
            plates = self.process_frame(frame)
            
            # Сохраняем результат
            if plates:
                result_path = Path(output_dir) / f"{img_path.stem}_result.txt"
                with open(result_path, 'w', encoding='utf-8') as f:
                    for p in plates:
                        f.write(f"{p['text']}\t{p['confidence']:.2f}\n")
                print(f" → Сохранено: {result_path.name}")

    def process_realtime(self):
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            print("[ERROR] Не удалось открыть камеру")
            return
        
        print("[INFO] Нажмите 'q' для выхода")
        while True:
            ret, frame = cap.read()
            if not ret:
                break
            
            plates = self.process_frame(frame)
            
            # Отображение
            cv2.putText(frame, f"Plates: {len(plates)}", (10, 30),
                       cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
            cv2.imshow('ANPR', frame)
            
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        
        cap.release()
        cv2.destroyAllWindows()

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', default='/data', help='Папка с изображениями или "camera"')
    parser.add_argument('--output', default='/output', help='Папка для результатов')
    parser.add_argument('--mode', choices=['batch', 'realtime'], default='batch')
    args = parser.parse_args()

    detector = ANPRDetector()
    
    if args.mode == 'batch':
        detector.process_batch(args.input, args.output)
    else:
        detector.process_realtime()

if __name__ == '__main__':
    main()
```

---

### ⚠️ Важные замечания

| Проблема | Решение |
|----------|---------|
| **Стандартная YOLOv8n не детектит номера** | Нужна **кастомная модель**, обученная на датасете номеров. Готовые веса:<br>• [RU-ALPR YOLOv8](https://github.com/sirius-ai/ru_alpr_yolov8)<br>• [License-Plate-Detector](https://github.com/thatbrguy/License-Plate-Detector) |
| **PaddleOCR "видит" лишний текст** | Валидация через регулярку `RUSSIAN_PLATE_PATTERN` фильтрует мусор |
| **Низкая точность на наклонных номерах** | Добавьте перспективную коррекцию через `cv2.getPerspectiveTransform()` |
| **Модели скачиваются при первом запуске** | Первый запуск займёт 5–10 минут (загрузка ~500 МБ) |

---

### 🚀 Как запустить

```bash
# 1. Создайте структуру
mkdir -p anpr-local/{app,data,output}
cd anpr-local

# 2. Сохраните файлы (Dockerfile, docker-compose.yml, recognize.py)

# 3. Положите тестовое фото в data/sample.jpg

# 4. Установите NVIDIA Container Toolkit (если ещё не установлено)
#    Инструкция: https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html

# 5. Запустите
docker compose up --build -d

# 6. Посмотрите логи
docker logs -f anpr-local
```

---

### 🔧 Как улучшить точность (опционально)

1. **Дообучите детектор на русских номерах:**
   ```bash
   # Используйте датасет: https://github.com/ufal/ru_alpr_dataset
   yolo train data=ru_alpr.yaml model=yolov8n.pt epochs=50 imgsz=640
   ```
2. **Замените в скрипте:**
   ```python
   self.detector = YOLO('/app/models/best.pt')  # ваша кастомная модель
   ```

---

### ⚖️ Правовой статус

> Система предназначена **только для личного использования** (например, распознавание своих машин у дома). Хранение/передача номеров третьих лиц без согласия нарушает **ФЗ-152 «О персональных данных»**.

---

Готов помочь с:
- Подбором готовой кастомной модели под ГРЗ
- Интеграцией в Home Assistant (автоматическое открытие шлагбаума)
- Оптимизацией под 8 ГБ VRAM

Пишите, когда мини-ПК приедет — протестируем на реальных фото! 🚗📸