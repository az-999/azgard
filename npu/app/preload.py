"""
Предзагрузка модели в HF-кеш и инициализация OpenVINO **без** запуска веб-сервера.

Те же переменные, что у приложения: MODEL_ID, OV_DEVICE, HF_HOME, HF_TOKEN.

Примеры:

  # из каталога npu (локально, venv активирован)
  export HF_HOME=\"$PWD/hf-cache\"
  export MODEL_ID=\"OpenVINO/Qwen2.5-7B-Instruct-int4-ov\"
  export OV_DEVICE=\"CPU\"
  python -m app.preload

  # тот же образ, что и у сервера — только команда другая
  docker run --rm \\
    -e MODEL_ID=\"OpenVINO/Qwen2.5-7B-Instruct-int4-ov\" \\
    -e OV_DEVICE=\"CPU\" \\
    -v \"\\$PWD/hf-cache:/app/hf-cache\" \\
    ov-qwen2-assistant python -m app.preload
"""

from __future__ import annotations

import argparse
import logging
import sys


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="Скачать модель в кеш и подготовить OpenVINO (без uvicorn).",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Подробные логи (DEBUG).",
    )
    args = parser.parse_args(argv)

    level = logging.DEBUG if args.verbose else logging.INFO
    logging.basicConfig(
        level=level,
        format="%(asctime)s %(levelname)s %(name)s %(message)s",
    )
    log = logging.getLogger("app.preload")

    log.info("Старт предзагрузки (см. MODEL_ID, OV_DEVICE, HF_HOME в окружении).")

    try:
        from .model import load_model

        load_model()
    except Exception:
        log.exception("Предзагрузка завершилась с ошибкой.")
        return 1

    print(
        "OK: модель скачана/подготовлена. Можно запускать сервер: "
        "uvicorn app.main:app --host 0.0.0.0 --port 8000",
        file=sys.stderr,
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
