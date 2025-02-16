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
        std::ostringstream expression_stream;
        std::string command;

        // Разбираем аргументы, чтобы найти -u (имя пользователя) в любом месте
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "-u" && i + 1 < argc) {
                user = argv[i + 1];
                ++i;  // Пропускаем следующий аргумент, т.к. это имя пользователя
            } else if (arg == "-c" || arg == "-e") {
                flag = arg;
            } else {
                if (flag == "-c") {
                    if (!command.empty()) command += " ";
                    command += arg;
                } else if (flag == "-e") {
                    if (!expression_stream.str().empty()) expression_stream << " ";
                    expression_stream << arg;
                }
            }
        }

        if (flag.empty()) {
            std::cerr << "Unknown flag: please specify -c or -e\n";
            return;
        }

        nlohmann::json request_json;

        if (flag == "-c") {
            request_json["cmd"] = command;
        } else if (flag == "-e") {
            request_json["exp"] = expression_stream.str();
        }

        if (!user.empty()) {
            request_json["user"] = user;
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
