#include <iostream>
#include "echo_server.h"
#include "Calculator.h"
#include "Server_Calculator.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    start_calculator_server(); 
    //run_calculator();
    //start_http_server();

    return 0;
}