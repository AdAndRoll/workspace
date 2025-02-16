#pragma once

#include <httplib.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace httplib;
using namespace std;

// Объявление функции обработки запроса
void handle_request(const Request& req, Response& res);

// Функция запуска HTTP-сервера
void start_http_server() {
    Server svr;

    // Регистрация обработчиков для всех методов
    svr.Get(".*", handle_request);
    svr.Post(".*", handle_request);
    svr.Put(".*", handle_request);
    svr.Patch(".*", handle_request);
    svr.Delete(".*", handle_request);
    svr.Options(".*", handle_request);

    cout << "HTTP Server started on port 8080\n";
    svr.listen("0.0.0.0", 8080);
}

// Реализация обработчика запросов
void handle_request(const Request& req, Response& res) {
    vector<string> IGNORED_HEADERS = {"LOCAL_ADDR", "REMOTE_ADDR", "REMOTE_PORT", "LOCAL_PORT"};
    stringstream response;
    
    response << "Method: " << req.method << "\n"
             << "Path: " << req.path << "\n"
             << "Headers:\n";

    // Фильтрация заголовков
    for (const auto& h : req.headers) {
        bool ignore = false;
        for (const auto& ih : IGNORED_HEADERS) {
            if (h.first == ih) {
                ignore = true;
                break;
            }
        }
        if (!ignore) {
            response << "  " << h.first << ": " << h.second << "\n";
        }
    }
    
    response << "Body:\n" << req.body << "\n";
    res.set_content(response.str(), "text/plain");
}