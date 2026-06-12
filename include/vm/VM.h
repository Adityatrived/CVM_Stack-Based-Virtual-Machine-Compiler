#pragma once
#include <vector>
#include <cstdint>
#include "compiler/Opcode.h"

class VM {
public:
    VM();
    void load(const std::vector<uint8_t>& bytecode);
    void run();

private:
    std::vector<uint8_t> code;
    size_t ip = 0;
    std::vector<int> stack;
    std::vector<int> variables;

    uint8_t readByte();
    int readInt();
    void push(int value);
    int pop();
};