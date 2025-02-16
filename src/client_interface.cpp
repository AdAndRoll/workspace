#include <iostream>
#include <vector>
#include <httplib.h>
#include <nlohmann/json.hpp>
#include "../include/Calculator.h"

using json = nlohmann::json;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " -u <user> [-e <expression> | -c clean]\n";
        return 1;
    }

    std::string user, expr;
    bool clean = false;

    // Парсинг аргументов
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-u" && i+1 < argc) user = argv[++i];
        else if (arg == "-e" && i+1 < argc) expr = argv[++i];
        else if (arg == "-c" && i+1 < argc) clean = (std::string(argv[++i]) == "clean");
    }

    json req;
    if (clean) {
        req["cmd"] = "clean";
        req["user"] = user;
    } else {
        // Убрана замена символов, передаём выражение "как есть"
        req["exp"] = expr;
        req["user"] = user;
    }

    httplib::Client cli("localhost", 8080);
    if (auto res = cli.Post("/api/calculate", req.dump(), "application/json")) {
        auto j = json::parse(res->body);
        if (j.contains("res")) std::cout << j["res"] << "\n";
        if (j.contains("error")) std::cerr << "Error: " << j["error"] << "\n";
    } else {
        std::cerr << "Connection error!\n";
    }
    
    return 0;
}