**Подготовка окружения (Windows 11 + WSL2, GPU):** [setup-win-wsl.md](setup-win-wsl.md)  
**Голос по образцу (OpenVoice):** [openvoice-usage.md](openvoice-usage.md)

**TTS:** используется Qwen3-TTS-12Hz-1.7B-**Base** (включён Voice Cloning). Для русского в Open WebUI модель `tts-1-ru`. Веб-интерфейс: http://127.0.0.1:5002 — там раздел **Voice Cloning**: загрузите короткий аудиообразец голоса (3–10 сек чистой речи), задайте имя профиля (например `alice`), затем в запросах указывайте голос `clone:alice`. Чтобы сделать «голос как у Алисы» — нужен образец этого голоса (запись или вырезка из ролика).  

**Если в WebUI голос «металлический»:** воспроизведение идёт через браузерный TTS (Kokoro.js). В compose заданы `AUDIO_TTS_ENGINE=openai` и URL Qwen3-TTS — оставьте в настройках Audio **«Система синтеза речи» = «По умолчанию»** (это и есть backend Qwen3-TTS). Не выбирайте «Kokoro.js (Браузер)». После смены настроек перезапустите контейнер open-webui: `docker compose up -d open-webui`.  

**GPU RTX 50xx (Blackwell, sm_120):** образ по умолчанию собран под старый PyTorch (до sm_90), поэтому TTS уходит на CPU. Чтобы задействовать видеокарту, пересоберите образ с PyTorch nightly (в каталоге `ai/`):
```bash
docker compose build qwen3-tts
docker build -f Dockerfile.qwen3-tts-sm120 -t qwen3-tts:local .
docker compose up -d qwen3-tts
```
Сборка при отключённой карте не мешает — важно, что в образе будет установлен PyTorch с поддержкой sm_120.

https://chat.qwen.ai/s/350b039f-aab6-4ba7-bbe7-a19dba65c293?fev=0.1.38


NPU
https://chat.qwen.ai/s/8240140d-84bb-404e-8be0-cc9c6d50c206?fev=0.2.9

whisper on gpu
https://chat.qwen.ai/s/t_6a3cd3b2-915b-4505-b842-7fe300df06a6?fev=0.2.9