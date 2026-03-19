//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTVisitor.h"
#include "linter/Linter.h"

namespace compiler
{
    class ControlFlowVisitor final : public ASTVisitor
    {
    private:
        int  _loop_depth = 0;
        bool _current_is_void = true;

        static void flow_error(const std::string& message, const LexerToken& token) {
            Linter::instance().report(message, token, LintData::Severity::ERR);
        }

        static bool always_returns(ASTNode* node);

    public:
        void visit(ASTMainProgramNode* node) override;
        void visit(ASTFunctionProgramNode* node) override;

        void visit(ASTBlockStatementNode* node) override;
        void visit(ASTIfStatementNode* node) override;
        void visit(ASTWhileStatementNode* node) override;
        void visit(ASTForStatementNode* node) override;
        void visit(ASTReturnStatement* node) override;
        void visit(ASTExpressionStatement* node) override;
        void visit(ASTVariableStatement* node) override;

        void visit(ASTBreakStatement* node) override;
        void visit(ASTContinueStatement* node) override;

        void visit(ASTParameterProgramNode*) override {}
        void visit(ASTLiteralExpressionNode*) override {}
        void visit(ASTIdentifierExpressionNode*) override {}
        void visit(ASTCallExpressionNode*) override {}
        void visit(ASTBinaryExpressionNode*) override {}
        void visit(ASTUnaryExpressionNode*) override {}
        void visit(ASTAssignExpressionStatement* node) override;
        void visit(ASTAddressOfExpressionNode*) override {}
        void visit(ASTDereferenceExpressionNode*) override {}
        void visit(ASTIndexExpressionNode*) override {}
        void visit(ASTTypeNode*) override {}
        void visit(ASTErrorNode*) override {}
        void visit(ASTStatementError*) override {}
        void visit(ASTImportProgramNode*) override {}
        void visit(ASTFilesProgramNode*) override {}
        void visit(ASTFileProgramNode*) override {}
    };
}