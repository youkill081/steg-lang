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

    inline auto varStatement = as_statement(parseVariableDeclaration << lintedParseToken<TOKEN_PUNCTUATION_SEMICOLON>);
    inline auto ifStatement = as_statement(parseIfStatement);
    inline auto whileStatement = as_statement(parseWhileStatement);
    inline auto forStatement = as_statement(parseForStatement);
    inline auto returnStatement = as_statement(parseReturn << lintedParseToken<TOKEN_PUNCTUATION_SEMICOLON>);
    inline auto breakStatement = as_statement(parseBreak << lintedParseToken<TOKEN_PUNCTUATION_SEMICOLON>);
    inline auto continueStatement = as_statement(parseContinue << lintedParseToken<TOKEN_PUNCTUATION_SEMICOLON>);
    inline auto expressionStatement = as_statement(parseExpressionStatement << lintedParseToken<TOKEN_PUNCTUATION_SEMICOLON>);

    inline Parser<std::unique_ptr<ASTStatementNode>, TokenSpan> parseStatement =
        choice(
            varStatement,
            ifStatement,
            whileStatement,
            forStatement,
            returnStatement, breakStatement,
            continueStatement,
            expressionStatement
        );

    using block_recovery = RecoverySet<
        TOKEN_PUNCTUATION_RIGHT_BRACKET,
        TOKEN_KEYWORD_IF,
        TOKEN_KEYWORD_WHILE,
        TOKEN_KEYWORD_RETURN,
        TOKEN_TYPE_BOOL,
        TOKEN_TYPE_UINT8,
        TOKEN_TYPE_UINT16,
        TOKEN_TYPE_INT,
        TOKEN_TYPE_UINT32
    >;

    using block_stop = StopSet<
        TOKEN_PUNCTUATION_RIGHT_BRACKET, TOKEN_KEYWORD_FUNCTION, TOKEN_KEYWORD_EXPORT,
        TOKEN_KEYWORD_IMPORT, TOKEN_KEYWORD_FILES, TOKEN_EOF
    >;

    using block_sync = SyncSet<
        TOKEN_TYPE_UINT8, TOKEN_TYPE_UINT16, TOKEN_TYPE_UINT32, TOKEN_TYPE_INT,
        TOKEN_TYPE_BOOL,  TOKEN_TYPE_VOID, TOKEN_KEYWORD_IF,
        TOKEN_KEYWORD_WHILE, TOKEN_KEYWORD_FOR, TOKEN_KEYWORD_RETURN,
        TOKEN_KEYWORD_BREAK, TOKEN_KEYWORD_CONTINUE, TOKEN_IDENTIFIER
    >;


    inline Parser<std::vector<std::unique_ptr<ASTStatementNode>>, TokenSpan> parseStatements =
        many(lint_checkpoint<block_stop, block_sync, ASTStatementError>(compiler::ref(parseStatement)));
}
