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
    {
    public:
        ASTProgramNode(const LexerToken& token) { this->token = token; }
    };

    class ASTFileProgramNode final : public ASTProgramNode
    {
    public:
        ASTFileProgramNode(const std::string &name, const std::string &path, const LexerToken &token, const LexerToken &path_token)
            : ASTProgramNode(token), name(name), path(path), path_token(path_token) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::string name;
        std::string path;
        LexerToken path_token;
    };

    class ASTFilesProgramNode final : public ASTProgramNode
    {
    public:
        ASTFilesProgramNode(std::vector<std::unique_ptr<ASTFileProgramNode>> files, const LexerToken& token)
            : ASTProgramNode(token), files(std::move(files)) {}

        void display(std::size_t left_padding) override
        {
            std::cout << "Only a wrapper node, shouldn't be displayed..."  << std::endl;
        };
        void accept(ASTVisitor* visitor) override;

        std::vector<std::unique_ptr<ASTFileProgramNode>> files;
    };

    class ASTImportProgramNode final : public ASTProgramNode
    {
    public:
        ASTImportProgramNode(std::vector<LexerToken> functions, const std::string& path, const LexerToken& token)
            : ASTProgramNode(token), functions_variables(std::move(functions)), path(path) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::vector<LexerToken> functions_variables;
        std::string path;
    };

    class ASTParameterProgramNode final : public ASTProgramNode
    {
    public:
        ASTParameterProgramNode(const std::string &name, std::unique_ptr<ASTTypeNode> type, const LexerToken& token)
            : ASTProgramNode(token), name(name), type(std::move(type)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

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
            bool is_exported,
            const LexerToken& token,
            const std::filesystem::path &path
        ) : ASTProgramNode(token),
            name(name),
            parameters(std::move(parameters)),
            return_type(std::move(return_type)),
            statement(std::move(statement)),
            is_exported(is_exported),
            path(path) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::string name;
        std::filesystem::path path;
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
            std::vector<std::unique_ptr<ASTFileProgramNode>> files,
            const LexerToken& token
        ) : ASTProgramNode(token), functions(std::move(functions)), global_variables(std::move(global_variables)),
            imports(std::move(imports)), files(std::move(files)) {}

        std::vector<std::unique_ptr<ASTFunctionProgramNode>> functions;
        std::vector<std::unique_ptr<ASTVariableStatement>> global_variables;
        std::vector<std::unique_ptr<ASTImportProgramNode>> imports;
        std::vector<std::unique_ptr<ASTFileProgramNode>> files;

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;
    };
}
