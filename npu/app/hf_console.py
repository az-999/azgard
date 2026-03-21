"""
Консольные команды для работы с кешем Hugging Face (без веб‑сервера).

  # Скачать репозиторий — в терминале стандартные полосы tqdm (файлы / байты)
  export HF_HOME="$PWD/hf-cache"
  export MODEL_ID="OpenVINO/Qwen2.5-7B-Instruct-int4-ov"
  python -m app.hf_console download

  # Удалить только эту модель из кеша (потом снова download)
  python -m app.hf_console reset --yes

  # Полностью очистить HF_HOME (все модели в этом каталоге)
  python -m app.hf_console reset --all --yes

Docker:

  docker run --rm -it -e MODEL_ID=... -v "$PWD/npu/hf-cache:/app/hf-cache" ov-qwen2-assistant \\
    python -m app.hf_console download
"""

from __future__ import annotations

import argparse
import logging
import os
import re
import shutil
import sys
from pathlib import Path
from typing import Any, Type

from tqdm import tqdm

_log = logging.getLogger("app.hf_console")


def _setup_download_logging(verbose: bool) -> None:
    level = logging.DEBUG if verbose else logging.INFO
    logging.basicConfig(
        level=level,
        format="%(asctime)s %(levelname)s %(message)s",
        stream=sys.stderr,
        force=True,
    )
    _log.setLevel(level)
    # Подробные запросы/пути внутри huggingface_hub
    logging.getLogger("huggingface_hub").setLevel(logging.DEBUG if verbose else logging.WARNING)
    logging.getLogger("urllib3").setLevel(logging.WARNING)


def _looks_like_hf_filename(desc: str) -> bool:
    d = desc.strip()
    if not d or len(d) > 512:
        return False
    if d.startswith("Fetching ") or "it/s]" in d:
        return False
    # «Сводный» прогресс по файлам
    if re.match(r"^Fetching\s+\d+\s+files", d):
        return False
    return True


def _hf_resolve_url(model_id: str, relative_path: str) -> str:
    rel = relative_path.strip().lstrip("/")
    return f"https://huggingface.co/{model_id}/resolve/main/{rel}"


def _make_verbose_tqdm(model_id: str) -> Type[tqdm]:
    """tqdm с логами: что качается + прямой URL для файлов (ветка main)."""

    class FilenameLoggingTqdm(tqdm):
        def __init__(self, *args: Any, **kwargs: Any) -> None:
            super().__init__(*args, **kwargs)
            self._vf_logged_initial = False

        def _vf_emit(self) -> None:
            desc = (self.desc or "").strip()
            if not desc:
                return
            _log.info("── Загрузка / этап: %s", desc)
            if _looks_like_hf_filename(desc):
                url = _hf_resolve_url(model_id, desc)
                _log.info("   Прямая ссылка (main): %s", url)
            total = self.total
            if total is not None:
                _log.info("   Прогресс tqdm: %s / %s единиц (часто файлы или чанки, не всегда байты)", self.n, total)

        def update(self, n: int = 1) -> bool:
            if not self._vf_logged_initial and (self.desc or "").strip():
                self._vf_emit()
                self._vf_logged_initial = True
            r = super().update(n)
            return bool(r)

        def refresh(self, nolock: bool = False, lock_args: Any = None) -> None:
            d = (self.desc or "").strip()
            if d and d != getattr(self, "_vf_last_desc", ""):
                self._vf_last_desc = d
                self._vf_logged_initial = False
                self._vf_emit()
                self._vf_logged_initial = True
            super().refresh(nolock=nolock, lock_args=lock_args)

    return FilenameLoggingTqdm


def _cache_dir() -> Path:
    from .config import get_settings

    s = get_settings()
    if s.cache_dir:
        return Path(s.cache_dir)
    return Path(os.path.expanduser("~/.cache/huggingface"))


def _model_id(cli_id: str | None) -> str:
    from .config import get_settings

    return (cli_id or get_settings().model_id).strip()


def _hub_models_prefix(repo_id: str) -> str:
    return "models--" + repo_id.replace("/", "--")


