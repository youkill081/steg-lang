//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "parse_expressions_utils.h"

namespace compiler
{
    /* Layer 2  */

    inline Parser<InfixPart, TokenSpan> parseMultiplicationPart = map(
        seq(parseToken<TOKEN_OPERATOR_MULTIPLY>, compiler::ref(parsePrimary)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::MULTIPLICATION, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseDivisionPart = map(
        seq(parseToken<TOKEN_OPERATOR_DIVIDE>, compiler::ref(parsePrimary)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::DIVISION, std::move(std::get<1>(data))}; }
    );

    /* Layer 1  */

    inline Parser<InfixPart, TokenSpan> parseAdditionPart = map(
        seq(parseToken<TOKEN_OPERATOR_PLUS>, compiler::ref(parseLayer2)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::ADDITION, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseSubtractionPart = map(
        seq(parseToken<TOKEN_OPERATOR_MINUS>, compiler::ref(parseLayer2)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::SUBTRACTION, std::move(std::get<1>(data))}; }
    );
}