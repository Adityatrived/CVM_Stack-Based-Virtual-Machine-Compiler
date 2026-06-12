#include "lexer/Lexer.h"
#include <cctype>

Lexer::Lexer(const std::string& source) : source(source) {
    keywords["let"] = TokenType::LET;
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["while"] = TokenType::WHILE;
    keywords["print"] = TokenType::PRINT;
    keywords["input"] = TokenType::INPUT;
    keywords["true"] = TokenType::TRUE;
    keywords["false"] = TokenType::FALSE;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;
        start = current;
        
        char c = advance();
        if (std::isalpha(c)) { tokens.push_back(identifier()); continue; }
        if (std::isdigit(c)) { tokens.push_back(number()); continue; }
        
        switch (c) {
            case '(': tokens.push_back(makeToken(TokenType::LEFT_PAREN)); break;
            case ')': tokens.push_back(makeToken(TokenType::RIGHT_PAREN)); break;
            case '{': tokens.push_back(makeToken(TokenType::LEFT_BRACE)); break;
            case '}': tokens.push_back(makeToken(TokenType::RIGHT_BRACE)); break;
            case ';': tokens.push_back(makeToken(TokenType::SEMICOLON)); break;
            case '+': tokens.push_back(makeToken(TokenType::PLUS)); break;
            case '-': tokens.push_back(makeToken(TokenType::MINUS)); break;
            case '*': tokens.push_back(makeToken(TokenType::STAR)); break;
            case '/': tokens.push_back(makeToken(TokenType::SLASH)); break;
            case '=': tokens.push_back(makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL)); break;
            case '<': tokens.push_back(makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS)); break;
            case '>': tokens.push_back(makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER)); break;
            default: tokens.push_back(makeToken(TokenType::ERROR)); break;
        }
    }
    tokens.push_back({TokenType::EOF_TOKEN, "", line});
    return tokens;
}

void Lexer::skipWhitespaceAndComments() {
    while (true) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') advance();
        else if (c == '\n') { line++; advance(); }
        else if (c == '/' && peekNext() == '/') { while (peek() != '\n' && !isAtEnd()) advance(); }
        else break;
    }
}

bool Lexer::isAtEnd() const { return current >= source.length(); }
char Lexer::advance() { return source[current++]; }
char Lexer::peek() const { return isAtEnd() ? '\0' : source[current]; }
char Lexer::peekNext() const { return (current + 1 >= source.length()) ? '\0' : source[current + 1]; }
bool Lexer::match(char expected) { if (isAtEnd() || source[current] != expected) return false; current++; return true; }

Token Lexer::number() {
    while (std::isdigit(peek())) advance();
    return makeToken(TokenType::NUMBER);
}

Token Lexer::identifier() {
    while (std::isalnum(peek()) || peek() == '_') advance();
    std::string text = source.substr(start, current - start);
    if (keywords.find(text) != keywords.end()) return makeToken(keywords[text]);
    return makeToken(TokenType::IDENTIFIER);
}

Token Lexer::makeToken(TokenType type) { return {type, source.substr(start, current - start), line}; }