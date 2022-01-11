#include "lexer/lexer.hpp"

#include <sstream>
#include <string>
#include <unordered_map>
#include <cassert>
#include <iostream>

const std::unordered_map<std::string_view, TokenType> KEYWORD_TYPES = {
    {"and", TokenType::AND},
    {"and_eq", TokenType::BITAND_ASSIGN},
    {"asm", TokenType::KEY_ASM},
    {"auto", TokenType::KEY_AUTO},
    {"bitand", TokenType::BITAND},
    {"bitor", TokenType::BITOR},
    {"bool", TokenType::KEY_BOOL},
    {"break", TokenType::KEY_BREAK},
    {"case", TokenType::KEY_CASE},
    {"class", TokenType::KEY_CLASS},
    {"compl", TokenType::BITNOT},
    {"const", TokenType::KEY_CONST},
    {"const_cast", TokenType::KEY_CONST_CAST},
    {"continue", TokenType::KEY_CONTINUE},
    {"default", TokenType::KEY_DEFAULT},
    {"delete", TokenType::KEY_DELETE},
    {"do", TokenType::KEY_DO},
    {"double", TokenType::KEY_DOUBLE},
    {"dynamic_cast", TokenType::KEY_DYNAMIC_CAST},
    {"else", TokenType::KEY_ELSE},
    {"enum", TokenType::KEY_ENUM},
    {"explicit", TokenType::KEY_EXPLICIT},
    {"export", TokenType::KEY_EXPORT},
    {"extern", TokenType::KEY_EXTERN},
    {"false", TokenType::KEY_FALSE},
    {"float", TokenType::KEY_FLOAT},
    {"for", TokenType::KEY_FOR},
    {"friend", TokenType::KEY_FRIEND},
    {"goto", TokenType::KEY_GOTO},
    {"if", TokenType::KEY_IF},
    {"inline", TokenType::KEY_INLINE},
    {"int", TokenType::KEY_INT},
    {"long", TokenType::KEY_LONG},
    {"mutable", TokenType::KEY_MUTABLE},
    {"namespace", TokenType::KEY_NAMESPACE},
    {"new", TokenType::KEY_NEW},
    {"not", TokenType::NOT},
    {"not_eq", TokenType::NOTEQUAL},
    {"operator", TokenType::KEY_OPERATOR},
    {"or", TokenType::OR},
    {"or_eq", TokenType::BITOR_ASSIGN},
    {"private", TokenType::KEY_PRIVATE},
    {"protected", TokenType::KEY_PROTECTED},
    {"public", TokenType::KEY_PUBLIC},
    {"register", TokenType::KEY_REGISTER},
    {"reinterpret_cast", TokenType::KEY_REINTERPRET_CAST},
    {"return", TokenType::KEY_RETURN},
    {"short", TokenType::KEY_SHORT},
    {"signed", TokenType::KEY_SIGNED},
    {"sizeof", TokenType::KEY_SIZEOF},
    {"static", TokenType::KEY_STATIC},
    {"static_cast", TokenType::KEY_STATIC_CAST},
    {"struct", TokenType::KEY_STRUCT},
    {"switch", TokenType::KEY_SWITCH},
    {"template", TokenType::KEY_TEMPLATE},
    {"this", TokenType::KEY_THIS},
    {"throw", TokenType::KEY_THROW},
    {"true", TokenType::KEY_TRUE},
    {"try", TokenType::KEY_TRY},
    {"typedef", TokenType::KEY_TYPEDEF},
    {"typeid", TokenType::KEY_TYPEID},
    {"typename", TokenType::KEY_TYPENAME},
    {"union", TokenType::KEY_UNION},
    {"unsigned", TokenType::KEY_UNSIGNED},
    {"using", TokenType::KEY_USING},
    {"virtual", TokenType::KEY_VIRTUAL},
    {"void", TokenType::KEY_VOID},
    {"volatile", TokenType::KEY_VOLATILE},
    {"wchar_t", TokenType::KEY_WCHAR_T},
    {"while", TokenType::KEY_WHILE},
    {"xor", TokenType::XOR},
    {"xor_eq", TokenType::XOR_ASSIGN},
};

Lexer::Lexer(std::string_view input, FileTable& files) : input(input), input_offset(0),
        position({1, 0, 0}), token_start_offset(0), files(files) {
    this->files.addFile("<unknown>");
}

int Lexer::read() {
    if(this->input_offset >= this->input.size())
        return -1;
    ++this->position.column;
    return (unsigned)this->input[this->input_offset++];
}

void Lexer::unread(size_t num) {
    this->position.column -= num;
    --this->input_offset;
}

Token Lexer::makeToken(TokenType type) {
    Token result;
    result.type = type;
    result.pos = this->token_start;
    result.raw = this->tokenString();
    return result;
}

void Lexer::startToken() {
    this->token_start = this->position;
    this->token_start_offset = this->input_offset;
}

std::string_view Lexer::tokenString() {
    return this->input.substr(this->token_start_offset, this->input_offset - this->token_start_offset);
}

