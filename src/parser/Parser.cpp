#include "parser/Parser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::vector<std::unique_ptr<Stmt>> Parser::parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    //while (!check(TokenType::EOF_TOKEN)) statements.push_back(declaration());
    while (!isAtEnd()) { 
        statements.push_back(declaration());
    }
    return statements;
}

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::LET})) return varDeclaration();
    return statement();
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
    consume(TokenType::EQUAL, "Expect '=' after variable name.");
    auto initializer = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after declaration.");
    return std::make_unique<VarDeclStmt>(name.lexeme, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement() {
    if (match({TokenType::IF})) return ifStatement();
    if (match({TokenType::WHILE})) return whileStatement();
    if (match({TokenType::LEFT_BRACE})) return block();
    if (match({TokenType::PRINT})) return printStatement();
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");
    auto thenBranch = statement();
    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ELSE})) elseBranch = statement();
    return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
    auto body = statement();
    return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::unique_ptr<Stmt> Parser::block() {
    std::vector<std::unique_ptr<Stmt>> stmts;
    //while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) stmts.push_back(declaration());

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        stmts.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return std::make_unique<BlockStmt>(std::move(stmts));
}

std::unique_ptr<Stmt> Parser::printStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'print'.");
    auto expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after print expression.");
    consume(TokenType::SEMICOLON, "Expect ';' after print statement.");
    return std::make_unique<PrintStmt>(std::move(expr));
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExprStmt>(std::move(expr));
}

std::unique_ptr<Expr> Parser::expression() { return assignment(); }

std::unique_ptr<Expr> Parser::assignment() {
    auto expr = equality();
    if (match({TokenType::EQUAL})) {
        auto value = assignment();
        if (auto varExpr = dynamic_cast<VarExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(varExpr->name, std::move(value));
        }
        throw std::runtime_error("Invalid assignment target.");
    }
    return expr;
}

std::unique_ptr<Expr> Parser::equality() {
    auto expr = comparison();
    while (match({TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        auto right = comparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
    auto expr = term();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        auto right = term();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = primary();
    while (match({TokenType::SLASH, TokenType::STAR})) {
        Token op = previous();
        auto right = primary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::FALSE})) return std::make_unique<LiteralExpr>(0);
    if (match({TokenType::TRUE})) return std::make_unique<LiteralExpr>(1);
    if (match({TokenType::NUMBER})) return std::make_unique<LiteralExpr>(std::stoi(previous().lexeme));
    if (match({TokenType::IDENTIFIER})) return std::make_unique<VarExpr>(previous().lexeme);
    if (match({TokenType::INPUT})) {
        consume(TokenType::LEFT_PAREN, "Expect '('");
        consume(TokenType::RIGHT_PAREN, "Expect ')'");
        return std::make_unique<InputExpr>();
    }
    if (match({TokenType::LEFT_PAREN})) {
        auto expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return expr;
    }
    throw std::runtime_error("Expected expression.");
}

// Utility Methods
bool Parser::match(std::vector<TokenType> types) {
    for (auto type : types) { if (check(type)) { advance(); return true; } }
    return false;
}
bool Parser::check(TokenType type) const { return isAtEnd() ? false : peek().type == type; }
Token Parser::advance() { if (!isAtEnd()) current++; return previous(); }
bool Parser::isAtEnd() const { return peek().type == TokenType::EOF_TOKEN; }
Token Parser::peek() const { return tokens[current]; }
Token Parser::previous() const { return tokens[current - 1]; }
Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message);
}