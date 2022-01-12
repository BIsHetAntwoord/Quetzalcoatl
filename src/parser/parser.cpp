#include "parser/parser.hpp"
#include "parser/parseexception.hpp"

#include <sstream>
#include <limits>

Parser::Parser(Lexer& lexer, CompileInfo& compile_info, AstTable& ast) :
        lexer(lexer), compile_info(compile_info), ast(ast) {

}

Token Parser::next_token() {
    if(this->token_stack.size() > 0) {
        Token result = this->token_stack.back();
        this->token_stack.pop_back();
        return result;
    }
    return this->lexer.lex();
}

void Parser::unread(const Token& t) {
    this->token_stack.push_back(t);
}

void Parser::throwError(const Token& err_token, const std::vector<TokenType>& expected) {
    std::stringstream ss;
    ss << "unexpected ";
    if(err_token.type == TokenType::EOI)
        ss << "eof";
    else
        ss << err_token.raw;

    if(expected.size() > 0) {
        ss << ", expected ";
        bool first = true;
        for(TokenType t : expected) {
            if(first)
                first = false;
            else
                ss << ", ";
            ss << tokenTypeToString(t);
        }
    }

    this->compile_info.diagnostics.error(err_token.pos, ss.str());
    throw ParseException();
}

size_t Parser::parseAtom() {
    Token lookahead = this->next_token();

    if(lookahead.type == TokenType::LITERAL_INTEGER)
        return this->ast.addNode(AstNodeType::INTEGER_CONSTANT, lookahead.integer.type, lookahead.integer.value);
    else
        this->throwError(lookahead, {TokenType::LITERAL_INTEGER});
}

size_t Parser::parseExpr() {
    return this->parseAtom();
}

size_t Parser::parse() {
    //TODO, change to actual root
    try {
        size_t result = this->parseExpr();

        Token lookahead = this->next_token();
        if(lookahead.type != TokenType::EOI)
            this->throwError(lookahead, {TokenType::EOI});
        return result;
    }
    catch(const ParseException& p) {
        return INVALID_ASTNODE_ID;
    }
}