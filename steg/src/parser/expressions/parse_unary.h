//
// Created by Roumite on 17/03/2026.
//

#pragma once
#include "parse_expressions_utils.h"
#include "ast/ASTExpressionNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseNegationUnary =
        map(
            seq(parseToken<TOKEN_OPERATOR_MINUS>, compiler::ref(parseUnary)),
            [](auto pair) -> std::unique_ptr<ASTExpressionNode> {
                return std::make_unique<ASTUnaryExpressionNode>(
                    std::move(std::get<1>(pair)),
                    ASTUnaryExpressionNode::NEGATION,
                    std::get<0>(pair)
                );
            }
        );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseAddressOfUnary =
        map(
            seq(parseToken<TOKEN_OPERATOR_REFERENCE>, compiler::ref(parseUnary)),
            [](auto pair) -> std::unique_ptr<ASTExpressionNode>
            {
                return std::make_unique<ASTAddressOfExpressionNode>(
                    std::move(std::get<1>(pair)),
                    std::get<0>(pair)
                );
            }
        );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseDereferenceUnary =
        map(
            seq(parseToken<TOKEN_OPERATOR_DEREFERENCE>, compiler::ref(parseUnary)),
            [](auto pair) -> std::unique_ptr<ASTExpressionNode>
            {
                return std::make_unique<ASTDereferenceExpressionNode>(
                    std::move(std::get<1>(pair)),
                    std::get<0>(pair)
                );
            }
        );
}
