//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTProgramNode.h"
#include "IR/IRGenerator.h"
#include "IR/IRPrinter.h"
#include "IR/IRLowering.h"
#include "IR/ir_structure.h"
#include "lexer/TokenMap.h"
#include "parser/parser_program.h"
#include "semantic_analysis/utils/SymbolTable.h"
#include "semantic_analysis/step1/SymbolCollector.h"
#include "semantic_analysis/step2/TypeInferenceVisitor.h"
#include "semantic_analysis/step3/ControlFlowVisitor.h"
#include "semantic_analysis/step4/SemanticTokensVisitor.h"

#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <unordered_set>

#include "asm_gen/AsmGenerator.h"
#include "asm_gen/Registerallocator.h"


namespace compiler
{
    struct AnalysisResult {
        std::unique_ptr<ASTMainProgramNode> ast;
        SymbolTable symbols;
        TokenMap tokens;
    };

    /*
     * Analyse flow, only parse given file and return needed data for hightlight
     */
    inline std::optional<AnalysisResult> analyze(TextParser& parser)
    {
        ModuleManager::instance().clear();

        Lexer lexer(parser);
        lexer.compute();
        auto tokens = lexer.tokens();

        auto result = parseMainProgram(tokens);
        if (!result.has_value())
            return std::nullopt;

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


    /* Full Compiler */

    /* One compiled file */
    struct CompilationUnit {
        std::filesystem::path path;
        std::vector<std::shared_ptr<IrBasicBlock>> ir_blocks;
        std::vector<IrGlobal> globals;
        std::vector<std::filesystem::path> imported_paths;
    };

    /* Full program compiled */
    struct CompilationResult {
        std::vector<std::shared_ptr<IrBasicBlock>> ir_blocks;
        std::vector<IrGlobal> globals;
        RegisterAllocation registers;
        std::string asm_output;
    };

    /* Compile single file IR */
    inline std::optional<CompilationUnit> compile_single(const std::filesystem::path& path)
    {
        TextParser parser = TextParser::from_file(path.string());

        Lexer lexer(parser);
        lexer.compute();
        const auto token_span = lexer.tokens();

        auto result = parseMainProgram(token_span);
        if (!result.has_value())
            return std::nullopt;

        SymbolCollector collector;
        result->value->accept(&collector);

        TypeInferenceVisitor typer;
        result->value->accept(&typer);

        ControlFlowVisitor flow;
        result->value->accept(&flow);

        /* Hight Level IR  */
        IRGenerator ir_gen;
        result->value->accept(&ir_gen);

        /* Lowering it */
        IRLowering lowering(ir_gen.all_blocks, ir_gen.globals);
        lowering.lower();

        return CompilationUnit{
            .path = path,
            .ir_blocks = std::move(lowering.lowered_blocks),
            .globals = std::move(lowering.lowered_globals),
            .imported_paths = std::move(collector.imported_paths)
        };
    }

    /* Compile a program */
    inline std::optional<CompilationResult> compile(const std::filesystem::path& path)
    {
        Linter::instance().set_compile_mode(true);
        ModuleManager::instance().clear();

        CompilationResult out;

        std::set<std::filesystem::path> visited; // Follow the already compiled file
        std::set<std::string> seen_globals; // Follow already defined global variables

        std::function<bool(const std::filesystem::path&)> compile_recursive =
            [&](const std::filesystem::path& file) -> bool
            {
                const auto absolute = std::filesystem::absolute(file);
                if (visited.contains(absolute))
                    return true;
                visited.insert(absolute);

                auto unit = compile_single(absolute);
                if (!unit)
                    return false;

                for (const auto& dep : unit->imported_paths)
                    if (!compile_recursive(dep))
                        return false;

                for (auto& block : unit->ir_blocks)
                    out.ir_blocks.push_back(std::move(block));

                for (auto& global : unit->globals)
                {
                    if (!seen_globals.contains(global.name))
                    {
                        seen_globals.insert(global.name);
                        out.globals.push_back(std::move(global));
                    }
                }

                return true;
            };

        if (!compile_recursive(path))
            return std::nullopt;

        if (Linter::instance().has_errors())
            return std::nullopt;

        /* Register Allocation */
        std::unordered_set<std::string> global_name_set;
        for (const auto &g : out.globals)
            global_name_set.insert(g.name);

        RegisterAllocator allocator(out.ir_blocks, global_name_set);
        out.registers = allocator.allocate();

        for (auto& spill : out.registers.spill_globals)
        {
            if (!seen_globals.contains(spill.name))
            {
                seen_globals.insert(spill.name);
                out.globals.push_back(spill);
            }
        }

        /* ASM Generation */
        AsmGenerator asm_gen(out.ir_blocks, out.globals, out.registers);
        out.asm_output = asm_gen.generate();

        return out;
    }
}
