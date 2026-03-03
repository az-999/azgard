# Подготовка окружения: Windows 11 + WSL2 для AI-стека (Ollama, Whisper, TTS, Open WebUI)

Инструкция для запуска `docker-compose` с GPU (NVIDIA) в WSL2.

---

## 1. Windows 11

### 1.1 Включить WSL2 и установить дистрибутив

В PowerShell **от имени администратора**:

```powershell
wsl --install
```

После перезагрузки по умолчанию установится Ubuntu. Либо указать дистрибутив:

```powershell
wsl --install -d Ubuntu
```

Проверка версии WSL (должно быть 2):

```powershell
wsl -l -v
```

### 1.2 Драйвер NVIDIA для WSL2

1. Скачать **последний драйвер** с [nvidia.com/drivers](https://www.nvidia.com/Download/index.aspx) для вашей видеокарты (для RTX 50xx — актуальный Game Ready или Studio).
2. Установить драйвер в Windows.  
   Для видеокарт с поддержкой WSL2 в драйвер уже встроена поддержка GPU в WSL — отдельный пакет «CUDA on WSL» не обязателен для контейнеров с образом Ollama.

---

## 2. WSL2 (Ubuntu)

Запустить WSL (из меню Пуск или `wsl` в PowerShell/CMD) и выполнять команды ниже в терминале Ubuntu.

### 2.1 Обновление системы

```bash
sudo apt update && sudo apt upgrade -y
```

### 2.2 Установка Docker

Официальный способ через репозиторий Docker:

```bash
# Зависимости
sudo apt install -y ca-certificates curl

# Ключ и репозиторий Docker
sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo chmod a+r /etc/apt/keyrings/docker.gpg

echo "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt update
sudo apt install -y docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
```

Добавить своего пользователя в группу `docker` (чтобы не писать `sudo` перед каждой командой):

```bash
sudo usermod -aG docker $USER
```

Применить группу без перезагрузки (выйти из WSL и зайти снова или выполнить):

```bash
newgrp docker
```

### 2.3 NVIDIA Container Toolkit

Чтобы контейнеры видели GPU:

```bash
# Репозиторий NVIDIA
curl -fsSL https://nvidia.github.io/libnvidia-container/gpgkey | sudo gpg --dearmor -o /usr/share/keyrings/nvidia-container-toolkit-keyring.gpg
curl -s -L https://nvidia.github.io/libnvidia-container/stable/deb/nvidia-container-toolkit.list | \
  sed 's#deb https://#deb [signed-by=/usr/share/keyrings/nvidia-container-toolkit-keyring.gpg] https://#g' | \
  sudo tee /etc/apt/sources.list.d/nvidia-container-toolkit.list

sudo apt update
sudo apt install -y nvidia-container-toolkit
```

Настроить Docker на использование рантайма NVIDIA:

```bash
sudo nvidia-ctk runtime configure --runtime=docker
```

Перезапустить Docker:

```bash
sudo systemctl restart docker
```

Если в WSL нет `systemctl` (в некоторых установках по умолчанию его нет):

```bash
sudo service docker restart
```

---

## 3. Проверка

### 3.1 GPU видна в WSL2

```bash
nvidia-smi
```

Должна отображаться ваша видеокарта (например, GeForce RTX 5070 Ti) и версия драйвера.

### 3.2 Docker видит GPU

```bash
docker run --rm --gpus all nvidia/cuda:12.0-base nvidia-smi
```

Та же видеокарта должна быть видна внутри контейнера. После проверки образ можно удалить:

```bash
docker rmi nvidia/cuda:12.0-base
```

---

## 4. Запуск AI-стека

Перейти в каталог с проектом (в WSL):

```bash
cd /mnt/c/Users/<ВАШ_ПОЛЬЗОВАТЕЛЬ>/path/to/azgard/ai
```

Либо, если репозиторий уже в файловой системе WSL (например, `~/azgard/ai`):

```bash
cd ~/azgard/ai
```

Запуск (файл у вас назван `docker-compoase.yml` — при необходимости замените на своё имя файла):

```bash
docker compose -f docker-compoase.yml up -d
```

При первом запуске контейнер `ollama-pull` скачает модель Qwen3-8B (это может занять время). Остальные сервисы поднимутся параллельно.

Проверка контейнеров:

```bash
docker compose -f docker-compoase.yml ps
```

- **Open WebUI:** http://localhost:3000  
- **Ollama API:** http://localhost:11434  

Остановка:

```bash
docker compose -f docker-compoase.yml down
```

---

## 5. Частые проблемы

| Проблема | Что проверить |
|----------|----------------|
| `nvidia-smi` в WSL не находит GPU | Драйвер установлен в Windows; перезагрузка ПК после установки драйвера; в диспетчере устройств нет ошибок по видеокарте. |
| `docker run --gpus all ...` падает с ошибкой про GPU | В WSL установлен `nvidia-container-toolkit`, выполнен `nvidia-ctk runtime configure --runtime=docker`, Docker перезапущен. |
| В контейнере Ollama модель не на GPU | Убедиться по логам: `docker logs ollama`. В host должен работать `nvidia-smi` и тестовый контейнер с `--gpus all` показывать GPU. |
| После перезагрузки Windows Docker в WSL не стартует | В WSL выполнить `sudo service docker start` или включить автозапуск сервиса (зависит от настроек WSL). |

---

## 6. Краткий чеклист

- [ ] Windows 11: WSL2 установлен (`wsl --install`), дистрибутив (Ubuntu) установлен.
- [ ] Windows: установлен актуальный драйвер NVIDIA.
- [ ] WSL: установлены Docker Engine и Docker Compose plugin.
- [ ] WSL: установлен NVIDIA Container Toolkit, выполнен `nvidia-ctk runtime configure --runtime=docker`.
- [ ] WSL: `nvidia-smi` показывает видеокарту.
- [ ] WSL: `docker run --rm --gpus all nvidia/cuda:12.0-base nvidia-smi` показывает видеокарту.
- [ ] Запуск: `docker compose -f docker-compoase.yml up -d`, открыть http://localhost:3000.
