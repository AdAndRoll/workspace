#pragma once

#include <httplib.h>
#include <nlohmann/json.hpp>
#include "Calculator.h"
#include <map>
#include <sstream>

using namespace httplib;
using json = nlohmann::json;

std::map<std::string, double> variables;

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t");
    return str.substr(first, (last - first + 1));
}

void start_calculator_server() {
    Server svr;

    svr.Post("/api/calculate", [](const Request& req, Response& res) {
        try {
            json request = json::parse(req.body);

            if (request.contains("cmd") && request["cmd"] == "clean") {
                variables.clear();
                json response = {{"res", "OK"}};  // Теперь clean возвращает "OK"
                res.set_content(response.dump(), "application/json");
                return;
            }

            if (request.contains("exp")) {
                std::string expression = request["exp"];
                std::istringstream expr_stream(expression);
                std::string line;
                double last_result = 0.0;
                bool has_result = false;
                bool assignment_made = false;

                while (std::getline(expr_stream, line, ';')) {
                    line = trim(line);
                    if (line.empty()) continue;

                    size_t eq_pos = line.find('=');
                    if (eq_pos != std::string::npos) {
                        std::string var_name = trim(line.substr(0, eq_pos));
                        std::string value_str = trim(line.substr(eq_pos + 1));

                        auto tokens = tokenize(value_str);
                        auto postfix = shunting_yard(tokens);
                        double value = evaluate(postfix);

                        variables[var_name] = value;
                        assignment_made = true;
                        continue;  // Не вычисляем результат, если это присваивание
                    }

                    for (const auto& var : variables) {
                        size_t pos = 0;
                        while ((pos = line.find(var.first, pos)) != std::string::npos) {
                            line.replace(pos, var.first.length(), std::to_string(var.second));
                            pos += std::to_string(var.second).length();
                        }
                    }

                    auto tokens = tokenize(line);
                    auto postfix = shunting_yard(tokens);
                    last_result = evaluate(postfix);
                    has_result = true;
                }

                if (assignment_made) {
                    json response = {{"res", "OK"}};  // Теперь присваивание возвращает "OK"
                    res.set_content(response.dump(), "application/json");
                } else if (has_result) {
                    json response = {{"res", last_result}};
                    res.set_content(response.dump(), "application/json");
                }
                return;
            }

            throw std::runtime_error("Invalid request format");

        } catch (const std::exception& e) {
            json error = {{"err", e.what()}};
            res.status = 400;
            res.set_content(error.dump(), "application/json");
        }
    });

    std::cout << "Calculator Server started at http://localhost:8080\n";
    svr.listen("0.0.0.0", 8080);
}
