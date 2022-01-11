#ifndef _QUETZALCOATL_LEXER_TOKEN_HPP
#define _QUETZALCOATL_LEXER_TOKEN_HPP

#include <string_view>

enum class TokenType {
    INVALID,
    EOI,

    KEY_AND,
    KEY_AND_EQ,
    KEY_ASM,
    KEY_AUTO,
    KEY_BITAND,
    KEY_BITOR,
    KEY_BOOL,
    KEY_BREAK,
    KEY_CASE,
    KEY_CLASS,
    KEY_COMPL,
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
    KEY_NOT,
    KEY_NOT_EQ,
    KEY_OPERATOR,
    KEY_OR,
    KEY_OR_EQ,
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
    KEY_XOR,
    KEY_XOR_EQ,

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
