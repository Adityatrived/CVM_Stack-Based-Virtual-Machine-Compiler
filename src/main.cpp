#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "compiler/Compiler.h"
#include "vm/VM.h"

// Notice we now pass compiler and vm by reference (&)
void execute(const std::string& source, Compiler& compiler, VM& vm, bool debugTokens, bool debugBytecode) {
    try {
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        if (debugTokens) {
            std::cout << "--- TOKENS ---\n";
            for (const auto& t : tokens) std::cout << "Type: " << (int)t.type << " Lexeme: " << t.lexeme << "\n";
        }

        Parser parser(tokens);
        auto ast = parser.parse();

        auto bytecode = compiler.compile(ast);
        if (debugBytecode) {
            std::cout << "--- BYTECODE SIZE ---\n" << bytecode.size() << " bytes\n";
        }

        vm.load(bytecode);
        vm.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void runREPL() {
    Compiler compiler; // PERSISTENT COMPILER: Remembers variable indexes
    VM vm;             // PERSISTENT VM: Remembers variable values

    std::string line;
    std::cout << "CVM++ REPL v1.0. Type 'exit' to quit.\n";
    while (true) {
        std::cout << "cvm> ";
        std::getline(std::cin, line);
        if (line == "exit") break;
        if (line.empty()) continue;
        execute(line, compiler, vm, false, false);
    }
}

void runFile(const std::string& path, bool debugTokens, bool debugBytecode) {
    std::ifstream file(path);
    if (!file.is_open()) { std::cerr << "Could not open file: " << path << std::endl; return; }
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    Compiler compiler;
    VM vm;
    execute(buffer.str(), compiler, vm, debugTokens, debugBytecode);
}

int main(int argc, char* argv[]) {
    bool debugTokens = false, debugBytecode = false;
    std::string filePath;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--tokens") debugTokens = true;
        else if (arg == "--bytecode") debugBytecode = true;
        else filePath = arg;
    }

    if (!filePath.empty()) runFile(filePath, debugTokens, debugBytecode);
    else runREPL();

    return 0;
}