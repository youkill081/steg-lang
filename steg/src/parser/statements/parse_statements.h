//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "parse_break_continue.h"
#include "parse_expression_statement.h"
#include "parse_for.h"
#include "parse_if.h"
#include "parse_return.h"
#include "parse_variables.h"
#include "parse_while.h"
#include "ast/ASTStatementNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    template <typename P>
    auto as_statement(P p) {
        return map(std::move(p), [](auto node) -> std::unique_ptr<ASTStatementNode> {
            return node;
        });
    }

    inline auto varStatement = as_statement(parseVariableDeclaration << parseToken<TOKEN_PUNCTUATION_SEMICOLON>);
    inline auto ifStatement = as_statement(parseIfStatement);
    inline auto whileStatement = as_statement(parseWhileStatement);
    inline auto forStatement = as_statement(parseForStatement);
    inline auto returnStatement = as_statement(parseReturn << parseToken<TOKEN_PUNCTUATION_SEMICOLON>);
    inline auto breakStatement = as_statement(parseBreak << parseToken<TOKEN_PUNCTUATION_SEMICOLON>);
    inline auto continueStatement = as_statement(parseContinue << parseToken<TOKEN_PUNCTUATION_SEMICOLON>);
    inline auto expressionStatement = as_statement(parseExpressionStatement << parseToken<TOKEN_PUNCTUATION_SEMICOLON>);

    inline Parser<std::unique_ptr<ASTStatementNode>, TokenSpan> parseStatement =
        varStatement | ifStatement | whileStatement | forStatement | returnStatement | breakStatement | continueStatement | expressionStatement;

    inline Parser<std::vector<std::unique_ptr<ASTStatementNode>>, TokenSpan> parseStatements =
        many(parseStatement);
}
