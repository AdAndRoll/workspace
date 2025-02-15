# Используем базовый образ с Ubuntu
FROM ubuntu:latest

# Обновляем пакеты и устанавливаем необходимые инструменты для C/C++ разработки
RUN apt-get update && \
    apt-get install -y build-essential gdb cmake git

# Устанавливаем рабочую директорию внутри контейнера
WORKDIR /workspace

# Команда по умолчанию при запуске контейнера
CMD ["bash"]