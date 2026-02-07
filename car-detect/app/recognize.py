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