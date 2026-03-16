//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include <string_view>

#include "monadic.hpp"

namespace compiler
{
    template <FixedString S>
    constexpr auto parseString = [](const CharSpan input) -> std::optional<Result<std::string_view, CharSpan>>
    {
        constexpr auto expected = S.view();

        if (input.size() >= expected.size() &&
            std::string_view(input.data(), expected.size()) == expected)
        {
            return Result<std::string_view, CharSpan>{
                expected,
                input.subspan(expected.size())
            };
        }
        return std::nullopt;
    };

    template <char C>
    constexpr auto parseChar = [](const CharSpan input) -> std::optional<Result<char, CharSpan>>
    {
        if (!input.empty() && input.front() == C)
            return Result{ C, input.subspan(1) };
        return std::nullopt;
    };

    template <FixedString First, FixedString... Rest>
    constexpr auto _parseOneOfStringsImpl()
    {
        if constexpr (sizeof...(Rest) == 0)
            return parseString<First>;
        else
            return parseString<First> | _parseOneOfStringsImpl<Rest...>();
    }

    template <FixedString... Strings>
    constexpr auto parseOneOf = _parseOneOfStringsImpl<Strings...>();
}