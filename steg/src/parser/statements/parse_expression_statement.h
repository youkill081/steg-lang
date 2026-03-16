//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "ast/ASTStatementNode.h"
#include "../expressions/parse_expressions.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTExpressionStatement>, TokenSpan> parseExpressionStatement =
        map(parseExpression, [](auto data)
        {
            return std::make_unique<ASTExpressionStatement>(std::move(data));
        });
}
