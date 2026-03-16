//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "ast/ASTStatementNode.h"
#include "../expressions/parse_expressions.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    extern Parser<std::unique_ptr<ASTBlockStatementNode>, TokenSpan> parseBlock;
    extern  Parser<std::unique_ptr<ASTExpressionNode>, TokenSpan> parseCondition; // declared in parseIf

    inline Parser<std::unique_ptr<ASTWhileStatementNode>, TokenSpan> parseWhileStatement =
        map ( seq (parseToken<TOKEN_KEYWORD_WHILE> >> compiler::ref(parseCondition), compiler::ref(parseBlock) ),
            [](auto data)
            {
                auto [condition, body] = std::move(data);

                return std::make_unique<ASTWhileStatementNode>(
                    std::move(condition),
                    std::move(body)
                );
            }
    );
}
