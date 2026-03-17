//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "ast/ASTStatementNode.h"
#include "../expressions/parse_expressions.h"
#include "parser/parser_utils.h"
#include "parser/expressions/parse_types.h"

namespace compiler
{
    inline Parser<std::unique_ptr<ASTVariableStatement>, TokenSpan> parseVariableDeclaration =
        map (seq(
            parseTypeNoVoid, (lintedParseToken<TOKEN_IDENTIFIER> << lintedParseToken<TOKEN_ASSIGNMENT>), (parseExpression)),
            [](auto data)
            {
                return std::make_unique<ASTVariableStatement>(
                    std::move(std::get<1>(data).value),
                    std::move(std::get<0>(data)),
                    std::move(std::get<2>(data))
                );
            }
        );

    inline Parser<std::unique_ptr<ASTVariableStatement>, TokenSpan> parseVariableDeclarationWithSemicolon =
        parseVariableDeclaration << parseToken<TOKEN_PUNCTUATION_SEMICOLON>;
}
