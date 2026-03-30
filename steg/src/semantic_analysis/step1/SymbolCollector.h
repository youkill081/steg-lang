//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTVisitor.h"
#include "semantic_analysis/utils/SymbolTable.h"
#include "linter/Linter.h"
#include "semantic_analysis/analyzeExports.h"
#include "../utils/helpers.h"
#include "../utils/ModuleManager.h"

namespace compiler
{
    class SymbolCollector : public ASTVisitor
    {
    public:
        SymbolTable table;
        std::vector<std::filesystem::path> imported_paths;

        void visit(ASTMainProgramNode* node) override {
            for (const auto &import : node->imports)
                import->accept(this);
            for (const auto &file : node->files)
                file->accept(this);
            for (const auto &var : node->global_variables)
                var->accept(this);
            for (const auto &func : node->functions)
                func->accept(this);
        }

        void visit(ASTFunctionProgramNode* node) override {
            if (table.contains(node->name))
                Linter::instance().report("Function already declared: " + node->name, node->token);

            std::vector<ResolvedType> param_types;
            for (auto& param : node->parameters)
                param_types.push_back(ResolvedType::from(param->type));

            table.declare(node->name, SymbolInfo{
                .kind = SymbolKind::FUNCTION,
                .type = ResolvedType::from(node->return_type),
                .param_types = param_types,
                .is_exported = node->is_exported,
                .source_file = node->token.path,
                .token = node->token
            });

            table.enter_scope();
            for (auto& param : node->parameters)
                param->accept(this);
            node->statement->accept(this);
            table.exit_scope();
        }

        void visit(ASTParameterProgramNode* node) override {
            table.declare(node->name, SymbolInfo{
                .kind = SymbolKind::VARIABLE,
                .type = ResolvedType::from(node->type),
                .token = node->token
            });
        }

        void visit(ASTVariableStatement* node) override {
            if (table.contains(node->name))
                Linter::instance().report("Variable already declared: " + node->name, node->token);

            if (node->expression)
                node->expression->accept(this);

            table.declare(node->name, SymbolInfo{
                .kind  = SymbolKind::VARIABLE,
                .type  = ResolvedType::from(node->type),
                .token = node->token
            });
        }

        void visit(ASTBlockStatementNode* node) override {
            table.enter_scope();
            for (auto &stmt : node->statements)
                stmt->accept(this);
            table.exit_scope();
        }

        void visit(ASTIfStatementNode* node) override {
            node->condition->accept(this);
            node->then_statement->accept(this);
            if (node->false_statement)
                node->false_statement->accept(this);
        }

        void visit(ASTWhileStatementNode* node) override {
            node->condition->accept(this);
            node->body_statement->accept(this);
        }

        void visit(ASTForStatementNode* node) override {
            table.enter_scope();
            if (node->init_expression)
                node->init_expression->accept(this);
            if (node->condition_expression)
                node->condition_expression->accept(this);
            if (node->post_expression)
                node->post_expression->accept(this);
            node->body_statement->accept(this);
            table.exit_scope();
        }

        void visit(ASTExpressionStatement* node) override {
            node->expression->accept(this);
        }

        void visit(ASTReturnStatement* node) override {
            if (node->expression) node->expression->accept(this);
        }

        void visit(ASTFileProgramNode* node) override {
            if (table.contains(node->name))
                Linter::instance().report("File identifier already declared: " + node->name, node->token);

            const std::filesystem::path source_dir = node->token.path.parent_path();
            const std::filesystem::path exec_dir   = std::filesystem::current_path();

            const std::filesystem::path relative_to_source = source_dir / node->path;

            if (const std::filesystem::path relative_to_exec   = exec_dir   / node->path; !std::filesystem::exists(relative_to_source) &&
                !std::filesystem::exists(relative_to_exec))
            {
                Linter::instance().report(
                    "File not found: '" + node->path + "'\n"
                    "  Tried: " + relative_to_source.string() + "\n"
                    "  Tried: " + relative_to_exec.string(),
                    node->token,
                    LintData::Severity::ERR
                );
            }

            if (table.get(node->name) != nullptr)
            {
                Linter::instance().report(
                    "File identifier already declared: " + node->name,
                    node->token,
                    LintData::Severity::ERR
                );
            }

            table.declare(node->name, SymbolInfo{
                .kind = SymbolKind::VARIABLE,
                .type = ResolvedType::from(ASTTypeNode::Types::FILE, 0),
                .is_exported = true,
                .token = node->token
            });
        }

        void visit(ASTFilesProgramNode*) override {}

        void visit(ASTImportProgramNode* node) override {
            auto path = utils::resolve_path(node->token.path, node->path);
            if (!path) {
                Linter::instance().report("Module not found: " + node->path, node->token);
                return;
            }

            imported_paths.push_back(std::filesystem::absolute(*path));

            std::string absolute_path = std::filesystem::absolute(*path).string();
            auto &manager = ModuleManager::instance();

            if (manager.currently_parsing.contains(absolute_path))
                return;

            if (!manager.module_cache.contains(absolute_path)) {
                manager.currently_parsing.insert(absolute_path);

                auto exports = analyzeExports(absolute_path);

                if (exports) {
                    manager.module_cache[absolute_path] = std::move(*exports);
                } else {
                    Linter::instance().report("Failed to parse imported file: " + node->path, node->token);
                }

                manager.currently_parsing.erase(absolute_path);
            }

            auto &cache = manager.module_cache[absolute_path];
            for (const auto& name : node->functions_variables) {
                if (cache.contains(name.value)) {
                    if (table.contains(name.value)) {
                        Linter::instance().report("Import conflict: '" + name.value + "' is already defined.", name);
                    } else {
                        auto symbol = std::make_shared<SymbolInfo>(*cache[name.value]);
                        symbol->source_file = absolute_path;
                        table.declare(name.value, *symbol);
                    }
                } else {
                    Linter::instance().report("Symbol '" + name.value + "' is not exported by " + node->path, name);
                }
            }
        }

        void visit(ASTBreakStatement*) override {}
        void visit(ASTContinueStatement*) override {}

        void visit(ASTBinaryExpressionNode* node) override {
            node->left->accept(this);
            node->right->accept(this);
        }

        void visit(ASTUnaryExpressionNode* node) override
        {
            node->expression->accept(this);
        }

        void visit(ASTLiteralExpressionNode*) override {}

        void visit(ASTIdentifierExpressionNode* node) override {
            const auto symbol = table.get(node->name);

            if (symbol) {
                node->resolved_symbol = symbol;
            } else {
                Linter::instance().report("Undeclared identifier: " + node->name, node->token);
                Linter::instance().report("Undeclared identifier: " + node->name, node->token);
            }
        }

        void visit(ASTCallExpressionNode* node) override {
            node->callee->accept(this);
            node->resolved_symbol = node->callee->resolved_symbol;

            for (auto& arg : node->args) {
                arg->accept(this);
            }
        }

        void visit(ASTIndexExpressionNode* node) override {
            node->array->accept(this);
            node->index->accept(this);
        }

        void visit(ASTAssignExpressionStatement* node) override {
            node->target->accept(this);
            node->value->accept(this);
        }

        void visit(ASTAddressOfExpressionNode* node) override {
            node->expression->accept(this);
        }
        void visit(ASTDereferenceExpressionNode* node) override {
            node->expression->accept(this);
        }
        void visit(ASTTypeNode*) override {}
        void visit(ASTErrorNode*) override {}
        void visit(ASTStatementError*) override {}
    };
}
