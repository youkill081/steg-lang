//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <set>
#include <string>
#include <unordered_map>

#include "SymbolTable.h"

class ModuleManager {
public:
    std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<compiler::SymbolInfo>>> module_cache;

    std::set<std::string> currently_parsing;

    static ModuleManager& instance() {
        static ModuleManager inst;
        return inst;
    }

    void clear()
    {
        module_cache.clear();
        currently_parsing.clear();
    }
};
