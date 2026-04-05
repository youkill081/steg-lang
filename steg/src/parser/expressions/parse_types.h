//
// Created by Roumite on 16/03/2026.
//

#pragma once

#include <memory>
#include <utility>
#include <optional>

#include "ast/ASTExpressionNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    inline Parser<uint8_t, TokenSpan> parsePointerDepth = map(
        many(parseToken<TOKEN_OPERATOR_REFERENCE>),
        [](const auto &stars) { return static_cast<uint8_t>(stars.size()); }
    );

    inline Parser<std::pair<ASTTypeNode::Types, LexerToken>, TokenSpan> parseBaseType =
        choice(
            map(parseToken<TOKEN_TYPE_UINT8>, [](const auto &t) { return std::make_pair(ASTTypeNode::UINT8,  t); }),
            map(parseToken<TOKEN_TYPE_UINT16>, [](const auto &t) { return std::make_pair(ASTTypeNode::UINT16, t); }),
            map(parseToken<TOKEN_TYPE_UINT32>, [](const auto &t) { return std::make_pair(ASTTypeNode::UINT32, t); }),
            map(parseToken<TOKEN_TYPE_INT>, [](const auto &t) { return std::make_pair(ASTTypeNode::INT,  t); }),
            map(parseToken<TOKEN_TYPE_BOOL>, [](const auto &t) { return std::make_pair(ASTTypeNode::BOOL,   t); }),
            map(parseToken<TOKEN_TYPE_FILE>, [](const auto &t) { return std::make_pair(ASTTypeNode::FILE,   t); }),
            map(parseToken<TOKEN_TYPE_CLOCK>, [](const auto &t) { return std::make_pair(ASTTypeNode::CLOCK,  t); }),
            map(parseToken<TOKEN_TYPE_VOID>, [](const auto &t) { return std::make_pair(ASTTypeNode::VOID,   t); }),
            map(parseToken<TOKEN_TYPE_FRAMEBUFFER>, [](const auto &t) { return std::make_pair(ASTTypeNode::FRAMEBUFFER,   t); })
        );

    inline Parser<std::unique_ptr<ASTTypeNode>, TokenSpan> parseType = map(
            seq(parseBaseType, parsePointerDepth),
            [](auto pair) {
                auto [base, depth ] = pair;
                auto [kind, token] = base;
                return std::make_unique<ASTTypeNode>(kind, depth, token);
            }
        );
}
