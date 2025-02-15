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
```

### **2. Откройте проект в VS Code**
```bash
code .
```
### **3. Запустите контейнер**

1. Нажмите Ctrl+Shift+P → Remote-Containers: Reopen in Container.

2. Дождитесь сборки образа (3-5 минут при первом запуске).

---
   
### 🔨 **Сборка проекта**

После запуска контейнера откройте терминал в VS Code (Terminal → New Terminal) и выполните:
```bash
# Сгенерировать файлы сборки
cmake -B build -S .
```
---

🏃 ### **Запуск программы**
```bash
# Запустить исполняемый файл (замените `hello` на имя вашей программы)
./build/hello

# Собрать проект
cmake --build build
```
Пример вывода:
```bash
Hello, World!
```
---
🔄 ###**Пересборка контейнера** 

Если вы изменили Dockerfile или зависимости:

1. Нажмите Ctrl+Shift+P → Remote-Containers: Rebuild Container.
---

### 🧹 Очистка артефактов сборки

Для полной пересборки CMake:
```bash
rm -rf build && cmake -B build -S . && cmake --build build
```




```

