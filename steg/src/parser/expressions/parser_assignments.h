//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include "ast/ASTExpressionNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"
#include "./parse_expressions_utils.h"

namespace compiler
{
    struct AssignPart {
        ASTAssignExpressionStatement::assignmentType op;
        std::unique_ptr<ASTExpressionNode> value;
        LexerToken op_token;
    };

    inline Parser<AssignPart, TokenSpan> parseAssignPart = map(
        seq(parseToken<TOKEN_ASSIGNMENT>, lint<"Expected expression after '='">(compiler::ref(parseLayer1))),
        [](auto data) {
            return AssignPart{
                ASTAssignExpressionStatement::ASSIGN,
                std::move(std::get<1>(data)),
                std::get<0>(data)
            };
        }
    );

    inline Parser<AssignPart, TokenSpan> parseAssignAddPart = map(
        seq(parseToken<TOKEN_ADD_ASSIGNMENT>, lint<"Expected expression after '+='">(compiler::ref(parseLayer1))),
        [](auto data) {
            return AssignPart{
                ASTAssignExpressionStatement::ADD_ASSIGN,
                std::move(std::get<1>(data)),
                std::get<0>(data)
            };
        }
    );

    inline Parser<AssignPart, TokenSpan> parseAssignSubPart = map(
        seq(parseToken<TOKEN_SUB_ASSIGNMENT>, lint<"Expected expression after '-='">(compiler::ref(parseLayer1))),
        [](auto data) {
            return AssignPart{
                ASTAssignExpressionStatement::SUB_ASSIGN,
                std::move(std::get<1>(data)),
                std::get<0>(data)
            };
        }
    );

    inline Parser<AssignPart, TokenSpan> parseAssignMulPart = map(
        seq(parseToken<TOKEN_MUL_ASSIGNMENT>, lint<"Expected expression after '*='">(compiler::ref(parseLayer1))),
        [](auto data) {
            return AssignPart{
                ASTAssignExpressionStatement::MUL_ASSIGN,
                std::move(std::get<1>(data)),
                std::get<0>(data)
            };
        }
    );

    inline Parser<AssignPart, TokenSpan> parseAssignDivPart = map(
        seq(parseToken<TOKEN_DIV_ASSIGNMENT>, lint<"Expected expression after '/='">(compiler::ref(parseLayer1))),
        [](auto data) {
            return AssignPart{
                ASTAssignExpressionStatement::DIV_ASSIGN,
                std::move(std::get<1>(data)),
                std::get<0>(data)
            };
        }
    );
}

