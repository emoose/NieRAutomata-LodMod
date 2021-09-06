#pragma once

#include <string>

enum class Kind : char {
    End,
    Invalid,
    Number,
    String,
    Parallel,
    Print,
    FloorDiv,
    Delete,
    FuncDef,
    For,

    Plus = '+', 
    Minus = '-',
    Mul = '*', 
    Div = '/',
    Mod = '%',
    Pow = '^',
    Fac = '!',
    Assign = '=',
    Comma = ',',
    Colon = ':',

    LParen = '(',
    RParen = ')',
    LBrace = '{', 
    RBrace = '}',
    LBracket = '[',
    RBracket = ']',
};

struct Token {
    Kind kind{};
    std::string str;
    double num{};
};

inline std::ostream& operator<<(std::ostream& os, Kind kind)
{
    switch (kind) {
    case Kind::End: return os << "END";
    case Kind::Invalid: return os << "INVALID";
    case Kind::Number: return os << "NUMBER";
    case Kind::String: return os << "STRING";
    case Kind::Parallel: return os << "PARALLEL";
    case Kind::Print: return os << "PRINT";
    case Kind::FloorDiv: return os << "div";
    default: return os << static_cast<char>(kind);
    }
}

inline std::ostream& operator<<(std::ostream& os, const Token& token)
{
    switch (token.kind) {
    case Kind::Number: return os << token.num;
    case Kind::String: return os << token.str;
    default: return os << token.kind;
    }
}