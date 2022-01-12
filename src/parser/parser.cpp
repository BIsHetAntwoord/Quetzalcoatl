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

Token Parser::peek_token() {
    if(this->token_stack.size() > 0)
        return this->token_stack.back();
    Token result = this->lexer.lex();
    this->token_stack.push_back(result);
    return result;
}

void Parser::unread(const Token& t) {
    this->token_stack.push_back(t);
}

Token Parser::expect(TokenType token) {
    Token lookahead = this->next_token();
    if(lookahead.type != token)
        this->throwError(lookahead, {token});
    return lookahead;
}

void Parser::consume() {
    this->next_token();
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

size_t Parser::parseComma() {
    size_t lop = this->parseAtom();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::COMMA) {
        this->consume();

        size_t rop = this->parseAtom();
        lop = this->ast.addNode(AstNodeType::COMMA_EXPR, {lop, rop});
        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseExpr() {
    return this->parseComma();
}

size_t Parser::parseStatement() {
    Token lookahead = this->peek_token();
    //TODO: add lookahead for various other statement types
    switch(lookahead.type) {
        case TokenType::LITERAL_INTEGER: {
            size_t expr = this->parseExpr();
            this->expect(TokenType::SEMICOLON);
            return this->ast.addNode(AstNodeType::EXPR_STAT, {expr});
        }
        case TokenType::SEMICOLON:
            this->consume();
            return this->ast.addNode(AstNodeType::EMPTY_STAT);
        default:
            this->throwError(lookahead, {TokenType::LITERAL_INTEGER, TokenType::SEMICOLON});
    }
}

size_t Parser::parseStatementList() {
    //TODO: update lookaheads to all possible statement starters
    Token lookahead = this->peek_token();
    std::vector<size_t> children;
    while(lookahead.type == TokenType::LITERAL_INTEGER
            || lookahead.type == TokenType::SEMICOLON) {
        size_t sub_stat = this->parseStatement();
        children.push_back(sub_stat);

        lookahead = this->peek_token();
    }
    return this->ast.addNode(AstNodeType::STATEMENT_LIST, children);
}

size_t Parser::parse() {
    //TODO, change to actual root
    try {
        size_t result = this->parseStatementList();

        Token lookahead = this->next_token();
        if(lookahead.type != TokenType::EOI)
            this->throwError(lookahead, {TokenType::EOI});
        return result;
    }
    catch(const ParseException& p) {
        return INVALID_ASTNODE_ID;
    }
}