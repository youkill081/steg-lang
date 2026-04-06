//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include "ASTNode.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "ASTTypeNode.h"
#include "lexer/Lexer.h"
#include "semantic_analysis/utils/ResolvedType.h"

namespace compiler
{
    struct SymbolInfo;

    class ASTExpressionNode : public ASTNode
    {
    public:
        ASTExpressionNode(const LexerToken& token) { this->token = token; }

        ResolvedType resolved_type = ResolvedType::from(ASTTypeNode::VOID);
    };


    class ASTBinaryExpressionNode final : public ASTExpressionNode
    {
    public:
        enum binaryOperationType
        {
            ADDITION,
            SUBTRACTION,
            MULTIPLICATION,
            DIVISION,
            MODULO,
            COMPARISON_EQUAL,
            COMPARISON_NOT_EQUAL,
            COMPARISON_LESS,
            COMPARISON_GREATER,
            COMPARISON_LESS_OR_EQUAL,
            COMPARISON_GREATER_OR_EQUAL,
            COMPARISON_AND,
            COMPARISON_OR,

            SIGNED_MULTIPLICATION,
            SIGNED_DIVISION,

            FLOAT_ADDITION,
            FLOAT_SUBTRACTION,
            FLOAT_MULTIPLICATION,
            FLOAT_DIVISION,
            FLOAT_MODULO,
        };

        ASTBinaryExpressionNode(
            std::unique_ptr<ASTExpressionNode> left,
            std::unique_ptr<ASTExpressionNode> right,
            const binaryOperationType op_type,
            const LexerToken& token
        ) : ASTExpressionNode(token), left(std::move(left)), right(std::move(right)), op_type(op_type) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> left;
        std::unique_ptr<ASTExpressionNode> right;
        binaryOperationType op_type;
        bool is_float_cmp = false;
    };


    class ASTUnaryExpressionNode final : public ASTExpressionNode
    {
    public:
        enum unaryOperationType
        {
            NEGATION,
            NOT,

            FLOAT_NEGATION,
        };

        ASTUnaryExpressionNode(
            std::unique_ptr<ASTExpressionNode> expression,
            const unaryOperationType op_type,
            const LexerToken& token
        ) : ASTExpressionNode(token), expression(std::move(expression)), op_type(op_type) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> expression;
        unaryOperationType op_type;
    };

    class ASTLiteralExpressionNode final : public ASTExpressionNode
    {
    public:
        ASTLiteralExpressionNode(const std::string &value, std::unique_ptr<ASTTypeNode> type, const LexerToken& token)
            : ASTExpressionNode(token), value(value), type(std::move(type)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::string value;
        std::unique_ptr<ASTTypeNode> type;
    };


    class ASTIdentifierExpressionNode final : public ASTExpressionNode
    {
    public:
        ASTIdentifierExpressionNode(const std::string &name, const LexerToken &token)
            : ASTExpressionNode(token), name(name) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;
        std::string name;
        std::shared_ptr<SymbolInfo> resolved_symbol = nullptr;
    };


    class ASTCallExpressionNode final : public ASTExpressionNode
    {
    public:
        ASTCallExpressionNode(
            std::unique_ptr<ASTIdentifierExpressionNode> callee,
            std::vector<std::unique_ptr<ASTExpressionNode>> args,
            const LexerToken& token
        ) : ASTExpressionNode(token), callee(std::move(callee)), args(std::move(args)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTIdentifierExpressionNode> callee;
        std::vector<std::unique_ptr<ASTExpressionNode>> args;
        std::shared_ptr<SymbolInfo> resolved_symbol = nullptr;
    };


    class ASTIndexExpressionNode final : public ASTExpressionNode
    {
    public:
        ASTIndexExpressionNode(
            std::unique_ptr<ASTIdentifierExpressionNode> array,
            std::unique_ptr<ASTExpressionNode> index,
            const LexerToken& token
        ) : ASTExpressionNode(token), array(std::move(array)), index(std::move(index)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTIdentifierExpressionNode> array;
        std::unique_ptr<ASTExpressionNode> index;
    };

