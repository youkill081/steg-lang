//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include <vector>

#include "ASTNode.h"
#include "ASTExpressionNode.h"

namespace compiler
{
    class ASTStatementNode : public ASTNode
        {};

    class ASTStatementError : public ASTStatementNode
    {
    public:
        ASTStatementError() = default;
        void display(std::size_t left_padding) override
        {
            display_indent(left_padding);
            std::cout << "Error" << std::endl;
        };
        void accept(ASTVisitor* visitor) override;
    };

    class ASTBlockStatementNode final : public ASTStatementNode
    {
    public:
        ASTBlockStatementNode(std::vector<std::unique_ptr<ASTStatementNode>> statement) :
            statements(std::move(statement)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::vector<std::unique_ptr<ASTStatementNode>> statements;
    };

    class ASTIfStatementNode final : public ASTStatementNode
    {
    public:
        ASTIfStatementNode(
            std::unique_ptr<ASTExpressionNode> condition,
            std::unique_ptr<ASTBlockStatementNode> then_statement,
            std::unique_ptr<ASTBlockStatementNode> false_statement
        ) : condition(std::move(condition)), then_statement(std::move(then_statement)), false_statement( std::move(false_statement)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> condition;
        std::unique_ptr<ASTBlockStatementNode> then_statement;
        std::unique_ptr<ASTBlockStatementNode> false_statement; // For the else, this is optionnal
    };

    class ASTWhileStatementNode final : public ASTStatementNode
    {
    public:
        ASTWhileStatementNode(std::unique_ptr<ASTExpressionNode> condition, std::unique_ptr<ASTStatementNode> body_statement)
            : condition(std::move(condition)), body_statement( std::move(body_statement)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> condition;
        std::unique_ptr<ASTStatementNode> body_statement;
    };

    class ASTForStatementNode final : public ASTStatementNode
    {
    public:
        ASTForStatementNode(
            std::unique_ptr<ASTExpressionNode> init_expression,
            std::unique_ptr<ASTExpressionNode> condition_expression,
            std::unique_ptr<ASTExpressionNode> post_expression,
            std::unique_ptr<ASTStatementNode> body_statement
        ) : init_expression(std::move(init_expression)),
            condition_expression(std::move(condition_expression)),
            post_expression(std::move(post_expression)),
            body_statement( std::move(body_statement)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> init_expression;
        std::unique_ptr<ASTExpressionNode> condition_expression;
        std::unique_ptr<ASTExpressionNode> post_expression;

        std::unique_ptr<ASTStatementNode> body_statement;
    };

    class ASTReturnStatement final : public ASTStatementNode
    {
    public:
        ASTReturnStatement(std::unique_ptr<ASTExpressionNode> expression)
            : expression(std::move(expression)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> expression; // The return expression is optional
    };

    class ASTBreakStatement final : public ASTStatementNode
    {
    public:
        ASTBreakStatement() = default;
        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;
    };

    class ASTContinueStatement final : public ASTStatementNode
    {
    public:
        ASTContinueStatement() = default;
        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;
    };

    class ASTVariableStatement final : public ASTStatementNode
    {
    public:
        ASTVariableStatement(
            const std::string &name,
            std::unique_ptr<ASTTypeNode> type,
            std::unique_ptr<ASTExpressionNode> expression
        ) : name(name), type(std::move(type)), expression(std::move(expression)) {}

        std::string name;
        std::unique_ptr<ASTTypeNode> type;
        std::unique_ptr<ASTExpressionNode> expression; // Optional expression to init the variable

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;
    };

    class ASTExpressionStatement final : public ASTStatementNode
    {
    public:
        ASTExpressionStatement(std::unique_ptr<ASTExpressionNode> expression)
            : expression(std::move(expression)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;
        std::unique_ptr<ASTExpressionNode> expression;
    };
}
