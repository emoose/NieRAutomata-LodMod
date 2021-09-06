#include "Parser.hpp"

#include <cmath>
#include <iostream>

#include "math_util.hpp"
#include "SymbolTable.hpp"

Parser::Parser(SymbolTable& table)
    : table{ table }  { }

void Parser::set_symbol_table(SymbolTable& t)
{
    table = t;
}

void Parser::parse(std::istream& is)
{
    ts.set_input(is);
    parse();
}

void Parser::parse(const std::string& str)
{
    ts.set_input(str);
    parse();
}

void Parser::parse()
{
    ts.get();
    while (!consume(Kind::End))
        stmt();
}

void Parser::stmt()
{
    hasResult = true;
    if (peek(Kind::FuncDef))
        func_def();
    else if (peek(Kind::Delete))
        deletion();
    else if (!peek(Kind::Print))
        res = expr();

    if (hasResult && onRes)
        onRes(res);

    if (!peek(Kind::End))
        expect(Kind::Print);
}

void Parser::func_def()
{
    expect(Kind::FuncDef);

    Function func{ ident(), table };
    parse_param_list(func);
    expect(Kind::Assign);
    parse_func_term(func);

    const List test_args(func.numArgs(), 1);
    func(test_args);

    table.set_func(func.name(), func);
    hasResult = false;
}

void Parser::parse_param_list(Function& func)
{
    expect(Kind::LParen);
    do {
        func.add_var(ident());
    } while (consume(Kind::Comma));
    expect(Kind::RParen);
}

void Parser::parse_func_term(Function& func)
{
    std::ostringstream term;
    while (!peek(Kind::Print) && !peek(Kind::End) && !peek(Kind::RBracket)) {
        term << ts.current();
        ts.get();
    }
    func.set_term(term.str());
}

void Parser::deletion()
{
    expect(Kind::Delete);
    if (!peek(Kind::String))
        error("No arguments provided to delete");

    do {
        del_symbol();
    } while (peek(Kind::String));
    hasResult = false;
}

void Parser::del_symbol()
{
    const auto& name = ident();
    if (table.is_const(name))
        error(name, " is a constant");
    if (table.is_reserved_func(name))
        error(name, " is a built-in function");
    table.remove_symbol(name);
}

void Parser::list_def(const std::string& name)
{
    table.set_list(name, list());
}

Complex Parser::expr()
{
    auto left = term();
    for (;;) {
        if (consume(Kind::Plus)) 
            left += term();
        else if (consume(Kind::Minus)) 
            left -= term();
        else
            return left;
    }
}

Complex Parser::term()
{
    auto left = sign();
    for (;;) {
        if (consume(Kind::Mul))
            left *= sign();
        else if (consume(Kind::Div))
            left = safe_div(left, sign());
        else if (consume(Kind::FloorDiv))
            left = safe_floordiv(left, sign());
        else if (consume(Kind::Mod))
            left = safe_mod(left, sign());
        else if (consume(Kind::Parallel))
            left = impedance_parallel(left, sign());
        else 
            return left;
    }
}

Complex Parser::sign()
{
    if (consume(Kind::Minus)) 
        return -postfix();
    consume(Kind::Plus);
    return postfix();
}

Complex Parser::postfix()
{
    auto left = prim();
    for (;;) {
        if (consume(Kind::Pow))
            return pretty_pow(left, sign());
        else if (peek(Kind::String))
            return left * postfix();
        else if (peek(Kind::LParen))
            return left * prim();
        else if (consume(Kind::Fac))
            left = factorial(left);
        else
            return left;
    }
}

Complex Parser::prim()
{
    if (consume(Kind::Number)) 
        return prevTok.num;
    if (peek(Kind::String))
        return resolve_str_tok();
    if (consume(Kind::LParen)) {
        auto val = expr();
        expect(Kind::RParen);
        return val;
    }
    error("Unexpected Token ", ts.current());
    throw std::logic_error{ "Fall through prim()" }; // silence C4715
}

Complex Parser::resolve_str_tok()
{
    const auto name = ident();
    if (peek(Kind::LParen))
        return table.call_func(name, arg_list());
    else if (consume(Kind::Assign)) {
        if (peek(Kind::LBracket)) {
            list_def(name);
            return no_result();
        }
        return var_def(name);
    }
    else if (table.has_list(name)) {
        if (!peek(Kind::Print) && !peek(Kind::End))
            error("Unexpected Token ", ts.current());
        print_list(std::cout, table.list(name));
        std::cout << '\n';
        return no_result();
    }
    return table.value_of(name);
}

Complex Parser::var_def(const std::string& name)
{
    if (table.is_const(name))
        error("Cannot override constant ", name);
    if (peek(Kind::LBracket)) {
        table.set_list(name, list());
        return no_result();
    }
    if (table.isset(name) && !table.has_var(name))
        error(name, " is already defined");
    const auto val = expr();
    table.set_var(name, val);
    return varDefIsRes ? val : no_result();
}

Complex Parser::no_result()
{
    hasResult = false;
    return 0; // dummy value
}

List Parser::list()
{
    expect(Kind::LBracket);
    List l;
    if (consume(Kind::For)) { // [for var=start, end:step loopExpression]
        auto var = ident();
        expect(Kind::Assign);
        auto start = prim().real();

        expect(Kind::Comma);
        auto end = prim().real();
        double step{ 1 };
        if (consume(Kind::Colon))
            step = prim().real();

        Function f{ "__internal__", table };
        f.add_var(var);
        parse_func_term(f);
        if (start < end && step <= 0 || start > end && step >= 0)
            error("Infinite loop");
        for (auto i = start; i <= end; i += step)
            l.emplace_back(f({ Complex(i) }));
    }
    else
        l = list_elem();
    expect(Kind::RBracket);
    return l;
}

List Parser::arg_list()
{
    expect(Kind::LParen);
    List args;

    if (peek(Kind::String) && table.has_list(ts.current().str))
        args = table.list(ident());
    else if (peek(Kind::LBracket))
        args = list();
    else
        args = list_elem();

    expect(Kind::RParen);
    if (args.empty())
        error("Invalid empty argument list");
    return args;
}

List Parser::list_elem()
{
    List list;
    if (!peek(Kind::RParen) && !peek(Kind::RBracket)) {
        do {
            list.emplace_back(expr());
        } while (consume(Kind::Comma));
    }
    return list;
}

const std::string& Parser::ident()
{
    expect(Kind::String);
    return prevTok.str;
}

bool Parser::consume(Kind kind)
{
    if (ts.current().kind == kind) {
        prevTok = ts.current();
        ts.get();
        return true;
    }
    return false;
}

bool Parser::peek(Kind kind) const
{
    return ts.current().kind == kind;
}

void Parser::expect(Kind kind)
{
    if (!consume(kind))
        error("Expected Token ", kind);
}
