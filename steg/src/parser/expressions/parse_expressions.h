//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "parse_expressions_utils.h"
#include "parse_primary.h"
#include "parse_binary.h"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer2 = map(
        seq(compiler::ref(parsePrimary), many(parseMultiplicationPart | parseDivisionPart)),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLayer1 = map(
        seq(compiler::ref(parseLayer2), many(parseAdditionPart | parseSubtractionPart)),
        foldInfix
    );

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseExpression = parseLayer1;
}