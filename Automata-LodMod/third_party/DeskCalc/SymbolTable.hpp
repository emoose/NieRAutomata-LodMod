#pragma once

#include <map>
#include <string>

#include "Function.hpp"
#include "types.hpp"

enum class VarAccess {
    Mutable,
    Const
};

struct Var {
    Var() = default;
    Var(Complex v, VarAccess acc)
        : value{std::move(v)}, access{acc} { }

    Complex value;
    VarAccess access{};
};

class SymbolTable {
public:
    using ConstStrRef = const std::string&;
    using FuncMap = std::map<std::string, Func>;

    SymbolTable();

    bool is_reserved_func(const std::string& name) const;

    void set_const(ConstStrRef name, Complex value);
    void set_var(ConstStrRef name, Complex value);
    void set_list(ConstStrRef name, List&& list);
    void set_func(ConstStrRef name, Function func);

    Complex value_of(ConstStrRef var) const;
    const List& list(ConstStrRef var) const;
    Complex call_func(ConstStrRef func, const List& args) const;

    bool is_const(ConstStrRef name) const;
    bool has_var(ConstStrRef name) const;
    bool has_list(ConstStrRef name) const;
    bool has_func(ConstStrRef name) const;
    bool isset(ConstStrRef name) const;

    void remove_var(ConstStrRef name);
    void remove_list(ConstStrRef name);
    void remove_func(ConstStrRef name);
    void remove_symbol(ConstStrRef name);

    void clear();
    void clear_vars();
    void clear_funcs();
    void clear_lists();

    const std::map<std::string, Var>& vars() const { return varTable; }
    const std::map<std::string, List>& lists() const { return listTable; }
    const std::map<std::string, Function>& funcs() const { return funcTable; }

private:
    void add_constants();

    static const FuncMap defaultFuncTable;

    std::map<std::string, Var> varTable;
    std::map<std::string, List> listTable;
    std::map<std::string, Function> funcTable;
};

Var make_const_var(Complex value);
Var make_var(Complex value);
