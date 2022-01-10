#ifndef _QUETZALCOATL_LEXER_LEXER_HPP
#define _QUETZALCOATL_LEXER_LEXER_HPP

#include <string_view>

#include "lexer/token.hpp"

class Lexer {
private:
    std::string_view input;
    size_t position;
public:
    Lexer(const std::string_view&);
};

#endif
