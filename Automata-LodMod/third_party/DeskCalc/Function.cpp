#include "Function.hpp"

#include <ostream>

#include "Parser.hpp"
#include "SymbolGuard.hpp"
#include "SymbolTable.hpp"
#include "types.hpp"

Function::Function(std::string name, SymbolTable& table)
    : funcName{ std::move(name) }, table{ table }
{
}

Complex Function::operator()(const List& args) const
{
    if (args.size() != vars.size())
        throw std::runtime_error{ funcName + " expects " + std::to_string(vars.size()) +
        " arguments (received " + std::to_string(args.size()) + ")" };
    Parser parser{ table };
    SymbolGuard guard{ table };
    for (std::size_t i = 0; i < vars.size(); ++i)
        guard.shadow_var(vars[i], args[i]);
    parser.parse(term);
    return parser.result();
}

std::ostream& operator<<(std::ostream& os, const Function& func)
{
    os << func.funcName << '(';
    std::string sep;  // string instead of char, because it needs to be empty during first iteration
    for (const auto& v : func.vars) {
        os << sep << v;
        sep = ",";
    }
    return os << ") = " << func.term;
}