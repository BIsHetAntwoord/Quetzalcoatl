#ifndef _QUETZALCOATL_LEXER_TOKEN_HPP
#define _QUETZALCOATL_LEXER_TOKEN_HPP

#include <string_view>

enum class TokenType {
    INVALID,
    EOI,

    KEY_INT,
    KEY_RETURN,

    ID,

    OPEN_PAR,
    CLOSE_PAR,
    OPEN_CB,
    CLOSE_CB,

    SEMICOLON
};

struct TokenPosition {
    size_t line;
    size_t offset;
    size_t file_id;
};

struct Token {
    TokenType type;
    union {

    };
    std::string_view raw;
    TokenPosition pos;
};

#endif
