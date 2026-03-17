//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "parse_expressions_utils.h"
#include "ast/ASTProgramNode.h"

#include <vector>
#include <memory>

namespace compiler
{
    extern Parser<std::vector<std::unique_ptr<ASTParameterProgramNode>>, TokenSpan> parseFunctionParameters;

    /* Parenthesis */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseParenthesizedExpr =
        as_expression(
            parseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS>
            >> compiler::ref(parseExpression)
            << lintedParseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>
        );

    /* Literal */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseLiteral =
        as_expression(map(parseToken<TOKEN_INTEGER>, [](LexerToken token) {
            return std::make_unique<ASTLiteralExpressionNode>(
                token.value,
                std::make_unique<ASTTypeNode>(ASTTypeNode::INT32)
            );
        }));

    /* Bool */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseBool =
        as_expression(map(parseToken<TOKEN_BOOL_TRUE>, [](LexerToken token) {
            return std::make_unique<ASTLiteralExpressionNode>(
                "true",
                std::make_unique<ASTTypeNode>(ASTTypeNode::BOOL)
            );
    })) | as_expression(map(parseToken<TOKEN_BOOL_FALSE>, [](LexerToken token) {
        return std::make_unique<ASTLiteralExpressionNode>(
            "false",
            std::make_unique<ASTTypeNode>(ASTTypeNode::BOOL)
        );
    }));

    /* String */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseStringLiteral =
       as_expression(map(parseToken<TOKEN_STRING>, [](LexerToken token) {
           return std::make_unique<ASTLiteralExpressionNode>(
               token.value,
               std::make_unique<ASTTypeNode>(ASTTypeNode::STRING)
           );
    }));

    /* Index Expression */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseIndexExpression =
        as_expression( map ( seq (
            parseToken<TOKEN_IDENTIFIER>,
            parseToken<TOKEN_PUNCTUATION_LEFT_SQUARE_BRACKET> >> compiler::ref(parseLayer1) << parseToken<TOKEN_PUNCTUATION_RIGHT_SQUARE_BRACKET> ),
            [](auto data)
            {
                auto [identifier, index] = std::move(data);
                std::unique_ptr<ASTIdentifierExpressionNode> expressionIdentifier =
                    std::make_unique<ASTIdentifierExpressionNode>(std::move(identifier.value));
                return std::make_unique<ASTIndexExpressionNode>(
                    std::move(expressionIdentifier),
                    std::move(index)
                );
            })
        );

    /* Function call */

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseFirstFunctionCallParameter
        = compiler::ref(parseLayer1);
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseFunctionCallParameter
        = parseToken<TOKEN_PUNCTUATION_COMMA> >> compiler::ref(parseLayer1);

    inline Parser<std::vector<std::unique_ptr<ASTExpressionNode>>, TokenSpan> parseFunctionCallParameters =
        map(parseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS> >>
            seq(optional(parseFirstFunctionCallParameter), many(parseFunctionCallParameter) <<
                parseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>),
    [](auto data)
        {
            auto [first, rest] = std::move(data);
            std::vector<std::unique_ptr<ASTExpressionNode>> end{};

            if (first.has_value())
                end.push_back(std::move(*first));
            for (auto &param : rest)
                end.push_back(std::move(param));
            return end;
        });

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseFunctionCall =
        as_expression(map(seq(parseToken<TOKEN_IDENTIFIER>, parseFunctionCallParameters), [](auto data) {
            std::unique_ptr<ASTIdentifierExpressionNode> identifier =
                std::make_unique<ASTIdentifierExpressionNode>(std::move(std::get<0>(data).value));
            return std::make_unique<ASTCallExpressionNode>(
                std::move(identifier),
                std::move(std::get<1>(data))
            );
        }));

    /* Identifier */
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseIdentifier =
        as_expression(map(parseToken<TOKEN_IDENTIFIER>, [](LexerToken token) {
            return std::unique_ptr<ASTIdentifierExpressionNode>(
                new ASTIdentifierExpressionNode(token.value)
            );
        }));
}
