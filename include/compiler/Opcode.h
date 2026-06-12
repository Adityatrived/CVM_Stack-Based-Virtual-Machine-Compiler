#pragma once
#include <cstdint>

enum class Opcode : uint8_t {
    PUSH,           // PUSH <4-byte int>
    ADD, SUB, MUL, DIV,
    EQ, LT, GT, LTE, GTE,
    LOAD,           // LOAD <1-byte index>
    STORE,          // STORE <1-byte index>
    JMP,            // JMP <4-byte address>
    JMP_IF_FALSE,   // JMP_IF_FALSE <4-byte address>
    PRINT,
    INPUT,
    HALT
};