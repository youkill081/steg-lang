//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "ast/ASTStatementNode.h"
#include "lexer/lexer_definitions.h"
#include "parser/parser_expressions.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    extern Parser<std::unique_ptr<ASTBlockStatementNode>, TokenSpan> parseBlock;
    extern  Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseCondition;

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseForInit =
        parseToken<TOKEN_KEYWORD_FOR> >> parseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS> >> parseExpression << parseToken<TOKEN_PUNCTUATION_SEMICOLON>;
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseForCondition = parseExpression << parseToken<TOKEN_PUNCTUATION_SEMICOLON>;
    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseForPost = parseExpression << parseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>;

    inline Parser<std::unique_ptr<ASTForStatementNode>, TokenSpan> parseForStatement =
        map(seq(
                parseForInit,
                parseForCondition,
                parseForPost,
                compiler::ref(parseBlock)
            ),
            [](auto data)
            {
                auto [init, condition, post, body] = std::move(data);

                return std::make_unique<ASTForStatementNode>(
                    std::move(init),
                    std::move(condition),
                    std::move(post),
                    std::move(body)
                );
            }
        );
}
