//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include "./lexer_definitions.h"
#include "TextParser.h"

#include <span>
#include <vector>

namespace compiler
{
    struct LexerToken
    {
        LexerTokensTypes type;
        std::string value;
        std::size_t line_number;
        std::size_t column_number;
    };

    class Lexer
    {
    private:
        bool _has_compute = false;
        TextParser &_parser;

        std::vector<LexerToken> _tokens{};

        std::size_t _browse_index = 0;

        [[nodiscard]] LexerToken _parse_identifier_keyword(std::size_t line, std::size_t column) const;
        [[nodiscard]] LexerToken _parse_string(std::size_t line, std::size_t column) const;
        [[nodiscard]] LexerToken _parse_number(std::size_t line, std::size_t column) const;
        [[nodiscard]] LexerToken _parse_symbol(std::size_t line, std::size_t column) const;

        [[nodiscard]] LexerToken _compute_next_token() const;
        void _skip_space_and_comment() const;
    public:
        explicit Lexer(TextParser &parser) : _parser(parser) {}
        void compute();

        [[nodiscard]] std::span<const LexerToken> tokens() const;

        void display() const;
    };
}
