//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <unordered_map>
#include "ast/ASTVisitor.h"
#include "semantic_analysis/SymbolTable.h"

namespace compiler {
    class ExportCollector final : public ASTVisitor {
    public:
        std::unordered_map<std::string, std::shared_ptr<SymbolInfo>> exported_symbols;

        void visit(ASTMainProgramNode* node) override {
            for (auto &func : node->functions)
                func->accept(this);
            for (auto &file : node->files)
                file->accept(this);
        }

        void visit(ASTFunctionProgramNode* node) override {
            if (node->is_exported) {
                std::vector<ASTTypeNode::Types> params;
                for (auto& p : node->parameters) params.push_back(p->type->type);

                exported_symbols[node->name] = std::make_shared<SymbolInfo>(SymbolInfo{
                    .kind = SymbolKind::FUNCTION,
                    .type = node->return_type->type,
                    .param_types = params,
                    .is_exported = true,
                    .token = node->token
                });
            }
        }

        void visit(ASTFileProgramNode* node) override {
            exported_symbols[node->name] = std::make_shared<SymbolInfo>(SymbolInfo{ // All files are exported
                .kind = SymbolKind::VARIABLE,
                .type = ASTTypeNode::Types::FILE,
                .is_exported = true,
                .token = node->token
            });
        }
        
        void visit(ASTBlockStatementNode*) override {}
        void visit(ASTVariableStatement*) override {}
        void visit(ASTFilesProgramNode* node) override {}
        void visit(ASTParameterProgramNode* node) override {}
        void visit(ASTImportProgramNode* node) override {}
        void visit(ASTIfStatementNode* node) override {}
        void visit(ASTWhileStatementNode* node) override {}
        void visit(ASTForStatementNode* node) override {}
        void visit(ASTReturnStatement* node) override {}
        void visit(ASTBreakStatement* node) override {}
        void visit(ASTContinueStatement* node) override {}
        void visit(ASTExpressionStatement* node) override {}
        void visit(ASTBinaryExpressionNode* node) override {}
        void visit(ASTUnaryExpressionNode* node) override {}
        void visit(ASTLiteralExpressionNode* node) override {}
        void visit(ASTIdentifierExpressionNode* node) override {}
        void visit(ASTCallExpressionNode* node) override {}
        void visit(ASTIndexExpressionNode* node) override {}
        void visit(ASTAssignExpressionStatement* node) override {}
        void visit(ASTAddressOfExpressionNode* node) override {}
        void visit(ASTDereferenceExpressionNode* node) override {}
        void visit(ASTTypeNode* node) override {}
        void visit(ASTErrorNode* node) override {}
        void visit(ASTStatementError* node) override {}
    };
}
