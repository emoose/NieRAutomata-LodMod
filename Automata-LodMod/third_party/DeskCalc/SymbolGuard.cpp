#include "SymbolGuard.hpp"

SymbolGuard::SymbolGuard(SymbolTable& table)
    : table{ table }
{
}

SymbolGuard::~SymbolGuard()
{
    for (const auto& var : shadowedVars)
        table.remove_var(var);

    for (const auto& pair : varCache)
        table.set_var(pair.first, pair.second);
}

void SymbolGuard::shadow_var(const std::string& name, Complex tempVal)
{
    if (table.has_var(name)) {
        const auto val = table.value_of(name);
        varCache[name] = val;
        table.remove_var(name);
    }
    table.set_var(name, tempVal);
    shadowedVars.push_back(name);
}