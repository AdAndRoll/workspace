#pragma once

#include <httplib.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include "Calculator.h" // Ваши функции калькулятора

using namespace httplib;
using json = nlohmann::json;

// Структура для хранения значений переменных
std::map<std::string, double> variables;

// Функция для удаления пробелов в начале и конце строки
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, last - first + 1);
}

void start_calculator_server() {
    Server svr;

    // Эндпоинт для вычислений
    svr.Post("/api/calculate", [](const Request& req, Response& res) {
        try {
            json request = json::parse(req.body);

            // Обработка команды echo
            if (request.contains("cmd") && request["cmd"] == "echo") {
                json response = {{"res", "echo"}};
                res.set_content(response.dump(), "application/json");
                return;
            }

            // Обработка выражений
            if (request.contains("exp")) {
                std::string full_expression = request["exp"];

                std::istringstream expr_stream(full_expression);
                std::string line;
                double last_result = 0;

                while (std::getline(expr_stream, line, ';')) { // Разбиваем выражение на части
                    line = trim(line);
                    if (line.empty()) continue;

                    size_t eq_pos = line.find('=');
                    if (eq_pos != std::string::npos) {
                        // Обнаружено присваивание переменной: `var = ...`
                        std::string var_name = trim(line.substr(0, eq_pos));
                        std::string value_str = trim(line.substr(eq_pos + 1));

                        // Подставляем уже сохранённые переменные перед вычислением
                        for (const auto& var : variables) {
                            size_t pos = 0;
                            while ((pos = value_str.find(var.first, pos)) != std::string::npos) {
                                value_str.replace(pos, var.first.length(), std::to_string(var.second));
                                pos += std::to_string(var.second).length();
                            }
                        }

                        // Вычисляем значение
                        auto tokens = tokenize(value_str);
                        auto postfix = shunting_yard(tokens);
                        double value = evaluate(postfix);

                        variables[var_name] = value;
                        last_result = value;
                    } else {
                        // Обычное математическое выражение
                        for (const auto& var : variables) {
                            size_t pos = 0;
                            while ((pos = line.find(var.first, pos)) != std::string::npos) {
                                line.replace(pos, var.first.length(), std::to_string(var.second));
                                pos += std::to_string(var.second).length();
                            }
                        }

                        // Вычисляем выражение
                        auto tokens = tokenize(line);
                        auto postfix = shunting_yard(tokens);
                        last_result = evaluate(postfix);
                    }
                }

                // Возвращаем результат последнего выражения
                json response = {{"res", last_result}};
                res.set_content(response.dump(), "application/json");
                return;
            }

            throw std::runtime_error("Invalid request format");

        } catch (const std::exception& e) {
            json error = {{"error", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // Запуск сервера
    std::cout << "Calculator Server started at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}
