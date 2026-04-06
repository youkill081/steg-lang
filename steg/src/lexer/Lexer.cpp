//
// Created by Roumite on 14/03/2026.
//

#include "Lexer.h"

#include <iostream>
#include <optional>
#include <ostream>

#include "exceptions.h"
#include "TokenMap.h"

using namespace compiler;

LexerTokenCategory get_token_category(const LexerTokenType token_type)
{
    if (!token_type_to_category.contains(token_type))
        return TOKEN_CATH_NO_CATH;
    return token_type_to_category.at(token_type);
}

std::optional<LexerTokenType> lookup_keyword_or_symbol(const std::string_view &text) {
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

char Lexer::_decode_char() const
{
    if (!_parser.has_next())
        throw LexerException("Unexpected end of input while parsing character");

    const char current = _parser.get_next();

    if (current == '\\') {
        if (!_parser.has_next())
            throw LexerException("Incomplete escape sequence");

        char escape_char = _parser.get_next();
        switch (escape_char) {
        case 'n':  return '\n';
        case 'r':  return '\r';
        case 't':  return '\t';
        case '\\': return '\\';
        case '\'': return '\'';
        case '\"': return '\"';
        case '0':  return '\0';
        default:
            throw LexerException("Unknown escape sequence: \\" + std::string(1, escape_char));
        }
    }

    return current;
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
            .category = get_token_category(is_known_type.value()),
            .path = _parser.get_path(),
            .value = value,
            .line_number = line,
            .column_number = column
        };
    }

    return {
        .type = TOKEN_IDENTIFIER,
        .category = get_token_category(TOKEN_IDENTIFIER),
        .path = _parser.get_path(),
        .value = value,
        .line_number = line,
        .column_number = column
    };
}

LexerToken Lexer::_parse_char(std::size_t line, std::size_t column) const
{
    _parser.get_next(); // Skip opening '

    const char value = _decode_char();

    if (!_parser.has_next() || _parser.get_next() != '\'')
        throw LexerException("Unterminated char literal");

    return {
        .type = TOKEN_INTEGER,
        .category = get_token_category(TOKEN_INTEGER),
        .path = _parser.get_path(),
        .value = std::to_string(static_cast<uint8_t>(value)),
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
        .category = get_token_category(TOKEN_STRING),
        .path = _parser.get_path(),
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

    bool is_float = false;
    if (_parser.has_next() && _parser.peek_x() == '.')
    {
        is_float = true;
        value += _parser.get_next(); // the '.'

        while (_parser.has_next() && std::isdigit(_parser.peek_x())) {
            value += _parser.get_next();
        }
    }

    if (_parser.has_next() && (_parser.peek_x() == 'f' || _parser.peek_x() == 'F')) {
        is_float = true;
        _parser.get_next(); // Ignore the f/F
    }

    return {
        .type = is_float ? TOKEN_FLOAT : TOKEN_INTEGER,
        .category = get_token_category(is_float ? TOKEN_FLOAT : TOKEN_INTEGER),
        .path = _parser.get_path(),
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
            .category = get_token_category(known_type.value()),
            .path = _parser.get_path(),
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
            .category = get_token_category(TOKEN_EOF),
            .path = _parser.get_path(),
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

    if (current_char == '\'')
        { return _parse_char(start_line, start_column); }

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
    const std::span span = _tokens;
    return span.subspan(1); // Convert vector to span
}

TokenMap Lexer::get_token_map() const
{
    TokenMap map;
    for (const auto &token : _tokens)
        map.add_token(token);
    return map;
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
