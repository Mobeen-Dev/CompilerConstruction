#ifndef CITU_TOKEN_HPP
#define CITU_TOKEN_HPP

#include <string>
#include <variant>
#include <cstdint>
#include "location.hpp"

enum class TokenType {
    // Keywords
    TOKEN_JABTAK,
    TOKEN_SHRO,
    TOKEN_KHATAM,
    TOKEN_AGAR,
    TOKEN_MAGAR,
    TOKEN_NIKAL,
    TOKEN_MEDALO,
    TOKEN_WAPIS,
    TOKEN_DIKHAO,
    TOKEN_PARHO,

    // Data Types
    TOKEN_INT16,
    TOKEN_INT32,
    TOKEN_UINT16,
    TOKEN_UINT32,
    TOKEN_REAL,
    TOKEN_FLOAT,
    TOKEN_CHAR,
    TOKEN_VOID,

    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_EQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_LTE,
    TOKEN_GT,
    TOKEN_GTE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    // Delimiters
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,

    // Identifiers & Literals
    TOKEN_IDENTIFIER,
    TOKEN_INT_LITERAL,
    TOKEN_FLOAT_LITERAL,
    TOKEN_CHAR_LITERAL,
    TOKEN_STRING_LITERAL,

    // Special
    TOKEN_EOF,
    TOKEN_ERROR
};

inline const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::TOKEN_JABTAK:         return "TOKEN_JABTAK";
        case TokenType::TOKEN_SHRO:           return "TOKEN_SHRO";
        case TokenType::TOKEN_KHATAM:         return "TOKEN_KHATAM";
        case TokenType::TOKEN_AGAR:           return "TOKEN_AGAR";
        case TokenType::TOKEN_MAGAR:          return "TOKEN_MAGAR";
        case TokenType::TOKEN_NIKAL:          return "TOKEN_NIKAL";
        case TokenType::TOKEN_MEDALO:         return "TOKEN_MEDALO";
        case TokenType::TOKEN_WAPIS:          return "TOKEN_WAPIS";
        case TokenType::TOKEN_DIKHAO:         return "TOKEN_DIKHAO";
        case TokenType::TOKEN_PARHO:          return "TOKEN_PARHO";
        case TokenType::TOKEN_INT16:          return "TOKEN_INT16";
        case TokenType::TOKEN_INT32:          return "TOKEN_INT32";
        case TokenType::TOKEN_UINT16:         return "TOKEN_UINT16";
        case TokenType::TOKEN_UINT32:         return "TOKEN_UINT32";
        case TokenType::TOKEN_REAL:           return "TOKEN_REAL";
        case TokenType::TOKEN_FLOAT:          return "TOKEN_FLOAT";
        case TokenType::TOKEN_CHAR:           return "TOKEN_CHAR";
        case TokenType::TOKEN_VOID:           return "TOKEN_VOID";
        case TokenType::TOKEN_PLUS:           return "TOKEN_PLUS";
        case TokenType::TOKEN_MINUS:          return "TOKEN_MINUS";
        case TokenType::TOKEN_STAR:           return "TOKEN_STAR";
        case TokenType::TOKEN_SLASH:          return "TOKEN_SLASH";
        case TokenType::TOKEN_PERCENT:        return "TOKEN_PERCENT";
        case TokenType::TOKEN_EQ:             return "TOKEN_EQ";
        case TokenType::TOKEN_NEQ:            return "TOKEN_NEQ";
        case TokenType::TOKEN_LT:             return "TOKEN_LT";
        case TokenType::TOKEN_LTE:            return "TOKEN_LTE";
        case TokenType::TOKEN_GT:             return "TOKEN_GT";
        case TokenType::TOKEN_GTE:            return "TOKEN_GTE";
        case TokenType::TOKEN_AND:            return "TOKEN_AND";
        case TokenType::TOKEN_OR:             return "TOKEN_OR";
        case TokenType::TOKEN_NOT:            return "TOKEN_NOT";
        case TokenType::TOKEN_LPAREN:         return "TOKEN_LPAREN";
        case TokenType::TOKEN_RPAREN:         return "TOKEN_RPAREN";
        case TokenType::TOKEN_LBRACKET:       return "TOKEN_LBRACKET";
        case TokenType::TOKEN_RBRACKET:       return "TOKEN_RBRACKET";
        case TokenType::TOKEN_SEMICOLON:      return "TOKEN_SEMICOLON";
        case TokenType::TOKEN_COMMA:          return "TOKEN_COMMA";
        case TokenType::TOKEN_IDENTIFIER:     return "TOKEN_IDENTIFIER";
        case TokenType::TOKEN_INT_LITERAL:    return "TOKEN_INT_LITERAL";
        case TokenType::TOKEN_FLOAT_LITERAL:  return "TOKEN_FLOAT_LITERAL";
        case TokenType::TOKEN_CHAR_LITERAL:   return "TOKEN_CHAR_LITERAL";
        case TokenType::TOKEN_STRING_LITERAL: return "TOKEN_STRING_LITERAL";
        case TokenType::TOKEN_EOF:            return "TOKEN_EOF";
        case TokenType::TOKEN_ERROR:          return "TOKEN_ERROR";
        default:                              return "TOKEN_UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string lexeme;
    SourceLocation location;
    std::variant<std::monostate, int64_t, double, char, std::string> value;

    Token(TokenType t, std::string lex, SourceLocation loc)
        : type(t), lexeme(std::move(lex)), location(loc), value(std::monostate{}) {}
};

#endif // CITU_TOKEN_HPP
