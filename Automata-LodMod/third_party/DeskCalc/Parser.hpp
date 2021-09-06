#pragma once

#include <istream>
#include <map>
#include <string>

#include "ErrorReporter.hpp"
#include "Function.hpp"
#include "TokenStream.hpp"
#include "types.hpp"

class SymbolTable;

class Parser {
public:
    Parser(SymbolTable& table);

    void parse(std::istream& is);
    void parse(const std::string& input);

    const Complex& result() const { return res; }
    bool has_result() const { return hasResult; }

    SymbolTable& symbol_table() { return table; }
    void set_symbol_table(SymbolTable& t);

    void set_vardef_is_res(bool isRes) { varDefIsRes = isRes; }
    void on_result(std::function<void(Complex)> handler) { onRes = std::move(handler); }

private:
    void parse();
    void stmt();
    void func_def();
    void parse_param_list(Function& func);
    void parse_func_term(Function& func);
    void deletion();
    void del_symbol();
    void list_def(const std::string& name);

    Complex expr();
    Complex term();
    Complex sign();
    Complex postfix();
    Complex prim();
    Complex resolve_str_tok();
    Complex var_def(const std::string& name);
    Complex no_result();

    List list();
    List arg_list();
    List list_elem();

    const std::string& ident();

    bool peek(Kind kind) const;
    bool consume(Kind kind);
    void expect(Kind kind);

    Token prevTok;
    SymbolTable& table;
    TokenStream ts;
    ErrorReporter error;

    Complex res;
    bool hasResult{};
    bool varDefIsRes{ true };
    std::function<void(Complex)> onRes;
};



