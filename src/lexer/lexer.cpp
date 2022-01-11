#include "lexer/lexer.hpp"

#include <unordered_map>

const std::unordered_map<std::string_view, TokenType> KEYWORD_TYPES = {
    {"and", TokenType::KEY_AND},
    {"and_eq", TokenType::KEY_AND_EQ},
    {"asm", TokenType::KEY_ASM},
    {"auto", TokenType::KEY_AUTO},
    {"bitand", TokenType::KEY_BITAND},
    {"bitor", TokenType::KEY_BITOR},
    {"bool", TokenType::KEY_BOOL},
    {"break", TokenType::KEY_BREAK},
    {"case", TokenType::KEY_CASE},
    {"class", TokenType::KEY_CLASS},
    {"compl", TokenType::KEY_COMPL},
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
    {"not", TokenType::KEY_NOT},
    {"not_eq", TokenType::KEY_NOT_EQ},
    {"operator", TokenType::KEY_OPERATOR},
    {"or", TokenType::KEY_OR},
    {"or_eq", TokenType::KEY_OR_EQ},
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
    {"xor", TokenType::KEY_XOR},
    {"xor_eq", TokenType::KEY_XOR_EQ},
};

Lexer::Lexer(const std::string_view& input, FileTable& files) : input(input), input_offset(0),
        position({1, 0, 0}), token_start_offset(0), files(files) {
    this->files.addFile("<unknown>");
}

int Lexer::read() {
    if(this->input_offset >= this->input.size())
        return -1;
    ++this->position.offset;
    return (unsigned)this->input[this->input_offset++];
}

void Lexer::unread(size_t num) {
    this->position.offset -= num;
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

bool Lexer::isIdChar(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || (c == '_');
}

bool Lexer::isDigit(int c) {
    return (c >= '0' && c <= '9');
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
                this->position.offset = 0;
                break;
            case '{':
                return this->makeToken(TokenType::OPEN_CB);
            case '}':
                return this->makeToken(TokenType::CLOSE_CB);
            case '(':
                return this->makeToken(TokenType::OPEN_PAR);
            case ')':
                return this->makeToken(TokenType::CLOSE_PAR);
            case ';':
                return this->makeToken(TokenType::SEMICOLON);
            default: {
                if(this->isIdChar(lookahead) && !this->isDigit(lookahead))
                    return this->lexId();
                return this->makeToken(TokenType::INVALID);
            }
        }
    }
}
