#ifndef _QUETZALCOATL_LEXER_TOKEN_HPP
#define _QUETZALCOATL_LEXER_TOKEN_HPP

#include <string_view>

enum class TokenType {
    INVALID
};

struct Token {
    TokenType type;
    union {

    };
    std::string_view raw;
};

#endif
