//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "ast/ASTExpressionNode.h"
#include "lexer/lexer_definitions.h"
#include "parser/monadic/monadic.hpp"

#include <memory>
#include <vector>

namespace compiler
{
    struct InfixPart {
        ASTBinaryExpressionNode::binaryOperationType type;
        std::unique_ptr<ASTExpressionNode> right;
    };

    extern Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseExpression;
    extern Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parsePrimary;
    extern Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer1;
    extern Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer2;

    template <typename P>
    auto as_expression(P p) {
        return map(std::move(p), [](auto node) -> std::unique_ptr<ASTExpressionNode> {
            return std::move(node);
        });
    }

    inline auto foldInfix = [](auto data) -> std::unique_ptr<ASTExpressionNode> {
        auto [left, parts] = std::move(data);
        for (auto& part : parts) {
            left = std::make_unique<ASTBinaryExpressionNode>(
                std::move(left),
                std::move(part.right),
                part.type
            );
        }
        return std::move(left);
    };
}