Token Lexer::error(SourceLocation loc, std::string_view msg) {
    this->errors.emplace_back(loc, msg);
    return this->makeToken(TokenType::INVALID);
}

bool Lexer::isIdChar(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || (c == '_');
}

bool Lexer::isDigit(int c) {
    return (c >= '0' && c <= '9');
}

bool Lexer::isHexDigit(int c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'z');
}

void Lexer::consumeLine() {
    int lookahead = this->read();
    while(lookahead != '\n' && lookahead != -1)
        lookahead = this->read();
    this->unread();
}

void Lexer::consumeMultiline() {
    int lookahead = this->read();
    while(lookahead != -1) {
        if(lookahead == '*') {
            lookahead = this->read();
            if(lookahead == '/')
                return;
        }

        if(lookahead != -1)
            lookahead = this->read();
    }

    this->error(this->position, "unexpected end of file in multiline comment");
}

Token Lexer::lexId() {
    int lookahead = this->read();
    while(this->isIdChar(lookahead))
        lookahead = this->read();
    this->unread(1);

    std::string_view token_content = this->tokenString();
    if(KEYWORD_TYPES.count(token_content) > 0)
        return this->makeToken(KEYWORD_TYPES.at(token_content));
    else
        return this->makeToken(TokenType::ID);
}

Token Lexer::lexPlus() {
    int lookahead = this->read();

    switch(lookahead) {
        case '+':
            return this->makeToken(TokenType::INCREMENT);
        case '=':
            return this->makeToken(TokenType::ADD_ASSIGN);
    }

    this->unread();
    return this->makeToken(TokenType::PLUS);
}

Token Lexer::lexMinus() {
    int lookahead = this->read();

    switch(lookahead) {
        case '>': {
            lookahead = this->read();
            if(lookahead == '*')
                return this->makeToken(TokenType::STARROW);
            this->unread();
            return this->makeToken(TokenType::ARROW);
        }
        case '-':
            return this->makeToken(TokenType::DECREMENT);
        case '=':
            return this->makeToken(TokenType::SUB_ASSIGN);
    }

    this->unread();
    return this->makeToken(TokenType::MINUS);
}

Token Lexer::lexStar() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::MUL_ASSIGN);

    this->unread();
    return this->makeToken(TokenType::STAR);
}

Token Lexer::lexMod() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::MOD_ASSIGN);

    this->unread();
    return this->makeToken(TokenType::MOD);
}

Token Lexer::lexLt() {
    int lookahead = this->read();

    switch(lookahead) {
        case '<': {
            lookahead = this->read();
            if(lookahead == '=')
                return this->makeToken(TokenType::LSHIFT_ASSIGN);
            this->unread();
            return this->makeToken(TokenType::LSHIFT);
        }
        case '=':
            return this->makeToken(TokenType::LESSEQ);
    }

    this->unread();
    return this->makeToken(TokenType::LESS);
}

Token Lexer::lexGt() {
    int lookahead = this->read();

    switch(lookahead) {
        case '>': {
            lookahead = this->read();
            if(lookahead == '=')
                return this->makeToken(TokenType::RSHIFT_ASSIGN);
            this->unread();
            return this->makeToken(TokenType::RSHIFT);
        }
        case '=':
            return this->makeToken(TokenType::GREATEREQ);
    }

    this->unread();
    return this->makeToken(TokenType::GREATER);
}

Token Lexer::lexAnd() {
    int lookahead = this->read();

    switch(lookahead) {
        case '&':
            return this->makeToken(TokenType::AND);
        case '=':
            return this->makeToken(TokenType::BITAND_ASSIGN);
    }

    this->unread();
    return this->makeToken(TokenType::BITAND);
}

Token Lexer::lexOr() {
    int lookahead = this->read();

    switch(lookahead) {
        case '|':
            return this->makeToken(TokenType::OR);
        case '=':
            return this->makeToken(TokenType::BITOR_ASSIGN);
    }

    this->unread();
    return this->makeToken(TokenType::BITOR);
}

Token Lexer::lexXor() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::XOR_ASSIGN);

    this->unread();
    return this->makeToken(TokenType::XOR);
}

Token Lexer::lexAssign() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::EQUAL);

    this->unread();
    return this->makeToken(TokenType::ASSIGN);
}

Token Lexer::lexNot() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::NOTEQUAL);

    this->unread();
    return this->makeToken(TokenType::NOT);
}

Token Lexer::lexDot() {
    int lookahead = this->read();
    if(lookahead == '*')
        return this->makeToken(TokenType::DOTSTAR);

    this->unread();
    return this->makeToken(TokenType::DOT);
}

Token Lexer::lexColon() {
    int lookahead = this->read();
    if(lookahead == ':')
        return this->makeToken(TokenType::SCOPE);

    this->unread();
    return this->makeToken(TokenType::COLON);
}

