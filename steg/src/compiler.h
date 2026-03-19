//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTProgramNode.h"
#include "lexer/TokenMap.h"
#include "parser/parser_program.h"
#include "semantic_analysis/utils/SymbolTable.h"
#include "semantic_analysis/step1/SymbolCollector.h"
#include "semantic_analysis/step2/TypeInferenceVisitor.h"
#include "semantic_analysis/step3/ControlFlowVisitor.h"
#include "semantic_analysis/step4/SemanticTokensVisitor.h"


namespace compiler
{
    struct AnalysisResult {
        std::unique_ptr<ASTMainProgramNode> ast;
        SymbolTable symbols;
        TokenMap tokens;
    };

    inline std::optional<AnalysisResult> analyze(TextParser& parser)
    {
        ModuleManager::instance().clear();

        Lexer lexer(parser);
        lexer.compute();
        auto tokens = lexer.tokens();

        auto result = parseMainProgram(tokens);
        if (!result.has_value())
            return std::nullopt;

        // get all symbols
        SymbolCollector collector;
        result->value->accept(&collector);

        TokenMap token_map = lexer.get_token_map();
        SymbolTable symbols = collector.table;

        // Check all types inferences
        TypeInferenceVisitor types_inference;
        result->value->accept(&types_inference);

        ControlFlowVisitor flow;
        result->value->accept(&flow);

        // Tag all identifiers (function, function call and variable definition)
        SemanticTokensVisitor tokens_visitor(token_map, symbols);
        result->value->accept(&tokens_visitor);

        return AnalysisResult {
            .ast = std::move(result->value),
            .symbols = std::move(collector.table),
            .tokens = std::move(token_map)
        };
    }
}
