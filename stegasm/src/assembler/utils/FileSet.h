//
// Created by Roumite on 23/02/2026.
//

#pragma once

#include <vector>
#include <string>
#include "ByteBuffer.h"

namespace assembler
{
    struct ParsedLine;

    struct File
    {
        std::string user_name;
        std::string path;
        std::string extension;
        uint16_t descriptor = 0;
        ByteBuffer file_data{};
    };

    class FileSet : public std::vector<File>
    {
    private:
        uint16_t current_descriptor = 0;
    public:
        void push_file(const std::string &user_name, const std::string &path, const std::string &extension);
        void push_file_from_parsed_line(const ParsedLine &line);
        static FileSet from_parsed_lines(const std::vector<ParsedLine> &lines);
    };
}