std::optional<uint32_t> Lexer::lexEscapeLiteral(int base, size_t length, bool allow_shorter, uint32_t max_value) {
    uint32_t value = 0;
    for(size_t i = 0; length == 0 || i < length; ++i) {
        int c = this->read();
        if(c < 0) {
            this->error(this->position, "unexpected end of input");
            return std::nullopt;
        }

        uint32_t digit = (c >= '0' && c <= '9') ? c - '0' :
            (c >= 'a' && c <= 'z') ? c - 'a' + 1 :
            (c >= 'A' && c <= 'Z') ? c - 'A' + 1 :
            base;

        if(digit >= base) {
            this->unread();
            if(allow_shorter && i > 0)
                return value;
            this->error(this->position, "invalid escape sequence value");
            return std::nullopt;
        }

        if ((value != 0 && base > max_value / value) || digit > max_value - value * base) {
            // Overflow
            this->unread();
            this->error(this->position, "escape sequence value out of range");
            return std::nullopt;
        }

        value = value * base + digit;
    }

    return value;
}

std::optional<CodePoint> Lexer::lexEscapeSequence() {
    int c = this->read();
    switch (c) {
        case -1:
            this->error(this->position, "unexpected end of escape sequence");
            return std::nullopt;
        case '\'':
        case '"':
        case '\\':
            return {{static_cast<uint32_t>(c)}};
        case 'a':
            return {{'\a'}};
        case 'b':
            return {{'\b'}};
        case 'f':
            return {{'\f'}};
        case 'n':
            return {{'\n'}};
        case 'r':
            return {{'\r'}};
        case 't':
            return {{'\t'}};
        case 'v':
            return {{'\v'}};
        case 'x': {
            if(auto value = this->lexEscapeLiteral(16, 0, true, 0xFF))
                return {{value.value()}};
            return std::nullopt;
        }
        case 'u': {
            if(auto value = this->lexEscapeLiteral(16, 4, false))
                return {{value.value()}};
            return std::nullopt;
        }
        case 'U': {
            if(auto value = this->lexEscapeLiteral(16, 8, false))
                return {{value.value()}};
            return std::nullopt;
        }
        default:
            if(c >= '0' && c <= '9') {
                if(auto value = this->lexEscapeLiteral(0, 3, true, 255))
                    return {{value.value()}};
                assert(false); // unreachable
            }
            this->unread();
            this->error(this->position, "invalid escape sequence");
            return std::nullopt;
    }
}

Token Lexer::lexStringLiteral() {
    auto ss = std::stringstream();

    while(true) {
        int c = this->read();
        switch (c) {
            case '"':
                return this->makeToken(TokenType::LITERAL_STRING);
            case '\\': {
                if(auto cp = this->lexEscapeSequence()) {
                    ss << cp->toUtf8();
                    break;
                }
                // Attempt to continue parsing even if we encounter an invalid literal.
                break;
            }
            case '\n':
                this->unread();
                return this->error(this->position, "newline in string literal");
            case -1:
                return this->error(this->position, "unexpected end of string literal");
            default:
                ss.put(c);
        }
    }
}

Token Lexer::lex() {
    while(true) {
        this->startToken();
        int lookahead = this->read();
        switch(lookahead) {
            case -1:
                return this->makeToken(TokenType::EOI);
            case ' ':
            case '\t':
            case '\r':
                break;
            case '\n':
                ++this->position.line;
                this->position.column = 0;
                break;
            case '+':
                return this->lexPlus();
            case '-':
                return this->lexMinus();
            case '*':
                return this->lexStar();
            case '%':
                return this->lexMod();
            case '<':
                return this->lexLt();
            case '>':
                return this->lexGt();
            case '&':
                return this->lexAnd();
            case '|':
                return this->lexOr();
            case '^':
                return this->lexXor();
            case '=':
                return this->lexAssign();
            case '!':
                return this->lexNot();
            case '.':
                return this->lexDot();
            case ':':
                return this->lexColon();
            case '~':
                return this->makeToken(TokenType::BITNOT);
            case '{':
                return this->makeToken(TokenType::OPEN_CB);
            case '}':
                return this->makeToken(TokenType::CLOSE_CB);
            case '(':
                return this->makeToken(TokenType::OPEN_PAR);
            case ')':
                return this->makeToken(TokenType::CLOSE_PAR);
            case '[':
                return this->makeToken(TokenType::OPEN_SB);
            case ']':
                return this->makeToken(TokenType::CLOSE_SB);
            case ';':
                return this->makeToken(TokenType::SEMICOLON);
            case ',':
                return this->makeToken(TokenType::COMMA);
            case '?':
                return this->makeToken(TokenType::QUESTION);
            case '/': {
                lookahead = this->read();
                if(lookahead == '=')
                    return this->makeToken(TokenType::DIV_ASSIGN);
                else if(lookahead == '/')
                    this->consumeLine();
                else if(lookahead == '*')
                   this->consumeMultiline();
                else {
                    this->unread();
                    return this->makeToken(TokenType::DIV);
                }
                break;
            }
            case '"':
                return this->lexStringLiteral(); // TODO: L-Strings
            default: {
                if(this->isIdChar(lookahead) && !this->isDigit(lookahead))
                    return this->lexId();
                return this->makeToken(TokenType::INVALID);
            }
        }
    }
}

std::span<const CompileError> Lexer::compileErrors() const {
    return this->errors;
}
