//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "ast/ASTExpressionNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    extern Parser<std::unique_ptr<ASTBlockStatementNode>, TokenSpan> parseBlock;

    inline Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseCondition =
        parseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS> >> parseExpression << parseToken<
            TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>;

    inline Parser<std::unique_ptr<ASTIfStatementNode>, TokenSpan> parseIfStatement =
        map(seq(
                parseToken<TOKEN_KEYWORD_IF> >> parseCondition,
                compiler::ref(parseBlock),
                optional(parseToken<TOKEN_KEYWORD_ELSE> >> compiler::ref(parseBlock))
            ),
            [](auto data)
            {
                auto [condition, then_block, else_opt] = std::move(data);

                return std::make_unique<ASTIfStatementNode>(
                    std::move(condition),
                    std::move(then_block),
                    else_opt.has_value() ? std::move(*else_opt) : nullptr
                );
            });
}
