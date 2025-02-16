#pragma once
#include "Token.h"
#include <queue>
#include <map>
#include <sstream>
#include <stdexcept>
#include <cctype>

class Calculator {
public:
    static std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" ");
        auto end = s.find_last_not_of(" ");
        return (start == std::string::npos) ? "" : s.substr(start, end - start + 1);
    }

    double calculate(const std::string& expr, std::map<std::string, double>& vars) {
        auto tokens = tokenize(expr);
        bool is_assignment = process_assignments(tokens, vars);
        
        if (is_assignment) {
            return vars[last_assigned_var_];
        } else {
            auto postfix = shunting_yard(tokens);
            return evaluate(postfix, vars);
        }
    }

    bool was_assignment() const { return !last_assigned_var_.empty(); }
    std::string get_last_var() const { return last_assigned_var_; }

private:
    std::string last_assigned_var_;

    std::queue<Token> tokenize(const std::string& expr) {
        std::queue<Token> tokens;
        std::string buffer;
        bool negative = false;

        for (size_t i = 0; i < expr.size(); ++i) {
            char c = expr[i];

            if (isdigit(c) || c == '.' || (c == '-' && (i == 0 || expr[i-1] == '('))) {
                if (c == '-' && (i == 0 || expr[i-1] == '(')) {
                    negative = true;
                    continue;
                }
                buffer += c;
            } else {
                if (!buffer.empty()) {
                    handle_buffer(buffer, negative, tokens);
                    buffer.clear();
                    negative = false;
                }

                if (c == ' ') continue;

                switch(c) {
                    case '+': case '-': case '*': case '/': 
                        tokens.emplace(c); break;
                    case '(': tokens.emplace(TokenType::LeftParen); break;
                    case ')': tokens.emplace(TokenType::RightParen); break;
                    case '=': tokens.emplace(TokenType::Assignment); break;
                    default:
                        if (isalpha(c)) {
                            std::string var;
                            while (i < expr.size() && (isalnum(expr[i]) || expr[i] == '_')) {
                                var += expr[i++];
                            }
                            i--;
                            tokens.emplace(var);
                        } else {
                            throw std::runtime_error("Invalid character: " + std::string(1, c));
                        }
                }
            }
        }

        if (!buffer.empty()) handle_buffer(buffer, negative, tokens);
        return tokens;
    }

    void handle_buffer(std::string& buffer, bool negative, std::queue<Token>& tokens) {
        double num = stod(buffer);
        if (negative) num = -num;
        tokens.emplace(num);
    }

    std::queue<Token> shunting_yard(std::queue<Token> tokens) {
        std::queue<Token> output;
        std::stack<Token> ops;

        while (!tokens.empty()) {
            Token token = tokens.front();
            tokens.pop();

            switch(token.type) {
                case TokenType::Number:
                case TokenType::Variable:
                    output.push(token); break;

                case TokenType::Operator:
                    while (!ops.empty() && is_higher_precedence(ops.top(), token)) {
                        output.push(ops.top());
                        ops.pop();
                    }
                    ops.push(token);
                    break;

                case TokenType::LeftParen:
                    ops.push(token); break;

                case TokenType::RightParen:
                    while (!ops.empty() && ops.top().type != TokenType::LeftParen) {
                        output.push(ops.top());
                        ops.pop();
                    }
                    if (ops.empty()) throw std::runtime_error("Mismatched parentheses");
                    ops.pop();
                    break;

                default: break;
            }
        }

        while (!ops.empty()) {
            output.push(ops.top());
            ops.pop();
        }
        return output;
    }

    bool is_higher_precedence(const Token& op1, const Token& op2) {
        if (op1.type != TokenType::Operator) return false;
        return (op1.operator_symbol == '*' || op1.operator_symbol == '/') &&
               (op2.operator_symbol == '+' || op2.operator_symbol == '-');
    }

    bool process_assignments(std::queue<Token>& tokens, std::map<std::string, double>& vars) {
        if (tokens.size() < 3) return false;
        
        // Сохраняем исходные токены для восстановления в случае ошибки
        std::queue<Token> original_tokens = tokens;
        
        try {
            Token var_token = tokens.front();
            if (var_token.type != TokenType::Variable) return false;
            std::string var = var_token.variable_name;
            tokens.pop();

            if (tokens.front().type != TokenType::Assignment) {
                tokens = original_tokens;
                return false;
            }
            tokens.pop();

            if (tokens.empty()) {
                throw std::runtime_error("Missing value after assignment");
            }

            auto postfix = shunting_yard(tokens);
            double value = evaluate(postfix, vars);
            vars[var] = value;
            last_assigned_var_ = var;
            tokens = {}; // Полностью очищаем токены
            return true;

        } catch (...) {
            tokens = original_tokens; // Восстанавливаем исходные токены
            last_assigned_var_.clear();
            throw;
        }
    }

    double evaluate(std::queue<Token> postfix, std::map<std::string, double>& vars) {
        std::stack<double> stack;
        
        while (!postfix.empty()) {
            Token token = postfix.front();
            postfix.pop();

            switch(token.type) {
                case TokenType::Number:
                    stack.push(token.number_value); break;
                    
                case TokenType::Variable: {
                    auto it = vars.find(token.variable_name);
                    if (it == vars.end()) {
                        throw std::runtime_error("Undefined variable: " + token.variable_name);
                    }
                    stack.push(it->second);
                    break;
                }
                    
                case TokenType::Operator: {
                    if (stack.size() < 2) throw std::runtime_error("Not enough operands");
                    double b = stack.top(); stack.pop();
                    double a = stack.top(); stack.pop();
                    
                    switch(token.operator_symbol) {
                        case '+': stack.push(a + b); break;
                        case '-': stack.push(a - b); break;
                        case '*': stack.push(a * b); break;
                        case '/': 
                            if (b == 0) throw std::runtime_error("Division by zero");
                            stack.push(a / b); break;
                    }
                    break;
                }
                
                default: break;
            }
        }
        
        if (stack.size() != 1) throw std::runtime_error("Invalid expression");
        return stack.top();
    }
};