def cmd_download(args: argparse.Namespace) -> int:
    """Только snapshot с HF — в консоли tqdm + логи путей/URL."""
    from huggingface_hub import snapshot_download

    _setup_download_logging(verbose=bool(getattr(args, "verbose", False)))

    cache = _cache_dir()
    model_id = _model_id(args.model_id)
    from .config import get_settings

    token = get_settings().hf_token

    hub = cache / "hub"
    models_dir = hub / _hub_models_prefix(model_id)

    _log.info("HF_HOME (кеш): %s", cache.resolve())
    _log.info("Страница репозитория: https://huggingface.co/%s", model_id)
    _log.info("Ожидаемый каталог модели в кеше: %s", models_dir)
    _log.info("(появится после первой успешной загрузки; blobs лежат под %s)", hub)
    if getattr(args, "verbose", False):
        _log.info("Режим --verbose: DEBUG для huggingface_hub (запросы, редиректы и т.д.)")

    print("Ниже — tqdm; строки с INFO — текущий файл и URL для ручной проверки.\n", file=sys.stderr)

    cache.mkdir(parents=True, exist_ok=True)

    tqdm_cls: Type[tqdm] = _make_verbose_tqdm(model_id) if not args.no_log_files else tqdm

    try:
        snapshot_download(
            repo_id=model_id,
            cache_dir=str(cache),
            token=token,
            tqdm_class=tqdm_cls,
        )
    except Exception as e:
        print(f"Ошибка: {e}", file=sys.stderr)
        return 1

    print("\nOK: snapshot в кеше. Для полной инициализации OpenVINO: python -m app.preload", file=sys.stderr)
    return 0


def cmd_reset(args: argparse.Namespace) -> int:
    """Удаление кеша модели или всего HF_HOME."""
    if not args.yes:
        print(
            "Укажите -y / --yes для подтверждения удаления.\n"
            "  python -m app.hf_console reset --yes           — только текущая MODEL_ID\n"
            "  python -m app.hf_console reset --all --yes     — весь каталог HF_HOME",
            file=sys.stderr,
        )
        return 2

    cache = _cache_dir()
    model_id = _model_id(args.model_id)

    if args.all:
        if cache.exists():
            shutil.rmtree(cache)
        cache.mkdir(parents=True, exist_ok=True)
        print(f"OK: полностью очищен каталог {cache}", file=sys.stderr)
        return 0

    hub = cache / "hub"
    if not hub.is_dir():
        print(f"Нет {hub} — нечего удалять.", file=sys.stderr)
        return 0

    prefix = _hub_models_prefix(model_id)
    removed: list[Path] = []
    for p in sorted(hub.iterdir()):
        if p.is_dir() and p.name.startswith(prefix):
            shutil.rmtree(p)
            removed.append(p)

    if not removed:
        print(
            f"Каталоги для «{model_id}» (префикс {prefix!r}) не найдены в {hub}. "
            f"Проверьте MODEL_ID или используйте --all --yes.",
            file=sys.stderr,
        )
        return 0

    print(f"OK: удалено каталогов: {len(removed)}", file=sys.stderr)
    for p in removed[:30]:
        print(f"  - {p}", file=sys.stderr)
    if len(removed) > 30:
        print(f"  … и ещё {len(removed) - 30}", file=sys.stderr)
    return 0


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(
        description="HF-кеш: скачивание с прогрессом в консоли и сброс.",
    )
    sub = parser.add_subparsers(dest="command", required=True)

    p_dl = sub.add_parser(
        "download",
        help="Скачать репозиторий модели в HF-кеш (прогресс tqdm в терминале).",
    )
    p_dl.add_argument(
        "--model-id",
        default=None,
        help="Переопределить MODEL_ID (иначе из окружения).",
    )
    p_dl.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="DEBUG от huggingface_hub (сетевые детали).",
    )
    p_dl.add_argument(
        "--no-log-files",
        action="store_true",
        help="Не писать в лог имена файлов и URL (только обычный tqdm).",
    )
    p_dl.set_defaults(func=cmd_download)

    p_rs = sub.add_parser(
        "reset",
        help="Удалить кеш текущей модели или весь HF_HOME.",
    )
    p_rs.add_argument(
        "--model-id",
        default=None,
        help="Переопределить MODEL_ID при удалении только этой модели.",
    )
    p_rs.add_argument(
        "--all",
        action="store_true",
        help="Удалить весь каталог HF_HOME (все модели в этом пути).",
    )
    p_rs.add_argument(
        "-y",
        "--yes",
        action="store_true",
        help="Подтверждение удаления.",
    )
    p_rs.set_defaults(func=cmd_reset)

    args = parser.parse_args(argv)
    return int(args.func(args))


if __name__ == "__main__":
    raise SystemExit(main())
