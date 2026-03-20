## Локальный NPU‑ассистент на OpenVINO

Этот каталог содержит проект чат‑ассистента на базе модели **Qwen2‑7B‑Instruct**, оптимизированной через **OpenVINO / Optimum‑Intel** для работы на Beelink GTi15 Ultra (Intel Core Ultra 9 285H, Intel Arc, NPU).

### Структура

- `app/` — код FastAPI‑бэкенда и web‑интерфейса
  - `app/main.py` — API (`/api/chat`) и раздача статики `/static`
  - `app/model.py` — загрузка модели через `OVModelForCausalLM`
  - `app/preload.py` — **отдельная команда** предзагрузки модели в кеш (без веб‑сервера)
  - `app/config.py` — настройки (MODEL_ID, OV_DEVICE и т.п.)
  - `app/static/index.html` — SPA‑чат
- `requirements.txt` — Python‑зависимости
- `Dockerfile` — образ с OpenVINO‑ассистентом

### Зависимости (локальный запуск без Docker)

```bash
cd /home/svyat/azgard/npu
python -m venv .venv
source .venv/bin/activate
pip install --upgrade pip
pip install -r requirements.txt
```

Переменные окружения (значения по умолчанию можно не задавать, они уже прописаны в коде/образе). Кеш моделей Hugging Face хранится в **npu/hf-cache**:

```bash
export MODEL_ID="Qwen/Qwen2-7B-Instruct"       # или, например: OpenVINO/Qwen2.5-7B-Instruct-int4-ov
export OV_DEVICE="AUTO"                        # AUTO | CPU | GPU | NPU
export HF_HOME="$PWD/hf-cache"                 # при запуске из npu — кеш в npu/hf-cache
```

Запуск сервера:

```bash
uvicorn app.main:app --host 0.0.0.0 --port 8000
```

Открыть в браузере: `http://localhost:8000/static/`.

### Предзагрузка модели (отдельно, в консоли)

Чтобы **спокойно скачать и подготовить** модель до запуска UI, используйте ту же `MODEL_ID` / `HF_HOME` / `OV_DEVICE`, что и у сервера:

```bash
cd /path/to/azgard/npu
export HF_HOME="$PWD/hf-cache"
export MODEL_ID="OpenVINO/Qwen2.5-7B-Instruct-int4-ov"
export OV_DEVICE="CPU"   # в Docker без NPU надёжнее CPU
python -m app.preload
```

Либо скрипт:

```bash
cd /path/to/azgard/npu
chmod +x preload-model.sh   # один раз
./preload-model.sh
```

После строки `OK: модель скачана/подготовлена` можно запускать `uvicorn` — первый ответ в чате не будет ждать полной загрузки с нуля (остаётся только инициализация в памяти процесса).

### Запуск через Docker

Сборка образа:

```bash
cd /home/svyat/azgard/npu
docker build -t ov-qwen2-assistant .
```

**Предзагрузка в Docker** (один раз, пока качается в `npu/hf-cache`; порт 8000 не нужен):

```bash
# из корня репозитория azgard
docker run --rm \
  -e MODEL_ID="OpenVINO/Qwen2.5-7B-Instruct-int4-ov" \
  -e OV_DEVICE="CPU" \
  -v "$PWD/npu/hf-cache:/app/hf-cache" \
  ov-qwen2-assistant \
  python -m app.preload
```

Затем обычный запуск контейнера с тем же монтированием `hf-cache` (см. ниже).

Запуск (минимальная команда). Кеш моделей — **npu/hf-cache**.

Из корня репозитория (каталог `azgard`):

```bash
docker run --rm \
  -p 8000:8000 \
  -e MODEL_ID="OpenVINO/Qwen2.5-7B-Instruct-int4-ov" \
  -e OV_DEVICE="CPU" \
  -v "$PWD/npu/hf-cache:/app/hf-cache" \
  --name ov-qwen2-assistant \
  ov-qwen2-assistant
```

Либо из каталога `npu`: тогда монтирование кеша — `-v "$PWD/hf-cache:/app/hf-cache"` (путь на хосте снова npu/hf-cache).

Рекомендации:

- Для ускорения старта и снижения потребления памяти можно использовать уже конвертированную модель:
  - `MODEL_ID="OpenVINO/Qwen2.5-7B-Instruct-int4-ov"`
- Для явного выбора устройства:
  - `OV_DEVICE="CPU"` — только CPU
  - `OV_DEVICE="GPU"` — Intel Arc
  - `OV_DEVICE="NPU"` — попытка использовать NPU (если поддерживается установленным OpenVINO и драйверами)

После запуска открыть `http://localhost:8000/static/` и общаться с ассистентом.

