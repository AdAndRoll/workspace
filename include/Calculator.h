#pragma once

#include <iostream>
#include <stack>
#include <queue>
#include <sstream>
#include <map>
#include <stdexcept>
#include <unordered_map>

using namespace std;

// ========================
// Структуры данных
// ========================

enum TokenType { NUMBER, OPERATOR, LEFT_PAREN, RIGHT_PAREN, VARIABLE };

struct Token {
    TokenType type;
    double value;
    char op;
    string variable_name;

    Token(double val) : type(NUMBER), value(val), op(0) {}
    Token(char ch) : type(OPERATOR), value(0), op(ch) {}
    Token(TokenType t) : type(t), value(0), op(0) {}
    Token(string var) : type(VARIABLE), value(0), variable_name(var) {}
};

// Теперь переменные хранятся в сессиях
extern map<string, map<string, double>> sessions;

// ========================
// Объявления функций
// ========================

queue<Token> tokenize(const string& expr);
queue<Token> shunting_yard(queue<Token> tokens);
double evaluate(queue<Token> postfix, const string& user);
void run_calculator();

// ========================
// Реализация функций
// ========================

queue<Token> tokenize(const string& expr) {
    queue<Token> tokens;
    string num_str, var_str;
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

            if (!var_str.empty()) {
                tokens.push(Token(var_str));
                var_str.clear();
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
                case '=':
                    if (!tokens.empty() && tokens.back().type == VARIABLE) {
                        tokens.push(Token(OPERATOR));
                    } else {
                        throw invalid_argument("Invalid assignment format");
                    }
                    break;
                default:
                    if (isalpha(c)) {
                        var_str += c;
                    } else {
                        throw invalid_argument("Invalid character: " + string(1, c));
                    }
            }
        }
    }

    if (!num_str.empty()) {
        double num = stod(num_str);
        if (negative_number) num = -num;
        tokens.push(Token(num));
    }

    if (!var_str.empty()) {
        tokens.push(Token(var_str));
    }

    return tokens;
}

// Используем пользовательскую сессию при вычислениях
double evaluate(queue<Token> postfix, const string& user) {
    if (postfix.empty()) {
        throw runtime_error("Empty expression");
    }

    stack<double> values;
    if (user.empty()) {
        throw runtime_error("User session not specified");
    }
    if (sessions.find(user) == sessions.end()) {
        throw runtime_error("User session does not exist");
    }
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
        else if (token.type == VARIABLE) {
            if (sessions[user].find(token.variable_name) == sessions[user].end()) {
                throw runtime_error("Variable not defined: " + token.variable_name);
            }
            values.push(sessions[user][token.variable_name]);
        }
    }

    if (values.size() != 1) throw invalid_argument("Invalid expression");
    return values.top();
}

// Функция преобразования из инфиксной записи в постфиксную (реализация в заголовочном файле)
inline queue<Token> shunting_yard(queue<Token> tokens) {
    stack<Token> ops;
    queue<Token> output;

    while (!tokens.empty()) {
        Token token = tokens.front();
        tokens.pop();

        if (token.type == NUMBER || token.type == VARIABLE) {
            output.push(token);
        } else if (token.type == OPERATOR) {
            while (!ops.empty() && ops.top().type == OPERATOR &&
                   ((token.op == '+' || token.op == '-') && (ops.top().op == '*' || ops.top().op == '/'))) {
                output.push(ops.top());
                ops.pop();
            }
            ops.push(token);
        } else if (token.type == LEFT_PAREN) {
            ops.push(token);
        } else if (token.type == RIGHT_PAREN) {
            while (!ops.empty() && ops.top().type != LEFT_PAREN) {
                output.push(ops.top());
                ops.pop();
            }
            if (ops.empty()) throw invalid_argument("Mismatched parentheses");
            ops.pop();
        }
    }

    while (!ops.empty()) {
        output.push(ops.top());
        ops.pop();
    }

    return output;
}

void run_calculator() {
    cout << "Calculator (enter 'exit' to quit, 'set <var> <value>' to set variable, 'user <name>' to change session)\n";
    string input;
    string user = "default"; // По умолчанию используем сессию "default"

    while (true) {
        cout << "[" << user << "]> ";
        getline(cin, input);
        if (input.empty()) continue;
        if (input == "exit") break;

        if (input.substr(0, 3) == "set") {
            stringstream ss(input.substr(4));
            string var_name;
            double value;
            if (!(ss >> var_name >> value)) {
                cerr << "Invalid set command format\n";
                continue;
            }
            sessions[user][var_name] = value;
            cout << var_name << " set to " << value << " in session " << user << "\n";
        } 
        else if (input.substr(0, 4) == "user") {
            stringstream ss(input.substr(5));
            ss >> user;
            if (user.empty()) user = "default";
            cout << "Switched to user session: " << user << "\n";
        } 
        else {
            try {
                queue<Token> tokens = tokenize(input);
                queue<Token> postfix = shunting_yard(tokens);
                double result = evaluate(postfix, user);
                cout << "= " << result << "\n\n";
            } 
            catch (const exception& e) {
                cerr << "Error: " << e.what() << "\n\n";
            }
        }
    }
}
