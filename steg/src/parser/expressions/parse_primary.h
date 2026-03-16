//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "parse_expressions_utils.h"

namespace compiler
{
    /* Parenthesis */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseParenthesizedExpr =
        as_expression(
            parseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS>
            >> compiler::ref(parseExpression)
            << parseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>
        );

    /* Lliteral */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLiteral =
        as_expression(map(parseToken<TOKEN_INTEGER>, [](LexerToken token) {
            return std::make_unique<ASTLiteralExpressionNode>(
                token.value,
                std::make_unique<ASTTypeNode>(ASTTypeNode::INT32)
            );
        }));

    /* Identifier */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseIdentifier =
        as_expression(map(parseToken<TOKEN_IDENTIFIER>, [](LexerToken token) {
            return std::unique_ptr<ASTIdentifierExpressionNode>(
                new ASTIdentifierExpressionNode(token.value)
            );
        }));

    // 4. Point d'entrée des atomes
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parsePrimary =
        parseParenthesizedExpr | parseLiteral | parseIdentifier;
}