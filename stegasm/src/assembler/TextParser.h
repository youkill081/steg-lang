//
// Created by Roumite on 20/02/2026.

#pragma once

#include <stdexcept>
#include <cstdint>
#include <vector>

#define COMMENT_CHAR ';'

namespace assembler
{
    class TextParserError : public std::runtime_error
    {
    public:
        explicit TextParserError(const std::string& Message) : runtime_error(Message) {}
    };

    struct ParsedLine
    {
        uint32_t line_number = 0;
        std::string original_line = "";
        std::vector<std::string> tokens = {};
        uint32_t column_offset = 0; // Number of space before beg of line

        mutable uint64_t line_number_in_section = 0; // Used by compiler
        mutable bool is_instruction; // Used by assembler
    };

    class TextParser
    {
    private:
        std::string _file_path;
        std::string _file_content;

        static std::string remove_comments(const std::string &line);
        static std::string trim(const std::string &line);
        static std::vector<std::string> tokenize(const std::string &line);
    public:
        explicit TextParser(std::string file_path);
        static TextParser from_string(const std::string &file_content);

        void set_file_content(const std::string &file_content);
        [[nodiscard]] std::vector<ParsedLine> parse() const;
    };
}