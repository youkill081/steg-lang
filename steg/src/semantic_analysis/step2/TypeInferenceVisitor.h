//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include "ast/ASTVisitor.h"
#include "linter/Linter.h"
#include "semantic_analysis/utils/SymbolTable.h"

namespace compiler
{
    class TypeInferenceVisitor final : public ASTVisitor
    {
    private:
        bool _checking_globals = false;
        ResolvedType _current_return_type = ResolvedType::from(ASTTypeNode::VOID);

        static void type_error(const std::string& message, const LexerToken& token) {
            Linter::instance().report(message, token, LintData::Severity::ERR);
        }

        static void type_hint(const std::string& message, const LexerToken& token) {
            Linter::instance().report(message, token, LintData::Severity::HINT);
        }

        static bool is_opaque(const ResolvedType& t) {
            return !t.is_pointer() && (t.base == ASTTypeNode::FILE || t.base == ASTTypeNode::CLOCK);
        }

        static bool is_invalid_pointer(const ResolvedType& t) {
            if (!t.is_pointer()) return false;
            return t.base == ASTTypeNode::VOID ||
                   t.base == ASTTypeNode::FILE ||
                   t.base == ASTTypeNode::CLOCK;
        }

        static ResolvedType check_assignable(const ResolvedType& from, const ResolvedType& to, const LexerToken& token);

    public:
        void visit(ASTMainProgramNode* node) override;
        void visit(ASTFunctionProgramNode* node) override;

        void visit(ASTParameterProgramNode*) override;

        void visit(ASTBlockStatementNode* node) override;
        void visit(ASTVariableStatement* node) override;

        void visit(ASTIfStatementNode* node) override;
        void visit(ASTWhileStatementNode* node) override;
        void visit(ASTForStatementNode* node) override;

        void visit(ASTReturnStatement* node) override;

        void visit(ASTExpressionStatement* node) override;

        void visit(ASTBreakStatement*) override {}
        void visit(ASTContinueStatement*) override {}

        /* Expression */

        void visit(ASTLiteralExpressionNode* node) override;
        void visit(ASTIdentifierExpressionNode* node) override;

        void visit(ASTCallExpressionNode* node) override;
        void visit(ASTBinaryExpressionNode* node) override;
        void visit(ASTUnaryExpressionNode* node) override;

        void visit(ASTAssignExpressionStatement* node) override;

        void visit(ASTAddressOfExpressionNode* node) override;
        void visit(ASTDereferenceExpressionNode* node) override;

        void visit(ASTIndexExpressionNode* node) override;


        void visit(ASTTypeNode*)          override {}
        void visit(ASTErrorNode*)         override {}
        void visit(ASTStatementError*)    override {}
        void visit(ASTImportProgramNode*) override {}
        void visit(ASTFilesProgramNode*)  override {}
        void visit(ASTFileProgramNode*)   override {}
    };
}
