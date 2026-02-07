# Создайте директорию для постоянного хранения данных
mkdir -p ~/homeassistant/config

# Запустите контейнер
docker run -d \
  --name homeassistant \
  --privileged \
  --restart=unless-stopped \
  -e TZ=Europe/Moscow \
  -v ~/homeassistant/config:/config \
  -v /run/dbus:/run/dbus:ro \
  --network=host \
  ghcr.io/home-assistant/home-assistant:stable