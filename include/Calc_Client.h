#pragma once
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <algorithm>
#include <httplib.h>
#include <nlohmann/json.hpp>

namespace calcclient {

    using json = nlohmann::json;

    // =============================================
    // Базовые классы и исключения
    // =============================================
    class ICommand {
    public:
        virtual ~ICommand() = default;
        virtual void execute() = 0;
    };

    class CommandException : public std::exception {
        std::string message_;
    public:
        CommandException(const std::string& msg) : message_(msg) {}
        const char* what() const noexcept override { return message_.c_str(); }
    };

    // =============================================
    // Конкретные команды
    // =============================================
    class CalculateCommand : public ICommand {
        std::string expression_;
        std::string user_;

        // void replace_special_chars(std::string& expr) {
        //     std::replace(expr.begin(), expr.end(), '*', '#'); 
        // }

    public:
        CalculateCommand(const std::string& expr, const std::string& user)
            : expression_(expr), user_(user) {}

        void execute() override {
            httplib::Client cli("localhost", 8080);
            cli.set_connection_timeout(3);

            // Обработка спецсимволов и формирование запроса
            std::string processed_expr = expression_;
            // replace_special_chars(processed_expr);

            json req;
            req["exp"] = expression_;
            if (!user_.empty()) req["user"] = user_;

            if (auto res = cli.Post("/api/calculate", req.dump(), "application/json")) {
                handle_response(*res);
            } else {
                throw CommandException("Ошибка соединения: " + httplib::to_string(res.error()));
            }
        }

    private:
        void handle_response(const httplib::Response& res) {
            try {
                json response = json::parse(res.body);
                
                if (res.status == 200) {
                    if (response.contains("res")) {
                        std::cout << "Результат: " << response["res"].get<double>() << "\n";
                    } else if (response.contains("error")) {
                        throw std::runtime_error(response["error"].get<std::string>());
                    }
                } else {
                    throw std::runtime_error("Ошибка сервера: " + res.body);
                }
            } catch (const json::exception& e) {
                throw CommandException("Некорректный JSON: " + std::string(e.what()));
            }
        }
    };

    class CleanCommand : public ICommand {
        std::string user_;

    public:
        CleanCommand(const std::string& user) : user_(user) {}

        void execute() override {
            httplib::Client cli("localhost", 8080);
            cli.set_connection_timeout(3);

            json req;
            req["cmd"] = "clean";
            if (!user_.empty()) req["user"] = user_;

            if (auto res = cli.Post("/api/calculate", req.dump(), "application/json")) {
                if (res->status == 200) {
                    std::cout << "Сессия пользователя '" << user_ << "' очищена\n";
                } else {
                    throw CommandException("Ошибка: " + res->body);
                }
            } else {
                throw CommandException("Ошибка соединения: " + httplib::to_string(res.error()));
            }
        }
    };

    // =============================================
    // Фабрика команд
    // =============================================
    class CommandFactory {
    public:
        static std::unique_ptr<ICommand> create(int argc, char* argv[]) {
            std::string user, command_type;
            std::vector<std::string> args;

            // Парсинг аргументов
            for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "-u" && i + 1 < argc) {
                    user = argv[++i];
                } else if (arg == "-c" || arg == "-e") {
                    command_type = arg;
                    while (++i < argc && argv[i][0] != '-') {
                        args.push_back(argv[i]);
                    }
                    --i;
                }
            }

            // Валидация
            if (command_type.empty()) {
                throw CommandException("Не указан тип команды (-c/-e)");
            }

            // Сборка выражения
            std::ostringstream expr_stream;
            for (const auto& part : args) {
                expr_stream << part << " ";
            }
            std::string expr = expr_stream.str();
            if (!expr.empty()) expr.pop_back();

            // Создание команды
            if (command_type == "-c") {
                if (expr == "clean") {
                    return std::make_unique<CleanCommand>(user);
                }
                throw CommandException("Неизвестная команда: " + expr);
            }
            else if (command_type == "-e") {
                if (expr.empty()) {
                    throw CommandException("Пустое выражение");
                }
                return std::make_unique<CalculateCommand>(expr, user);
            }

            throw CommandException("Некорректный формат команды");
        }
    };

    // =============================================
    // Точка входа клиента
    // =============================================
    void run_client(int argc, char* argv[]) {
        try {
            auto command = CommandFactory::create(argc, argv);
            if (command) {
                command->execute();
            }
        } catch (const CommandException& e) {
            std::cerr << "ОШИБКА: " << e.what() << "\n\n";
            std::cerr << "Использование:\n"
                      << "  " << argv[0] << " [-u пользователь] (-c <команда> | -e <выражение>)\n"
                      << "Команды:\n"
                      << "  -c clean    Очистить сессию\n"
                      << "  -e <expr>   Вычислить выражение\n"
                      << "Примеры:\n"
                      << "  " << argv[0] << " -u alice -e \"x=5; x*2\"\n"
                      << "  " << argv[0] << " -u bob -c clean\n";
        } catch (const std::exception& e) {
            std::cerr << "КРИТИЧЕСКАЯ ОШИБКА: " << e.what() << "\n";
        }
    }

} // namespace calcclient