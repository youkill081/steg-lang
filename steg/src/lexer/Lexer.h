//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include <filesystem>

#include "./lexer_definitions.h"
#include "TextParser.h"

#include <span>
#include <vector>

namespace compiler
{
    class TokenMap;

    struct LexerToken
    {
        LexerTokenType type;
        LexerTokenCategory category;
        std::filesystem::path path;
        std::string value;
        std::size_t line_number;
        std::size_t column_number;

        static const LexerToken dummy()
        {
            return LexerToken{
                .type = TOKEN_NO_TYPE,
                .category = TOKEN_CATH_NO_CATH,
                .path = "",
                .value = "",
                .line_number = 1,
                .column_number = 1
            };
        }
    };



    class Lexer
    {
    private:
        bool _has_compute = false;
        TextParser &_parser;

        std::vector<LexerToken> _tokens{};

        std::size_t _browse_index = 0;

        [[nodiscard]] char _decode_char() const;

        [[nodiscard]] LexerToken _parse_identifier_keyword(std::size_t line, std::size_t column) const;
        [[nodiscard]] LexerToken _parse_string(std::size_t line, std::size_t column) const;
        [[nodiscard]] LexerToken _parse_char(std::size_t line, std::size_t column) const;
        [[nodiscard]] LexerToken _parse_number(std::size_t line, std::size_t column) const;
        [[nodiscard]] LexerToken _parse_symbol(std::size_t line, std::size_t column) const;

        [[nodiscard]] LexerToken _compute_next_token() const;
        void _skip_space_and_comment() const;
    public:
        explicit Lexer(TextParser &parser) : _parser(parser)
        {
            _tokens.push_back({TOKEN_BOF, TOKEN_CATH_NO_CATH, _parser.get_path(), "", 1, 1});
        }

        void compute();

        [[nodiscard]] std::span<const LexerToken> tokens() const;
        [[nodiscard]] TokenMap get_token_map() const;

        void display() const;
    };
}
