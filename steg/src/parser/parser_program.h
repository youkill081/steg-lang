//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include "parser_utils.h"

#include <memory>

#include "ast/ASTProgramNode.h"

#include "parser_expressions.h"

namespace compilator
{
    inline auto parseParameter =
        map(
            seq(
                lint<"Unknown type %v">(parseTypeNoVoid),
                lint<"Missing or invalid identifier">(parseToken<TOKEN_IDENTIFIER>)
            ),
            [](auto data)
            {
                return std::make_unique<ASTParameterProgramNode>(
                    std::get<1>(data).value,
                    std::move(std::get<0>(data))
                );
            }
        );

    inline auto parseParamWithComma = parseParameter << parseToken<TOKEN_PUNCTUATION_COMMA>;

    inline auto parseFunctionParameters = map (seq (
        many(parseParamWithComma),  optional(parseParameter)
    ), [](auto data)
        {
            auto params_vec = std::move(std::get<0>(data));
            auto last_param = std::move(std::get<1>(data));

            std::vector<std::unique_ptr<ASTParameterProgramNode>> parameters;

            for (auto& parameter : params_vec) {
                parameters.push_back(std::move(parameter));
            }
            if (last_param.has_value()) {
                parameters.push_back(std::move(*last_param));
            }

            return parameters;
        }
    );
}
