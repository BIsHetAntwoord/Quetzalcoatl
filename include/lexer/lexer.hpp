#ifndef _QUETZALCOATL_LEXER_LEXER_HPP
#define _QUETZALCOATL_LEXER_LEXER_HPP

#include <string_view>

#include "lexer/token.hpp"
#include "lexer/filetable.hpp"

class Lexer {
private:
    std::string_view input;
    size_t input_offset;

    TokenPosition position;
    TokenPosition token_start;
    size_t token_start_offset;

    FileTable& files;

    int read();
    void unread(size_t = 1);
    Token makeToken(TokenType);
    void startToken();
    std::string_view tokenString();

    bool isIdChar(int);
    bool isDigit(int);

    Token lexId();
    Token lexPlus();
    Token lexMinus();
    Token lexStar();
    Token lexDiv();
    Token lexMod();
    Token lexLt();
    Token lexGt();
    Token lexAnd();
    Token lexOr();
    Token lexXor();
    Token lexAssign();
    Token lexNot();
    Token lexDot();
    Token lexColon();
public:
    Lexer(const std::string_view&, FileTable&);

    Token lex();
};

#endif
