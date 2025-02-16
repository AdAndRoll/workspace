#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <httplib.h>
#include <nlohmann/json.hpp>

namespace calcclient {

    inline void run_client(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " -c <command> OR -e <expression...> [-u <user>]\n";
            return;
        }

        std::string flag;
        std::string user;
        nlohmann::json request_json;

        // Разбираем аргументы, чтобы найти -u (имя пользователя) в любом месте
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "-u" && i + 1 < argc) {
                user = argv[i + 1];
                ++i;  // Пропускаем следующий аргумент, т.к. это значение пользователя
            } else if (argv[i] == std::string("-c") || argv[i] == std::string("-e")) {
                flag = argv[i]; // Определяем флаг (или -c, или -e)
            }
        }

        if (flag.empty()) {
            std::cerr << "Unknown flag: please specify -c or -e\n";
            return;
        }

        if (flag == "-c") {
            // Режим команды (например, clean или echo)
            request_json["cmd"] = argv[2];
        } else if (flag == "-e") {
            // Режим выражения: объединяем все аргументы начиная с argv[2]
            std::ostringstream oss;
            std::string expression;
            for (int i = 2; i < argc; ++i) {
                if (std::string(argv[i]) == "-u") break; // Останавливаемся перед флагом пользователя
                expression += argv[i];
                if (i < argc - 1)
                    expression += " ";
            }

            // Обрабатываем многострочные запросы
            std::string full_expr;
            std::istringstream expr_stream(expression);
            std::string line;
            bool multi_line = false;

            while (std::getline(expr_stream, line)) {
                // Проверяем на наличие продолжения строки с символом '\'
                if (!line.empty() && line.back() == '\\') {
                    line.pop_back();  // Убираем символ продолжения
                    full_expr += line + " ";  // Добавляем строку в полный запрос
                    multi_line = true;
                } else {
                    full_expr += line;  // Строка завершена
                    request_json["exp"] = full_expr;
                    multi_line = false;
                }
            }
        } else {
            std::cerr << "Unknown flag: " << flag << "\n";
            return;
        }

        if (!user.empty()) {
            request_json["user"] = user;  // Добавляем пользователя в запрос, если он указан
        }

        // Создаем HTTP-клиент, обращающийся к серверу на localhost:8080
        httplib::Client cli("localhost", 8080); 
        cli.set_connection_timeout(3); 

        // Отправляем POST-запрос на эндпоинт /api/calculate с JSON-телом
        auto res = cli.Post("/api/calculate", request_json.dump(), "application/json");

        if (res && res->status == 200) {
            try {
                auto response_json = nlohmann::json::parse(res->body);
                if (response_json.contains("res")) {
                    std::cout << response_json["res"] << "\n";
                } else if (response_json.contains("error")) {
                    std::cerr << "Error: " << response_json["error"] << "\n";
                } else {
                    std::cerr << "Invalid response: " << res->body << "\n";
                }
            } catch (const std::exception& ex) {
                std::cerr << "Failed to parse response: " << ex.what() << "\n";
            }
        } else {
            std::cerr << "Request failed. ";
            if (res) {
                std::cerr << "Status: " << res->status << "\n";
            } else {
                std::cerr << "No response.\n";
            }
        }
    }

} // namespace calcclient
