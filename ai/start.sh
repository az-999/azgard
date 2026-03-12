# 1. Установите NVIDIA Container Toolkit
curl -fsSL https://nvidia.github.io/libnvidia-container/gpgkey | sudo gpg --dearmor -o /usr/share/keyrings/nvidia-container-toolkit-keyring.gpg
curl -s -L https://nvidia.github.io/libnvidia-container/stable/deb/nvidia-container-toolkit.list | \
  sed 's#deb https://#deb [signed-by=/usr/share/keyrings/nvidia-container-toolkit-keyring.gpg] https://#g' | \
  sudo tee /etc/apt/sources.list.d/nvidia-container-toolkit.list
sudo apt-get update && sudo apt-get install -y nvidia-container-toolkit
sudo nvidia-ctk runtime configure --runtime=docker
sudo systemctl restart docker

# 2. Запустите стек
docker compose up -d

# 3. Скачать модель Qwen3-8B в Ollama (один раз, после старта ollama)
docker exec ollama ollama pull qwen3:8b
docker exec ollama ollama pull mychen76/qwen3_cline_roocode:
docker exec ollama ollama pull deepcoder:14b
docker exec ollama ollama pull qwen2.5-coder:14b
docker exec ollama ollama pull qwen3-coder:30b
docker exec ollama ollama pull deepseek-coder:6.7b

# 4. Проверьте GPU
docker exec -it ollama nvidia-smi