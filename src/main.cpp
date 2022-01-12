#include "lexer/lexer.hpp"
#include "frontend/filetable.hpp"
#include "frontend/stringtable.hpp"
#include "unicode.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>

int main(int argc, char* argv[]) {
    if(argc < 2)
        return 1;
    std::ifstream input(argv[1]);
    std::stringstream ss;
    ss << input.rdbuf();

    std::string input_str = ss.str();

    CompileInfo compile_info;
    Lexer lexer(input_str, compile_info);

    Token tok;
    do {
        tok = lexer.lex();

        std::cout << tokenTypeToString(tok.type) << " (\"" << tok.raw << "\", ";

        if(tok.type == TokenType::LITERAL_INTEGER) {
            std::cout << "(" << tok.integer.value << ", " << tok.integer.type << "), ";
        }

        std::cout << tok.pos.line
            << ", " << tok.pos.column << ", " << compile_info.files.getFile(tok.pos.file_id) << ")" << std::endl;
    } while(tok.type != TokenType::EOI);

    for (auto e : lexer.compileErrors()) {
        std::cout << "error: " << e.msg << '\n';
    }

    return 0;
}
