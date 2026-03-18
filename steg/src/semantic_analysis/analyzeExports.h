//
// Created by Roumite on 18/03/2026.
//

#pragma once
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "SymbolTable.h"
#include "exports/ExportCollector.h"
#include "parser/parser_program.h"

namespace compiler {
    inline std::optional<std::unordered_map<std::string, std::shared_ptr<SymbolInfo>>> analyzeExports(const std::string path) {
        TextParser parser = TextParser::from_file(path);
        Lexer lexer(parser);
        lexer.compute();
        auto tokens = lexer.tokens();

        auto result = parseMainProgram(tokens);
        if (!result.has_value()) return std::nullopt;

        ExportCollector export_collector;
        result->value->accept(&export_collector);

        return export_collector.exported_symbols;
    }
}
