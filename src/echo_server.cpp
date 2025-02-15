#include <httplib.h>
#include <iostream>
#include <sstream>

using namespace httplib;
using namespace std;

void handle_request(const Request& req, Response& res) {
    stringstream response;
    response << "Method: " << req.method << "\n"
             << "Path: " << req.path << "\n"
             << "Headers:\n";
    for (const auto& h : req.headers)
        response << "  " << h.first << ": " << h.second << "\n";
    response << "Body:\n" << req.body<<" \n";
    
    res.set_content(response.str(), "text/plain");
}

int main() {
    Server svr;

    // Регистрируем обработчики для основных методов
    svr.Get(".*", handle_request);
    svr.Post(".*", handle_request);
    svr.Put(".*", handle_request);
    svr.Patch(".*", handle_request);
    svr.Delete(".*", handle_request);
    svr.Options(".*", handle_request);

    cout << "Server started on port 8080\n";
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}