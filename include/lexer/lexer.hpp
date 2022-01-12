#ifndef _QUETZALCOATL_LEXER_LEXER_HPP
#define _QUETZALCOATL_LEXER_LEXER_HPP

#include "lexer/token.hpp"
#include "frontend/compile_info.hpp"
#include "frontend/source_location.hpp"
#include "compile_error.hpp"

#include <string_view>
#include <vector>
#include <span>
#include <optional>
#include <cstdint>

class Lexer {
private:
    struct Char {
        uint8_t bytes[4];
        uint8_t len; // 0 is invalid
    };

    std::string_view input;
    size_t input_offset;

    SourceLocation position;
    SourceLocation token_start;
    size_t token_start_offset;

    bool made_token_on_line;

    CompileInfo& compile_info;

    int read();
    void unread(size_t = 1);

    Token makeToken(TokenType);
    Token makeIntToken(TokenType, BaseDataType, uint64_t);

    void startToken();
    std::string_view tokenString();
    void error(SourceLocation loc, std::string_view msg);

    bool isIdChar(int);
    bool isDigit(int, size_t = 10);
    bool isHexDigit(int);
    bool isWhitespace(int);

    void consumeLine();
    void consumeMultiline();

    Token lexNumber();
    Token lexId();
    Token lexPlus();
    Token lexMinus();
    Token lexStar();
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
    std::optional<uint32_t> lexEscapeLiteral(uint32_t base, size_t length, bool allow_shorter, uint32_t max_value = 0xFFFFFFFF);
    Char lexEscapeSequence();
    Token lexStringLiteral();
    Token lexCharLiteral();
    void lexPreprocessor();
public:
    Lexer(std::string_view, CompileInfo&);

    Token lex();
};

#endif
