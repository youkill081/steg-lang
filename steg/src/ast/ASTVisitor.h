//
// Created by Roumite on 17/03/2026.
//

#pragma once

#include "ASTExpressionNode.h"
#include "ASTStatementNode.h"
#include "ASTProgramNode.h"

namespace compiler
{
    class ASTVisitor
    {
    public:
        virtual ~ASTVisitor() = default;

        // Program
        virtual void visit(ASTMainProgramNode* node) = 0;
        virtual void visit(ASTFilesProgramNode* node) = 0;
        virtual void visit(ASTFileProgramNode* node) = 0;
        virtual void visit(ASTFunctionProgramNode* node) = 0;
        virtual void visit(ASTParameterProgramNode* node) = 0;
        virtual void visit(ASTImportProgramNode* node) = 0;

        // Statements
        virtual void visit(ASTBlockStatementNode* node) = 0;
        virtual void visit(ASTIfStatementNode* node) = 0;
        virtual void visit(ASTWhileStatementNode* node) = 0;
        virtual void visit(ASTForStatementNode* node) = 0;
        virtual void visit(ASTReturnStatement* node) = 0;
        virtual void visit(ASTBreakStatement* node) = 0;
        virtual void visit(ASTContinueStatement* node) = 0;
        virtual void visit(ASTVariableStatement* node) = 0;
        virtual void visit(ASTExpressionStatement* node) = 0;

        // Expressions
        virtual void visit(ASTBinaryExpressionNode* node) = 0;
        virtual void visit(ASTUnaryExpressionNode* node) = 0;
        virtual void visit(ASTLiteralExpressionNode* node) = 0;
        virtual void visit(ASTIdentifierExpressionNode* node) = 0;
        virtual void visit(ASTCallExpressionNode* node) = 0;
        virtual void visit(ASTIndexExpressionNode* node) = 0;
        virtual void visit(ASTAssignExpressionStatement* node) = 0;
        virtual void visit(ASTAddressOfExpressionNode* node) = 0;
        virtual void visit(ASTDereferenceExpressionNode* node) = 0;

        // Other
        virtual void visit(ASTTypeNode* node) = 0;
        virtual void visit(ASTErrorNode* node) = 0;
        virtual void visit(ASTStatementError* node) = 0;
    };
}
