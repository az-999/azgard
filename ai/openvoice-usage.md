# OpenVoice: голос ассистента по вашему образцу

Чтобы ассистент говорил **вашим голосом** (или любым голосом по образцу аудио), используется OpenVoice: вы загружаете короткую запись голоса, затем указываете её имя в настройках TTS в Open WebUI.

## 1. Запуск стека

После `docker compose up -d` работают:

- **OpenVoice API:** http://localhost:5003  
- **Адаптер (OpenAI-совместимый TTS):** http://localhost:5010  

## 2. Загрузка образца голоса

Загрузите аудио с голосом, которым должен говорить ассистент (желательно чистый голос, 10–30 секунд, WAV или MP3).

**Через curl (с хоста):**

```bash
curl -X POST "http://localhost:5003/upload_audio/" \
  -F "file=@/path/to/your_voice.wav" \
  -F "audio_file_label=my_voice"
```

`audio_file_label` — произвольное имя (например `my_voice`, `alice`, `диктор`). Его нужно будет указать в Open WebUI как голос TTS.

**Через Python:**

```python
import requests
url = "http://localhost:5003/upload_audio/"
with open("образец.wav", "rb") as f:
    r = requests.post(url, data={"audio_file_label": "my_voice"}, files={"file": f})
print(r.json())
```

## 3. Настройка Open WebUI

1. Откройте **Настройки** → **Audio** (или Admin → Settings → Audio).
2. В блоке **Text-to-Speech** выберите движок **OpenAI** (или тот, что использует OpenAI-совместимый API).
3. Укажите:
   - **API Base URL:** `http://localhost:5010` (или `http://openvoice-adapter:5010`, если настраиваете из другого контейнера в той же сети).
   - **API Key:** можно оставить пустым или любое значение (адаптер ключ не проверяет).
   - **Voice:** имя голоса из шага 2, например `my_voice`.
4. Сохраните настройки.

После этого при запросе озвучки ответов ассистента запросы пойдут в адаптер → OpenVoice с выбранным голосом.

## 4. Встроенные голоса OpenVoice (без загрузки)

Если образец ещё не загружали, можно использовать демо-голоса OpenVoice. В поле **Voice** укажите один из:

- `demo_speaker0`
- `demo_speaker1`
- `demo_speaker2`
- `example_reference`

По умолчанию в адаптере задан `demo_speaker0` (переменная `OPENVOICE_DEFAULT_VOICE` в `docker-compose`).

## 5. Проверка синтеза напрямую

Проверка через адаптер (должен вернуться WAV):

```bash
curl -X POST "http://localhost:5010/v1/audio/speech" \
  -H "Content-Type: application/json" \
  -d '{"input": "Привет, это тест голоса.", "voice": "my_voice"}' \
  --output test.wav
```

Проверка напрямую OpenVoice:

```bash
curl "http://localhost:5003/synthesize_speech/?text=Hello&voice=my_voice" --output out.wav
```

## Важно

- Используйте только **свои** записи или записи с согласием на использование (не копируйте коммерческие голоса вроде Алисы).
- OpenVoice лучше всего работает с английским; для русского можно задать `accent` в адаптере (при необходимости добавьте переменную окружения в сервис `openvoice-adapter`).
