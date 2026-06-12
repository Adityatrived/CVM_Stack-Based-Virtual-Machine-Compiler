#pragma once
#include <string>
#include <vector>
#include <memory>
#include "lexer/Token.h"

// Forward declarations
class ASTVisitor;

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// Expressions
struct Expr : public ASTNode {};
struct LiteralExpr : public Expr { int value; LiteralExpr(int v) : value(v) {} void accept(ASTVisitor& v) override; };
struct VarExpr : public Expr { std::string name; VarExpr(std::string n) : name(n) {} void accept(ASTVisitor& v) override; };
struct BinaryExpr : public Expr { 
    std::unique_ptr<Expr> left, right; TokenType op;
    BinaryExpr(std::unique_ptr<Expr> l, TokenType o, std::unique_ptr<Expr> r) : left(std::move(l)), right(std::move(r)), op(o) {}
    void accept(ASTVisitor& v) override; 
};
struct AssignExpr : public Expr {
    std::string name; std::unique_ptr<Expr> value;
    AssignExpr(std::string n, std::unique_ptr<Expr> v) : name(n), value(std::move(v)) {}
    void accept(ASTVisitor& v) override;
};
struct InputExpr : public Expr { void accept(ASTVisitor& v) override; };

// Statements
struct Stmt : public ASTNode {};
struct ExprStmt : public Stmt { std::unique_ptr<Expr> expr; ExprStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {} void accept(ASTVisitor& v) override; };
struct PrintStmt : public Stmt { std::unique_ptr<Expr> expr; PrintStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {} void accept(ASTVisitor& v) override; };
struct VarDeclStmt : public Stmt { std::string name; std::unique_ptr<Expr> initializer; VarDeclStmt(std::string n, std::unique_ptr<Expr> i) : name(n), initializer(std::move(i)) {} void accept(ASTVisitor& v) override; };
struct BlockStmt : public Stmt { std::vector<std::unique_ptr<Stmt>> statements; BlockStmt(std::vector<std::unique_ptr<Stmt>> s) : statements(std::move(s)) {} void accept(ASTVisitor& v) override; };
struct IfStmt : public Stmt { 
    std::unique_ptr<Expr> condition; std::unique_ptr<Stmt> thenBranch, elseBranch;
    IfStmt(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> t, std::unique_ptr<Stmt> e) : condition(std::move(c)), thenBranch(std::move(t)), elseBranch(std::move(e)) {}
    void accept(ASTVisitor& v) override; 
};
struct WhileStmt : public Stmt { 
    std::unique_ptr<Expr> condition; std::unique_ptr<Stmt> body;
    WhileStmt(std::unique_ptr<Expr> c, std::unique_ptr<Stmt> b) : condition(std::move(c)), body(std::move(b)) {}
    void accept(ASTVisitor& v) override; 
};

class ASTVisitor {
public:
    virtual void visit(LiteralExpr* node) = 0; virtual void visit(VarExpr* node) = 0;
    virtual void visit(BinaryExpr* node) = 0; virtual void visit(AssignExpr* node) = 0;
    virtual void visit(InputExpr* node) = 0;  virtual void visit(ExprStmt* node) = 0;
    virtual void visit(PrintStmt* node) = 0;  virtual void visit(VarDeclStmt* node) = 0;
    virtual void visit(BlockStmt* node) = 0;  virtual void visit(IfStmt* node) = 0;
    virtual void visit(WhileStmt* node) = 0;
};

// Accept implementations (inline for brevity)
inline void LiteralExpr::accept(ASTVisitor& v) { v.visit(this); }
inline void VarExpr::accept(ASTVisitor& v) { v.visit(this); }
inline void BinaryExpr::accept(ASTVisitor& v) { v.visit(this); }
inline void AssignExpr::accept(ASTVisitor& v) { v.visit(this); }
inline void InputExpr::accept(ASTVisitor& v) { v.visit(this); }
inline void ExprStmt::accept(ASTVisitor& v) { v.visit(this); }
inline void PrintStmt::accept(ASTVisitor& v) { v.visit(this); }
inline void VarDeclStmt::accept(ASTVisitor& v) { v.visit(this); }
inline void BlockStmt::accept(ASTVisitor& v) { v.visit(this); }
inline void IfStmt::accept(ASTVisitor& v) { v.visit(this); }
inline void WhileStmt::accept(ASTVisitor& v) { v.visit(this); }