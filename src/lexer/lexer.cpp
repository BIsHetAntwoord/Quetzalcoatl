#include "lexer/lexer.hpp"

#include <unordered_map>

const std::unordered_map<std::string_view, TokenType> KEYWORD_TYPES = {
    {"int", TokenType::KEY_INT},
    {"return", TokenType::KEY_RETURN}
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

    //TODO: figure out how to use string_view on unordered_map
    std::string_view token_content = this->tokenString();
    if(KEYWORD_TYPES.count(token_content) > 0)
        return this->makeToken(KEYWORD_TYPES.at(token_content));
    else
        return this->makeToken(TokenType::ID);
}

Token Lexer::lex() {
    this->startToken();
    int lookahead = this->read();
    while(lookahead != -1) {
        switch(lookahead) {
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

        this->startToken();
        lookahead = this->read();
    }
    return this->makeToken(TokenType::EOI);
}