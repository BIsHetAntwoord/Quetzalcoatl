#ifndef _QUETZALCOATL_LEXER_TOKEN_HPP
#define _QUETZALCOATL_LEXER_TOKEN_HPP

#include <string_view>

enum class TokenType {
    INVALID,
    EOI,

    KEY_ASM,
    KEY_AUTO,
    KEY_BOOL,
    KEY_BREAK,
    KEY_CASE,
    KEY_CLASS,
    KEY_CONST,
    KEY_CONST_CAST,
    KEY_CONTINUE,
    KEY_DEFAULT,
    KEY_DELETE,
    KEY_DO,
    KEY_DOUBLE,
    KEY_DYNAMIC_CAST,
    KEY_ELSE,
    KEY_ENUM,
    KEY_EXPLICIT,
    KEY_EXPORT,
    KEY_EXTERN,
    KEY_FALSE,
    KEY_FLOAT,
    KEY_FOR,
    KEY_FRIEND,
    KEY_GOTO,
    KEY_IF,
    KEY_INLINE,
    KEY_INT,
    KEY_LONG,
    KEY_MUTABLE,
    KEY_NAMESPACE,
    KEY_NEW,
    KEY_OPERATOR,
    KEY_PRIVATE,
    KEY_PROTECTED,
    KEY_PUBLIC,
    KEY_REGISTER,
    KEY_REINTERPRET_CAST,
    KEY_RETURN,
    KEY_SHORT,
    KEY_SIGNED,
    KEY_SIZEOF,
    KEY_STATIC,
    KEY_STATIC_CAST,
    KEY_STRUCT,
    KEY_SWITCH,
    KEY_TEMPLATE,
    KEY_THIS,
    KEY_THROW,
    KEY_TRUE,
    KEY_TRY,
    KEY_TYPEDEF,
    KEY_TYPEID,
    KEY_TYPENAME,
    KEY_UNION,
    KEY_UNSIGNED,
    KEY_USING,
    KEY_VIRTUAL,
    KEY_VOID,
    KEY_VOLATILE,
    KEY_WCHAR_T,
    KEY_WHILE,

    ID,

    PLUS, //TODO
    MINUS, //TODO
    STAR, //TODO
    DIV, //TODO
    MOD, //TODO
    LSHIFT, //TODO
    RSHIFT, //TODO
    BITAND, //TODO
    BITOR, //TODO
    XOR, //TODO
    BITNOT, //TODO

    INCREMENT, //TODO
    DECREMENT, //TODO

    EQUAL, //TODO
    NOTEQUAL, //TODO
    LESS, //TODO
    LESSEQ, //TODO
    GREATER, //TODO
    GREATEREQ, //TODO

    ARROW, //TODO
    DOTSTAR, //TODO
    STARROW, //TODO

    NOT, //TODO
    AND, //TODO
    OR, //TODO

    ASSIGN, //TODO
    ADD_ASSIGN, //TODO
    SUB_ASSIGN, //TODO
    MUL_ASSIGN, //TODO
    DIV_ASSIGN, //TODO
    MOD_ASSIGN, //TODO
    LSHIFT_ASSIGN, //TODO
    RSHIFT_ASSIGN, //TODO
    BITAND_ASSIGN, //TODO
    BITOR_ASSIGN, //TODO
    XOR_ASSIGN, //TODO

    SCOPE, //TODO

    OPEN_PAR,
    CLOSE_PAR,
    OPEN_CB,
    CLOSE_CB,
    OPEN_SB,
    CLOSE_SB,

    QUESTION, //TODO
    COLON, //TODO
    DOT, //TODO
    COMMA, //TODO
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
