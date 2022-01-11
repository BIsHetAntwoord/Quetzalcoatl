#ifndef _QUETZALCOATL_LEXER_LEXER_HPP
#define _QUETZALCOATL_LEXER_LEXER_HPP

#include "lexer/token.hpp"
#include "lexer/filetable.hpp"
#include "source_location.hpp"
#include "compile_error.hpp"
#include "unicode.hpp"

#include <string_view>
#include <vector>
#include <span>
#include <optional>
#include <cstdint>

class Lexer {
private:
    std::string_view input;
    size_t input_offset;

    SourceLocation position;
    SourceLocation token_start;
    size_t token_start_offset;

    FileTable& files;

    std::vector<CompileError> errors;

    int read();
    void unread(size_t = 1);
    Token makeToken(TokenType);
    void startToken();
    std::string_view tokenString();
    void error(SourceLocation loc, std::string_view msg);

    bool isIdChar(int);
    bool isDigit(int);
    bool isHexDigit(int);

    void consumeLine();
    void consumeMultiline();

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
    std::optional<uint32_t> lexEscapeLiteral(int base, size_t length, bool allow_shorter, uint32_t max_value = 0xFFFFFFFF);
    std::optional<CodePoint> lexEscapeSequence();
    Token lexStringLiteral();
public:
    Lexer(std::string_view, FileTable&);

    Token lex();

    std::span<const CompileError> compileErrors() const;
};

#endif
