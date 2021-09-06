#include "TokenStream.hpp"

#include <cassert>
#include <cctype>
#include <iostream>
#include <map>
#include <sstream>

TokenStream::TokenStream(std::istream& is)
    : input{ &is } { }

TokenStream::TokenStream(std::istream* is)
    : input{ is }, ownsInput{ true } { }

TokenStream::TokenStream(const std::string& str)
    : input{ new std::istringstream{str} }, ownsInput{ true } { }

TokenStream::~TokenStream()
{
    cleanup();
}

void TokenStream::set_input(std::istream& is)
{
    cleanup();
    input = &is;
}

void TokenStream::set_input(std::istream* is)
{
    cleanup();
    input = is;
    ownsInput = true;
}

void TokenStream::set_input(const std::string& str)
{
    set_input(new std::istringstream{ str });
}

static constexpr unsigned char uchar(char ch)
{   // for isalpha etc. which expect ch >= -1 and ch <= 255
    return static_cast<unsigned char>(ch);
}

Token TokenStream::get()
{
    char ch;
    do {
        if (!input || !input->get(ch)) 
            return ct = { Kind::End };
    } while (std::isspace(uchar(ch)) && ch != '\n');

    switch (ch) {
    case ';':
    case '\n':
        return ct = { Kind::Print };
    case '+':
    case '-':
    case '%':
    case '!':
    case '^':
    case ',':
    case '=':
    case ':':
    case '(': case ')':
    case '[': case ']':
    case '{': case '}':
        return ct = { static_cast<Kind>(ch) };
    case '*':
        return ct = parse_double_op('*', Kind::Pow, Kind::Mul);
    case '/':
        return ct = parse_double_op('/', Kind::FloorDiv, Kind::Div);
    case '|':
        return ct = parse_double_op('|', Kind::Parallel, Kind::Invalid);
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
    case '.':
        input->unget();
        *input >> ct.num;
        ct.kind = Kind::Number;
        return ct;
    default:
        return ct = parse_identifier(ch);
    }
}

Token TokenStream::parse_double_op(char next, Kind onMatch, Kind onFailure)
{
    char c;
    if (input->get(c) && c == next) 
        return { onMatch };
    else {
        input->unget();
        if (onFailure != Kind::Invalid) 
            return { onFailure };
        error("Expected Token: ", next);
    }
    return { Kind::Invalid };
}

Token TokenStream::parse_identifier(char ch)
{
    if (std::isalpha(uchar(ch)) || ch == '_') {
        std::string s{ ch };
        while (input->get(ch) && (std::isalnum(uchar(ch)) || ch == '_'))
            s += ch;
        input->unget();
        return identifier_to_token(s);
    }
    error("Bad Token ", ch);
    return { Kind::Invalid };
}

static const std::map<std::string, Kind> strTokens{
    { "div", Kind::FloorDiv },
    { "mod", Kind::Mod },
    { "del", Kind::Delete },
    { "fn", Kind::FuncDef },
    { "for", Kind::For }
};

Token TokenStream::identifier_to_token(const std::string& str) const
{
    const auto found = strTokens.find(str);
    if (found != cend(strTokens))
        return { found->second };
    return { Kind::String, str };
}

void TokenStream::cleanup()
{
    if (ownsInput) {
        delete input;
        ownsInput = false;
    }
}