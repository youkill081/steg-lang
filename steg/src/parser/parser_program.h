//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include "parser_utils.h"
#include <memory>
#include "ast/ASTProgramNode.h"
#include "parser_expressions.h"
#include "statements/parse_block.h"

namespace compiler
{
    /* Function parameters */
    inline Parser<std::unique_ptr<ASTParameterProgramNode>, TokenSpan>
    parseParameter =
         map(
            seq(
                parseTypeNoVoid,
                parseToken<TOKEN_IDENTIFIER>
            ),
            [](auto data)
            {
                return std::make_unique<ASTParameterProgramNode>(
                    std::get<1>(data).value,
                    std::move(std::get<0>(data))
                );
            }
        );

    inline Parser<std::unique_ptr<ASTParameterProgramNode>, TokenSpan>
    parseParamWithComma = parseToken<TOKEN_PUNCTUATION_COMMA> >> parseParameter;

    inline Parser<std::vector<std::unique_ptr<ASTParameterProgramNode>>, TokenSpan>
        parseFunctionParameters = parseToken<TOKEN_PUNCTUATION_LEFT_PARENTHESIS> >> map(seq(
            optional(parseParameter), many(parseParamWithComma)
        ), [](auto data)
        {
            auto params_vec = std::move(std::get<1>(data));
            auto first_param = std::move(std::get<0>(data));

            std::vector<std::unique_ptr<ASTParameterProgramNode>> parameters;

            for (auto &parameter : params_vec)
            {
                parameters.push_back(std::move(parameter));
            }
            if (first_param.has_value())
            {
                parameters.insert(parameters.begin(), std::move(*first_param)); // It's the first parameter
            }

            return parameters;
        }
    ) << parseToken<TOKEN_PUNCTUATION_RIGHT_PARENTHESIS>;

    /* Functions */

    inline Parser<LexerToken, TokenSpan> parseFunctionDeclaration = parseToken<TOKEN_KEYWORD_FUNCTION> >> parseToken<TOKEN_IDENTIFIER>;
    inline Parser<std::unique_ptr<ASTTypeNode>, TokenSpan> parseFunctionReturnType = parseToken<TOKEN_PUNCTUATION_ARROW> >> parseType;

    inline Parser<std::unique_ptr<ASTFunctionProgramNode>, TokenSpan> parseFunction =
        map(seq(
                optional(parseToken<TOKEN_KEYWORD_EXPORT>),
                parseFunctionDeclaration,
                parseFunctionParameters,
                parseFunctionReturnType,
                parseBlock
            ), [](auto data)
            {
                return std::make_unique<ASTFunctionProgramNode>(
                    std::move(std::get<1>(data).value),
                    std::move(std::get<2>(data)),
                    std::move(std::get<3>(data)),
                    std::move(std::get<4>(data)),
                    std::get<0>(data).has_value()
                );
            });

    /* Main Program Node*/

    inline Parser<std::unique_ptr<ASTMainProgramNode>, TokenSpan> parseMainProgram =
    map(many(
        map(parseFunction, [](auto f) -> std::unique_ptr<ASTNode> { return std::move(f); })
        |
            map(parseVariableDeclarationWithSemicolon, [](auto v) -> std::unique_ptr<ASTNode> { return std::move(v); })
    ), [](std::vector<std::unique_ptr<ASTNode>> nodes)
    {
        std::vector<std::unique_ptr<ASTFunctionProgramNode>> functions;
        std::vector<std::unique_ptr<ASTVariableStatement>> global_variables;

        for (auto &node : nodes)
        {
            if (auto *f = dynamic_cast<ASTFunctionProgramNode*>(node.get()))
            {
                node.release();
                functions.push_back(std::unique_ptr<ASTFunctionProgramNode>(f));
            }
            else if (auto* v = dynamic_cast<ASTVariableStatement*>(node.get()))
            {
                node.release();
                global_variables.push_back(std::unique_ptr<ASTVariableStatement>(v));
            }
        }

        return std::make_unique<ASTMainProgramNode>(
            std::move(functions),
            std::move(global_variables)
        );
    });
}
