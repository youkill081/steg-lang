//
// Created by Roumite on 16/03/2026.
//

#pragma once
#include "ast/ASTExpressionNode.h"
#include "parser/parser_utils.h"
#include "parser/monadic/monadic.hpp"

namespace compiler
{
    inline auto parseUINT8 = map(parseToken<TOKEN_TYPE_UINT8>, [](auto) { return std::make_unique<ASTTypeNode>(ASTTypeNode::UINT8); });
    inline auto parseINT8 = map(parseToken<TOKEN_TYPE_INT8>, [](auto) { return std::make_unique<ASTTypeNode>(ASTTypeNode::INT8); });
    inline auto parseUINT16 = map(parseToken<TOKEN_TYPE_UINT16>, [](auto) { return std::make_unique<ASTTypeNode>(ASTTypeNode::UINT16); });
    inline auto parseINT16 = map(parseToken<TOKEN_TYPE_INT16>, [](auto) { return std::make_unique<ASTTypeNode>(ASTTypeNode::INT16); });
    inline auto parseUINT32 = map(parseToken<TOKEN_TYPE_UINT32>, [](auto) { return std::make_unique<ASTTypeNode>(ASTTypeNode::UINT32); });
    inline auto parseINT32 = map(parseToken<TOKEN_TYPE_INT32>, [](auto) { return std::make_unique<ASTTypeNode>(ASTTypeNode::INT32); });
    inline auto parseBOOL = map(parseToken<TOKEN_TYPE_BOOL>, [](auto) { return std::make_unique<ASTTypeNode>(ASTTypeNode::BOOL); });
    inline auto parseVOID = map(parseToken<TOKEN_TYPE_VOID>, [](auto) { return std::make_unique<ASTTypeNode>(ASTTypeNode::VOID); });

    inline auto parseType = parseUINT8 | parseINT8 | parseUINT16 | parseINT16 | parseUINT32 | parseINT32 | parseBOOL | parseVOID;
    inline auto parseTypeNoVoid = parseUINT8 | parseINT8 | parseUINT16 | parseINT16 | parseUINT32 | parseINT32 | parseBOOL;
}
