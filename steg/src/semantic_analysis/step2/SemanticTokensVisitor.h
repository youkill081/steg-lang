//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTVisitor.h"
#include "lexer/TokenMap.h"
#include "semantic_analysis/SymbolTable.h"

namespace compiler
{
    class SemanticTokensVisitor : public ASTVisitor
    {
        TokenMap& _map;
        SymbolTable& _symbols;
    public:
        SemanticTokensVisitor(TokenMap& map, SymbolTable& symbols)
            : _map(map), _symbols(symbols) {}

        void visit(ASTIdentifierExpressionNode* node) override {
            auto symbol = _symbols.get(node->token.value);
            if (!symbol) return;

            auto category = symbol->kind == SymbolKind::FUNCTION
                ? TOKEN_CATH_FUNCTION
                : TOKEN_CATH_VARIABLE;

            _map.set_token(node->token, category);
        }

        void visit(ASTCallExpressionNode* node) override {
            _map.set_token(node->callee->token, TOKEN_CATH_FUNCTION_CALL);
            for (auto& arg : node->args)
                arg->accept(this);
        }

        void visit(ASTParameterProgramNode* node) override {
            _map.set_token(node->token, TOKEN_CATH_PARAMETER);
        }

        void visit(ASTFunctionProgramNode* node) override {
            _map.set_token(node->token, TOKEN_CATH_FUNCTION);
            for (auto& param : node->parameters)
                param->accept(this);
            node->statement->accept(this);
        }

        void visit(ASTMainProgramNode* node) override {
            for (auto& func : node->functions) func->accept(this);
            for (auto& var : node->global_variables) var->accept(this);
        }
        void visit(ASTBlockStatementNode* node) override {
            for (auto& stmt : node->statements) stmt->accept(this);
        }
        void visit(ASTIfStatementNode* node) override {
            node->condition->accept(this);
            node->then_statement->accept(this);
            if (node->false_statement) node->false_statement->accept(this);
        }
        void visit(ASTWhileStatementNode* node) override {
            node->condition->accept(this);
            node->body_statement->accept(this);
        }
        void visit(ASTForStatementNode* node) override {
            if (node->init_expression) node->init_expression->accept(this);
            if (node->condition_expression) node->condition_expression->accept(this);
            if (node->post_expression) node->post_expression->accept(this);
            node->body_statement->accept(this);
        }
        void visit(ASTReturnStatement* node) override {
            if (node->expression) node->expression->accept(this);
        }
        void visit(ASTExpressionStatement* node) override {
            node->expression->accept(this);
        }
        void visit(ASTBinaryExpressionNode* node) override {
            node->left->accept(this);
            node->right->accept(this);
        }
        void visit(ASTUnaryExpressionNode* node) override {
            node->expression->accept(this);
        }
        void visit(ASTAssignExpressionStatement* node) override {
            node->target->accept(this);
            node->value->accept(this);
        }
        void visit(ASTVariableStatement* node) override {
            if (node->expression) node->expression->accept(this);
        }

        // Feuilles sans enfants
        void visit(ASTLiteralExpressionNode*) override {}
        void visit(ASTBreakStatement*) override {}
        void visit(ASTContinueStatement*) override {}
        void visit(ASTTypeNode*) override {}
        void visit(ASTErrorNode*) override {}
        void visit(ASTStatementError*) override {}
        void visit(ASTImportProgramNode*) override {}
        void visit(ASTFilesProgramNode*) override {}
        void visit(ASTFileProgramNode*) override {}
        void visit(ASTAddressOfExpressionNode* node) override {
            node->expression->accept(this);
        }
        void visit(ASTDereferenceExpressionNode* node) override {
            node->expression->accept(this);
        }
        void visit(ASTIndexExpressionNode* node) override {
            node->array->accept(this);
            node->index->accept(this);
        }
    };
}
