#pragma once

#include <istream>

#include "ErrorReporter.hpp"
#include "Token.hpp"

class TokenStream {
public:
    TokenStream() = default;
    explicit TokenStream(std::istream& input);
    explicit TokenStream(std::istream* input);
    explicit TokenStream(const std::string& input);

    ~TokenStream();

    TokenStream(const TokenStream&) = delete;
    TokenStream& operator=(const TokenStream&) = delete;

    void set_input(std::istream& input);
    void set_input(std::istream* input);
    void set_input(const std::string& input);

    Token get();
    const Token& current() const { return ct; }

private:
    Token parse_identifier(char firstChar);
    Token parse_double_op(char expected, Kind onSuccess, Kind onFailure);
    Token identifier_to_token(const std::string& str) const;
    void cleanup();

    Token ct{ Kind::End };
    std::istream* input{};
    bool ownsInput{};
    ErrorReporter error;
};
