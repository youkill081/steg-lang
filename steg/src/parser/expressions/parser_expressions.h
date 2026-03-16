//
// Created by Roumite on 15/03/2026.
//

#pragma once

#include "../parser_utils.h"
#include <memory>
#include "ast/ASTProgramNode.h"

namespace compiler
{
    constexpr auto parseExpression = [](TokenSpan tokens) -> std::optional<Result<std::unique_ptr<ASTExpressionNode>, TokenSpan>>
    {
        return std::optional{ Result<std::unique_ptr<ASTExpressionNode>, TokenSpan>{
            std::make_unique<ASTLiteralExpressionNode>(
                "valeur",
                std::make_unique<ASTTypeNode>(ASTTypeNode::UINT8)
            ),
            tokens.subspan(1)
        }};
    };
}