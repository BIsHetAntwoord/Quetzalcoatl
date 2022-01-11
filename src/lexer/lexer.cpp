#include "lexer/lexer.hpp"
#include "unicode.hpp"

#include <sstream>
#include <string>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <limits>

const std::unordered_map<std::string_view, TokenType> KEYWORD_TYPES = {
    {"and", TokenType::AND},
    {"and_eq", TokenType::BITAND_ASSIGN},
    {"asm", TokenType::KEY_ASM},
    {"auto", TokenType::KEY_AUTO},
    {"bitand", TokenType::BITAND},
    {"bitor", TokenType::BITOR},
    {"bool", TokenType::KEY_BOOL},
    {"break", TokenType::KEY_BREAK},
    {"case", TokenType::KEY_CASE},
    {"class", TokenType::KEY_CLASS},
    {"compl", TokenType::BITNOT},
    {"const", TokenType::KEY_CONST},
    {"const_cast", TokenType::KEY_CONST_CAST},
    {"continue", TokenType::KEY_CONTINUE},
    {"default", TokenType::KEY_DEFAULT},
    {"delete", TokenType::KEY_DELETE},
    {"do", TokenType::KEY_DO},
    {"double", TokenType::KEY_DOUBLE},
    {"dynamic_cast", TokenType::KEY_DYNAMIC_CAST},
    {"else", TokenType::KEY_ELSE},
    {"enum", TokenType::KEY_ENUM},
    {"explicit", TokenType::KEY_EXPLICIT},
    {"export", TokenType::KEY_EXPORT},
    {"extern", TokenType::KEY_EXTERN},
    {"false", TokenType::KEY_FALSE},
    {"float", TokenType::KEY_FLOAT},
    {"for", TokenType::KEY_FOR},
    {"friend", TokenType::KEY_FRIEND},
    {"goto", TokenType::KEY_GOTO},
    {"if", TokenType::KEY_IF},
    {"inline", TokenType::KEY_INLINE},
    {"int", TokenType::KEY_INT},
    {"long", TokenType::KEY_LONG},
    {"mutable", TokenType::KEY_MUTABLE},
    {"namespace", TokenType::KEY_NAMESPACE},
    {"new", TokenType::KEY_NEW},
    {"not", TokenType::NOT},
    {"not_eq", TokenType::NOTEQUAL},
    {"operator", TokenType::KEY_OPERATOR},
    {"or", TokenType::OR},
    {"or_eq", TokenType::BITOR_ASSIGN},
    {"private", TokenType::KEY_PRIVATE},
    {"protected", TokenType::KEY_PROTECTED},
    {"public", TokenType::KEY_PUBLIC},
    {"register", TokenType::KEY_REGISTER},
    {"reinterpret_cast", TokenType::KEY_REINTERPRET_CAST},
    {"return", TokenType::KEY_RETURN},
    {"short", TokenType::KEY_SHORT},
    {"signed", TokenType::KEY_SIGNED},
    {"sizeof", TokenType::KEY_SIZEOF},
    {"static", TokenType::KEY_STATIC},
    {"static_cast", TokenType::KEY_STATIC_CAST},
    {"struct", TokenType::KEY_STRUCT},
    {"switch", TokenType::KEY_SWITCH},
    {"template", TokenType::KEY_TEMPLATE},
    {"this", TokenType::KEY_THIS},
    {"throw", TokenType::KEY_THROW},
    {"true", TokenType::KEY_TRUE},
    {"try", TokenType::KEY_TRY},
    {"typedef", TokenType::KEY_TYPEDEF},
    {"typeid", TokenType::KEY_TYPEID},
    {"typename", TokenType::KEY_TYPENAME},
    {"union", TokenType::KEY_UNION},
    {"unsigned", TokenType::KEY_UNSIGNED},
    {"using", TokenType::KEY_USING},
    {"virtual", TokenType::KEY_VIRTUAL},
    {"void", TokenType::KEY_VOID},
    {"volatile", TokenType::KEY_VOLATILE},
    {"wchar_t", TokenType::KEY_WCHAR_T},
    {"while", TokenType::KEY_WHILE},
    {"xor", TokenType::XOR},
    {"xor_eq", TokenType::XOR_ASSIGN},
};

