//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "ast/ASTExpressionNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    inline auto parseUINT8 = map(parseToken<TOKEN_TYPE_UINT8>, [](auto token) { return std::make_unique<ASTTypeNode>(ASTTypeNode::UINT8, token); });
    inline auto parseINT8 = map(parseToken<TOKEN_TYPE_INT8>, [](auto token) { return std::make_unique<ASTTypeNode>(ASTTypeNode::INT8, token); });
    inline auto parseUINT16 = map(parseToken<TOKEN_TYPE_UINT16>, [](auto token) { return std::make_unique<ASTTypeNode>(ASTTypeNode::UINT16, token); });
    inline auto parseINT16 = map(parseToken<TOKEN_TYPE_INT16>, [](auto token) { return std::make_unique<ASTTypeNode>(ASTTypeNode::INT16, token); });
    inline auto parseUINT32 = map(parseToken<TOKEN_TYPE_UINT32>, [](auto token) { return std::make_unique<ASTTypeNode>(ASTTypeNode::UINT32, token); });
    inline auto parseINT32 = map(parseToken<TOKEN_TYPE_INT32>, [](auto token) { return std::make_unique<ASTTypeNode>(ASTTypeNode::INT32, token); });
    inline auto parseBOOL = map(parseToken<TOKEN_TYPE_BOOL>, [](auto token) { return std::make_unique<ASTTypeNode>(ASTTypeNode::BOOL, token); });
    inline auto parseVOID = map(parseToken<TOKEN_TYPE_VOID>, [](auto token) { return std::make_unique<ASTTypeNode>(ASTTypeNode::VOID, token); });

    inline auto parseType = parseUINT8 | parseINT8 | parseUINT16 | parseINT16 | parseUINT32 | parseINT32 | parseBOOL | parseVOID;
    inline auto parseTypeNoVoid = parseUINT8 | parseINT8 | parseUINT16 | parseINT16 | parseUINT32 | parseINT32 | parseBOOL;
}
