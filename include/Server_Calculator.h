#pragma once

#include <httplib.h>
#include <nlohmann/json.hpp>
#include "Calculator.h" // Ваши функции калькулятора

using namespace httplib;
using json = nlohmann::json;

// Структура для хранения значений переменных
std::map<std::string, double> variables;

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

            // Обработка присвоения значения переменной
            if (request.contains("exp")) {
                std::string expression = request["exp"];

                // Проверка на присваивание значения переменной, например: x = 5
                size_t eq_pos = expression.find('=');
                if (eq_pos != std::string::npos) {
                    std::string var_name = expression.substr(0, eq_pos);
                    var_name = var_name.substr(var_name.find_first_not_of(" \t")); // Убираем пробелы
                    std::string value_str = expression.substr(eq_pos + 1);
                    value_str = value_str.substr(value_str.find_first_not_of(" \t")); // Убираем пробелы

                    // Преобразование в число и сохранение в переменную
                    double value = std::stod(value_str);
                    variables[var_name] = value;

                    json response = {{"res", "Variable " + var_name + " set to " + std::to_string(value)}};
                    res.set_content(response.dump(), "application/json");
                    return;
                }

                // Замена переменных на их значения
                for (const auto& var : variables) {
                    size_t pos = 0;
                    while ((pos = expression.find(var.first, pos)) != std::string::npos) {
                        expression.replace(pos, var.first.length(), std::to_string(var.second));
                        pos += std::to_string(var.second).length();
                    }
                }

                // Токенизация и вычисление
                auto tokens = tokenize(expression);
                auto postfix = shunting_yard(tokens);
                double result = evaluate(postfix);

                json response = {{"res", result}};
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
