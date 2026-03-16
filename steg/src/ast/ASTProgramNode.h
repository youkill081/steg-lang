//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include <vector>
#include <memory>
#include <string>

#include "ASTExpressionNode.h"
#include "ASTNode.h"
#include "ASTStatementNode.h"

namespace compilator
{
    class ASTProgramNode : public ASTNode
        {};

    class ASTParameterProgramNode final : public ASTProgramNode
    {
    public:
        ASTParameterProgramNode(const std::string &name, std::unique_ptr<ASTTypeNode> type)
            : name(name), type(std::move(type)) {}

        void display(std::size_t left_padding) override;

        std::string name;
        std::unique_ptr<ASTTypeNode> type;
    };

    class ASTFunctionProgramNode final : public ASTProgramNode
    {
    public:
        ASTFunctionProgramNode(
            const std::string &name,
            std::vector<std::unique_ptr<ASTParameterProgramNode>> parameters,
            std::unique_ptr<ASTTypeNode> return_type,
            std::unique_ptr<ASTBlockStatementNode> statement,
            bool is_exported
        ) : name(name),
            parameters(std::move(parameters)),
            return_type(std::move(return_type)),
            statement(std::move(statement)),
            is_exported(is_exported) {}

        void display(std::size_t left_padding) override;

        std::string name;
        std::vector<std::unique_ptr<ASTParameterProgramNode>> parameters;
        std::unique_ptr<ASTTypeNode> return_type;
        std::unique_ptr<ASTBlockStatementNode> statement;
        bool is_exported;
    };

    class ASTVariableProgramNode final : public ASTProgramNode
    {
    public:
        ASTVariableProgramNode(
            const std::string &name,
            std::unique_ptr<ASTTypeNode> type,
            std::unique_ptr<ASTExpressionNode> expression
        ) : name(name), type(std::move(type)), expression(std::move(expression)) {}

        std::string name;
        std::unique_ptr<ASTTypeNode> type;
        std::unique_ptr<ASTExpressionNode> expression; // Optional expression to init the variable

        void display(std::size_t left_padding) override;
    };

    class ASTMainProgramNode final : public ASTProgramNode
    {
    public:
        ASTMainProgramNode(
            std::vector<std::unique_ptr<ASTFunctionProgramNode>> functions,
            std::vector<std::unique_ptr<ASTVariableProgramNode>> global_variables
        ) : functions(std::move(functions)), global_variables(std::move(global_variables)) {}

        std::vector<std::unique_ptr<ASTFunctionProgramNode>> functions;
        std::vector<std::unique_ptr<ASTVariableProgramNode>> global_variables;

        void display(std::size_t left_padding) override;
    };
}
