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
---
## Задание 5

### для запуска сервера нужно ввести комманду 
```bash
./build/workspace
```
### пример ответа на любой запрос:
```bash
curl -X POST http://localhost:8080 -H "X-Test: true" -d '{"message": "Test"}'
```
```bash
Method: POST
Path: /
Headers:
  LOCAL_ADDR: 127.0.0.1
  REMOTE_ADDR: 127.0.0.1
  Content-Type: application/x-www-form-urlencoded
  X-Test: true
  REMOTE_PORT: 43990
  Accept: */*
  LOCAL_PORT: 8080
  Content-Length: 19
  User-Agent: curl/8.5.0
  Host: localhost:8080
Body:
{"message": "Test"}
```
---
### Задание 6
принимает любое выражение и выдает в ответ 

input
```bash
(-3+4)*5
```
output
```bash
5
```
---
### Задание 7

После включение сервера можно отправить запрос в формате 
```bash
curl -X POST http://localhost:8080/api/calculate   -H "Content-Type: application/json"   -d '{"exp":"(3 + 4) * 2"}'
```
output
```bash
{"res":14.0}
```
---

### Задание 8

Теперь для отправки запроса с клиента нужно сначала запустить сервер коммандой
```bash
./build/workspace 
```
После этого с другого терминала сервер может принять запрос в виде
```bash
./build/calc_client -e 2 + 2

./build/calc_client -c echo

```

```bash
4.0

"echo"

```
---
### Задание 9
После запуска сервера теперь клиент может формировать такие запросы:
```bash
./build/calc_client -e "var = 2 + 3; \
var * 2"

```
c ответом 
```bash
10.0
```
---
### Задание 10
```bash
./build/calc_client -e x=2
"OK"
./build/calc_client -e x
2.0
./build/calc_client -e "var = 2 + 3; \
var * 2"
10.0
./build/calc_client -e var
5.0
./build/calc_client -c clean
"OK"
./build/calc_client -e x
Request failed. Status: 400

```




