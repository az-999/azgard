# azgard

Переезд с HAOS на Raspberry Pi OS + HA
https://chat.qwen.ai/s/t_fa450e6f-b637-4906-9d01-b1a80fc82446?fev=0.2.7

Газовое отопление и газовая колонка
https://chat.qwen.ai/s/aa35116e-1f4d-4b60-a00a-635d20f2910a?fev=0.2.9

sudo chown -R $USER:$USER /home/az/homeassistant/config

usb_max_current_enable=1

# install

sudo apt update && sudo apt upgrade -y

curl -fsSL https://get.docker.com -o get-docker.sh && sudo sh get-docker.sh

sudo usermod -aG docker $USER

sudo reboot

sudo apt update
sudo apt install docker-compose-plugin -y

# Газ Отопление

Отопление газовыми баллонами
https://www.youtube.com/watch?v=CvX5JjRaWrg

газовый распределительный коллектор или Рампа своими руками на 6 баллонов часть 2
https://www.youtube.com/watch?v=bQ6oSc6pbvE

Насосно смесительный узел теплого пола.
https://www.youtube.com/watch?v=FT1EKE4JeRQ

Рампа для газовых баллонов частного дома. Увеличиваем количество баллонов газового котла
https://www.youtube.com/watch?v=9Y3lI8RSbR4