Lexer::Lexer(std::string_view input, FileTable& files, DataTypeTable& type_table, StringTable& strings) :
    input(input), input_offset(0), position({1, 0, 0}), token_start_offset(0),
    files(files), type_table(type_table), strings(strings) {
    this->files.addFile("<unknown>");
}

int Lexer::read() {
    if(this->input_offset >= this->input.size())
        return -1;
    ++this->position.column;
    return (unsigned)this->input[this->input_offset++];
}

void Lexer::unread(size_t num) {
    this->position.column -= num;
    --this->input_offset;
}

Token Lexer::makeToken(TokenType type) {
    Token result;
    result.type = type;
    result.pos = this->token_start;
    result.raw = this->tokenString();
    return result;
}

Token Lexer::makeIntToken(TokenType type, BaseDataType base_type, uint64_t value) {
    Token result = this->makeToken(type);
    result.integer.type = this->type_table.getBaseTypeId(base_type);
    result.integer.value = value;
    return result;
}

void Lexer::startToken() {
    this->token_start = this->position;
    this->token_start_offset = this->input_offset;
}

std::string_view Lexer::tokenString() {
    return this->input.substr(this->token_start_offset, this->input_offset - this->token_start_offset);
}

void Lexer::error(SourceLocation loc, std::string_view msg) {
    this->errors.emplace_back(loc, msg);
}

bool Lexer::isIdChar(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
        (c >= '0' && c <= '9') || (c == '_');
}

bool Lexer::isDigit(int c, size_t base) {
    if(c >= '0' && c <= '9')
        return size_t(c - '0') < base;
    if(c >= 'a' && c <= 'z')
        return size_t(c - 'a' + 10) < base;
    if(c >= 'A' && c <= 'Z')
        return size_t(c - 'A' + 10) < base;
    return false;
}

bool Lexer::isHexDigit(int c) {
    return this->isDigit(c, 16);
}

void Lexer::consumeLine() {
    int lookahead = this->read();
    while(lookahead != '\n' && lookahead != -1)
        lookahead = this->read();
    this->unread();
}

void Lexer::consumeMultiline() {
    int lookahead = this->read();
    while(lookahead != -1) {
        if(lookahead == '*') {
            lookahead = this->read();
            if(lookahead == '/')
                return;
        }

        if(lookahead != -1)
            lookahead = this->read();
    }

    this->error(this->position, "unexpected end of file in multiline comment");
}

Token Lexer::lexNumber() {
    uint64_t base = 10;

    int lookahead = this->read();
    if(lookahead == '0') {
        lookahead = this->read();

        if(lookahead == 'x' || lookahead == 'X')
            base = 16;
        else if(lookahead == 'b' || lookahead == 'B')
            base = 2;
        else
            base = 8;

        if(base != 8) {
            lookahead = this->read();
            if(!this->isDigit(lookahead, base)) {
                this->error(this->position, "Invalid sequence after integer base");
                this->unread();
                return this->makeIntToken(TokenType::LITERAL_INTEGER, BaseDataType::INT, 0);
            }
        }
    }

    uint64_t value = 0;
    while(this->isDigit(lookahead, base)) {
        value *= base;

        if(lookahead >= '0' && lookahead <= '9')
            value += lookahead - '0';
        if(lookahead >= 'a' && lookahead <= 'z')
            value += lookahead - 'a' + 10;
        if(lookahead >= 'A' && lookahead <= 'Z')
            value += lookahead - 'A' + 10;

        lookahead = this->read();
    }

    BaseDataType data_type;

    if(lookahead == 'l' || lookahead == 'L') {
        lookahead = this->read();
        if(lookahead == 'u' || lookahead == 'U')
            data_type = BaseDataType::UNSIGNED_LONG;
        else {
            if(value <= std::numeric_limits<int64_t>::max())
                data_type = BaseDataType::LONG;
            else
                data_type = BaseDataType::UNSIGNED_LONG;
            this->unread();
        }
    }
    else if(lookahead == 'u' || lookahead == 'U') {
        lookahead = this->read();
        if(lookahead == 'l' || lookahead == 'L')
            data_type = BaseDataType::UNSIGNED_LONG;
        else {
            if(value <= std::numeric_limits<uint32_t>::max())
                data_type = BaseDataType::UNSIGNED_INT;
            else
                data_type = BaseDataType::UNSIGNED_LONG;
            this->unread();
        }
    }
    else {
        this->unread();

        if(value <= std::numeric_limits<int32_t>::max())
            data_type = BaseDataType::INT;
        else if(value <= std::numeric_limits<uint32_t>::max() && base != 10)
            data_type = BaseDataType::UNSIGNED_INT;
        else if(value <= std::numeric_limits<int64_t>::max() || base == 10)
            data_type = BaseDataType::LONG;
        else
            data_type = BaseDataType::UNSIGNED_LONG;
    }

    return this->makeIntToken(TokenType::LITERAL_INTEGER, data_type, value);
}

