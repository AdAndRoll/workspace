#pragma once
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <memory>
#include <mutex>
#include <sstream>
#include "SessionManager.h"
#include "Calculator.h"

namespace calcserver {

using json = nlohmann::json;

// =============================================
// Request Processing Chain
// =============================================
class IRequestHandler {
protected:
    std::shared_ptr<IRequestHandler> next_;
    
public:
    virtual ~IRequestHandler() = default;
    
    void set_next(std::shared_ptr<IRequestHandler> handler) {
        next_ = handler;
    }

    virtual bool handle(const json& request, 
                       json& response,
                       SessionManager& session_manager,
                       const std::string& user) = 0;
};

class CleanCommandHandler : public IRequestHandler {
public:
    bool handle(const json& request, 
               json& response,
               SessionManager& session_manager,
               const std::string& user) override 
    {
        if (request.contains("cmd") && request["cmd"] == "clean") {
            session_manager.clear_session(user);
            response["res"] = "OK";
            return true;
        }
        return next_ ? next_->handle(request, response, session_manager, user) : false;
    }
};

class ExpressionHandler : public IRequestHandler {
public:
    bool handle(const json& request, 
               json& response,
               SessionManager& session_manager,
               const std::string& user) override 
    {
        if (request.contains("exp")) {
            auto& vars = session_manager.get_session(user);
            Calculator calc;
            json results = json::array();
            std::istringstream iss(request["exp"].get<std::string>());
            std::string line;
            bool has_result = false;

            while (std::getline(iss, line, ';')) {
                line = Calculator::trim(line);
                if (line.empty()) continue;

                try {
                    double result = calc.calculate(line, vars);
                    has_result = true;

                    // Формируем результат в зависимости от типа операции
                    if (calc.was_assignment()) {
                        results.push_back({{calc.get_last_var(), result}});
                    } else {
                        results.push_back(result);
                    }
                } catch (const std::exception& e) {
                    throw std::runtime_error("Error in '" + line + "': " + e.what());
                }
            }

            if (!has_result) {
                throw std::runtime_error("No valid expressions");
            }

            response["res"] = results;
            return true;
        }
        return false;
    }
};

// =============================================
// Calculator Service Facade
// =============================================
class CalculatorService {
    httplib::Server server_;
    std::shared_ptr<SessionManager> session_manager_;
    std::shared_ptr<IRequestHandler> request_chain_;
    
public:
    CalculatorService(std::shared_ptr<SessionManager> session_manager)
        : session_manager_(session_manager)
    {
        build_handler_chain();
        setup_routes();
    }

    void start(int port = 8080) {
        std::cout << "Calculator service running on port " << port << "\n";
        server_.listen("0.0.0.0", port);
    }

private:
    void build_handler_chain() {
        auto clean_handler = std::make_shared<CleanCommandHandler>();
        auto expr_handler = std::make_shared<ExpressionHandler>();
        
        clean_handler->set_next(expr_handler);
        request_chain_ = clean_handler;
    }

    void setup_routes() {
        server_.Post("/api/calculate", [&](const httplib::Request& req, httplib::Response& res) {
            try {
                json request = json::parse(req.body);
                json response;
                std::string user = request.value("user", "default");
                
                if (!request_chain_->handle(request, response, *session_manager_, user)) {
                    throw std::runtime_error("Unsupported request format");
                }
                
                res.set_content(response.dump(), "application/json");
                
            } catch (const std::exception& e) {
                res.status = 400;
                json error = {{"error", e.what()}};
                res.set_content(error.dump(), "application/json");
            }
        });

        server_.set_error_handler([](const httplib::Request&, httplib::Response& res) {
            json error = {{"error", "Internal server error"}};
            res.set_content(error.dump(), "application/json");
        });
    }
};

} // namespace calcserver