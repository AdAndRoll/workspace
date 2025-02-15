# 🐋 C++ Development in Docker Container

Этот проект позволяет разрабатывать и запускать C++ приложения в изолированной Docker-среде.  
Используется связка **CMake** + **VS Code** + **Dev Containers**.

---

## 📋 Требования

- [Docker Desktop](https://www.docker.com/products/docker-desktop)
- [Visual Studio Code](https://code.visualstudio.com/)
- Расширение [Remote - Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

---

## 🚀 Быстрый старт

### 1. Клонируйте репозиторий
```bash
git clone https://github.com/ваш-username/ваш-репозиторий.git
cd ваш-репозиторий
2. Откройте проект в VS Code
bash
Copy
code .
3. Запустите контейнер
Нажмите Ctrl+Shift+P → Remote-Containers: Reopen in Container.

Дождитесь сборки образа (3-5 минут при первом запуске).

🔨 Сборка проекта
После запуска контейнера откройте терминал в VS Code (Terminal → New Terminal) и выполните:

bash

# Сгенерировать файлы сборки
cmake -B build -S .

# Собрать проект
cmake --build build
🏃 Запуск программы
bash

# Запустить исполняемый файл (замените `hello` на имя вашей программы)
./build/hello
Пример вывода:

Hello, World!
🔄 Пересборка контейнера
Если вы изменили Dockerfile или зависимости:

Нажмите Ctrl+Shift+P → Remote-Containers: Rebuild Container.

🧹 Очистка артефактов сборки
Для полной пересборки CMake:

bash
Copy
rm -rf build && cmake -B build -S . && cmake --build build
❓ FAQ
Как добавить новые зависимости?
Обновите Dockerfile:

dockerfile
Copy
RUN apt-get update && apt-get install -y ваш-пакет
Пересоберите контейнер.

Где найти сгенерированные файлы?
Все артефакты сборки хранятся в папке build.

Как отладить программу?
Используйте встроенный отладчик VS Code. Пример конфигурации:

json
Copy
// .vscode/launch.json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Запуск программы",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/hello",
            "args": [],
            "cwd": "${workspaceFolder}"
        }
    ]
}
