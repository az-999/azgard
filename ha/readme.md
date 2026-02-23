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

3. **Пересоздать контейнер после правок:**
   ```bash
   cd /path/to/ha
   docker compose down
   docker compose up -d
   ```
   Затем в HA: Настройки → Устройства и службы → Bluetooth — удалить проблемную запись и заново добавить интеграцию Bluetooth (или перезапустить HA).

### Если адаптер по-прежнему «not found»

- На RPi5 иногда помогают обновление прошивки и ядра: `sudo rpi-update` (осторожно, тестовые ядра) или, наоборот, откат на более стабильное ядро.
- Альтернатива без прямого доступа к адаптеру: [Bluetooth proxy на ESP32](https://esphome.io/components/bluetooth_proxy.html) — HA подключается к прокси по Wi‑Fi, Bluetooth работает на ESP32.