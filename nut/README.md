# UPS Controller (ESP32‑C3) + Diagflow

Конфигурация прошивки: [`esphome.yml`](./esphome.yml) — **ESPHome** для платы **ESP32‑C3 Super Mini**, интеграция с **Home Assistant**.

## Что умеет `esphome.yml`

| Сущность в HA | Назначение |
|---------------|------------|
| **UPS Сеть 220В** (`binary_sensor`) | Наличие сети по выходу оптопарного модуля 220 В (GPIO из `grid_gpio`, по умолчанию `GPIO20`). |
| **UPS ВКЛ/ВЫКЛ** (`button`) | Одно нажатие в HA = имитация удержания физической кнопки UPS **3 секунды** через реле (HIGH на GPIO, пин `relay_gpio`, по умолчанию `GPIO21`). |
| **UPS Кнопка (реле)** (`switch`, internal) | Сервисный переключатель; в обычной работе скрыт в HA, управляется скриптом. |

Перед прошивкой задайте реальные GPIO в `substitutions` и при необходимости скорректируйте `inverted` у датчика и реле под вашу схему.

## Сборка и прошивка

1. В каталоге с `esphome.yml` положите `secrets.yaml` (или используйте общий путь секретов ESPHome) с ключами Wi‑Fi и `api_encryption_key`, как в вашей установке HA/ESPHome.
2. Скомпилируйте и прошейте устройство (USB или [ESPHome Web](https://web.esphome.io/) — для первого раза обычно нужен **Factory** образ).
3. Добавьте узел в Home Assistant (тот же encryption key, что в секретах).

## Файлы

| Файл | Описание |
|------|----------|
| `esphome.yml` | Прошивка ESPHome для UPS |
| `ups-controller-c3.factory.bin` | Собранный factory-образ (опционально, не обязательно хранить в git) |
