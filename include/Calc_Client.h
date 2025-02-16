#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <httplib.h>
#include <nlohmann/json.hpp>

namespace calcclient {

    inline void run_client(int argc, char* argv[]) {
        if (argc < 3) {
            std::cerr << "Usage: " << argv[0] << " -c <command> OR -e <expression...>\n";
            return;
        }

        std::string flag = argv[1];
        nlohmann::json request_json;

        if (flag == "-c") {
            // Режим команды (например, echo)
            request_json["cmd"] = argv[2];
        } else if (flag == "-e") {
            // Режим выражения: объединяем все аргументы начиная с argv[2]
            std::ostringstream oss;
            for (int i = 2; i < argc; ++i) {
                oss << argv[i];
                if (i < argc - 1)
                    oss << " ";
            }
            request_json["exp"] = oss.str();
        } else {
            std::cerr << "Unknown flag: " << flag << "\n";
            std::cerr << "Usage: " << argv[0] << " -c <command> OR -e <expression...>\n";
            return;
        }

        // Создаем HTTP-клиент, обращающийся к серверу на localhost:8080
        httplib::Client cli("localhost", 8080); 
        cli.set_connection_timeout(3); 
        // std::cout << "Отправка запроса на сервер: " << request_json.dump() << std::endl;
        // std::cout << "Адрес подключения: " << cli.host() << ":" << cli.port() << std::endl;

        // Отправляем POST-запрос на эндпоинт /api/calculate с JSON-телом
        auto res = cli.Post("/api/calculate", request_json.dump(), "application/json");
        
        // if (!res) {
        //     auto err = res.error();
        //     std::cerr << "Connection error: " 
        //               << httplib::to_string(err) << "\n";
        //     return;
        // }

        // if (res) {
        //     std::cout << "Ответ сервера: " << res->body << std::endl;
        // } else {
        //     std::cerr << "Ошибка запроса: " << res.error() << std::endl;
        // }

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