Token Lexer::lexId() {
    int lookahead = this->read();
    while(this->isIdChar(lookahead))
        lookahead = this->read();
    this->unread(1);

    std::string_view token_content = this->tokenString();
    if(KEYWORD_TYPES.count(token_content) > 0)
        return this->makeToken(KEYWORD_TYPES.at(token_content));
    else
        return this->makeToken(TokenType::ID);
}

Token Lexer::lexPlus() {
    int lookahead = this->read();

    switch(lookahead) {
        case '+':
            return this->makeToken(TokenType::INCREMENT);
        case '=':
            return this->makeToken(TokenType::ADD_ASSIGN);
    }

    this->unread();
    return this->makeToken(TokenType::PLUS);
}

Token Lexer::lexMinus() {
    int lookahead = this->read();

    switch(lookahead) {
        case '>': {
            lookahead = this->read();
            if(lookahead == '*')
                return this->makeToken(TokenType::STARROW);
            this->unread();
            return this->makeToken(TokenType::ARROW);
        }
        case '-':
            return this->makeToken(TokenType::DECREMENT);
        case '=':
            return this->makeToken(TokenType::SUB_ASSIGN);
    }

    this->unread();
    return this->makeToken(TokenType::MINUS);
}

Token Lexer::lexStar() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::MUL_ASSIGN);

    this->unread();
    return this->makeToken(TokenType::STAR);
}

Token Lexer::lexMod() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::MOD_ASSIGN);

    this->unread();
    return this->makeToken(TokenType::MOD);
}

Token Lexer::lexLt() {
    int lookahead = this->read();

    switch(lookahead) {
        case '<': {
            lookahead = this->read();
            if(lookahead == '=')
                return this->makeToken(TokenType::LSHIFT_ASSIGN);
            this->unread();
            return this->makeToken(TokenType::LSHIFT);
        }
        case '=':
            return this->makeToken(TokenType::LESSEQ);
    }

    this->unread();
    return this->makeToken(TokenType::LESS);
}

Token Lexer::lexGt() {
    int lookahead = this->read();

    switch(lookahead) {
        case '>': {
            lookahead = this->read();
            if(lookahead == '=')
                return this->makeToken(TokenType::RSHIFT_ASSIGN);
            this->unread();
            return this->makeToken(TokenType::RSHIFT);
        }
        case '=':
            return this->makeToken(TokenType::GREATEREQ);
    }

    this->unread();
    return this->makeToken(TokenType::GREATER);
}

Token Lexer::lexAnd() {
    int lookahead = this->read();

    switch(lookahead) {
        case '&':
            return this->makeToken(TokenType::AND);
        case '=':
            return this->makeToken(TokenType::BITAND_ASSIGN);
    }

    this->unread();
    return this->makeToken(TokenType::BITAND);
}

Token Lexer::lexOr() {
    int lookahead = this->read();

    switch(lookahead) {
        case '|':
            return this->makeToken(TokenType::OR);
        case '=':
            return this->makeToken(TokenType::BITOR_ASSIGN);
    }

    this->unread();
    return this->makeToken(TokenType::BITOR);
}

Token Lexer::lexXor() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::XOR_ASSIGN);

    this->unread();
    return this->makeToken(TokenType::XOR);
}

Token Lexer::lexAssign() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::EQUAL);

    this->unread();
    return this->makeToken(TokenType::ASSIGN);
}

Token Lexer::lexNot() {
    int lookahead = this->read();

    if(lookahead == '=')
        return this->makeToken(TokenType::NOTEQUAL);

    this->unread();
    return this->makeToken(TokenType::NOT);
}

