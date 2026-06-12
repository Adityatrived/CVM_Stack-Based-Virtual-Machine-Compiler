#pragma once
#include "ast/AST.h"
#include "Opcode.h"
#include <vector>
#include <unordered_map>
#include <cstdint>

class Compiler : public ASTVisitor {
public:
    std::vector<uint8_t> compile(const std::vector<std::unique_ptr<Stmt>>& ast);

    // ASTVisitor Overrides
    void visit(LiteralExpr* node) override; void visit(VarExpr* node) override;
    void visit(BinaryExpr* node) override;  void visit(AssignExpr* node) override;
    void visit(InputExpr* node) override;   void visit(ExprStmt* node) override;
    void visit(PrintStmt* node) override;   void visit(VarDeclStmt* node) override;
    void visit(BlockStmt* node) override;   void visit(IfStmt* node) override;
    void visit(WhileStmt* node) override;

private:
    std::vector<uint8_t> bytecode;
    std::unordered_map<std::string, uint8_t> variables;

    void emitByte(uint8_t byte);
    void emitOpcode(Opcode op);
    void emitInt(int value);
    uint32_t emitJump(Opcode op);
    void patchJump(uint32_t offset);
    uint8_t resolveVariable(const std::string& name);
};