    class ASTAssignExpressionStatement final : public ASTExpressionNode
    {
    public:
        enum assignmentType {
            ASSIGN,
            ADD_ASSIGN,
            SUB_ASSIGN,
            MUL_ASSIGN,
            DIV_ASSIGN,

            ADD_ASSIGN_FLOAT,
            SUB_ASSIGN_FLOAT,
            MUL_ASSIGN_FLOAT,
            DIV_ASSIGN_FLOAT,

            MUL_ASSIGN_SIGNED,
            DIV_ASSIGN_SIGNED,
        };

        ASTAssignExpressionStatement(
            std::unique_ptr<ASTExpressionNode> target,
            const assignmentType op,
            std::unique_ptr<ASTExpressionNode> value,
            const LexerToken& token
        ) : ASTExpressionNode(token), target(std::move(target)), op(op), value(std::move(value)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> target;
        assignmentType op;
        std::unique_ptr<ASTExpressionNode> value;
    };

    class ASTAddressOfExpressionNode final : public ASTExpressionNode
    {
    public:
        ASTAddressOfExpressionNode(std::unique_ptr<ASTExpressionNode> expr, const LexerToken &token)
            : ASTExpressionNode(token), expression(std::move(expr)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> expression;
    };

    class ASTDereferenceExpressionNode final : public ASTExpressionNode
    {
    public:
        ASTDereferenceExpressionNode(std::unique_ptr<ASTExpressionNode> expr, const LexerToken &token)
            : ASTExpressionNode(token), expression(std::move(expr)) {}

        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        std::unique_ptr<ASTExpressionNode> expression;
    };

    /* UTILS */

    static inline std::map<ASTUnaryExpressionNode::unaryOperationType, std::string_view> ASTUnaryExpressionNode_type_to_string = {
        {ASTUnaryExpressionNode::unaryOperationType::NEGATION, "NEGATION"}
    };

    static inline std::map<ASTBinaryExpressionNode::binaryOperationType, std::string_view> ASTBinaryExpressionNode_type_to_string = {
        {ASTBinaryExpressionNode::binaryOperationType::ADDITION, "ADDITION"},
        {ASTBinaryExpressionNode::binaryOperationType::SUBTRACTION, "SUBTRACTION"},
        {ASTBinaryExpressionNode::binaryOperationType::MULTIPLICATION, "MULTIPLICATION"},
        {ASTBinaryExpressionNode::binaryOperationType::DIVISION, "DIVISION"},
        {ASTBinaryExpressionNode::binaryOperationType::MODULO, "MODULO"},
        {ASTBinaryExpressionNode::binaryOperationType::COMPARISON_EQUAL, "COMPARISON_EQUAL"},
        {ASTBinaryExpressionNode::binaryOperationType::COMPARISON_NOT_EQUAL, "COMPARISON_NOT_EQUAL"},
        {ASTBinaryExpressionNode::binaryOperationType::COMPARISON_LESS, "COMPARISON_LESS"},
        {ASTBinaryExpressionNode::binaryOperationType::COMPARISON_GREATER, "COMPARISON_GREATER"},
        {ASTBinaryExpressionNode::binaryOperationType::COMPARISON_LESS_OR_EQUAL, "COMPARISON_LESS_OR_EQUAL"},
        {ASTBinaryExpressionNode::binaryOperationType::COMPARISON_GREATER_OR_EQUAL, "COMPARISON_GREATER_OR_EQUAL"},
        {ASTBinaryExpressionNode::binaryOperationType::COMPARISON_AND, "COMPARISON_AND"},
        {ASTBinaryExpressionNode::binaryOperationType::COMPARISON_OR, "COMPARISON_OR"},
    };

    static inline std::map<ASTAssignExpressionStatement::assignmentType, std::string_view> ASTAssignExpressionStatement_type_to_string = {
        {ASTAssignExpressionStatement::ASSIGN, "ASSIGN"},
        {ASTAssignExpressionStatement::ADD_ASSIGN, "ADD_ASSIGN"},
        {ASTAssignExpressionStatement::SUB_ASSIGN, "SUB_ASSIGN"},
        {ASTAssignExpressionStatement::MUL_ASSIGN, "MUL_ASSIGN"},
        {ASTAssignExpressionStatement::DIV_ASSIGN, "DIV_ASSIGN"}
    };
}
