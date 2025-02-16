#pragma once

#include <httplib.h>
#include <nlohmann/json.hpp>
#include "Calculator.h" // Ваши функции калькулятора

using namespace httplib;
using json = nlohmann::json;

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

            // Обработка выражения
            if (request.contains("exp")) {
                string expression = request["exp"];
                
                // Используем существующие функции калькулятора
                auto tokens = tokenize(expression);
                auto postfix = shunting_yard(tokens);
                double result = evaluate(postfix);
                
                json response = {{"res", result }};
                res.set_content(response.dump(), "application/json");
                return;
            }

            throw runtime_error("Invalid request format");

        } catch (const exception& e) {
            json error = {{"error", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    // Запуск сервера
    cout << "Calculator Server started at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}

