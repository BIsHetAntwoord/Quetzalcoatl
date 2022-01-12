#ifndef _QUETZALCOATL_PARSER_PARSER_HPP
#define _QUETZALCOATL_PARSER_PARSER_HPP

#include <cstddef>

#include "lexer/lexer.hpp"
#include "frontend/compile_info.hpp"
#include "frontend/ast.hpp"

class Parser {
private:
    Lexer& lexer;
    CompileInfo& compile_info;
    AstTable& ast;

    size_t parseExpr();
public:
    Parser(Lexer&, CompileInfo&, AstTable&);

    size_t parse();
};

#endif
