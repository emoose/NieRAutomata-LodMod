#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "SymbolTable.hpp"
#include "types.hpp"

class SymbolGuard {
public:
    SymbolGuard(SymbolTable& table);
    ~SymbolGuard();

    void shadow_var(const std::string& name, Complex value);

private:
    SymbolTable& table;
    std::unordered_map<std::string, Complex> varCache;
    std::vector<std::string> shadowedVars;
};