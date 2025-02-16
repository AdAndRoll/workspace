#include <httplib.h>
#include <nlohmann/json.hpp>
#include "../include/Calculator.h"
#include "../include/SessionManager.h"

using json = nlohmann::json;

int main() {
    SessionManager sessions;
    httplib::Server svr;

    svr.Post("/api/calculate", [&](const auto& req, auto& res) {
        try {
            auto j = json::parse(req.body);
            std::string user = j.value("user", "default");
            auto& vars = sessions.get_session(user);

            if (j.contains("cmd") && j["cmd"] == "clean") {
                sessions.clear_session(user);
                res.set_content(json{{"res", "OK"}}.dump(), "application/json");
                return;
            }

            if (j.contains("exp")) {
                std::string expr = j["exp"];
                Calculator calc;
                std::istringstream iss(expr);
                std::string line;
                json results = json::array();
                bool has_result = false;

                while (std::getline(iss, line, ';')) {
                    line = Calculator::trim(line);
                    if (line.empty()) continue;
                
                    try {
                        // Логируем текущую обрабатываемую строку
                        std::cout << "[DEBUG] Processing line: '" << line << "'" << std::endl;
                
                        double result = calc.calculate(line, vars);
                        has_result = true;
                
                        if (line.find('=') != std::string::npos) {
                            size_t eq_pos = line.find('=');
                            std::string var_name = Calculator::trim(line.substr(0, eq_pos));
                            
                            // Логируем значение переменной после присваивания
                            std::cout << "[DEBUG] Assigned variable: " << var_name 
                                      << " = " << vars[var_name] << std::endl;
                            
                            results.push_back({{var_name, vars[var_name]}});
                        } else {
                            results.push_back(result);
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "[ERROR] Failed to process line '" << line << "': " << e.what() << std::endl;
                        throw std::runtime_error("Error in '" + line + "': " + e.what());
                    }
                }

                if (!has_result) {
                    throw std::runtime_error("No valid expressions provided");
                }

                res.set_content(json{{"res", results}}.dump(), "application/json");
            }
        } catch (const std::exception& e) {
            res.status = 400;
            res.set_content(json{{"error", e.what()}}.dump(), "application/json");
        } catch (...) {
            res.status = 500;
            res.set_content(json{{"error", "Internal server error"}}.dump(), "application/json");
        }
    });

    svr.set_error_handler([](const auto& req, auto& res) {
        res.set_content(json{{"error", "Internal server error"}}.dump(), "application/json");
    });

    std::cout << "Server running on port 8080\n";
    svr.listen("0.0.0.0", 8080);
    return 0;
}