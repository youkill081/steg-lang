//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <cstdint>

#include "ASTNode.h"

namespace compiler
{
    class ASTVisitor;

    class ASTTypeNode final : public ASTNode
    {
    public:
        enum Types
        {
            UINT8,
            INT8,
            UINT16,
            INT16,
            UINT32,
            INT32,
            BOOL,
            STRING,
            VOID,
            FILE,
            CLOCK
        };

        ASTTypeNode(
            const Types type,
            const uint8_t pointer_depth,
            const LexerToken &token
        ) : type(type), pointer_depth(pointer_depth) { this->token = token; }
        void display(std::size_t left_padding) override;
        void accept(ASTVisitor* visitor) override;

        Types type;
        uint8_t pointer_depth = 0;
    };

    static inline std::map<ASTTypeNode::Types, std::string_view> ASTTypeNode_type_to_string = {
        {ASTTypeNode::Types::UINT8, "uint8"},
        {ASTTypeNode::Types::INT8, "int8"},
        {ASTTypeNode::Types::UINT16, "uint16"},
        {ASTTypeNode::Types::INT16, "int16"},
        {ASTTypeNode::Types::UINT32, "uint32"},
        {ASTTypeNode::Types::INT32, "int32"},
        {ASTTypeNode::Types::BOOL, "bool"},
        {ASTTypeNode::Types::STRING, "string"},
        {ASTTypeNode::Types::VOID, "void"},
        {ASTTypeNode::Types::FILE, "file"},
        {ASTTypeNode::Types::CLOCK, "clock"}
    };
}
