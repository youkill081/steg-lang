//
// Created by Roumite on 20/02/2026.
//

#include "TextParser.h"

#include <fstream>
#include <string>
#include <utility>

using namespace assembler;

std::string TextParser::remove_comments(const std::string& line)
{
    auto pos = line.find(COMMENT_CHAR);
    if (pos == std::string::npos) { return line; }
    return line.substr(0, pos);
}

std::string TextParser::trim(const std::string& line)
{
    size_t start = 0;
    while (start < line.size() && std::isspace(line[start])) { ++start; }

    size_t end = line.size();
    while (end > start && std::isspace(line[end - 1])) { --end; }

    return line.substr(start, end - start);
}

/*
 * AI Generated line tokenizer
 */
std::vector<std::string> TextParser::tokenize(const std::string &line)
{
    std::vector<std::string> tokens;
    std::string current;

    bool inString = false;

    for (char c : line)
    {
        if (inString)
        {
            if (c == '"')
            {
                if (current.empty())
                    throw TextParserError("Empty string literal");
                tokens.push_back(current);
                current.clear();
                inString = false;
            }
            else
            {
                current += c;
            }
        }
        else
        {
            if (c == '"')
            {
                if (!current.empty())
                {
                    tokens.push_back(current);
                    current.clear();
                }
                inString = true;
            }
            else if (std::isspace(static_cast<unsigned char>(c)) || c == ',')
            {
                if (!current.empty())
                {
                    tokens.push_back(current);
                    current.clear();
                }
            }
            else
            {
                current += c;
            }
        }
    }

    if (inString)
        throw TextParserError("Unterminated string literal");

    if (!current.empty())
        tokens.push_back(current);

    return tokens;
}

TextParser::TextParser(std::string file_path) : _file_path(std::move(file_path)) {}

std::vector<ParsedLine> TextParser::parse() const
{
    std::ifstream file(_file_path);
    if (!file.is_open())
        throw TextParserError("Failed to open file: " + _file_path);

    std::vector<ParsedLine> result;
    std::string line;
    size_t lineNumber = 0;

    while (std::getline(file, line))
    {
        std::string clean_string = trim(remove_comments(line));
        if (clean_string.empty())
            continue;

        ParsedLine parsed;
        parsed.line_number = lineNumber++;
        parsed.original_line = clean_string;
        parsed.tokens = tokenize(clean_string);

        result.push_back(std::move(parsed));
    }

    return result;
}
