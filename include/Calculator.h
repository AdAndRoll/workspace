#pragma once

#include <iostream>
#include <stack>
#include <queue>
#include <sstream>
#include <map>
#include <stdexcept>

using namespace std;

// ========================
// Структуры данных
// ========================

enum TokenType { NUMBER, OPERATOR, LEFT_PAREN, RIGHT_PAREN };

struct Token {
    TokenType type;
    double value;
    char op;
    
    Token(double val) : type(NUMBER), value(val), op(0) {}
    Token(char ch) : type(OPERATOR), value(0), op(ch) {}
    Token(TokenType t) : type(t), value(0), op(0) {}
};

// ========================
// Объявления функций
// ========================

queue<Token> tokenize(const string& expr);
queue<Token> shunting_yard(queue<Token> tokens);
double evaluate(queue<Token> postfix);
void run_calculator();

// ========================
// Реализация функций
// ========================

queue<Token> tokenize(const string& expr) {
    queue<Token> tokens;
    string num_str;
    bool negative_number = false;

    for (size_t i = 0; i < expr.size(); ++i) {
        char c = expr[i];
        
        if (isdigit(c) || c == '.' || (c == '-' && (i == 0 || expr[i-1] == '('))) {
            if (c == '-' && (i == 0 || expr[i-1] == '(')) {
                negative_number = true;
                continue;
            }
            num_str += c;
        } else {
            if (!num_str.empty()) {
                double num = stod(num_str);
                if (negative_number) {
                    num = -num;
                    negative_number = false;
                }
                tokens.push(Token(num));
                num_str.clear();
            }
            
            if (c == ' ') continue;
            
            switch(c) {
                case '+': case '-': case '*': case '/':
                    tokens.push(Token(c));
                    break;
                case '(':
                    tokens.push(Token(LEFT_PAREN));
                    break;
                case ')':
                    tokens.push(Token(RIGHT_PAREN));
                    break;
                default:
                    throw invalid_argument("Invalid character: " + string(1, c));
            }
        }
    }
    
    if (!num_str.empty()) {
        double num = stod(num_str);
        if (negative_number) num = -num;
        tokens.push(Token(num));
    }
    
    return tokens;
}

queue<Token> shunting_yard(queue<Token> tokens) {
    queue<Token> output;
    stack<Token> ops_stack;
    map<char, int> precedence = {{'+',1}, {'-',1}, {'*',2}, {'/',2}};

    while (!tokens.empty()) {
        Token token = tokens.front();
        tokens.pop();

        switch(token.type) {
            case NUMBER:
                output.push(token);
                break;
                
            case OPERATOR: {
                while (!ops_stack.empty() && 
                      (ops_stack.top().type == OPERATOR) &&
                      (precedence[ops_stack.top().op] >= precedence[token.op])) {
                    output.push(ops_stack.top());
                    ops_stack.pop();
                }
                ops_stack.push(token);
                break;
            }
                
            case LEFT_PAREN:
                ops_stack.push(token);
                break;
                
            case RIGHT_PAREN: {
                while (!ops_stack.empty() && ops_stack.top().type != LEFT_PAREN) {
                    output.push(ops_stack.top());
                    ops_stack.pop();
                }
                if (ops_stack.empty()) throw invalid_argument("Mismatched parentheses");
                ops_stack.pop();
                break;
            }
        }
    }

    while (!ops_stack.empty()) {
        if (ops_stack.top().type == LEFT_PAREN) {
            throw invalid_argument("Mismatched parentheses");
        }
        output.push(ops_stack.top());
        ops_stack.pop();
    }

    return output;
}

double evaluate(queue<Token> postfix) {
    stack<double> values;

    while (!postfix.empty()) {
        Token token = postfix.front();
        postfix.pop();

        if (token.type == NUMBER) {
            values.push(token.value);
        } 
        else if (token.type == OPERATOR) {
            if (values.size() < 2) throw invalid_argument("Invalid expression");
            
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            
            switch(token.op) {
                case '+': values.push(a + b); break;
                case '-': values.push(a - b); break;
                case '*': values.push(a * b); break;
                case '/': 
                    if (b == 0) throw runtime_error("Division by zero");
                    values.push(a / b);
                    break;
            }
        }
    }

    if (values.size() != 1) throw invalid_argument("Invalid expression");
    return values.top();
}

void run_calculator() {
    cout << "Calculator (enter 'exit' to quit)\n";
    string input;
    
    while (true) {
        cout << "> ";
        getline(cin, input);
        if (input.empty()) continue;
        if (input == "exit") break;

        try {
            queue<Token> tokens = tokenize(input);
            queue<Token> postfix = shunting_yard(tokens);
            double result = evaluate(postfix);
            cout << "= " << result << "\n\n";
        } 
        catch (const exception& e) {
            cerr << "Error: " << e.what() << "\n\n";
        }
    }
}

// ========================
// Пример использования
// ========================
/*
int main() {
    // Только калькулятор
    run_calculator();

    // Или вместе с HTTP-сервером
    // thread server_thread(start_http_server);
    // run_calculator();
    // server_thread.join();
    
    return 0;
}
*/