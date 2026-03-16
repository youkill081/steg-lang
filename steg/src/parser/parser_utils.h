//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include <optional>

#include "lexer/Lexer.h"
#include "lexer/lexer_definitions.h"
#include "monadic/monadic.hpp"

namespace compiler
{
    template <LexerTokensTypes token_type>
    constexpr auto parseToken = [](TokenSpan tokens) -> std::optional<Result<LexerToken, TokenSpan>>
    {
        if (tokens.empty()) return std::nullopt;
        if (tokens.front().type != token_type) return std::nullopt;
        return std::optional{ Result{ tokens.front(), tokens.subspan(1) } };
    };
}
