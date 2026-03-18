//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "parse_statements.h"
#include "../parser_utils.h"
#include "ast/ASTStatementNode.h"
#include "lexer/lexer_definitions.h"
#include "../monadic/monadic.hpp"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTBlockStatementNode>, TokenSpan> parseBlock =
        map(seq(parseToken<TOKEN_PUNCTUATION_LEFT_BRACKET>, parseStatements << parseToken<
                TOKEN_PUNCTUATION_RIGHT_BRACKET>),
            [](auto data)
            {
                auto [open, statements] = std::move(data);
                return std::make_unique<ASTBlockStatementNode>(std::move(statements), open);
            });
}
