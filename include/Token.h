#pragma once
#include <string>

enum class TokenType {
    Number,
    Operator,
    LeftParen,
    RightParen,
    Variable,
    Assignment
};

struct Token {
    TokenType type;
    double number_value;
    char operator_symbol;
    std::string variable_name;

    Token(double val) : type(TokenType::Number), number_value(val) {}
    Token(char op) : type(TokenType::Operator), operator_symbol(op) {}
    Token(TokenType t) : type(t) {}
    Token(std::string var) : type(TokenType::Variable), variable_name(var) {}
};