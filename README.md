# CVM++ (Stack-Based Virtual Machine & Compiler)

A lightweight, strictly-typed academic scripting language, compiled and executed on a custom stack-based Virtual Machine. 

## Features
- **Lexical & Syntax Analysis**: Custom Lexer and Recursive Descent Parser.
- **Code Generation**: Translates AST directly into a custom Bytecode ISA.
- **Virtual Machine**: Fast, stack-based execution runtime.
- **Language**: Supports `int` & `bool` abstractions, local variables (`let`), control flow (`if/else`, `while`), and I/O (`print()`, `input()`).

## Architecture
Source Code -> Lexer -> Tokens -> Parser -> AST -> Compiler -> CVM Bytecode -> Stack VM

## Build Instructions
1. `mkdir build && cd build`
2. `cmake ..`
3. `make`

## Usage
**Run REPL:**
`./cvm`

**Run File:**
`./cvm ../examples/loops.cvm`

**Debug Flags:**
`./cvm --tokens --bytecode ../examples/loops.cvm`
