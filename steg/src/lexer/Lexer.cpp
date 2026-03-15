//
// Created by Roumite on 14/03/2026.
//

#include "Lexer.h"

#include <iostream>
#include <optional>
#include <ostream>

#include "exceptions.h"

using namespace compilator;

std::optional<LexerTokensTypes> lookup_keyword_or_symbol(const std::string_view &text) {
    for (const auto &entry : KEYWORDS) {
        if (entry.name == text) {
            return entry.type;
        }
    }
    return std::nullopt;
}

void Lexer::_skip_space_and_comment() const
{
    while (_parser.has_next())
    {
        const char current = _parser.peek_x();

        if (std::isspace(current))
        {
            _parser.get_next();
            continue;
        }

        if (current == '/')
        {
            const char current_next = _parser.peek_x(2);

            if (current_next == '/') // Is a commentary
            {
                _parser.get_next();
                while (_parser.has_next() && _parser.peek_x() != '\n') { // Skip utils end of the line
                    _parser.get_next();
                }
                continue;
            } else if (current_next == '*')
            {
                _parser.get_next(); // Get the '/'
                _parser.get_next(); // Get the '*'
                bool closed = false;

                while (_parser.has_next())
                {
                    if (_parser.get_next() == '*' && _parser.peek_x() == '/') {
                        _parser.get_next(); // get the final '/'
                        closed = true;
                        break;
                    }
                }

                if (!closed)
                    throw LexerException("Unterminated block comment");
                continue;
            }
            // If here it's juste a '/' for division
        }

        break; // Not a space or commentray
    }
}

LexerToken Lexer::_parse_identifier_keyword(const std::size_t line, const std::size_t column) const
{
    std::string value;

    while (_parser.has_next() &&
        (std::isalnum(_parser.peek_x()) || _parser.peek_x() == '_')) {
        value += _parser.get_next();
    }

    const auto is_known_type = lookup_keyword_or_symbol(value);
    if (is_known_type.has_value()) {
        return {
            .type = is_known_type.value(),
            .value = value,
            .line_number = line,
            .column_number = column
        };
    }

    return {
        .type = TOKEN_IDENTIFIER,
        .value = value,
        .line_number = line,
        .column_number = column
    };
}

LexerToken Lexer::_parse_string(std::size_t line, std::size_t column) const
{
    _parser.get_next(); // Skip the first '"'
    std::string value;

    bool terminated = false;
    bool escaped = false;

    while (_parser.has_next()) {
        const char next_char = _parser.get_next();

        if (next_char == '\"' && !escaped) // End of loop
        {
            terminated = true;
            break;
        }

        if (next_char == '\\' && !escaped) // Escape the next char
        {
            escaped = true;
            continue;
        }

        if (next_char == '\n')
        {
            break;
        }

        escaped = false;
        value += next_char;
    }

    if (!terminated)
        throw LexerException("Unterminated string");
    return {
        .type = TOKEN_STRING,
        .value = value,
        .line_number = line,
        .column_number = column
    };
}

LexerToken Lexer::_parse_number(std::size_t line, std::size_t column) const
{
    std::string value; // is a number, checked before
    value += _parser.get_next();

    while (_parser.has_next() && std::isdigit(_parser.peek_x())) {
        value += _parser.get_next();
    }
    return {
        .type = TOKEN_INTEGER,
        .value = value,
        .line_number = line,
        .column_number = column
    };
}

LexerToken Lexer::_parse_symbol(std::size_t line, std::size_t column) const
{
    std::string value;
    value += _parser.get_next();

    if (_parser.has_next()) {
        const char next_char = _parser.peek_x();
        const std::string potential_double_op = value + next_char;

        if (lookup_keyword_or_symbol(potential_double_op).has_value()) {
            value += _parser.get_next();
        }
    }

    const auto known_type = lookup_keyword_or_symbol(value);
    if (known_type.has_value()) {
        return {
            .type = known_type.value(),
            .value = value,
            .line_number = line,
            .column_number = column
        };
    }

    throw LexerException("Unknown symbol: \"" + value + "\"");
}


LexerToken Lexer::_compute_next_token() const
{
    _skip_space_and_comment();

    if (!_parser.has_next())
    {
        return {
            .type = TOKEN_EOF,
            .value = "",
            .line_number = _parser.get_line_number(),
            .column_number = _parser.get_column_number()
        };
    }

    char current_char = _parser.peek_x();

    std::size_t start_line = _parser.get_line_number();
    std::size_t start_column = _parser.get_column_number();

    // It's an identifier or a keyword
    if (std::isalpha(current_char) || current_char == '_')
        { return _parse_identifier_keyword(start_line, start_column); }

    // It's a number
    if (std::isdigit(current_char))
        { return _parse_number(start_line, start_column); }

    // It's a string
    if (current_char == '"')
        { return _parse_string(start_line, start_column); }

    return _parse_symbol(start_line, start_column);
}

void Lexer::compute()
{
    if (_has_compute)
        return;

    while (true)
    {
        _tokens.push_back(_compute_next_token());
        if (_tokens.back().type == TOKEN_EOF)
            break;
    }
    _has_compute = true;
}

std::span<const LexerToken> Lexer::tokens() const
{
    return _tokens; // Convert vector to span
}

void Lexer::display() const
{
    for (const auto &token : _tokens)
    {
        std::cout   << "Type: " << token_type_to_string[token.type]
                    << " Value: \"" << token.value
                    << "\" line: " << token.line_number
                    << " column: " << token.column_number << std::endl;
    }
}
