#include <iostream>
#include "echo_server.h"
#include "Calculator.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    run_calculator();
    start_http_server();

    return 0;
}