//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "parse_expressions_utils.h"

namespace compiler
{
    /* Layer 3 */

    inline Parser<InfixPart, TokenSpan> parseMultiplicationPart = map(
        seq(parseToken<TOKEN_OPERATOR_MULTIPLY>, compiler::ref(parsePrimary)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::MULTIPLICATION, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseDivisionPart = map(
        seq(parseToken<TOKEN_OPERATOR_DIVIDE>, compiler::ref(parsePrimary)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::DIVISION, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseModuloPart = map(
        seq(parseToken<TOKEN_OPERATOR_MODULUS>, compiler::ref(parsePrimary)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::MODULO, std::move(std::get<1>(data))}; }
    );

    /* Layer 2  */

    inline Parser<InfixPart, TokenSpan> parseAdditionPart = map(
        seq(parseToken<TOKEN_OPERATOR_PLUS>, compiler::ref(parseLayer4)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::ADDITION, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseSubtractionPart = map(
        seq(parseToken<TOKEN_OPERATOR_MINUS>, compiler::ref(parseLayer4)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::SUBTRACTION, std::move(std::get<1>(data))}; }
    );

    /* Layer 1 */

    inline Parser<InfixPart, TokenSpan> parseComparisonEqual = map(
        seq(parseToken<TOKEN_COMPARISON_EQUAL>, compiler::ref(parseLayer3)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_EQUAL, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonNotEqual = map(
        seq(parseToken<TOKEN_COMPARISON_NOT_EQUAL>, compiler::ref(parseLayer3)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_NOT_EQUAL, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonLess = map(
        seq(parseToken<TOKEN_COMPARISON_LESS>, compiler::ref(parseLayer3)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_LESS, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonGreater = map(
        seq(parseToken<TOKEN_COMPARISON_GREATER>, compiler::ref(parseLayer3)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_GREATER, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonLessOrEqual = map(
        seq(parseToken<TOKEN_COMPARISON_LESS_OR_EQUAL>, compiler::ref(parseLayer3)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_LESS_OR_EQUAL, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonGreaterOrEqual = map(
        seq(parseToken<TOKEN_COMPARISON_GREATER_OR_EQUAL>, compiler::ref(parseLayer3)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_GREATER_OR_EQUAL, std::move(std::get<1>(data))}; }
    );

    /* Layer 0 */

    inline Parser<InfixPart, TokenSpan> parseComparisonAnd = map(
        seq(parseToken<TOKEN_COMPARISON_AND>, compiler::ref(parseLayer2)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_AND, std::move(std::get<1>(data))}; }
    );

    inline Parser<InfixPart, TokenSpan> parseComparisonOr = map(
        seq(parseToken<TOKEN_COMPARISON_OR>, compiler::ref(parseLayer2)),
        [](auto data) { return InfixPart{ASTBinaryExpressionNode::COMPARISON_OR, std::move(std::get<1>(data))}; }
    );
}