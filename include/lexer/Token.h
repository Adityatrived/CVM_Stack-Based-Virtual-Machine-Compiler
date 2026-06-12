#pragma once
#include <string>

enum class TokenType {
    // Literals and Identifiers
    NUMBER, IDENTIFIER,
    
    // Keywords
    LET, IF, ELSE, WHILE, PRINT, INPUT, TRUE, FALSE,
    
    // Operators
    PLUS, MINUS, STAR, SLASH, 
    EQUAL, EQUAL_EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL,
    
    // Syntax
    SEMICOLON, LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    
    // End of File
    EOF_TOKEN, ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};