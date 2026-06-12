#include "vm/VM.h"
#include <iostream>
#include <cstring>

// ---CONSTRUCTOR ---
VM::VM() {
    variables.resize(256, 0); 
}
// ----------------------------

void VM::load(const std::vector<uint8_t>& bytecode) {
    code = bytecode;
    ip = 0;
    variables.resize(256, 0); // Support up to 256 local variables for simplicity
    stack.clear();
}

uint8_t VM::readByte() { return code[ip++]; }

int VM::readInt() {
    int value;
    std::memcpy(&value, &code[ip], 4);
    ip += 4;
    return value;
}

void VM::push(int value) { stack.push_back(value); }

int VM::pop() { 
    int val = stack.back(); 
    stack.pop_back(); 
    return val; 
}

void VM::run() {
    while (ip < code.size()) {
        Opcode op = static_cast<Opcode>(readByte());
        switch (op) {
            case Opcode::PUSH: push(readInt()); break;
            case Opcode::ADD: { int b = pop(); int a = pop(); push(a + b); break; }
            case Opcode::SUB: { int b = pop(); int a = pop(); push(a - b); break; }
            case Opcode::MUL: { int b = pop(); int a = pop(); push(a * b); break; }
            case Opcode::DIV: { int b = pop(); int a = pop(); push(a / b); break; }
            case Opcode::EQ:  { int b = pop(); int a = pop(); push(a == b); break; }
            case Opcode::LT:  { int b = pop(); int a = pop(); push(a < b); break; }
            case Opcode::GT:  { int b = pop(); int a = pop(); push(a > b); break; }
            case Opcode::LTE: { int b = pop(); int a = pop(); push(a <= b); break; }
            case Opcode::GTE: { int b = pop(); int a = pop(); push(a >= b); break; }
            case Opcode::LOAD: push(variables[readByte()]); break;
            case Opcode::STORE: variables[readByte()] = pop(); break;
            case Opcode::JMP: ip = readInt(); break;
            case Opcode::JMP_IF_FALSE: {
                int target = readInt();
                if (pop() == 0) ip = target;
                break;
            }
            case Opcode::PRINT: std::cout << pop() << std::endl; break;
            case Opcode::INPUT: {
                int val; 
                std::cout << "> "; std::cin >> val; push(val);
                break;
            }
            case Opcode::HALT: return;
        }
    }
}