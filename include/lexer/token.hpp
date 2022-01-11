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

    PLUS,
    MINUS,
    STAR,
    DIV,
    MOD,
    LSHIFT,
    RSHIFT,
    BITAND,
    BITOR,
    XOR,
    BITNOT,

    INCREMENT,
    DECREMENT,

    EQUAL,
    NOTEQUAL,
    LESS,
    LESSEQ,
    GREATER,
    GREATEREQ,

    ARROW,
    DOTSTAR,
    STARROW,

    NOT,
    AND,
    OR,

    ASSIGN,
    ADD_ASSIGN,
    SUB_ASSIGN,
    MUL_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    LSHIFT_ASSIGN,
    RSHIFT_ASSIGN,
    BITAND_ASSIGN,
    BITOR_ASSIGN,
    XOR_ASSIGN,

    SCOPE,

    OPEN_PAR,
    CLOSE_PAR,
    OPEN_CB,
    CLOSE_CB,
    OPEN_SB,
    CLOSE_SB,

    QUESTION,
    COLON,
    DOT,
    COMMA,
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
