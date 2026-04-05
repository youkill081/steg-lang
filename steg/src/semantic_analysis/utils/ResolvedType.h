//
// Created by Roumite on 18/03/2026.
//

#pragma once

#include <string>
#include <memory>

#include "linter/Linter.h"
#include "ast/ASTTypeNode.h"

namespace compiler
{
    struct ResolvedType
    {
        ASTTypeNode::Types base = ASTTypeNode::VOID;
        uint8_t pointer_depth = 0;
        std::shared_ptr<LexerToken> token = nullptr;

        [[nodiscard]] bool is_pointer() const
            { return pointer_depth > 0; }
        [[nodiscard]] bool is_void() const
            { return base == ASTTypeNode::VOID && !is_pointer(); }
        [[nodiscard]] bool is_bool() const
            { return base == ASTTypeNode::BOOL && !is_pointer(); }

        [[nodiscard]] bool is_numeric() const
        {
            if (is_pointer()) return false;
            switch (base)
            {
            case ASTTypeNode::UINT8:
            case ASTTypeNode::UINT16:
            case ASTTypeNode::UINT32:
            case ASTTypeNode::INT: return true;
            default: return false;
            }
        }

        [[nodiscard]] bool is_signed() const
        {
            return base == ASTTypeNode::INT;
        }

        [[nodiscard]] uint8_t bit_width() const
        {
            switch (base)
            {
            case ASTTypeNode::UINT8: return 8;
            case ASTTypeNode::UINT16: return 16;
            case ASTTypeNode::UINT32:
            case ASTTypeNode::INT: return 32;
            default: return 0;
            }
        }

        [[nodiscard]] ResolvedType deref() const
        {
            if (not is_pointer())
            {
                Linter::instance().report("Deref a no-pointer", *token, LintData::Severity::ERR);
                return {base, static_cast<uint8_t>(pointer_depth)};
            }
            return {base, static_cast<uint8_t>(pointer_depth - 1)};
        }

        [[nodiscard]] ResolvedType addr_of() const
        {
            return {base, static_cast<uint8_t>(pointer_depth + 1)};
        }

        bool operator==(const ResolvedType &other) const
        {
            return this->base == other.base && this->pointer_depth == other.pointer_depth;
        };

        [[nodiscard]] std::string to_string() const
        {

            std::string str(ASTTypeNode_type_to_string.at(base));
            for (uint8_t i = 0; i < pointer_depth; ++i)
                str += '@';
            return str;
        }

        static ResolvedType from(ASTTypeNode::Types t, uint8_t depth = 0) { return {t, depth}; }
        static ResolvedType from(const std::unique_ptr<ASTTypeNode> &node) { return {node->type, node->pointer_depth}; }
        static ResolvedType string_literal() { return {ASTTypeNode::UINT8, 1}; }
    };

    inline ResolvedType promote(const ResolvedType &a, const ResolvedType &b)
    {
        if (a == b)
            return a;

        if (!a.is_numeric() || !b.is_numeric())
            return ResolvedType::from(ASTTypeNode::VOID);

        const uint8_t width = std::max(a.bit_width(), b.bit_width());
        const bool sign = a.is_signed() || b.is_signed();
        if (width == 8)
            return ResolvedType::from(ASTTypeNode::UINT8);
        if (width == 16)
            return ResolvedType::from(ASTTypeNode::UINT16);
        return ResolvedType::from(sign ? ASTTypeNode::INT : ASTTypeNode::UINT32);
    }
}
