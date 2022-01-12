#include "parser/parser.hpp"

Parser::Parser(Lexer& lexer, CompileInfo& compile_info, AstTable& ast) :
        lexer(lexer), compile_info(compile_info), ast(ast) {

}

size_t Parser::parseExpr() {
    return this->ast.addNode(AstNodeType::INVALID);
}

size_t Parser::parse() {
    //TODO, change to actual root
    return this->parseExpr();
}