//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "ast/ASTStatementNode.h"
#include "lexer/lexer_definitions.h"
#include "../expressions/parse_expressions.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    extern Parser<std::unique_ptr<ASTBlockStatementNode>, TokenSpan> parseBlock;
    extern  Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseCondition;

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseForInit =
        lintedParseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS> >> parseExpression << lintedParseToken<TOKEN_PUNCTUATION_SEMICOLON>;
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseForCondition = parseExpression << lintedParseToken<TOKEN_PUNCTUATION_SEMICOLON>;
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseForPost = parseExpression << lintedParseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>;

    inline Parser<std::unique_ptr<ASTForStatementNode>, TokenSpan> parseForStatement =
        map(seq(
                parseToken<TOKEN_KEYWORD_FOR>,
                parseForInit,
                parseForCondition,
                parseForPost,
                compiler::ref(parseBlock)
            ),
            [](auto data)
            {
                auto [for_token, init, condition, post, body] = std::move(data);

                return std::make_unique<ASTForStatementNode>(
                    std::move(init),
                    std::move(condition),
                    std::move(post),
                    std::move(body),
                    for_token
                );
            }
        );
}
