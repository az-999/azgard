"""
Прогресс скачивания с Hugging Face Hub: логи в консоль + состояние для /api/model-status.
"""

from __future__ import annotations

import logging
import os
import threading
import time
from typing import Any, Optional

from tqdm import tqdm

logger = logging.getLogger(__name__)

_progress_lock = threading.Lock()
_progress: dict[str, Any] = {
    "phase": "idle",
    "message": "",
    "percent": None,
    "downloaded_bytes": 0,
    "total_bytes": None,
    "current_file": "",
}

# throttling логов по времени и по процентам
_log_state = {"t": 0.0, "pct": -1.0}
_LOG_INTERVAL_SEC = 2.0
_LOG_PCT_STEP = 2.0


def _default_message_for_phase(phase: str) -> str:
    return {
        "idle": "Инициализация…",
        "downloading": "Скачивание с Hugging Face…",
        "download_done": "Файлы в кеше, дальше — токенизатор…",
        "tokenizer": "Загрузка токенизатора…",
        "openvino": "Загрузка / компиляция OpenVINO (проценты для этого этапа недоступны, см. логи)…",
        "done": "Готово",
        "error": "Ошибка загрузки",
    }.get(phase, f"Этап: {phase}")


def get_download_progress() -> dict[str, Any]:
    """Всегда отдаёт непустой display_message для UI, даже если message забыли задать."""
    with _progress_lock:
        out = dict(_progress)
    msg = (out.get("message") or "").strip()
    if not msg:
        msg = _default_message_for_phase(str(out.get("phase", "idle")))
    out["display_message"] = msg
    return out


def set_phase(phase: str, message: str = "") -> None:
    with _progress_lock:
        _progress["phase"] = phase
        _progress["message"] = message
        if phase in ("done", "idle", "error"):
            _progress["percent"] = None
            _progress["downloaded_bytes"] = 0
            _progress["total_bytes"] = None
            _progress["current_file"] = ""


def _update_progress_from_bar(
    desc: str,
    n: float,
    total: Optional[float],
) -> None:
    with _progress_lock:
        _progress["phase"] = "downloading"
        _progress["current_file"] = desc or "файлы"
        n_int = int(n)
        total_int = int(total) if total is not None else None
        _progress["downloaded_bytes"] = n_int
        _progress["total_bytes"] = total_int
        if total_int and total_int > 0:
            _progress["percent"] = round(100.0 * n_int / total_int, 2)
            _progress["message"] = (
                f"Скачивание: {_progress['percent']}% "
                f"({_fmt_bytes(n_int)} / {_fmt_bytes(total_int)}) — {desc or 'hub'}"
            )
        else:
            _progress["percent"] = None
            _progress["message"] = f"Скачивание: {_fmt_bytes(n_int)} — {desc or 'hub'}"


def _fmt_bytes(n: int) -> str:
    x = float(n)
    units = ("Б", "КБ", "МБ", "ГБ", "ТБ")
    for i, unit in enumerate(units):
        if x < 1024 or unit == "ТБ":
            return f"{int(x)} {unit}" if i == 0 else f"{x:.1f} {unit}"
        x /= 1024.0
    return f"{n} Б"


def _throttled_log(desc: str, n: float, total: Optional[float]) -> None:
    global _log_state
    now = time.monotonic()
    pct: float
    if total and total > 0:
        pct = 100.0 * float(n) / float(total)
    else:
        pct = -1.0

    t_ok = now - _log_state["t"] >= _LOG_INTERVAL_SEC
    pct_ok = pct >= 0 and (
        _log_state["pct"] < 0 or abs(pct - _log_state["pct"]) >= _LOG_PCT_STEP
    )
    if not (t_ok or pct_ok):
        return

    _log_state["t"] = now
    if pct >= 0:
        _log_state["pct"] = pct

    if total and total > 0:
        logger.info(
            "HF: %.1f%% (%s / %s) %s",
            pct,
            _fmt_bytes(int(n)),
            _fmt_bytes(int(total)),
            desc or "",
        )
    else:
        logger.info("HF: %s загружено %s", _fmt_bytes(int(n)), desc or "")


class LoggingTqdm(tqdm):
    """tqdm для huggingface_hub: обновляет глобальный прогресс и пишет в лог."""

    def update(self, n: int = 1) -> bool:
        r = super().update(n)
        try:
            desc = str(self.desc or "")
            _update_progress_from_bar(desc, self.n, self.total if self.total else None)
            _throttled_log(desc, self.n, self.total if self.total else None)
        except Exception:
            logger.debug("progress update failed", exc_info=True)
        return bool(r)

    def refresh(self, nolock: bool = False, lock_args=None) -> None:
        super().refresh(nolock=nolock, lock_args=lock_args)
        try:
            desc = str(self.desc or "")
            _update_progress_from_bar(desc, self.n, self.total if self.total else None)
        except Exception:
            pass


def is_hub_model_id(model_id: str) -> bool:
    """Грубая проверка: похоже на repo_id на HF, а не на локальный путь."""
    mid = model_id.strip()
    if not mid or "/" not in mid:
        return False
    if mid.startswith(("/", ".", "~")):
        return False
    if os.path.isdir(mid):
        return False
    # Windows drive e.g. C:\
    if len(mid) > 1 and mid[1] == ":":
        return False
    return True


def snapshot_download_with_progress(
    repo_id: str,
    *,
    cache_dir: str | None,
    token: str | None,
) -> None:
    """
    Скачивает снимок репозитория с отображением прогресса (tqdm_class).
    Если репозиторий уже в кеше, будет быстро.
    """
    from huggingface_hub import snapshot_download

    logger.info("Начало snapshot_download для %s (кеш: %s)", repo_id, cache_dir)
    set_phase("downloading", f"Скачивание репозитория {repo_id}…")

    snapshot_download(
        repo_id=repo_id,
        cache_dir=cache_dir,
        token=token,
        tqdm_class=LoggingTqdm,
    )

    set_phase("download_done", "Файлы репозитория в кеше")
    logger.info("snapshot_download завершён для %s", repo_id)