Token Lexer::lexDot() {
    int lookahead = this->read();
    if(lookahead == '*')
        return this->makeToken(TokenType::DOTSTAR);

    this->unread();
    return this->makeToken(TokenType::DOT);
}

Token Lexer::lexColon() {
    int lookahead = this->read();
    if(lookahead == ':')
        return this->makeToken(TokenType::SCOPE);

    this->unread();
    return this->makeToken(TokenType::COLON);
}

std::optional<uint32_t> Lexer::lexEscapeLiteral(uint32_t base, size_t length, bool allow_shorter, uint32_t max_value) {
    uint32_t value = 0;
    for(size_t i = 0; length == 0 || i < length; ++i) {
        int c = this->read();
        if(c < 0) {
            this->error(this->position, "unexpected end of input");
            return std::nullopt;
        }

        uint32_t digit = (c >= '0' && c <= '9') ? c - '0' :
            (c >= 'a' && c <= 'z') ? c - 'a' + 10 :
            (c >= 'A' && c <= 'Z') ? c - 'A' + 10 :
            base;

        if(digit >= base) {
            this->unread();
            if(allow_shorter && i > 0)
                return value;
            this->error(this->position, "invalid escape sequence value");
            return std::nullopt;
        }

        if ((value != 0 && base > max_value / value) || digit > max_value - value * base) {
            // Overflow
            std::cout << (char) c << " " << value << " " << (max_value / value)  << std::endl;
            this->unread();
            this->error(this->position, "escape sequence value out of range");
            return std::nullopt;
        }

        value = value * base + digit;
    }

    return value;
}

Lexer::Char Lexer::lexEscapeSequence() {
    constexpr Char invalid = {{}, 0};

    auto loc = this->position;
    auto utf8 = [loc, this](uint32_t codepoint) {
        auto cp = CodePoint{codepoint};
        if (!cp.isValidUtf8())
            this->error(loc, "invalid universal character constant");
        Char result;
        uint8_t i = 0;
        for (auto c : cp.toUtf8()) {
            result.bytes[i++] = c;
        }
        result.len = i;
        return result;
    };

    auto chr = [](uint8_t value) {
        Char result;
        result.bytes[0] = value;
        result.len = 1;
        return result;
    };

    int c = this->read();
    switch (c) {
        case -1:
            this->error(this->position, "unexpected end of escape sequence");
            return invalid;
        case '\'':
        case '"':
        case '\\':
            return chr(c);
        case 'a':
            return chr('\a');
        case 'b':
            return chr('\b');
        case 'f':
            return chr('\f');
        case 'n':
            return chr('\n');
        case 'r':
            return chr('\r');
        case 't':
            return chr('\t');
        case 'v':
            return chr('\v');
        case 'x': {
            if(auto value = this->lexEscapeLiteral(16, 0, true, 0xFF))
                return utf8(value.value());
            return invalid;
        }
        case 'u': {
            if(auto value = this->lexEscapeLiteral(16, 4, false))
                return utf8(value.value());
            return invalid;
        }
        case 'U': {
            if(auto value = this->lexEscapeLiteral(16, 8, false))
                return utf8(value.value());
            return invalid;
        }
        default:
            if(c >= '0' && c <= '9') {
                if(auto value = this->lexEscapeLiteral(8, 3, true, 255))
                    return chr(value.value());
                return invalid;
            }
            this->unread();
            this->error(this->position, "invalid escape sequence");
            return invalid;
    }
}

Token Lexer::lexStringLiteral() {
    auto str = this->strings.add({});

    auto makeStringToken = [this, str]{
        auto tok = this->makeToken(TokenType::LITERAL_STRING);
        tok.string_literal = str;
        return tok;
    };

    while(true) {
        int c = this->read();
        switch (c) {
            case '"':
                return makeStringToken();
            case '\\': {
                auto chr = this->lexEscapeSequence();
                if(chr.len == 0)
                    break; // Continue even if the escape sequence was invalid.
                for(uint8_t i = 0; i < chr.len; ++i)
                    this->strings.pushToMostRecent(chr.bytes[i]);
                break;
            }
            case '\n':
                this->unread();
                this->error(this->position, "newline in string literal");
                return makeStringToken();
            case -1:
                this->error(this->position, "unexpected end of string literal");
                return makeStringToken();
            default:
                this->strings.pushToMostRecent(c);
                break;
        }
    }
}

