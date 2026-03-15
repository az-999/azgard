Home Assistant установлен на Raspberry Pi 5 4Gb на Raspberry Pi OS Desktop.

## Bluetooth

В `docker-compose.yml` для контейнера Home Assistant добавлены:
- **cap_add:** `NET_ADMIN`, `NET_RAW` — права для управления Bluetooth;
- **volume:** `/run/dbus:/run/dbus:ro` — доступ к D-Bus хоста (через него HA общается с BlueZ и адаптером).

### Что проверить на Raspberry Pi (хост)

1. **BlueZ установлен и сервис запущен:**
   ```bash
   sudo apt-get install -y bluez
   sudo systemctl status bluetooth
   ```
   Должен быть `active (running)`.

2. **Адаптер виден в системе:**
   ```bash
   bluetoothctl show
   ```
   Должен отображаться контроллер (например, hci0 с адресом 2C:CF:67:6F:78:FB).

3. **Пересоздать контейнер после правок (обязательно — иначе cap_add не применится):**
   ```bash
   cd ~/azgard/ha
   docker compose down
   docker compose up -d
   ```
   Затем в HA: Настройки → Устройства и службы → Bluetooth — при необходимости удалить запись и заново добавить интеграцию Bluetooth.

### Если адаптер по-прежнему «not found»

- На RPi5 иногда помогают обновление прошивки и ядра: `sudo rpi-update` (осторожно, тестовые ядра) или, наоборот, откат на более стабильное ядро.
- Альтернатива без прямого доступа к адаптеру: [Bluetooth proxy на ESP32](https://esphome.io/components/bluetooth_proxy.html) — HA подключается к прокси по Wi‑Fi, Bluetooth работает на ESP32.

## ESPHome

ESPHome запущен отдельным контейнером (в Docker-установке HA аддон недоступен).

- **Дашборд:** после `docker compose up -d` откройте в браузере `http://<IP_хоста>:6052`.
- **Логин/пароль:** по умолчанию `USERNAME=admin`, `PASSWORD=admin` — см. `docker-compose.yml`, секция `esphome`; лучше сменить в переменных окружения.
- **Конфиги:** хранятся в каталоге `ha/esphome/` (YAML-файлы устройств).
- **Секреты:** общий файл `ha/config/secrets.yaml` (и для HA, и для ESPHome); в контейнер ESPHome он монтируется как `/config/secrets.yaml`.
- **Прошивка по USB:** раскомментируйте в `docker-compose.yml` блок `devices` и укажите свой порт (например `/dev/ttyUSB0`), затем пересоберите контейнеры.
- В Home Assistant добавьте интеграцию **ESPHome** (Настройки → Устройства и службы) — устройства, настроенные в ESPHome с API Home Assistant, появятся автоматически после прошивки.