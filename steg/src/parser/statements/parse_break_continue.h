//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "ast/ASTStatementNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTBreakStatement>, TokenSpan> parseBreak =
        map (parseToken<TOKEN_KEYWORD_BREAK>, [](auto data)
        {
            return std::make_unique<ASTBreakStatement>(data);
        });

    inline Parser<std::unique_ptr<ASTContinueStatement>, TokenSpan> parseContinue =
        map (parseToken<TOKEN_KEYWORD_CONTINUE>, [](auto data)
        {
            return std::make_unique<ASTContinueStatement>(data);
        });
}
