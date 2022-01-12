#ifndef _QUETZALCOATL_PARSER_PARSER_HPP
#define _QUETZALCOATL_PARSER_PARSER_HPP

#include <cstddef>
#include <vector>

#include "lexer/lexer.hpp"
#include "lexer/token.hpp"
#include "frontend/compile_info.hpp"
#include "frontend/ast.hpp"

class Parser {
private:
    Lexer& lexer;
    CompileInfo& compile_info;
    AstTable& ast;

    std::vector<Token> token_stack;

    Token next_token();
    void unread(const Token&);

    [[noreturn]] void throwError(const Token&, const std::vector<TokenType>&);

    size_t parseAtom();
    size_t parseExpr();
public:
    Parser(Lexer&, CompileInfo&, AstTable&);

    size_t parse();
};

#endif
