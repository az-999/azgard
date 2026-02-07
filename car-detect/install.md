# 1. Создайте структуру
mkdir -p anpr-local/{app,data,output}
cd anpr-local

# 2. Сохраните файлы (Dockerfile, docker-compose.yml, recognize.py)

# 3. Положите тестовое фото в data/sample.jpg

# 4. Установите NVIDIA Container Toolkit (если ещё не установлено)
#    Инструкция: https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/latest/install-guide.html

# 5. Запустите
docker compose up --build -d

# 6. Посмотрите логи
docker logs -f anpr-local