Token Lexer::lexCharLiteral() {
    auto loc = this->position;
    int c = this->read();
    uint8_t char_literal = 'b';
    auto makeCharToken = [this, char_literal]{
        auto tok = this->makeToken(TokenType::LITERAL_CHAR);
        tok.char_literal = char_literal;
        return tok;
    };
    switch (c) {
        case '\'':
            this->error(loc, "empty character literal");
            break;
        case '\\': {
            auto chr = this->lexEscapeSequence();
            if(chr.len > 1) {
                this->error(loc, "character literal too large");
            }else if(chr.len != 0)
                char_literal = chr.bytes[0];
            break;
        }
        case '\n':
            this->unread();
            this->error(loc, "newline in character literal");
            return makeCharToken();
        case -1:
            this->unread();
            this->error(loc, "unexpected end of character literal");
            return makeCharToken();
        default:
            char_literal = c;
            break;
    }

    loc = this->position;
    c = this->read();
    if (c != '\'') {
        // Attempt to still find a closing quote or newline
        bool end = false;
        while (!end) {
            switch (c) {
                case '\'':
                    this->error(loc, "multi-character character literal");
                    end = true;
                    break;
                case -1:
                case '\n':
                    this->unread();
                    this->error(loc, "unterminated character literal");
                    end = true;
                    break;
                default:
                    c = this->read();
                    continue;
            }
        }
    }

    return makeCharToken();
}

Token Lexer::lex() {
    while(true) {
        this->startToken();
        int lookahead = this->read();
        switch(lookahead) {
            case -1:
                return this->makeToken(TokenType::EOI);
            case ' ':
            case '\t':
            case '\r':
                break;
            case '\n':
                ++this->position.line;
                this->position.column = 0;
                break;
            case '+':
                return this->lexPlus();
            case '-':
                return this->lexMinus();
            case '*':
                return this->lexStar();
            case '%':
                return this->lexMod();
            case '<':
                return this->lexLt();
            case '>':
                return this->lexGt();
            case '&':
                return this->lexAnd();
            case '|':
                return this->lexOr();
            case '^':
                return this->lexXor();
            case '=':
                return this->lexAssign();
            case '!':
                return this->lexNot();
            case '.':
                return this->lexDot();
            case ':':
                return this->lexColon();
            case '~':
                return this->makeToken(TokenType::BITNOT);
            case '{':
                return this->makeToken(TokenType::OPEN_CB);
            case '}':
                return this->makeToken(TokenType::CLOSE_CB);
            case '(':
                return this->makeToken(TokenType::OPEN_PAR);
            case ')':
                return this->makeToken(TokenType::CLOSE_PAR);
            case '[':
                return this->makeToken(TokenType::OPEN_SB);
            case ']':
                return this->makeToken(TokenType::CLOSE_SB);
            case ';':
                return this->makeToken(TokenType::SEMICOLON);
            case ',':
                return this->makeToken(TokenType::COMMA);
            case '?':
                return this->makeToken(TokenType::QUESTION);
            case '/': {
                lookahead = this->read();
                if(lookahead == '=')
                    return this->makeToken(TokenType::DIV_ASSIGN);
                else if(lookahead == '/')
                    this->consumeLine();
                else if(lookahead == '*')
                   this->consumeMultiline();
                else {
                    this->unread();
                    return this->makeToken(TokenType::DIV);
                }
                break;
            }
            case '"':
                return this->lexStringLiteral();
            case '\'':
                return this->lexCharLiteral();
            case 'L': {
                switch (this->read()) {
                    case '"':
                        return this->lexStringLiteral();
                    case '\'':
                        return this->lexCharLiteral();
                    default:
                        this->unread();
                        return this->lexId();
                }
            }
            default: {
                if(this->isDigit(lookahead)) {
                    this->unread();
                    return this->lexNumber();
                }
                if(this->isIdChar(lookahead) && !this->isDigit(lookahead))
                    return this->lexId();
                return this->makeToken(TokenType::INVALID);
            }
        }
    }
}

std::span<const CompileError> Lexer::compileErrors() const {
    return this->errors;
}
