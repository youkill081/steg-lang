//
// Created by Roumite on 14/03/2026.
//

#include "TextParser.h"

#include <fstream>
#include <sstream>

#include "exceptions.h"

using namespace compiler;

TextParser TextParser::from_file(const std::string &filepath)
{
    const std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw LexerException("Can't open file \"" + filepath + "\"");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return TextParser(buffer.str());
}

char TextParser::get_next()
{
    if (!has_next())
        return '\0';

    const char current_char = _content[_index];
    _index++;

    if (current_char == '\n') {
        _line_number++;
        _column_number = 1;
    } else {
        _column_number++;
    }

    return current_char;
}

char TextParser::peek_x(std::uint32_t x) const
{
    if (!has_next())
        return '\0';
    return _content[_index + x - 1];
}

bool TextParser::has_next() const
{
    return _index < _content.length();
}

std::size_t TextParser::get_line_number() const
{
    return _line_number;
}

std::size_t TextParser::get_column_number() const
{
    return _column_number;
}