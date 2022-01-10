#include "lexer/lexer.hpp"
#include "lexer/filetable.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {
    if(argc < 2)
        return 1;
    std::ifstream input(argv[1]);
    std::stringstream ss;
    ss << input.rdbuf();

    std::string input_str = ss.str();

    FileTable filetab;
    Lexer lexer(input_str, filetab);

    Token tok;
    do {
        tok = lexer.lex();

        std::cout << size_t(tok.type) << " (\"" << tok.raw << "\", " << tok.pos.line
            << ", " << tok.pos.offset << ", " << filetab.getFile(tok.pos.file_id) << ")" << std::endl;
    } while(tok.type != TokenType::EOI);

    return 0;
}