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

namespace compiler
{
    class ASTProgramNode : public ASTNode
        {};

    class ASTFileProgramNode final : public ASTProgramNode
    {
    public:
        ASTFileProgramNode(const std::string &name, const std::string &path) : name(name), path(path) {}

        void display(std::size_t left_padding) override;

        std::string name;
        std::string path;
    };

    class ASTFilesProgramNode final : public ASTProgramNode
    {
    public:
        ASTFilesProgramNode(std::vector<std::unique_ptr<ASTFileProgramNode>> files)
            : files(std::move(files)) {}

        void display(std::size_t left_padding) override
        {
            std::cout << "Only a wrapper node, shouldn't be displayed..."  << std::endl;
        };

        std::vector<std::unique_ptr<ASTFileProgramNode>> files;
    };

    class ASTImportProgramNode final : public ASTProgramNode
    {
    public:
        ASTImportProgramNode(std::vector<std::string> functions, const std::string& path)
            : functions_variables(std::move(functions)), path(path) {}

        void display(std::size_t left_padding) override;

        std::vector<std::string> functions_variables;
        std::string path;
    };

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

    class ASTMainProgramNode final : public ASTProgramNode
    {
    public:
        ASTMainProgramNode(
            std::vector<std::unique_ptr<ASTFunctionProgramNode>> functions,
            std::vector<std::unique_ptr<ASTVariableStatement>> global_variables,
            std::vector<std::unique_ptr<ASTImportProgramNode>> imports,
            std::vector<std::unique_ptr<ASTFileProgramNode>> files
        ) : functions(std::move(functions)), global_variables(std::move(global_variables)),
            imports(std::move(imports)), files(std::move(files)) {}

        std::vector<std::unique_ptr<ASTFunctionProgramNode>> functions;
        std::vector<std::unique_ptr<ASTVariableStatement>> global_variables;
        std::vector<std::unique_ptr<ASTImportProgramNode>> imports;
        std::vector<std::unique_ptr<ASTFileProgramNode>> files;

        void display(std::size_t left_padding) override;
    };
}
