//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include <memory>

#include "ast/ASTStatementNode.h"
#include "lexer/lexer_definitions.h"
#include "../expressions/parser_expressions.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTReturnStatement>, TokenSpan> parseReturn =
        map (seq(parseToken<TOKEN_KEYWORD_RETURN>, optional(parseExpression)), [](auto data)
        {
            auto [keyword, value] = std::move(data);
            return std::make_unique<ASTReturnStatement>(
                value.has_value() ? std::move(*value) : nullptr
            );
        });
}
