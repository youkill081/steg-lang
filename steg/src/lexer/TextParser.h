//
// Created by Roumite on 14/03/2026.
//

#pragma once

#include <filesystem>
#include <string>

namespace compiler
{
    class TextParser
    {
    private:
        std::string _content;
        std::filesystem::path _path;

        std::size_t _index = 0;

        std::size_t _line_number = 1;
        std::size_t _column_number = 1;

        explicit TextParser(const std::string &content, const std::filesystem::path &path)
            : _content(content), _path(path) {}
    public:
        static TextParser from_file(const std::string &content);

        char get_next();
        char peek_x(std::uint32_t x = 1) const;

        bool has_next() const;

        [[nodiscard]] std::size_t get_line_number() const;
        [[nodiscard]] std::size_t get_column_number() const;
        [[nodiscard]] std::filesystem::path get_path() const;
    };
}