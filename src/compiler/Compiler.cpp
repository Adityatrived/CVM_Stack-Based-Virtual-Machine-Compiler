#include "compiler/Compiler.h"
#include <stdexcept>
#include <cstring>

std::vector<uint8_t> Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& ast) {
    bytecode.clear();
    for (const auto& stmt : ast) stmt->accept(*this);
    emitOpcode(Opcode::HALT);
    return bytecode;
}

void Compiler::emitByte(uint8_t byte) { bytecode.push_back(byte); }
void Compiler::emitOpcode(Opcode op) { emitByte(static_cast<uint8_t>(op)); }
void Compiler::emitInt(int value) {
    uint8_t bytes[4];
    std::memcpy(bytes, &value, 4);
    for (int i=0; i<4; ++i) emitByte(bytes[i]);
}

uint32_t Compiler::emitJump(Opcode op) {
    emitOpcode(op);
    emitInt(0); // placeholder
    return static_cast<uint32_t>(bytecode.size() - 4);
}

void Compiler::patchJump(uint32_t offset) {
    int jumpAddress = static_cast<int>(bytecode.size());
    std::memcpy(&bytecode[offset], &jumpAddress, 4);
}

uint8_t Compiler::resolveVariable(const std::string& name) {
    if (variables.find(name) == variables.end()) {
        variables[name] = static_cast<uint8_t>(variables.size());
    }
    return variables[name];
}

void Compiler::visit(LiteralExpr* node) { emitOpcode(Opcode::PUSH); emitInt(node->value); }
void Compiler::visit(VarExpr* node) { emitOpcode(Opcode::LOAD); emitByte(resolveVariable(node->name)); }
void Compiler::visit(BinaryExpr* node) {
    node->left->accept(*this);
    node->right->accept(*this);
    switch (node->op) {
        case TokenType::PLUS: emitOpcode(Opcode::ADD); break;
        case TokenType::MINUS: emitOpcode(Opcode::SUB); break;
        case TokenType::STAR: emitOpcode(Opcode::MUL); break;
        case TokenType::SLASH: emitOpcode(Opcode::DIV); break;
        case TokenType::EQUAL_EQUAL: emitOpcode(Opcode::EQ); break;
        case TokenType::LESS: emitOpcode(Opcode::LT); break;
        case TokenType::GREATER: emitOpcode(Opcode::GT); break;
        case TokenType::LESS_EQUAL: emitOpcode(Opcode::LTE); break;
        case TokenType::GREATER_EQUAL: emitOpcode(Opcode::GTE); break;
        default: throw std::runtime_error("Unknown operator");
    }
}
void Compiler::visit(AssignExpr* node) {
    node->value->accept(*this);
    emitOpcode(Opcode::STORE);
    emitByte(resolveVariable(node->name));
}
void Compiler::visit(InputExpr* node) { emitOpcode(Opcode::INPUT); }
void Compiler::visit(ExprStmt* node) { node->expr->accept(*this); }
void Compiler::visit(PrintStmt* node) { node->expr->accept(*this); emitOpcode(Opcode::PRINT); }
void Compiler::visit(VarDeclStmt* node) {
    node->initializer->accept(*this);
    emitOpcode(Opcode::STORE);
    emitByte(resolveVariable(node->name));
}
void Compiler::visit(BlockStmt* node) { for (const auto& s : node->statements) s->accept(*this); }
void Compiler::visit(IfStmt* node) {
    node->condition->accept(*this);
    uint32_t thenJump = emitJump(Opcode::JMP_IF_FALSE);
    node->thenBranch->accept(*this);
    if (node->elseBranch) {
        uint32_t elseJump = emitJump(Opcode::JMP);
        patchJump(thenJump);
        node->elseBranch->accept(*this);
        patchJump(elseJump);
    } else {
        patchJump(thenJump);
    }
}
void Compiler::visit(WhileStmt* node) {
    uint32_t loopStart = static_cast<uint32_t>(bytecode.size());
    
    node->condition->accept(*this);
    uint32_t exitJump = emitJump(Opcode::JMP_IF_FALSE);
    node->body->accept(*this);
    emitOpcode(Opcode::JMP);
    emitInt(loopStart);
    patchJump(exitJump);
}