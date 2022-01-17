#include "parser/parser.hpp"
#include "parser/parseexception.hpp"

#include <sstream>
#include <limits>

#include <iostream>

Parser::Parser(Lexer& lexer, CompileInfo& compile_info, AstTable& ast) :
        lexer(lexer), compile_info(compile_info), ast(ast), nearest_switch(INVALID_ASTNODE_ID) {

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

std::vector<size_t> Parser::parseList(TokenType expected_end) {
    Token lookahead = this->peek_token();

    std::vector<size_t> result;
    bool first = true;

    while(lookahead.type != expected_end) {
        if(first)
            first = false;
        else
            this->expect(TokenType::COMMA);

        size_t next_expr = this->parseAssign();
        result.push_back(next_expr);

        lookahead = this->peek_token();
    }

    this->consume();
    return result;
}

size_t Parser::parseAtom() {
    Token lookahead = this->next_token();

    switch(lookahead.type) {
        case TokenType::LITERAL_INTEGER:
            return this->ast.addIntegerNode(AstNodeType::INTEGER_CONSTANT, lookahead.integer.type, lookahead.integer.value);
        case TokenType::OPEN_PAR: {
            size_t result = this->parseExpr();
            this->expect(TokenType::CLOSE_PAR);
            return result;
        }
        default:
            this->throwError(lookahead, {TokenType::LITERAL_INTEGER});
    }
}

size_t Parser::parsePostfix() {
    size_t lop = this->parseAtom();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::INCREMENT ||
            lookahead.type == TokenType::DECREMENT ||
            lookahead.type == TokenType::OPEN_SB ||
            lookahead.type == TokenType::OPEN_PAR) {
        this->consume();
        switch(lookahead.type) {
            case TokenType::INCREMENT:
                lop = this->ast.addNode(AstNodeType::POSTFIX_INCREMENT_EXPR, {lop});
                break;
            case TokenType::DECREMENT:
                lop = this->ast.addNode(AstNodeType::POSTFIX_DECREMENT_EXPR, {lop});
                break;
            case TokenType::OPEN_PAR: {
                auto args = this->parseList(TokenType::CLOSE_PAR);
                args.insert(args.begin(), lop);
                lop = this->ast.addNode(AstNodeType::CALL_EXPR, args);
                break;
            }
            case TokenType::OPEN_SB:
                lop = this->ast.addNode(AstNodeType::SUBSCRIPT_EXPR, {lop, this->parseExpr()});
                this->expect(TokenType::CLOSE_SB);
                break;
            default:
                break;
        }

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parsePrefix(bool disable_ccast) {
    Token lookahead = this->peek_token();

    //TODO: implement c-style casts and disable them when the flag is set

    switch(lookahead.type) {
        case TokenType::INCREMENT:
            this->consume();
            return this->ast.addNode(AstNodeType::PREFIX_INCREMENT_EXPR, {this->parsePrefix()});
        case TokenType::DECREMENT:
            this->consume();
            return this->ast.addNode(AstNodeType::PREFIX_DECREMENT_EXPR, {this->parsePrefix()});
        case TokenType::PLUS:
            this->consume();
            return this->ast.addNode(AstNodeType::UNARY_PLUS_EXPR, {this->parsePrefix()});
        case TokenType::MINUS:
            this->consume();
            return this->ast.addNode(AstNodeType::UNARY_MINUS_EXPR, {this->parsePrefix()});
        case TokenType::NOT:
            this->consume();
            return this->ast.addNode(AstNodeType::LOGICAL_NOT_EXPR, {this->parsePrefix()});
        case TokenType::BITNOT:
            this->consume();
            return this->ast.addNode(AstNodeType::BITWISE_NOT_EXPR, {this->parsePrefix()});
        case TokenType::STAR:
            this->consume();
            return this->ast.addNode(AstNodeType::DEREF_EXPR, {this->parsePrefix()});
        case TokenType::BITAND:
            this->consume();
            return this->ast.addNode(AstNodeType::ADDRESS_OF_EXPR, {this->parsePrefix()});
        case TokenType::KEY_SIZEOF:
            //TODO: parse datatype
            this->consume();
            return this->ast.addNode(AstNodeType::SIZEOF_EXPR, {this->parsePrefix(true)});
        default:
            return this->parsePostfix();
    }
}

size_t Parser::parsePtrToMember() {
    size_t lop = this->parsePrefix();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::DOTSTAR ||
        lookahead.type == TokenType::STARROW) {
        this->consume();

        size_t rop = this->parsePrefix();

        switch(lookahead.type) {
            case TokenType::DOTSTAR:
                lop = this->ast.addNode(AstNodeType::POINTER_TO_MEMBER_EXPR, {lop, rop});
                break;
            case TokenType::STARROW:
                lop = this->ast.addNode(AstNodeType::INDIRECT_POINTER_TO_MEMBER_EXPR, {lop, rop});
                break;
            default:
                break;
        }

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseMultiply() {
    size_t lop = this->parsePtrToMember();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::STAR ||
        lookahead.type == TokenType::DIV ||
        lookahead.type == TokenType::MOD) {
        this->consume();

        size_t rop = this->parsePtrToMember();

        switch(lookahead.type) {
            case TokenType::STAR:
                lop = this->ast.addNode(AstNodeType::MUL_EXPR, {lop, rop});
                break;
            case TokenType::DIV:
                lop = this->ast.addNode(AstNodeType::DIV_EXPR, {lop, rop});
                break;
            case TokenType::MOD:
                lop = this->ast.addNode(AstNodeType::MOD_EXPR, {lop, rop});
                break;
            default:
                break;
        }

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseAdd() {
    size_t lop = this->parseMultiply();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::PLUS ||
        lookahead.type == TokenType::MINUS) {
        this->consume();

        size_t rop = this->parseMultiply();

        switch(lookahead.type) {
            case TokenType::PLUS:
                lop = this->ast.addNode(AstNodeType::ADD_EXPR, {lop, rop});
                break;
            case TokenType::MINUS:
                lop = this->ast.addNode(AstNodeType::SUB_EXPR, {lop, rop});
                break;
            default:
                break;
        }

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseShift() {
    size_t lop = this->parseAdd();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::LSHIFT ||
        lookahead.type == TokenType::RSHIFT) {
        this->consume();

        size_t rop = this->parseAdd();

        switch(lookahead.type) {
            case TokenType::LSHIFT:
                lop = this->ast.addNode(AstNodeType::LSHIFT_EXPR, {lop, rop});
                break;
            case TokenType::RSHIFT:
                lop = this->ast.addNode(AstNodeType::RSHIFT_EXPR, {lop, rop});
                break;
            default:
                break;
        }

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseRelational() {
    size_t lop = this->parseShift();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::LESS ||
        lookahead.type == TokenType::GREATER ||
        lookahead.type == TokenType::LESSEQ ||
        lookahead.type == TokenType::GREATEREQ) {
        this->consume();

        size_t rop = this->parseShift();

        switch(lookahead.type) {
            case TokenType::LESS:
                lop = this->ast.addNode(AstNodeType::LESS_EXPR, {lop, rop});
                break;
            case TokenType::GREATER:
                lop = this->ast.addNode(AstNodeType::GREATER_EXPR, {lop, rop});
                break;
            case TokenType::LESSEQ:
                lop = this->ast.addNode(AstNodeType::LESSEQ_EXPR, {lop, rop});
                break;
            case TokenType::GREATEREQ:
                lop = this->ast.addNode(AstNodeType::GREATEREQ_EXPR, {lop, rop});
                break;
            default:
                break;
        }

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseEquality() {
    size_t lop = this->parseRelational();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::EQUAL ||
        lookahead.type == TokenType::NOTEQUAL) {
        this->consume();

        size_t rop = this->parseRelational();

        switch(lookahead.type) {
            case TokenType::EQUAL:
                lop = this->ast.addNode(AstNodeType::EQUAL_EXPR, {lop, rop});
                break;
            case TokenType::NOTEQUAL:
                lop = this->ast.addNode(AstNodeType::NOTEQUAL_EXPR, {lop, rop});
                break;
            default:
                break;
        }

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseBitand() {
    size_t lop = this->parseEquality();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::BITAND) {
        this->consume();

        size_t rop = this->parseEquality();
        lop = this->ast.addNode(AstNodeType::BITWISE_AND_EXPR, {lop, rop});

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseBitxor() {
    size_t lop = this->parseBitand();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::XOR) {
        this->consume();

        size_t rop = this->parseBitand();
        lop = this->ast.addNode(AstNodeType::BITWISE_XOR_EXPR, {lop, rop});

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseBitor() {
    size_t lop = this->parseBitxor();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::BITOR) {
        this->consume();

        size_t rop = this->parseBitxor();
        lop = this->ast.addNode(AstNodeType::BITWISE_OR_EXPR, {lop, rop});

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseAnd() {
    size_t lop = this->parseBitor();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::AND) {
        this->consume();

        size_t rop = this->parseBitor();
        lop = this->ast.addNode(AstNodeType::LOGICAL_AND_EXPR, {lop, rop});

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseOr() {
    size_t lop = this->parseAnd();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::OR) {
        this->consume();

        size_t rop = this->parseAnd();
        lop = this->ast.addNode(AstNodeType::LOGICAL_OR_EXPR, {lop, rop});

        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseAssign() {
    Token lookahead = this->peek_token();

    if(lookahead.type == TokenType::KEY_THROW) {
        this->consume();

        lookahead = this->peek_token();
        switch(lookahead.type) {
            //TODO: add other expression initial tokens
            case TokenType::LITERAL_INTEGER:
            case TokenType::INCREMENT:
            case TokenType::DECREMENT:
            case TokenType::OPEN_PAR:
            case TokenType::NOT:
            case TokenType::BITNOT:
            case TokenType::STAR:
            case TokenType::BITAND:
            case TokenType::KEY_SIZEOF:
            case TokenType::KEY_THROW:
                return this->ast.addNode(AstNodeType::THROW_EXPR, this->compile_info.types.getBaseTypeId(BaseDataType::VOID), {this->parseAssign()});
            default:
                return this->ast.addNode(AstNodeType::RETHROW_EXPR, this->compile_info.types.getBaseTypeId(BaseDataType::VOID));
        }
    }

    size_t lop = this->parseOr();

    lookahead = this->peek_token();
    switch(lookahead.type) {
        case TokenType::ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::ADD_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::ADD_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::SUB_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::SUB_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::MUL_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::MUL_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::DIV_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::DIV_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::MOD_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::MOD_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::LSHIFT_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::LSHIFT_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::RSHIFT_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::RSHIFT_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::BITAND_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::BITAND_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::BITOR_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::BITOR_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::XOR_ASSIGN:
            this->consume();
            lop = this->ast.addNode(AstNodeType::BITXOR_ASSIGN_EXPR, {lop, this->parseAssign()});
            break;
        case TokenType::QUESTION: {
            this->consume();
            size_t middle_op = this->parseExpr();
            this->expect(TokenType::COLON);
            size_t rop = this->parseAssign();
            lop = this->ast.addNode(AstNodeType::TERNARY_EXPR, {lop, middle_op, rop});
            break;
        }
        default:
            break;
    }
    return lop;
}

size_t Parser::parseComma() {
    size_t lop = this->parseAssign();

    Token lookahead = this->peek_token();
    while(lookahead.type == TokenType::COMMA) {
        this->consume();

        size_t rop = this->parseAssign();
        lop = this->ast.addNode(AstNodeType::COMMA_EXPR, {lop, rop});
        lookahead = this->peek_token();
    }
    return lop;
}

size_t Parser::parseExpr() {
    return this->parseComma();
}

size_t Parser::parseCompoundStatement() {
    this->expect(TokenType::OPEN_CB);
    size_t result = this->parseStatementList();
    this->expect(TokenType::CLOSE_CB);
    return result;
}

size_t Parser::parseIf() {
    this->expect(TokenType::KEY_IF);
    this->expect(TokenType::OPEN_PAR);

    size_t expr = this->parseExpr();

    this->expect(TokenType::CLOSE_PAR);

    size_t stat = this->parseStatement();

    Token lookahead = this->peek_token();
    if(lookahead.type == TokenType::KEY_ELSE) {
        this->consume();

        size_t else_stat = this->parseStatement();
        return this->ast.addNode(AstNodeType::IF_ELSE_STAT, {expr, stat, else_stat});
    }
    else
        return this->ast.addNode(AstNodeType::IF_STAT, {expr, stat});
}

size_t Parser::parseSwitch() {
    std::cout << "Making switch " << std::endl;
    size_t switch_stat = this->ast.addSwitchNode(AstNodeType::SWITCH_STAT);
    std::cout << "Allocated switch" << std::endl;

    this->expect(TokenType::KEY_SWITCH);
    this->expect(TokenType::OPEN_PAR);
    size_t expr = this->parseExpr();
    this->expect(TokenType::CLOSE_PAR);

    std::cout << "Parsed expression" << std::endl;

    size_t old_switch_stat = this->nearest_switch;
    this->nearest_switch = switch_stat;

    size_t stat = this->parseStatement();

    std::cout << "Parsed statement" << std::endl;

    AstNode& switch_node = this->ast.getNode(switch_stat);
    switch_node.children.push_back(expr);
    switch_node.children.push_back(stat);

    this->nearest_switch = old_switch_stat;

    std::cout << "Returning" << std::endl;
    return switch_stat;
}

size_t Parser::parseDefault() {
    Token def_tok = this->expect(TokenType::KEY_DEFAULT);
    this->expect(TokenType::COLON);

    if(this->nearest_switch == INVALID_ASTNODE_ID) {
        this->compile_info.diagnostics.error(def_tok.pos, "default outside of switch");
        throw ParseException();
    }

    size_t def_node = this->ast.addNode(AstNodeType::DEFAULT_LABEL);
    SwitchAstNode& switch_node = (SwitchAstNode&)this->ast.getNode(this->nearest_switch);
    if(switch_node.default_id != INVALID_ASTNODE_ID) {
        this->compile_info.diagnostics.error(def_tok.pos, "multiple default in switch");
        throw ParseException();
    }

    switch_node.default_id = def_node;
    return def_node;
}

size_t Parser::parseCase() {
    Token case_tok = this->expect(TokenType::KEY_CASE);
    size_t case_expr = this->parseExpr();
    this->expect(TokenType::COLON);

    if(this->nearest_switch == INVALID_ASTNODE_ID) {
        this->compile_info.diagnostics.error(case_tok.pos, "case outside of switch");
        throw ParseException();
    }

    size_t case_node = this->ast.addNode(AstNodeType::CASE_LABEL, {case_expr});
    SwitchAstNode& switch_node = (SwitchAstNode&)this->ast.getNode(this->nearest_switch);
    switch_node.case_nodes.push_back(case_node);

    //TODO: check for multiple identical switch cases
    return case_node;
}

size_t Parser::parseStatement() {
    Token lookahead = this->peek_token();
    //TODO: add lookahead for various other statement types
    switch(lookahead.type) {
        //TODO: add other expression initial tokens
        case TokenType::LITERAL_INTEGER:
        case TokenType::INCREMENT:
        case TokenType::DECREMENT:
        case TokenType::OPEN_PAR:
        case TokenType::NOT:
        case TokenType::BITNOT:
        case TokenType::STAR:
        case TokenType::BITAND:
        case TokenType::KEY_SIZEOF:
        case TokenType::KEY_THROW: {
            size_t expr = this->parseExpr();
            this->expect(TokenType::SEMICOLON);
            return this->ast.addNode(AstNodeType::EXPR_STAT, {expr});
        }
        case TokenType::SEMICOLON:
            this->consume();
            return this->ast.addNode(AstNodeType::EMPTY_STAT);
        case TokenType::OPEN_CB:
            return this->parseCompoundStatement();
        case TokenType::KEY_IF:
            return this->parseIf();
        case TokenType::KEY_SWITCH:
            return this->parseSwitch();
        case TokenType::KEY_DEFAULT:
            return this->parseDefault();
        case TokenType::KEY_CASE:
            return this->parseCase();
        default:
            this->throwError(lookahead, {TokenType::LITERAL_INTEGER, TokenType::SEMICOLON});
    }
}

size_t Parser::parseStatementList() {
    Token lookahead = this->peek_token();
    std::vector<size_t> children;

    //TODO: update lookaheads to all possible statement starters
    //TODO: add other expression initial tokens
    while(lookahead.type == TokenType::LITERAL_INTEGER
            || lookahead.type == TokenType::INCREMENT
            || lookahead.type == TokenType::DECREMENT
            || lookahead.type == TokenType::OPEN_PAR
            || lookahead.type == TokenType::NOT
            || lookahead.type == TokenType::BITNOT
            || lookahead.type == TokenType::STAR
            || lookahead.type == TokenType::BITAND
            || lookahead.type == TokenType::KEY_SIZEOF
            || lookahead.type == TokenType::KEY_THROW
            || lookahead.type == TokenType::SEMICOLON
            || lookahead.type == TokenType::OPEN_CB
            || lookahead.type == TokenType::KEY_IF
            || lookahead.type == TokenType::KEY_SWITCH
            || lookahead.type == TokenType::KEY_DEFAULT
            || lookahead.type == TokenType::KEY_CASE) {
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