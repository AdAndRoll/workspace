#include <iostream>
#include "echo_server.h"
int main() {
    std::cout << "Hello, World!" << std::endl;

    start_http_server();

    return 0;
}