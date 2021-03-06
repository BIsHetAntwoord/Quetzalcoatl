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
    size_t nearest_switch;

    Token next_token();
    Token peek_token();
    void unread(const Token&);
    void consume();

    Token expect(TokenType);

    [[noreturn]] void throwError(const Token&, const std::vector<TokenType>&);

    std::vector<size_t> parseList(TokenType);
    size_t parseAtom();
    size_t parsePostfix();
    size_t parsePrefix(bool = false);
    size_t parsePtrToMember();
    size_t parseMultiply();
    size_t parseAdd();
    size_t parseShift();
    size_t parseRelational();
    size_t parseEquality();
    size_t parseBitand();
    size_t parseBitxor();
    size_t parseBitor();
    size_t parseAnd();
    size_t parseOr();
    size_t parseAssign();
    size_t parseComma();
    size_t parseExpr();
    size_t parseCompoundStatement();
    size_t parseIf();
    size_t parseSwitch();
    size_t parseDefault();
    size_t parseCase();
    size_t parseCondition();
    size_t parseWhile();
    size_t parseDoWhile();
    size_t parseSimpleDecl();
    size_t parseForInit();
    size_t parseFor();
    size_t parseReturn();
    size_t parseStatement();
    size_t parseStatementList();
public:
    Parser(Lexer&, CompileInfo&, AstTable&);

    size_t parse();
};

#endif
